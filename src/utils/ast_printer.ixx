module;
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <variant>

#define SUBDIR(...)             \
    prefix.resize(++depth + 1); \
    __VA_ARGS__                 \
    prefix.resize(--depth + 1);

export module ast_printer;
import ast;
import token;

namespace tc {

constexpr auto Vbar = std::u8string_view((u8"│ "));
constexpr auto Cbar = std::u8string_view((u8"├─"));
constexpr auto Ebar = std::u8string_view((u8"└─"));
constexpr auto Nbar = std::u8string_view((u8"  "));

namespace {
    std::string constexpr operator+(std::string s, std::string_view sw) {
        return s + std::string{sw};
    }
}

export class AST_printer final : public I_ast_visitor {
   public:
    void PrintProgramASTasHierarcy(Programm& node);

   private:
    size_t depth = 0;
    std::vector<std::string_view> prefix;
    size_t tempPrefixIndex = 0;
    std::u8string_view afterPrefix;

    void updateTempPrefix();
    void setTempPrefix(const std::u8string_view& tempPrefix, const std::u8string_view& expiritionPrefix);

    void print(const std::string_view& str);
    void printVec(const std::vector<std::unique_ptr<Node>>& vec);

    void* visit(Binary_operation& node) override;
    void* visit(Unary_operation& node) override;
    void* visit(Function_call& node) override;
    void* visit(Type_operation& node) override;
    void* visit(Integer_literal& node) override;
    void* visit(String_literal& node) override;
    void* visit(Char_literal& node) override;
    void* visit(Variable& node) override;
    void* visit(Expression_statement& node) override;
    void* visit(Variable_declaration_statement& node) override;
    void* visit(If_statement& node) override;
    void* visit(While_statement& node) override;
    void* visit(Return_statement& node) override;
    void* visit(Function_definition& node) override;
    void* visit(Programm& node) override;

    template <typename T>
        requires std::derived_from<T, Node>
    void printVec(const std::vector<std::unique_ptr<T>>& vec) {
        if (vec.empty()) return;

        // SUBDIR(
        prefix.resize(++depth + 1);

        for (size_t i = 0; i < vec.size() - 1; ++i) {
            setTempPrefix(Cbar, Vbar);
            vec[i]->accept(*this);
        }

        setTempPrefix(Ebar, Nbar);
        vec.back()->accept(*this);

        prefix.resize(--depth + 1);  // )
    }
};

void AST_printer::PrintProgramASTasHierarcy(Programm& node) {
    prefix.push_back(std::string_view(reinterpret_cast<const char*>(Nbar.data())));  // set first row to empty
    visit(node);
    depth = 0;
    prefix.clear();
    tempPrefixIndex = 0;
}

void AST_printer::setTempPrefix(const std::u8string_view& tempPrefix, const std::u8string_view& expiritionPrefix) {
    tempPrefixIndex = depth;
    afterPrefix = expiritionPrefix;
    prefix.at(depth) = std::string_view(reinterpret_cast<const char*>(tempPrefix.data()));
}

void AST_printer::updateTempPrefix() {
    if (tempPrefixIndex != 0) {
        prefix.at(tempPrefixIndex) = std::string_view(reinterpret_cast<const char*>(afterPrefix.data()));
        tempPrefixIndex = 0;
    }
}

void AST_printer::print(const std::string_view& str) {
    for (const auto& pref : prefix) std::cout << pref;
    std::cout << str << std::endl;

    updateTempPrefix();
}

void* AST_printer::visit(Binary_operation& node) {
    print(node.operation.content_str());
    SUBDIR(setTempPrefix(Cbar, Vbar); node.left_value->accept(*this); setTempPrefix(Ebar, Nbar);
           node.right_value->accept(*this);)
    return nullptr;
}

void* AST_printer::visit(Unary_operation& node) {
    print(node.operation.content_str());
    SUBDIR(setTempPrefix(Ebar, Nbar); node.value->accept(*this);)
    return nullptr;
}

void* AST_printer::visit(Type_operation& node) {
    if (node.operation.type == Token_type::KEYWORD_AS) {
        print(std::string{"casted as "} + node.type.content_str());
    } else {
        print(std::string{"@ as "} + node.type.content_str());
    }
    SUBDIR(setTempPrefix(Ebar, Nbar); node.value->accept(*this);)
    return nullptr;
}

void* AST_printer::visit(Function_call& node) {
    print("FunctionCall");
    SUBDIR(setTempPrefix(Cbar, Vbar); print("adress");
           SUBDIR(setTempPrefix(Ebar, Nbar); 
        std::visit([this](auto&& e){ e->accept(*this); }, node.function_address);)

               setTempPrefix(Ebar, Nbar);
           print("args " + std::to_string(node.arguments.size())); SUBDIR(printVec(node.arguments);)

    )
    return nullptr;
}

void* AST_printer::visit(Integer_literal& node) {
    print("num(" + std::to_string(node.value) + ")");
    return nullptr;
}

void* AST_printer::visit(String_literal& node) {
    print("str(" + std::string(node.token.content_str()) + ")");
    return nullptr;
}

void* AST_printer::visit(Char_literal& node) {
    print("char(" + std::to_string(node.value) + ")");
    return nullptr;
}

void* AST_printer::visit(Variable& node) {
    print("var(" + std::string(node.token.content_str()) + ")");
    return nullptr;
}

void* AST_printer::visit(Expression_statement& node) {
    print("Statement");
    SUBDIR(setTempPrefix(Ebar, Nbar); node.expression->accept(*this);)
    return nullptr;
}

void* AST_printer::visit(Variable_declaration_statement& node) {
    print(std::string(node.type.content_str()) + " " + std::string(node.name.content_str()));
    return nullptr;
}

void* AST_printer::visit(If_statement& node) {
    print("If");
    SUBDIR(setTempPrefix(Cbar, Vbar); print("condition");
           SUBDIR(setTempPrefix(Ebar, Nbar); node.condition->accept(*this);)

               setTempPrefix(Cbar, Vbar);
           print("ifBody " + std::to_string(node.if_body.size()));
           SUBDIR(printVec(node.if_body);) setTempPrefix(Ebar, Nbar);
           print("elseBody " + std::to_string(node.else_body.size())); SUBDIR(printVec(node.else_body);)

    )
    return nullptr;
}

void* AST_printer::visit(While_statement& node) {
    print("While");
    SUBDIR(

        setTempPrefix(Cbar, Vbar); print("condition"); SUBDIR(setTempPrefix(Ebar, Nbar); node.condition->accept(*this);)

            setTempPrefix(Ebar, Nbar);
        print("body " + std::to_string(node.body.size())); SUBDIR(printVec(node.body);))
    return nullptr;
}

void* AST_printer::visit(Return_statement& node) {
    print("Return");
    SUBDIR(setTempPrefix(Ebar, Nbar); node.expression->accept(*this);)
    return nullptr;
}

void* AST_printer::visit(Function_definition& node) {
    print("Function " + std::string(node.return_type.content_str()) + " " + std::string(node.var.name.content_str()));
    SUBDIR(

        setTempPrefix(Cbar, Vbar); print("args " + std::to_string(node.arguments.size()));
        SUBDIR(printVec(node.arguments);)

            setTempPrefix(Ebar, Nbar);
        print("body " + std::to_string(node.body.size())); printVec(node.body);

    )
    return nullptr;
}

void* AST_printer::visit(Programm& node) {
    print("Programm");
    SUBDIR(

        setTempPrefix(Cbar, Vbar); print("Globals " + std::to_string(node.globals_vars.size()));
        printVec(node.globals_vars);

        setTempPrefix(Ebar, Nbar); print("Functions " + std::to_string(node.functions.size()));
        printVec(node.functions);

    )
    return nullptr;
}

}  // namespace tc
