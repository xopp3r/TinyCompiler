module;

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

namespace {
constinit const char* MODULE_NAME = "The module";
}

namespace tc {

class Codegenerator : public I_ast_visitor {
   public:
    Codegenerator()
        : builder(std::make_unique<llvm::IRBuilder<>>(context)),
          module(std::make_unique<llvm::Module>(MODULE_NAME, context)) {}

    void* visit(Binary_operation& node) override {}

    void* visit(Unary_operation& node) override {}

    void* visit(Type_operation& node) override {}

    void* visit(Function_call& node) override {}

    void* visit(Integer_literal& node) override {}

    void* visit(String_literal& node) override {}

    void* visit(Char_literal& node) override {}

    void* visit(Variable& node) override {}

    void* visit(Expression_statement& node) override {}

    void* visit(Variable_declaration_statement& node) override {}

    void* visit(If_statement& node) override {}

    void* visit(While_statement& node) override {}

    void* visit(Return_statement& node) override {}

    void* visit(Function_definition& node) override {}

    void* visit(Programm& node) override {}

   private:
    llvm::LLVMContext context{};
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
};

}  // namespace tc
