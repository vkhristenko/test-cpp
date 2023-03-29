find_package(protobuf REQUIRED)
find_package(fmt REQUIRED)

tcpp_target_name(_target)
tcpp_sse(SRC main.cpp DEPS protobuf::protobuf core fmt::fmt)
