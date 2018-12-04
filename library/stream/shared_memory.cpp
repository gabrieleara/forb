//
// Created by gabriele on 04/11/18.
//

#include <cstddef>
#include <cstdlib>      // mkstemp
#include <cstring>      // memcpy

#include <sys/stat.h>   // mkdir
#include <sys/types.h>  // ssize_t, size_t
#include <sys/ipc.h>    // shmem functions
#include <sys/shm.h>    // shmem functions
#include <unistd.h>     // close

#include <stdexcept>    // std::logic_error

#include <pthread.h>    // mutexes and condition variables

#include <iostream>     // cerr logs

#include <forb/stream/shared_memory.hpp>
#include <forb/exception.hpp>

// Some aliases declarations
using mutex_t = pthread_mutex_t;
using condition_t = pthread_cond_t;
using mutexattr_t = pthread_mutexattr_t;
using condattr_t = pthread_condattr_t;

/* ********************************** PRIVATE IMPLEMENTATION (WITH DOCUMENTATION) *********************************** */

/// Definition of the private structure that will be allocated within the shared memory area.
/// It will share mutexes, condition variables, indexes, the data buffer and its size and
/// counters.
/// NOTICE: the dimension of this data structure is variable, in that the last argument will
/// be "stretched" when allocating the shared memory area to fit the given size.
struct forb::streams::shared_memory::shmem_data {
    /// Mutex that avoids concurrent access to the shared memory.
    mutex_t mutex;

    /// Condition that signals whenever the shared memory is not empty.
    condition_t non_empty;

    /// Condition that signals whenever the shared memory is not full.
    condition_t non_full;

    /// Consuming pointer
    index_t rear;

    /// Producing pointer
    index_t front;

    /// The dimension of the data buffer, since the structure will stretch
    /// automatically that attribute to fit the requested buffer size.
    size_t data_size;

    /// Counts the number of free spaces within the shared memory.
    size_t how_many_free;

    /// Counts the number of data spaces occupied within the shared memory.
    /// NOTICE: some spaces may be temporarily not free nor marked as data,
    /// while they are filled by the producer or emptied by the consumer.
    size_t how_many_data;

    /// Data buffer, to be expanded when shared memory is allocated
    unsigned char data[1]; // from C++98
    // std::byte data[1]; // from C++17
};

/// This structure is used to initialize mutex and condition variables attributes at program startup.
struct static_attributes {
    /// Attributes used to construct mutexes
    mutexattr_t mutex_attrs{};
    /// Attributes used to construct condition variables
    condattr_t  cond_attrs{};

    /// This constructor will be executed during the initialization of the variable static_attrs.
    static_attributes() {
        pthread_mutexattr_init(&mutex_attrs);
        pthread_mutexattr_setpshared(&mutex_attrs, PTHREAD_PROCESS_SHARED);

        pthread_condattr_init(&cond_attrs);
        pthread_condattr_setpshared(&cond_attrs, PTHREAD_PROCESS_SHARED);
    }
};

/// This variable contains attributes for mutex and condition variables,
/// which are reused for each new allocation.
static const static_attributes static_attrs;

/* *************************************** SHARED MEMORY CLASS IMPLEMENTATION *************************************** */

// For documentation of following methods, see corresponding header file

void forb::streams::shared_memory::_move_attributes(forb::streams::shared_memory &other) noexcept {
    this->_key      = other._key;
    this->_shmem_id = other._shmem_id;
    this->_pointer  = other._pointer;
    this->_is_owner = other._is_owner;

    other._key      = 0;
    other._shmem_id = 0;
    other._pointer  = nullptr;
    other._is_owner = false;

    // Remember to move other members if necessary
}

void forb::streams::shared_memory::_get(key_t key, size_t size, int flags) {
    // The size of the shmem_data structure allocated will depend on the actual
    // size of the data array
    id_t shmem_id = ::shmget(key, offsetof(shmem_data, data) + size, PRIVILEGES | flags);

    if (shmem_id < 0) {
        throw forb::exception{"Error during shared memory get: " + std::string(::strerror(errno)) + "."};
    }

    // After successful creation we store its attributes
    this->_key      = key;
    this->_shmem_id = shmem_id;
    this->_is_owner = (flags & IPC_CREAT) != 0;

    // We access the shared memory area as a "stretched" shmem_data structure
    this->_pointer = (shmem_data *) ::shmat(shmem_id, nullptr, 0);

    if (this->_pointer == (void *) -1) {
        throw forb::exception{"Error during shared memory pointer attach: " + std::string(::strerror(errno)) + "."};
    }
}

void forb::streams::shared_memory::_init(size_t size) {
    if (size == 0) {
        throw forb::exception{"Cannot initialize an empty shared memory."};
    }

    // First, a random file is generated in /tmp;
    // it will be used to generate the IPC key using ftok.

    // FIXME: file name should be stored somewhere to delete the file later
    // Last six characters are used to generate a random name
    char filename[] = "/tmp/forb_shmem_XXXXXX";
    int  fd         = mkstemp(filename);

    if (fd < 0) {
        throw forb::exception{
                "Unable to create temporary file for shared memory creation: "
                + std::string(::strerror(errno))
                + "."};
    }

    // The file is just a placeholder and it's always empty
    ::close(fd);

    // This is the key that will be used to refer to the shared memory.
    // The second argument is totally arbitrary.
    int key = ftok(filename, 'A');
    if (key < 0) {
        throw forb::exception{"Unable to create a shared memory key: " + std::string(::strerror(errno)) + "."};
    }

    // Perform a shmget creating the shared memory area
    _get(key, size, IPC_CREAT | IPC_EXCL);

    // Initializing the content of the shared memory area
    _pointer->how_many_data = 0;
    _pointer->data_size     = size;
    _pointer->how_many_free = size;
    _pointer->front         = 0;
    _pointer->rear          = 0;

    // Initializing mutex and conditions
    int res = pthread_mutex_init(&_pointer->mutex, &static_attrs.mutex_attrs);
    if (res != 0) {
        throw forb::exception{"Unable to initialize mutex within shared memory: "
                              + std::string(::strerror(res))
                              + "."};
    }

    res = pthread_cond_init(&_pointer->non_empty, &static_attrs.cond_attrs);
    if (res != 0) {
        throw forb::exception{
                "Unable to initialize condition variable within shared memory: "
                + std::string(::strerror(res))
                + "."};
    }

    res = pthread_cond_init(&_pointer->non_full, &static_attrs.cond_attrs);
    if (res != 0) {
        throw forb::exception{
                "Unable to initialize condition variable within shared memory: "
                + std::string(::strerror(res))
                + "."};
    }
}

void forb::streams::shared_memory::send(const void *buffer, size_t size) {
    auto    buffer_ptr = (const unsigned char *) buffer;
    size_t  remaining  = size;
    index_t start_index;
    size_t  available_contiguous_space;
    size_t  how_many_sending_now;

    if (_pointer == nullptr) {
        throw forb::exception{"Tried to send data over a closed shared memory."};
    }

    pthread_mutex_lock(&_pointer->mutex);

    // Repeat until all data is sent
    while (remaining > 0) {
        // Until the shared memory is full, I need to wait
        while (_pointer->how_many_free == 0) {
            pthread_cond_wait(&_pointer->non_full, &_pointer->mutex);
        }

        // Now I know for sure there is some space
        start_index = _pointer->front;

        if (_pointer->front >= _pointer->rear) {
            // Space is free until the end of the buffer
            available_contiguous_space = _pointer->data_size - _pointer->front;
        } else {
            // Space is free until rear is reached
            available_contiguous_space = _pointer->rear - _pointer->front;
        }

        how_many_sending_now = (remaining <= available_contiguous_space) ?
                               remaining : available_contiguous_space;

        _pointer->how_many_free -= how_many_sending_now;
        _pointer->front      = (_pointer->front + how_many_sending_now) % _pointer->data_size;

        pthread_mutex_unlock(&_pointer->mutex);

        // Ok, I can write how_many_sending_now data starting from start_index position
        ::memcpy(_pointer->data + start_index,
                 buffer_ptr,
                 how_many_sending_now);

        remaining -= how_many_sending_now;
        buffer_ptr += how_many_sending_now;

        // Since there is new data I need to increase count and signal the consumer
        pthread_mutex_lock(&_pointer->mutex);
        _pointer->how_many_data += how_many_sending_now;
        pthread_cond_signal(&_pointer->non_empty);
    }

    pthread_mutex_unlock(&_pointer->mutex);
}

void forb::streams::shared_memory::recv(void *buffer, size_t size) {
    auto    buffer_ptr = (unsigned char *) buffer;
    size_t  remaining  = size;
    index_t start_index;
    size_t  available_contiguous_data;
    size_t  how_many_receiving_now;

    if (_pointer == nullptr) {
        throw forb::exception{"Tried to receive data from a closed shared memory."};
    }

    pthread_mutex_lock(&_pointer->mutex);

    // Repeat until all data is received
    while (remaining > 0) {
        // Until the shared memory is empty, I need to wait
        while (_pointer->how_many_data == 0) {
            pthread_cond_wait(&_pointer->non_empty, &_pointer->mutex);
        }

        // Now I know for sure there is some data
        start_index = _pointer->rear;

        if (_pointer->front > _pointer->rear) {
            // Data is present between rear and front
            available_contiguous_data = _pointer->front - _pointer->rear;
        } else {
            // Data is present until the end of the memory space
            available_contiguous_data = _pointer->data_size - _pointer->rear;
        }

        how_many_receiving_now = (remaining <= available_contiguous_data) ?
                                 remaining : available_contiguous_data;

        _pointer->how_many_data -= how_many_receiving_now;
        _pointer->rear         = (_pointer->rear + how_many_receiving_now) % _pointer->data_size;

        pthread_mutex_unlock(&_pointer->mutex);

        // Ok, I can read how_many_receiving_now data starting from start_index position
        ::memcpy(buffer_ptr,
                 _pointer->data + start_index,
                 how_many_receiving_now);

        remaining -= how_many_receiving_now;
        buffer_ptr += how_many_receiving_now;

        // Since there is new space available I need to increase count and signal the producer
        pthread_mutex_lock(&_pointer->mutex);
        _pointer->how_many_free += how_many_receiving_now;
        pthread_cond_signal(&_pointer->non_full);
    }

    pthread_mutex_unlock(&_pointer->mutex);
}

void forb::streams::shared_memory::close() noexcept {
    if (this->_pointer != nullptr) {
        // Release attachment to shared memory area
        int res = ::shmdt(this->_pointer);

        if (res < 0) {
            std::cerr << "An error occurred while detaching shared memory: " << std::strerror(errno) << "."<< std::endl;
        }

        this->_pointer = nullptr;
    }

    if (this->_shmem_id != 0 && this->_is_owner) {
        // Mark the shared memory to be deleted
        int res = ::shmctl(this->_shmem_id, IPC_RMID, nullptr);

        if (res < 0) {
            std::cerr << "An error occurred while removing shared memory: " << std::strerror(errno) << "."<< std::endl;
        }

        this->_is_owner = false;
    }

    // FIXME: Delete also temporary file if I am the last to hold the shared memory?

    this->_shmem_id = 0;
}