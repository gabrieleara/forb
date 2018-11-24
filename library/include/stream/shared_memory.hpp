//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_SHARED_MEMORY_HPP
#define LIBFORB_SHARED_MEMORY_HPP

#include <sys/types.h>

#include "stream.hpp"

namespace forb {
    namespace streams {

        class shared_memory : public stream {
            /* ********************** ALIAS DECLARATIONS ************************ */
        private:

        public:
            using index_t = std::size_t;
            using size_t = std::size_t;
            using key_t = ::key_t;
            using id_t = int;

            /* ********************** TYPES DECLARATIONS ************************ */
        private:
            // Forward declaration, see implementation file
            struct shmem_data;

            /* *************************** CONSTANTS **************************** */
        private:
            constexpr static int PRIVILEGES = 0666;

            /* *************************** ATTRIBUTES *************************** */
        private:
            key_t      key      = 0;
            id_t       shmem_id = 0;
            shmem_data *pointer = nullptr;
            bool       is_owner = false;

            /* ************************ PRIVATE METHODS ************************* */
        private:
            void _get(key_t key, size_t size = 0, int flags = 0);

            void _init(size_t size);

            void _move_attributes(shared_memory &other) noexcept {
                this->key      = other.key;
                this->shmem_id = other.shmem_id;
                this->pointer  = other.pointer;
                this->is_owner = other.is_owner;

                other.key      = 0;
                other.shmem_id = 0;
                other.pointer  = nullptr;
                other.is_owner = false;

                // Remember to move other members if necessary
            };

            /* ************************** CONSTRUCTOR *************************** */
        public:
            shared_memory() = default;

            /* ************************* MAKER METHODS ************************** */
        public:
            static shared_memory get(key_t key) {
                shared_memory shmem{};
                shmem._get(key);
                return shmem;
            };

            static shared_memory create(size_t size) {
                shared_memory shmem{};
                shmem._init(size);
                return shmem;
            };

            /* *********************** RULE OF THE FIVE ************************* */

            shared_memory(const shared_memory &other) = delete;

            shared_memory &operator=(const shared_memory &other) = delete;

            shared_memory(shared_memory &&other) noexcept {
                _move_attributes(other);
            };

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

            // TODO: this is a reminder to check manually whether close() is a noexcept call, since g++ has some problems
            // with conditionally noexcept qualifiers in function members
            ~shared_memory() noexcept override {
                close();
            };

            /* ************************* PUBLIC METHODS ************************* */

            key_t get_key() noexcept {
                return key;
            };

            /* ************************ VIRTUAL METHODS ************************* */

            void send(const void *buffer, size_t size) override;

            void recv(void *buffer, size_t size) override;

            // TODO: check whether the implementation should throw an exception when dealing with shmdt and so on
            void close() noexcept override;

            type get_type() noexcept override {
                return type::SHMEM;
            };

        };

    } // namespace streams

} // namespace forb


#endif //LIBFORB_SHARED_MEMORY_HPP
