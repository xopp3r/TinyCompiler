export module ast:I_ast_visitor;

export namespace tc {

class Node;
class Binary_operation;
class Unary_operation;
class Type_operation;
class Function_call;
class Integer_literal;
class String_literal;
class Char_literal;
class Variable;
class Expression_statement;
class Variable_declaration_statement;
class If_statement;
class While_statement;
class Return_statement;
class Function_definition;
class Programm;

// interface for visitor pattern
class I_ast_visitor {
   public:
    virtual ~I_ast_visitor() = default;

    virtual void visit(Binary_operation& node) = 0;
    virtual void visit(Unary_operation& node) = 0;
    virtual void visit(Type_operation& node) = 0;
    virtual void visit(Function_call& node) = 0;
    virtual void visit(Integer_literal& node) = 0;
    virtual void visit(String_literal& node) = 0;
    virtual void visit(Char_literal& node) = 0;
    virtual void visit(Variable& node) = 0;
    virtual void visit(Expression_statement& node) = 0;
    virtual void visit(Variable_declaration_statement& node) = 0;
    virtual void visit(If_statement& node) = 0;
    virtual void visit(While_statement& node) = 0;
    virtual void visit(Return_statement& node) = 0;
    virtual void visit(Function_definition& node) = 0;
    virtual void visit(Programm& node) = 0;
};

}  // namespace tc