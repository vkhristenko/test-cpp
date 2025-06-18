tcpp_module_name(_module)

tcpp_target_form(_target_address ${_module} address_sanitizer)
add_library(${_target_address} INTERFACE)
target_compile_options(${_target_address} INTERFACE -fsanitize=address -fsanitize-address-use-after-return=always -fsanitize=undefined -fno-omit-frame-pointer -fsanitize-coverage=trace-pc-guard)
target_link_options(${_target_address} INTERFACE -fsanitize=address -fsanitize-address-use-after-return=always -fsanitize=undefined -fsanitize-coverage=trace-pc-guard)

tcpp_target_form(_target_pc_guard_cb ${_module} pc_guard_callback)
add_library(${_target_pc_guard_cb} OBJECT trace_pc_guard_example.cpp)
target_compile_options(${_target_pc_guard_cb} PUBLIC -fsanitize-coverage=trace-pc-guard)
target_link_options(${_target_pc_guard_cb} PUBLIC -fsanitize-coverage=trace-pc-guard)

# apple clang does not support -fsanitize=leak out of the box
if (0)
    target_compile_options(${_target} INTERFACE -fsanitize=leak)
    target_link_options(${_target} INTERFACE -fsanitize=leak)
endif()

tcpp_target_form(_target_thread ${_module} thread_sanitizer)
add_library(${_target_thread} INTERFACE)
target_compile_options(${_target_thread} INTERFACE -fsanitize=thread)
target_link_options(${_target_thread} INTERFACE -fsanitize=thread)

# apple clang does not support -fsanitize=memory out of the box
if (0)
    tcpp_target_form(_target_memory ${_module} memory_sanitizer)
    add_library(${_target_memory} INTERFACE)
    target_compile_options(${_target_memory} INTERFACE -fsanitize=memory)
    target_link_options(${_target_memory} INTERFACE -fsanitize=memory)
endif()
