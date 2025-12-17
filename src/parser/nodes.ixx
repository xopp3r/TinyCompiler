module;
#include <format>
#include <memory>
#include <optional>
#include <vector>

export module ast:nodes;
export import :I_ast_visitor;
import token;
import parser_exception;

export namespace tc {

// Base class for every node in ast
class Node {
   public:
    Node() = delete;
    virtual ~Node() = default;

    // virtual method for Visitor pattern
    virtual void accept(I_ast_visitor& visitor) = 0;
    // add this function to every final class
    // void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) = default;
    Node& operator=(Node&&) = default;

    Token token;

   protected:
    Node(Token tok) : token(tok) {}
};

// < ================ EXPRESSIONS ================ >

class Expression : public Node {
   public:
    virtual ~Expression() override = default;

   protected:
    Expression(Token tok) : Node(tok) {}
};

class Unary_operation final : public Expression {
   public:
    explicit Unary_operation(Token tok, std::unique_ptr<Expression> value, Token_type Operation)
        : Expression(tok), value(std::move(value)), operation(Operation){};

    std::unique_ptr<Expression> value;
    Token_type operation;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Binary_operation final : public Expression {
   public:
    explicit Binary_operation(Token tok, std::unique_ptr<Expression> left_value, std::unique_ptr<Expression> right_value,
                             Token_type Operation)
        : Expression(tok),
          left_value(std::move(left_value)),
          right_value(std::move(right_value)),
          operation(Operation){};

    std::unique_ptr<Expression> left_value;
    std::unique_ptr<Expression> right_value;
    Token_type operation;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Function_call final : public Expression {
   public:
    explicit Function_call(Token tok, std::unique_ptr<Expression> function_adress,
                          std::vector<std::unique_ptr<Expression>>&& arguments)
        : Expression(tok), functionAdress(std::move(function_adress)), arguments(std::move(arguments)){};

    std::unique_ptr<Expression> functionAdress;
    std::vector<std::unique_ptr<Expression>> arguments;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

// < ================ LITERALS and VARIABLES ================ >

class Primitive : public Expression {
   public:
    ~Primitive() override = default;

   protected:
    Primitive(Token tok) : Expression(tok){};
};

class Integer_literal final : public Primitive {
   public:
    explicit Integer_literal(Token literal) : Primitive(literal) {
        try {
            value = std::stoll(token.lexeme.value());  // string to int
        } catch (std::invalid_argument&) {
            throw Parser_exception(std::format("Invalid number literal: {}", token), token.position);
        } catch (std::out_of_range&) {
            throw Parser_exception(std::format("Number literal {} is too big", token), token.position);
        }
    };

    long long value;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class String_literal final : public Primitive {
   public:
    explicit String_literal(Token literal) : Primitive(literal), value(literal.lexeme.value()){};

    std::string_view value;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Char_literal final : public Primitive {
   public:
    explicit Char_literal(Token literal) : Primitive(literal), value(literal.lexeme.value().at(0)) {
        if (std::size(literal.lexeme.value()) != 1) {
            throw Parser_exception(std::format("Invalid char literal: {}", token), token.position);
        }
    };

    char value;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Variable final : public Primitive {
   public:
    explicit Variable(Token tok) : Primitive(tok), identifier(tok.lexeme.value()){};

    std::string_view identifier;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

// < ================ STATEMENTS ================ >

class Statement : public Node {
   public:
    ~Statement() override = default;

   protected:
    Statement(Token tok) : Node(tok) {}
};

class Expression_statement final : public Statement {
   public:
    explicit Expression_statement(Token tok, std::unique_ptr<Expression> expr)
        : Statement(tok), expression(std::move(expr)){};

    std::unique_ptr<Expression> expression;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Variable_declaration_statement final : public Statement {
   public:
    explicit Variable_declaration_statement(Token tok, Token type, Token name) : Statement(tok), type(type), name(name){};

    Token type;
    Token name;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class If_statement final : public Statement {
   public:
    explicit If_statement(Token tok, std::unique_ptr<Expression> condition,
                         std::vector<std::unique_ptr<Statement>>&& if_body,
                         std::vector<std::unique_ptr<Statement>>&& else_body)
        : Statement(tok),
          condition(std::move(condition)),
          if_body(std::move(if_body)),
          else_body(std::move(else_body)){};

    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> if_body;
    std::vector<std::unique_ptr<Statement>> else_body;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class While_statement final : public Statement {
   public:
    explicit While_statement(Token tok, std::unique_ptr<Expression> condition,
                            std::vector<std::unique_ptr<Statement>>&& body)
        : Statement(tok), condition(std::move(condition)), body(std::move(body)){};

    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Return_statement final : public Statement {
   public:
    explicit Return_statement(Token tok, std::unique_ptr<Expression> expr)
        : Statement(tok), expression(std::move(expr)){};

    std::unique_ptr<Expression> expression;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

// < ================ OTHER ================ >

class Function_definition final : public Node {
   public:
    explicit Function_definition(Token tok, Token name, Token return_type,
                                std::vector<std::unique_ptr<Variable_declaration_statement>>&& arguments,
                                std::vector<std::unique_ptr<Statement>>&& body)
        : Node(tok), name(name), return_type(return_type), arguments(std::move(arguments)), body(std::move(body)){};

    Token name;
    Token return_type;
    std::vector<std::unique_ptr<Variable_declaration_statement>> arguments;
    std::vector<std::unique_ptr<Statement>> body;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Programm final : public Node {
   public:
    explicit Programm(std::vector<std::unique_ptr<Function_definition>>&& functions,
                      std::vector<std::unique_ptr<Variable_declaration_statement>>&& globals)
        : Node(Token{std::nullopt, Position{0, 0, 0}, Token_type::INVALID}),
          functions(std::move(functions)),
          globals_vars(std::move(globals)) {}

    std::vector<std::unique_ptr<Function_definition>> functions;
    std::vector<std::unique_ptr<Variable_declaration_statement>> globals_vars;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

}  // namespace tc
