#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <memory>

using namespace llvm;

static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<IRBuilder<>> Builder; // Removed the comment from this line
static std::unique_ptr<Module> TheModule;

Function* createFunc(IRBuilder<>& builder, std::string name) {
  FunctionType* funcType = FunctionType::get(builder.getInt32Ty(), false);
  Function* fooFunc = Function::Create(funcType, Function::ExternalLinkage, name, TheModule.get());
  return fooFunc;
}

BasicBlock* createBB(Function* fooFunc, std::string name) {
  return BasicBlock::Create(*TheContext, name, fooFunc);
}

int main(int argc, char* argv[]) {
  TheContext = std::make_unique<LLVMContext>();
  Builder = std::make_unique<IRBuilder<>>(*TheContext);
  TheModule = std::make_unique<Module>("llvm_ess", *TheContext);

  Function* fooFunc = createFunc(*Builder, "foo");
  BasicBlock* entry = createBB(fooFunc, "entry");

  Builder->SetInsertPoint(entry);
  verifyFunction(*fooFunc);

  TheModule->print(errs(), nullptr);
  return 0;
}

