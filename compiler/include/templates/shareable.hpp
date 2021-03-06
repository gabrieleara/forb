//
// Created by gabriele on 18/11/18.
//

#ifndef FORBCC_SHAREABLE_H
#define FORBCC_SHAREABLE_H

#include <memory>

namespace forbcc {
    /// A template that can be used to ease the declaration of a shared_pointer of a given type
    template<typename T>
    class shareable {

        /* *************************************************** ALIAS **************************************************** */
    public:
        /// Alias to the same type T, but with const qualifier
        using const_T = typename std::add_const<T>::type;

        /// Alias to std::shared_ptr<T>
        using ptr_t = std::shared_ptr<T>;

        /// Alias to std::shared_ptr<const T>
        using ptr_const_t = std::shared_ptr<const_T>;

        /* *************************************************** STATIC *************************************************** */

        /// Calls the constructor of the class with the given arguments to create a shared_pointer to T.
        /// Alias of std::make_shared<T>.
        template<typename ...Args>
        static ptr_t new_ptr(Args &&... args) {
            return std::make_shared<T>(args...);
        }

        /// Calls the constructor of the class with the given arguments to create a shared_pointer to a const T.
        /// Alias of std::make_shared<const_T>.
        template<typename ...Args>
        static ptr_const_t new_ptr_const(Args &&... args) {
            return std::make_shared<const_T>(args...);
        }

    };

} // namespace forbcc

#endif //FORBCC_SHAREABLE_H
