module;
#include <concepts>
#include <cstddef>
#include <format>
#include <functional>
#include <memory>
#include <optional>
#include <print>
#include <stdexcept>
#include <type_traits>
#include <utility>

#define COMMA_OP ,
#define EXPAND_KEYWORDS_TOKENS(op) \
    TT::TYPE_CHAR op TT::TYPE_VOID op TT::TYPE_PTR op TT::TYPE_INT op TT::TYPE_UINT op TT::TYPE_BOOL

export module parser;
import exceptions;
import token;
import ast;


namespace tc {
using TT = Token_type;

template <typename F, typename Return_t, typename... Args>
concept next_token_callback = requires(F&& f, Args&&... args) {
    { std::invoke(std::forward<F>(f), std::forward<Args>(args)...) } -> std::convertible_to<Return_t>;
};

export template <next_token_callback<Token> Func>
class Parser {
   public:
    Parser() = delete;
    Parser(Func&& get_next_token_f)
        : current_token(std::invoke(get_next_token_f)), get_next_token(std::move(get_next_token_f)) {
        if (current_token.type == Token_type::END) {
            throw Parser_exception("Provided file is empty");
        }
    }

    AST build_AST();


   private:
    Token current_token;
    Func get_next_token;  // get next token

    template <Token_type... types>
    [[nodiscard]] bool current_is_one_of() {
        return (((current_token.type == types) or ... or false));
    }

    template <Token_type... types>
    [[nodiscard]] bool try_discard_token() {
        if (((current_token.type != types) and ... and true)) {
            return false;
        }
        current_token = std::invoke(get_next_token);
        return true;
    }

    template <Token_type... types>
    void discard_token() {
        if (not try_discard_token<types...>()) {
            auto fmt = std::format("Unexpected token: {}, type '{}' do not match any of allowed types: ", current_token, current_token.type_str());
            fmt = (fmt + ... + (std::string{type_str(types)} + " or "));
            fmt.resize(std::size(fmt) - 4);
            throw Parser_exception(std::move(fmt), current_token.position);
        }
    }

    template <Token_type... types>
    [[nodiscard]] Token consume_token() {
        Token t{std::move(current_token)};
        discard_token<types...>();
        return t;
    }

    template <Token_type... types>
    [[nodiscard]] std::optional<Token> try_consume_token() {
        if (((current_token.type != types) and ... and true)) {
            return std::nullopt;
        }
        Token t{std::move(current_token)};
        current_token = std::invoke(get_next_token);
        return t;
    }

    template <int priority>
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, priority>) {
        static_assert(false,
                      "Invalid priority level in parse_priority\n"
                      "Main expression parsing function (parse_priority) compiling error");
    }

    template <int operation_priority, Token_type... Allowed_tokens_types>
    std::unique_ptr<Expression> parse_left_associative_binary_operation_sequence() {
        auto left = parse_priority(std::integral_constant<int, operation_priority - 10>{});  // parse higher priority expressions first

        // while priority of current operation match
        while (auto op = try_consume_token<Allowed_tokens_types...>()) {
            // parse next trem
            auto right = parse_priority(std::integral_constant<int, operation_priority - 10>{});

            // build tree and left rotate
            left = std::make_unique<Binary_operation>(std::move(op.value()), std::move(left), std::move(right));
        }
        return left;
    }

    template <int operation_priority, Token_type... Allowed_tokens_types>
    std::unique_ptr<Expression> parse_right_associative_binary_operation_sequence() {
        // parse most left expr untill operator matches priority
        auto left = parse_priority(std::integral_constant<int, operation_priority - 10>{});

        if (auto op = try_consume_token<Allowed_tokens_types...>()) {
            // recursivly parse right expression for right-to-left associativity
            auto right =
                parse_right_associative_binary_operation_sequence<operation_priority, Allowed_tokens_types...>();

            left = std::make_unique<Binary_operation>(std::move(op.value()), std::move(left), std::move(right));
        }
        return left;
    }

    // priority_1              // highest priority
    // 	: identifier | string_literal | number_literal | char_literal
    //  | '(' expression ')'
    // 	;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 1>) {
        switch (current_token.type) {
            case TT::CHAR:
                return std::make_unique<Char_literal>(consume_token<TT::CHAR>());
            case TT::STRING:
                return std::make_unique<String_literal>(consume_token<TT::STRING>());
            case TT::NUMBER:
                return std::make_unique<Integer_literal>(consume_token<TT::NUMBER>());
            case TT::IDENTIFIER:
                return std::make_unique<Variable>(consume_token<TT::IDENTIFIER>());
            case TT::PARENTHESES_OPEN: {
                discard_token<TT::PARENTHESES_OPEN>();
                auto expr = parse_expression();
                discard_token<TT::PARENTHESES_CLOSE>();
                return expr;
            };
            default:
                throw Parser_exception(std::format("Invalid expression {}", current_token), current_token.position);
        }
    }

    // priority_10
    //   : priority_1 ( ('(' (expression (',' expression)*)? ')') | ('[' expression ']') )*
    //   ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 10>) {
        auto left = parse_priority(std::integral_constant<int, 1>{});

        while (auto tok = try_consume_token<TT::PARENTHESES_OPEN, TT::SQUARE_BRACE_OPEN>()) {
            switch (tok->type) {
                case TT::SQUARE_BRACE_OPEN: {
                    // array indexing
                    auto right = parse_expression();
                    discard_token<TT::SQUARE_BRACE_CLOSE>();

                    // converting arr[i] to @(arr + i)
                    left = std::make_unique<Binary_operation>(Token{std::nullopt, tok->position, TT::OP_PLUS},
                                                              std::move(left), std::move(right));
                    left = std::make_unique<Unary_operation>(Token{std::nullopt, tok->position, TT::OP_DEREFERENCE},
                                                             std::move(left));
                    break;
                }
                case TT::PARENTHESES_OPEN: {
                    // function call
                    std::vector<std::unique_ptr<Expression>> args{};

                    if (not try_discard_token<TT::PARENTHESES_CLOSE>()) {
                        do {
                            args.push_back(parse_expression());
                        } while (try_discard_token<TT::COMMA>());
                        discard_token<TT::PARENTHESES_CLOSE>();
                    }

                    left = std::make_unique<Function_call>(std::move(left), std::move(args));
                    break;
                }
                default:
                    throw std::logic_error("Unreachable state of parse_priority<10>");
            }
        }

        return left;
    }

    // priority_15
    // 	 : priority_10
    // 	 | ('&' | ('@' type) | '-' | '!') priority_15
    // 	 ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 15>) {
        if (auto op = try_consume_token<TT::OP_ADRESS, TT::OP_DEREFERENCE, TT::OP_MINUS, TT::OP_NOT>()) {
            if (op->type == TT::OP_DEREFERENCE) {
                auto type = consume_token<EXPAND_KEYWORDS_TOKENS(COMMA_OP)>();
                auto inner = parse_priority(std::integral_constant<int, 15>{});
                return std::make_unique<Type_operation>(std::move(op.value()), std::move(inner), std::move(type));
            } else {
                auto inner = parse_priority(std::integral_constant<int, 15>{});
                return std::make_unique<Unary_operation>(std::move(op.value()), std::move(inner));
            }
        } else {
            return parse_priority(std::integral_constant<int, 10>{});
        }
    }

    // priority_20
    // 	 : priority_15 ('as' type)*
    // 	 ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 20>) {
        auto left = parse_priority(std::integral_constant<int, 15>{});
        while (auto op = try_consume_token<TT::KEYWORD_AS>()) {
            auto type = consume_token<EXPAND_KEYWORDS_TOKENS(COMMA_OP)>();

            left = std::make_unique<Type_operation>(std::move(op.value()), std::move(left), std::move(type));
        }
        return left;
    }

    // priority_30
    //   : priority_20 (('*' | '/' | '%') priority_20)*
    //   ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 30>) {
        return parse_left_associative_binary_operation_sequence<30, TT::OP_MUL, TT::OP_DIV, TT::OP_MOD>();
    }

    // priority_40
    //   : priority_30 (('+' | '-') priority_30)*
    //   ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 40>) {
        return parse_left_associative_binary_operation_sequence<40, TT::OP_PLUS, TT::OP_MINUS>();
    }

    // priority_50
    //   : priority_40 (('>' | '<' | '>=' | '<=' | '==' | '!=') priority_40)*
    //   ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 50>) {
        return parse_left_associative_binary_operation_sequence<50, TT::OP_GREATER, TT::OP_GREATER_EQ, TT::OP_LESS,
                                                                TT::OP_LESS_EQ, TT::OP_EQUAL, TT::OP_NOT_EQUAL>();
    }

    // priority_60
    //   : priority_50 ('&&' priority_50)*
    //   ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 60>) {
        return parse_left_associative_binary_operation_sequence<60, TT::OP_AND>();
    }

    // priority_70
    //   : priority_60 ('||' priority_60)*
    //   ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 70>) {
        return parse_left_associative_binary_operation_sequence<70, TT::OP_OR>();
    }

    // priority_80
    //   : priority_70 ('=' priority_80)?
    //   ;
    std::unique_ptr<Expression> parse_priority(std::integral_constant<int, 80>) {
        return parse_right_associative_binary_operation_sequence<80, TT::OP_ASSIGNMENT>();
    }

    std::unique_ptr<Expression> parse_expression() { 
            return parse_priority(std::integral_constant<int, 80>{}); }

    // variable_declaration
    //     : type identifier
    std::unique_ptr<Variable_declaration_statement> parse_variable_declaration_statement() {
        auto type = consume_token<EXPAND_KEYWORDS_TOKENS(COMMA_OP)>();
        auto name = consume_token<TT::IDENTIFIER>();
        return std::make_unique<Variable_declaration_statement>(std::move(type), std::move(name));
    }

    // body
    //     : '{' statement* '}'
    //     ;
    std::vector<std::unique_ptr<Statement>> parse_body() {
        std::vector<std::unique_ptr<Statement>> statements;

        discard_token<TT::BRACE_OPEN>();
        while (not try_discard_token<TT::BRACE_CLOSE>()) {
            statements.push_back(parse_statement());
        }

        return statements;
    }

    // statement
    //     : expression ';'
    //     | variable_declaration ';'
    //     | 'return' expression ';'
    //     | if_statement
    //     | while_statement
    //     ;
    std::unique_ptr<Statement> parse_statement() {
        switch (current_token.type) {
            case TT::KEYWORD_IF:
                return parse_if_statement();
            case TT::KEYWORD_WHILE:
                return parse_while_statement();
            case TT::KEYWORD_RETURN:
                return parse_return_statement();
            case EXPAND_KEYWORDS_TOKENS(: case): {
                    auto s = parse_variable_declaration_statement();
                    discard_token<TT::SEMICOLON>();
                    return s;
                }
            default:
                return parse_expression_statement();
        }
    }

    // if_statement
    //     : 'if' '(' expression ')' body ('else' body)?
    //     ;
    std::unique_ptr<If_statement> parse_if_statement() {
        discard_token<TT::KEYWORD_IF>();
        discard_token<TT::PARENTHESES_OPEN>();
        auto cond = parse_expression();
        discard_token<TT::PARENTHESES_CLOSE>();
        auto body = parse_body();
        if (try_discard_token<TT::KEYWORD_ELSE>()) {
            auto else_body = parse_body();
            return std::make_unique<If_statement>(std::move(cond), std::move(body), std::move(else_body));
        } else {
            return std::make_unique<If_statement>(std::move(cond), std::move(body),
                                                  std::vector<std::unique_ptr<Statement>>{});
        }
    }

    // while_statement
    //     : 'while' '(' expression ')' body
    //     ;
    std::unique_ptr<While_statement> parse_while_statement() {
        discard_token<TT::KEYWORD_WHILE>();
        discard_token<TT::PARENTHESES_OPEN>();
        auto cond = parse_expression();
        discard_token<TT::PARENTHESES_CLOSE>();
        auto body = parse_body();
        return std::make_unique<While_statement>(std::move(cond), std::move(body));
    }

    // 'return' expression ';'
    std::unique_ptr<Return_statement> parse_return_statement() {
        discard_token<TT::KEYWORD_RETURN>();
        auto expr = parse_expression();
        discard_token<TT::SEMICOLON>();
        return std::make_unique<Return_statement>(std::move(expr));
    }

    // expression ';'
    std::unique_ptr<Expression_statement> parse_expression_statement() {
        auto expr = parse_expression();
        discard_token<TT::SEMICOLON>();
        return std::make_unique<Expression_statement>(std::move(expr));
    }

    // argument_list_defenition
    //     : (variable_declaration (',' variable_declaration)*)?
    //     ;
    std::vector<std::unique_ptr<Variable_declaration_statement>> parse_argument_list_defenition() {
        std::vector<std::unique_ptr<Variable_declaration_statement>> args;

        if (current_token.type == TT::PARENTHESES_CLOSE) {
            return args;
        }

        do {
            args.push_back(parse_variable_declaration_statement());
        } while (try_discard_token<TT::COMMA>());

        return args;
    }

    // function_defenition
    //     : 'function' type identifier '(' argument_list_defenition ')' body
    //     ;
    std::unique_ptr<Function_definition> parse_function_definition() {
        discard_token<TT::KEYWORD_FUNCTION>();
        auto type = consume_token<EXPAND_KEYWORDS_TOKENS(COMMA_OP)>();
        auto name = consume_token<TT::IDENTIFIER>();
        discard_token<TT::PARENTHESES_OPEN>();
        auto args = parse_argument_list_defenition();
        discard_token<TT::PARENTHESES_CLOSE>();
        auto body = parse_body();
        return std::make_unique<Function_definition>(std::move(name), std::move(type), std::move(args),
                                                     std::move(body));
    }

    // linkage_qualifiers
    //     : export
    //     | extern
    //     ;

    // programm
    //     : (linkage_qualifiers?  (function_defenition | variable_declaration))+
    //     ;
    std::unique_ptr<Programm> parse_programm() {
        std::vector<std::unique_ptr<Function_definition>> functions;
        std::vector<std::unique_ptr<Variable_declaration_statement>> global_variables;

        while (current_token) {
            Linkage_type lt = Linkage_type::NONE;
            if (auto t = try_consume_token<TT::KEYWORD_EXPORT, TT::KEYWORD_EXTERN>()) {
                lt = linkage_mapping(t->type);
            }

            switch (current_token.type) {
                case TT::KEYWORD_FUNCTION: {
                    auto f = parse_function_definition();
                    f->var.linkage = lt;
                    functions.push_back(std::move(f));
                    break;
                }
                case EXPAND_KEYWORDS_TOKENS(: case): {
                    auto f = parse_variable_declaration_statement();
                    discard_token<TT::SEMICOLON>();
                    f->linkage = lt;
                    global_variables.push_back(std::move(f));
                    break;
                }
                default:
                    throw Parser_exception(std::format("Unexpected token: {}", current_token), current_token.position);
            }
        }

        return std::make_unique<Programm>(std::move(functions), std::move(global_variables));
    }
};

template <next_token_callback<Token> Func>
AST Parser<Func>::build_AST() {
    return AST{parse_programm()};
}

}  // namespace tc