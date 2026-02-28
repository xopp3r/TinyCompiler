module;
#include <cstddef>
#include <format>
#include <string_view>

export module ast:annotations;
import token;

namespace tc {

export enum class Linkage_type : unsigned char { INVALID, NONE, EXPORT, EXTERN, size };

export constexpr Linkage_type linkage_mapping(Token_type t) noexcept {
    switch (t) {
        case Token_type::KEYWORD_EXTERN:
            return Linkage_type::EXTERN;
        case Token_type::KEYWORD_EXPORT:
            return Linkage_type::EXPORT;
        default:
            return Linkage_type::INVALID;
    }
}

export enum class Expression_type : unsigned char { INVALID, UINT, INT, CHAR, PTR, VOID, BOOL, size };

export constexpr size_t operator+(Expression_type e) { return static_cast<size_t>(e); }

export constexpr Expression_type type_mapping(Token_type t) noexcept {
    switch (t) {
        case Token_type::TYPE_UINT:
            return Expression_type::UINT;
        case Token_type::TYPE_INT:
            return Expression_type::INT;
        case Token_type::TYPE_CHAR:
            return Expression_type::CHAR;
        case Token_type::TYPE_PTR:
            return Expression_type::PTR;
        case Token_type::TYPE_VOID:
            return Expression_type::VOID;
        case Token_type::TYPE_BOOL:
            return Expression_type::BOOL;
        default:
            return Expression_type::INVALID;
    }
}

export constexpr Token_type type_mapping_reverse(Expression_type t) noexcept {
    switch (t) {
        case Expression_type::UINT:
            return Token_type::TYPE_UINT;
        case Expression_type::INT:
            return Token_type::TYPE_INT;
        case Expression_type::CHAR:
            return Token_type::TYPE_CHAR;
        case Expression_type::PTR:
            return Token_type::TYPE_PTR;
        case Expression_type::VOID:
            return Token_type::TYPE_VOID;
        case Expression_type::BOOL:
            return Token_type::TYPE_BOOL;
        default:
            return Token_type::INVALID;
    }
}

export enum class Expression_category : unsigned char { INVALID, LVALUE, RVALUE, size };

export struct Expression_metadata {
    Expression_type type{};
    Expression_category category{};
};

constexpr const std::string_view type_name(Expression_type t) {
    static constinit const std::array<std::string_view, static_cast<size_t>(Expression_type::size)> typename_mappings =
        {"INVALID", "uint", "int", "char", "ptr", "void"};

    return typename_mappings[static_cast<size_t>(t)];
}

export unsigned char size_of_type(Expression_type t) {
    switch (t) {
        case Expression_type::INT:
        case Expression_type::UINT:
        case Expression_type::PTR:
            return 4;
        case Expression_type::CHAR:
        case Expression_type::BOOL:
            return 1;
        default:
            return 0;
    }
};

}  // namespace tc

template <>
struct std::formatter<tc::Expression_type> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    constexpr auto format(const tc::Expression_type& e, auto& ctx) const {
        return std::format_to(ctx.out(), "{}", type_name(e));
    }
};

