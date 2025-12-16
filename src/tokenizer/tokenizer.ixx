module;
#include <cctype>
#include <concepts>
#include <cstddef>
#include <format>
#include <iterator>
#include <optional>
#include <string>
#include <utility>

export module tokenizer;
import position;
import token;
import diagnostics;
import grammar;

namespace tc {

template <typename P, typename Symbol, typename Output>
concept Parser_c = requires(P p, Symbol s) {
    { p.step(s) } -> std::convertible_to<std::optional<Output>>;
    { p.try_step(s) } -> std::convertible_to<bool>;
    { p.reset() };
};

export template <std::forward_iterator Iter, Parser_c<char, Token_type> Parser>
class Tokenizer {
   public:
    Tokenizer() = delete;
    Tokenizer(Iter begin, Iter end, Parser parser) : current(std::move(begin)), end(std::move(end)), parser(parser){};

    Position position();
    std::optional<Token> next_token();

   private:
    Iter current, end;

    std::string buffer{};

    Parser parser;

    Position cursor{};

    [[nodiscard]] bool ended() const noexcept { return current == end; }

    char eat() noexcept {
        char c = *current;
        current++;

        cursor.offset++;
        if (c == '\n') {  // if end of line
            cursor.line++;
            cursor.column = 0;
        } else {
            cursor.column++;
        }

        return c;
    }

    void discard_sequence(std::predicate<char> auto&& func) {
        while (not ended()) {
            if (not std::invoke(func, *current)) {
                break;
            }
            eat();
        }
    }
};

template <std::forward_iterator Iter, Parser_c<char, Token_type> Parser>
std::optional<Token> Tokenizer<Iter, Parser>::next_token() {
    buffer.clear();
    discard_sequence(std::isspace);  // skip whitespaces
    Position token_start = cursor;

    if (ended()) {
        return std::nullopt;
    }

    struct {
        size_t size{};
        Token_type type{Token_type::INVALID};
    } matched_token;

    while (ended()) {
        const char c = eat();
        buffer += c;

        if (not parser.try_step(c)) {
            break;
        }

        if (auto type = parser.step(c)) {
            matched_token.size = std::size(buffer);
            matched_token.type = *type;
        }
    }

    if (matched_token.type == Token_type::INVALID or std::size(buffer) != matched_token.size) {
        throw Parser_exception(std::format("Unexpected symbol '{}' at {}", buffer[std::size(buffer) - 1], token_start),
                               token_start);
    }

    if (matched_token.type == Token_type::IDENTIFIER) {
        matched_token.type = check_for_keyword(buffer);
    }

    return create_token(matched_token.type, buffer, token_start);
};

// static_assert(concepts::I_tokenizer<Tokenizer>);

}  // namespace tc
