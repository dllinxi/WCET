cmake_minimum_required (VERSION 2.6)
project (GEL)
set(GEL_MAJOR	1)
set(GEL_MINOR	2)
enable_testing()

# OS dependencies
if(NOT WIN32)
	find_library(DL dl)
	set(LIBDIR "lib")
else(NOT WIN32)
	set(LIBDIR "bin")
endif(NOT WIN32)

# check endianness
include(TestBigEndian)
TEST_BIG_ENDIAN(BIG_ENDIAN)

# build configuration file
configure_file (
  "${PROJECT_SOURCE_DIR}/config.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

# installation level
set(INSTALL_TYPE "all" CACHE STRING "Type of installation (one of all, dev-only, dev, lib, bin).")
if(INSTALL_TYPE MATCHES "all")
    set(INSTALL_DEV "yes")
    set(INSTALL_LIB "yes")
    set(INSTALL_BIN "yes")
elseif(INSTALL_TYPE MATCHES "dev")
    set(INSTALL_DEV "yes")
    set(INSTALL_LIB "yes")
elseif(INSTALL_TYPE MATCHES "dev-only")
    set(INSTALL_DEV "yes")
elseif(INSTALL_TYPE MATCHES "lib")
    set(INSTALL_LIB "yes")
elseif(INSTALL_TYPE MATCHES "bin")
    set(INSTALL_LIB "yes")
    set(INSTALL_BIN "yes")
else()
    set(INSTALL_DEV "yes")
    set(INSTALL_LIB "yes")
    set(INSTALL_BIN "yes")
    message(ERROR ": unknown INSTALL_TYPE: ${INSTALL_TYPE}. Falling back to all.")
endif()


# process subdirectories
if(INSTALL_LIB)
    add_subdirectory(src)
    add_subdirectory(test)
endif()
if(INSTALL_BIN)
    add_subdirectory(tools)
endif()


# install include
if(INSTALL_DEV)
    install(DIRECTORY include/gel DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "CVS" EXCLUDE)
endif()


# documentation
install(FILES "README" "COPYING" "AUTHORS" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/GEL/")

# auto-documentation
if(INSTALL_DEV)
    find_program(DOXYGEN doxygen)
    if(DOXYGEN)
        add_custom_target(autodoc ALL COMMAND "${DOXYGEN}" )
        set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "autodoc")
        message(STATUS "CLEAN=${ADDITIONAL_MAKE_CLEAN_FILES}")
	    install(DIRECTORY autodoc DESTINATION "${CMAKE_INSTALL_PREFIX}/share/GEL/")
    endif()
endif()

