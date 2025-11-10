module;
#include <iterator>

export module tokenizer;
import I_tokenizer;
import token;
import position;

template <std::forward_iterator Iter>
class Tokenizer {
public:
    Tokenizer() = delete;
    Tokenizer(Iter begin, Iter end) : begin(begin), end(end) {};

    std::optional<Token> next_token();
    Position position();

private:
    Iter begin;
    Iter end;
};

template <std::forward_iterator Iter> 
std::optional<Token> Tokenizer<Iter>::next_token() {
    
};

// static_assert(concepts::I_tokenizer<Tokenizer>);