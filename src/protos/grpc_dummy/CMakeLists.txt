file(GLOB _protos CONFIGURE_DEPENDS *.proto)

tcpp_protobuf_generate(
    PROTOS ${_protos}
    SRCS_VAR _srcs
    HDRS_VAR _hdrs
)

tcpp_target_name(_target)
add_library(${_target} ${_srcs} ${_hdrs})
target_link_libraries(${_target} PUBLIC gRPC::gRPC base_include++base_include)

add_subdirectory(examples)
