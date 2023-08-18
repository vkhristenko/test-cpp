tcpp_print("skipping for now TODO")
return()

file(GLOB _hdrs CONFIGURE_DEPENDS *.h)
list(REMOVE_ITEM _hdrs ${CMAKE_CURRENT_SOURCE_DIR}/socket.h)

tcpp_target_name(_target)
add_library(${_target} INTERFACE ${_hdrs})
target_link_libraries(${_target} INTERFACE core)

add_subdirectory(unittests)
