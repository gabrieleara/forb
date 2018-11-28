project(forb_compiler)


# The FORB Library Compiler
add_executable(forbcc
        types/type.cpp
        types/type_primitive.cpp
        types/type_struct.cpp
        types/type_array.cpp

        module.cpp
        interface.cpp
        method.cpp

        code_ostream.cpp
        preprocessor.cpp
        parser.cpp
        file_utils.cpp
        compiler_main.cpp)

# The FORB Library Pre-processor
add_executable(forbpp
        preprocessor.cpp
        file_utils.cpp
        preprocessor_main.cpp)

target_include_directories(forbcc
        PRIVATE include)

target_include_directories(forbpp
        PRIVATE include)
