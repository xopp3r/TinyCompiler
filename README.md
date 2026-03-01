# Tiny compiler: A compiler for C-like Language with libc seamless Integration

A toy compiler for a C-like programming language built with C++23 and the LLVM API. It generates LLVM IR, which can be either interpreted directly or compiled into a native executable. This project was created to explore the LLVM infrastructure and gain hands‑on experience with compiler construction and modern C++.

## Overview

The compiler translates a simple, statically typed language (inspired by C) into LLVM intermediate representation. By leveraging LLVM's powerful libraries, it demonstrates key stages of compilation: lexing, parsing, semantic analysis, and code generation. A distinctive feature is the seamless ability to call standard C library functions (e.g., printf, malloc, exit, etc) from within the language.

### The generated LLVM IR can be:
  - Interpreted on the fly using lli (the LLVM interpreter).
  - Compiled to machine code using llc and a native linker (e.g., gcc) to produce a standalone executable.

### Features
  - Direct calls to any libc function (no wrappers needed), even for variadic functions
  - Static typing and type checking
  - Built with modern C++23 and CMake
  - fully capable C‑like language
