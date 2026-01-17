module;


export module type_checker;
import ast;

namespace tc {


class Type_checker final : public I_ast_visitor {
   public:
    void visit(Binary_operation& node) override {

    }
    void visit(Unary_operation& node) override {

    }
    void visit(Function_call& node) override {

    }
    void visit(Integer_literal& node) override {

    }
    void visit(String_literal& node) override {

    }
    void visit(Char_literal& node) override {

    }
    void visit(Variable& node) override {

    }
    void visit(Expression_statement& node) override {

    }
    void visit(Variable_declaration_statement& node) override {

    }
    void visit(If_statement& node) override {

    }
    void visit(While_statement& node) override {

    }
    void visit(Return_statement& node) override {

    }
    void visit(Function_definition& node) override {

    }
    void visit(Programm& node) override {

    }
};



}
