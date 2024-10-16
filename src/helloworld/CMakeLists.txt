file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
file(GLOB _hdrs CONFIGURE_DEPENDS *.h)

tcpp_target_name(_target)

add_executable(${_target} ${_srcs} ${_hdrs})
target_link_libraries(${_target} Boost::Boost fmt::fmt)

add_subdirectory(helloworld2)
