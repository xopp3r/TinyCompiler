module;
#include <array>
#include <limits>

export module grammar;

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

static constexpr size_t operator+(Symbol_type s) { return static_cast<size_t>(s); }
static constexpr size_t operator+(Parsing_state s) { return static_cast<size_t>(s); }


static consteval auto create_char_symbol_mapping() {
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

    return table;
}


static consteval auto create_translation_table() {
    constexpr size_t parsing_states_size = static_cast<size_t>(Parsing_state::size);
    constexpr size_t symbol_type_size = static_cast<size_t>(Symbol_type::size);
    std::array<std::array<Parsing_state, symbol_type_size>, parsing_states_size> table{};

    // chars and strings
    table[+Parsing_state::INITIAL][+Symbol_type::SINGLE_QUOTE] = Parsing_state::CHAR_LITERAL;
    table[+Parsing_state::CHAR_LITERAL][+Symbol_type::BACKSLASH] = Parsing_state::CHAR_LITERAL_escaped;
    // ... TODO
    table[+Parsing_state::CHAR_LITERAL][+Symbol_type::SINGLE_QUOTE] = Parsing_state::CHAR_LITERAL_end;

    table[+Parsing_state::INITIAL][+Symbol_type::DOUBLE_QUOTE] = Parsing_state::STRING_LITERAL;
    table[+Parsing_state::STRING_LITERAL][+Symbol_type::BACKSLASH] = Parsing_state::STRING_LITERAL_escaped;
    // ... TODO
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
    table[+Parsing_state::INITIAL][+Symbol_type::PLUS] = Parsing_state::PLUS;
    table[+Parsing_state::INITIAL][+Symbol_type::MINUS] = Parsing_state::MINUS;
    table[+Parsing_state::INITIAL][+Symbol_type::ASTERISK] = Parsing_state::ASTERISK;
    table[+Parsing_state::INITIAL][+Symbol_type::SLASH] = Parsing_state::SLASH;
    table[+Parsing_state::INITIAL][+Symbol_type::PERCENT] = Parsing_state::PERCENT;
    table[+Parsing_state::INITIAL][+Symbol_type::GREATER] = Parsing_state::GREATER;
    table[+Parsing_state::INITIAL][+Symbol_type::LESS] = Parsing_state::LESS;
    table[+Parsing_state::INITIAL][+Symbol_type::EXCLAMATION] = Parsing_state::EXCLAMATION;
    table[+Parsing_state::INITIAL][+Symbol_type::EQUALS] = Parsing_state::EQUALS;
    table[+Parsing_state::INITIAL][+Symbol_type::AMPERSAND] = Parsing_state::AMPERSAND;
    table[+Parsing_state::INITIAL][+Symbol_type::PIPE] = Parsing_state::PIPE;
    table[+Parsing_state::INITIAL][+Symbol_type::AT] = Parsing_state::AT;

    // double operators
    table[+Parsing_state::AMPERSAND][+Symbol_type::AMPERSAND] = Parsing_state::DOUBLE_AMPERSAND;
    table[+Parsing_state::PIPE][+Symbol_type::PIPE] = Parsing_state::DOUBLE_PIPE;


    return table;
}

constinit static const auto translation_table = create_translation_table();
constinit static const auto char_to_symbol_mapping = create_char_symbol_mapping();

export constexpr Parsing_state transition_function(Parsing_state state, Symbol_type sym) {
    return translation_table[+state][+sym];
}

export constexpr Symbol_type map_char(char c) {
    return char_to_symbol_mapping[static_cast<unsigned char>(c)];
}

}
