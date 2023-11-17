// Usage:  Instructing all comparison instruction
//
// Tested with LLVM Version: 3.9.1 / 7.0.1
//
// Basic usage of this file:
// g++/clang++ -c -fPIC -fno-rtti -std=c++11 `llvm-config --cxxflags` passModule.cpp -o passModule.o
// g++/clang++ -shared -fPIC -fno-rtti `llvm-config --ldflags` passModule.o -o libPassModule.so
// opt -S -load libPassModule.so -funcpass <original-ir.ll> -o <instructed-ir.ll>

#include "llvm/ADT/StringRef.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"

#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
// #include "llvm/Analysis/ValueLivenessAnalysis.h"

#include <cstdio>
#include <string>
#include <cassert>
#include <cstdint>
#include <vector>
// Headers for output InstIDList
#include <fstream>
// Headers for opcode
#include "opcode.h"

using namespace llvm;

namespace {
class MyFunctionPass : public FunctionPass {
// Some variables shared in class member functions.
private:
    // Created Constant.
    // IRBuilder CreateGlobalStringPtr() return Value* in llvm 3.9 / Constant* in latest version
    Value* moduleName;
    Value* functionName;

    // Created Function Declaration.
    // Module getOrInsertFunction() return Constant*.
    Constant* fCmpInstHandler;
    Constant* iCmpInstHandler;
    Constant* fAddHandler;
    Constant* fSubHandler;
    Constant* fMulHandler;
    Constant* fDivHandler;
    Constant* callOneArgHandler;
    Constant* callTwoArgsHandler;
    // C and C++ does not support '%' for floating point.
    // It is implemented in math.h as fmod(a,b) function;

    // Function modified.
    bool modified;

    // Modified Compare Instructions' ID List.
    std::vector<uint64_t> cmpInstIDList;
    // Instructed Binary Operation Instructions' ID List.
    std::vector<uint64_t> binInstIDList;
    std::vector<uint64_t> binInstOpList;
    // Instructed call instructions' ID List.
    std::vector<uint64_t> callInstIDList;
    std::vector<uint64_t> callinstOpList;
    // Instruction's debuginfo
    std::vector<uint64_t> calcInstIDList;
    std::vector<int32_t>  debugLineList;
    std::vector<int32_t>  debugColList;

    void initHandlersAndConstants(Function &func) {
        // Create constant string(char*) value for Module name and Function name.
        // Will be passed to handler function. See:
        // http://llvm.org/doxygen/classllvm_1_1IRBuilder.html
        // IRBuilder CreateGlobalStringPtr() return Value* in llvm 3.9 / Constant* in latest(8.0) version
        IRBuilder<> globalstrs(&(func.getEntryBlock()));
        this->functionName = globalstrs.CreateGlobalStringPtr(func.getName());
        this->moduleName   = globalstrs.CreateGlobalStringPtr(func.getParent()->getName());

        // Create Handler Function Declaration.
        // Module getOrInsertFunction() return Constant*.
        this->fCmpInstHandler = initFCmpInstHandler(func);
        this->iCmpInstHandler = initICmpInstHandler(func);
        this->fAddHandler = initFloatingPointOpHandler(func, Instruction::FAdd);
        this->fSubHandler = initFloatingPointOpHandler(func, Instruction::FSub);
        this->fMulHandler = initFloatingPointOpHandler(func, Instruction::FMul);
        this->fDivHandler = initFloatingPointOpHandler(func, Instruction::FDiv);
        this->callOneArgHandler  = initCallInstOneArgHandler(func);
        this->callTwoArgsHandler = initCallInstTwoArgsHandler(func);
        // Function modified.
        // return value for runOnFunction.
        this->modified = false;

        // Clear the InstIDList;
        this->cmpInstIDList.clear();
        this->binInstIDList.clear();
        this->binInstOpList.clear();
        this->callInstIDList.clear();
        this->callinstOpList.clear();
        this->calcInstIDList.clear();
        this->debugLineList.clear();
        this->debugColList.clear();

        return;
    }
    IRBuilder<> createBuilderAfter(Instruction *inst)
    {
        // Get next instruction
      BasicBlock::iterator tmpIt(inst);
      tmpIt++;
      Instruction *nextInst = &(*(tmpIt));
      assert(nextInst && "Invalid instruction!");

        IRBuilder<> builder(nextInst);

        return builder;
    }

    void catchBranchAndSelectInst(Instruction* pInst, LLVMContext& ctx) {
        Instruction &inst = *pInst;

        // Output BranchInst/SelectInst.
        #ifdef DEBUGINFO
        outs() << "Branch/Select Instruction:\n" << inst << '\n';
        #endif

        // Get the Condition Instruction.
        // We want to know where the comparison happens.
        // CmpInst has two derived classes: FCmpInst and ICmpInst, See:
        // http://llvm.org/doxygen/classllvm_1_1CmpInst.html
        CmpInst *cmpInst;

        // More about BranchInst, See:
        // http://llvm.org/doxygen/classllvm_1_1BranchInst.html
        if (isa<BranchInst>(inst)) {
            // outs() << "    ** Branch caught\n";
            BranchInst *branchInst = cast<BranchInst> ( &inst );
            // Ignore Unconditional
            if (branchInst->isUnconditional()) {
                // outs() << "    -- Unconditional\n";
                return;
            }

            cmpInst = cast<CmpInst> (branchInst->getCondition());
            #ifdef DEBUGINFO
            outs() << "    -- Condition: " << *cmpInst << '\n';
            #endif
        }
        // More about SelectInst, See:
        // http://llvm.org/doxygen/classllvm_1_1SelectInst.html
        if (isa<SelectInst>(inst)) {
            // outs() << "    ** Select caught\n";
            SelectInst *selectInst = cast<SelectInst> ( &inst );

            cmpInst = cast<CmpInst> (selectInst->getCondition());
            #ifdef DEBUGINFO
            outs() << "    -- Condition: " << *cmpInst << '\n';
            #endif
        }

        // Handling the Comparison Instruction (CmpInst).
        assert(cmpInst != NULL);

        // Handle CmpInst.
        // http://llvm.org/doxygen/classllvm_1_1CmpInst.html
        if (!isa<FCmpInst>(cmpInst) && !isa<ICmpInst>(cmpInst)) {
            // The branch or select inst's condition may be a 'xor' or 'phi' or etc..
            #ifdef DEBUGINFO
            outs() << "The condition is not a Comparison Instruction.\n";
            outs() << *cmpInst << '\n';
            #endif
            return;
        }

        // Whether FCmp, or ICmp.
        bool isFCmpInst = 0;
        if (isa<FCmpInst>(cmpInst)) {
            isFCmpInst = 1;
        }

        // Ignore the Pointer comparison instruction.
        if (isa<ICmpInst>(cmpInst)) {
            Value* tmp0 = cmpInst->getOperand(0);
            Value* tmp1 = cmpInst->getOperand(1);

            #ifdef DEBUGINFO
            outs() << "    ** Op1 is ptr: " << tmp0->getType()->isPointerTy() << "\n";
            outs() << "    ** Op2 is ptr: " << tmp1->getType()->isPointerTy() << "\n";
            #endif

            if (tmp0->getType()->isPointerTy()
                || tmp1->getType()->isPointerTy()) {
                return;
            }
        }

        // Now we have the CmpInst(cmpInst) and the BranchInst/SelectInst(inst).
        // We need to insert our own code between them.
        // Use IRBuilder class. See:
        // http://llvm.org/doxygen/classllvm_1_1IRBuilder.html
        // http://llvm.org/docs/ProgrammersManual.html#creating-and-inserting-new-instructions
        IRBuilder<> builder(&inst);

        // We want to:
        // 1. Operands:     Get the operands / the result value of CmpInst.
        // 2. Params:       Pass them to a handler function.
        // 3. Call Handler: Call the handler function.
        // 4. Modify Flag:  Modify the condition of BranchInst/SelectInst.

        // Step 1. Operands.
        Value* v0 = cmpInst->getOperand(0);
        Value* v1 = cmpInst->getOperand(1);
        // The CmpInst itself is an Value (Derived from Value)
        Value* cmpResult = cast<Value>(cmpInst);

        // Step 2. Params.
        std::vector<Value*> params;
        // FCmpInst
        if (isFCmpInst) {
            // If v0, v1 are floating point but not double.
            // Convert them to double type. See:
            // http://llvm.org/doxygen/classllvm_1_1Type.html
            // http://llvm.org/doxygen/classllvm_1_1CastInst.html
            if (v0->getType()->isFloatingPointTy() && !v0->getType()->isDoubleTy()) {
                v0 = builder.CreateFPCast(v0, Type::getDoubleTy(ctx));
                modified = true;
            }
            if (v1->getType()->isFloatingPointTy() && !v1->getType()->isDoubleTy()) {
                v1 = builder.CreateFPCast(v1, Type::getDoubleTy(ctx));
                modified = true;
            }
            params.push_back(v0);               // double
            params.push_back(v1);               // double
        }
        // ICmpInst
        else {
            // Convert v0, v1 to i64.
            // We use the Cast Function in IRBuilder.
            // But the detailed information can be found in CastInst:
            // http://llvm.org/doxygen/classllvm_1_1CastInst.html
            // The third param: bool isSigned.
            v0 = builder.CreateIntCast(v0, Type::getInt64Ty(ctx), true);
            v1 = builder.CreateIntCast(v1, Type::getInt64Ty(ctx), true);
            modified = true;
            params.push_back(v0);               // int64_t / i64
            params.push_back(v1);               // int64_t / i64
        }
        // The Compare Result.
        params.push_back(cmpResult);            // bool / i1
        // The Predicate.
        int32_t pred_int    = cmpInst->getPredicate();
        ConstantInt* pred   = ConstantInt::get(Type::getInt32Ty(ctx), pred_int);
        // Push predicate param.
        params.push_back(pred);                 // int32_t / i32

        // Step 2 Plus.
        // We also want the module/function name information.
        // Which are already stored in 'functionName' and 'moduleName'.
        // We also want the instruction's ID.
        uint64_t instID_int = reinterpret_cast<uint64_t>(cmpInst);
        ConstantInt* instID = ConstantInt::get(Type::getInt64Ty(ctx), instID_int);
        // Push params.
        params.push_back(functionName);         // char* / i8*
        params.push_back(moduleName);           // char* / i8*
        params.push_back(instID);               // uint64_t / i64
        // We also want the debug information.
        // If the target LLVM IR file (.ll/.bc) is compiled with '-g' flag.
        const DebugLoc &debugLoc = cmpInst->getDebugLoc();
        int32_t debugLine_int = 0;
        int32_t debugCol_int  = 0;
        // If the debug flag is on, the debugLoc will not be NULL.
        if (debugLoc) {
            debugLine_int = debugLoc.getLine(); // int32_t / i32
            debugCol_int  = debugLoc.getCol();  // int32_t / i32
        }
        ConstantInt* debugLine = ConstantInt::get(Type::getInt32Ty(ctx), debugLine_int);
        ConstantInt* debugCol  = ConstantInt::get(Type::getInt32Ty(ctx), debugCol_int);
        // Push params.
        params.push_back(debugLine);
        params.push_back(debugCol);

        // Step 3. Call handler.
        // handler function is stored in 'fCmpInstHandler'.
        CallInst* callInst;
        if (isFCmpInst) {
            callInst = builder.CreateCall(fCmpInstHandler, params);
        }
        else {
            callInst = builder.CreateCall(iCmpInstHandler, params);
        }
        modified = true;

        // Step 4. Make the branch/select inst to use modified results.
        // BranchInst / SelectInst: inst
        // modified results:        callInst
        // http://llvm.org/doxygen/classllvm_1_1BranchInst.html
        // http://llvm.org/doxygen/classllvm_1_1SelectInst.html
        if (isa<BranchInst>(inst)) {
            BranchInst *branchInst = cast<BranchInst> ( &inst );
            branchInst->setCondition(callInst);
        }
        if (isa<SelectInst>(inst)) {
            SelectInst *selectInst = cast<SelectInst> ( &inst );
            selectInst->setCondition(callInst);
        }

        // Record the InstID.
        cmpInstIDList.push_back(instID_int);

        // Try Print Some Debug Information
        if (debugLoc) {
            #ifdef DEBUGINFO
            outs() << "    ^^ Line:   " << debugLoc.getLine() << '\n';
            outs() << "    ^^ Column: " << debugLoc.getCol() << '\n';
            #endif
        }

        // outs() << "    == Operand 0: " << *v0 << '\n';
        // outs() << "    == Operand 0 Type: " << v0->getType()->getTypeID() << '\n';
        // outs() << "    == Operand 1: " << *v1 << '\n';
        // outs() << "    == Operand 1 Type: " << v1->getType()->getTypeID() << '\n';

        // outs() << "    ++ Try insert function call: " << callInst << '\n';
    }

    void catchBinaryOperatorInst(Instruction* pInst, LLVMContext& ctx) {
        Instruction &inst = *pInst;

        #ifdef DEBUGINFO
        outs() << "BinaryOperator Instruction:\n" << inst << '\n';
        #endif

        BinaryOperator* binaryOp = cast<BinaryOperator>(&inst);

        // Use opCode to determine the operation type, See:
        // https://llvm.org/doxygen/classllvm_1_1BinaryOperator.html
        Instruction::BinaryOps opCode = binaryOp->getOpcode();
        if (opCode == Instruction::FAdd
            || opCode == Instruction::FSub
            || opCode == Instruction::FMul
            || opCode == Instruction::FDiv)
        {
            #ifdef DEBUGINFO
            outs() << "  ** Floating-Point Operation Caught." << '\n';
            #endif
        }
        else {
            // Not Floating Point operation. Return.
            return;
        }

        // We want to insert a function call to our handler function.
        // Since IRBuilder only allow insert instruction (function call) before a specific instruction.
        // We cannot send the calculated results of FP operations.
        // IRBuilder<> builder(binaryOp);
        IRBuilder<> builder = createBuilderAfter(pInst);

        // We want to:
        // 1. Operands:     Get the operands
        // 2. Params:       Pass them to a handler functionn.
        // 3. Call Handler: Call the handler function.

        // Step 1. Operands.
        Value* v0 = binaryOp->getOperand(0);
        Value* v1 = binaryOp->getOperand(1);
        Value* v2 = cast<Value>(pInst);
        // v0 = cast<Value>(pInst);
        uint64_t v0_int = reinterpret_cast<uint64_t>(v0);
        uint64_t v1_int = reinterpret_cast<uint64_t>(v1);
        ConstantInt* v0ID;
        ConstantInt* v1ID;
        if(isa<ConstantFP>(v0)){
             v0ID = ConstantInt::get(Type::getInt64Ty(ctx), 0);
        }else{
             v0ID = ConstantInt::get(Type::getInt64Ty(ctx), v0_int);
        }
        if(isa<ConstantFP>(v1)){
             v1ID = ConstantInt::get(Type::getInt64Ty(ctx), 0);
        }else{
             v1ID = ConstantInt::get(Type::getInt64Ty(ctx), v1_int);
        }

        // Step 2. Params.
        std::vector<Value*> params;
        // If v0, v1 are floating-point but not double.
        // Convert them to double type. See:
        // http://llvm.org/doxygen/classllvm_1_1Type.html
        // http://llvm.org/doxygen/classllvm_1_1CastInst.html
        if (v0->getType()->isFloatingPointTy() && !v0->getType()->isDoubleTy()) {
            v0 = builder.CreateFPCast(v0, Type::getDoubleTy(ctx));
            // v0 = cast<Value>(pInst);
            modified = true;
        }
        if (v1->getType()->isFloatingPointTy() && !v1->getType()->isDoubleTy()) {
            v1 = builder.CreateFPCast(v1, Type::getDoubleTy(ctx));
            modified = true;
        }
        if (v2->getType()->isFloatingPointTy() && !v2->getType()->isDoubleTy()) {
            v2 = builder.CreateFPCast(v2, Type::getDoubleTy(ctx));
            modified = true;
        }
        params.push_back(v0);               // double
        params.push_back(v1);               // double
        params.push_back(v2);               // double
        params.push_back(v0ID);           // uint64_t / i64
        params.push_back(v1ID);           // uint64_t / i64

        // Step 2 Plus.
        // We also want the module/function name information.
        // Which are already stored in 'functionName' and 'moduleName'.
        // We also want the instruction's ID.
        uint64_t instID_int = reinterpret_cast<uint64_t>(binaryOp);
        ConstantInt* instID = ConstantInt::get(Type::getInt64Ty(ctx), instID_int);
        // Push params.
        params.push_back(functionName);     // char* / i8*
        params.push_back(moduleName);       // char* / i8*
        params.push_back(instID);           // uint64_t / i64
        // We also want the debug information.
        // If the target LLVM IR file (.ll/.bc) is compiled with '-g' flag.
        const DebugLoc &debugLoc = pInst->getDebugLoc();
        int32_t debugLine_int = 0;
        int32_t debugCol_int  = 0;
        // If the debug flag is on, the debugLoc will not be NULL.
        if (debugLoc) {
            debugLine_int = debugLoc.getLine(); // int32_t / i32
            debugCol_int  = debugLoc.getCol();  // int32_t / i32
        }
        ConstantInt* debugLine = ConstantInt::get(Type::getInt32Ty(ctx), debugLine_int);
        ConstantInt* debugCol  = ConstantInt::get(Type::getInt32Ty(ctx), debugCol_int);
        // Push params.
        params.push_back(debugLine);
        params.push_back(debugCol);

        // Step 3. Call handler.
        // handler function is stored in fAddHandler, fSubHandler...
        CallInst* callInst;
        if (opCode == Instruction::FAdd) {
            callInst = builder.CreateCall(fAddHandler, params);
        }
        if (opCode == Instruction::FSub) {
            callInst = builder.CreateCall(fSubHandler, params);
        }
        if (opCode == Instruction::FMul) {
            callInst = builder.CreateCall(fMulHandler, params);
        }
        if (opCode == Instruction::FDiv) {
            callInst = builder.CreateCall(fDivHandler, params);
        }
        modified = true;

        // Record the InstID.
        binInstIDList.push_back(instID_int);
        // Recode the InstOp.
        binInstOpList.push_back(opCode);
        // Recode debug info.
        calcInstIDList.push_back(instID_int);
        debugLineList.push_back(debugLine_int);
        debugColList.push_back(debugCol_int);

        // Try Print Some Debug Information
        if (debugLoc) {
            #ifdef DEBUGINFO
            outs() << "    ^^ Line:   " << debugLoc.getLine() << '\n';
            outs() << "    ^^ Column: " << debugLoc.getCol() << '\n';
            #endif
        }
    }

    void catchCallInst(Instruction* pInst, LLVMContext& ctx) {
        Instruction &inst = *pInst;

        #ifdef DEBUGINFO
        outs() << "CallInst:\n" << inst << '\n';
        #endif

        CallInst* callInst = cast<CallInst>(&inst);

        std::string calledFunctionName = callInst->getCalledFunction()->getName();

        // Basic functions in math.h
        static const std::map<std::string, int> functionMap {
            {"sin", OP_SIN},
            {"cos", OP_COS},
            {"tan", OP_TAN},
            {"asin", OP_ASIN},
            {"acos", OP_ACOS},
            {"atan", OP_ATAN},
            {"atan2", OP_ATAN2},
            {"sinh", OP_SINH},
            {"cosh", OP_COSH},
            {"tanh", OP_TANH},
            {"exp", OP_EXP},
            {"log", OP_LOG},
            {"log10", OP_LOG10},
            {"sqrt", OP_SQRT},
            {"pow", OP_POW},
        };

        bool caught = false;
        int functionID_int = 0;
        auto it = functionMap.find(calledFunctionName);
        if (it == functionMap.end()) {
            // Not Basic function call in math.h
            return;
        }
        caught = true;
        functionID_int = it->second;
        #ifdef DEBUGINFO
        outs() << "  ** Basic Function Call Caught.\n";
        #endif

        #ifdef DEBUGINFO
        outs() << "  Called Funtion: " << calledFunctionName << '\n';
        outs() << "  Args size:" << callInst->getNumArgOperands() << '\n';
        #endif

        // We want to insert a function call to our handler function.
        // IRBuilder<> builder(callInst);
        IRBuilder<> builder = createBuilderAfter(pInst);

        // Currently the function we handle only have one or two params.
        uint32_t argNumber = callInst->getNumArgOperands();
        Value* v0;
        Value* v1;
        Value* v2 = cast<Value>(pInst);
        // ConstantInt* v0ID; 
        // ConstantInt* v1ID; 
        std::vector<Value*> params;
        ConstantInt* functionID = ConstantInt::get(Type::getInt32Ty(ctx), functionID_int);
        // 0. The functionID.
        params.push_back(functionID);       // int32_t / i32
        ConstantInt* v0ID;
        ConstantInt* v1ID;
        if (argNumber == 1) {
            v0 = callInst->getArgOperand(0);
            uint64_t v0_int = reinterpret_cast<uint64_t>(v0);
            //ConstantInt* v0ID = ConstantInt::get(Type::getInt64Ty(ctx), v0_int);
            if(isa<ConstantFP>(v0)){
                 v0ID = ConstantInt::get(Type::getInt64Ty(ctx), 0);
            }else{
                 v0ID = ConstantInt::get(Type::getInt64Ty(ctx), v0_int);
            }
            params.push_back(v0);           // double
            params.push_back(v2);           // double
            params.push_back(v0ID);           // double
        }
        else if (argNumber == 2) {
            v0 = callInst->getArgOperand(0);
            v1 = callInst->getArgOperand(1);
            uint64_t v0_int = reinterpret_cast<uint64_t>(v0);
            uint64_t v1_int = reinterpret_cast<uint64_t>(v1);
            if(isa<ConstantFP>(v0)){
                 v0ID = ConstantInt::get(Type::getInt64Ty(ctx), 0);
            }else{
                 v0ID = ConstantInt::get(Type::getInt64Ty(ctx), v0_int);
            }
            if(isa<ConstantFP>(v1)){
                 v1ID = ConstantInt::get(Type::getInt64Ty(ctx), 0);
            }else{
                 v1ID = ConstantInt::get(Type::getInt64Ty(ctx), v1_int);
            }
            //ConstantInt* v0ID = ConstantInt::get(Type::getInt64Ty(ctx), v0_int);
            //ConstantInt* v1ID = ConstantInt::get(Type::getInt64Ty(ctx), v1_int);
            params.push_back(v0);           // double
            params.push_back(v1);           // double
            params.push_back(v2);           // double
            params.push_back(v0ID);           // double
            params.push_back(v1ID);           // double
        }
        else {
            errs() << "Cannot Handle This Function: Wrong Args Number.\n";
            return;
        }



        // 2. ModuleName, FunctionName, instID.
        uint64_t instID_int = reinterpret_cast<uint64_t>(callInst);
        ConstantInt* instID = ConstantInt::get(Type::getInt64Ty(ctx), instID_int);
        // Push params.
        params.push_back(functionName);     // char* / i8*
        params.push_back(moduleName);       // char* / i8*
        params.push_back(instID);           // uint64_t / i64

        // 3. Debug information.
        // If the target LLVM IR file (.ll/.bc) is compiled with '-g' flag.
        const DebugLoc &debugLoc = pInst->getDebugLoc();
        int32_t debugLine_int = 0;
        int32_t debugCol_int  = 0;
        // If the debug flag is on, the debugLoc will not be NULL.
        if (debugLoc) {
            debugLine_int = debugLoc.getLine(); // int32_t / i32
            debugCol_int  = debugLoc.getCol();  // int32_t / i32
        }
        ConstantInt* debugLine = ConstantInt::get(Type::getInt32Ty(ctx), debugLine_int);
        ConstantInt* debugCol  = ConstantInt::get(Type::getInt32Ty(ctx), debugCol_int);
        // Push params.
        params.push_back(debugLine);
        params.push_back(debugCol);

        // Call handler.
        CallInst* handlerCall;
        if (argNumber == 1) {
            handlerCall = builder.CreateCall(callOneArgHandler, params);
        }
        else {
            handlerCall = builder.CreateCall(callTwoArgsHandler, params);
        }
        modified = true;

        // Record the InstID.
        callInstIDList.push_back(instID_int);
        // Record the FunctionID.
        callinstOpList.push_back(functionID_int);
        // Record debug info.
        calcInstIDList.push_back(instID_int);
        debugLineList.push_back(debugLine_int);
        debugColList.push_back(debugCol_int);

        // Try Print Some Debug Information
        if (debugLoc) {
            #ifdef DEBUGINFO
            outs() << "    ^^ Line:   " << debugLoc.getLine() << '\n';
            outs() << "    ^^ Column: " << debugLoc.getCol() << '\n';
            #endif
        }
    }

public:
    static char ID;
    MyFunctionPass () : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &func) {
        // Init some shared variables.
        this->initHandlersAndConstants(func);

        // Reference's declaration must come with definition.
        // we cannot put ctx in init function.
        LLVMContext &ctx = func.getContext();

        // Output Function information.
        outs() << "  Module: " << func.getParent()->getName()
            << "  Function: " << func.getName() << '\n';

        // The handler function.
        #ifdef DEBUGINFO
        outs() << "fCmpHandler: " << fCmpInstHandler << '\n';
        outs() << "iCmpHandler: " << iCmpInstHandler << '\n';
        #endif

        // Traverse every BasicBlock.
        for (auto bb_it = func.begin(), bb_it_end = func.end(); bb_it != bb_it_end; ++bb_it) {
            BasicBlock &bb = *bb_it;
            // Output BasicBlock separation.
            // outs() << "\n";
            // Traverse every Instruction.
            for (auto inst_it = bb.begin(), inst_it_end = bb.end(); inst_it != inst_it_end; ++inst_it) {
                Instruction &inst = *inst_it;
                // Output All Instruction.
                // outs() << "Instruction: " << inst << '\n';

                // Check Instruction type: isa<>.
                // Convert instruction class to derived classes: cast<>
                //
                // More about the isa<> and cast<>, See:
                // https://releases.llvm.org/3.9.1/docs/ProgrammersManual.html#the-isa-cast-and-dyn-cast-templates
                //
                // More about Instruction class and derived classes, See:
                // http://llvm.org/doxygen/classllvm_1_1Instruction.html

                // Handle BranchInst and SelectInst.
                if (isa<BranchInst>(inst) || isa<SelectInst>(inst)) {
                    catchBranchAndSelectInst(&inst, ctx);
                }
                // Handle BinaryOperator.
                // We now focus on: fadd, fsub, fmul, and fdiv.
                if (isa<BinaryOperator>(inst)) {
                    catchBinaryOperatorInst(&inst, ctx);
                }
                // Handle functionCalls.
                // We also focus on some basic function, such as log, sin, cos..
                if (isa<CallInst>(inst)) {
                    catchCallInst(&inst, ctx);
                }

            } // Instruction Loop Finish.
        } // BasicBlock Loop Finish.

        // Write InstIDList to file.
        writeInstIDList(func);

        return modified;
    } // runOnFunction Finish.

private:
    // Some utility function.
    // Just to make the runOnFunction more clear.

    Constant* initFCmpInstHandler(Function &func) {
        // Get the Function* to Predicate Handler Function.
        // Use Module's 'getOrInsertFunction' to create/get handler function.
        LLVMContext &ctx = func.getContext();
        Module* module   = func.getParent();
        // Handler function name.
        std::string fCmpHandlerName = "fCmpInstHandler";
        // FunctionType consists of returnType and paramType
        // Return Type
        Type* fCmpHandlerRetType = Type::getInt1Ty(ctx);            // NewCMP: bool / i1
        // Matches the handlers functions in src/handlers.cpp
        // Param Types: FCmpInst
        std::vector<Type*> fCmpHandlerParamTypes;
        fCmpHandlerParamTypes.push_back(Type::getDoubleTy(ctx));    // LHS:    double   / double
        fCmpHandlerParamTypes.push_back(Type::getDoubleTy(ctx));    // RHS:    double   / double
        fCmpHandlerParamTypes.push_back(Type::getInt1Ty(ctx));      // CMP:    bool     / i1
        fCmpHandlerParamTypes.push_back(Type::getInt32Ty(ctx));     // PRED:   int32_t  / i32
        fCmpHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));   // FNAME:  char*    / i8*
        fCmpHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));   // MNAME:  char*    / i8*
        fCmpHandlerParamTypes.push_back(Type::getInt64Ty(ctx));     // INSTID: uint64_t / i64
        fCmpHandlerParamTypes.push_back(Type::getInt32Ty(ctx));     // LINE:   int32_t  / i32
        fCmpHandlerParamTypes.push_back(Type::getInt32Ty(ctx));     // COL:    int32_t  / i32
        // Function Type
        FunctionType* fCmpHandlerFuncType = FunctionType::get(
            fCmpHandlerRetType,
            fCmpHandlerParamTypes,
            false);
        // getOrInsertFunction. See:
        // http://llvm.org/doxygen/classllvm_1_1Module.html#af36d72ed01ad0bc227d690058d59c0ef
        //Constant* handler = module->getOrInsertFunction(
        //                                        fCmpHandlerName,
        //                                        fCmpHandlerFuncType);
        Constant* handler = module->getOrInsertFunction(
                                                fCmpHandlerName,
                                                fCmpHandlerFuncType);
        //auto handler = dyn_cast<Constant>(handler_f.getCallee());
        return handler;
    }

    Constant* initICmpInstHandler(Function &func) {
        // Get the Function* to Predicate Handler Function.
        // Use Module's 'getOrInsertFunction' to create/get handler function.
        LLVMContext &ctx = func.getContext();
        Module* module   = func.getParent();
        // Handler function name.
        std::string iCmpHandlerName = "iCmpInstHandler";
        // FunctionType consists of returnType and paramType
        // Return Type
        Type* iCmpHandlerRetType = Type::getInt1Ty(ctx);            // NewCMP: bool / i1
        // Matches the handlers functions in src/handlers.cpp
        // Param Types: ICmpInst
        std::vector<Type*> iCmpHandlerParamTypes;
        iCmpHandlerParamTypes.push_back(Type::getInt64Ty(ctx));     // LHS:    int64_t  / i64
        iCmpHandlerParamTypes.push_back(Type::getInt64Ty(ctx));     // RHS:    int64_t  / i64
        iCmpHandlerParamTypes.push_back(Type::getInt1Ty(ctx));      // CMP:    bool     / i1
        iCmpHandlerParamTypes.push_back(Type::getInt32Ty(ctx));     // PRED:   int32_t  / i32
        iCmpHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));   // FNAME:  char*    / i8*
        iCmpHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));   // MNAME:  char*    / i8*
        iCmpHandlerParamTypes.push_back(Type::getInt64Ty(ctx));     // INSTID: uint64_t / i64
        iCmpHandlerParamTypes.push_back(Type::getInt32Ty(ctx));     // LINE:   int32_t  / i32
        iCmpHandlerParamTypes.push_back(Type::getInt32Ty(ctx));     // COL:    int32_t  / i32
        // Function Type
        FunctionType* iCmpHandlerFuncType = FunctionType::get(
            iCmpHandlerRetType,
            iCmpHandlerParamTypes,
            false);
        // getOrInsertFunction. See:
        // http://llvm.org/doxygen/classllvm_1_1Module.html#af36d72ed01ad0bc227d690058d59c0ef
        Constant* handler = module->getOrInsertFunction(
                                                iCmpHandlerName,
                                                iCmpHandlerFuncType);
        //auto handler = dyn_cast<Constant>(handler_f.getCallee());
        return handler;
    }

    Constant* initFloatingPointOpHandler(Function &func, Instruction::BinaryOps opCode) {
        // Get the Function* to 'fadd' Handler Function.
        // Use Module's 'getOrInsertFunction' to create/get handler function.
        LLVMContext &ctx = func.getContext();
        Module* module   = func.getParent();
        // Handler function name.
        std::string floatingPointOpHandlerName;
        if (opCode == Instruction::FAdd) {
            floatingPointOpHandlerName = "fAddHandler";
        }
        if (opCode == Instruction::FSub) {
            floatingPointOpHandlerName = "fSubHandler";
        }
        if (opCode == Instruction::FMul) {
            floatingPointOpHandlerName = "fMulHandler";
        }
        if (opCode == Instruction::FDiv) {
            floatingPointOpHandlerName = "fDivHandler";
        }
        // FunctionType consists of returnType and paramType
        // Return Type
        Type* floatingPointOpHandlerRetType = Type::getVoidTy(ctx);            // void
        // Matches the handlers functions in src/handlers.cpp
        // Param Types: BinaryOperator
        std::vector<Type*> floatingPointOpHandlerParamTypes;
        floatingPointOpHandlerParamTypes.push_back(Type::getDoubleTy(ctx)); // LHS:    double / double
        floatingPointOpHandlerParamTypes.push_back(Type::getDoubleTy(ctx)); // RHS:    double / double
        floatingPointOpHandlerParamTypes.push_back(Type::getDoubleTy(ctx)); // RHS:    double / double
        floatingPointOpHandlerParamTypes.push_back(Type::getInt64Ty(ctx));  // INSTID: uint64_t / i64
        floatingPointOpHandlerParamTypes.push_back(Type::getInt64Ty(ctx));  // INSTID: uint64_t / i64
        floatingPointOpHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));// FNAME:  char*    / i8*
        floatingPointOpHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));// MNAME:  char*    / i8*
        floatingPointOpHandlerParamTypes.push_back(Type::getInt64Ty(ctx));  // INSTID: uint64_t / i64
        floatingPointOpHandlerParamTypes.push_back(Type::getInt32Ty(ctx));  // LINE:   int32_t  / i32
        floatingPointOpHandlerParamTypes.push_back(Type::getInt32Ty(ctx));  // COL:    int32_t  / i32
        // Function Type
        FunctionType* floatingPointOpHandlerFuncType = FunctionType::get(
            floatingPointOpHandlerRetType,
            floatingPointOpHandlerParamTypes,
            false);
        // getOrInsertFunction. See:
        // http://llvm.org/doxygen/classllvm_1_1Module.html#af36d72ed01ad0bc227d690058d59c0ef
        Constant* handler = module->getOrInsertFunction(
                                                floatingPointOpHandlerName,
                                                floatingPointOpHandlerFuncType);
        //auto handler = dyn_cast<Constant>(handler_f.getCallee());
        return handler;
    }

    Constant* initCallInstOneArgHandler(Function &func) {
        LLVMContext &ctx = func.getContext();
        Module* module   = func.getParent();
        // Name
        std::string callOneArgHandlerName = "callOneArgHandler";
        // Function Type
        //  - return type
        Type* callOneArgHandlerRetType = Type::getVoidTy(ctx);
        //  - params type
        std::vector<Type*> callOneArgHandlerParamTypes;
        callOneArgHandlerParamTypes.push_back(Type::getInt32Ty(ctx));
        callOneArgHandlerParamTypes.push_back(Type::getDoubleTy(ctx));
        callOneArgHandlerParamTypes.push_back(Type::getDoubleTy(ctx));
        callOneArgHandlerParamTypes.push_back(Type::getInt64Ty(ctx));
        callOneArgHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));
        callOneArgHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));
        callOneArgHandlerParamTypes.push_back(Type::getInt64Ty(ctx));
        callOneArgHandlerParamTypes.push_back(Type::getInt32Ty(ctx));
        callOneArgHandlerParamTypes.push_back(Type::getInt32Ty(ctx));
        // Function Type
        FunctionType* callOneArgHandlerFuncType = FunctionType::get(
            callOneArgHandlerRetType,
            callOneArgHandlerParamTypes,
            false);
        // getOrInsertFunction. See:
        // http://llvm.org/doxygen/classllvm_1_1Module.html#af36d72ed01ad0bc227d690058d59c0ef
        Constant* handler = module->getOrInsertFunction(
            callOneArgHandlerName,
            callOneArgHandlerFuncType);
        //auto handler = dyn_cast<Constant>(handler_f.getCallee());
        return handler;
    }

    Constant* initCallInstTwoArgsHandler(Function &func) {
        LLVMContext &ctx = func.getContext();
        Module* module   = func.getParent();
        // Name
        std::string callTwoArgsHandlerName = "callTwoArgsHandler";
        // Function Type
        //  - return type
        Type* callTwoArgsHandlerRetType = Type::getVoidTy(ctx);
        //  - params type
        std::vector<Type*> callTwoArgsHandlerParamTypes;
        callTwoArgsHandlerParamTypes.push_back(Type::getInt32Ty(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getDoubleTy(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getDoubleTy(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getDoubleTy(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getInt64Ty(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getInt64Ty(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getInt8PtrTy(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getInt64Ty(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getInt32Ty(ctx));
        callTwoArgsHandlerParamTypes.push_back(Type::getInt32Ty(ctx));
        // Function Type
        FunctionType* callTwoArgsHandlerFuncType = FunctionType::get(
            callTwoArgsHandlerRetType,
            callTwoArgsHandlerParamTypes,
            false);
        // getOrInsertFunction. See:
        // http://llvm.org/doxygen/classllvm_1_1Module.html#af36d72ed01ad0bc227d690058d59c0ef
        Constant* handler = module->getOrInsertFunction(
            callTwoArgsHandlerName,
            callTwoArgsHandlerFuncType);
        //auto handler = dyn_cast<Constant>(handler_f.getCallee());
        return handler;
    }

    void writeInstIDList(Function &func) {
        // Write InstIDList to file.
        std::ofstream fout;
        // Current targetFileName: build/target.ll
        // We want: target
        std::string targetFileName = func.getParent()->getName().str();
        std::size_t truncPos = targetFileName.find('.');
        // About C++ string::substr :
        // http://www.cplusplus.com/reference/string/string/substr/
        // Remove ".ll"
        if (truncPos != std::string::npos) {
            targetFileName = targetFileName.substr(0, truncPos);
        }
        // Remove "build/"
        truncPos = targetFileName.find('/');
        if (truncPos != std::string::npos) {
            targetFileName = targetFileName.substr(truncPos+1);
        }
        std::string cmpFileName  = "data/" + targetFileName + ".cmpinst";
        std::string binFileName  = "data/" + targetFileName + ".bininst";
        std::string callFileName = "data/" + targetFileName + ".callinst";
        std::string debugFileName= "data/" + targetFileName + ".debuginfo";
        // Make Directory First.
        system("mkdir -p data");

        // Write Compare Inst.
        #ifdef DEBUGINFO
        outs() << "---\nWriting cmpInstIDList to: " << cmpFileName << '\n';
        #endif
        fout.open(cmpFileName, std::ofstream::out | std::ofstream::app);
        if (!fout.is_open()) {
            errs() << "Open File Failed: " << cmpFileName << '\n';
        }
        for (std::size_t i = 0; i < cmpInstIDList.size(); i++) {
            fout << std::hex << cmpInstIDList[i] << '\n';
        }
        fout.close();

        // Write Binary Inst.
        #ifdef DEBUGINFO
        outs() << "---\nWriting binInst Info to:" << binFileName << '\n';
        #endif
        fout.open(binFileName, std::ofstream::out | std::ofstream::app);
        if (!fout.is_open()) {
            errs() << "Open File Failed:" << binFileName << '\n';
        }
        for (std::size_t i = 0; i < binInstIDList.size(); i++) {
            fout << std::hex << binInstIDList[i] << std::dec << ' ' << binInstOpList[i] << '\n';
        }
        // Output the Opcode for +-*/
        #ifdef DEBUGINFO
        outs() << "+: " << Instruction::FAdd << '\n'
            << "-: " << Instruction::FSub << '\n'
            << "*: " << Instruction::FMul << '\n'
            << "\\: " << Instruction::FDiv << '\n';
        #endif
        fout.close();

        // Write Call Inst.
        #ifdef DEBUGINFO
        outs() << "---\nWriting callInst Info to:" << callFileName << '\n';
        #endif
        fout.open(callFileName, std::ofstream::out | std::ofstream::app);
        if (!fout.is_open()) {
            errs() << "Open File Failed:" << callFileName << '\n';
        }
        for (std::size_t i = 0; i < callInstIDList.size(); i++) {
            fout << std::hex << callInstIDList[i] << std::dec << ' ' << callinstOpList[i] << '\n';
        }
        fout.close();

        // Write debuginfo.
        #ifdef DEBUGINFO
        outs() << "---\nWriting Debug Info to:" << debugFileName << '\n';
        #endif
        fout.open(debugFileName, std::ofstream::out | std::ofstream::app);
        if (!fout.is_open()) {
            errs() << "Open File Failed:" << debugFileName << '\n';
        }
        for (std::size_t i = 0; i < calcInstIDList.size(); i++) {
            fout << std::dec << calcInstIDList[i] << ", Line: " << debugLineList[i] << ", Col: " << debugColList[i] << '\n';
        }
        fout.close();
    }
};
}

char MyFunctionPass::ID = 0;

static RegisterPass<MyFunctionPass> FP ("funcpass", "My Function Pass");
