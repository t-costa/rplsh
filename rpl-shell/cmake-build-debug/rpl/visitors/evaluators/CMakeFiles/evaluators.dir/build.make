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
CMAKE_SOURCE_DIR = /home/tommaso/rplsh/rpl-shell

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tommaso/rplsh/rpl-shell/cmake-build-debug

# Include any dependencies generated for this target.
include rpl/visitors/evaluators/CMakeFiles/evaluators.dir/depend.make

# Include the progress variables for this target.
include rpl/visitors/evaluators/CMakeFiles/evaluators.dir/progress.make

# Include the compile flags for this target's objects.
include rpl/visitors/evaluators/CMakeFiles/evaluators.dir/flags.make

rpl/visitors/evaluators/CMakeFiles/evaluators.dir/evaluators.cpp.o: rpl/visitors/evaluators/CMakeFiles/evaluators.dir/flags.make
rpl/visitors/evaluators/CMakeFiles/evaluators.dir/evaluators.cpp.o: ../rpl/visitors/evaluators/evaluators.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object rpl/visitors/evaluators/CMakeFiles/evaluators.dir/evaluators.cpp.o"
	cd /home/tommaso/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/evaluators && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/evaluators.dir/evaluators.cpp.o -c /home/tommaso/rplsh/rpl-shell/rpl/visitors/evaluators/evaluators.cpp

rpl/visitors/evaluators/CMakeFiles/evaluators.dir/evaluators.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/evaluators.dir/evaluators.cpp.i"
	cd /home/tommaso/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/evaluators && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/rplsh/rpl-shell/rpl/visitors/evaluators/evaluators.cpp > CMakeFiles/evaluators.dir/evaluators.cpp.i

rpl/visitors/evaluators/CMakeFiles/evaluators.dir/evaluators.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/evaluators.dir/evaluators.cpp.s"
	cd /home/tommaso/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/evaluators && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/rplsh/rpl-shell/rpl/visitors/evaluators/evaluators.cpp -o CMakeFiles/evaluators.dir/evaluators.cpp.s

# Object files for target evaluators
evaluators_OBJECTS = \
"CMakeFiles/evaluators.dir/evaluators.cpp.o"

# External object files for target evaluators
evaluators_EXTERNAL_OBJECTS =

rpl/visitors/evaluators/libevaluators.a: rpl/visitors/evaluators/CMakeFiles/evaluators.dir/evaluators.cpp.o
rpl/visitors/evaluators/libevaluators.a: rpl/visitors/evaluators/CMakeFiles/evaluators.dir/build.make
rpl/visitors/evaluators/libevaluators.a: rpl/visitors/evaluators/CMakeFiles/evaluators.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tommaso/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libevaluators.a"
	cd /home/tommaso/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/evaluators && $(CMAKE_COMMAND) -P CMakeFiles/evaluators.dir/cmake_clean_target.cmake
	cd /home/tommaso/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/evaluators && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/evaluators.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rpl/visitors/evaluators/CMakeFiles/evaluators.dir/build: rpl/visitors/evaluators/libevaluators.a

.PHONY : rpl/visitors/evaluators/CMakeFiles/evaluators.dir/build

rpl/visitors/evaluators/CMakeFiles/evaluators.dir/clean:
	cd /home/tommaso/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/evaluators && $(CMAKE_COMMAND) -P CMakeFiles/evaluators.dir/cmake_clean.cmake
.PHONY : rpl/visitors/evaluators/CMakeFiles/evaluators.dir/clean

rpl/visitors/evaluators/CMakeFiles/evaluators.dir/depend:
	cd /home/tommaso/rplsh/rpl-shell/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tommaso/rplsh/rpl-shell /home/tommaso/rplsh/rpl-shell/rpl/visitors/evaluators /home/tommaso/rplsh/rpl-shell/cmake-build-debug /home/tommaso/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/evaluators /home/tommaso/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/evaluators/CMakeFiles/evaluators.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rpl/visitors/evaluators/CMakeFiles/evaluators.dir/depend

