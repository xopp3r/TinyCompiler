module;
#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
#include <functional>

export module parsing_dfa:grammar;
import token;
import position;

namespace tc {

export enum class Parsing_state : unsigned char {
    INVALID = 0,
    INITIAL,
    STRING_LITERAL,
    STRING_LITERAL_escaped,
    STRING_LITERAL_end,
    CHAR_LITERAL_escaped,
    CHAR_LITERAL,
    CHAR_LITERAL_end,
    NUMBER_LITERAL,
    INDENTIFIER,

    SEMICOLON,           // ;
    COLON,               // :
    BRACE_OPEN,          // {
    BRACE_CLOSE,         // }
    SQUARE_BRACE_OPEN,   // [
    SQUARE_BRACE_CLOSE,  // ]
    PARENTHESES_OPEN,    // (
    PARENTHESES_CLOSE,   // )
    COMMA,               // ,
    DOT,                 // .
    PLUS,                // +
    MINUS,               // -
    ASTERISK,            // *
    SLASH,               // /
    PERCENT,             // %
    GREATER,             // >
    LESS,                // <
    EXCLAMATION,         // !
    EQUALS,              // =
    AMPERSAND,           // &
    PIPE,                // |
    AT,                  // @
    DOUBLE_AMPERSAND,
    DOUBLE_PIPE,
    DOUBLE_EQUALS,
    LESS_EQ,
    GREATER_EQ,
    size
};

export enum class Symbol_type : unsigned char {
    INVALID = 0,         // ERROR
    LETTER_OR_,          // [a-z] | [A-Z] | _
    NUMBER,              // [0-9]
    SEMICOLON,           // ;
    COLON,               // :
    BRACE_OPEN,          // {
    BRACE_CLOSE,         // }
    SQUARE_BRACE_OPEN,   // [
    SQUARE_BRACE_CLOSE,  // ]
    PARENTHESES_OPEN,    // (
    PARENTHESES_CLOSE,   // )
    COMMA,               // ,
    DOT,                 // ,
    PLUS,                // +
    MINUS,               // -
    ASTERISK,            // *
    SLASH,               // /
    PERCENT,             // %
    GREATER,             // >
    LESS,                // <
    EXCLAMATION,         // !
    EQUALS,              // =
    AMPERSAND,           // &
    PIPE,                // |
    AT,                  // @
    SINGLE_QUOTE,        // '
    DOUBLE_QUOTE,        // "
    BACKSLASH,           // \ .
    size
};

constexpr size_t operator+(Symbol_type s) { return static_cast<size_t>(s); }
constexpr size_t operator+(Parsing_state s) { return static_cast<size_t>(s); }

template <std::ranges::random_access_range Range, typename T, typename Proj>
constexpr auto binary_search_ranges(Range&& range, const T& value, Proj&& proj = {}) {
    const auto it = std::ranges::lower_bound(range, value, {}, proj);
    return (it != std::ranges::end(range) && std::invoke(proj, *it) == value) ? it : std::ranges::end(range);
}

export constexpr auto transition_function = [](Parsing_state state, Symbol_type sym) noexcept {
    constinit static const auto translation_table = []() {
        constexpr size_t parsing_states_size = static_cast<size_t>(Parsing_state::size);
        constexpr size_t symbol_type_size = static_cast<size_t>(Symbol_type::size);
        std::array<std::array<Parsing_state, symbol_type_size>, parsing_states_size> table{};

        // chars and strings
        table[+Parsing_state::INITIAL][+Symbol_type::SINGLE_QUOTE] = Parsing_state::CHAR_LITERAL;
        for (size_t i = 0; i < +Symbol_type::size; i++) {
            table[+Parsing_state::CHAR_LITERAL][i] = Parsing_state::CHAR_LITERAL;
            table[+Parsing_state::CHAR_LITERAL_escaped][i] = Parsing_state::CHAR_LITERAL;
        }
        table[+Parsing_state::CHAR_LITERAL][+Symbol_type::BACKSLASH] = Parsing_state::CHAR_LITERAL_escaped;
        table[+Parsing_state::CHAR_LITERAL][+Symbol_type::SINGLE_QUOTE] = Parsing_state::CHAR_LITERAL_end;

        table[+Parsing_state::INITIAL][+Symbol_type::DOUBLE_QUOTE] = Parsing_state::STRING_LITERAL;
        for (size_t i = 0; i < +Symbol_type::size; i++) {
            table[+Parsing_state::STRING_LITERAL][i] = Parsing_state::STRING_LITERAL;
            table[+Parsing_state::STRING_LITERAL_escaped][i] = Parsing_state::STRING_LITERAL;
        }
        table[+Parsing_state::STRING_LITERAL][+Symbol_type::BACKSLASH] = Parsing_state::STRING_LITERAL_escaped;
        table[+Parsing_state::STRING_LITERAL][+Symbol_type::DOUBLE_QUOTE] = Parsing_state::STRING_LITERAL_end;

        // identifiers
        table[+Parsing_state::INITIAL][+Symbol_type::LETTER_OR_] = Parsing_state::INDENTIFIER;
        table[+Parsing_state::INDENTIFIER][+Symbol_type::LETTER_OR_] = Parsing_state::INDENTIFIER;
        table[+Parsing_state::INDENTIFIER][+Symbol_type::NUMBER] = Parsing_state::INDENTIFIER;

        // numbers
        table[+Parsing_state::INITIAL][+Symbol_type::NUMBER] = Parsing_state::NUMBER_LITERAL;
        table[+Parsing_state::NUMBER_LITERAL][+Symbol_type::NUMBER] = Parsing_state::NUMBER_LITERAL;

        // operators
        table[+Parsing_state::INITIAL][+Symbol_type::SEMICOLON] = Parsing_state::SEMICOLON;
        table[+Parsing_state::INITIAL][+Symbol_type::COLON] = Parsing_state::COLON;
        table[+Parsing_state::INITIAL][+Symbol_type::BRACE_OPEN] = Parsing_state::BRACE_OPEN;
        table[+Parsing_state::INITIAL][+Symbol_type::BRACE_CLOSE] = Parsing_state::BRACE_CLOSE;
        table[+Parsing_state::INITIAL][+Symbol_type::SQUARE_BRACE_OPEN] = Parsing_state::SQUARE_BRACE_OPEN;
        table[+Parsing_state::INITIAL][+Symbol_type::SQUARE_BRACE_CLOSE] = Parsing_state::SQUARE_BRACE_CLOSE;
        table[+Parsing_state::INITIAL][+Symbol_type::PARENTHESES_OPEN] = Parsing_state::PARENTHESES_OPEN;
        table[+Parsing_state::INITIAL][+Symbol_type::PARENTHESES_CLOSE] = Parsing_state::PARENTHESES_CLOSE;
        table[+Parsing_state::INITIAL][+Symbol_type::COMMA] = Parsing_state::COMMA;
        table[+Parsing_state::INITIAL][+Symbol_type::DOT] = Parsing_state::DOT;
        table[+Parsing_state::INITIAL][+Symbol_type::PLUS] = Parsing_state::PLUS;
        table[+Parsing_state::INITIAL][+Symbol_type::MINUS] = Parsing_state::MINUS;
        table[+Parsing_state::INITIAL][+Symbol_type::ASTERISK] = Parsing_state::ASTERISK;
        table[+Parsing_state::INITIAL][+Symbol_type::SLASH] = Parsing_state::SLASH;
        table[+Parsing_state::INITIAL][+Symbol_type::PERCENT] = Parsing_state::PERCENT;
        table[+Parsing_state::INITIAL][+Symbol_type::GREATER] = Parsing_state::GREATER;
        table[+Parsing_state::INITIAL][+Symbol_type::LESS] = Parsing_state::LESS;
        table[+Parsing_state::INITIAL][+Symbol_type::EXCLAMATION] = Parsing_state::EXCLAMATION;
        table[+Parsing_state::INITIAL][+Symbol_type::AMPERSAND] = Parsing_state::AMPERSAND;
        table[+Parsing_state::INITIAL][+Symbol_type::PIPE] = Parsing_state::PIPE;
        table[+Parsing_state::INITIAL][+Symbol_type::AT] = Parsing_state::AT;

        // double operators
        table[+Parsing_state::AMPERSAND][+Symbol_type::AMPERSAND] = Parsing_state::DOUBLE_AMPERSAND;
        table[+Parsing_state::PIPE][+Symbol_type::PIPE] = Parsing_state::DOUBLE_PIPE;

        table[+Parsing_state::LESS][+Symbol_type::EQUALS] = Parsing_state::LESS_EQ;
        table[+Parsing_state::GREATER][+Symbol_type::EQUALS] = Parsing_state::GREATER_EQ;

        table[+Parsing_state::INITIAL][+Symbol_type::EQUALS] = Parsing_state::EQUALS;
        table[+Parsing_state::EQUALS][+Symbol_type::EQUALS] = Parsing_state::DOUBLE_EQUALS;

        return table;
    }();

    return translation_table[+state][+sym];
};

export constexpr auto map_char = [](char c) noexcept {
    constinit static const auto char_to_symbol_mapping = []() {
        std::array<Symbol_type, std::numeric_limits<unsigned char>::max() + 1> table{};

        for (auto c : u8"QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_") {
            table[c] = Symbol_type::LETTER_OR_;
        }

        for (auto c : u8"0123456789") {
            table[c] = Symbol_type::NUMBER;
        }

        table[';'] = Symbol_type::SEMICOLON;
        table[':'] = Symbol_type::COLON;
        table['{'] = Symbol_type::BRACE_OPEN;
        table['}'] = Symbol_type::BRACE_CLOSE;
        table['['] = Symbol_type::SQUARE_BRACE_OPEN;
        table[']'] = Symbol_type::SQUARE_BRACE_CLOSE;
        table['('] = Symbol_type::PARENTHESES_OPEN;
        table[')'] = Symbol_type::PARENTHESES_CLOSE;
        table[','] = Symbol_type::COMMA;
        table['.'] = Symbol_type::DOT;
        table['+'] = Symbol_type::PLUS;
        table['-'] = Symbol_type::MINUS;
        table['*'] = Symbol_type::ASTERISK;
        table['/'] = Symbol_type::SLASH;
        table['%'] = Symbol_type::PERCENT;
        table['>'] = Symbol_type::GREATER;
        table['<'] = Symbol_type::LESS;
        table['!'] = Symbol_type::EXCLAMATION;
        table['='] = Symbol_type::EQUALS;
        table['&'] = Symbol_type::AMPERSAND;
        table['|'] = Symbol_type::PIPE;
        table['@'] = Symbol_type::AT;
        table['\''] = Symbol_type::SINGLE_QUOTE;
        table['"'] = Symbol_type::DOUBLE_QUOTE;
        table['\\'] = Symbol_type::BACKSLASH;

        table['\0'] = Symbol_type::INVALID;
        return table;
    }();

    return char_to_symbol_mapping[static_cast<unsigned char>(c)];
};

export constexpr auto output_function = [](Parsing_state state) noexcept {
    constinit static const auto output_table = []() {
        std::array<Token_type, +Parsing_state::size> table{};

        table[+Parsing_state::CHAR_LITERAL_end] = Token_type::CHAR;
        table[+Parsing_state::STRING_LITERAL_end] = Token_type::STRING;

        table[+Parsing_state::INDENTIFIER] = Token_type::IDENTIFIER;
        table[+Parsing_state::NUMBER_LITERAL] = Token_type::NUMBER;

        table[+Parsing_state::SEMICOLON] = Token_type::SEMICOLON;
        table[+Parsing_state::COLON] = Token_type::COLON;
        table[+Parsing_state::BRACE_OPEN] = Token_type::BRACE_OPEN;
        table[+Parsing_state::BRACE_CLOSE] = Token_type::BRACE_CLOSE;
        table[+Parsing_state::SQUARE_BRACE_OPEN] = Token_type::SQUARE_BRACE_OPEN;
        table[+Parsing_state::SQUARE_BRACE_CLOSE] = Token_type::SQUARE_BRACE_CLOSE;
        table[+Parsing_state::PARENTHESES_OPEN] = Token_type::PARENTHESES_OPEN;
        table[+Parsing_state::PARENTHESES_CLOSE] = Token_type::PARENTHESES_CLOSE;
        table[+Parsing_state::COMMA] = Token_type::COMMA;
        table[+Parsing_state::DOT] = Token_type::DOT;
        table[+Parsing_state::PLUS] = Token_type::OP_PLUS;
        table[+Parsing_state::MINUS] = Token_type::OP_MINUS;
        table[+Parsing_state::ASTERISK] = Token_type::OP_MUL;
        table[+Parsing_state::SLASH] = Token_type::OP_DIV;
        table[+Parsing_state::PERCENT] = Token_type::OP_MOD;
        table[+Parsing_state::GREATER] = Token_type::OP_GREATER;
        table[+Parsing_state::GREATER_EQ] = Token_type::OP_GREATER_EQ;
        table[+Parsing_state::LESS] = Token_type::OP_LESS;
        table[+Parsing_state::LESS_EQ] = Token_type::OP_LESS_EQ;
        table[+Parsing_state::EXCLAMATION] = Token_type::OP_NOT;
        table[+Parsing_state::EQUALS] = Token_type::OP_ASSIGNMENT;
        table[+Parsing_state::DOUBLE_EQUALS] = Token_type::OP_EQUAL;
        table[+Parsing_state::AMPERSAND] = Token_type::OP_ADRESS;
        table[+Parsing_state::DOUBLE_AMPERSAND] = Token_type::OP_AND;
        // table[+Parsing_state::PIPE] = Token_type::;
        table[+Parsing_state::DOUBLE_PIPE] = Token_type::OP_OR;
        table[+Parsing_state::AT] = Token_type::OP_DEREFERENCE;

        return table;
    }();

    return output_table[static_cast<unsigned char>(state)];
};

export constexpr auto check_for_keyword = [](std::string_view lexeme) {
    constinit static const auto keywords_list = []() {
        using namespace std::string_view_literals;
        std::array keywords = {
            std::pair("char"sv, Token_type::TYPE_CHAR),        std::pair("int"sv, Token_type::TYPE_INT),
            std::pair("uint"sv, Token_type::TYPE_UINT),        std::pair("ptr"sv, Token_type::TYPE_PTR),
            std::pair("void"sv, Token_type::TYPE_VOID),        std::pair("if"sv, Token_type::KEYWORD_IF),
            std::pair("else"sv, Token_type::KEYWORD_ELSE),     std::pair("while"sv, Token_type::KEYWORD_WHILE),
            std::pair("break"sv, Token_type::KEYWORD_BREAK),   std::pair("fn"sv, Token_type::KEYWORD_FUNCTION),
            std::pair("extern"sv, Token_type::KEYWORD_EXTERN), std::pair("return"sv, Token_type::KEYWORD_RETURN),
            std::pair("export"sv, Token_type::KEYWORD_EXPORT), std::pair("as"sv, Token_type::KEYWORD_AS),
            std::pair("bool"sv, Token_type::TYPE_BOOL)};

        std::ranges::sort(keywords, {}, &decltype(keywords)::value_type::first);

        return keywords;
    }();

    const auto it = binary_search_ranges(keywords_list, lexeme, &decltype(keywords_list)::value_type::first);
    return it != std::ranges::end(keywords_list) ? it->second : Token_type::IDENTIFIER;
};

}  // namespace tc