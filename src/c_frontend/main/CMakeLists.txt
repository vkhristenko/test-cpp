tcpp_module_name(_module)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)

tcpp_target_form(_target ${_module} c_frontend)
add_executable(${_target} ${_srcs})
target_link_libraries(
    ${_target}
    PUBLIC
        c_frontend--lib++lib
        bext--utils++utils
)
