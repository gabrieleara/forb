project(forb_compiler)


# The FORB Library Compiler
add_executable(forbcc
        types/type.cpp
        types/type_custom.cpp
        types/type_array.cpp

        module.cpp
        interface.cpp
        method.cpp

        code_ostream.cpp
        prova.cpp
        )


target_include_directories(forbcc
        PRIVATE include)
