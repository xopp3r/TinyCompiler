module;

#include <cstddef>
#include <format>
#include <functional>
#include <optional>
export module type_checker;
import type_system;
import ast;
import exceptions;
import operation_categorires;

namespace tc {
using Type = Expression_type;
using Category = Expression_category;
using Func_ref = std::reference_wrapper<Function_definition>;

export class Type_checker final : public I_ast_visitor {
   public:
    void visit(Binary_operation& node) override {
        node.left_value->accept(*this);
        node.right_value->accept(*this);
        node.metadata = {Type::INVALID, Category::INVALID};

        switch (op_category_mapping(node.operation.type)) {
            case Op_category::ARIHMETIC: {
                const auto ctype = common_type(node.left_value->metadata.type, node.right_value->metadata.type);
                add_implicit_conversion(node.left_value, ctype);
                add_implicit_conversion(node.right_value, ctype);
                node.metadata = {ctype, Category::RVALUE};
                break;
            }
            case Op_category::LOGIC: {
                add_implicit_conversion(node.left_value, Type::BOOL);
                add_implicit_conversion(node.right_value, Type::BOOL);
                node.metadata = {Type::BOOL, Category::RVALUE};
                break;
            }
            case Op_category::ORDERING: {
                const auto ctype = common_type(node.left_value->metadata.type, node.right_value->metadata.type);
                add_implicit_conversion(node.left_value, ctype);
                add_implicit_conversion(node.right_value, ctype);
                node.metadata = {(ctype != Type::INVALID) ? Type::BOOL : Type::INVALID, Category::RVALUE};
                break;
            }
            case Op_category::ASSIGNMENT: {
                if (node.left_value->metadata.category != Category::LVALUE)
                    throw Type_exception(std::format("lhs of assignment {} must be lvalue", node.operation),
                                         node.operation.position);
                const auto ctype = node.left_value->metadata.type;
                if (convertible_to(node.right_value->metadata.type, ctype)) {
                    add_implicit_conversion(node.right_value, ctype);
                    node.metadata = {ctype, Category::RVALUE};
                }
                break;
            }
            default:
                throw Type_exception(std::format("invalid binary operator {}", node.operation),
                                     node.operation.position);
        }

        if (node.metadata.type == Type::INVALID or node.metadata.category == Category::INVALID)
            throw Type_exception(std::format("Type deduction of {} failed", node.operation), node.operation.position);
    }

    void visit(Unary_operation& node) override {
        node.value->accept(*this);
        node.metadata = {Type::INVALID, Category::INVALID};

        switch (op_category_mapping(node.operation.type)) {
            case Op_category::ARIHMETIC:
                node.metadata = {node.value->metadata.type, Category::RVALUE};
                break;
            case Op_category::LOGIC:
                add_implicit_conversion(node.value, Type::BOOL);
                node.metadata = {node.value->metadata.type, Category::RVALUE};
                break;
            default:
                throw Type_exception(std::format("invalid binary operator {}", node.operation),
                                     node.operation.position);
        }

        if (node.metadata.type == Type::INVALID or node.metadata.category == Category::INVALID)
            throw Type_exception(std::format("Type deduction of {} failed", node.operation), node.operation.position);
    }

    void visit(Function_call& node) override {
        node.function_address->accept(*this);
        for (auto&& arg : node.arguments) arg->accept(*this);

        if (node.function_address->metadata.type != Type::PTR)
            throw Type_exception(std::format("trying to call a value of non-pointer type as function {}",
                                             node.function_address->metadata.type),
                                 current_function->get().return_type.position);

        std::optional<Func_ref> function;
        {  // I'm soooo sry...
            struct S : I_ast_visitor {
                std::optional<Func_ref>& t;
                S(std::optional<Func_ref>& t) : t(t) {}
                void visit(Binary_operation&) override {};
                void visit(Unary_operation&) override {};
                void visit(Type_operation&) override {};
                void visit(Function_call&) override {};
                void visit(Integer_literal&) override {};
                void visit(String_literal&) override {};
                void visit(Char_literal&) override {};
                void visit(Variable& node) override { t = node.source->get().if_function; };
                void visit(Expression_statement&) override {};
                void visit(Variable_declaration_statement&) override {};
                void visit(If_statement&) override {};
                void visit(While_statement&) override {};
                void visit(Return_statement&) override {};
                void visit(Function_definition&) override {};
                void visit(Programm&) override {};
            } driller{function};
            node.function_address->accept(driller);
        }

        if (not function.has_value()) {
            node.metadata = {Type::INT, Category::RVALUE};
            return;
        }

        const Function_definition& f = function->get();
        node.metadata = {type_mapping(f.return_type.type), Category::RVALUE};

        if (node.arguments.size() != f.arguments.size())
            throw Type_exception(
                std::format("wrong number of arguments to call '{}' function", f.var.name.content_str()),
                current_function->get().return_type.position);

        for (size_t i = 0; i < f.arguments.size(); i++) {
            const auto expected = type_mapping(f.arguments[i]->type.type);
            const auto got = node.arguments[i]->metadata.type;
            if (convertible_to(got, expected)) {
                add_implicit_conversion(node.arguments[i], expected);
            } else
                throw Type_exception(
                    std::format("wrong type of {}th argument to call '{}' function", i, f.var.name.content_str()),
                    current_function->get().return_type.position);
        }
    }

    void visit(Type_operation& node) override {
        node.value->accept(*this);
        const auto t = type_mapping(node.type.type);

        switch (op_category_mapping(node.operation.type)) {
            case Op_category::CAST:
                if (not convertible_to(node.value->metadata.type, t))
                    throw Type_exception(
                        std::format("no conversion from {} to {}", node.value->metadata.type, node.type),
                        node.operation.position);

                add_implicit_conversion(node.value, t);
                node.metadata = {t, Category::LVALUE};
                break;
            case Op_category::DEREFERENCE:
                if (node.value->metadata.type != Type::PTR)
                    throw Type_exception(std::format("Can not dereference non-pointer value"), node.operation.position);

                node.metadata = {t, Category::RVALUE};
                break;
            default:
                throw Type_exception(std::format("invalid type operator {}", node.operation), node.operation.position);
        }
    }

    void visit(Integer_literal& node) override { node.metadata.type = Type::INT; }
    void visit(String_literal& node) override { node.metadata.type = Type::PTR; }
    void visit(Char_literal& node) override { node.metadata.type = Type::CHAR; }
    void visit(Variable& node) override { node.metadata.type = type_mapping(node.source->get().type.type); }

    void visit(Expression_statement&) override {}
    void visit(If_statement&) override {}
    void visit(While_statement&) override {}
    void visit(Variable_declaration_statement&) override {}

    void visit(Return_statement& node) override {
        node.expression->accept(*this);
        const auto expected = type_mapping(current_function->get().return_type.type);
        if (convertible_to(node.expression->metadata.type, expected)) {
            add_implicit_conversion(node.expression, expected);
        } else
            throw Type_exception(std::format("type of return expression '{}' doesn't match those of function: '{}'",
                                             node.expression->metadata.type, expected),
                                 current_function->get().return_type.position);
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
    std::optional<Func_ref> current_function;
};

}  // namespace tc
