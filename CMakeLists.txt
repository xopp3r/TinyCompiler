cmake_minimum_required(VERSION 3.28)

project(TinyCompiler LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_SCAN_FOR_MODULES 1)


find_package(zstd CONFIG REQUIRED)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_executable(main)

target_compile_options(main PRIVATE 
    -Wall -Wextra -Wpedantic -Werror -Wredundant-decls -Wunused
    
    $<$<CONFIG:Debug>:
        -O0
        -g3
        -DDEBUG
        -fno-omit-frame-pointer
        -fno-inline
    >
    
    $<$<CONFIG:Release>:
        -O2
        -DNDEBUG
        -march=native
    >
) 

target_sources(main 
  PRIVATE
    src/main.cpp
)

target_sources(main
  PRIVATE
    FILE_SET modules TYPE CXX_MODULES
    FILES
      libs/finite_automata/DFA.ixx

      src/interfaces/I_ast_visitor.ixx
      src/exceptions/exceptions.ixx

      src/driver/driver.ixx

      src/utils/ast_printer.ixx
      src/utils/unescape.ixx
      
      src/semantics/type_checker.ixx
      src/semantics/type_system.ixx
      src/semantics/operation_categorires.ixx
      src/semantics/visibility_checker.ixx
      src/semantics/cfg_checker.ixx

      src/parser/ast.ixx
      src/parser/nodes.ixx
      src/parser/annotations.ixx
      src/parser/parser.ixx

      src/tokenizer/position.ixx
      src/tokenizer/tokenizer.ixx
      src/tokenizer/parsing_dfa.ixx
      src/tokenizer/grammar.ixx
      src/tokenizer/token.ixx

      src/codegen/codegen.ixx
)

llvm_map_components_to_libnames(llvm_libs support core codegen irreader)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(main SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(main ${llvm_libs})
