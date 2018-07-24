set(CMAKE_INSTALL_RPATH "\\$ORIGIN/../lib")
include_directories("${CMAKE_SOURCE_DIR}/include")
link_directories("${CMAKE_SOURCE_DIR}/src")

add_executable(gel-file "gel-file.c")
target_link_libraries(gel-file  "gel")
install(TARGETS gel-file DESTINATION bin)
if(APPLE)
	set_target_properties(gel-file PROPERTIES INSTALL_RPATH "@executable_path/../lib")
endif()

add_executable(gel-line "gel_line.c")
target_link_libraries(gel-line  "gel" "gel_dwarf")
install(TARGETS gel-line DESTINATION bin)
if(APPLE)
	set_target_properties(gel-line PROPERTIES INSTALL_RPATH "@executable_path/../lib")
endif()

add_executable(gel-prog "gel_prog.c")
target_link_libraries(gel-prog  "gel")
install(TARGETS gel-prog DESTINATION bin)
if(APPLE)
	set_target_properties(gel-prog PROPERTIES INSTALL_RPATH "@executable_path/../lib")
endif()

add_executable(gel-sect "gel-sect.c")
target_link_libraries(gel-sect  "gel")
install(TARGETS gel-sect DESTINATION bin)
if(APPLE)
	set_target_properties(gel-sect PROPERTIES INSTALL_RPATH "@executable_path/../lib")
endif()

add_executable(gel-sym "gel-sym.c")
target_link_libraries(gel-sym  "gel")
install(TARGETS gel-sym DESTINATION bin)
if(APPLE)
	set_target_properties(gel-sym PROPERTIES INSTALL_RPATH "@executable_path/../lib")
endif()

add_executable(gel-im "gel-im.c")
target_link_libraries(gel-im  "gel")
install(TARGETS gel-im DESTINATION bin)
if(APPLE)
	set_target_properties(gel-im PROPERTIES INSTALL_RPATH "@executable_path/../lib")
endif()

# tests
set(TOOLS
	"file"
	"line"
	"prog"
	"sect"
	"sym"
	"im")
set(IM_INPUTS
	"bs.arm"
	"crc-stabs.elf")
set(INPUTS
	${IM_INPUTS}
	"fibcall.ppc"
	"empty.ppc")


# run tests
foreach(TOOL ${TOOLS})
	if("${TOOL}" STREQUAL "im")
		set(CUR_INPUTS ${IM_INPUTS})
	else()
		set(CUR_INPUTS ${INPUTS})
	endif()
	foreach(INPUT ${CUR_INPUTS})
		add_test(simple-${TOOL}-${INPUT} gel-${TOOL} ${CMAKE_SOURCE_DIR}/test/bin/${INPUT})
	endforeach()
endforeach()


# diff tests
foreach(TOOL ${TOOLS})
	if("${TOOL}" STREQUAL "im")
		set(CUR_INPUTS ${IM_INPUTS})
	else()
		set(CUR_INPUTS ${INPUTS})
	endif()
	foreach(INPUT ${CUR_INPUTS})
		add_test(diff-${TOOL}-${INPUT} cmake .. -DDIFF_TOOL=${TOOL} -DDIFF_INPUT=${INPUT})
	endforeach()
endforeach()


# diff tests generation
if(GEN_TOOL)
	if("${GEN_TOOL}" STREQUAL "im")
		set(CUR_INPUTS ${IM_INPUTS})
	else()
		set(CUR_INPUTS ${INPUTS})
	endif()
	foreach(INPUT ${CUR_INPUTS})
		set(IN "${CMAKE_SOURCE_DIR}/test/bin/${INPUT}")
		set(OUT "${CMAKE_SOURCE_DIR}/test/bin/${GEN_TOOL}-${INPUT}.out")
		execute_process(COMMAND gel-${GEN_TOOL} ${IN} OUTPUT_FILE ${OUT})
		message(STATUS "TEST: ${OUT} generated !")
	endforeach()
endif()


# diff test implementation
#	DIFF_TOOL	tool to test
#	DIFF_INPUT	input file
if(DIFF_TOOL)
	set(IN "${CMAKE_SOURCE_DIR}/test/bin/${DIFF_INPUT}")
	set(OUT "${HOME}/tmp/out")
	set(REF "${CMAKE_SOURCE_DIR}/test/bin/${DIFF_TOOL}-${DIFF_INPUT}.out")
	execute_process(COMMAND gel-${DIFF_TOOL} ${IN} RESULT_VARIABLE DIFF_RES OUTPUT_FILE "${OUT}")
	message(STATUS "generated to ${OUT}")
	if(NOT (${DIFF_RES} EQUAL 0))
		message(FATAL_ERROR "command failed")
	else()
		execute_process(COMMAND diff "${OUT}" "${REF}" RESULT_VARIABLE DIFF_RES)
		message(STATUS "making diff between ${OUT} and ${REF}")
		if(NOT (${DIFF_RES} EQUAL 0))
			message(FATAL_ERROR "differences in output")
		else()
			message(STATUS "test successful !")
		endif()
	endif()
	unset(DIFF_TOOL CACHE)
endif()
