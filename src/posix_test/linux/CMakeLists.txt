if (UNIX AND APPLE)
    tcpp_print("only for linux")
    return()
endif()

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS fmt::fmt core++core TARGET_VAR _target)
endforeach()
