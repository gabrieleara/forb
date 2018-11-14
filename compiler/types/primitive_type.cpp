#include "types/primitive_type.hpp"

using primitive_type = forbcc::primitive_type;

std::string primitive_type::get_codename() const {
    return actual_type;
}
