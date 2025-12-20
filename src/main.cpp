#include <print>

import tokenizer;
import parsing_dfa;
import token;

auto main() -> int {
    const char text[] = R"(  int abc; func(); return abc * 2 - 1;  )";
    tc::Tokenizer tokenizer{std::begin(text), std::end(text), tc::Parsing_dfa{}};

    while (auto tok = tokenizer.next_token()) {
        std::println("{}", tok);
    }
}
