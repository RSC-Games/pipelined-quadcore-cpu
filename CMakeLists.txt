# This cmake file is modified version based on the examples found in the link below.
#   - https://github.com/ttroy50/cmake-examples

# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# TODO: make this change based on the OS
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the project name
project (pipelined_qc_cpu)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    soc/src/core/memory.cpp
    soc/src/core/bootrom.cpp
    soc/src/core/mmu.cpp
    soc/src/core/core.cpp
    soc/src/util/logger.cpp
    soc/src/soc.cpp
    soc/src/main.cpp

    isa/src/common/isa_decode.cpp
)

# Add an executable with the above sources
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/soc/include ${PROJECT_SOURCE_DIR}/isa/include)

# Final executable.
target_link_libraries(${PROJECT_NAME})