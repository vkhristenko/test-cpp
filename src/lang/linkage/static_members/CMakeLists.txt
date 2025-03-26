tcpp_module_name(_module)

tcpp_target_form(_target_hdrs ${_module} hdrs)
add_library(${_target_hdrs} INTERFACE common.h)
target_link_libraries(${_target_hdrs} INTERFACE base_include cpp20)

tcpp_target_form(_target_srcs1 ${_module} srcs1)
add_library(${_target_srcs1} SHARED src1.cpp src1.h)
target_link_libraries(${_target_srcs1} PUBLIC ${_target_hdrs})

tcpp_target_form(_target_srcs2 ${_module} srcs2)
add_library(${_target_srcs2} SHARED src2.cpp src2.h)
target_link_libraries(${_target_srcs2} PUBLIC ${_target_hdrs})

file(GLOB _srcs CONFIGURE_DEPENDS test_*.cpp)
foreach(_src ${_srcs})
    tcpp_sse(SRC ${_src} DEPS ${_target_srcs1} ${_target_srcs2} TARGET_VAR _target)
endforeach()
