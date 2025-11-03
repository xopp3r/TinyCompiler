cmake_minimum_required(VERSION 3.28)

project(TinyCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_SCAN_FOR_MODULES 1)


# set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "...")
# set(CMAKE_CXX_MODULE_STD 1)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -pedantic-errors -Werror -Wconversion -Wlogical-op -Wredundant-decls -Wshadow -Wunused -fsanitize=address,undefined,leak")

add_executable(main)

# FIXME
# target_include_directories(main PRIVATE /usr/lib/llvm-22/include/c++/v1)
target_compile_options(main PRIVATE -stdlib=libc++) 
target_link_options(main PRIVATE -stdlib=libc++)
# FIXME

target_sources(main 
  PRIVATE
    src/main.cpp
)

target_sources(main
  PRIVATE
    FILE_SET modules TYPE CXX_MODULES
    FILES
      src/my_module.cppm
)
