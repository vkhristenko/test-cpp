file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)

tcpp_target_name(_target_base)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS protos--grpc_dummy++grpc_dummy fmt::fmt)
endforeach()
