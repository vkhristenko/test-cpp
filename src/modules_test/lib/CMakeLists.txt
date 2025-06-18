tcpp_target_name(_target)
add_library(${_target} OBJECT math.ixx)
target_link_libraries(${_target} PUBLIC base_include++base_include cpp20++cpp20)
set_source_files_properties(math.ixx PROPERTIES LANGUAGE CXX)
