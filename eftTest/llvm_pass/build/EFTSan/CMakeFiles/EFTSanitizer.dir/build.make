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


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /fpcc/eftTest/copy_file/llvm_pass

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /fpcc/eftTest/copy_file/llvm_pass/build

# Include any dependencies generated for this target.
include EFTSan/CMakeFiles/EFTSanitizer.dir/depend.make

# Include the progress variables for this target.
include EFTSan/CMakeFiles/EFTSanitizer.dir/progress.make

# Include the compile flags for this target's objects.
include EFTSan/CMakeFiles/EFTSanitizer.dir/flags.make

EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o: EFTSan/CMakeFiles/EFTSanitizer.dir/flags.make
EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o: ../EFTSan/EFTSanitizer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/fpcc/eftTest/copy_file/llvm_pass/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o"
	cd /fpcc/eftTest/copy_file/llvm_pass/build/EFTSan && /usr/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o -c /fpcc/eftTest/copy_file/llvm_pass/EFTSan/EFTSanitizer.cpp

EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.i"
	cd /fpcc/eftTest/copy_file/llvm_pass/build/EFTSan && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /fpcc/eftTest/copy_file/llvm_pass/EFTSan/EFTSanitizer.cpp > CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.i

EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.s"
	cd /fpcc/eftTest/copy_file/llvm_pass/build/EFTSan && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /fpcc/eftTest/copy_file/llvm_pass/EFTSan/EFTSanitizer.cpp -o CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.s

EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o.requires:

.PHONY : EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o.requires

EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o.provides: EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o.requires
	$(MAKE) -f EFTSan/CMakeFiles/EFTSanitizer.dir/build.make EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o.provides.build
.PHONY : EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o.provides

EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o.provides.build: EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o


# Object files for target EFTSanitizer
EFTSanitizer_OBJECTS = \
"CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o"

# External object files for target EFTSanitizer
EFTSanitizer_EXTERNAL_OBJECTS =

EFTSan/libEFTSanitizer.so: EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o
EFTSan/libEFTSanitizer.so: EFTSan/CMakeFiles/EFTSanitizer.dir/build.make
EFTSan/libEFTSanitizer.so: EFTSan/CMakeFiles/EFTSanitizer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/fpcc/eftTest/copy_file/llvm_pass/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module libEFTSanitizer.so"
	cd /fpcc/eftTest/copy_file/llvm_pass/build/EFTSan && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/EFTSanitizer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
EFTSan/CMakeFiles/EFTSanitizer.dir/build: EFTSan/libEFTSanitizer.so

.PHONY : EFTSan/CMakeFiles/EFTSanitizer.dir/build

EFTSan/CMakeFiles/EFTSanitizer.dir/requires: EFTSan/CMakeFiles/EFTSanitizer.dir/EFTSanitizer.cpp.o.requires

.PHONY : EFTSan/CMakeFiles/EFTSanitizer.dir/requires

EFTSan/CMakeFiles/EFTSanitizer.dir/clean:
	cd /fpcc/eftTest/copy_file/llvm_pass/build/EFTSan && $(CMAKE_COMMAND) -P CMakeFiles/EFTSanitizer.dir/cmake_clean.cmake
.PHONY : EFTSan/CMakeFiles/EFTSanitizer.dir/clean

EFTSan/CMakeFiles/EFTSanitizer.dir/depend:
	cd /fpcc/eftTest/copy_file/llvm_pass/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /fpcc/eftTest/copy_file/llvm_pass /fpcc/eftTest/copy_file/llvm_pass/EFTSan /fpcc/eftTest/copy_file/llvm_pass/build /fpcc/eftTest/copy_file/llvm_pass/build/EFTSan /fpcc/eftTest/copy_file/llvm_pass/build/EFTSan/CMakeFiles/EFTSanitizer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : EFTSan/CMakeFiles/EFTSanitizer.dir/depend

