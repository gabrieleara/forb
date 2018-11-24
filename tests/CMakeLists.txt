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
add_executable(rpc_server_gen
        rpc_server_gen.cpp
        rpc_class_generated.cpp)

add_executable(rpc_client_gen
        rpc_client_gen.cpp
        rpc_class_generated.cpp)

target_link_libraries(rpc_client_gen PRIVATE forb)
target_link_libraries(rpc_server_gen PRIVATE forb)

# Then we have speed tests
# TODO