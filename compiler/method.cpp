//
// Created by gabriele on 14/11/18.
//

// TODO: remove some includes
#include <forbcc.hpp>
#include "method.hpp"

#include "code_ostream.hpp"
#include "variable.hpp"
#include "parameter.hpp"
#include "types/type.hpp"

// For documentation, see corresponding header file

inline void forbcc::method::print_prototype(forbcc::code_ostream &out, const std::string &thename) const {
    out << _return_type->codename() << " " << thename << "(";

    bool first = true;
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

void forbcc::method::print_stub_definition(code_ostream &out, const std::string &scope,
                                           const std::string &enumname) const {
    // First of all, let's print the header of the function
    print_prototype(out, scope + "::" + name());
    out << " {" << std::endl;

    out.increment_indentation();

    // TODO: add mutexes somewhere
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

        for (const auto &it: list()) {
            if (it.dir() != direction::OUT) {
                it.var().print_marshal(out, marshal::MARSHAL);
            }
        }
        out.decrement_indentation();
        out << "}" << std::endl;

        out << std::endl;

        out << "// Now write stuff to the stream" << std::endl;

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

        for (const auto &it: list()) {
            if (it.dir() != direction::IN) {
                it.var().print_marshal(out, marshal::UNMARSHAL);
            }
        }

        out.decrement_indentation();
        out << "}" << std::endl;
        out << std::endl;
    }

    // TODO: change to a macro

    // Finally if the return type is not void we receive that too
    if (_return_type->name() != "void") {
        out << "// Finally read result" << std::endl;

        std::string resvariable_name = "resvalue";
        while (is_contained(resvariable_name)) {
            // The variable name is already used by a parameter! Keep going
            resvariable_name = "_" + resvariable_name; // NOLINT
        }

        variable resvariable{_return_type, resvariable_name};

        resvariable.print_declaration(out);
        out << ";" << std::endl;
        out << std::endl;

        resvariable.print_serialize(out, serialize::RECV);

        out << std::endl;

        out << "if (datastream->require_marshal()) {" << std::endl;
        out.increment_indentation();

        resvariable.print_marshal(out, marshal::UNMARSHAL);

        out.decrement_indentation();
        out << "}" << std::endl;
        out << std::endl;

        out << "return " << resvariable.name() << ";" << std::endl;
    }

    out.decrement_indentation();

    out << "}" << std::endl;
}

void forbcc::method::print_skeleton_definition(forbcc::code_ostream &out) const {
    // First of all, declare variables to contain all parameters
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

    // Then receive each IN/INOUT parameter
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

    std::string rescode_name = "rescode";
    while (is_contained(rescode_name)) {
        // The variable name is already used by a parameter! Keep going
        rescode_name = "_" + rescode_name; // NOLINT
    }

    out << "uint16_t " << rescode_name << " = 1;" << std::endl;
    out << "if (callstream->require_marshal()) {" << std::endl;
    out.increment_indentation();
    out << rescode_name << " = forb::streams::marshal(" << rescode_name << ");" << std::endl;
    out.decrement_indentation();
    out << "}" << std::endl;

    out << std::endl;

    out << "callstream->send(&" << rescode_name << ", sizeof(" << rescode_name << "));" << std::endl;

    out << std::endl;
    out << "// Perform virtual call" << std::endl;

    if (_return_type->name() != "void") {
        std::string resvariable_name = "resvalue";
        while (is_contained(resvariable_name)) {
            // The variable name is already used by a parameter! Keep going
            resvariable_name = "_" + resvariable_name; // NOLINT
        }

        variable resvariable{_return_type, resvariable_name};

        resvariable.print_declaration(out);
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

        out << it.var().name();
    }

    out << ");" << std::endl;

    out << std::endl;

    out << "// Send back output variables" << std::endl;

    out << "if (datastream->require_marshal()) {" << std::endl;
    out.increment_indentation();

    for (const auto &it : list()) {
        if (it.dir() != direction::IN) {
            it.var().print_marshal(out, marshal::MARSHAL);
        }
    }

    out.decrement_indentation();
    out << "}" << std::endl;

    out << std::endl;

    for (const auto &param : list()) {
        if (param.dir() != direction::IN) {
            param.var().print_serialize(out, serialize::SEND);
        }
    }

    out << std::endl;


    if (_return_type->name() != "void") {
        out << "if (datastream->require_marshal()) {" << std::endl;
        out.increment_indentation();

        std::string resvariable_name = "resvalue";
        while (is_contained(resvariable_name)) {
            // The variable name is already used by a parameter! Keep going
            resvariable_name = "_" + resvariable_name; // NOLINT
        }

        variable resvariable{_return_type, resvariable_name};

        resvariable.print_marshal(out, marshal::MARSHAL);

        out.decrement_indentation();
        out << "}" << std::endl;

        resvariable.print_serialize(out, serialize::SEND);
    }

    out << "break;" << std::endl;
}

std::string forbcc::method::id() const {
    std::ostringstream oss{};

    oss << "_F" << name() << "E";

    if (list().empty()) {
        oss << "void";
    } else {
        for (const auto &param : list()) {
            const std::string var_type_name = param.var().var_type()->name();
            oss << var_type_name.length() << var_type_name;

            oss << param.var().name().length() << param.var().name();
        }
    }

    return oss.str();
}
