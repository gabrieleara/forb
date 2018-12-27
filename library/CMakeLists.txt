###################### SETTINGS #######################

# CMake Version Required
cmake_minimum_required(VERSION 3.2)

# The project name
project(forb_library VERSION 1.0 LANGUAGES CXX)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# The C++ Standard Version
set(CMAKE_CXX_STANDARD 11)

# Flags change depending on DEBUG or RELEASE compilation
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall -Wextra --pedantic -Werror")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

################## LIBRARY SETTINGS ###################

# The version of the shared library
set(FORB_LIB_VERSION 0.1)
set(FORB_LIB_SOVERSION 0)

# By specifying forb as target, we build the libforb.so file
add_library(forb SHARED
        stream/shared_memory.cpp
        base_skeleton.cpp
        base_stub.cpp
        remote_registry.cpp stream/socket.cpp include/forb/exception.hpp include/forb/make_unique.hpp)

# Setting the version of the shared library
set_target_properties(forb PROPERTIES
        VERSION ${FORB_LIB_VERSION}
        SOVERSION ${FORB_LIB_SOVERSION})

# Include paths for this library. PUBLIC headers are used both to compile
# this library and will be added to consumers' build paths when built
# using CMake.
# In this case, "include" directory will be exported, while "private"
# directory will not and will only be used to build the library itself.
target_include_directories(forb PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE private)

# PThread is required to link this library
target_link_libraries(forb PRIVATE pthread)

# Install directory changes depending on build mode
if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
    # During debug, the library will be installed into a local directory
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/_install CACHE PATH "" FORCE)
else ()
    # This will install in /usr/lib and /usr/include
    set(CMAKE_INSTALL_PREFIX /usr CACHE PATH "" FORCE)
endif ()


# Specifying what to export when installing (GNUInstallDirs required)
install(TARGETS forb
        EXPORT libforb-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}) # Windows specific

# Actually to export the "include" directory we need to copy manually files
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This exports a CMake file that allows the project to be imported
# into other CMake projects from the install directory.
# The directory in which the file will be installed is a per-project
# directory in this case, otherwise use common cmake directory
install(EXPORT libforb-config DESTINATION share/libforb/cmake)

# This makes the project importable from the build directory
export(TARGETS forb FILE libforb-config.cmake)

# Any CMake project that needs to use FORB library can simply import all
# its settings by using the following command within CMakeLists.txt:
#
#   find_package(libforb)