module;
#include <format>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

export module ast:nodes;
export import :I_ast_visitor;
export import :annotations;
import parser_exception;
import token;

export namespace tc {

using Var_ref = std::reference_wrapper<Variable_declaration_statement>;

// Base class for every node in ast
class Node {
   public:
    virtual ~Node() = default;

    // virtual method for Visitor pattern
    virtual void accept(I_ast_visitor& visitor) = 0;
    // add this function to every final class
    // void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) = default;
    Node& operator=(Node&&) = default;

   protected:
    Node() = default;
};

// < ================ EXPRESSIONS ================ >

class Expression : public Node {
   public:
    virtual ~Expression() override = default;

    Expression_type type;
};

class Unary_operation final : public Expression {
   public:
    explicit Unary_operation(Token operation, std::unique_ptr<Expression> value)
        : operation(std::move(operation)), value(std::move(value)){};

    Token operation;
    std::unique_ptr<Expression> value;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Binary_operation final : public Expression {
   public:
    explicit Binary_operation(Token operation, std::unique_ptr<Expression> left_value,
                              std::unique_ptr<Expression> right_value)
        : left_value(std::move(left_value)), right_value(std::move(right_value)), operation(std::move(operation)){};

    std::unique_ptr<Expression> left_value;
    std::unique_ptr<Expression> right_value;
    Token operation;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Function_call final : public Expression {
   public:
    explicit Function_call(std::unique_ptr<Expression> function_adress,
                           std::vector<std::unique_ptr<Expression>> arguments)
        : function_address(std::move(function_adress)), arguments(std::move(arguments)){};

    std::unique_ptr<Expression> function_address;
    std::vector<std::unique_ptr<Expression>> arguments;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

// < ================ LITERALS and VARIABLES ================ >

class Primitive : public Expression {
   public:
    ~Primitive() override = default;

    Token token;

   protected:
    Primitive(Token tok) : token(std::move(tok)){};
};

class Integer_literal final : public Primitive {
   public:
    explicit Integer_literal(Token literal) : Primitive(std::move(literal)) {
        try {
            value = std::stoi(token.lexeme.value());  // string to int
        } catch (std::invalid_argument&) {
            throw Parser_exception(std::format("Invalid number literal: {}", token), token.position);
        } catch (std::out_of_range&) {
            throw Parser_exception(std::format("Number literal {} is too big", token), token.position);
        }
    };

    int value;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class String_literal final : public Primitive {
   public:
    explicit String_literal(Token literal) : Primitive(std::move(literal)){};

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Char_literal final : public Primitive {
   public:
    explicit Char_literal(Token literal) : Primitive(std::move(literal)), value(literal.lexeme.value().at(0)) {
        if (std::size(literal.lexeme.value()) != 1) {
            throw Parser_exception(std::format("Invalid char literal: {}", token));
        }
    };

    char value;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Variable final : public Primitive {
   public:
    explicit Variable(Token tok) : Primitive(std::move(tok)){};

    std::optional<Var_ref> source;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

// < ================ STATEMENTS ================ >

class Statement : public Node {
   public:
    ~Statement() override = default;
};

class Expression_statement final : public Statement {
   public:
    explicit Expression_statement(std::unique_ptr<Expression> expr) : expression(std::move(expr)){};

    std::unique_ptr<Expression> expression;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Variable_declaration_statement final : public Statement {
   public:
    explicit Variable_declaration_statement(Token type, Token name, Linkage_type linkage = Linkage_type::NONE)
        : type(std::move(type)), name(std::move(name)), linkage(linkage){};

    Token type;
    Token name;
    Linkage_type linkage;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class If_statement final : public Statement {
   public:
    explicit If_statement(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> if_body,
                          std::vector<std::unique_ptr<Statement>> else_body)
        : condition(std::move(condition)), if_body(std::move(if_body)), else_body(std::move(else_body)){};

    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> if_body;
    std::vector<std::unique_ptr<Statement>> else_body;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class While_statement final : public Statement {
   public:
    explicit While_statement(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Statement>> body)
        : condition(std::move(condition)), body(std::move(body)){};

    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Return_statement final : public Statement {
   public:
    explicit Return_statement(std::unique_ptr<Expression> expr) : expression(std::move(expr)){};

    std::unique_ptr<Expression> expression;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

// < ================ OTHER ================ >

class Function_definition final : public Node {
   public:
    explicit Function_definition(Token name, Token return_type,
                                 std::vector<std::unique_ptr<Variable_declaration_statement>> arguments,
                                 std::vector<std::unique_ptr<Statement>> body,
                                 Linkage_type linkage = Linkage_type::NONE)
        : 
          var(Token{{}, {}, Token_type::TYPE_PTR}, std::move(name), linkage),
          return_type(std::move(return_type)),
          arguments(std::move(arguments)),
          body(std::move(body)) {};

    Variable_declaration_statement var;
    Token return_type;
    std::vector<std::unique_ptr<Variable_declaration_statement>> arguments;
    std::vector<std::unique_ptr<Statement>> body;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

class Programm final : public Node {
   public:
    explicit Programm(std::vector<std::unique_ptr<Function_definition>> functions,
                      std::vector<std::unique_ptr<Variable_declaration_statement>> globals)
        : functions(std::move(functions)), globals_vars(std::move(globals)) {}

    std::vector<std::unique_ptr<Function_definition>> functions;
    std::vector<std::unique_ptr<Variable_declaration_statement>> globals_vars;

    void accept(I_ast_visitor& visitor) override { visitor.visit(*this); }
};

}  // namespace tc
