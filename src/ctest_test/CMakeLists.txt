file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS base_include++base_include cpp17++cpp17 TARGET_VAR _target)
    add_test(NAME ${_target} COMMAND ${_target})
endforeach()
