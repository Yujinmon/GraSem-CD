#ifndef _TYPE_PASS_H_
#define _TYPE_PASS_H_

#include "llvm/Pass.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"

using namespace llvm;

std::string result;

struct Test : public PassInfoMixin<Test> {
  void printModule(Module &M);
  void getTypeString(Module &M); 
  void printString(std::string s);
};

struct MenocatPass : public PassInfoMixin<MenocatPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MPM);
};

#endif
