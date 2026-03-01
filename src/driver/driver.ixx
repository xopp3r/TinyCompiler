module;
#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>
#include <print>
#include <ranges>
#include <string_view>

export module driver;
import tokenizer;
import parsing_dfa;
import parser;
import exceptions;
import type_checker;
import visibility_checker;
import cfg_checker;
import ast_printer;
import codegen;


import ast;

namespace tc {

AST build_ast(std::istream& in) {
    auto text = std::ranges::subrange(std::istreambuf_iterator<char>(in), std::default_sentinel);
    Tokenizer tokenizer{text, Parsing_dfa{}};
    Parser parser{[&tokenizer = tokenizer]() { return tokenizer.next_token(); }};
    AST ast = parser.build_AST();
    return ast;
}

void validate_and_annotate_ast(AST& ast) {
    Visibility_checker v;
    ast.root->accept(v);
    Type_checker t;
    ast.root->accept(t);
    CFG_checker c;
    ast.root->accept(c);
}

void generate_llvm_ir(const AST& ast, std::string_view module_name, std::ostream& out) {
    Codegenerator codegen{module_name, out};
    codegen.generate(ast);
}

export void compile(std::istream& in = std::cin, std::ostream& out = std::cout, std::string_view name = "out.ll", bool dump_ast = false) {
    try {
        AST ast;

        try {  // parse source code to build ast

            ast = build_ast(in);

        } catch (Parser_exception& e) {
            if (e.where()) {
                const auto pos = e.where().value();
                std::println(stderr, "Parsing error at line {}, col {}:\n {}", pos.line + 1, pos.column + 1, e.what());
            } else {
                std::println(stderr, "Parsing error:\n {}", e.what());
            }
            std::exit(EXIT_FAILURE);
        }

        try {  // transformations over ast

            validate_and_annotate_ast(ast);

        } catch (Visibility_exception& e) {
            if (e.where()) {
                const auto pos = e.where().value();
                std::println(stderr, "Visibility error at line {}, col {}:\n {}", pos.line + 1, pos.column + 1, e.what());
            } else {
                std::println(stderr, "Visibility error:\n {}", e.what());
            }
            std::exit(EXIT_FAILURE);
        } catch (Type_exception& e) {
            if (e.where()) {
                const auto pos = e.where().value();
                std::println(stderr, "Type error at line {}, col {}:\n {}", pos.line + 1, pos.column + 1, e.what());
            } else {
                std::println(stderr, "Type error:\n {}", e.what());
            }
            std::exit(EXIT_FAILURE);
        } catch (CFG_exception& e) {
            if (e.where()) {
                const auto pos = e.where().value();
                std::println(stderr, "CFG error at line {}, col {}:\n {}", pos.line + 1, pos.column + 1, e.what());
            } else {
                std::println(stderr, "CFG error:\n {}", e.what());
            }
            std::exit(EXIT_FAILURE);
        }

        if (dump_ast) {
            try {
                AST_printer p{out};
                ast.root->accept(p);
            } catch (std::exception& e) {
                std::println(stderr, "Ast dump fail: {}", e.what());
                std::exit(EXIT_FAILURE);
            }
        } else {
            try { // codegen            

                generate_llvm_ir(ast, name, out);

            } catch (Codegen_exception& e) {
                if (e.where()) {
                    const auto pos = e.where().value();
                    std::println(stderr, "CFG error at line {}, col {}:\n {}", pos.line + 1, pos.column + 1, e.what());
                } else {
                    std::println(stderr, "CFG error:\n {}", e.what());
                }
                std::exit(EXIT_FAILURE);
            }
        }

    } catch (std::exception& e) {
        std::println(stderr, "Internal error: {}", e.what());
        std::exit(EXIT_FAILURE);
    }
}

}  // namespace tc