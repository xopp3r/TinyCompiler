cmake_minimum_required(VERSION 3.28)

project(TinyCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_SCAN_FOR_MODULES 1)

add_executable(main)

# FIXME
target_link_options(main PRIVATE -stdlib=libc++)
target_compile_options(main PRIVATE -stdlib=libc++) 
# FIXME


target_compile_options(main PRIVATE 
    -Wall -Wextra -Wpedantic -Werror -Wredundant-decls -Wshadow -Wunused
    
    $<$<CONFIG:Debug>:
        -O0
        -g3
        -DDEBUG
        -fno-omit-frame-pointer
        # -fsanitize=address,leak,undefined
    >
    
    $<$<CONFIG:Release>:
        -O2
        -DNDEBUG
        -march=native
        -ffast-math
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
      
      src/semantics/type_checker.ixx
      src/semantics/type_system.ixx
      src/semantics/visibility_checker.ixx

      src/parser/ast.ixx
      src/parser/nodes.ixx
      src/parser/annotations.ixx
      src/parser/parser.ixx

      src/tokenizer/position.ixx
      src/tokenizer/tokenizer.ixx
      src/tokenizer/parsing_dfa.ixx
      src/tokenizer/grammar.ixx
      src/tokenizer/token.ixx
)

