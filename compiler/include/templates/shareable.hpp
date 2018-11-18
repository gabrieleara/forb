//
// Created by gabriele on 18/11/18.
//

#ifndef FORB_SHAREABLE_H
#define FORB_SHAREABLE_H

#include <memory>

template<typename T>
class shareable {
public:
    using ptr_t = std::shared_ptr<T>;
    using ptr_const_t = std::shared_ptr<const T>;

    template<typename ...Args>
    static ptr_t new_ptr(Args &&... args) {
        return std::make_shared<T>(args...);
    }

    template<typename ...Args>
    static ptr_const_t new_ptr_const(Args &&... args) {
        return std::make_shared<const T>(args...);
    }

};

#endif //FORB_SHAREABLE_H
