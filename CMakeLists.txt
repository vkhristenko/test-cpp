cmake_minimum_required(VERSION 3.20)
project (testcpp LANGUAGES C CXX)

# global settings
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libs vs static")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# use c++11 at a minimum
set(CMAKE_CXX_STANDARD 11)
if (APPLE)
    add_compile_options(-fvisibility=hidden) 
endif()

# conan setup
set(_conan_paths ${CMAKE_BINARY_DIR}/conan_paths.cmake)
if (NOT EXISTS ${_conan_paths})
    testcpp_fail_with_error("file ${_conan_paths} does not exist - rerun conan!")
endif()
include(${_conan_paths})

include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)

# TODO - auto discovery
add_subdirectory(src/helloworld)
