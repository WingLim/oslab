# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/dev/oslab/lab3/shared_mem

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/dev/oslab/lab3/shared_mem/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/recv.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/recv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/recv.dir/flags.make

CMakeFiles/recv.dir/recv.c.o: CMakeFiles/recv.dir/flags.make
CMakeFiles/recv.dir/recv.c.o: ../recv.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/oslab/lab3/shared_mem/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/recv.dir/recv.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/recv.dir/recv.c.o   -c /mnt/d/dev/oslab/lab3/shared_mem/recv.c

CMakeFiles/recv.dir/recv.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/recv.dir/recv.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/dev/oslab/lab3/shared_mem/recv.c > CMakeFiles/recv.dir/recv.c.i

CMakeFiles/recv.dir/recv.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/recv.dir/recv.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/dev/oslab/lab3/shared_mem/recv.c -o CMakeFiles/recv.dir/recv.c.s

CMakeFiles/recv.dir/recv.c.o.requires:

.PHONY : CMakeFiles/recv.dir/recv.c.o.requires

CMakeFiles/recv.dir/recv.c.o.provides: CMakeFiles/recv.dir/recv.c.o.requires
	$(MAKE) -f CMakeFiles/recv.dir/build.make CMakeFiles/recv.dir/recv.c.o.provides.build
.PHONY : CMakeFiles/recv.dir/recv.c.o.provides

CMakeFiles/recv.dir/recv.c.o.provides.build: CMakeFiles/recv.dir/recv.c.o


# Object files for target recv
recv_OBJECTS = \
"CMakeFiles/recv.dir/recv.c.o"

# External object files for target recv
recv_EXTERNAL_OBJECTS =

recv: CMakeFiles/recv.dir/recv.c.o
recv: CMakeFiles/recv.dir/build.make
recv: CMakeFiles/recv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/dev/oslab/lab3/shared_mem/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable recv"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/recv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/recv.dir/build: recv

.PHONY : CMakeFiles/recv.dir/build

CMakeFiles/recv.dir/requires: CMakeFiles/recv.dir/recv.c.o.requires

.PHONY : CMakeFiles/recv.dir/requires

CMakeFiles/recv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/recv.dir/cmake_clean.cmake
.PHONY : CMakeFiles/recv.dir/clean

CMakeFiles/recv.dir/depend:
	cd /mnt/d/dev/oslab/lab3/shared_mem/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/dev/oslab/lab3/shared_mem /mnt/d/dev/oslab/lab3/shared_mem /mnt/d/dev/oslab/lab3/shared_mem/cmake-build-debug /mnt/d/dev/oslab/lab3/shared_mem/cmake-build-debug /mnt/d/dev/oslab/lab3/shared_mem/cmake-build-debug/CMakeFiles/recv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/recv.dir/depend

