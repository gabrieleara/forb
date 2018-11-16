#include <iostream>

#include "include/forbcc.hpp"

namespace forbcc {

    const type_primitive _Void("void", "void");
    const type_primitive _Char("char", "char");
    const type_primitive _Int("int", "int");
    const type_primitive _Short("short", "short");
    const type_primitive _Long("long", "long");
    const type_primitive _LongLong("long long", "long long");

} // namespace forbcc

int main() {
    // TODO: something is wrong in this scheme because of pointers, this main works but another program would not

    forbcc::module example{static_cast<forbcc::entity*>(&forbcc::module::global_module), "example"};

    forbcc::type_custom structure{&example, "structure"};

    forbcc::type_array array{forbcc::_Char, 10};

    forbcc::variable attr1{forbcc::_Long, "a1"};
    forbcc::variable attr2{forbcc::_Short, "a2"};
    forbcc::variable attr3{array, "a3"};

    structure.insert(attr1.name, attr1);
    structure.insert(attr2.name, attr2);
    structure.insert(attr3.name, attr3);

    example.insert(structure.name, &structure);

    forbcc::interface rpc_class{&example, "rpc_class"};

    forbcc::method m{&rpc_class, "sum", forbcc::_Int};

    forbcc::parameter p1{forbcc::direction::IN, forbcc::_Int, "a"};
    forbcc::parameter p2{forbcc::direction::IN, forbcc::_Int, "b"};
    forbcc::parameter p3{forbcc::direction::OUT, structure, "c"};

    m.insert(p1.var.name, p1);
    m.insert(p2.var.name, p2);
    m.insert(p3.var.name, p3);

    rpc_class.insert(m.id(), m);

    example.insert(rpc_class.name, &rpc_class);

    forbcc::module::global_module.insert(example.name, &example);

    forbcc::code_ostream output{std::cout};

    std::cout << "############ DECLARATION " << std::endl << std::endl;

    forbcc::module::global_module.print_declaration(output);

    std::cout << "############ DEFINITION " << std::endl << std::endl;

    forbcc::module::global_module.print_definition(output);

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
