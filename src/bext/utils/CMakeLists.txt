find_package(Boost REQUIRED)
find_package(fmt REQUIRED)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
file(GLOB _hdrs CONFIGURE_DEPENDS *.h)
tcpp_target_name(_target)
add_library(${_target} ${_srcs} ${_hdrs})
target_link_libraries(${_target} PUBLIC cpp20 base_include Boost::Boost fmt::fmt)
