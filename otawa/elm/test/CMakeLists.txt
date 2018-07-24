
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")


include_directories("../include")
include_directories(".")

add_library(myplugin MODULE "plugin.cpp")
#set_target_properties(myplugin PROPERTIES OUTPUT_NAME "myplugin" )
target_link_libraries(myplugin elm)
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SOURCES
	"test.cc"
	"test_avl.cpp"
	"test_autostr.cpp"
	"test_bidilist.cpp"
	"test_bitvector.cpp"
	"test_char.cpp"
	"test_dllist.cpp"
	"test_enum_info.cpp"
	"test_file.cpp"
	"test_formatter.cpp"
	"test_frag_table.cpp"
	"test_hashkey.cpp"
	"test_hashtable.cpp"
	"test_ini.cpp"
	"test_int.cpp"
	"test_io_format.cpp"
	"test_jsched.cpp"
	"test_json.cpp"
	"test_ilist.cpp"
	"test_list.cpp"
	"test_listqueue.cpp"
	"test_lock.cpp"
	"test_md5.cpp"
	"test_mutex.cpp"
	"test_option.cpp"
	"test_path.cpp"
	"test_plugin.cpp"
	"test_process.cpp"
	"test_ptr.cpp"
	"test_quicksort"
	"test_ref.cpp"
	"test_simplegc.cpp"
	"test_stack_alloc.cpp"
	"test_stopwatch.cpp"
	"test_stree.cpp"
	"test_string.cpp"
	"test_string_buffer.cpp"
	"test_system.cpp"
	"test_table.cpp"
	"test_utility.cpp"
	"test_vararg.cpp"
	"test_vector.cpp"
	"test_vector_queue.cpp"
	"test_sorted_bintree.cpp"
	"test_type_info.cpp"
	"test_thread.cpp"
	"test_utf8.cpp"
    "test_wah"
)

if(HAS_SOCKET)
	add_subdirectory(socket)
endif()

include_directories("../include")
include_directories(".")

add_executable(test ${TEST_SOURCES})
target_link_libraries(test elm)

add_executable(test_io "test_io.cpp")
target_link_libraries(test_io elm)

add_executable(test_io_output "test_io_output.cpp")
target_link_libraries(test_io_output elm)

add_executable(test_sw "test_sw.cpp")
target_link_libraries(test_sw elm)

add_executable(test_bgc "test_bgc.cpp")
target_link_libraries(test_bgc elm)

add_executable(test_thread "thread.cpp")
target_link_libraries(test_thread elm)
