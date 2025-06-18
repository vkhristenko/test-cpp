tcpp_target_name(_target)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
file(GLOB _hdrs CONFIGURE_DEPENDS *.h)

add_library(${_target} ${_srcs} ${_hdrs})
target_link_libraries(
    ${_target}
    PUBLIC
        core++core
        clang++clang
)

add_subdirectory(unittests)
