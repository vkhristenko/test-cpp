if (UNIX AND NOT APPLE)
    return()
endif()

tcpp_target_name(_target)
add_library(${_target} MODULE dummy.cpp)
target_link_libraries(${_target} PRIVATE pybind11::module pybind11::lto)
pybind11_extension(${_target})
