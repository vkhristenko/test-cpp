file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS bext--utils++utils TARGET_VAR _target)
endforeach()
