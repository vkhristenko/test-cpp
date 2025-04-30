file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS core++core core_cpp20++core_cpp20 TARGET_VAR _target)
endforeach()
