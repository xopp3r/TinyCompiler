module;
#include <sys/types.h>

#include <cstdlib>
#include <exception>
#include <print>
#include <ranges>

export module driver;
import tokenizer;
import parsing_dfa;
import parser;
import exceptions;
import type_checker;
import visibility_checker;
import ast_printer;


import ast;

namespace tc {

AST build_ast(std::ranges::input_range auto text) {
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
}

export void compile(std::ranges::input_range auto text, bool verbose = false) {
    AST ast;

    try {  // parse source code to build ast
        ast = build_ast(text);
    } catch (Parser_exception& e) {
        if (e.where()) {
            const auto pos = e.where().value();
            std::println("Parsing error at line {}, col {}:\n {}", pos.line + 1, pos.column + 1, e.what());
        } else {
            std::println("Parsing error:\n {}", e.what());
        }
        std::exit(EXIT_FAILURE);
    }

    try {  // transformations over ast
        validate_and_annotate_ast(ast);
    } catch (Visibility_exception& e) {
        if (e.where()) {
            const auto pos = e.where().value();
            std::println("Visibility error at line {}, col {}:\n {}", pos.line + 1, pos.column + 1, e.what());
        } else {
            std::println("Visibility error:\n {}", e.what());
        }
        std::exit(EXIT_FAILURE);
    } catch (Type_exception& e) {
        if (e.where()) {
            const auto pos = e.where().value();
            std::println("Type error at line {}, col {}:\n {}", pos.line + 1, pos.column + 1, e.what());
        } else {
            std::println("Type error:\n {}", e.what());
        }
        std::exit(EXIT_FAILURE);
    }

    try {
        if (verbose) {
            AST_printer p;
            ast.root->accept(p);
        }
    } catch (std::exception& e) {
        std::println("Ast dump fail: {}", e.what());
        std::exit(EXIT_FAILURE);
    }
}

}  // namespace tc