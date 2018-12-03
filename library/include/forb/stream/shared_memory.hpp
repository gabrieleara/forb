//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_SHARED_MEMORY_HPP
#define LIBFORB_SHARED_MEMORY_HPP

#include <sys/types.h>

#include <forb/stream/stream.hpp>

namespace forb {
    namespace streams {

        /// Implementation of the forbcc::strams::stream API that uses a Linux Shared Memory
        /// to communicate.
        /// NOTICE: this is an unidirectional stream, it can be used as a bidirectional
        /// stream only if you are extremely sure there can be no overlap between various
        /// phases of each program; in each phase, the stream shall be used only in one
        /// direction.
        class shared_memory : public stream {
            /* *********************************** ALIASES AND STATIC ATTRIBUTES ************************************ */
        public:
            using index_t = stream::size_t;
            using key_t = ::key_t; // Defined in sys/types.h
            using id_t = int;

        private:
            /// The privileges of the shared memory file.
            static constexpr int PRIVILEGES = 0666;

            /// This is the forward declararion of the structure that will be allocated
            /// within the shared memory. This pattern is similar to the private implementation
            /// pattern (PIMPL), but only part of the implementation is forward-declared.
            struct shmem_data;

            /* ********************************************* ATTRIBUTES ********************************************* */
        private:
            /// The key used to retrieve the shared memory.
            key_t _key = 0;

            /// The ID of the shared memory.
            id_t _shmem_id = 0;

            /// The pointer to the shared memory structure.
            shmem_data *_pointer = nullptr;

            /// Whether this object is the one that created the shared memory space or not.
            bool _is_owner = false;

            /* ******************************************** CONSTRUCTORS ******************************************** */
        public:

            /// Default constructor, creates an empty shared memory object, with no associated shared memory area.
            /// It can be used in variable declarations that will then be overwritten with actual shared memory objects.
            shared_memory() = default;

            /**********************************************************************************************************/

            /// Virtual destructor that closes shared memory (if open).
            ~shared_memory() override {
                close();
            };

            /// This class does not support copy construction.
            shared_memory(const shared_memory &other) = delete;

            /// This class does not support copy assignment.
            shared_memory &operator=(const shared_memory &other) = delete;

            /// This class supports move construction.
            shared_memory(shared_memory &&other) noexcept {
                _move_attributes(other);
            };

            /// This class supports move assignment.
            shared_memory &operator=(shared_memory &&other) noexcept {
                if (this != &other) // Probably useless pointer-assignment check
                {
                    // If the same memory space is reused, the old shared
                    // memory must be released
                    close();

                    _move_attributes(other);
                }

                return *this;
            };

            /**********************************************************************************************************/

            // Following static methods are implicitly inline

            /// Retrieves a shared memory area from the given key.
            static shared_memory get(key_t key) {
                shared_memory shmem{};
                shmem._get(key);
                return shmem;
            };

            /// Creates a new shared memory area with the given size.
            static shared_memory create(size_t size) {
                shared_memory shmem{};
                shmem._init(size);
                return shmem;
            };

            /**********************************************************************************************************/
        private:
            /// Method called by move constructor and move assignment operators.
            void _move_attributes(shared_memory &other) noexcept;

            /// Wrapper for the shmget function, which is able to create a new shared memory area or retrieve
            /// an already-created shared memory location, depending on the input flags.
            /// Arguments are the same as shmget, only flags should not specify any privileges
            /// (they are automatically ORed with flags by this method).
            void _get(key_t key, size_t size = 0, int flags = 0);

            /// Creates a new shared memory location with the given size.
            /// It automatically generates the shared memory key creating a temporary file and calling ftok, then calls
            /// shared_memory::_get to actually create the shared memory area.
            /// Finally it initializes the shared memory content (mutexes, counters and so on).
            void _init(size_t size);

        public:
            /// Returns the key of the shared memory.
            key_t get_key() noexcept {
                return _key;
            };

            /// Blocking call that fills the shared memory area with size data from the given buffer.
            void send(const void *buffer, size_t size) override;

            /// Blocking call that fills buffer with size data received through the shared_memory area.
            void recv(void *buffer, size_t size) override;

            /// Closes the socket, called by the virtual destructor.
            void close() noexcept override;

            /// Returns the type of the stream.
            type get_type() const noexcept override {
                return type::SHMEM;
            };

            /// Returns true if the execution platform requires marshalling before sending
            /// data on the socket, false otherwise.
            /// In this stream implementation, this method returns always false.
            bool require_marshal() const override {
                return false;
            };

        };

    } // namespace streams

} // namespace forb


#endif //LIBFORB_SHARED_MEMORY_HPP
