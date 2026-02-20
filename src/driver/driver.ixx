module;
#include <sys/types.h>

#include <cstdlib>
#include <print>
#include <ranges>

export module driver;
import tokenizer;
import parsing_dfa;
import parser;
import exceptions;
import type_checker;
import visibility_checker;

import ast;

namespace tc {

AST build_ast(std::ranges::input_range auto text) {
    Tokenizer tokenizer{text, Parsing_dfa{}};
    Parser parser{[&tokenizer = tokenizer]() { return tokenizer.next_token(); }};
    return parser.build_AST();
}

void validate_and_annotate_ast(AST& ast) {
    Visibility_checker v;
    ast.root->accept(v);
    Type_checker t;
    ast.root->accept(t);
}

export void compile(std::ranges::input_range auto text) {
    AST ast;

    try {  // parse source code to build ast
        ast = build_ast(text);
    } catch (Parser_exception& e) {
        std::println("Parsing error at {}\n: {}", e.where().value_or({}), e.what());
        std::exit(EXIT_FAILURE);
    }

    try {  // transformations over ast
        validate_and_annotate_ast(ast);
    } catch (Visibility_exception& e) {
        std::println("Visibility error at {}\n: {}", e.where().value_or({}), e.what());
        std::exit(EXIT_FAILURE);
    } catch (Type_exception& e) {
        std::println("Type error at {}\n: {}", e.where().value_or({}), e.what());
        std::exit(EXIT_FAILURE);
    }
}

}  // namespace tc