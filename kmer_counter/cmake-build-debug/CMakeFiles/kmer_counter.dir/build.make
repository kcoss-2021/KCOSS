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
CMAKE_SOURCE_DIR = /tmp/tmp.umc0nrqPSJ

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.umc0nrqPSJ/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/kmer_counter.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/kmer_counter.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/kmer_counter.dir/flags.make

CMakeFiles/kmer_counter.dir/Counter.cpp.o: CMakeFiles/kmer_counter.dir/flags.make
CMakeFiles/kmer_counter.dir/Counter.cpp.o: ../Counter.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.umc0nrqPSJ/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/kmer_counter.dir/Counter.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kmer_counter.dir/Counter.cpp.o -c /tmp/tmp.umc0nrqPSJ/Counter.cpp

CMakeFiles/kmer_counter.dir/Counter.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kmer_counter.dir/Counter.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.umc0nrqPSJ/Counter.cpp > CMakeFiles/kmer_counter.dir/Counter.cpp.i

CMakeFiles/kmer_counter.dir/Counter.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kmer_counter.dir/Counter.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.umc0nrqPSJ/Counter.cpp -o CMakeFiles/kmer_counter.dir/Counter.cpp.s

CMakeFiles/kmer_counter.dir/count_15to32.cpp.o: CMakeFiles/kmer_counter.dir/flags.make
CMakeFiles/kmer_counter.dir/count_15to32.cpp.o: ../count_15to32.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.umc0nrqPSJ/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/kmer_counter.dir/count_15to32.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kmer_counter.dir/count_15to32.cpp.o -c /tmp/tmp.umc0nrqPSJ/count_15to32.cpp

CMakeFiles/kmer_counter.dir/count_15to32.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kmer_counter.dir/count_15to32.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.umc0nrqPSJ/count_15to32.cpp > CMakeFiles/kmer_counter.dir/count_15to32.cpp.i

CMakeFiles/kmer_counter.dir/count_15to32.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kmer_counter.dir/count_15to32.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.umc0nrqPSJ/count_15to32.cpp -o CMakeFiles/kmer_counter.dir/count_15to32.cpp.s

CMakeFiles/kmer_counter.dir/count_33to64.cpp.o: CMakeFiles/kmer_counter.dir/flags.make
CMakeFiles/kmer_counter.dir/count_33to64.cpp.o: ../count_33to64.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.umc0nrqPSJ/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/kmer_counter.dir/count_33to64.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kmer_counter.dir/count_33to64.cpp.o -c /tmp/tmp.umc0nrqPSJ/count_33to64.cpp

CMakeFiles/kmer_counter.dir/count_33to64.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kmer_counter.dir/count_33to64.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.umc0nrqPSJ/count_33to64.cpp > CMakeFiles/kmer_counter.dir/count_33to64.cpp.i

CMakeFiles/kmer_counter.dir/count_33to64.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kmer_counter.dir/count_33to64.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.umc0nrqPSJ/count_33to64.cpp -o CMakeFiles/kmer_counter.dir/count_33to64.cpp.s

CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.o: CMakeFiles/kmer_counter.dir/flags.make
CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.o: ../counter_less_than_14.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.umc0nrqPSJ/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.o -c /tmp/tmp.umc0nrqPSJ/counter_less_than_14.cpp

CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.umc0nrqPSJ/counter_less_than_14.cpp > CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.i

CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.umc0nrqPSJ/counter_less_than_14.cpp -o CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.s

CMakeFiles/kmer_counter.dir/main.cpp.o: CMakeFiles/kmer_counter.dir/flags.make
CMakeFiles/kmer_counter.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.umc0nrqPSJ/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/kmer_counter.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kmer_counter.dir/main.cpp.o -c /tmp/tmp.umc0nrqPSJ/main.cpp

CMakeFiles/kmer_counter.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kmer_counter.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.umc0nrqPSJ/main.cpp > CMakeFiles/kmer_counter.dir/main.cpp.i

CMakeFiles/kmer_counter.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kmer_counter.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.umc0nrqPSJ/main.cpp -o CMakeFiles/kmer_counter.dir/main.cpp.s

# Object files for target kmer_counter
kmer_counter_OBJECTS = \
"CMakeFiles/kmer_counter.dir/Counter.cpp.o" \
"CMakeFiles/kmer_counter.dir/count_15to32.cpp.o" \
"CMakeFiles/kmer_counter.dir/count_33to64.cpp.o" \
"CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.o" \
"CMakeFiles/kmer_counter.dir/main.cpp.o"

# External object files for target kmer_counter
kmer_counter_EXTERNAL_OBJECTS =

kmer_counter: CMakeFiles/kmer_counter.dir/Counter.cpp.o
kmer_counter: CMakeFiles/kmer_counter.dir/count_15to32.cpp.o
kmer_counter: CMakeFiles/kmer_counter.dir/count_33to64.cpp.o
kmer_counter: CMakeFiles/kmer_counter.dir/counter_less_than_14.cpp.o
kmer_counter: CMakeFiles/kmer_counter.dir/main.cpp.o
kmer_counter: CMakeFiles/kmer_counter.dir/build.make
kmer_counter: CMakeFiles/kmer_counter.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.umc0nrqPSJ/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable kmer_counter"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kmer_counter.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/kmer_counter.dir/build: kmer_counter

.PHONY : CMakeFiles/kmer_counter.dir/build

CMakeFiles/kmer_counter.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/kmer_counter.dir/cmake_clean.cmake
.PHONY : CMakeFiles/kmer_counter.dir/clean

CMakeFiles/kmer_counter.dir/depend:
	cd /tmp/tmp.umc0nrqPSJ/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.umc0nrqPSJ /tmp/tmp.umc0nrqPSJ /tmp/tmp.umc0nrqPSJ/cmake-build-debug /tmp/tmp.umc0nrqPSJ/cmake-build-debug /tmp/tmp.umc0nrqPSJ/cmake-build-debug/CMakeFiles/kmer_counter.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/kmer_counter.dir/depend

