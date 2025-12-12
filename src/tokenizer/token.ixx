module;
#include <array>
#include <cstddef>
#include <optional>
#include <string>

export module token;
import position;

export enum class Token_type {
    INVALID = 0,  // ERROR
    IDENTIFIER,   // user-defined name of variable/function
    KEYWORD_BREAK,
    KEYWORD_CONTINUE,
    KEYWORD_ELSE,
    KEYWORD_IF,
    KEYWORD_WHILE,
    KEYWORD_RETURN,
    KEYWORD_EXTERN,
    KEYWORD_FUNCTION,
    TYPE_I32,
    TYPE_U32,
    TYPE_U8,
    TYPE_VOID,
    TYPE_PTR,
    NUMBER,              // number literal
    CHAR,                // char literal
    STRING,              // string literal
    SEMICOLON,           // ;
    COLON,               // :
    BRACE_OPEN,          // {
    BRACE_CLOSE,         // }
    SQUARE_BRACE_OPEN,   // [
    SQUARE_BRACE_CLOSE,  // ]
    PARENTHESES_OPEN,    // (
    PARENTHESES_CLOSE,   // )
    COMMA,               // ,
    OP_PLUS,             // +
    OP_MINUS,            // -
    OP_MUL,              // *
    OP_DIV,              // /
    OP_MOD,              // %
    OP_EQUAL,            // ==
    OP_GREATER,          // >
    OP_GREATER_EQ,       // >=
    OP_LESS,             // <
    OP_LESS_EQ,          // <=
    OP_NOT_EQUAL,        // !=
    OP_AND,              // &&
    OP_OR,               // ||
    OP_NOT,              // !
    OP_ASSIGNMENT,       // =
    OP_ADRESS,           // &
    OP_DEREFERENCE,      // @
    PLACEHOLDER          // system internal token
};

static constinit const std::array token_names = {"INVALID",
                                                 "END",
                                                 "IDENTIFIER",
                                                 "KEYWORD_BREAK",
                                                 "KEYWORD_CONTINUE",
                                                 "KEYWORD_ELSE",
                                                 "KEYWORD_IF",
                                                 "KEYWORD_WHILE",
                                                 "KEYWORD_RETURN",
                                                 "KEYWORD_EXTERN",
                                                 "KEYWORD_TYPE",
                                                 "KEYWORD_FUNCTION",
                                                 "INTEGER",
                                                 "CHAR",
                                                 "STRING",
                                                 "SEMICOLON",
                                                 "COLON",
                                                 "BRACE_OPEN",
                                                 "BRACE_CLOSE",
                                                 "SQUARE_BRACE_OPEN",
                                                 "SQUARE_BRACE_CLOSE",
                                                 "PARENTHESES_OPEN",
                                                 "PARENTHESES_CLOSE",
                                                 "COMMA",
                                                 "OP_PLUS",
                                                 "OP_MINUS",
                                                 "OP_MUL",
                                                 "OP_DIV",
                                                 "OP_MOD",
                                                 "OP_EQUAL",
                                                 "OP_GREATER",
                                                 "OP_GREATER_EQ",
                                                 "OP_LESS",
                                                 "OP_LESS_EQ",
                                                 "OP_NOT_EQUAL",
                                                 "OP_AND",
                                                 "OP_OR",
                                                 "OP_NOT",
                                                 "OP_ASSIGNMENT",
                                                 "OP_ADRESS",
                                                 "OP_DEREFERENCE",
                                                 "PLACEHOLDER"};

export struct Token {
    std::optional<std::string> lexeme;  // text of token
    Position position;
    Token_type type;

    constexpr std::string_view name(Token_type tok_type) const noexcept {
        return token_names[static_cast<size_t>(tok_type)];
    };
};
