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
include src/config/CMakeFiles/otawa-config.dir/depend.make

# Include the progress variables for this target.
include src/config/CMakeFiles/otawa-config.dir/progress.make

# Include the compile flags for this target's objects.
include src/config/CMakeFiles/otawa-config.dir/flags.make

src/config/CMakeFiles/otawa-config.dir/config.cpp.o: src/config/CMakeFiles/otawa-config.dir/flags.make
src/config/CMakeFiles/otawa-config.dir/config.cpp.o: src/config/config.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/config/CMakeFiles/otawa-config.dir/config.cpp.o"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config && /usr/bin/c++.exe   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/otawa-config.dir/config.cpp.o -c /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config/config.cpp

src/config/CMakeFiles/otawa-config.dir/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/otawa-config.dir/config.cpp.i"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config && /usr/bin/c++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config/config.cpp > CMakeFiles/otawa-config.dir/config.cpp.i

src/config/CMakeFiles/otawa-config.dir/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/otawa-config.dir/config.cpp.s"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config && /usr/bin/c++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config/config.cpp -o CMakeFiles/otawa-config.dir/config.cpp.s

src/config/CMakeFiles/otawa-config.dir/config.cpp.o.requires:

.PHONY : src/config/CMakeFiles/otawa-config.dir/config.cpp.o.requires

src/config/CMakeFiles/otawa-config.dir/config.cpp.o.provides: src/config/CMakeFiles/otawa-config.dir/config.cpp.o.requires
	$(MAKE) -f src/config/CMakeFiles/otawa-config.dir/build.make src/config/CMakeFiles/otawa-config.dir/config.cpp.o.provides.build
.PHONY : src/config/CMakeFiles/otawa-config.dir/config.cpp.o.provides

src/config/CMakeFiles/otawa-config.dir/config.cpp.o.provides.build: src/config/CMakeFiles/otawa-config.dir/config.cpp.o


# Object files for target otawa-config
otawa__config_OBJECTS = \
"CMakeFiles/otawa-config.dir/config.cpp.o"

# External object files for target otawa-config
otawa__config_EXTERNAL_OBJECTS =

src/config/otawa-config.exe: src/config/CMakeFiles/otawa-config.dir/config.cpp.o
src/config/otawa-config.exe: src/config/CMakeFiles/otawa-config.dir/build.make
src/config/otawa-config.exe: src/prog/libotawa.dll.a
src/config/otawa-config.exe: /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/elm/src/libelm.dll.a
src/config/otawa-config.exe: /cygdrive/c/otawa/lib/libgel.dll.a
src/config/otawa-config.exe: /cygdrive/c/otawa/lib/libgel_dwarf.dll.a
src/config/otawa-config.exe: src/config/CMakeFiles/otawa-config.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable otawa-config.exe"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/otawa-config.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/config/CMakeFiles/otawa-config.dir/build: src/config/otawa-config.exe

.PHONY : src/config/CMakeFiles/otawa-config.dir/build

src/config/CMakeFiles/otawa-config.dir/requires: src/config/CMakeFiles/otawa-config.dir/config.cpp.o.requires

.PHONY : src/config/CMakeFiles/otawa-config.dir/requires

src/config/CMakeFiles/otawa-config.dir/clean:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config && $(CMAKE_COMMAND) -P CMakeFiles/otawa-config.dir/cmake_clean.cmake
.PHONY : src/config/CMakeFiles/otawa-config.dir/clean

src/config/CMakeFiles/otawa-config.dir/depend:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/otawa/src/config/CMakeFiles/otawa-config.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/config/CMakeFiles/otawa-config.dir/depend

