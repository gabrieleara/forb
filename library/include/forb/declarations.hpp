//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_DECLARATIONS_HPP
#define LIBFORB_DECLARATIONS_HPP

#include <cstdint> // uint16_t

namespace forb {
    /// The type that will be used to transmit the call_id
    using call_id_t = uint16_t;

    /// The type that will be used to exchange the res_code
    using res_code_t = uint16_t;

    /// Casts any integer type to call_id_t.
    /// Used to send as call_id_t enumerators that use call_id_t as underlying type.
    template<typename T>
    constexpr forb::call_id_t call_id_t_cast(T &&v) {
        return static_cast<forb::call_id_t>(v);
    }

} // namespace forb

#endif //LIBFORB_DECLARATIONS_HPP
