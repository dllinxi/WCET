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
CMAKE_SOURCE_DIR = /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel

# Include any dependencies generated for this target.
include tools/CMakeFiles/gel-line.dir/depend.make

# Include the progress variables for this target.
include tools/CMakeFiles/gel-line.dir/progress.make

# Include the compile flags for this target's objects.
include tools/CMakeFiles/gel-line.dir/flags.make

tools/CMakeFiles/gel-line.dir/gel_line.c.o: tools/CMakeFiles/gel-line.dir/flags.make
tools/CMakeFiles/gel-line.dir/gel_line.c.o: tools/gel_line.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tools/CMakeFiles/gel-line.dir/gel_line.c.o"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/gel-line.dir/gel_line.c.o   -c /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools/gel_line.c

tools/CMakeFiles/gel-line.dir/gel_line.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gel-line.dir/gel_line.c.i"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools/gel_line.c > CMakeFiles/gel-line.dir/gel_line.c.i

tools/CMakeFiles/gel-line.dir/gel_line.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gel-line.dir/gel_line.c.s"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools/gel_line.c -o CMakeFiles/gel-line.dir/gel_line.c.s

tools/CMakeFiles/gel-line.dir/gel_line.c.o.requires:

.PHONY : tools/CMakeFiles/gel-line.dir/gel_line.c.o.requires

tools/CMakeFiles/gel-line.dir/gel_line.c.o.provides: tools/CMakeFiles/gel-line.dir/gel_line.c.o.requires
	$(MAKE) -f tools/CMakeFiles/gel-line.dir/build.make tools/CMakeFiles/gel-line.dir/gel_line.c.o.provides.build
.PHONY : tools/CMakeFiles/gel-line.dir/gel_line.c.o.provides

tools/CMakeFiles/gel-line.dir/gel_line.c.o.provides.build: tools/CMakeFiles/gel-line.dir/gel_line.c.o


# Object files for target gel-line
gel__line_OBJECTS = \
"CMakeFiles/gel-line.dir/gel_line.c.o"

# External object files for target gel-line
gel__line_EXTERNAL_OBJECTS =

tools/gel-line.exe: tools/CMakeFiles/gel-line.dir/gel_line.c.o
tools/gel-line.exe: tools/CMakeFiles/gel-line.dir/build.make
tools/gel-line.exe: src/libgel_dwarf.dll.a
tools/gel-line.exe: src/libgel.dll.a
tools/gel-line.exe: tools/CMakeFiles/gel-line.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable gel-line.exe"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gel-line.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/CMakeFiles/gel-line.dir/build: tools/gel-line.exe

.PHONY : tools/CMakeFiles/gel-line.dir/build

tools/CMakeFiles/gel-line.dir/requires: tools/CMakeFiles/gel-line.dir/gel_line.c.o.requires

.PHONY : tools/CMakeFiles/gel-line.dir/requires

tools/CMakeFiles/gel-line.dir/clean:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools && $(CMAKE_COMMAND) -P CMakeFiles/gel-line.dir/cmake_clean.cmake
.PHONY : tools/CMakeFiles/gel-line.dir/clean

tools/CMakeFiles/gel-line.dir/depend:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/tools/CMakeFiles/gel-line.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/CMakeFiles/gel-line.dir/depend
