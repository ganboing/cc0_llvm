#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/ToolOutputFile.h>

using namespace llvm;

Module* make1() {
	Module* mod = new Module("sum.ll", getGlobalContext());
	mod->setDataLayout("e-p:64:64:64-i1:8:8-i8:8:8-"
			"i16:16:16-i32:32:32-i64:64:64-"
			"f32:32:32-f64:64:64-v64:64:64-"
			"v128:128:128-a0:0:64-s0:64:64-"
			"f80:128:128-n8:16:32:64-S128");
	mod->setTargetTriple("x86_64-pc-linux-gnu");
	SmallVector<Type*, 2> FuncTyArgs;
	FuncTyArgs.push_back(IntegerType::get(mod->getContext(), 32));
	FuncTyArgs.push_back(IntegerType::get(mod->getContext(), 32));
	FunctionType* FuncTy = FunctionType::get(
			IntegerType::get(mod->getContext(), 32), FuncTyArgs, false);
	Function* funcsum = Function::Create(FuncTy, GlobalValue::ExternalLinkage,
			"sum", mod);
	funcsum->setCallingConv(CallingConv::C);
	auto args = funcsum->arg_begin();
	Value* int32_a = args++;
	Value* int32_b = args++;
	int32_a->setName("a");
	int32_b->setName("b");
	BasicBlock* entryblock = BasicBlock::Create(mod->getContext(), "entry",
			funcsum, nullptr);
	AllocaInst* ptra = new AllocaInst(IntegerType::get(mod->getContext(), 32),
			"a.addr", entryblock);
	AllocaInst* ptrb = new AllocaInst(IntegerType::get(mod->getContext(), 32),
			"b.addr", entryblock);
	ptra->setAlignment(4);
	ptrb->setAlignment(4);
	StoreInst* st0 = new StoreInst(int32_a, ptra, false, entryblock);
	StoreInst* st1 = new StoreInst(int32_b, ptrb, false, entryblock);
	st0->setAlignment(4);
	st1->setAlignment(4);
	LoadInst* ld0 = new LoadInst(ptra, "", false, entryblock);
	LoadInst* ld1 = new LoadInst(ptrb, "", false, entryblock);
	ld0->setAlignment(4);
	ld1->setAlignment(4);
	BinaryOperator* addinst = BinaryOperator::Create(Instruction::Add, ld0, ld1,
			"add", entryblock);
	ReturnInst::Create(mod->getContext(), addinst, entryblock);
	return mod;
}

int main() {
	Module* mod = make1();
	verifyModule(*mod, &outs());
	std::string errinfo;
	std::unique_ptr<tool_output_file> Out(
			new tool_output_file("./sum.bc", errinfo, sys::fs::F_None));
	if(!errinfo.empty())
	{
		errs() << errinfo << "\n";
		return -1;
	}
	WriteBitcodeToFile(mod, Out->os());
	Out->keep();
	return 0;
}
