module;


#include <format>
#include <functional>
#include <optional>
#include <ranges>
#include <string_view>
#include <vector>
export module visibility_checker;
import ast;


namespace tc {
using Var_ref = std::reference_wrapper<Variable_declaration_statement>;

class Visibility_checker final : public I_ast_visitor {
  private:
    std::optional<Var_ref> resolve_name(std::string_view name) {
        for (auto&& scope : std::ranges::reverse_view(visible_vars)) {
            for (auto&& var : scope) {
                if (var.get().name.lexeme.value() == name) {
                    return var;
                }
            }
        }
        return std::nullopt;
    }

  public:
    void visit(Binary_operation& node) override {
        node.left_value->accept(*this);
        node.right_value->accept(*this);
    }

    void visit(Unary_operation& node) override {
        node.value->accept(*this);
    }

    void visit(Function_call& node) override {
        node.function_address->accept(*this);
        for (auto&& arg : node.arguments) {
            arg->accept(*this);
        }
    }

    void visit(Integer_literal&) override {}
    void visit(String_literal&) override {}
    void visit(Char_literal&) override {}

    void visit(Variable& node) override {
        if (auto ref = resolve_name(node.token.lexeme.value())) {
            node.source = ref;
        } else {
            throw std::format("Reference to undefined symbol {}", node.token.lexeme.value()); // TODO
        }
    }

    void visit(Expression_statement& node) override {
        node.expression->accept(*this);
    }

    void visit(Variable_declaration_statement& node) override {
        visible_vars.back().push_back(std::ref(node));
    }

    void visit(If_statement& node) override {
        node.condition->accept(*this);
        visible_vars.push_back({});
        for (auto&& stmt : node.if_body) {
            stmt->accept(*this);
        }
        visible_vars.pop_back();
        visible_vars.push_back({});
        for (auto&& stmt : node.else_body) {
            stmt->accept(*this);
        }
        visible_vars.pop_back();
    }

    void visit(While_statement& node) override {
        node.condition->accept(*this);
        visible_vars.push_back({});
        for (auto&& stmt : node.body) {
            stmt->accept(*this);
        }
        visible_vars.pop_back();
    }

    void visit(Return_statement& node) override {
        node.expression->accept(*this);
    }

    void visit(Function_definition& node) override {
        node.var.accept(*this);
        visible_vars.push_back({});
        for (auto&& arg : node.arguments) {
            arg->accept(*this);
        }
        for (auto&& stmt : node.body) {
            stmt->accept(*this);
        }
        visible_vars.pop_back();
    }

    void visit(Programm& node) override {
        visible_vars.clear();
        visible_vars.push_back({});

        for (auto&& v : node.globals_vars) {
            v->accept(*this);
        }
        for (auto&& f : node.functions) {
            f->accept(*this);
        }

        visible_vars.pop_back();
    }

  private:
    std::vector<std::vector<Var_ref>> visible_vars{};
};



}
