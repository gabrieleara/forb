//
// Created by gabriele on 14/11/18.
//

#include <method.hpp>
#include <code_ostream.hpp>

// For documentation, see corresponding header file

const std::string forbcc::res_code_type = "uint16_t";

inline void forbcc::method::print_prototype(forbcc::code_ostream &out, const std::string &thename) const {
    out << _return_type->codename() << " " << thename << "(";

    bool first = true;

    // Print all parameters, separated by commas
    for (const parameter &it : list()) {
        if (first) {
            first = false;
        } else {
            out << ", ";
        }

        it.print_declaration(out);
    };

    out << ")";
}

void forbcc::method::print_declaration(forbcc::code_ostream &out) const {
    print_prototype(out, name());
    out << ";" << std::endl;
}

void forbcc::method::print_virtual_declaration(forbcc::code_ostream &out) const {
    out << "virtual ";
    print_prototype(out, name());
    out << " = 0;" << std::endl;
}

inline std::string forbcc::method::get_unused_variable_name(std::string name) const {
    while (contains(name)) {
        // The variable name is already used by a parameter! Keep going...
        name = "_" + name;
    }

    return name;
}

void forbcc::method::print_stub_definition(code_ostream &out, const std::string &scope,
                                           const std::string &enumname) const {
    // First of all, let's print the header of the function
    print_prototype(out, scope + "::" + name());
    out << " {" << std::endl;

    out.increment_indentation();

    std::string lock_guard_name = get_unused_variable_name("lock");

    out << "// Calls cannot be executed concurrently, since the connection is recycled for each call." << std::endl
        << "std::lock_guard<std::mutex> " << lock_guard_name << "{base_stub::_mutex};"
        << std::endl << std::endl;

    // This is standard, each call must be initialized
    out << "this->init_call(forb::call_id_t_cast(" << enumname << "::" << id() << "));" << std::endl;
    out << std::endl;

    // Then we start to serialize input parameters
    if (_counters[static_cast<int>(direction::IN)] > 0 ||
        _counters[static_cast<int>(direction::INOUT)] > 0) {

        out << "// Objects serialization" << std::endl;
        out << "if (datastream->require_marshal()) {" << std::endl;

        out.increment_indentation();
        out << "// Data requires marshalling before being sent" << std::endl;

        // Marshal each input parameter
        for (const auto &it: list()) {
            if (it.dir() != direction::OUT) {
                it.var().print_marshal(out, marshal::MARSHAL);
            }
        }
        out.decrement_indentation();
        out << "}" << std::endl;

        out << std::endl;

        out << "// Now write stuff to the stream" << std::endl;

        // Serialize each input parameter
        for (const auto &it: list()) {
            if (it.dir() != direction::OUT) {
                it.var().print_serialize(out, serialize::SEND);
            }
        }
    }

    out << std::endl;
    out << "// Wait for the response" << std::endl;
    out << "this->wait_return();" << std::endl;

    out << std::endl;

    // After successful call invocation, we receive output parameters
    if (_counters[static_cast<int>(direction::OUT)] > 0 ||
        _counters[static_cast<int>(direction::INOUT)] > 0) {

        out << "// Read output parameters" << std::endl;
        for (const auto &it: list()) {
            if (it.dir() != direction::IN) {
                it.var().print_serialize(out, serialize::RECV);
            }
        }
        out << std::endl;

        out << "if (datastream->require_marshal()) {" << std::endl;
        out.increment_indentation();

        // Unmarshal each output parameter
        for (const auto &it: list()) {
            if (it.dir() != direction::IN) {
                it.var().print_marshal(out, marshal::UNMARSHAL);
            }
        }

        out.decrement_indentation();
        out << "}" << std::endl;
        out << std::endl;
    }

    // Finally, if the return type is not void, we receive the returned value too
    if (_return_type->name() != "void") {
        out << "// Finally read returned value" << std::endl;

        variable res_variable{_return_type, get_unused_variable_name("res_value")};

        res_variable.print_declaration(out);
        out << ";" << std::endl;
        out << std::endl;

        res_variable.print_serialize(out, serialize::RECV);

        out << std::endl;

        out << "if (datastream->require_marshal()) {" << std::endl;
        out.increment_indentation();

        res_variable.print_marshal(out, marshal::UNMARSHAL);

        out.decrement_indentation();
        out << "}" << std::endl;
        out << std::endl;

        out << "return " << res_variable.name() << ";" << std::endl;
    }

    out.decrement_indentation();

    out << "}" << std::endl;
}

void forbcc::method::print_skeleton_definition(forbcc::code_ostream &out) const {
    // First of all, declare variables to contain all parameters, both input and output ones
    for (const auto &it : list()) {
        it.var().print_declaration(out);
        out << ";" << std::endl;
    };

    out << std::endl;

    // Then receive each IN/INOUT parameter
    for (const auto &it: list()) {
        if (it.dir() != direction::OUT) {
            it.var().print_serialize(out, serialize::RECV);
        }
    }

    out << std::endl;

    // Then check marshalling
    out << "if (datastream->require_marshal()) {" << std::endl;
    out.increment_indentation();

    // If marshalling was required, unmarshal each IN/INOUT parameter
    for (const auto &it: list()) {
        if (it.dir() != direction::OUT) {
            it.var().print_marshal(out, marshal::UNMARSHAL);
        }
    }

    out.decrement_indentation();
    out << "}" << std::endl;

    out << std::endl;

    // Send ACK for signal-like behavior
    out << "// Send over callstream an ACK" << std::endl;

    std::string res_code_name = get_unused_variable_name("res_code");

    out << res_code_type << " " << res_code_name << " = 1;" << std::endl;
    out << "if (callstream->require_marshal()) {" << std::endl;

    out.increment_indentation();
    out << res_code_name << " = forb::streams::marshal(" << res_code_name << ");" << std::endl;
    out.decrement_indentation();

    out << "}" << std::endl;

    out << std::endl;

    out << "callstream->send(&" << res_code_name << ", sizeof(" << res_code_name << "));" << std::endl;

    out << std::endl;
    out << "// Perform virtual call" << std::endl;

    if (_return_type->name() != "void") {
        variable res_variable{_return_type, get_unused_variable_name("res_value")};

        res_variable.print_declaration(out);
        out << " = ";
    }

    out << name() << "(";

    bool first = true;

    for (const auto &it : list()) {
        if (first) {
            first = false;
        } else {
            out << ", ";
        }

        out << it.name();
    }

    out << ");" << std::endl;

    out << std::endl;

    out << "// Send back output variables" << std::endl;

    out << "if (datastream->require_marshal()) {" << std::endl;
    out.increment_indentation();

    // If required, marshal each output parameter
    for (const auto &it : list()) {
        if (it.dir() != direction::IN) {
            it.var().print_marshal(out, marshal::MARSHAL);
        }
    }

    out.decrement_indentation();
    out << "}" << std::endl;

    out << std::endl;

    // Serialize each output parameter
    for (const auto &param : list()) {
        if (param.dir() != direction::IN) {
            param.var().print_serialize(out, serialize::SEND);
        }
    }

    out << std::endl;

    // If the function has a return value, marshal/serialize it too
    if (_return_type->name() != "void") {
        out << "if (datastream->require_marshal()) {" << std::endl;
        out.increment_indentation();

        variable res_variable{_return_type, get_unused_variable_name("res_value")};

        res_variable.print_marshal(out, marshal::MARSHAL);

        out.decrement_indentation();
        out << "}" << std::endl;

        res_variable.print_serialize(out, serialize::SEND);
    }
}

std::string forbcc::method::id() const {
    std::ostringstream oss{};

    oss << "_F" << name() << "E";

    if (list().empty()) {
        oss << "void";
    } else {
        for (const auto &param : list()) {
            std::string var_type_name = param.var().var_type()->codename();

            std::replace(var_type_name.begin(), var_type_name.end(), ':', '_');

            oss << var_type_name.length() << var_type_name;
        }
    }

    return oss.str();
}