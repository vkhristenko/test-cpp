return()

tcpp_module_name(_module)

find_package(Clang REQUIRED)
include(${LLVM_CMAKE_DIR}/AddLLVM.cmake)
include(${CLANG_CMAKE_DIR}/AddClang.cmake)

# TODO tradeoffs vs imported target?
add_library(${_target_libtooling} INTERFACE)
target_link_libraries(
        ${_target_libtooling} 
    INTERFACE
        clangAST clangASTMatchers clangBasic clangFrontend clangSerialization clangTooling 
)
target_compile_options(${_target_libtooling} INTERFACE -fno-rtti)
target_compile_definitions(${_target_libtooling} INTEFACE ${LLVM_DEFINITIONS})
target_include_directories(${_target_libtooling} INTERFACE 
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS}
)
target_link_directories(${_target_libtooling} INTERFACE ${LLVM_LIBRARY_DIRS} ${CLANG_CMAKE_DIR}/../..)
