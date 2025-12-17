module;
#include <concepts>
#include <memory>
#include <optional>

export module parser;
import token;
import ast;






namespace tc {

template <typename F, typename Return_t, typename... Args>
concept next_token_callback = requires(F&& f, Args&&... args) {
    { std::invoke(std::forward<F>(f), std::forward<Args>(args)...) } -> std::convertible_to<Return_t>;
};


template <next_token_callback<Token> Func>
class MyParser {
    public:
    MyParser() = delete;
    MyParser(Func &&get_next_token) : get_next_token(std::move(get_next_token)) {}

    AST build_AST();

    private:
    Func get_next_token; // get next token
    Token currentToken;

    void discardToken(Token_type type); 
    bool discardTokenOpt(Token_type type); 
    Token consumeToken(Token_type type); 
    std::optional<Token> consumeTokenOpt(Token_type type); 

    // parsing functions
    std::unique_ptr<Programm> parse();
    
    std::unique_ptr<FunctionDefinition> parseFunctionDefinition();
    std::vector<std::unique_ptr<VariableDeclarationStatement>> parseFunctionDefinitionArguments();
    std::vector<std::unique_ptr<Expression>> parseFunctionCallArguments();

    std::vector<std::unique_ptr<Statement>> parseStatementSequence();
    
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<IfStatement> parseIfStatement();
    std::unique_ptr<WhileStatement> parseWhileStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<VariableDeclarationStatement> parseVariableDeclarationStatement();
    
    std::unique_ptr<Expression> parseExpression();

    
};


}