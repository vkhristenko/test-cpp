tcpp_target_name(_target)

file(GLOB _srcs CONFIGURE_DEPENDS *.py)
set(_output ${CMAKE_CURRENT_SOURCE_DIR}/headers_enum_gen.h)

tcpp_enum_gen(
    INPUT ${_srcs}
    OUTPUT ${_output}
)

add_library(${_target} INTERFACE ${_output})
target_link_libraries(${_target} INTERFACE base_include cpp20)
