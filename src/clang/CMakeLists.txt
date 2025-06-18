tcpp_target_name(_target)

find_package(Clang REQUIRED)
include(${LLVM_CMAKE_DIR}/AddLLVM.cmake)
include(${CLANG_CMAKE_DIR}/AddClang.cmake)

llvm_map_components_to_libnames(LLVM_LIBS
  AArch64
  AArch64AsmParser
  AArch64CodeGen
  AArch64Desc
  AArch64Disassembler
  AArch64Info
  AArch64Utils
  MC
  MCParser
  Support
  Target
)

add_library(${_target} INTERFACE)
target_link_libraries(
        ${_target} 
    INTERFACE
        clangAST clangASTMatchers clangBasic clangFrontend clangSerialization clangTooling 
        # TODO separaet llvm and clang
        ${LLVM_LIBS}
        LLVMCore
        LLVMSupport
        LLVMIRReader
        LLVMTargetParser
)

target_compile_options(${_target} INTERFACE -fno-rtti)
target_compile_definitions(${_target} INTERFACE ${LLVM_DEFINITIONS})
target_include_directories(${_target} INTERFACE 
    ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS}
)
target_link_directories(${_target} INTERFACE ${LLVM_LIBRARY_DIRS} ${CLANG_CMAKE_DIR}/../..)
