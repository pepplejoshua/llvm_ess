#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <vector>
#include <memory>

using namespace llvm;

static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<IRBuilder<>> Builder; // Removed the comment from this line
static std::unique_ptr<Module> TheModule;
static std::vector <std::string> f_args;

Function* createFunc(IRBuilder<>& builder, std::string name) {
  std::vector<Type*> IntegerArgs(f_args.size(), Builder->getInt32Ty());
  FunctionType* funcType = FunctionType::get(builder.getInt32Ty(), IntegerArgs, false);
  Function* fooFunc = Function::Create(funcType, Function::ExternalLinkage, name, TheModule.get());
  return fooFunc;
}

BasicBlock* createFuncBasicBlock(Function* fooFunc, std::string name) {
  return BasicBlock::Create(*TheContext, name, fooFunc);
}

GlobalVariable* createGlob(IRBuilder<>& builder, std::string name) {
  TheModule->getOrInsertGlobal(name, builder.getInt32Ty());
  GlobalVariable* gVar = TheModule->getNamedGlobal(name);
  gVar->setLinkage(GlobalValue::CommonLinkage);
  gVar->setAlignment(MaybeAlign(4));
  return gVar;
}

void setFuncArgs(Function* fn, std::vector<std::string> args) {
  unsigned idx = 0;
  Function::arg_iterator AI, AE;
  for (AI = fn->arg_begin(), AE = fn->arg_end(); AI != AE; ++AI, ++idx)
    AI->setName(args[idx]);
}

Value* createArith(IRBuilder<>& builder, Value* L, Value* R) {
  return builder.CreateMul(L, R, "multmp");
}

int main(int argc, char* argv[]) {
  f_args.push_back("a");
  f_args.push_back("b");
  TheContext = std::make_unique<LLVMContext>();
  Builder = std::make_unique<IRBuilder<>>(*TheContext);
  TheModule = std::make_unique<Module>("llvm_ess", *TheContext);

  GlobalVariable* gVar = createGlob(*Builder, "proto");
  Function* fooFunc = createFunc(*Builder, "foo");
  setFuncArgs(fooFunc, f_args);
  BasicBlock* entry = createFuncBasicBlock(fooFunc, "entry");

  Builder->SetInsertPoint(entry);
  Value* constant = Builder->getInt32(16);
  Value* arg_a = fooFunc->arg_begin();
  Value* res = createArith(*Builder, arg_a, constant);
  Value* constant_100 = Builder->getInt32(100);
  Value* compare = Builder->CreateICmpULT(arg_a, constant_100);
  Builder->CreateRet(res);
  verifyFunction(*fooFunc);

  TheModule->print(errs(), nullptr);
  return 0;
}

