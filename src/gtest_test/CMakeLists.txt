find_package(gTest REQUIRED)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)

tcpp_target_name(_target_base)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS core GTest::gtest GTest::gmock)
endforeach()
