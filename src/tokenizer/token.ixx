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

static constexpr size_t operator+(Token_type s) { return static_cast<size_t>(s); }

static constinit const std::array<std::pair<std::string_view, std::string_view>, +Token_type::size> token_mappings = {
    std::pair("INVALID", "INVALID"),
    std::pair("IDENTIFIER", "<IDENTIFIER>"),
    std::pair("KEYWORD_BREAK", "break"),
    std::pair("KEYWORD_CONTINUE", "continue"),
    std::pair("KEYWORD_ELSE", "else"),
    std::pair("KEYWORD_IF", "if"),
    std::pair("KEYWORD_WHILE", "while"),
    std::pair("KEYWORD_RETURN", "return"),
    std::pair("KEYWORD_EXTERN", "extern"),
    std::pair("KEYWORD_EXPORT", "export"),
    std::pair("KEYWORD_AS", "as"),
    std::pair("KEYWORD_FUNCTION", "function"),
    std::pair("TYPE_INT", "int"),
    std::pair("TYPE_UINT", "uint"),
    std::pair("TYPE_CHAR", "char"),
    std::pair("TYPE_VOID", "void"),
    std::pair("TYPE_PTR", "ptr"),
    std::pair("NUMBER", "<NUMBER>"),
    std::pair("CHAR", "<CHAR>"),
    std::pair("STRING", "<STRING>"),
    std::pair("SEMICOLON", ";"),
    std::pair("COLON", ":"),
    std::pair("BRACE_OPEN", "{"),
    std::pair("BRACE_CLOSE", "}"),
    std::pair("SQUARE_BRACE_OPEN", "["),
    std::pair("SQUARE_BRACE_CLOSE", "]"),
    std::pair("PARENTHESES_OPEN", "("),
    std::pair("PARENTHESES_CLOSE", ")"),
    std::pair("COMMA", ","),
    std::pair("OP_PLUS", "+"),
    std::pair("OP_MINUS", "-"),
    std::pair("OP_MUL", "*"),
    std::pair("OP_DIV", "/"),
    std::pair("OP_MOD", "%"),
    std::pair("OP_EQUAL", "=="),
    std::pair("OP_GREATER", ">"),
    std::pair("OP_GREATER_EQ", ">="),
    std::pair("OP_LESS", "<"),
    std::pair("OP_LESS_EQ", "<="),
    std::pair("OP_NOT_EQUAL", "!="),
    std::pair("OP_AND", "&&"),
    std::pair("OP_OR", "||"),
    std::pair("OP_NOT", "!"),
    std::pair("OP_ASSIGNMENT", "="),
    std::pair("OP_ADRESS", "&"),
    std::pair("OP_DEREFERENCE", "@"),
    std::pair("PLACEHOLDER", ""),
    std::pair("END", "")};

export constexpr std::string_view type_str(Token_type type) noexcept { return token_mappings[+type].first; }

export struct Token {
    std::optional<std::string> lexeme;  // text of token
    Position position;
    Token_type type{};

    constexpr std::string_view type_str() const noexcept { return tc::type_str(type); }
    constexpr std::string_view content_str() const noexcept {
        return lexeme ? std::string_view{*lexeme} : token_mappings[+type].second;
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
            return Token{std::string{lexeme.subview(1, std::size(lexeme) - 1)}, pos, type};
        default:
            return Token{std::nullopt, pos, type};
    }
}

}  // namespace tc

template <>
struct std::formatter<tc::Token> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    constexpr auto format(const tc::Token &tok, auto &ctx) const {
        return std::format_to(ctx.out(), R"(["{}" {}@{}])", tok.content_str(), tok.type_str(), tok.position);
    }
};
