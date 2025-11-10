module;
#include <array>
#include <cstddef>
#include <optional>
#include <string>

export module token;
import position;

export enum class TokenType {
    TOKEN_INVALID = 0, // ERROR
    TOKEN_END, // end of file
    TOKEN_IDENTIFIER, // user-defined namse (variable / function name)
    TOKEN_KEYWORD_BREAK,
    TOKEN_KEYWORD_CONTINUE,
    TOKEN_KEYWORD_ELSE,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_WHILE,
    TOKEN_KEYWORD_RETURN,
    TOKEN_KEYWORD_EXTERN,
    TOKEN_KEYWORD_TYPE, // type (int/char/...)
    TOKEN_KEYWORD_FUNCTION,
    TOKEN_INTEGER, // number literal
    TOKEN_CHAR, // char literal
    TOKEN_STRING, // string literal
    TOKEN_SEMICOLON, // ;
    TOKEN_COLON, // :
    TOKEN_BRACE_OPEN, // {
    TOKEN_BRACE_CLOSE, // }
    TOKEN_SQUARE_BRACE_OPEN, // [
    TOKEN_SQUARE_BRACE_CLOSE, // ]
    TOKEN_PARENTHESES_OPEN, // (
    TOKEN_PARENTHESES_CLOSE, // )
    TOKEN_COMMA, // ,
    TOKEN_OP_PLUS, // +
    TOKEN_OP_MINUS, // -
    TOKEN_OP_MUL, // *
    TOKEN_OP_DIV, // /
    TOKEN_OP_MOD, // %
    TOKEN_OP_EQUAL, // ==
    TOKEN_OP_GREATER, // >
    TOKEN_OP_GREATER_EQ, // >=
    TOKEN_OP_LESS, // <
    TOKEN_OP_LESS_EQ, // <=
    TOKEN_OP_NOT_EQUAL, // !=
    TOKEN_OP_AND, // &&
    TOKEN_OP_OR, // ||
    TOKEN_OP_NOT, // !
    TOKEN_OP_ASSIGNMENT,  // =
    TOKEN_OP_ADRESS, // &
    TOKEN_OP_DEREFERENCE, // @
    TOKEN_PLACEHOLDER // system internal token
};


static std::array tokenNames = {
    "TOKEN_INVALID",
    "TOKEN_END",
    "TOKEN_IDENTIFIER",
    "TOKEN_KEYWORD_BREAK",
    "TOKEN_KEYWORD_CONTINUE",
    "TOKEN_KEYWORD_ELSE",
    "TOKEN_KEYWORD_IF",
    "TOKEN_KEYWORD_WHILE",
    "TOKEN_KEYWORD_RETURN",
    "TOKEN_KEYWORD_EXTERN",
    "TOKEN_KEYWORD_TYPE",
    "TOKEN_KEYWORD_FUNCTION",
    "TOKEN_INTEGER",
    "TOKEN_CHAR",
    "TOKEN_STRING",
    "TOKEN_SEMICOLON",
    "TOKEN_COLON",
    "TOKEN_BRACE_OPEN",
    "TOKEN_BRACE_CLOSE",
    "TOKEN_SQUARE_BRACE_OPEN",
    "TOKEN_SQUARE_BRACE_CLOSE",
    "TOKEN_PARENTHESES_OPEN",
    "TOKEN_PARENTHESES_CLOSE",
    "TOKEN_COMMA",
    "TOKEN_OP_PLUS",
    "TOKEN_OP_MINUS",
    "TOKEN_OP_MUL",
    "TOKEN_OP_DIV",
    "TOKEN_OP_MOD",
    "TOKEN_OP_EQUAL",
    "TOKEN_OP_GREATER",
    "TOKEN_OP_GREATER_EQ",
    "TOKEN_OP_LESS",
    "TOKEN_OP_LESS_EQ",
    "TOKEN_OP_NOT_EQUAL",
    "TOKEN_OP_AND",
    "TOKEN_OP_OR",
    "TOKEN_OP_NOT",
    "TOKEN_OP_ASSIGNMENT",
    "TOKEN_OP_ADRESS",
    "TOKEN_OP_DEREFERENCE",
    "TOKEN_PLACEHOLDER"
};



export {

    struct Token {
        std::optional<std::string> lexeme{std::nullopt}; // text of token
        Position position{};
        TokenType type{};
    };
    
    std::string_view tokenName(TokenType tokType) noexcept {
        return tokenNames[static_cast<size_t>(tokType)];
    };

}
