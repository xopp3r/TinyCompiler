module;
#include <array>
#include <cstddef>
#include <format>
#include <optional>
#include <string>
#include <string_view>

export module token;
export import position;

namespace tc {

export enum class Token_type : unsigned char {
    INVALID = 0,  // ERROR
    IDENTIFIER,   // user-defined name of variable/function
    KEYWORD_BREAK,
    KEYWORD_CONTINUE,
    KEYWORD_ELSE,
    KEYWORD_IF,
    KEYWORD_WHILE,
    KEYWORD_RETURN,
    KEYWORD_EXTERN,
    KEYWORD_EXPORT,
    KEYWORD_AS,
    KEYWORD_FUNCTION,
    TYPE_INT,
    TYPE_UINT,
    TYPE_CHAR,
    TYPE_VOID,
    TYPE_PTR,
    TYPE_BOOL,
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
    PLACEHOLDER,         // system internal token
    END,                 // end of tokenstream
    size
};

constexpr size_t operator+(Token_type s) { return static_cast<size_t>(s); }


export constexpr std::string_view type_str(Token_type type) noexcept { 
    static constinit const std::array<std::string_view, +Token_type::size> token_mappings_name = {
        "INVALID",
        "IDENTIFIER",
        "KEYWORD_BREAK",
        "KEYWORD_CONTINUE",
        "KEYWORD_ELSE",
        "KEYWORD_IF",
        "KEYWORD_WHILE",
        "KEYWORD_RETURN",
        "KEYWORD_EXTERN",
        "KEYWORD_EXPORT",
        "KEYWORD_AS",
        "KEYWORD_FUNCTION",
        "TYPE_INT",
        "TYPE_UINT",
        "TYPE_CHAR",
        "TYPE_VOID",
        "TYPE_PTR",
        "TYPE_BOOL",
        "NUMBER",
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
        "PLACEHOLDER",
        "END"
    };

    return token_mappings_name[+type]; 
}

export struct Token {
    std::optional<std::string> lexeme;  // text of token
    Position position;
    Token_type type{};

    constexpr std::string_view type_str() const noexcept { return tc::type_str(type); }
    constexpr std::string_view content_str() const noexcept {
        static constinit const std::array<std::string_view, +Token_type::size> token_mappings_content = {
            "INVALID",
            "<IDENTIFIER>",
            "break",
            "continue",
            "else",
            "if",
            "while",
            "return",
            "extern",
            "export",
            "as",
            "function",
            "int",
            "uint",
            "char",
            "void",
            "ptr",
            "bool",
            "<NUMBER>",
            "<CHAR>",
            "<STRING>",
            ";",
            ":",
            "{",
            "}",
            "[",
            "]",
            "(",
            ")",
            ",",
            "+",
            "-",
            "*",
            "/",
            "%",
            "==",
            ">",
            ">=",
            "<",
            "<=",
            "!=",
            "&&",
            "||",
            "!",
            "=",
            "&",
            "@",
            "",
            ")"
        };

        return lexeme ? std::string_view{*lexeme} : token_mappings_content[+type];
    }
    constexpr operator bool() const noexcept { return type != Token_type::END; }
};

export constexpr Token create_token(Token_type type, std::string_view lexeme, Position pos) noexcept {
    switch (type) {
        case Token_type::IDENTIFIER:
            [[fallthrough]];
        case Token_type::NUMBER:
            return Token{std::string{lexeme}, pos, type};
        case Token_type::CHAR:
            [[fallthrough]];
        case Token_type::STRING:
            return Token{std::string{lexeme.substr(1, std::size(lexeme) - 2)}, pos, type};
        default:
            return Token{std::nullopt, pos, type};
    }
}

}  // namespace tc

template <>
struct std::formatter<tc::Token> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    constexpr auto format(const tc::Token& tok, auto& ctx) const {
        return std::format_to(ctx.out(), R"(["{}" {}@{}])", tok.content_str(), tok.type_str(), tok.position);
    }
};
