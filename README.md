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
  - Fully capable C‑like language

### Code sample
```
extern fn int scanf(ptr fmt, ...);
extern fn int printf(ptr fmt, ...);

fn uint collatz(int n) {
    uint len; len = 0 as uint;

    while (n > 1) {
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
        len = len + 1 as uint;
    }

    return len;
}

fn uint longest_collatz(int n) {
    uint max_l; max_l = 0 as uint;
    
    uint i; i = 0 as uint;
    while (i < n as uint) {
        uint curr_l; curr_l = collatz(i);

        if (curr_l > max_l) {
            max_l = curr_l;
        }

        i = i + 1 as uint;
    }

    return max_l;
}

export fn int main() {
    int n;
    scanf("%d", &n);
    printf("longest collatz seq from 0 to %d has %u steps\n", n, longest_collatz(n));
    return 0;
}
```

### WIP

This is a prototype, it will have bugs, not all features are implemented yet (like indirect calls).
