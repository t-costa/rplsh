# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/tommaso/forked/rplsh/business/dotproduct

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tommaso/forked/rplsh/business/dotproduct/build

# Include any dependencies generated for this target.
include CMakeFiles/dotproduct.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/dotproduct.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dotproduct.dir/flags.make

CMakeFiles/dotproduct.dir/ff_code.cpp.o: CMakeFiles/dotproduct.dir/flags.make
CMakeFiles/dotproduct.dir/ff_code.cpp.o: ../ff_code.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tommaso/forked/rplsh/business/dotproduct/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/dotproduct.dir/ff_code.cpp.o"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/dotproduct.dir/ff_code.cpp.o -c /home/tommaso/forked/rplsh/business/dotproduct/ff_code.cpp

CMakeFiles/dotproduct.dir/ff_code.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dotproduct.dir/ff_code.cpp.i"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tommaso/forked/rplsh/business/dotproduct/ff_code.cpp > CMakeFiles/dotproduct.dir/ff_code.cpp.i

CMakeFiles/dotproduct.dir/ff_code.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dotproduct.dir/ff_code.cpp.s"
	/usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tommaso/forked/rplsh/business/dotproduct/ff_code.cpp -o CMakeFiles/dotproduct.dir/ff_code.cpp.s

# Object files for target dotproduct
dotproduct_OBJECTS = \
"CMakeFiles/dotproduct.dir/ff_code.cpp.o"

# External object files for target dotproduct
dotproduct_EXTERNAL_OBJECTS =

dotproduct: CMakeFiles/dotproduct.dir/ff_code.cpp.o
dotproduct: CMakeFiles/dotproduct.dir/build.make
dotproduct: CMakeFiles/dotproduct.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tommaso/forked/rplsh/business/dotproduct/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable dotproduct"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dotproduct.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dotproduct.dir/build: dotproduct

.PHONY : CMakeFiles/dotproduct.dir/build

CMakeFiles/dotproduct.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dotproduct.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dotproduct.dir/clean

CMakeFiles/dotproduct.dir/depend:
	cd /home/tommaso/forked/rplsh/business/dotproduct/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tommaso/forked/rplsh/business/dotproduct /home/tommaso/forked/rplsh/business/dotproduct /home/tommaso/forked/rplsh/business/dotproduct/build /home/tommaso/forked/rplsh/business/dotproduct/build /home/tommaso/forked/rplsh/business/dotproduct/build/CMakeFiles/dotproduct.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dotproduct.dir/depend

