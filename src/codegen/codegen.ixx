module;

#include <llvm-22/llvm/IR/Constant.h>
#include <llvm-22/llvm/IR/GlobalVariable.h>
#include <llvm-22/llvm/IR/LLVMContext.h>
#include <algorithm>
#include <functional>
#include <optional>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <print>

#include <memory>
#include <utility>
#include <variant>
#include <vector>

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
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/TargetParser/Host.h"

#define cast2llvmv (llvm::Value *)

export module codegen;
import ast;
import exceptions;
import token;


namespace tc {
namespace rn = std::ranges;
namespace rnv = std::ranges::views;

export class Codegenerator : public I_ast_visitor {
   public:
    Codegenerator(std::string_view name, std::ostream& out, std::string asked_triple = llvm::sys::getDefaultTargetTriple())
        : module{name, ctx}, out{out} {
            llvm::Triple triple{asked_triple};
            triple.setArchName("i386");
            module.setTargetTriple(triple);

            const char* DataLayout32Bit = 
                "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-"
                "f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-n32-S32";
            module.setDataLayout(DataLayout32Bit);
            
            const llvm::DataLayout& DL = module.getDataLayout();
            if (DL.getPointerSize() != 4) throw Codegen_exception{std::format("Target pointer size is not 4 bytes but {}", DL.getPointerSize())};
        }

    void generate(const AST& ast) {
        ast.root->accept(*this);
        llvm::raw_os_ostream out_file{out};
        module.print(out_file, nullptr);
    }

    llvm::Type* llvm_type(Token_type type) {
        return llvm_type(type_mapping(type));
    }

    llvm::Type* llvm_type(Expression_type type) {
        switch (type) {
            case Expression_type::BOOL: return llvm::Type::getInt1Ty(ctx);
            case Expression_type::UINT: return llvm::Type::getInt32Ty(ctx);
            case Expression_type::INT: return llvm::Type::getInt32Ty(ctx);
            case Expression_type::CHAR: return llvm::Type::getInt8Ty(ctx);
            case Expression_type::PTR: return llvm::PointerType::getUnqual(ctx);
            case Expression_type::VOID: return llvm::Type::getVoidTy(ctx);
            default: throw std::logic_error{"Invalid type"};
        }
    }

    llvm::GlobalValue::LinkageTypes llvm_linkage(Linkage_type t) {
        switch (t) {
            case Linkage_type::EXPORT: return llvm::Function::ExternalLinkage;
            case Linkage_type::EXTERN: return llvm::Function::ExternalWeakLinkage;
            case Linkage_type::NONE: return llvm::Function::PrivateLinkage;
            default: throw std::logic_error{"Invalid linkage type"};
        }
    }

    void* visit(Binary_operation& node) override {
        if (node.operation.type == Token_type::OP_ASSIGNMENT) {
            request_lvalue = node.left_value.get(); // (>~<)
        }

        auto* l_val = cast2llvmv node.left_value->accept(*this);
        auto* r_val = cast2llvmv node.right_value->accept(*this);

        switch (node.operation.type) {
            case Token_type::OP_PLUS:
                return builder.CreateAdd(l_val, r_val);
            case Token_type::OP_MINUS:
                return builder.CreateSub(l_val, r_val);
            case Token_type::OP_GREATER:
                return builder.CreateICmpSGT(l_val, r_val);
            case Token_type::OP_LESS:
                return builder.CreateICmpSLT(l_val, r_val);
            case Token_type::OP_GREATER_EQ:
                return builder.CreateICmpSGE(l_val, r_val);
            case Token_type::OP_LESS_EQ:
                return builder.CreateICmpSLE(l_val, r_val);
            case Token_type::OP_EQUAL:
                return builder.CreateICmpEQ(l_val, r_val);
            case Token_type::OP_NOT_EQUAL:
                return builder.CreateICmpNE(l_val, r_val);
            case Token_type::OP_AND:
                return builder.CreateAnd(l_val, r_val);
            case Token_type::OP_OR:
                return builder.CreateOr(l_val, r_val);
            case Token_type::OP_DIV:
                return builder.CreateSDiv(l_val, r_val);
            case Token_type::OP_MUL:
                return builder.CreateMul(l_val, r_val);
            case Token_type::OP_MOD:
                return builder.CreateSRem(l_val, r_val);
            case Token_type::OP_ASSIGNMENT: 
                builder.CreateStore(r_val, l_val);
                return r_val;
            default: 
                throw std::logic_error{"Invalid binary operation type"};
        }
    }

    void* visit(Unary_operation& node) override { 
        if (node.operation.type == Token_type::OP_ADRESS) {
            request_lvalue = node.value.get(); // (>~<)
        }

        auto* val = cast2llvmv node.value->accept(*this);

        switch (node.operation.type) {
            case Token_type::OP_NOT: return builder.CreateNot(val);
            case Token_type::OP_MINUS: {
                auto* zero = llvm::ConstantInt::get(llvm_type(node.value->metadata.type), 0);
                return builder.CreateAdd(zero, val);
            }
            case Token_type::OP_ADRESS: return val;
            default: throw std::logic_error{"Invalid unary operation type"};
        }
    }

    void* visit(Type_operation& node) override { 
        auto request_lvalue_backup = std::exchange(request_lvalue, nullptr);
        auto* val = cast2llvmv node.value->accept(*this);
        const auto type = node.type.type;

        switch (node.operation.type) {
            case Token_type::KEYWORD_AS: {
                auto s1 = size_of_type(node.value->metadata.type);
                auto s2 = size_of_type(type_mapping(type));

                // bool
                if (type == Token_type::TYPE_BOOL) {
                    auto* zero = llvm::ConstantInt::get(llvm_type(node.value->metadata.type), 0);
                    return builder.CreateICmpNE(val, zero, "to_bool");
                }

                // to/from pointers
                if (type == Token_type::TYPE_PTR) {
                    return builder.CreateIntToPtr(val, llvm_type(type));
                } 
                if (node.value->metadata.type == Expression_type::PTR) {
                    return builder.CreatePtrToInt(val, llvm_type(type));
                }

                // integer
                if (s1 == s2) {
                    return builder.CreateBitCast(val, llvm_type(type));
                } 
                if (s1 > s2) {
                    return builder.CreateTrunc(val, llvm_type(type));
                }
                
                // if less consider signess
                if (type_mapping(type) == Expression_type::UINT) {
                    return builder.CreateZExt(val, llvm_type(type));
                } else {
                    return builder.CreateSExt(val, llvm_type(type));
                }
                break;   
            }
            case Token_type::OP_DEREFERENCE: {
                if (request_lvalue_backup == &node) {
                    return val;
                } else {
                    return builder.CreateLoad(llvm_type(type), val, "deref");
                }
                break;
            }

            default: break;
        }
        throw std::logic_error{"Invalid type operation type"};
    } 

    void* visit(Function_call& node) override {
        llvm::Function* function = nullptr;
        if (auto* v = std::get_if<std::unique_ptr<Variable>>(&node.function_address)) {
            auto function_name = (*v)->source.value().get().name.content_str();
            function = module.getFunction(function_name);
        } else {
            // function = cast2llvmv std::get_if<std::unique_ptr<Variable>>(&node.function_address)->get()->accept(*this);
            throw std::logic_error{"Indirect calls are not implemented yet"}; // TODO 
        }

        auto llvm_args_r = node.arguments | rnv::transform([this](auto&& arg){ return cast2llvmv arg->accept(*this); });
        std::vector llvm_args{std::from_range, llvm_args_r};
        return builder.CreateCall(function, llvm_args);
    } 

    void* visit(Integer_literal& node) override { 
        return builder.getInt32(node.value);
    }

    void* visit(String_literal& node) override { 
        llvm::Constant *str = llvm::ConstantDataArray::getString(ctx, node.token.lexeme.value(), true);
        return new llvm::GlobalVariable(module, str->getType(), true, llvm::GlobalValue::InternalLinkage, str, "str");
    }

    void* visit(Char_literal& node) override { 
        return builder.getInt8(node.value);
    }

    llvm::Value* get_variable_ptr(const Variable& node) {
        const auto* var_def_p = &node.source.value().get();
        auto found = local_variables.find(var_def_p);
        if (found != local_variables.end()) return found->second;
        return global_symbols.at(var_def_p);
    }

    void* visit(Variable& node) override {
        auto request_lvalue_backup = std::exchange(request_lvalue, nullptr);
        auto* var_ptr = get_variable_ptr(node);
        if (request_lvalue_backup == &node) {
            return var_ptr;
        } else {
            return builder.CreateLoad(llvm_type(node.metadata.type), var_ptr);
        }
    }

    void* visit(Expression_statement& node) override { 
        node.expression->accept(*this);
        return nullptr; 
    }

    void* visit(Variable_declaration_statement& node) override { 
        llvm::Type* llvm_t = llvm_type(node.type.type);
        const auto var_name = node.name.content_str();
        
        if (current_function) {
            auto& BB = current_function->getEntryBlock();
            llvm::IRBuilder<> tmp_b(&BB, BB.begin());
            llvm::Value* var = tmp_b.CreateAlloca(llvm_t, nullptr, var_name); 
            local_variables.insert({&node, var});
            return var;
        } else {
            llvm::Constant* zero = (node.linkage != Linkage_type::EXTERN) ? llvm::ConstantInt::getNullValue(llvm_t) : nullptr; 
            llvm::Value* var = new llvm::GlobalVariable{module, llvm_t, false, llvm_linkage(node.linkage), zero, var_name};
            global_symbols.insert({&node, var});
            return var;
        }
    }

    void* visit(If_statement& node) override {
        llvm::BasicBlock* cond_bb = llvm::BasicBlock::Create(ctx, "if.cond_bb", current_function);
        llvm::BasicBlock* then_bb = llvm::BasicBlock::Create(ctx, "if.then_bb", current_function);
        llvm::BasicBlock* else_bb = llvm::BasicBlock::Create(ctx, "if.else_bb", current_function);
        llvm::BasicBlock* after_bb = llvm::BasicBlock::Create(ctx, "if.after_bb", current_function);
        
        builder.CreateBr(cond_bb);
        builder.SetInsertPoint(cond_bb);

        llvm::Value* condition = cast2llvmv node.condition->accept(*this);
        builder.CreateCondBr(condition, then_bb, else_bb); 
        
        builder.SetInsertPoint(then_bb);
        rn::for_each(node.if_body, [this](auto&& stmt){ stmt->accept(*this); });
        if (not builder.GetInsertBlock()->getTerminator()) builder.CreateBr(after_bb);

        builder.SetInsertPoint(else_bb);
        rn::for_each(node.else_body, [this](auto&& stmt){ stmt->accept(*this); });
        if (not builder.GetInsertBlock()->getTerminator()) builder.CreateBr(after_bb);

        builder.SetInsertPoint(after_bb);
        return nullptr;
    }

    void* visit(While_statement& node) override {
        llvm::BasicBlock* preheader_bb = llvm::BasicBlock::Create(ctx, "while.preheader_bb", current_function);
        llvm::BasicBlock* cond_bb = llvm::BasicBlock::Create(ctx, "while.cond_bb", current_function);
        llvm::BasicBlock* body_bb = llvm::BasicBlock::Create(ctx, "while.body_bb", current_function);
        llvm::BasicBlock* after_bb = llvm::BasicBlock::Create(ctx, "while.after_bb", current_function);

        builder.CreateBr(preheader_bb);
        builder.SetInsertPoint(preheader_bb);
        builder.CreateBr(cond_bb);
        builder.SetInsertPoint(cond_bb);
        
        llvm::Value* condition = cast2llvmv node.condition->accept(*this);
        builder.CreateCondBr(condition, body_bb, after_bb); 
        
        builder.SetInsertPoint(body_bb);
        rn::for_each(node.body, [this](auto&& stmt){ stmt->accept(*this); });
        
        if (not builder.GetInsertBlock()->getTerminator()) builder.CreateBr(cond_bb);
        builder.SetInsertPoint(after_bb);
        
        return nullptr;
    }

    void* visit(Return_statement& node) override { 
        auto* val = cast2llvmv node.expression->accept(*this);
        builder.CreateRet(val);
        return nullptr; 
    }

    void* visit(Function_definition& node) override {
        llvm::Type* ret = llvm_type(node.return_type.type);
        auto a = node.arguments | rnv::transform([this](auto&& p){ return llvm_type(p->type.type); });
        std::vector<llvm::Type*> args(std::from_range, a);
        llvm::FunctionType* func_type = llvm::FunctionType::get(ret, args, node.variadic);
        current_function = llvm::Function::Create(func_type, llvm_linkage(node.var.linkage), node.var.name.content_str(), &module);

        if (node.var.linkage == Linkage_type::EXTERN) return nullptr;

        llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx, "entry", current_function);
        builder.SetInsertPoint(entry);

        local_variables.clear();
        rn::for_each(node.arguments | rnv::enumerate, [this](auto&& t){ 
            auto&& [num, arg] = t;
            auto* arg_ptr = cast2llvmv arg->accept(*this); 
            builder.CreateStore(current_function->getArg(num), arg_ptr);
        });

        rn::for_each(node.body, [this](auto&& stmt){ stmt->accept(*this); });
        return nullptr;
    }

    void* visit(Programm& node) override { 
        rn::for_each(node.globals_vars, [this](auto&& v){ v->accept(*this); });
        rn::for_each(node.functions, [this](auto&& f){ f->accept(*this); });
        return nullptr; 
    }

   private:
    std::unordered_map<const Variable_declaration_statement*, llvm::Value*> local_variables{};
    std::unordered_map<const Variable_declaration_statement*, llvm::Value*> global_symbols{};
    llvm::LLVMContext ctx{};
    llvm::IRBuilder<> builder{ctx};
    llvm::Module module;
    llvm::Function* current_function{nullptr};
    Expression* request_lvalue{nullptr}; // oops... design flaw
    std::ostream& out;
};

}  // namespace tc
