find_package(fmt REQUIRED)

file(GLOB _hdrs CONFIGURE_DEPENDS *.h)

tcpp_target_name(_target)
add_library(${_target} INTERFACE ${_hdrs})
target_link_libraries(${_target} INTERFACE base_include fmt::fmt cpp20)
