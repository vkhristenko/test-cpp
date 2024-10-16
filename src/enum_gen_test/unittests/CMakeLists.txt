file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS enum_gen_test--enum_gen_hxx TARGET_VAR _target)
endforeach()
