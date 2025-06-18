file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS base_include++base_include cpp23++cpp23 sanitizers++thread_sanitizer TARGET_VAR _target)
endforeach()
