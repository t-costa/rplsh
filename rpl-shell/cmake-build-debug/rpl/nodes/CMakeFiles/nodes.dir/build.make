# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /home/tommaso/Downloads/clion-2019.1/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/tommaso/Downloads/clion-2019.1/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tommaso/forked/rplsh/rpl-shell

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug

# Include any dependencies generated for this target.
include rpl/nodes/CMakeFiles/nodes.dir/depend.make

# Include the progress variables for this target.
include rpl/nodes/CMakeFiles/nodes.dir/progress.make

# Include the compile flags for this target's objects.
include rpl/nodes/CMakeFiles/nodes.dir/flags.make

rpl/nodes/CMakeFiles/nodes.dir/skeletons.cpp.o: rpl/nodes/CMakeFiles/nodes.dir/flags.make
rpl/nodes/CMakeFiles/nodes.dir/skeletons.cpp.o: ../rpl/nodes/skeletons.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object rpl/nodes/CMakeFiles/nodes.dir/skeletons.cpp.o"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nodes.dir/skeletons.cpp.o -c /home/tommaso/forked/rplsh/rpl-shell/rpl/nodes/skeletons.cpp

rpl/nodes/CMakeFiles/nodes.dir/skeletons.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nodes.dir/skeletons.cpp.i"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/rpl-shell/rpl/nodes/skeletons.cpp > CMakeFiles/nodes.dir/skeletons.cpp.i

rpl/nodes/CMakeFiles/nodes.dir/skeletons.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nodes.dir/skeletons.cpp.s"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/rpl-shell/rpl/nodes/skeletons.cpp -o CMakeFiles/nodes.dir/skeletons.cpp.s

rpl/nodes/CMakeFiles/nodes.dir/verbs.cpp.o: rpl/nodes/CMakeFiles/nodes.dir/flags.make
rpl/nodes/CMakeFiles/nodes.dir/verbs.cpp.o: ../rpl/nodes/verbs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object rpl/nodes/CMakeFiles/nodes.dir/verbs.cpp.o"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/nodes.dir/verbs.cpp.o -c /home/tommaso/forked/rplsh/rpl-shell/rpl/nodes/verbs.cpp

rpl/nodes/CMakeFiles/nodes.dir/verbs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/nodes.dir/verbs.cpp.i"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/rpl-shell/rpl/nodes/verbs.cpp > CMakeFiles/nodes.dir/verbs.cpp.i

rpl/nodes/CMakeFiles/nodes.dir/verbs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/nodes.dir/verbs.cpp.s"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/rpl-shell/rpl/nodes/verbs.cpp -o CMakeFiles/nodes.dir/verbs.cpp.s

# Object files for target nodes
nodes_OBJECTS = \
"CMakeFiles/nodes.dir/skeletons.cpp.o" \
"CMakeFiles/nodes.dir/verbs.cpp.o"

# External object files for target nodes
nodes_EXTERNAL_OBJECTS =

rpl/nodes/libnodes.a: rpl/nodes/CMakeFiles/nodes.dir/skeletons.cpp.o
rpl/nodes/libnodes.a: rpl/nodes/CMakeFiles/nodes.dir/verbs.cpp.o
rpl/nodes/libnodes.a: rpl/nodes/CMakeFiles/nodes.dir/build.make
rpl/nodes/libnodes.a: rpl/nodes/CMakeFiles/nodes.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libnodes.a"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && $(CMAKE_COMMAND) -P CMakeFiles/nodes.dir/cmake_clean_target.cmake
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/nodes.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rpl/nodes/CMakeFiles/nodes.dir/build: rpl/nodes/libnodes.a

.PHONY : rpl/nodes/CMakeFiles/nodes.dir/build

rpl/nodes/CMakeFiles/nodes.dir/clean:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes && $(CMAKE_COMMAND) -P CMakeFiles/nodes.dir/cmake_clean.cmake
.PHONY : rpl/nodes/CMakeFiles/nodes.dir/clean

rpl/nodes/CMakeFiles/nodes.dir/depend:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tommaso/forked/rplsh/rpl-shell /home/tommaso/forked/rplsh/rpl-shell/rpl/nodes /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/nodes/CMakeFiles/nodes.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rpl/nodes/CMakeFiles/nodes.dir/depend

