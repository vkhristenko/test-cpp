find_package(fmt REQUIRED)

file(GLOB _hdrs CONFIGURE_DEPENDS *.h)
list(REMOVE_ITEM _hdrs ${CMAKE_CURRENT_SOURCE_DIR}/result.h)

tcpp_target_name(_target)
add_library(${_target} INTERFACE ${_hdrs})
target_link_libraries(${_target} INTERFACE base_include fmt::fmt cpp17)

add_subdirectory(unittests)
