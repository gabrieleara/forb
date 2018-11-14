project(libforb)

# Needed to export the shared library in standard directories
include(GNUInstallDirs)

# The version of the shared library
set(FORB_LIB_VERSION 0.1)
set(FORB_LIB_SOVERSION 0)

# Our target is the libforb.so
add_library(forb SHARED
    stream/shared_memory.cpp
    stream/socket.cpp
    base_skeleton.cpp
    base_stub.cpp
    remote_registry.cpp)

# Setting the version of the shared library
set_target_properties(forb PROPERTIES
        VERSION ${FORB_LIB_VERSION}
        SOVERSION ${FORB_LIB_SOVERSION})

# "include" directory will be exported
target_include_directories(forb 
    PUBLIC include
    PRIVATE private)

# PThread is required to link this library
target_link_libraries(forb
        PRIVATE pthread)

# This will install in /usr/lib and /usr/include
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/_install CACHE PATH "" FORCE)
else()
    set(CMAKE_INSTALL_PREFIX /usr CACHE PATH "" FORCE)
endif()


# Specifying what to export when installing
install(TARGETS forb
        EXPORT forb-export
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}) # Windows specific, might be useless

# Actually to export include directory we need to copy manually files
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/forb)
