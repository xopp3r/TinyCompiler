module;

export module operation_categorires;
import token;

namespace tc {

export enum class Op_category : unsigned char {
    INVALID,
    ARIHMETIC,
    LOGIC,
    ORDERING,
    DEREFERENCE,
    ASSIGNMENT,
    CAST,
    size
};

export Op_category op_category_mapping(Token_type t) {
    switch (t) {
        case Token_type::OP_PLUS:
        case Token_type::OP_MINUS:
        case Token_type::OP_MUL:
        case Token_type::OP_DIV:
        case Token_type::OP_MOD:
            return Op_category::ARIHMETIC;
        case Token_type::OP_EQUAL:
        case Token_type::OP_GREATER:
        case Token_type::OP_GREATER_EQ:
        case Token_type::OP_LESS:
        case Token_type::OP_LESS_EQ:
        case Token_type::OP_NOT_EQUAL:
            return Op_category::ORDERING;
        case Token_type::OP_AND:
        case Token_type::OP_OR:
        case Token_type::OP_NOT:
            return Op_category::LOGIC;
        case Token_type::OP_ASSIGNMENT:
            return Op_category::ASSIGNMENT;
        case Token_type::OP_ADRESS:
            return Op_category::ASSIGNMENT;
        case Token_type::OP_DEREFERENCE:
            return Op_category::DEREFERENCE;
        case Token_type::KEYWORD_AS:
            return Op_category::CAST;
        default:
            return Op_category::INVALID;
    }
}

}  // namespace tc