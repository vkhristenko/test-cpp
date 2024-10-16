tcpp_module_name(_module)
tcpp_debug_var(_module)

file(GLOB _py_srcs CONFIGURE_DEPENDS *.py)
set(_output ${CMAKE_CURRENT_BINARY_DIR}/defs_generated.h)

tcpp_target_form(_target_enum_gen ${_module} enum_gen)
tcpp_module_name_from_target(_module_tmp ${_target_enum_gen})
tcpp_debug_var(_module_tmp)
tcpp_enum_gen(
    TARGET ${_target_enum_gen}
    INPUT ${_py_srcs}
)

tcpp_target_form(_target_enum_gen_hxx  ${_module} enum_gen_hxx)
add_library(${_target_enum_gen_hxx} INTERFACE $<TARGET_PROPERTY:${_target_enum_gen},OUTPUT>)
target_link_libraries(${_target_enum_gen_hxx} INTERFACE base_include cpp20)

add_subdirectory(unittests)
