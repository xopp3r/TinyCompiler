module;
#include <array>
#include <cstddef>
#include <format>
#include <memory>
#include <utility>

export module type_system;
import ast;
import token;
import exceptions;


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

bool convertible_to(Type from, Type to) {
    constinit const static auto table = [] {
        std::array<std::array<bool, +Type::size>, +Type::size> table{};
        
        for (size_t i = 0; i < +Type::size; i++) {
            table[i][i] = true;
        }

        table[+Type::CHAR][+Type::INT] = true;
        table[+Type::BOOL][+Type::INT] = true;
        table[+Type::BOOL][+Type::CHAR] = true;
        table[+Type::BOOL][+Type::UINT] = true;

        table[+Type::INT][+Type::BOOL] = true;
        table[+Type::INT][+Type::UINT] = true;
        table[+Type::UINT][+Type::BOOL] = true;
        table[+Type::UINT][+Type::INT] = true;
        table[+Type::PTR][+Type::BOOL] = true;
        table[+Type::CHAR][+Type::BOOL] = true;

        return table;
    } ();
    
    return table[+from][+to];
}

Type common_type(Type t1, Type t2) {
    constinit const static auto table = [] {
        std::array<std::array<Type, +Type::size>, +Type::size> table{};
        
        for (size_t i = 0; i < +Type::size; i++) {
            table[i][i] = static_cast<Type>(i);
        }

        table[+Type::INT][+Type::BOOL] = Type::INT;
        table[+Type::INT][+Type::CHAR] = Type::INT;
        table[+Type::UINT][+Type::BOOL] = Type::UINT;
        table[+Type::CHAR][+Type::BOOL] = Type::CHAR;
        table[+Type::PTR][+Type::INT] = Type::PTR;
        table[+Type::PTR][+Type::UINT] = Type::PTR;
        table[+Type::PTR][+Type::CHAR] = Type::PTR;
        table[+Type::PTR][+Type::BOOL] = Type::PTR;

        // transposd
        table[+Type::BOOL][+Type::INT] = Type::INT;
        table[+Type::CHAR][+Type::INT] = Type::INT;
        table[+Type::BOOL][+Type::UINT] = Type::UINT;
        table[+Type::BOOL][+Type::CHAR] = Type::CHAR;
        table[+Type::INT][+Type::PTR] = Type::PTR;
        table[+Type::UINT][+Type::PTR] = Type::PTR;
        table[+Type::CHAR][+Type::PTR] = Type::PTR;
        table[+Type::BOOL][+Type::PTR] = Type::PTR;

        return table;
    } ();

    return table[+t1][+t2];
}

void add_implicit_conversion(std::unique_ptr<Expression>& expr, Type type) {
    if (expr->metadata.type == type) return;
    if (expr->metadata.type == Type::VOID) {
        throw Type_exception{std::format("no conversion from void to {}", type)};
    }
    auto tmp = std::move(expr);
    expr = std::make_unique<Type_operation>(Token{{}, {}, Token_type::KEYWORD_AS}, std::move(tmp),
                                            Token{{}, {}, type_mapping_reverse(type)});
    expr->metadata = {type, Expression_category::RVALUE};
}

}  // namespace tc
