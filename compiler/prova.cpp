#include <iostream>

#include "forbcc.hpp"

namespace forbcc {

    const primitive_type _Void("void", "void");
    const primitive_type _Char("char", "char");
    const primitive_type _Int("int", "int");
    const primitive_type _Short("short", "short");
    const primitive_type _Long("long", "long");
    const primitive_type _LongLong("long long", "long long");

} // namespace forbcc

int main() {
    // Openning a new stream controlled by our code formatter
    forbcc::code_ostream output{std::cout};

    // Array type used within our class
    forbcc::array_type int_array{forbcc::_Int, 10};

    // Our class
    forbcc::custom_type disney{forbcc::global_module, "disney"};

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

    forbcc::array_type intarray{forbcc::_Long, 15};
    forbcc::parameter out_arr{forbcc::direction::OUT, intarray, "arr"};

    forbcc::parameter disney_param{forbcc::direction::INOUT, disney, "custom"};

    forbcc::array_type intmatrix{intarray, 5};
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
    intmethod.print_stub_definition(output, "prova_class", "VOIDMETHOD");

    std::cout << "// Skeleton definition" << std::endl;
    intmethod.print_skeleton_definition(output);


}
