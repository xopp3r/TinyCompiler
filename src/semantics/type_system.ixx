module;
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

export module type_system;
import ast;
import token;

export namespace tc {
using Type = Expression_type;
using Metadata = Expression_metadata;

unsigned char size_of_type(Expression_type t) {
    switch (t) {
        case Type::INT:
        case Type::UINT:
        case Type::PTR:
            return 4;
        case Type::CHAR:
        case Type::BOOL:
            return 1;
        default:
            return 0;
    }
};

bool is_signed(Type t) { return t != Type::UINT; }

// Type deduce_type(Expression_metadata& expr) {}

void add_implicit_conversion(std::unique_ptr<Expression>& expr, Type type) {
    if (expr->metadata.type == type) return;
    auto tmp = std::move(expr);
    expr = std::make_unique<Type_operation>(Token{{}, {}, Token_type::KEYWORD_AS}, std::move(tmp),
                                            Token{{}, {}, type_mapping_reverse(type)});
}

bool convertible_to(Type from, Type to) {
    if (from == to) return true;
    if (is_signed(from) ^ is_signed(to)) return false;
    if (from == Type::PTR ^ from == Type::PTR) return false;
    if (from == Type::CHAR and to == Type::INT) return true;
    if (from == Type::BOOL and to == Type::CHAR) return true;
    if (from == Type::BOOL and to == Type::INT) return true;
    throw std::logic_error("unexpected state 1");
}

Type common_type(Type t1, Type t2) {
    if (t1 == Type::INVALID or t2 == Type::INVALID) return Type::INVALID;
    if (t1 == Type::VOID or t2 == Type::VOID) return Type::INVALID;
    if (t1 == t2) return t1;
    if (is_signed(t1) ^ is_signed(t2)) return Type::INVALID;
    if (t1 == Type::PTR or t2 == Type::PTR) return Type::PTR;
    if (t1 == Type::INT or t2 == Type::INT) return Type::INT;
    if (t1 == Type::BOOL or t2 == Type::BOOL) return Type::BOOL;
    throw std::logic_error("unexpected state 2");
}

}  // namespace tc
