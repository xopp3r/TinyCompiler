module;
#include <format>

export module cfg_checker;
import ast;
import exceptions;

namespace tc {

export class CFG_checker final : public I_ast_visitor {
   public:
    void* visit(Binary_operation&) override { return nullptr; }
    void* visit(Unary_operation&) override { return nullptr; }
    void* visit(Function_call&) override { return nullptr; }
    void* visit(Type_operation&) override { return nullptr; }
    void* visit(Integer_literal&) override { return nullptr; }
    void* visit(String_literal&) override { return nullptr; }
    void* visit(Char_literal&) override { return nullptr; }
    void* visit(Variable&) override { return nullptr; }
    void* visit(Expression_statement&) override { return nullptr; }
    void* visit(If_statement&) override { return nullptr; }
    void* visit(While_statement&) override { return nullptr; }
    void* visit(Variable_declaration_statement&) override { return nullptr; }
    void* visit(Return_statement&) override { return this; }

    void* visit(Function_definition& node) override {
        if (std::size(node.body) == 0 or not node.body.back()->accept(*this)) {
            throw CFG_exception{std::format("function {} does not end with a return", node.var.name.content_str()), node.var.name.position};
        }
        return nullptr;
    }

    void* visit(Programm& node) override {
        for (auto&& f : node.functions) {
            f->accept(*this);
        }
        return nullptr;
    }
};

}  // namespace tc
