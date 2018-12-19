#include <types/type_struct.hpp>

#include <code_ostream.hpp>
#include <variable.hpp>

// For documentation, see corresponding header file

void forbcc::type_struct::print_declaration(code_ostream &out) const {
    out << "struct " << name() << " {" << std::endl;

    out.increment_indentation();

    // Print each attribute declaration
    for (const auto &it : list()) {
        it.print_declaration(out);
        out << ";" << std::endl;
    }

    out.decrement_indentation();

    out << "};" << std::endl;

    out << std::endl;
}


void forbcc::type_struct::print_definition(forbcc::code_ostream &out) const {
    // Structure types define a partial specialization for the marshal template wihtin forb namespace, so any variable
    // can be simply marshalled/unmarshalled by calling forbcc::marshal or forbcc::unmarshal.
    out << "namespace forb {" << std::endl;
    out.increment_indentation();
    out << "namespace streams {" << std::endl;
    out.increment_indentation();

    out << "template <>" << std::endl
        << codename() << " marshal<" << codename() << ">(" << codename() << " v) {" << std::endl;

    out.increment_indentation();

    for (const auto &it : list()) {
        it.var_type()->print_var_marshal(out, "v." + it.name(), marshal::MARSHAL);
    }

    out << "return v;" << std::endl;

    out.decrement_indentation();
    out << "}" << std::endl;

    out.decrement_indentation();
    out << "} // namespace streams" << std::endl;

    out.decrement_indentation();
    out << "} // namespace forb" << std::endl
        << std::endl;
}

bool forbcc::type_struct::insert(std::string key, const variable &value) {
    // The next space to be checked for alignment if
    // the new attribute has the same alignment as the previous one
    static size_t next_byte_index = 0;

    // The alignment of the last inserted element
    static size_t last_attr_alignment = 0;

    // On successful insertion must re-calculate size (with alignment)
    if (ordered_unique_list::insert(key, value)) {
        // Doing simply this was not enough
        // _size += value.var_type()->size_of();

        // From Wikipedia: Padding is only inserted when a structure member is followed by a member with a
        // larger alignment requirement or at the end of the structure.

        size_t attr_size      = value.var_type()->size_of();
        size_t attr_alignment = value.var_type()->alignment();
        size_t padding;

        if (attr_alignment > last_attr_alignment) {
            padding         = next_byte_index % attr_alignment;
            next_byte_index = next_byte_index + padding + attr_size;
        } else {
            // Next element can be put without any padding
            next_byte_index = next_byte_index + attr_size;
        }

        if (attr_alignment > _alignment) {
            _alignment = attr_alignment;
        }

        last_attr_alignment = attr_alignment;

        padding = _alignment - (next_byte_index % _alignment);

        if (padding == _alignment) {
            padding = 0;
        }

        // Padding shall be inserted at the end if necessary
        _size = next_byte_index + padding;

        // Adding padding every time does not influence padding of the final structure

        return true;
    }

    return false;
}
