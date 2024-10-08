tcpp_target_name(_target)

add_executable(${_target} IMPORTED GLOBAL)
set_target_properties(${_target} PROPERTIES IMPORTED_LOCATION python3)

get_target_property(_prop ${_target} IMPORTED_LOCATION)
tcpp_debug_var(_prop)
