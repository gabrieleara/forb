#include <iostream>

#include "forbcc.hpp"
#include "ordered_unique_list.hpp"

namespace forbcc {

    const std::shared_ptr<const type_primitive> _Void = std::make_shared<type_primitive>("void", "void");
    const std::shared_ptr<const type_primitive> _Char = std::make_shared<type_primitive>("char", "char");
    const std::shared_ptr<const type_primitive> _Int = std::make_shared<type_primitive>("int", "int");
    const std::shared_ptr<const type_primitive> _Short = std::make_shared<type_primitive>("short", "short");
    const std::shared_ptr<const type_primitive> _Long = std::make_shared<type_primitive>("long", "long");

    // TODO: change primitive list
    using type_primitive_list = ordered_unique_list<std::shared_ptr<const type_primitive>>;
    // using type_custom_list = ordered_unique_list<type_custom>;
    using type_array_list = ordered_unique_list<std::shared_ptr<const type_array>>;

    // using interface_list = ordered_unique_list<interface>;
    // using module_list = ordered_unique_list<const module*>;

    type_primitive_list primitives;
    // type_custom_list customs;
    type_array_list arrays;

    // interface_list interfaces;
    // module_list modules;


} // namespace forbcc

using namespace std;
using namespace forbcc;

int main() {

    // Must be executed at startup

    // START INITIALIZATION CODE

    primitives.insert("void", forbcc::_Void);
    primitives.insert("char", forbcc::_Char);
    primitives.insert("int", forbcc::_Int);
    primitives.insert("short", forbcc::_Short);
    primitives.insert("long", forbcc::_Long);

    std::shared_ptr<module> current_module = forbcc::module::global_module;
    std::shared_ptr<type_custom> current_type = nullptr;
    std::shared_ptr<interface> current_interface = nullptr;
    std::shared_ptr<type_array> current_array = nullptr;

    // END INITIALIZATION CODE

    // TODO: For each insert check for successfullness or not

    // CHECKED OK
    std::shared_ptr<module> new_module = make_shared<module>(current_module, "example");
    current_module->insert(new_module->codename(), new_module);
    current_module = new_module;

    // module example{current_module, "example"};
    // string codename = example.codename();
    // modules.insert(codename, example);
    // current_module = &modules[codename];

    // CHECKED OK
    std::shared_ptr<type_custom> new_structure = make_shared<type_custom>(current_module, "structure");
    current_module->insert("structure", new_structure);
    current_type = new_structure;

    // customs.insert(codename, structure);
    // current_type = &customs[codename];
    // current_module->insert("structure", current_type);

    // CHECKED OK
    // Arrays should be put in an array list, to avoid allocating multiple similar arrays
    std::shared_ptr<type_array> new_array = make_shared<type_array>(primitives["char"], 10);
    arrays.insert(new_array->codename(), new_array);
    current_array = new_array;

    // Arrays do not need to be added to modules of course

    // Adding attributes to current_type is easy
    forbcc::variable attr1{primitives["long"], "a1"};
    current_type->insert(attr1.name(), attr1);
    forbcc::variable attr2{primitives["short"], "a2"};
    current_type->insert(attr2.name(), attr2);
    forbcc::variable attr3{current_array, "a3"};
    current_type->insert(attr3.name(), attr3);

    // CHECKED OK
    std::shared_ptr<forbcc::interface> new_interface = std::make_shared<interface>(current_module, "rpc_class");
    // interfaces.insert(rpc_class.codename(), rpc_class);
    // current_interface = &interfaces[rpc_class.codename()];
    current_module->insert("rpc_class", new_interface);
    current_interface = new_interface;

    forbcc::method m{current_interface, "sum", primitives["int"]};

    forbcc::parameter p1{forbcc::direction::IN, primitives["int"], "a"};
    forbcc::parameter p2{forbcc::direction::IN, primitives["int"], "b"};
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
