tcpp_module_name(_module)

tcpp_target_form(_target_test0 ${_module} test_0)
add_executable(${_target_test0} test_0.cpp)
target_link_libraries(${_target_test0} PUBLIC modules_test--lib++lib)
