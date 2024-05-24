#include "type-pass.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm-c/BitWriter.h"

using namespace llvm;

PreservedAnalyses MenocatPass::run(Module &M, ModuleAnalysisManager &MPM) {
  Test t;
  t.printModule(M);
  t.getTypeString(M);
}

void Test::getTypeString(Module &M) {
  std::error_code EC;
  raw_fd_ostream filename("inst_types.txt", EC);

  for(Function& F : M) {
    for(BasicBlock& BB : F) {
      for(Instruction& I : BB) {
        Type* ty = I.getType();
        //outs() << *ty << "\n";
        //filename << I.getName() << "\n";
        //filename << (*ty) << "\n";
        if(I.getOpcode() == Instruction::Add) {
           filename << "Add: ";
           Value* op1 = I.getOperand(0);
           Value* op2 = I.getOperand(1);
           Type* op1_ty = op1->getType();
           Type* op2_ty = op2->getType();

           filename << *op1_ty << " ";
           filename << *op2_ty << "\n";
        }else if(I.getOpcode() == Instruction::Call) {
          filename << "Call: ";
          filename << *ty << "\n";
          if(auto* callInst = dyn_cast<CallInst>(&I)) {
            Function* called_f = callInst->getCalledFunction();
            if(called_f != 0) {
              filename << "Called Function name: " << called_f->getName().str();
              filename << " Function type: " << *(called_f->getType()) << "\n";
            }
          }
        } else {
          filename << "Other: ";
          filename << *ty << "\n";
        }
      }
    }
  }
}

//void Test::printString(std::string s) {
//  std::error_code EC;
//  raw_fd_ostream filename("inst_types.txt", EC);
//  filename << s;
//}
//

void Test::printModule(Module &M){
 std::error_code EC;
 raw_fd_ostream filename("output.ll", EC);
 M.print(filename, nullptr);
 filename.close();
}


llvm::PassPluginLibraryInfo getPassPluginInfo() {
   return {
   LLVM_PLUGIN_API_VERSION, "MENOCAT Type Pass", LLVM_VERSION_STRING,
   [](PassBuilder &PB) {
     PB.registerPipelineParsingCallback(
       [](StringRef Name, ModulePassManager &MPM,
         ArrayRef<PassBuilder::PipelineElement>) {
           if(Name == "type-pass") {
             MPM.addPass(MenocatPass());
             return true;
           }
           return false;
         }
     );
   }
 };
}

extern "C" LLVM_ATTRIBUTE_WEAK :: llvm ::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getPassPluginInfo();
}
