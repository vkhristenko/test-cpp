find_package(fmt REQUIRED)

file(GLOB _hdrs CONFIGURE_DEPENDS *.h)

tcpp_target_name(_target)
add_library(${_target} INTERFACE ${_hdrs})
target_link_libraries(${_target} INTERFACE src_bin_include fmt::fmt)
target_compile_features(${_target} INTERFACE cxx_std_17)

add_subdirectory(unittests)
