module;
#include <array>
#include <cctype>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

export module tokenizer;
import position;
import token;
import diagnostics;

static constexpr const unsigned char letters[] =
    R"alphabet(AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz_0123456789(){}[]:;,+-*/%<=>&|!@)alphabet";
static constexpr std::array alphabet = std::to_array(letters);

template <std::forward_iterator Iter>
class Tokenizer {
   public:
    Tokenizer() = delete;
    Tokenizer(Iter begin, Iter end)
        : current(std::move(begin)),
          end(std::move(end)){

          };

    Position position();
    std::optional<Token> next_token();

   private:
    Iter current;
    Iter end;

    std::string buffer{};

    Position cursor{};
    Position token_start{};

    bool ended() { return current == end; }

    char eat() {
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

    std::string eat_sequence(std::predicate<char> auto&& func) {
        std::string tmp{};

        while (not ended()) {
            if (not std::invoke(func, *current)) {
                break;
            }
            tmp += eat();
        }

        return tmp;
    }

    std::optional<std::string> eat_string_literal() {
        if (*current != '"') return std::nullopt;
        eat();
        auto string_literal = eat_sequence([escape = false](char c) mutable {
            bool ret = c != '"' or escape;
            escape = c == '\\';
            return ret;
        });
        eat();
        return string_literal.substr(1, std::size(string_literal) - 1);
    }

    std::optional<char> eat_char_literal() {
        if (*current != '\'') return std::nullopt;
        eat();
        char c = eat();
        eat();
        return c;
    }
};

template <std::forward_iterator Iter>
std::optional<Token> Tokenizer<Iter>::next_token() {
    buffer.clear();
    eat_sequence(std::isspace);  // skip whitespaces
    token_start = cursor;

    if (auto l = eat_string_literal()) {
        return Token{.lexeme = std::move(*l), .position = token_start, .type = Token_type::STRING};
    }

    if (auto l = eat_char_literal()) {
        return Token{.lexeme = {*l}, .position = token_start, .type = Token_type::CHAR};
    }

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

        if (auto t = trie.find(buffer)) {
            matched_token.type = t;
            matched_token.size = std::size(buffer);
            continue;
        }

        if (trie.contains_prefix(buffer)) {
            continue;
        }

        // if (matched_token.type == Token_type::INVALID) {
        //     error("Invalid token", token_start);
        // }
    }
};

// static_assert(concepts::I_tokenizer<Tokenizer>);