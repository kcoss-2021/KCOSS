# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/histo.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/histo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/histo.dir/flags.make

CMakeFiles/histo.dir/main.cpp.o: CMakeFiles/histo.dir/flags.make
CMakeFiles/histo.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/histo.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/histo.dir/main.cpp.o -c /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/main.cpp

CMakeFiles/histo.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/histo.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/main.cpp > CMakeFiles/histo.dir/main.cpp.i

CMakeFiles/histo.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/histo.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/main.cpp -o CMakeFiles/histo.dir/main.cpp.s

# Object files for target histo
histo_OBJECTS = \
"CMakeFiles/histo.dir/main.cpp.o"

# External object files for target histo
histo_EXTERNAL_OBJECTS =

histo: CMakeFiles/histo.dir/main.cpp.o
histo: CMakeFiles/histo.dir/build.make
histo: CMakeFiles/histo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable histo"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/histo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/histo.dir/build: histo

.PHONY : CMakeFiles/histo.dir/build

CMakeFiles/histo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/histo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/histo.dir/clean

CMakeFiles/histo.dir/depend:
	cd /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/cmake-build-debug /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/cmake-build-debug /home/b8402/21_xiaoweihao/Clion/histo/tmp/tmp.Ibn4MmVqbw/cmake-build-debug/CMakeFiles/histo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/histo.dir/depend

