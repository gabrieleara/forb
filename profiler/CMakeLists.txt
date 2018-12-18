###################### SETTINGS #######################

# CMake Version Required
cmake_minimum_required(VERSION 3.2)

# The current project is used to profile the speedup provided by the library
project(forb_profiling)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# The C++ Standard Version
set(CMAKE_CXX_STANDARD 11)

# Flags change depending on DEBUG or RELEASE compilation
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra --pedantic -Werror")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# TODO: remove this one
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")

########################################################################################################################

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
                OUTPUT ${FILE_PATH}/${FILE_WE}.cpp ${FILE_PATH}/${FILE_WE}.hpp # Remove ${FILE_PATH}/ for out-of-source compilation of cpp and hpp files
                OUTPUT ${FILE_WE}.cpp ${FILE_WE}.hpp
                COMMAND forbcc
                ARGS ${FILE_IN}
                WORKING_DIRECTORY ${FILE_PATH} # Remove this line for out-of-source compilation of cpp and hpp files
                COMMENT "Generating ${FILE_WE}.cpp and ${FILE_WE}.hpp from ${FILE}"
                DEPENDS forbcc ${FILE_IN}
        )
    ENDFOREACH (FILE)
ENDMACRO(FORB_FILES_DECLARATION)


# Declaring the forb file to generate profiler_class.cpp and profiler_class.hpp
FORB_FILES_DECLARATION(profiler_class.forb)

########################################################################################################################

####################### TARGETS #######################

# Two targets will be defined, a server and a client. The server basically does nothing, it's a passive
# class that only receives a bunch of data sent by the client.
# The client will send to the server data with different sizes using various handles to the same server
# and it will provide in output timing informations.

# The generated header file will be in the binary directory (for out-of-source build)
# include_directories(profiler_server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# include_directories(profiler_client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# The server
add_executable(profiler_server profiler_class.cpp profiler_main_server.cpp)
target_link_libraries(profiler_server PRIVATE forb)

# The client
add_executable(profiler_client profiler_class.cpp profiler_main_client.cpp)
target_link_libraries(profiler_client PRIVATE forb)

# The profiler program will spawn both the server and the client
# add_executable(profiler profiler_main.cpp)
# add_dependencies(profiler profiler_client profiler_server)

message("CXX Flags DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message("CXX Flags RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
