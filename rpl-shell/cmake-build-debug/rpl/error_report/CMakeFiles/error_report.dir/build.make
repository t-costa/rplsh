# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
RM = /home/tommaso/Downloads/clion-2019.1/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tommaso/forked/rplsh/rpl-shell

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug

# Include any dependencies generated for this target.
include rpl/error_report/CMakeFiles/error_report.dir/depend.make

# Include the progress variables for this target.
include rpl/error_report/CMakeFiles/error_report.dir/progress.make

# Include the compile flags for this target's objects.
include rpl/error_report/CMakeFiles/error_report.dir/flags.make

rpl/error_report/CMakeFiles/error_report.dir/error_report.cpp.o: rpl/error_report/CMakeFiles/error_report.dir/flags.make
rpl/error_report/CMakeFiles/error_report.dir/error_report.cpp.o: ../rpl/error_report/error_report.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object rpl/error_report/CMakeFiles/error_report.dir/error_report.cpp.o"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/error_report && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/error_report.dir/error_report.cpp.o -c /home/tommaso/forked/rplsh/rpl-shell/rpl/error_report/error_report.cpp

rpl/error_report/CMakeFiles/error_report.dir/error_report.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/error_report.dir/error_report.cpp.i"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/error_report && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/rpl-shell/rpl/error_report/error_report.cpp > CMakeFiles/error_report.dir/error_report.cpp.i

rpl/error_report/CMakeFiles/error_report.dir/error_report.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/error_report.dir/error_report.cpp.s"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/error_report && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/rpl-shell/rpl/error_report/error_report.cpp -o CMakeFiles/error_report.dir/error_report.cpp.s

# Object files for target error_report
error_report_OBJECTS = \
"CMakeFiles/error_report.dir/error_report.cpp.o"

# External object files for target error_report
error_report_EXTERNAL_OBJECTS =

rpl/error_report/liberror_report.a: rpl/error_report/CMakeFiles/error_report.dir/error_report.cpp.o
rpl/error_report/liberror_report.a: rpl/error_report/CMakeFiles/error_report.dir/build.make
rpl/error_report/liberror_report.a: rpl/error_report/CMakeFiles/error_report.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library liberror_report.a"
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/error_report && $(CMAKE_COMMAND) -P CMakeFiles/error_report.dir/cmake_clean_target.cmake
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/error_report && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/error_report.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
rpl/error_report/CMakeFiles/error_report.dir/build: rpl/error_report/liberror_report.a

.PHONY : rpl/error_report/CMakeFiles/error_report.dir/build

rpl/error_report/CMakeFiles/error_report.dir/clean:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/error_report && $(CMAKE_COMMAND) -P CMakeFiles/error_report.dir/cmake_clean.cmake
.PHONY : rpl/error_report/CMakeFiles/error_report.dir/clean

rpl/error_report/CMakeFiles/error_report.dir/depend:
	cd /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tommaso/forked/rplsh/rpl-shell /home/tommaso/forked/rplsh/rpl-shell/rpl/error_report /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/error_report /home/tommaso/forked/rplsh/rpl-shell/cmake-build-debug/rpl/error_report/CMakeFiles/error_report.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : rpl/error_report/CMakeFiles/error_report.dir/depend

