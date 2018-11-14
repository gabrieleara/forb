//
// Created by gabriele on 04/11/18.
//

#include <cstddef>      // std::byte is a C++17 type
#include <cstdlib>      // mkstemp

#include <sys/stat.h>   // mkdir
#include <sys/types.h>  // ssize_t
#include <sys/ipc.h>    // shmem stuff
#include <sys/shm.h>    // shmem stuff
#include <unistd.h>     // close

#include <pthread.h> // mutexes and condition variables

#include "stream/shared_memory.hpp"

using mutex_t = pthread_mutex_t;
using condition_t = pthread_cond_t;
using mutexattr_t = pthread_mutexattr_t;
using condattr_t = pthread_condattr_t;

using shared_memory = forb::streams::shared_memory;

struct shared_memory::shmem_data {
    mutex_t     mutex;
    condition_t nonempty;
    condition_t nonfull;

    index_t rear;  // Consuming pointer
    index_t front; // Producing pointer

    size_t data_size;

    size_t how_many_free;
    size_t how_many_data;

    // Values, to be expanded when shared memory is allocated
    unsigned char data[1]; // C++98
    // std::byte data[1]; // C++17
};

class static_attributes {
public:
    mutexattr_t mutex_attrs{};
    condattr_t  cond_attrs{};

    static_attributes() {
        pthread_mutexattr_init(&mutex_attrs);
        pthread_mutexattr_setpshared(&mutex_attrs, PTHREAD_PROCESS_SHARED);

        pthread_condattr_init(&cond_attrs);
        pthread_condattr_setpshared(&cond_attrs, PTHREAD_PROCESS_SHARED);
    }
};


static static_attributes static_attrs{};

/* ************************ PRIVATE METHODS ************************* */

void shared_memory::_get(key_t key, size_t size, int flags) {
    // The size of the shmem_data structure allocated will depend on the actual
    // size of the data array
    id_t shmem_id = ::shmget(key, offsetof(shmem_data, data) + size, PRIVILEGES | flags);

    if (shmem_id < 0)
        throw (42);

    this->key      = key;
    this->shmem_id = shmem_id;
    this->is_owner = (flags & IPC_CREAT) != 0;

    this->pointer = (shmem_data *) ::shmat(shmem_id, nullptr, 0);

    if (this->pointer == (void *) -1)
        throw (17);
}

void shared_memory::_init(size_t size) {
    if (size == 0)
        throw (42);

    // First I generate a random file in /tmp, used then to generate the
    // IPC key using ftok

    // TODO: file name should be stored somewhere to delete the file later

    // Last six characters are used to generate a random name
    char filename[] = "/tmp/forb_shmem_XXXXXX";
    int  fd         = mkstemp(filename);

    if (fd < 0)
        throw (17);

    // The file is just a placeholder and it's always empty
    ::close(fd);

    // This is the key that will be used to refer to the shared memory
    int key = ftok(filename, 'A');
    if (key < 0)
        throw (12);

    // Perform a shmemget creating the shared memory area
    _get(key, size, IPC_CREAT | IPC_EXCL);

    // Initializing the memory area
    pointer->how_many_data = 0;
    pointer->data_size     = size;
    pointer->how_many_free = size;
    pointer->front         = 0;
    pointer->rear          = 0;

    // Initializing mutex and conditions
    int res = pthread_mutex_init(&pointer->mutex, &static_attrs.mutex_attrs);
    if (res != 0)
        throw (5);

    res = pthread_cond_init(&pointer->nonempty, &static_attrs.cond_attrs);
    if (res != 0)
        throw (6);

    res = pthread_cond_init(&pointer->nonfull, &static_attrs.cond_attrs);
    if (res != 0)
        throw (6);
}


void shared_memory::send(const void *buffer, size_t size) {
    auto    buffer_ptr = (const unsigned char *) buffer;
    size_t  remaining  = size;
    index_t start_index;
    size_t  available_contiguous_space;
    size_t  how_many_sending_now;

    if (pointer == nullptr)
        throw 13;

    pthread_mutex_lock(&pointer->mutex);

    while (remaining > 0) {
        // Until the shared memory is full, I need to wait
        while (pointer->how_many_free == 0) {
            pthread_cond_wait(&pointer->nonfull, &pointer->mutex);
        }

        // Now I know for sure there is some space
        start_index = pointer->front;

        if (pointer->front >= pointer->rear) {
            // Space is free until the end of the buffer
            available_contiguous_space = pointer->data_size - pointer->front;
        } else {
            // Space is free until rear is reached
            available_contiguous_space = pointer->rear - pointer->front;
        }

        how_many_sending_now = (remaining <= available_contiguous_space) ?
                               remaining : available_contiguous_space;

        pointer->how_many_free -= how_many_sending_now;
        pointer->front       = (pointer->front + how_many_sending_now) % pointer->data_size;

        pthread_mutex_unlock(&pointer->mutex);

        // Ok, I can write how_many_sending_now data starting from start_index position
        memcpy(pointer->data + start_index,
               buffer_ptr,
               how_many_sending_now);

        remaining -= how_many_sending_now;
        buffer_ptr += how_many_sending_now;

        // Since there is new data I need to increase count and signal the consumer
        pthread_mutex_lock(&pointer->mutex);
        pointer->how_many_data += how_many_sending_now;
        pthread_cond_signal(&pointer->nonempty);
    }

    pthread_mutex_unlock(&pointer
            ->mutex);
}

void shared_memory::recv(void *buffer, size_t size) {
    auto    buffer_ptr = (unsigned char *) buffer;
    size_t  remaining  = size;
    index_t start_index;
    size_t  available_contiguous_data;
    size_t  how_many_receiving_now;

    if (pointer == nullptr)
        throw 13;

    pthread_mutex_lock(&pointer->mutex);

    while (remaining > 0) {
        // Until the shared memory is empty, I need to wait
        while (pointer->how_many_data == 0) {
            pthread_cond_wait(&pointer->nonempty, &pointer->mutex);
        }

        // Now I know for sure there is some data
        start_index = pointer->rear;

        if (pointer->front > pointer->rear) {
            // Data is present between rear and front
            available_contiguous_data = pointer->front - pointer->rear;
        } else {
            // Data is present until the end of the memory space
            available_contiguous_data = pointer->data_size - pointer->rear;
        }

        how_many_receiving_now = (remaining <= available_contiguous_data) ?
                                 remaining : available_contiguous_data;

        pointer->how_many_data -= how_many_receiving_now;
        pointer->rear          = (pointer->rear + how_many_receiving_now) % pointer->data_size;

        pthread_mutex_unlock(&pointer->mutex);

        // Ok, I can read how_many_receiving_now data starting from start_index position
        ::memcpy(buffer_ptr,
                 pointer->data + start_index,
                 how_many_receiving_now);

        remaining -= how_many_receiving_now;
        buffer_ptr += how_many_receiving_now;

        // Since there is new space available I need to increase count and signal the producer
        pthread_mutex_lock(&pointer->mutex);
        pointer->how_many_free += how_many_receiving_now;
        pthread_cond_signal(&pointer->nonfull);
    }

    pthread_mutex_unlock(&pointer
            ->mutex);
}

void shared_memory::close() noexcept {
    if (this->pointer != nullptr) {
        // Release attachment to shared memory area
        // TODO: what to do if shmdt returns an error?
        ::shmdt(this->pointer);
        this->pointer = nullptr;
    }

    if (this->shmem_id != 0 && this->is_owner) {
        // Mark the shared memory to be deleted
        // TODO: what to do if shmctl returns an error?
        ::shmctl(this->shmem_id, IPC_RMID, nullptr);
        this->is_owner = false;
    }

    // TODO: Delete also temporary file if I am the last to hold the shared memory?

    this->shmem_id = 0;
}