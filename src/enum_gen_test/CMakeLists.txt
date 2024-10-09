tcpp_target_name(_target)

file(GLOB _py_srcs CONFIGURE_DEPENDS *.py)
set(_output ${CMAKE_CURRENT_BINARY_DIR}/defs_generated.h)

tcpp_enum_gen(
    INPUT ${_py_srcs}
    OUTPUT ${_output}
)

set(_target_generated ${_target}_generated)
add_library(${_target_generated} INTERFACE ${_output})
target_link_libraries(${_target_generated} INTERFACE base_include cpp20)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
add_executable(${_target} ${_srcs} ${_output})
target_link_libraries(${_target} PUBLIC ${_target_generated})
