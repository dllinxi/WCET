set(CMAKE_INSTALL_RPATH "\\$ORIGIN")
set(CMAKE_MACOSX_RPATH true)

set(debug_sources "dwarf_line.c" "debug_line.c" "stabs.c")

set(common_sources
	"block.c"
	${debug_sources}
	"file.c"
	"gel_base.c"
	"gel_error.c"
	"gel_cursor.c"
	"gel_infos.c"
	"gel_load.c"
	"gel_memory.c"
	"gel_mem.c"
	"image.c"
	"prog.c"
	"sect.c"
	"sym.c"
	"util.c"
	"plugsys0.c"
	"plugarch0.c")

add_library(gel SHARED ${common_sources} "gel_shared.c")
if(NOT WIN32)
	target_link_libraries(gel "dl")
endif(NOT WIN32)
install(TARGETS gel DESTINATION ${LIBDIR})

add_library(gel_dwarf SHARED ${debug_sources})
target_link_libraries(gel_dwarf "gel")
install(TARGETS gel_dwarf DESTINATION "${LIBDIR}")

add_library(gel_debug SHARED ${debug_sources})
target_link_libraries(gel_debug "gel")
install(TARGETS gel_debug DESTINATION "${LIBDIR}")

add_library(gel_ppc SHARED ${common_sources} "plugarch20.c" "gel_static.c")
install(TARGETS gel_ppc DESTINATION ${LIBDIR})

add_library(gel_arch_20 MODULE "plugarch20.c")
target_link_libraries(gel_arch_20 "gel")
install(TARGETS gel_arch_20 DESTINATION ${LIBDIR})

add_library(gel_arch_40 MODULE "plugarch40.c")
target_link_libraries(gel_arch_40 "gel")
install(TARGETS gel_arch_40 DESTINATION ${LIBDIR})

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/src")

