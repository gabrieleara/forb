project(forb_compiler)


# The FORB Library Compiler
add_executable(forbprova
        types/type.cpp
        types/type_primitive.cpp
        types/type_custom.cpp
        types/type_array.cpp

        module.cpp
        interface.cpp
        method.cpp

        code_ostream.cpp
        prova.cpp
        )

add_executable(forbcc
        types/type.cpp
        types/type_primitive.cpp
        types/type_custom.cpp
        types/type_array.cpp

        module.cpp
        interface.cpp
        method.cpp

        code_ostream.cpp
        preprocessor.cpp
        parser.cpp)

add_executable(forbpp
        preprocessor_main.cpp
        preprocessor.cpp)

target_include_directories(forbcc
        PRIVATE include)

target_include_directories(forbpp
        PRIVATE include)
