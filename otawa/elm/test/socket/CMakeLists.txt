add_executable(server server.cpp)
target_link_libraries(server elm)

add_executable(client client.cpp)
target_link_libraries(client elm)
