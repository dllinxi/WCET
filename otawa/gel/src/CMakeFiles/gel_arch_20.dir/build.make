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
include src/CMakeFiles/gel_arch_20.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/gel_arch_20.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/gel_arch_20.dir/flags.make

src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o: src/CMakeFiles/gel_arch_20.dir/flags.make
src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o: src/plugarch20.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/gel_arch_20.dir/plugarch20.c.o   -c /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src/plugarch20.c

src/CMakeFiles/gel_arch_20.dir/plugarch20.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gel_arch_20.dir/plugarch20.c.i"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src/plugarch20.c > CMakeFiles/gel_arch_20.dir/plugarch20.c.i

src/CMakeFiles/gel_arch_20.dir/plugarch20.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gel_arch_20.dir/plugarch20.c.s"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src/plugarch20.c -o CMakeFiles/gel_arch_20.dir/plugarch20.c.s

src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o.requires:

.PHONY : src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o.requires

src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o.provides: src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o.requires
	$(MAKE) -f src/CMakeFiles/gel_arch_20.dir/build.make src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o.provides.build
.PHONY : src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o.provides

src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o.provides.build: src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o


# Object files for target gel_arch_20
gel_arch_20_OBJECTS = \
"CMakeFiles/gel_arch_20.dir/plugarch20.c.o"

# External object files for target gel_arch_20
gel_arch_20_EXTERNAL_OBJECTS =

src/cyggel_arch_20.dll: src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o
src/cyggel_arch_20.dll: src/CMakeFiles/gel_arch_20.dir/build.make
src/cyggel_arch_20.dll: src/libgel.dll.a
src/cyggel_arch_20.dll: src/CMakeFiles/gel_arch_20.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared module cyggel_arch_20.dll"
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gel_arch_20.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/gel_arch_20.dir/build: src/cyggel_arch_20.dll

.PHONY : src/CMakeFiles/gel_arch_20.dir/build

src/CMakeFiles/gel_arch_20.dir/requires: src/CMakeFiles/gel_arch_20.dir/plugarch20.c.o.requires

.PHONY : src/CMakeFiles/gel_arch_20.dir/requires

src/CMakeFiles/gel_arch_20.dir/clean:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src && $(CMAKE_COMMAND) -P CMakeFiles/gel_arch_20.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/gel_arch_20.dir/clean

src/CMakeFiles/gel_arch_20.dir/depend:
	cd /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src /cygdrive/d/Seafile/Workspace/MyProject/WCET/WCET/otawa/gel/src/CMakeFiles/gel_arch_20.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/gel_arch_20.dir/depend

