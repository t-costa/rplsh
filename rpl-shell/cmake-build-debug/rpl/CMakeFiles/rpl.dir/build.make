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
include rpl/CMakeFiles/rpl.dir/depend.make

# Include the progress variables for this target.
include rpl/CMakeFiles/rpl.dir/progress.make

# Include the compile flags for this target's objects.
include rpl/CMakeFiles/rpl.dir/flags.make

rpl/CMakeFiles/rpl.dir/interpreter.cpp.o: rpl/CMakeFiles/rpl.dir/flags.make
rpl/CMakeFiles/rpl.dir/interpreter.cpp.o: ../rpl/interpreter.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object rpl/CMakeFiles/rpl.dir/interpreter.cpp.o"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rpl.dir/interpreter.cpp.o -c /home/tommaso/forked/rplsh/rpl-shell/rpl/interpreter.cpp

rpl/CMakeFiles/rpl.dir/interpreter.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rpl.dir/interpreter.cpp.i"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/rpl-shell/rpl/interpreter.cpp > CMakeFiles/rpl.dir/interpreter.cpp.i

rpl/CMakeFiles/rpl.dir/interpreter.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rpl.dir/interpreter.cpp.s"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/rpl-shell/rpl/interpreter.cpp -o CMakeFiles/rpl.dir/interpreter.cpp.s

rpl/CMakeFiles/rpl.dir/history.cpp.o: rpl/CMakeFiles/rpl.dir/flags.make
rpl/CMakeFiles/rpl.dir/history.cpp.o: ../rpl/history.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object rpl/CMakeFiles/rpl.dir/history.cpp.o"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rpl.dir/history.cpp.o -c /home/tommaso/forked/rplsh/rpl-shell/rpl/history.cpp

rpl/CMakeFiles/rpl.dir/history.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rpl.dir/history.cpp.i"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/rpl-shell/rpl/history.cpp > CMakeFiles/rpl.dir/history.cpp.i

rpl/CMakeFiles/rpl.dir/history.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rpl.dir/history.cpp.s"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/rpl-shell/rpl/history.cpp -o CMakeFiles/rpl.dir/history.cpp.s

rpl/CMakeFiles/rpl.dir/tab_completion.cpp.o: rpl/CMakeFiles/rpl.dir/flags.make
rpl/CMakeFiles/rpl.dir/tab_completion.cpp.o: ../rpl/tab_completion.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object rpl/CMakeFiles/rpl.dir/tab_completion.cpp.o"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/rpl.dir/tab_completion.cpp.o -c /home/tommaso/forked/rplsh/rpl-shell/rpl/tab_completion.cpp

rpl/CMakeFiles/rpl.dir/tab_completion.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rpl.dir/tab_completion.cpp.i"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/rpl-shell/rpl/tab_completion.cpp > CMakeFiles/rpl.dir/tab_completion.cpp.i

rpl/CMakeFiles/rpl.dir/tab_completion.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rpl.dir/tab_completion.cpp.s"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/rpl-shell/rpl/tab_completion.cpp -o CMakeFiles/rpl.dir/tab_completion.cpp.s

# Object files for target rpl
rpl_OBJECTS = \
"CMakeFiles/rpl.dir/interpreter.cpp.o" \
"CMakeFiles/rpl.dir/history.cpp.o" \
"CMakeFiles/rpl.dir/tab_completion.cpp.o"

# External object files for target rpl
rpl_EXTERNAL_OBJECTS =

rpl/librpl.a: rpl/CMakeFiles/rpl.dir/interpreter.cpp.o
rpl/librpl.a: rpl/CMakeFiles/rpl.dir/history.cpp.o
rpl/librpl.a: rpl/CMakeFiles/rpl.dir/tab_completion.cpp.o
rpl/librpl.a: rpl/CMakeFiles/rpl.dir/build.make
rpl/librpl.a: rpl/CMakeFiles/rpl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library librpl.a"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && $(CMAKE_COMMAND) -P CMakeFiles/rpl.dir/cmake_clean_target.cmake
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rpl.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rpl/CMakeFiles/rpl.dir/build: rpl/librpl.a

.PHONY : rpl/CMakeFiles/rpl.dir/build

rpl/CMakeFiles/rpl.dir/clean:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl && $(CMAKE_COMMAND) -P CMakeFiles/rpl.dir/cmake_clean.cmake
.PHONY : rpl/CMakeFiles/rpl.dir/clean

rpl/CMakeFiles/rpl.dir/depend:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tommaso/forked/rplsh/rpl-shell /home/tommaso/forked/rplsh/rpl-shell/rpl /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/CMakeFiles/rpl.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rpl/CMakeFiles/rpl.dir/depend

