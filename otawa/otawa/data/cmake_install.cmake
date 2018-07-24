# Install script for directory: /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/data

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/cygdrive/c/otawa")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Otawa/" TYPE DIRECTORY FILES "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/data/caches" REGEX "/CVS$" EXCLUDE REGEX "/Makefile[^/]*$" EXCLUDE REGEX "/\\.cvsignore$" EXCLUDE)
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Otawa/" TYPE DIRECTORY FILES "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/data/makefiles" REGEX "/CVS$" EXCLUDE REGEX "/Makefile[^/]*$" EXCLUDE REGEX "/\\.cvsignore$" EXCLUDE)
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Otawa/" TYPE DIRECTORY FILES "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/data/procs" REGEX "/CVS$" EXCLUDE REGEX "/Makefile[^/]*$" EXCLUDE REGEX "/\\.cvsignore$" EXCLUDE)
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Otawa/" TYPE DIRECTORY FILES "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/data/scripts" REGEX "/CVS$" EXCLUDE REGEX "/Makefile[^/]*$" EXCLUDE REGEX "/\\.cvsignore$" EXCLUDE)
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Otawa/" TYPE DIRECTORY FILES "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/data/dtd" REGEX "/CVS$" EXCLUDE REGEX "/Makefile[^/]*$" EXCLUDE REGEX "/\\.cvsignore$" EXCLUDE)
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/Otawa/" TYPE DIRECTORY FILES "/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/data/cmake" REGEX "/CVS$" EXCLUDE REGEX "/Makefile[^/]*$" EXCLUDE REGEX "/\\.cvsignore$" EXCLUDE)
endif()

