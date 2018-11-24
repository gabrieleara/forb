#include <iostream>

#include "forbcc.hpp"
#include "templates/ordered_unique_list.hpp"

namespace forbcc {
} // namespace forbcc

using namespace std;
using namespace forbcc;

int main() {

    // Must be executed at startup

    // START INITIALIZATION CODE

    type_primitive::known_types.insert("void", forbcc::_Void);
    type_primitive::known_types.insert("char", forbcc::_Char);
    type_primitive::known_types.insert("int", forbcc::_Int);
    type_primitive::known_types.insert("short", forbcc::_Short);
    type_primitive::known_types.insert("long", forbcc::_Long);

    std::shared_ptr<module>      current_module    = forbcc::module::global_module;
    std::shared_ptr<type_custom> current_type      = nullptr;
    std::shared_ptr<interface>   current_interface = nullptr;
    std::shared_ptr<type_array>  current_array     = nullptr;

    // END INITIALIZATION CODE

    // TODO: For each insert check for successfullness or not

    // CHECKED OK
    module::ptr_t new_module = module::new_ptr(current_module, "example");
    current_module->insert(new_module->codename(), new_module);
    current_module = new_module;

    // module example{current_module, "example"};
    // string codename = example.codename();
    // modules.insert(codename, example);
    // current_module = &modules[codename];

    // CHECKED OK
    type_custom::ptr_t new_structure = type_custom::new_ptr(current_module, "structure");
    current_module->insert("structure", new_structure);
    current_type = new_structure;

    // customs.insert(codename, structure);
    // current_type = &customs[codename];
    // current_module->insert("structure", current_type);

    // CHECKED OK
    // Arrays should be put in an array list, to avoid allocating multiple similar arrays
    type_array::ptr_t new_array = type_array::new_ptr(type_primitive::known_types["char"], 10);
    arrays.insert(new_array->codename(), new_array);
    current_array = new_array;

    // Arrays do not need to be added to modules of course

    // Adding attributes to current_type is easy
    forbcc::variable attr1{type_primitive::known_types["long"], "a1"};
    current_type->insert(attr1.name(), attr1);
    forbcc::variable attr2{type_primitive::known_types["short"], "a2"};
    current_type->insert(attr2.name(), attr2);
    forbcc::variable attr3{current_array, "a3"};
    current_type->insert(attr3.name(), attr3);

    // CHECKED OK
    forbcc::interface::ptr_t new_interface = interface::new_ptr(current_module, "rpc_class");
    // interfaces.insert(rpc_class.codename(), rpc_class);
    // current_interface = &interfaces[rpc_class.codename()];
    current_module->insert("rpc_class", new_interface);
    current_interface = new_interface;

    forbcc::method m{current_interface, "sum", type_primitive::known_types["int"]};

    forbcc::parameter p1{forbcc::direction::IN, type_primitive::known_types["int"], "a"};
    forbcc::parameter p2{forbcc::direction::IN, type_primitive::known_types["int"], "b"};
    forbcc::parameter p3{forbcc::direction::OUT, current_type, "c"};

    m.insert(p1.var().name(), p1);
    m.insert(p2.var().name(), p2);
    m.insert(p3.var().name(), p3);

    current_interface->insert(m.id(), m);

    forbcc::code_ostream output{std::cout};

    std::cout << "############ DECLARATION " << std::endl << std::endl;

    forbcc::module::global_module->print_declaration(output);

    std::cout << "############ DEFINITION " << std::endl << std::endl;

    forbcc::module::global_module->print_definition(output);

    /*
    // Openning a new stream controlled by our code formatter
    forbcc::code_ostream output{std::cout};

    // Array type used within our class
    forbcc::type_array int_array{forbcc::_Int, 10};

    // Our class
    forbcc::type_custom disney{forbcc::global_module, "disney"};

    // Attributes of our class
    forbcc::variable pippo{&disney, forbcc::_Int, "pippo"};
    forbcc::variable pluto{&disney, forbcc::_Long, "pluto"};
    forbcc::variable topolino{&disney, int_array, "topolino"};

    // Adding attributes to class
    disney.insert_attribute(pippo);
    disney.insert_attribute(pluto);
    disney.insert_attribute(topolino);

    std::cout << "The wrong operation returned: "
              << disney.insert_attribute(pippo) << std::endl;

    // Generate header file
    std::cout << "###### HEADER FILE ######" << std::endl;

    std::cout << "#include <forb/forb.hpp>" << std::endl
              << std::endl;

    disney.print_declaration(output);

    // Generate implementation file
    std::cout << "###### IMPLEMENTATION FILE ######" << std::endl;

    disney.print_definition(output, std::string(""));

    std::cout << std::endl;


    std::cout << "###### METHOD TRY ######" << std::endl;

    forbcc::method intmethod{forbcc::_Int, "voidm"};

    forbcc::parameter in_a{forbcc::direction::IN, forbcc::_Int, "a"};
    forbcc::parameter in_b{forbcc::direction::IN, forbcc::_Int, "b"};
    forbcc::parameter out_c{forbcc::direction::OUT, forbcc::_Int, "c"};

    forbcc::type_array intarray{forbcc::_Long, 15};
    forbcc::parameter out_arr{forbcc::direction::OUT, intarray, "arr"};

    forbcc::parameter disney_param{forbcc::direction::INOUT, disney, "custom"};

    forbcc::type_array intmatrix{intarray, 5};
    forbcc::parameter in_matrix{forbcc::direction::IN, intmatrix, "matrix"};

    intmethod.insert_parameter(in_a);
    intmethod.insert_parameter(in_b);
    intmethod.insert_parameter(out_c);
    intmethod.insert_parameter(out_arr);
    intmethod.insert_parameter(disney_param);
    intmethod.insert_parameter(in_matrix);

    std::cout << "// Declaration" << std::endl;
    intmethod.print_declaration(output);

    std::cout << "// Stub definition" << std::endl;
    intmethod.print_stub_definition(output, "prova_class");

    std::cout << "// Skeleton definition" << std::endl;
    intmethod.print_skeleton_definition(output);
     */


}
