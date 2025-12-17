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
import parser_exception;

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
    Tokenizer(Iter begin, Iter end, Parser&& parser)
        : current(std::move(begin)), end(std::move(end)), parser(std::move(parser)){};

    Position position();
    std::optional<Token> next_token();

   private:
    Iter current, end;

    std::string buffer{};

    Parser parser;

    Position cursor{};

    [[nodiscard]] bool ended() const noexcept { return current == end || *current == '\0'; }

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

    template <std::predicate<char> Func>
    void discard_sequence(Func&& func) {
        while (not ended()) {
            if (not std::invoke(std::forward<Func>(func), *current)) {
                break;
            }
            eat();
        }
    }
};

template <std::forward_iterator Iter, Parser_c<char, Token_type> Parser>
std::optional<Token> Tokenizer<Iter, Parser>::next_token() {
    buffer.clear();
    parser.reset();

    discard_sequence([](const char c) -> bool { return std::isspace(c); });
    Position token_start = cursor;

    if (ended()) {
        return std::nullopt;
    }

    struct {
        size_t size;
        Token_type type;
    } matched_token = {0, Token_type::INVALID};

    while (not ended()) {
        const char c = *current;

        if (not parser.try_step(c)) {
            break;
        }

        if (auto type = parser.step(c)) {
            buffer += eat();
            matched_token.size = std::size(buffer);
            matched_token.type = *type;
        }
    }

    if (matched_token.type == Token_type::INVALID or std::size(buffer) != matched_token.size) {
        throw Parser_exception(std::format("Unexpected symbol '{}' at {}", buffer[std::size(buffer) - 1], token_start),
                               token_start);
    }

    if (matched_token.type == Token_type::IDENTIFIER) {
        matched_token.type = parser.check_if_keyword(buffer);
    }

    return create_token(matched_token.type, buffer, token_start);
};

}  // namespace tc
