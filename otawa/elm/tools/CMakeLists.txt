set(CMAKE_INSTALL_RPATH "\\$ORIGIN/../lib")

include_directories("../include")

add_executable(elm-plugin "elm-plugin.cpp")
target_link_libraries(elm-plugin elm)
install(TARGETS elm-plugin DESTINATION bin)

if(APPLE)
	set_target_properties(elm-plugin PROPERTIES INSTALL_RPATH "@executable_path/../lib")
endif()
