# Install script for directory: /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/prog/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ast/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/branch/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/cat/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ccg/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/cfgio/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/etime/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ets/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odec/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odisplay/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/dcache/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ppc2/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/x86/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/lp_solve5/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/dumpcfg/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/mkff/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ostat/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/opcg/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odisasm/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/owcet/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/bpred/cmake_install.cmake")
  include("/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/oipet/cmake_install.cmake")

endif()

