project(forb_compiler)


# The FORB Library Compiler
add_executable(forbcc
        prova.cpp
        code_ostream.cpp
        module.cpp
        types/type.cpp
        types/type_custom.cpp
        types/type_array.cpp
        method.cpp
        interface.cpp)


target_include_directories(forbcc
        PRIVATE include)
