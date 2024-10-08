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

# NOTE: cmake must have a CTestTEstfile.cmkae generated 
# at the root of the build folder... otherwise it does not find tests
include(CTest)
include(${CMAKE_SOURCE_DIR}/cmake-tools/common.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-tools/target.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-tools/protobuf.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-tools/py.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-tools/GenerateEnum.cmake)

# conan setup
set(_conan_paths ${CMAKE_BINARY_DIR}/conan_paths.cmake)
if (NOT EXISTS ${_conan_paths})
    tcpp_fail_with_error("file ${_conan_paths} does not exist - rerun conan!")
endif()
include(${_conan_paths})

tcpp_auto_addsubdirs(src/)
