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
include src/odfa/CMakeFiles/odfa.dir/depend.make

# Include the progress variables for this target.
include src/odfa/CMakeFiles/odfa.dir/progress.make

# Include the compile flags for this target's objects.
include src/odfa/CMakeFiles/odfa.dir/flags.make

src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o: src/odfa/CMakeFiles/odfa.dir/flags.make
src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o: src/odfa/odfa.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa && /usr/bin/c++.exe   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/odfa.dir/odfa.cpp.o -c /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa/odfa.cpp

src/odfa/CMakeFiles/odfa.dir/odfa.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/odfa.dir/odfa.cpp.i"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa && /usr/bin/c++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa/odfa.cpp > CMakeFiles/odfa.dir/odfa.cpp.i

src/odfa/CMakeFiles/odfa.dir/odfa.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/odfa.dir/odfa.cpp.s"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa && /usr/bin/c++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa/odfa.cpp -o CMakeFiles/odfa.dir/odfa.cpp.s

src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o.requires:

.PHONY : src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o.requires

src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o.provides: src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o.requires
	$(MAKE) -f src/odfa/CMakeFiles/odfa.dir/build.make src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o.provides.build
.PHONY : src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o.provides

src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o.provides.build: src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o


# Object files for target odfa
odfa_OBJECTS = \
"CMakeFiles/odfa.dir/odfa.cpp.o"

# External object files for target odfa
odfa_EXTERNAL_OBJECTS =

src/odfa/odfa.exe: src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o
src/odfa/odfa.exe: src/odfa/CMakeFiles/odfa.dir/build.make
src/odfa/odfa.exe: src/odisplay/libodisplay.dll.a
src/odfa/odfa.exe: src/prog/libotawa.dll.a
src/odfa/odfa.exe: /cygdrive/c/otawa/lib/libgel.dll.a
src/odfa/odfa.exe: /cygdrive/c/otawa/lib/libgel_dwarf.dll.a
src/odfa/odfa.exe: /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/elm/src/libelm.dll.a
src/odfa/odfa.exe: src/odfa/CMakeFiles/odfa.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable odfa.exe"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/odfa.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/odfa/CMakeFiles/odfa.dir/build: src/odfa/odfa.exe

.PHONY : src/odfa/CMakeFiles/odfa.dir/build

src/odfa/CMakeFiles/odfa.dir/requires: src/odfa/CMakeFiles/odfa.dir/odfa.cpp.o.requires

.PHONY : src/odfa/CMakeFiles/odfa.dir/requires

src/odfa/CMakeFiles/odfa.dir/clean:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa && $(CMAKE_COMMAND) -P CMakeFiles/odfa.dir/cmake_clean.cmake
.PHONY : src/odfa/CMakeFiles/odfa.dir/clean

src/odfa/CMakeFiles/odfa.dir/depend:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/odfa/CMakeFiles/odfa.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/odfa/CMakeFiles/odfa.dir/depend
