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
include rpl/visitors/CMakeFiles/visitors.dir/depend.make

# Include the progress variables for this target.
include rpl/visitors/CMakeFiles/visitors.dir/progress.make

# Include the compile flags for this target's objects.
include rpl/visitors/CMakeFiles/visitors.dir/flags.make

rpl/visitors/CMakeFiles/visitors.dir/visitors.cpp.o: rpl/visitors/CMakeFiles/visitors.dir/flags.make
rpl/visitors/CMakeFiles/visitors.dir/visitors.cpp.o: ../rpl/visitors/visitors.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object rpl/visitors/CMakeFiles/visitors.dir/visitors.cpp.o"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/visitors && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/visitors.dir/visitors.cpp.o -c /home/tommaso/forked/rplsh/rpl-shell/rpl/visitors/visitors.cpp

rpl/visitors/CMakeFiles/visitors.dir/visitors.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/visitors.dir/visitors.cpp.i"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/visitors && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/rpl-shell/rpl/visitors/visitors.cpp > CMakeFiles/visitors.dir/visitors.cpp.i

rpl/visitors/CMakeFiles/visitors.dir/visitors.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/visitors.dir/visitors.cpp.s"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/visitors && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/rpl-shell/rpl/visitors/visitors.cpp -o CMakeFiles/visitors.dir/visitors.cpp.s

# Object files for target visitors
visitors_OBJECTS = \
"CMakeFiles/visitors.dir/visitors.cpp.o"

# External object files for target visitors
visitors_EXTERNAL_OBJECTS =

rpl/visitors/libvisitors.a: rpl/visitors/CMakeFiles/visitors.dir/visitors.cpp.o
rpl/visitors/libvisitors.a: rpl/visitors/CMakeFiles/visitors.dir/build.make
rpl/visitors/libvisitors.a: rpl/visitors/CMakeFiles/visitors.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libvisitors.a"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/visitors && $(CMAKE_COMMAND) -P CMakeFiles/visitors.dir/cmake_clean_target.cmake
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/visitors && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/visitors.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rpl/visitors/CMakeFiles/visitors.dir/build: rpl/visitors/libvisitors.a

.PHONY : rpl/visitors/CMakeFiles/visitors.dir/build

rpl/visitors/CMakeFiles/visitors.dir/clean:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/visitors && $(CMAKE_COMMAND) -P CMakeFiles/visitors.dir/cmake_clean.cmake
.PHONY : rpl/visitors/CMakeFiles/visitors.dir/clean

rpl/visitors/CMakeFiles/visitors.dir/depend:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tommaso/forked/rplsh/rpl-shell /home/tommaso/forked/rplsh/rpl-shell/rpl/visitors /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/visitors /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/visitors/CMakeFiles/visitors.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rpl/visitors/CMakeFiles/visitors.dir/depend

