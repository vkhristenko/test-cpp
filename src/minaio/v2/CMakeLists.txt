file(GLOB_RECURSE _hdrs CONFIGURE_DEPENDS *.h)
file(GLOB_RECURSE _srcs CONFIGURE_DEPENDS *.cpp *.ipp)
list(FILTER _hdrs EXCLUDE REGEX .*mocks\/.*)
list(FILTER _hdrs EXCLUDE REGEX .*unittests\/.*)
list(FILTER _srcs EXCLUDE REGEX .*mocks\/.*)
list(FILTER _srcs EXCLUDE REGEX .*unittests\/.*)

tcpp_target_name(_target)
add_library(${_target} ${_srcs} ${_hdrs})
target_link_libraries(${_target} PUBLIC core cpp20)

add_subdirectory(mocks)
add_subdirectory(unittests)
