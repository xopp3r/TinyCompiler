module;

#include <format>
#include <functional>
export module type_checker;
import ast;
import exceptions;
import type_system;
import operation_categorires;

namespace tc {
using ET = Expression_type;

export class Type_checker final : public I_ast_visitor {
   public:
    void visit(Binary_operation& node) override {
        switch (op_category_mapping(node.operation.type)) {
            case Op_category::ARIHMETIC: {
            }
            case Op_category::LOGIC:
            case Op_category::ORDERING:
            case Op_category::DEREFERENCE:
            case Op_category::ASSIGNMENT:
            case Op_category::CAST:
            default:
                throw Type_exception(std::format("invalid binary operator {}", node.operation),
                                     node.operation.position);
        }
    }
    void visit(Unary_operation& node) override {}
    void visit(Function_call& node) override { node.type = node.function_address->type; }
    void visit(Type_operation& node) override { node.type = node.function_address->type; }
    void visit(Integer_literal& node) override { node.type = ET::INT; }
    void visit(String_literal& node) override { node.type = ET::PTR; }
    void visit(Char_literal& node) override { node.type = ET::CHAR; }
    void visit(Variable& node) override { node.type = type_mapping(node.source->get().type.type); }
    void visit(Expression_statement&) override {}
    void visit(If_statement&) override {}
    void visit(While_statement&) override {}
    void visit(Variable_declaration_statement&) override {}

    void visit(Return_statement& node) override {
        node.expression->accept(*this);
        auto expected = type_mapping(current_function.get().return_type.type);
        if (ret_val != expected) {
            throw Type_exception(
                std::format("type of return expression '{}' doesn't match those of function: '{}'", ret_val, expected),
                current_function.get().return_type.position);
        }
        node.expression->type = ret_val;
    }

    void visit(Function_definition& node) override {
        current_function = node;
        for (auto&& stmt : node.body) {
            stmt->accept(*this);
        }
    }

    void visit(Programm& node) override {
        for (auto&& f : node.functions) {
            f->accept(*this);
        }
    }

   private:
    std::reference_wrapper<Function_definition> current_function;
    ET ret_val;

    void ret(ET return_value) noexcept { ret_val = return_value; }
};

}  // namespace tc
