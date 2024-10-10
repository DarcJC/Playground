cmake_minimum_required(VERSION 3.28)

project(Playground C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Building library and executable in same directory to avoid "dll not found" problem in Windows
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/Binaries")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/Binaries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Binaries")

# CMake modules
include(CMakeDependentOption)
include(GenerateExportHeader)
include(CMakeParseArguments)

# Loading thirdparty modules
add_subdirectory(thirdparty)

# Add playground projects
add_subdirectory(slang_test)
