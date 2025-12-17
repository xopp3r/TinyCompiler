export module ast:I_ast_visitor;

export namespace tc {

class Node;
class BinaryOperation;
class UnaryOperation;
class FunctionCall;
class Integer_literal;
class StringLiteral;
class CharLiteral;
class Variable;
class ExpressionStatement;
class VariableDeclarationStatement;
class IfStatement;
class WhileStatement;
class ReturnStatement;
class FunctionDefinition;
class Programm;

// interface for visitor pattern
class I_ast_visitor {
   public:
    virtual ~I_ast_visitor() = default;

    virtual void visit(BinaryOperation& node) = 0;
    virtual void visit(UnaryOperation& node) = 0;
    virtual void visit(FunctionCall& node) = 0;
    virtual void visit(Integer_literal& node) = 0;
    virtual void visit(StringLiteral& node) = 0;
    virtual void visit(CharLiteral& node) = 0;
    virtual void visit(Variable& node) = 0;
    virtual void visit(ExpressionStatement& node) = 0;
    virtual void visit(VariableDeclarationStatement& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(FunctionDefinition& node) = 0;
    virtual void visit(Programm& node) = 0;
};

}  // namespace tc