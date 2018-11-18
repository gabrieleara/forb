# CMake Version Required
cmake_minimum_required(VERSION 3.6)

# Project Name
project(forb)

# The C++ Standard Version
set(CMAKE_CXX_STANDARD 11)

# Setting default compiling mode to Release
# To compile in Debug mode use cmake -DCMAKE_BUILD_TYPE=Debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "" FORCE)
    message(STATUS "The CMAKE_BUILD_TYPE variable has been set to the default value")
endif ()

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# Flags change depending on DEBUG or RELEASE compilation
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra --pedantic") #-Werror
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# Each subdirectory contains a target
add_subdirectory(compiler)
add_subdirectory(library)
add_subdirectory(tests)


# Doxygen stuff

# Documentation will be built only in release mode
if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    # We specify that we want to build the docs
    option(BUILD_DOC "Build documentation" ON)

    # Checking if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # Set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # Request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # Note the option ALL which allows to build the docs together with the application
        add_custom_target(doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)

endif (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
