find_package(fmt REQUIRED)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS fmt::fmt core TARGET_VAR _target)
endforeach()
