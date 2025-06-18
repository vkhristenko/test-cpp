file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(
        SRC ${_src}
        DEPS
            clang_test++clang_test
    )
endforeach()
