file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS cpp23++cpp23 core++core TARGET_VAR _target)
endforeach()
