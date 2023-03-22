find_package(Boost REQUIRED)
find_package(fmt REQUIRED)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
file(GLOB _hdrs CONFIGURE_DEPENDS *.h)

set(_target helloworld)

add_executable(${_target} ${_srcs} ${_hdrs})
target_link_libraries(${_target} Boost::Boost fmt::fmt)
