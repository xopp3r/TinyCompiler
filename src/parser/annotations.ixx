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

export enum class Expression_type : unsigned char { INVALID, UINT, INT, CHAR, PTR, VOID, size };

export constexpr Expression_type type_mapping(Token_type t) noexcept {
    switch (t) {
        case Token_type::TYPE_UINT: return Expression_type::UINT;
        case Token_type::TYPE_INT: return Expression_type::INT;
        case Token_type::TYPE_CHAR: return Expression_type::CHAR;
        case Token_type::TYPE_PTR: return Expression_type::PTR;
        case Token_type::TYPE_VOID: return Expression_type::VOID;
        default: return Expression_type::INVALID;
    }
}


static constinit const std::array<std::string_view, static_cast<size_t>(Expression_type::size)> typename_mappings = {
    "INVALID",
    "uint",
    "int",
    "char",
    "ptr",
    "void"
};  // namespace tc

constexpr const std::string_view type_name(Expression_type t) {
    return typename_mappings[static_cast<size_t>(t)];
} 

}

template <>
struct std::formatter<tc::Expression_type> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    constexpr auto format(const tc::Expression_type &e, auto &ctx) const {
        return std::format_to(ctx.out(), "{}", type_name(e));
    }
};
