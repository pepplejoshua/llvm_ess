#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <memory>
#include <vector>

using namespace llvm;

static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<IRBuilder<>>
Builder; // Removed the comment from this line
static std::unique_ptr<Module> TheModule;
static std::vector<std::string> f_args;

Function* createFunc(IRBuilder<>& builder, std::string name) {
  std::vector<Type*> IntegerArgs(f_args.size(), Builder->getInt32Ty());
  FunctionType* funcType =
    FunctionType::get(builder.getInt32Ty(), IntegerArgs, false);
  Function* fooFunc = Function::Create(funcType, Function::ExternalLinkage,
    name, TheModule.get());
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

typedef SmallVector<BasicBlock*, 16> BasicBlockList;
typedef SmallVector<Value*, 16> ValueList;

Value* createIfElse(IRBuilder<>& Builder, BasicBlockList L, ValueList VL) {
  Value* condition = VL[0];
  Value* arg_1 = VL[1];
  BasicBlock* then = L[0];
  BasicBlock* else_bb = L[1];
  BasicBlock* merge = L[2];
  Builder.CreateCondBr(condition, then, else_bb);

  Builder.SetInsertPoint(then);
  Value* then_val = Builder.CreateAdd(arg_1, Builder.getInt32(1), "thenaddtmp");
  Builder.CreateBr(merge);

  Builder.SetInsertPoint(else_bb);
  Value* else_val = Builder.CreateAdd(arg_1, Builder.getInt32(2), "elseaddtmp");
  Builder.CreateBr(merge);

  unsigned PhiBasicBlockSize = L.size() - 1;
  Builder.SetInsertPoint(merge);
  PHINode* Phi =
    Builder.CreatePHI(Builder.getInt32Ty(), PhiBasicBlockSize, "iftmp");
  Phi->addIncoming(then_val, then);
  Phi->addIncoming(else_val, else_bb);

  return Phi;
}

int main(int argc, char* argv[]) {
  f_args.push_back("a");
  f_args.push_back("b");
  TheContext = std::make_unique<LLVMContext>();
  Builder = std::make_unique<IRBuilder<>>(*TheContext);
  TheModule = std::make_unique<Module>("llvm_ess", *TheContext);

  // GlobalVariable* gVar = createGlob(*Builder, "proto");
  Function* fooFunc = createFunc(*Builder, "foo");
  setFuncArgs(fooFunc, f_args);
  BasicBlock* entry = createFuncBasicBlock(fooFunc, "entry");

  Builder->SetInsertPoint(entry);
  Value* constant = Builder->getInt32(16);
  Value* arg_a = fooFunc->arg_begin();
  // creates a * 16
  Value* res = createArith(*Builder, arg_a, constant);

  Value* constant_100 = Builder->getInt32(100);
  // creates a < 100
  Value* compare = Builder->CreateICmpULT(arg_a, constant_100, "cmptmp");
  // Zero extend compare so their types match
  Value* ze_compare = Builder->CreateZExt(compare, Builder->getInt32Ty(), "ze_cmptmp");
  // creates if (compare != 0)
  Value* condition =
    Builder->CreateICmpNE(ze_compare, Builder->getInt32(0), "if_cond");
  ValueList VL;
  VL.push_back(condition);
  VL.push_back(arg_a);

  BasicBlock* then = createFuncBasicBlock(fooFunc, "then");
  BasicBlock* else_bb = createFuncBasicBlock(fooFunc, "else");
  BasicBlock* merge = createFuncBasicBlock(fooFunc, "ifcont");
  BasicBlockList List;
  List.push_back(then);
  List.push_back(else_bb);
  List.push_back(merge);

  Value* if_else_phi_val = createIfElse(*Builder, List, VL);

  Builder->CreateRet(if_else_phi_val);
  // Builder->CreateRet(res);
  verifyFunction(*fooFunc);

  TheModule->print(errs(), nullptr);
  return 0;
}
