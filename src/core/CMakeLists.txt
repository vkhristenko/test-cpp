find_package(fmt REQUIRED)

file(GLOB _hdrs CONFIGURE_DEPENDS *.h)
file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)

tcpp_target_name(_target)
add_library(${_target} ${_srcs} ${_hdrs})
target_link_libraries(${_target} PUBLIC base_include fmt::fmt cpp17)

add_subdirectory(unittests)
