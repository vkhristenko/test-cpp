tcpp_module_name(_module)

tcpp_target_form(_target_test_0 ${_module} test_0)
add_executable(${_target_test_0} test_0.cpp $<TARGET_OBJECTS:sanitizers++pc_guard_callback>)
target_link_libraries(${_target_test_0} PRIVATE
    sanitizers++address_sanitizer
)

tcpp_target_form(_target_test_1 ${_module} test_1)
add_executable(${_target_test_1} test_1.cpp)
target_compile_options(${_target_test_1} PRIVATE -fprofile-instr-generate -fcoverage-mapping)
target_link_options(${_target_test_1} PRIVATE -fprofile-instr-generate -fcoverage-mapping)
