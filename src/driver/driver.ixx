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

import ast;


namespace tc {


export AST build_ast(std::ranges::forward_range auto text) {
    Tokenizer tokenizer{std::move(text), Parsing_dfa{}};
    Parser parser{[&tokenizer = tokenizer](){ return tokenizer.next_token(); }};
    return parser.build_AST();
}

export void annotate_ast(AST& ast) {
    
}

export void validate_ast(AST& ast) {
    
}

export void compile(std::ranges::forward_range auto text) {
    AST ast;
    try {
        ast = build_ast(text);    
    } catch (Parser_exception& e) {
        std::println("Parsing error: {}", e.what());
        std::exit(EXIT_FAILURE);
    }


}

}