# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_SOURCE_DIR = /home/i3d5mok3/Videos/FACIALDETECTION

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/i3d5mok3/Videos/FACIALDETECTION/build

# Include any dependencies generated for this target.
include CMakeFiles/facedetect.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/facedetect.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/facedetect.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/facedetect.dir/flags.make

CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o: CMakeFiles/facedetect.dir/flags.make
CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o: ../include/MTCNN/mtcnn_opencv.cpp
CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o: CMakeFiles/facedetect.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/i3d5mok3/Videos/FACIALDETECTION/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o -MF CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o.d -o CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o -c /home/i3d5mok3/Videos/FACIALDETECTION/include/MTCNN/mtcnn_opencv.cpp

CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/i3d5mok3/Videos/FACIALDETECTION/include/MTCNN/mtcnn_opencv.cpp > CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.i

CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/i3d5mok3/Videos/FACIALDETECTION/include/MTCNN/mtcnn_opencv.cpp -o CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.s

CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o: CMakeFiles/facedetect.dir/flags.make
CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o: ../include/MTCNN/mtcnn.cpp
CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o: CMakeFiles/facedetect.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/i3d5mok3/Videos/FACIALDETECTION/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o -MF CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o.d -o CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o -c /home/i3d5mok3/Videos/FACIALDETECTION/include/MTCNN/mtcnn.cpp

CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/i3d5mok3/Videos/FACIALDETECTION/include/MTCNN/mtcnn.cpp > CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.i

CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/i3d5mok3/Videos/FACIALDETECTION/include/MTCNN/mtcnn.cpp -o CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.s

# Object files for target facedetect
facedetect_OBJECTS = \
"CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o" \
"CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o"

# External object files for target facedetect
facedetect_EXTERNAL_OBJECTS =

libfacedetect.so: CMakeFiles/facedetect.dir/include/MTCNN/mtcnn_opencv.cpp.o
libfacedetect.so: CMakeFiles/facedetect.dir/include/MTCNN/mtcnn.cpp.o
libfacedetect.so: CMakeFiles/facedetect.dir/build.make
libfacedetect.so: CMakeFiles/facedetect.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/i3d5mok3/Videos/FACIALDETECTION/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared library libfacedetect.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/facedetect.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/facedetect.dir/build: libfacedetect.so
.PHONY : CMakeFiles/facedetect.dir/build

CMakeFiles/facedetect.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/facedetect.dir/cmake_clean.cmake
.PHONY : CMakeFiles/facedetect.dir/clean

CMakeFiles/facedetect.dir/depend:
	cd /home/i3d5mok3/Videos/FACIALDETECTION/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/i3d5mok3/Videos/FACIALDETECTION /home/i3d5mok3/Videos/FACIALDETECTION /home/i3d5mok3/Videos/FACIALDETECTION/build /home/i3d5mok3/Videos/FACIALDETECTION/build /home/i3d5mok3/Videos/FACIALDETECTION/build/CMakeFiles/facedetect.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/facedetect.dir/depend

