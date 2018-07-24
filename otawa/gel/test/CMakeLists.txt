include_directories("../include")
link_directories("${CMAKE_SOURCE_DIR}/src")

add_executable(test_stabs "test_stabs.c")
target_link_libraries(test_stabs  "gel")
