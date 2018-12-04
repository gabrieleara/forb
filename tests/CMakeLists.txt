# The current project is used to test the library functionality
project(forb_test)

# The first two targets are used to test the library with manually written classes
add_executable(rpc_client_manual
        rpc_class_manual.cpp
        rpc_client_manual.cpp)
add_executable(rpc_server_manual
        rpc_class_manual.cpp
        rpc_server_manual.cpp)

# The two targets of course require the library
target_link_libraries(rpc_client_manual PRIVATE forb)
target_link_libraries(rpc_server_manual PRIVATE forb)

# The second two targets are basically the same, but with automatically generated classes


# The following macro tells CMake how to generate source/header pairs starting
# from *.forb files given as input, using the forbcc command.
MACRO(FORB_FILES_DECLARATION FORBFILES)

    # Current source directory for CMake, relative paths are relative
    # to this directory
    SET(PATH_SRC ${CMAKE_CURRENT_SOURCE_DIR})

    # Files shall be separated by ';'
    FOREACH (FILE ${FORBFILES})
        # File name without the extension
        get_filename_component(FILE_NAME ${FILE} NAME)
        get_filename_component(FILE_WE ${FILE} NAME_WE)
        get_filename_component(FILE_DIR ${FILE} DIRECTORY)

        # A complex check to get absolute path from input file string
        if (FILE_DIR)
            if (IS_ABSOLUTE FILE_DIR)
                set(FILE_PATH ${FILE_DIR})
            else ()
                set(FILE_PATH "${PATH_SRC}/${FILE_DIR}")
            endif (IS_ABSOLUTE FILE_DIR)
        else ()
            set(FILE_PATH ${PATH_SRC})
        endif (FILE_DIR)

        # FILE_PATH now is the absolute path of the input file
        # FILE_IN will be the absolute reference to the input file
        set(FILE_IN "${FILE_PATH}/${FILE_NAME}")

        # Convert
        ADD_CUSTOM_COMMAND(
                # OUTPUT ${FILE_PATH}/${FILE_WE}.cpp ${FILE_PATH}/${FILE_WE}.hpp # Remove ${FILE_PATH}/ for out-of-source compilation of cpp and hpp files
                OUTPUT ${FILE_WE}.cpp ${FILE_WE}.hpp
                COMMAND forbcc
                ARGS ${FILE_IN}
                # WORKING_DIRECTORY ${FILE_PATH} # Remove this line for out-of-source compilation of cpp and hpp files
                COMMENT "Generating ${FILE_WE}.cpp and ${FILE_WE}.hpp from ${FILE}"
                DEPENDS forbcc ${FILE_IN}
        )
    ENDFOREACH (FILE)
ENDMACRO(FORB_FILES_DECLARATION)

# Put each of them inside a string, separated by ';'
FORB_FILES_DECLARATION(example.forb)

# The generated header file will be in the binary directory (for out-of-source build)
include_directories(example_client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
include_directories(example_server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})


add_executable(example_client example.cpp main_client_example.cpp)

add_executable(example_server example.cpp main_server_example.cpp)


target_link_libraries(example_client PRIVATE forb)
target_link_libraries(example_server PRIVATE forb)