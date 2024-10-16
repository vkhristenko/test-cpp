tcpp_print("must add Clagn package - seems like no conan package...")
return()

include(${LLVM_CMAKE_DIR}/AddLLVM.cmake)
include(${CLANG_CMAKE_DIR}/AddClang.cmake)

file(GLOB _srcs CONFIGURE_DEPENDS *.cpp)
foreach(_src ${_srcs})
    tcpp_sse(
        SRC ${_src}
        DEPS
            core
            clangAST
            clangASTMatchers
            clangBasic
            clangFrontend
            clangSerialization
            clangTooling
        TARGET_VAR
            _target
    )
    target_compile_defintions(${_target} PUBLIC ${LLVM_DEFINTIONS})
    target_compile_options(${_target} PUBLIC -fno-rtti)
    target_include_directories(${_target}
        PUBLIC
            ${LLVM_CMAKE_DIR}
            ${CLANG_CMAKE_DIR}
    )
    target_link_directories(${_target}
        PUBLIC
            ${LLVM_LIBRARY_DIR}
            ${CLANG_CMAKE_DIR}/../..
    )
endforeach()
