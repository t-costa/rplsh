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
include rpl/cpp_parser/CMakeFiles/cpp_parser.dir/depend.make

# Include the progress variables for this target.
include rpl/cpp_parser/CMakeFiles/cpp_parser.dir/progress.make

# Include the compile flags for this target's objects.
include rpl/cpp_parser/CMakeFiles/cpp_parser.dir/flags.make

rpl/cpp_parser/CMakeFiles/cpp_parser.dir/cpp_parser.cpp.o: rpl/cpp_parser/CMakeFiles/cpp_parser.dir/flags.make
rpl/cpp_parser/CMakeFiles/cpp_parser.dir/cpp_parser.cpp.o: ../rpl/cpp_parser/cpp_parser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object rpl/cpp_parser/CMakeFiles/cpp_parser.dir/cpp_parser.cpp.o"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/cpp_parser && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cpp_parser.dir/cpp_parser.cpp.o -c /home/tommaso/forked/rplsh/rpl-shell/rpl/cpp_parser/cpp_parser.cpp

rpl/cpp_parser/CMakeFiles/cpp_parser.dir/cpp_parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cpp_parser.dir/cpp_parser.cpp.i"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/cpp_parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/rpl-shell/rpl/cpp_parser/cpp_parser.cpp > CMakeFiles/cpp_parser.dir/cpp_parser.cpp.i

rpl/cpp_parser/CMakeFiles/cpp_parser.dir/cpp_parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cpp_parser.dir/cpp_parser.cpp.s"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/cpp_parser && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/rpl-shell/rpl/cpp_parser/cpp_parser.cpp -o CMakeFiles/cpp_parser.dir/cpp_parser.cpp.s

# Object files for target cpp_parser
cpp_parser_OBJECTS = \
"CMakeFiles/cpp_parser.dir/cpp_parser.cpp.o"

# External object files for target cpp_parser
cpp_parser_EXTERNAL_OBJECTS =

rpl/cpp_parser/libcpp_parser.a: rpl/cpp_parser/CMakeFiles/cpp_parser.dir/cpp_parser.cpp.o
rpl/cpp_parser/libcpp_parser.a: rpl/cpp_parser/CMakeFiles/cpp_parser.dir/build.make
rpl/cpp_parser/libcpp_parser.a: rpl/cpp_parser/CMakeFiles/cpp_parser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libcpp_parser.a"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/cpp_parser && $(CMAKE_COMMAND) -P CMakeFiles/cpp_parser.dir/cmake_clean_target.cmake
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/cpp_parser && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cpp_parser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rpl/cpp_parser/CMakeFiles/cpp_parser.dir/build: rpl/cpp_parser/libcpp_parser.a

.PHONY : rpl/cpp_parser/CMakeFiles/cpp_parser.dir/build

rpl/cpp_parser/CMakeFiles/cpp_parser.dir/clean:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/cpp_parser && $(CMAKE_COMMAND) -P CMakeFiles/cpp_parser.dir/cmake_clean.cmake
.PHONY : rpl/cpp_parser/CMakeFiles/cpp_parser.dir/clean

rpl/cpp_parser/CMakeFiles/cpp_parser.dir/depend:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tommaso/forked/rplsh/rpl-shell /home/tommaso/forked/rplsh/rpl-shell/rpl/cpp_parser /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/cpp_parser /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/cpp_parser/CMakeFiles/cpp_parser.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rpl/cpp_parser/CMakeFiles/cpp_parser.dir/depend
