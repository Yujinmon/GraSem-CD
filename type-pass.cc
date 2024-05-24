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
  // std::error_code EC;
  // raw_fd_ostream filename("inst_types.txt", EC);

  result = "";

  for(Function& F : M) {
    for(BasicBlock& BB : F) {
      for(Instruction& I : BB) {
        std::string typeStr;
        raw_string_ostream rso(typeStr);

        Type* ty = I.getType();

        if(I.getOpcode() == Instruction::Ret){ // Ret: (if void) void (else) return_value's_type
          result += "Ret: ";
        
          if(auto* returnInst = dyn_cast<ReturnInst>(&I)) {
            Value* ret_value = returnInst->getReturnValue();

            if(ret_value) {
              ret_value->getType()->print(rso);
            }else{
              I.getType()->print(rso);
            }
          }

          result += typeStr + '\n'; 
        }else if(I.getOpcode() == Instruction::Br) { // Br: (if true/false exist) true_label_name false_label_name (else) branch_label_name
          result += "Br: ";

          if(auto* brInst = dyn_cast<BranchInst>(&I)) {
            typeStr += brInst->getSuccessor(0)->getName();
            if(brInst->isConditional()) {
              typeStr += " ";
              typeStr += brInst->getSuccessor(1)->getName();
            }
          }

          result += typeStr + '\n';
        }else if(I.getOpcode() == Instruction::Switch) {
          result += "Switch: ";
          
        }else if(I.getOpcode() == Instruction::IndirectBr) {
          result += "IndirectBr: ";

        }else if(I.getOpcode() == Instruction::Invoke) {
          result += "Invoke: ";

        }else if(I.getOpcode() == Instruction::Unreachable) {
          result += "Unreachable: ";

        } else if(I.getOpcode() == Instruction::Add) { // Add: op1_type op2_type
           result += "Add: ";

           Type* op1_ty = I.getOperand(0)->getType();
           Type* op2_ty = I.getOperand(1)->getType();
;
            op1_ty->print(rso);
            typeStr += " ";
            op2_ty->print(rso);

            result += typeStr + '\n';
        } else if(I.getOpcode() == Instruction::FAdd) {
            result += "FAdd: ";

            I.getOperand(0)->getType()->print(rso);
            typeStr += " ";
            I.getOperand(1)->getType()->print(rso);

            result += typeStr + '\n';
        } else if(I.getOpcode() == Instruction::Sub) {
            result += "Sub: ";

            I.getOperand(0)->getType()->print(rso);
            typeStr += " ";
            I.getOperand(1)->getType()->print(rso);

            result += typeStr + '\n';
        } else if(I.getOpcode() == Instruction::FSub) {
            result += "FSub: ";

            I.getOperand(0)->getType()->print(rso);
            typeStr += " ";
            I.getOperand(1)->getType()->print(rso);

            result += typeStr + '\n';
        } else if(I.getOpcode() == Instruction::Mul) {
            result += "Mul: ";

            I.getOperand(0)->getType()->print(rso);
            typeStr += " ";
            I.getOperand(1)->getType()->print(rso);

            result += typeStr + '\n';
        } else if(I.getOpcode() == Instruction::FMul) {
            result += "FMul: ";

            I.getOperand(0)->getType()->print(rso);
            typeStr += " ";
            I.getOperand(1)->getType()->print(rso);

            result += typeStr + '\n';
        }
        
        else if(I.getOpcode() == Instruction::Call) { // Call: funcion_name function_ret_type function_param_type
          result += "Call: ";
          
          if(auto* callInst = dyn_cast<CallInst>(&I)) { // checking called function 
            Function* called_f = callInst->getCalledFunction();
            if(called_f != 0) { 
              typeStr += called_f->getName().str();
              typeStr += " ";
              called_f->getType()->print(rso);
            }

            result += typeStr + '\n';
          }
        }
      }
    }
  }

  outs() << result;
}

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
