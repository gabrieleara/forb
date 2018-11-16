#ifndef FORBCC_PRIMITIVE_TYPE_H
#define FORBCC_PRIMITIVE_TYPE_H

#include "type.hpp"
#include "../module.hpp"

namespace forbcc {
    class type_primitive final : public virtual type {
        const std::string actual_type;

    public:
        /// Each primitive types corresponds to an actual type with fixed size
        type_primitive(const std::string &name, const std::string actual_type)
                : type(nullptr, name), actual_type(std::move(actual_type)) {};

        /// The codename for primitive types is defined by their actual type
        std::string codename() const final {
            return actual_type;
        };
    };

} // namespace forbcc

#endif //FORBCC_PRIMITIVE_TYPE_H
