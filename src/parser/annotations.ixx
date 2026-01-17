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

}  // namespace tc
