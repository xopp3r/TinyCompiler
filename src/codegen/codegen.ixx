module;

#include <llvm-22/llvm/IR/LLVMContext.h>
#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <vector>
#define vl (llvm::Value *)

#include <memory>

#include "llvm/ADT/APInt.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

export module codegen;
import ast;
import token;


namespace {
constinit const char* MODULE_NAME = "The module";

}

namespace tc {
namespace rn = std::ranges;
namespace rnv = std::ranges::views;

class Codegenerator : public I_ast_visitor {
   public:
    Codegenerator()
        : builder(std::make_unique<llvm::IRBuilder<>>(ctx)),
          module(std::make_unique<llvm::Module>(MODULE_NAME, ctx)) {}

    llvm::Type* llvm_type(tc::Token_type type) {
        const auto t = tc::type_mapping(type);
        switch (t) {
            case tc::Expression_type::BOOL: return llvm::Type::getInt1Ty(ctx);
            case tc::Expression_type::UINT: return llvm::Type::getInt32Ty(ctx);
            case tc::Expression_type::INT: return llvm::Type::getInt32Ty(ctx);
            case tc::Expression_type::CHAR: return llvm::Type::getInt8Ty(ctx);
            case tc::Expression_type::PTR: return llvm::Type::getInt32Ty(ctx);
            case tc::Expression_type::VOID: return llvm::Type::getVoidTy(ctx);
            default: throw std::logic_error{"Invalid type"};
        }
    }

    llvm::GlobalValue::LinkageTypes llvm_linkage(tc::Linkage_type t) {
        switch (t) {
            case tc::Linkage_type::EXPORT: return llvm::Function::ExternalLinkage;
            case tc::Linkage_type::EXTERN: return llvm::Function::ExternalWeakLinkage;
            case tc::Linkage_type::NONE: return llvm::Function::InternalLinkage;
            default: throw std::logic_error{"Invalid linkage type"};
        }
    }

    void* visit(Binary_operation& /*node*/) override { return nullptr; }

    void* visit(Unary_operation& /*node*/) override { return nullptr; }

    void* visit(Type_operation& /*node*/) override { return nullptr; }

    void* visit(Function_call& /*node*/) override { return nullptr; }

    void* visit(Integer_literal& /*node*/) override { return nullptr; }

    void* visit(String_literal& /*node*/) override { return nullptr; }

    void* visit(Char_literal& /*node*/) override { return nullptr; }

    void* visit(Variable& /*node*/) override { return nullptr; }

    void* visit(Expression_statement& /*node*/) override { return nullptr; }

    void* visit(Variable_declaration_statement& /*node*/) override { return nullptr; }

    void* visit(If_statement& /*node*/) override { return nullptr; }

    void* visit(While_statement& /*node*/) override { return nullptr; }

    void* visit(Return_statement& /*node*/) override { return nullptr; }

    void* visit(Function_definition& node) override {
        llvm::Type* ret = llvm_type(node.return_type.type);
        auto a = node.arguments | rnv::transform([this](auto&& p){ return llvm_type(p->type.type); });
        std::vector<llvm::Type*> args(std::from_range, a);
        llvm::FunctionType* func_type = llvm::FunctionType::get(ret, args, false); // TODO VARARGS;
        llvm::Function* function = llvm::Function::Create(func_type, llvm_linkage(node.var.linkage), node.var.name.content_str(), module.get());
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx, "entry", function);
        builder->SetInsertPoint(entry);
        module->print(llvm::outs(), nullptr);
        // rn::for_each(node.body, [this](auto&& stmt){ stmt->accept(*this); });
        return nullptr;
    }

    void* visit(Programm& /*node*/) override { return nullptr; }

   private:
    llvm::LLVMContext ctx{};
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
};

}  // namespace tc
