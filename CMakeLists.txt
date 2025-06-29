cmake_minimum_required(VERSION 3.26)
project (testcpp LANGUAGES C CXX)

# global settings
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libs vs static")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# use c++11 at a minimum
set(CMAKE_CXX_STANDARD 11)

# NOTE: cmake must have a CTestTEstfile.cmkae generated 
# at the root of the build folder... otherwise it does not find tests
include(CTest)
include(${CMAKE_SOURCE_DIR}/cmake-tools/common.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-tools/target.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-tools/protobuf.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-tools/py.cmake)
include(${CMAKE_SOURCE_DIR}/cmake-tools/enum_gen.cmake)

# conan setup
set(_conan_paths ${CMAKE_BINARY_DIR}/conan_paths.cmake)
if (NOT EXISTS ${_conan_paths})
    tcpp_fail_with_error("file ${_conan_paths} does not exist - rerun conan!")
endif()
include(${_conan_paths})

find_package(Boost REQUIRED)
#find_package(Clang REQUIRED)
find_package(fmt REQUIRED)
find_package(gTest REQUIRED)
find_package(pybind11 REQUIRED)
find_package(protobuf REQUIRED)
find_package(gRPC REQUIRED)

function(tcpp_add_unit_test)
    cmake_parse_arguments(
        tcpp_add_unit_test
        ""
        ""
        "DEPS"
        ${ARGN}
    )
    tcpp_fail_if_undefined(tcpp_add_unit_test_DEPS)
    set(_deps ${tcpp_add_unit_test_DEPS})

    file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
    tcpp_target_name(_target)
    add_executable(${_target} ${_srcs})
    target_link_libraries(
        ${_target}
        PUBLIC
            ${_deps}
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
    )
endfunction()

tcpp_auto_addsubdirs(src)
