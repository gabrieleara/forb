//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_DECLARATIONS_HPP
#define LIBFORB_DECLARATIONS_HPP

#include "cstdint" // uint16_t

namespace forb {

    using call_id_t = uint16_t;

    template<typename T>
    constexpr forb::call_id_t call_id_t_cast(T &&v) {
        return static_cast<forb::call_id_t>(v);
    }

} // namespace forb

#endif //LIBFORB_DECLARATIONS_HPP
