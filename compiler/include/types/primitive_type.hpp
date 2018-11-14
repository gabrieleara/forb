#ifndef FORBCC_PRIMITIVE_TYPE_H
#define FORBCC_PRIMITIVE_TYPE_H

#include <string>
#include <map>

#include "type.hpp"

namespace forbcc {
// Forward declarations to be used within this class declaration
    class variable;

    class primitive_type final : public type {
        const std::string actual_type;

    public:
        primitive_type(const std::string &name, const std::string &actual_type)
                : type(name), actual_type(actual_type) {};

        virtual std::string get_codename() const override final;

    protected:
        // virtual bool should_getter_return_const() const { return false; };
    };

} // namespace forbcc

#endif //FORBCC_PRIMITIVE_TYPE_H
