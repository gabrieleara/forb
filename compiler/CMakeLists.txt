project(forb_compiler)


# The FORB Library Compiler
add_executable(forbcc
        prova.cpp

        code_ostream.cpp
        types/type.cpp
        types/primitive_type.cpp
        types/custom_type.cpp
        types/array_type.cpp
        types/module.cpp
        variable.cpp types/module.cpp include/types/module.hpp parameter.cpp include/parameter.hpp method.cpp include/method.hpp)


target_include_directories(forbcc
        PRIVATE include)
