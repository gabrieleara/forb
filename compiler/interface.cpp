//
// Created by gabriele on 15/11/18.
//

#include <code_ostream.hpp>

#include <interface.hpp>
#include <parameter.hpp>
#include <method.hpp>

// For documentation, see corresponding header file

// The methods of this class are basically lots of prints based on the examples produced manually to test the library,
// so it's easy to get lost on the code. Don't care too much about prints, only code flow will be documented.

inline void forbcc::interface::print_stub_declaration(forbcc::code_ostream &out) const {
    out << "class " << name() << ";" << std::endl
        << std::endl
        << "using " << name_ptr() << " = std::shared_ptr<" << name() << ">;" << std::endl
        << "using " << name_var() << " = std::unique_ptr<" << name() << ">;" << std::endl
        << std::endl
        << "class " << name() << " : public virtual forb::base_stub {" << std::endl
        << "public:" << std::endl;

    out.increment_indentation();

    // Print constructor
    out << "// Importing constructor from superclass" << std::endl
        << "using forb::base_stub::base_stub;" << std::endl
        << std::endl;

    out.decrement_indentation();
    out << "public:" << std::endl;
    out.increment_indentation();

    out << "// Methods as listed in the specification file" << std::endl;

    // Print the declarations of all methods defined in FORB IDL file
    for (const auto &it : list()) {
        it.print_declaration(out);
    }

    out << std::endl;

    out.decrement_indentation();
    out << "protected:" << std::endl;
    out.increment_indentation();

    // Print declarations of methods used for the factory pattern
    out << "// Virtual methods needed for the factory" << std::endl
        << "bool _match(const std::string &type) const override;" << std::endl
        << std::endl
        << "forb::remote_var _create_empty() const override;" << std::endl
        << std::endl;

    out.decrement_indentation();
    out << "public:" << std::endl;
    out.increment_indentation();

    // Print declarations of downcasts from pointers to base class to pointers to specific class
    out << "static " << name_ptr() << " _narrow(forb::remote_var &&reference);" << std::endl
        << std::endl
        << "static " << name_ptr() << " _narrow(forb::remote_ptr &reference);" << std::endl
        << std::endl
        << "static " << name_var() << " _assign(forb::remote_var &&reference);" << std::endl
        << std::endl;

    out.decrement_indentation();
    out << "private:" << std::endl;
    out.increment_indentation();

    // Following static inner class and attributes are used by the factory pattern
    out << "// Static instance that will act as factory" << std::endl
        << "static " << name() << " _factory;" << std::endl
        << std::endl;

    out << "// Inner class used to push the factory in the polymorphic one" << std::endl
        << "class init_class {" << std::endl
        << "public:" << std::endl;
    out.increment_indentation();
    out << "init_class() {" << std::endl;
    out.increment_indentation();
    out << "forb::base_stub::_protos().push_back(&_factory);" << std::endl;
    out.decrement_indentation();
    out << "}" << std::endl;
    out.decrement_indentation();
    out << "};" << std::endl
        << std::endl;

    out << "// Attribute used to execute code at program initialization" << std::endl
        << "static init_class _init;" << std::endl;

    out.decrement_indentation();
    out << "};" << std::endl;
}

inline void forbcc::interface::print_skeleton_declaration(forbcc::code_ostream &out) const {
    out << "class " << name_skeleton() << " : public virtual forb::base_skeleton {" << std::endl
        << std::endl
        << "public:" << std::endl;

    out.increment_indentation();

    // Constructors of each skeleton base class are just forwards to forbcc::base_skeleton class constructors
    out << "// Importing constructors from base class" << std::endl
        << "using base_skeleton::base_skeleton;" << std::endl
        << std::endl;

    out << "// Methods as listed in the specification file" << std::endl;

    // Print again all methods, which are declared pure virtual, so that subclasses can define them.
    for (const auto &it : list()) {
        it.print_virtual_declaration(out);
    }

    out << std::endl;

    out.decrement_indentation();
    out << "protected:" << std::endl;
    out.increment_indentation();

    // Print the declaration of the method called whenever a new call request is received
    out << "void execute_call(forb::call_id_t code," << std::endl
        << "                  forb::streams::stream *callstream," << std::endl
        << "                  forb::streams::stream *datastream) final;" << std::endl
        << std::endl;

    out.decrement_indentation();

    out << "};" << std::endl;
}

inline void forbcc::interface::print_methods_enum(forbcc::code_ostream &out) const {
    out << "/// Enumerator used to distinguish the requested call." << std::endl;
    out << "enum class " << name_enum() << " : forb::call_id_t {" << std::endl;

    out.increment_indentation();

    // For each method, get its id and print it as a new enum value
    for (const auto &it : list()) {
        out << it.id() << "," << std::endl;
    }

    out.decrement_indentation();

    out << "};" << std::endl;
}

inline void forbcc::interface::print_static_attributes_definition(forbcc::code_ostream &out) const {
    // Prints factory attributes definition
    out << "/// Initializing static attributes for " << codename() << " class factory." << std::endl
        << codename() << "             " << codename() << "::_factory{};" << std::endl
        << codename() << "::init_class " << codename() << "::_init{};" << std::endl;
}

inline void forbcc::interface::print_factory_match(forbcc::code_ostream &out) const {
    out << "/// Match method, needed by the factory." << std::endl
        << "bool " << codename() << "::_match(const std::string &type) const {" << std::endl;

    out.increment_indentation();
    out << "return type == \"" << codename() << "\";" << std::endl;
    out.decrement_indentation();

    out << "}" << std::endl;
}

inline void forbcc::interface::print_factory_create(forbcc::code_ostream &out) const {
    out << "/// Create method, used by the factory." << std::endl
        << "forb::remote_var " << codename() << "::_create_empty() const {" << std::endl;

    out.increment_indentation();
    out << "return std::make_unique<" << codename() << ">();" << std::endl;
    out.decrement_indentation();

    out << "}" << std::endl;
}

inline void forbcc::interface::print_narrows(forbcc::code_ostream &out) const {
    // FIRST NARROW ASSIGNMENT BEGIN

    out << "/// Narrows a generic forb::remote_var reference to a " << codename_ptr() << " if possible." << std::endl
        << "/// Returns nullptr if not possible." << std::endl
        << codename_ptr() << " " << codename() << "::_narrow(forb::remote_var &&reference) {" << std::endl;

    out.increment_indentation();

    out << name() << " *ptr = dynamic_cast<" << name() << " *>(reference.get());" << std::endl
        << std::endl
        << "if (ptr == nullptr) {" << std::endl;

    out.increment_indentation();
    out << "return " << codename_ptr() << "{nullptr};" << std::endl;
    out.decrement_indentation();

    out << "} else {" << std::endl;

    out.increment_indentation();
    out << "// The reference is moved to the new one, after correct casting" << std::endl
        << "return std::dynamic_pointer_cast<" << name() << ">(" << std::endl
        << "        forb::remote_ptr{std::move(reference)});" << std::endl;
    out.decrement_indentation();

    out << "}" << std::endl;

    out.decrement_indentation();

    out << "}" << std::endl;

    // FIRST NARROW ASSIGNMENT END

    out << std::endl;

    // SECOND NARROW ASSIGNMENT BEGIN

    out << "/// Narrows a generic forb::remote_ptr reference to a " << codename_ptr() << " if possible." << std::endl
        << "/// Returns nullptr if not possible." << std::endl
        << codename_ptr() << " " << codename() << "::_narrow(forb::remote_ptr &reference) {" << std::endl;

    out.increment_indentation();

    out << name() << " *ptr = dynamic_cast<" << name() << " *>(reference.get());" << std::endl
        << std::endl
        << "if (ptr == nullptr) {" << std::endl;

    out.increment_indentation();
    out << "return " << codename_ptr() << "{nullptr};" << std::endl;
    out.decrement_indentation();

    out << "} else {" << std::endl;

    out.increment_indentation();
    out << "// The reference is copied to the new one, after correct casting" << std::endl
        << "return std::dynamic_pointer_cast<" << name() << ">(reference);" << std::endl;
    out.decrement_indentation();

    out << "}" << std::endl;

    out.decrement_indentation();

    out << "}" << std::endl;

    // SECOND NARROW ASSIGNMENT END

    out << std::endl;

    // ASSIGN BEGIN

    out << "/// Assigns a generic forb::remote_var reference to a " << codename_var() << " if possible." << std::endl
        << "/// Returns nullptr if not possible." << std::endl
        << codename_var() << " " << codename() << "::_assign(forb::remote_var &&reference) {" << std::endl;

    out.increment_indentation();

    out << name() << " *ptr = dynamic_cast<" << name() << " *>(reference.get());" << std::endl
        << std::endl
        << "if (ptr == nullptr) {" << std::endl;

    out.increment_indentation();
    out << "return " << name_var() << "{nullptr};" << std::endl;
    out.decrement_indentation();

    out << "} else {" << std::endl;

    out.increment_indentation();
    out << "// The original reference must be manually released first" << std::endl
        << "reference.release();" << std::endl
        << std::endl
        << "return " << name_var() << "{ptr};" << std::endl;
    out.decrement_indentation();

    out << "}" << std::endl;

    out.decrement_indentation();

    out << "}" << std::endl;

    // ASSIGN END
}

inline void forbcc::interface::print_stub_methods(forbcc::code_ostream &out) const {
    // For each method declared in FORB IDL file, print its stub implementation
    for (const auto &it : list()) {
        it.print_stub_definition(out, codename(), name_enum());

        out << std::endl;
    }
}

inline void forbcc::interface::print_skeleton_method(forbcc::code_ostream &out) const {
    // Used to align the printed code to opened parenthesis
    const size_t blanks_size = codename_skeleton().length() + 20;

    // Declared using string fill constructor
    const std::string blanks(blanks_size, ' ');

    out << "/// This method dispatches the call to the right virtual method, which must be redefined in a subclass."
        << std::endl
        << "void " << codename_skeleton() << "::execute_call(forb::call_id_t code," << std::endl
        << blanks << "forb::streams::stream *callstream," << std::endl
        << blanks << "forb::streams::stream *datastream) {" << std::endl;

    out.increment_indentation();

    out << "// callstream is the stream used by the RPC protocol and" << std::endl
        << "// datastream is the one used to exchange actual data" << std::endl
        << "// They might be the same stream" << std::endl;

    out << "switch ((" << name_enum() << ") code) {" << std::endl;
    out.increment_indentation();

    // For each method declared in FORB IDL, print the skeleton part of deserialization of parameters, virtual call
    // and outputs serialization
    for (const auto &it : list()) {
        out << "case " << name_enum() << "::" << it.id() << ": {" << std::endl;

        out.increment_indentation();
        it.print_skeleton_definition(out);
        out.decrement_indentation();

        out << "}" << std::endl;

        out.increment_indentation();
        out << "break;" << std::endl
            << std::endl;
        out.decrement_indentation();
    }

    out.decrement_indentation();
    out << "}" << std::endl;
    out.decrement_indentation();
    out << "}" << std::endl;

}

void forbcc::interface::print_declaration(forbcc::code_ostream &out) const {
    print_stub_declaration(out);
    out << std::endl;

    print_skeleton_declaration(out);
    out << std::endl;

    print_methods_enum(out);
    out << std::endl;
}

void forbcc::interface::print_definition(forbcc::code_ostream &out) const {
    print_static_attributes_definition(out);
    out << std::endl;

    print_factory_match(out);
    out << std::endl;

    print_factory_create(out);
    out << std::endl;

    print_narrows(out);
    out << std::endl;

    print_stub_methods(out);
    out << std::endl;

    print_skeleton_method(out);
    out << std::endl;
}
