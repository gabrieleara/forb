//
// Created by gabriele on 14/11/18.
//

#include "method.hpp"
#include "code_ostream.hpp"
#include "method.hpp"
#include "variable.hpp"
#include "parameter.hpp"
#include "types/type.hpp"

template<typename K, typename V>
inline bool insert_map_simplified(std::map<K, V> &mymap, K key, V value) {
    return mymap.insert(std::make_pair(key, value)).second;
}

void forbcc::method::print_prototype(forbcc::code_ostream &output, const std::string &thename) const {
    output << return_type.get_codename() << " " << thename << "(";

    bool first = true;
    for (const auto &it : parameters) {
        if (first) {
            first = false;
        } else {
            output << ", ";
        }

        output << it.second;
    };

    output << ")";
}

// TODO: methods cannot be declared const
void forbcc::method::print_declaration(forbcc::code_ostream &output) const {
    print_prototype(output, name);
    output << ";" << std::endl;
}

void forbcc::method::print_virtual_declaration(forbcc::code_ostream &output) const {
    output << "virtual ";
    print_prototype(output, name);
    output << " = 0;" << std::endl;
}

void forbcc::method::print_stub_definition(code_ostream &output, const std::string &scope,
                                           const std::string &enumname) const {
    // First of all, let's print the header of the function
    print_prototype(output, scope + "::" + name);
    output << " {" << std::endl;

    output.increment_indentation();

    // TODO: add mutexes somewhere
    // This is standard, each call must be initialized
    // TODO: use also the scoped enum given from outside
    output << "this->init_call(forb::call_id_t_cast(" << enumname << "::"<< get_id() << "));" << std::endl;
    output << std::endl;

    // Then we start to serialize input parameters
    if (counters[static_cast<int>(direction::IN)] > 0 ||
        counters[static_cast<int>(direction::INOUT)] > 0) {

        output << "// Objects serialization" << std::endl;
        output << "if (datastream->require_marshal()) {" << std::endl;

        output.increment_indentation();
        output << "// Data requires marshalling before being sent" << std::endl;

        for (const auto &it: parameters) {
            const parameter &param = it.second;
            if (param.dir != direction::OUT) {
                param.var.print_marshal(output);
                // output << std::endl;
            }
        }
        output.decrement_indentation();
        output << "}" << std::endl;

        output << std::endl;

        output << "// Now write stuff to the stream" << std::endl;

        for (const auto &it: parameters) {
            const parameter &param = it.second;
            if (param.dir != direction::OUT) {
                param.var.print_send(output);
            }
        }
    }

    output << std::endl;
    output << "// Wait for the response" << std::endl;
    output << "this->wait_return();" << std::endl;

    output << std::endl;

    // After successful call invocation, we receive output parameters
    if (counters[static_cast<int>(direction::OUT)] > 0 ||
        counters[static_cast<int>(direction::INOUT)] > 0) {

        output << "// Read output parameters" << std::endl;
        for (const auto &it: parameters) {
            const parameter &param = it.second;
            if (param.dir != direction::IN) {
                param.var.print_recv(output);
            }
        }
        output << std::endl;

        output << "if (datastream->require_marshal()) {" << std::endl;
        output.increment_indentation();

        for (const auto &it: parameters) {
            const parameter &param = it.second;
            if (param.dir != direction::IN) {
                param.var.print_unmarshal(output);
            }
        }

        output.decrement_indentation();
        output << "}" << std::endl;
        output << std::endl;
    }

    // Finally if the return type is not void we receive that too
    if (return_type.name != "void") {
        output << "// Finally read result" << std::endl;

        std::string resvariable_name = "resvalue";
        while (parameters.find(resvariable_name) != parameters.end()) {
            // The variable name is already used by a parameter! Keep going
            resvariable_name = "_" + resvariable_name;
        }

        variable resvariable{nullptr, return_type, resvariable_name};

        resvariable.print_declaration(output);
        output << ";" << std::endl;
        output << std::endl;

        resvariable.print_recv(output);

        output << std::endl;

        output << "if (datastream->require_marshal()) {" << std::endl;
        output.increment_indentation();

        resvariable.print_unmarshal(output);

        output.decrement_indentation();
        output << "}" << std::endl;
        output << std::endl;

        output << "return " << resvariable.name << ";" << std::endl;
    }

    output.decrement_indentation();

    output << "}" << std::endl;
}

void forbcc::method::print_skeleton_definition(forbcc::code_ostream &output) const {
    // First of all, declare variables to contain all parameters
    for (const auto &it : parameters) {
        it.second.var.print_declaration(output);
        output << ";" << std::endl;
    };

    output << std::endl;

    // Then receive each IN/INOUT parameter
    for (const auto &it: parameters) {
        const parameter &param = it.second;
        if (param.dir != direction::OUT) {
            param.var.print_recv(output);
        }
    }

    output << std::endl;

    // Then check marshalling
    output << "if (datastream->require_marshal()) {" << std::endl;
    output.increment_indentation();

    // Then receive each IN/INOUT parameter
    for (const auto &it: parameters) {
        const parameter &param = it.second;
        if (param.dir != direction::OUT) {
            param.var.print_unmarshal(output);
        }
    }

    output.decrement_indentation();
    output << "}" << std::endl;

    output << std::endl;

    // Send ACK for signal-like behavior
    output << "// Send over callstream an ACK" << std::endl;

    std::string rescode_name = "rescode";
    while (parameters.find(rescode_name) != parameters.end()) {
        // The variable name is already used by a parameter! Keep going
        rescode_name = "_" + rescode_name;
    }

    output << "uint16_t " << rescode_name << " = 1;" << std::endl;
    output << "if (callstream->require_marshal()) {" << std::endl;
    output.increment_indentation();
    output << rescode_name << " = forb::streams::marshal(" << rescode_name << ");" << std::endl;
    output.decrement_indentation();
    output << "}" << std::endl;

    output << std::endl;

    output << "callstream->send(&" << rescode_name << ", sizeof(" << rescode_name << "));" << std::endl;

    output << std::endl;
    output << "// Perform virtual call" << std::endl;

    if (return_type.name != "void") {
        std::string resvariable_name = "resvalue";
        while (parameters.find(resvariable_name) != parameters.end()) {
            // The variable name is already used by a parameter! Keep going
            resvariable_name = "_" + resvariable_name;
        }

        variable resvariable{nullptr, return_type, resvariable_name};

        resvariable.print_declaration(output);
        output << " = ";
    }

    output << name << "(";

    bool first = true;
    for (const auto &it : parameters) {
        if (first) {
            first = false;
        } else {
            output << ", ";
        }

        output << it.second.var.name;
    }

    output << ");" << std::endl;

    output << std::endl;

    output << "// Send back output variables" << std::endl;

    // TODO: iter over output variables
    output << "if (datastream->require_marshal()) {" << std::endl;
    output.increment_indentation();

    for (const auto &it : parameters) {
        const parameter &param = it.second;
        if (param.dir != direction::IN) {
            param.var.print_marshal(output);
        }
    }

    output.decrement_indentation();
    output << "}" << std::endl;

    output << std::endl;

    for (const auto &it : parameters) {
        const parameter &param = it.second;
        if (param.dir != direction::IN) {
            param.var.print_send(output);
        }
    }

    output << std::endl;


    if (return_type.name != "void") {
        output << "if (datastream->require_marshal()) {" << std::endl;
        output.increment_indentation();

        std::string resvariable_name = "resvalue";
        while (parameters.find(resvariable_name) != parameters.end()) {
            // The variable name is already used by a parameter! Keep going
            resvariable_name = "_" + resvariable_name;
        }

        variable resvariable{nullptr, return_type, resvariable_name};

        resvariable.print_marshal(output);

        output.decrement_indentation();
        output << "}" << std::endl;

        resvariable.print_send(output);
    }

    output << "break;" << std::endl;
}

bool forbcc::method::insert_parameter(const forbcc::parameter &param) {
    bool has_been_inserted = insert_map_simplified(parameters, param.var.name, param);

    ++counters[static_cast<int>(param.dir)];

    return has_been_inserted;
}

std::string forbcc::method::get_id() const {
    std::ostringstream oss{};

    oss << "_F" << name << "E";

    if (parameters.empty()) {
        oss << "void";
    } else {
        for (const auto &it : parameters) {
            const parameter &param = it.second;
            oss << param.var.var_type.name.length() << param.var.var_type.name;

            /*
            if (param.dir != direction::IN)
                oss << "R";
             */

            oss << param.var.name.length() << param.var.name;
        }
    }

    return oss.str();
}
