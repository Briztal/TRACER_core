# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_COMMAND = /opt/clion-2017.2.3/bin/cmake/bin/cmake

# The command to remove a file.
RM = /opt/clion-2017.2.3/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug

# Include any dependencies generated for this target.
include src/CMakeFiles/hardware.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/hardware.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/hardware.dir/flags.make

src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o: src/CMakeFiles/hardware.dir/flags.make
src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o: ../src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o -c /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp

src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.i"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp > CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.i

src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.s"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp -o CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.s

src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.requires:

.PHONY : src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.requires

src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.provides: src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/hardware.dir/build.make src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.provides.build
.PHONY : src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.provides

src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.provides.build: src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o


# Object files for target hardware
hardware_OBJECTS = \
"CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o"

# External object files for target hardware
hardware_EXTERNAL_OBJECTS =

src/libhardware.a: src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o
src/libhardware.a: src/CMakeFiles/hardware.dir/build.make
src/libhardware.a: src/CMakeFiles/hardware.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libhardware.a"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && $(CMAKE_COMMAND) -P CMakeFiles/hardware.dir/cmake_clean_target.cmake
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hardware.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/hardware.dir/build: src/libhardware.a

.PHONY : src/CMakeFiles/hardware.dir/build

src/CMakeFiles/hardware.dir/requires: src/CMakeFiles/hardware.dir/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.requires

.PHONY : src/CMakeFiles/hardware.dir/requires

src/CMakeFiles/hardware.dir/clean:
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && $(CMAKE_COMMAND) -P CMakeFiles/hardware.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/hardware.dir/clean

src/CMakeFiles/hardware.dir/depend:
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src/CMakeFiles/hardware.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/hardware.dir/depend

