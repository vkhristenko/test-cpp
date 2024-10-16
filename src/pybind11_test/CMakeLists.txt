if (UNIX AND NOT APPLE)
    return()
endif()

tcpp_target_name(_target)
pybind11_add_module(${_target} dummy.cpp)
