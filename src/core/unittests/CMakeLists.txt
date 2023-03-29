find_package(gTest REQUIRED)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
list(REMOVE_ITEM _srcs ${CMAKE_CURRENT_SOURCE_DIR}/test_result.cpp)

tcpp_target_name(_target_base)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS core GTest::gtest)
endforeach()
