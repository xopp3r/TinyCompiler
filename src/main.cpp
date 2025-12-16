import tokenizer;
import parsing_dfa;


#include <print>

auto main() -> int { 
    const char text[] = R"(  int abc; func(); retrun abc * 2 - 1;  )"; 
    tc::Tokenizer tokenizer{std::begin(text), std::end(text), tc::Parsing_dfa{}};

    while (auto tok = tokenizer.next_token()) {
        std::println("{}", tok.value());
    }
}
