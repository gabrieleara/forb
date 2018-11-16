//
// Created by gabriele on 15/11/18.
//

#include "code_ostream.hpp"

#include "interface.hpp"
#include "parameter.hpp"
#include "method.hpp"

inline void forbcc::interface::print_stub_declaration(forbcc::code_ostream &output) const {
    output << "class " << name << ";" << std::endl
           << std::endl
           << "using " << name_ptr() << " = std::shared_ptr<" << name << ">;" << std::endl
           << "using " << name_var() << " = std::unique_ptr<" << name << ">;" << std::endl
           << std::endl
           << "class " << name << " : public virtual forb::base_stub {" << std::endl
           << "private:" << std::endl;

    output.increment_indentation();

    output << "// Default constructor, no other constructor needed" << std::endl
           << name << "() = default;" << std::endl
           << std::endl;

    output.decrement_indentation();
    output << "public:" << std::endl;
    output.increment_indentation();

    output << "// Methods as listed in the specification file" << std::endl;

    for (const auto &it : list()) {
        it.print_declaration(output);
    }

    output << std::endl;

    output.decrement_indentation();
    output << "protected:" << std::endl;
    output.increment_indentation();
    output << "// Virtual methods needed for the factory" << std::endl
           << "bool _match(const std::string &type) const override;" << std::endl
           << std::endl
           << "forb::base_stub *_create_empty() const override;" << std::endl
           << std::endl;

    output.decrement_indentation();
    output << "public:" << std::endl;
    output.increment_indentation();

    output << "static " << name_ptr() << " _narrow(forb::remote_var &&reference);" << std::endl
           << std::endl
           << "static " << name_ptr() << " _narrow(forb::remote_ptr &reference);" << std::endl
           << std::endl
           << "static " << name_var() << " _assign(forb::remote_var &&reference);" << std::endl
           << std::endl;

    output.decrement_indentation();
    output << "private:" << std::endl;
    output.increment_indentation();

    output << "// Static instance that will act as factory" << std::endl
           << "static " << name << " _factory;" << std::endl
           << std::endl;

    output << "// Inner class used to push the factory in the polymorphic one" << std::endl
           << "class init_class {" << std::endl
           << "public:" << std::endl;
    output.increment_indentation();
    output << "init_class() {" << std::endl;
    output.increment_indentation();
    output << "forb::base_stub::_protos().push_back(&_factory);" << std::endl;
    output.decrement_indentation();
    output << "}" << std::endl;
    output.decrement_indentation();
    output << "};" << std::endl
           << std::endl;

    output << "// Attribute used to execute code at program initialization" << std::endl
           << "static init_class _init;" << std::endl;

    output.decrement_indentation();
    output << "};" << std::endl;
}

inline void forbcc::interface::print_skeleton_declaration(forbcc::code_ostream &output) const {
    output << "class " << name_skeleton() << " : public virtual forb::base_skeleton {" << std::endl
           << std::endl
           << "public:" << std::endl;

    output.increment_indentation();

    output << "// Importing constructors from base class" << std::endl
           << "using base_skeleton::base_skeleton;" << std::endl
           << std::endl;

    output << "// Methods as listed in the specification file" << std::endl;

    for (const auto &it : list()) {
        it.print_virtual_declaration(output);
    }

    output << std::endl;

    output.decrement_indentation();
    output << "protected:" << std::endl;
    output.increment_indentation();

    output << "void execute_call(forb::call_id_t code," << std::endl
           << "                  forb::streams::stream *callstream," << std::endl
           << "                  forb::streams::stream *datastream) final;" << std::endl
           << std::endl;

    output.decrement_indentation();

    output << "};" << std::endl;
}

inline void forbcc::interface::print_methods_enum(forbcc::code_ostream &output) const {
    output << "/// Enumerator used to distinguish the requested call." << std::endl;
    output << "enum class " << name_enum() << " : forb::call_id_t {" << std::endl;

    output.increment_indentation();

    for (const auto &it : list()) {
        output << it.id() << "," << std::endl;
    }

    output.decrement_indentation();

    output << "};" << std::endl;
}

inline void forbcc::interface::print_static_attributes_definition(forbcc::code_ostream &output) const {
    output << "/// Initializing static attributes for " << codename() << " class factory." << std::endl
           << codename() << "             " << codename() << "::_factory{};" << std::endl
           << codename() << "::init_class " << codename() << "::_init{};" << std::endl;
}

inline void forbcc::interface::print_factory_match(forbcc::code_ostream &output) const {
    output << "/// Match method, needed by the factory." << std::endl
           << "bool " << codename() << "::_match(const std::string &type) const {" << std::endl;

    output.increment_indentation();
    output << "return type == \"" << codename() << "\";" << std::endl;
    output.decrement_indentation();

    output << "}" << std::endl;
}

inline void forbcc::interface::print_factory_create(forbcc::code_ostream &output) const {
    output << "/// Create method, used by the factory." << std::endl
           << "forb::base_stub *" << codename() << "::_create_empty() const {" << std::endl;

    output.increment_indentation();
    output << "return new " << codename() << "{};" << std::endl;
    output.decrement_indentation();

    output << "}" << std::endl;
}

inline void forbcc::interface::print_narrows(forbcc::code_ostream &output) const {
    // FIRST NARROW ASSIGNMENT BEGIN

    output << "/// Narrows a generic forb::remote_var reference to a " << codename_ptr() << " if possible." << std::endl
           << "/// Returns nullptr if not possible." << std::endl
           << codename_ptr() << " " << codename() << "::_narrow(forb::remote_var &&reference) {" << std::endl;

    output.increment_indentation();

    output << name << " *ptr = dynamic_cast<" << name << " *>(reference.get());" << std::endl
           << std::endl
           << "if (ptr == nullptr) {" << std::endl;

    output.increment_indentation();
    output << "return " << codename_ptr() << "{nullptr};" << std::endl;
    output.decrement_indentation();

    output << "} else {" << std::endl;

    output.increment_indentation();
    output << "// The reference is moved to the new one, after correct casting" << std::endl
           << "return std::dynamic_pointer_cast<" << name << ">(" << std::endl
           << "        forb::remote_ptr{std::move(reference)});" << std::endl;
    output.decrement_indentation();

    output << "}" << std::endl;

    output.decrement_indentation();

    output << "}" << std::endl;

    // FIRST NARROW ASSIGNMENT END

    output << std::endl;

    // SECOND NARROW ASSIGNMENT BEGIN

    output << "/// Narrows a generic forb::remote_ptr reference to a " << codename_ptr() << " if possible." << std::endl
           << "/// Returns nullptr if not possible." << std::endl
           << codename_ptr() << " " << codename() << "::_narrow(forb::remote_ptr &reference) {" << std::endl;

    output.increment_indentation();

    output << name << " *ptr = dynamic_cast<" << name << " *>(reference.get());" << std::endl
           << std::endl
           << "if (ptr == nullptr) {" << std::endl;

    output.increment_indentation();
    output << "return " << codename_ptr() << "{nullptr};" << std::endl;
    output.decrement_indentation();

    output << "} else {" << std::endl;

    output.increment_indentation();
    output << "// The reference is copied to the new one, after correct casting" << std::endl
           << "return std::dynamic_pointer_cast<" << name << ">(reference);" << std::endl;
    output.decrement_indentation();

    output << "}" << std::endl;

    output.decrement_indentation();

    output << "}" << std::endl;

    // SECOND NARROW ASSIGNMENT END

    output << std::endl;

    // ASSIGN BEGIN

    output << "/// Assigns a generic forb::remote_var reference to a " << codename_var() << " if possible." << std::endl
           << "/// Returns nullptr if not possible." << std::endl
           << codename_var() << " " << codename() << "::_assign(forb::remote_var &&reference) {" << std::endl;

    output.increment_indentation();

    output << name << " *ptr = dynamic_cast<" << name << " *>(reference.get());" << std::endl
           << std::endl
           << "if (ptr == nullptr) {" << std::endl;

    output.increment_indentation();
    output << "return " << name_var() << "{nullptr};" << std::endl;
    output.decrement_indentation();

    output << "} else {" << std::endl;

    output.increment_indentation();
    output << "// The original reference must be manually released first" << std::endl
           << "reference.release();" << std::endl
           << std::endl
           << "return " << name_var() << "{ptr};" << std::endl;
    output.decrement_indentation();

    output << "}" << std::endl;

    output.decrement_indentation();

    output << "}" << std::endl;

    // ASSIGN END
}

inline void forbcc::interface::print_stub_methods(forbcc::code_ostream &output) const {
    for (const auto &it : list()) {
        it.print_stub_definition(output, codename(), name_enum());

        output << std::endl;
    }
}

inline void forbcc::interface::print_skeleton_method(forbcc::code_ostream &output) const {
    // Used to align the printed code to opened parenthesis
    size_t blanks_size = codename_skeleton().length() + 20;
    std::string blanks(blanks_size, ' ');

    output << "/// This method dispatches the call to the right virtual method, which must be redefined in a subclass."
           << std::endl
           << "void " << codename_skeleton() << "::execute_call(forb::call_id_t code," << std::endl
           << blanks << "forb::streams::stream *callstream," << std::endl
           << blanks << "forb::streams::stream *datastream) {" << std::endl;

    output.increment_indentation();

    output << "// callstream is the stream used by the RPC protocol and" << std::endl
           << "// datastream is the one used to exchange actual data" << std::endl
           << "// They might be the same stream" << std::endl;

    output << "switch((" << name_enum() << ") code) {" << std::endl;
    output.increment_indentation();

    for (const auto &it : list()) {
        output << "case " << name_enum() << "::" << it.id() << ":" << std::endl;

        output.increment_indentation();
        it.print_skeleton_definition(output);
        output.decrement_indentation();
    }

    output.decrement_indentation();
    output << "}" << std::endl;
    output.decrement_indentation();
    output << "}" << std::endl;


    /**************************************************************************************************************/

}

void forbcc::interface::print_declaration(forbcc::code_ostream &output) const {
    print_stub_declaration(output);

    output << std::endl;

    print_skeleton_declaration(output);
}

void forbcc::interface::print_definition(forbcc::code_ostream &output) const {
    print_methods_enum(output);
    output << std::endl;

    print_static_attributes_definition(output);
    output << std::endl;

    print_factory_match(output);
    output << std::endl;

    print_factory_create(output);
    output << std::endl;

    print_narrows(output);
    output << std::endl;

    print_stub_methods(output);
    output << std::endl;

    print_skeleton_method(output);
    output << std::endl;
}
