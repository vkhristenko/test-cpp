file(GLOB _srcs CONFIGURE_DEPENDS *.c)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS core TARGET_VAR _target)
endforeach()
