file(GLOB _py_srcs CONFIGURE_DEPENDS *.py)
tcpp_target_name(_target)
tcpp_add_python_executable(
    TARGET ${_target}
    INPUT ${_py_srcs}
    ENTRY_POINT enum_gen.py
)
