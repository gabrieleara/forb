###################### SETTINGS #######################

# CMake Version Required
cmake_minimum_required(VERSION 3.2)

# The project name
project(forb_compiler VERSION 1.0 LANGUAGES CXX)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# The C++ Standard Version
set(CMAKE_CXX_STANDARD 11)

# Flags change depending on DEBUG or RELEASE compilation
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wextra --pedantic -Werror")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

####################### TARGETS #######################

# The FORB Library Compiler
add_executable(forbcc
        types/type.cpp
        types/type_primitive.cpp
        types/type_struct.cpp
        types/type_array.cpp

        module.cpp
        interface.cpp
        method.cpp

        preprocessor.cpp
        parser.cpp
        generator.cpp

        code_ostream.cpp
        file_utils.cpp
        main_compiler.cpp)

# The FORB Library Pre-processor
add_executable(forbpp
        preprocessor.cpp
        file_utils.cpp
        main_preprocessor.cpp)

# Include directories, used to lookup for header files
target_include_directories(forbcc PRIVATE include)

# Include directories, used to lookup for header files
target_include_directories(forbpp PRIVATE include)

# Specifying what to export when installing (GNUInstallDirs required)
install(TARGETS forbcc
        EXPORT forbcc-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}) # Windows specific

# This exports a CMake file that allows the project to be imported
# into other CMake projects from the install directory.
# The directory in which the file will be installed is a per-project
# directory in this case, otherwise use common cmake directory
install(EXPORT forbcc-config DESTINATION share/forbcc/cmake)

# This makes the project importable from the build directory
export(TARGETS forbcc FILE forbcc-config.cmake)
