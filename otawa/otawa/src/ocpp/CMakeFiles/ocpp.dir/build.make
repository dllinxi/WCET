# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake.exe

# The command to remove a file.
RM = /usr/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa

# Include any dependencies generated for this target.
include src/ocpp/CMakeFiles/ocpp.dir/depend.make

# Include the progress variables for this target.
include src/ocpp/CMakeFiles/ocpp.dir/progress.make

# Include the compile flags for this target's objects.
include src/ocpp/CMakeFiles/ocpp.dir/flags.make

src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o: src/ocpp/CMakeFiles/ocpp.dir/flags.make
src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o: src/ocpp/Unmangler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp && /usr/bin/c++.exe   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ocpp.dir/Unmangler.cpp.o -c /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp/Unmangler.cpp

src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ocpp.dir/Unmangler.cpp.i"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp && /usr/bin/c++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp/Unmangler.cpp > CMakeFiles/ocpp.dir/Unmangler.cpp.i

src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ocpp.dir/Unmangler.cpp.s"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp && /usr/bin/c++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp/Unmangler.cpp -o CMakeFiles/ocpp.dir/Unmangler.cpp.s

src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o.requires:

.PHONY : src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o.requires

src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o.provides: src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o.requires
	$(MAKE) -f src/ocpp/CMakeFiles/ocpp.dir/build.make src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o.provides.build
.PHONY : src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o.provides

src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o.provides.build: src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o


# Object files for target ocpp
ocpp_OBJECTS = \
"CMakeFiles/ocpp.dir/Unmangler.cpp.o"

# External object files for target ocpp
ocpp_EXTERNAL_OBJECTS =

src/ocpp/cygocpp.dll: src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o
src/ocpp/cygocpp.dll: src/ocpp/CMakeFiles/ocpp.dir/build.make
src/ocpp/cygocpp.dll: /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/elm/src/libelm.dll.a
src/ocpp/cygocpp.dll: src/ocpp/CMakeFiles/ocpp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library cygocpp.dll"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ocpp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/ocpp/CMakeFiles/ocpp.dir/build: src/ocpp/cygocpp.dll

.PHONY : src/ocpp/CMakeFiles/ocpp.dir/build

src/ocpp/CMakeFiles/ocpp.dir/requires: src/ocpp/CMakeFiles/ocpp.dir/Unmangler.cpp.o.requires

.PHONY : src/ocpp/CMakeFiles/ocpp.dir/requires

src/ocpp/CMakeFiles/ocpp.dir/clean:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp && $(CMAKE_COMMAND) -P CMakeFiles/ocpp.dir/cmake_clean.cmake
.PHONY : src/ocpp/CMakeFiles/ocpp.dir/clean

src/ocpp/CMakeFiles/ocpp.dir/depend:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/ocpp/CMakeFiles/ocpp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/ocpp/CMakeFiles/ocpp.dir/depend

