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

}  // namespace tc
