#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CFG.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstIterator.h"
#include <iostream>
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include <map>
#include <list>
#include <string>
#include <queue>
#include <cstdlib>
#include <functional>   // std::logical_and
#include <algorithm>
#include <stack>

using namespace std;
using namespace llvm;

namespace {
  struct UninitializedVariables : public FunctionPass {
    static char ID;
    UninitializedVariables() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
		
	int count1=0;
	map<string,int> dict;
	map<BasicBlock *,int> blockmap;
	map<Instruction *,int> instructionmap;
	map<int, Instruction *> instructionmaprev;
	map<int, string> dictrev;
	map<int, string>::iterator dictiter;
	map<int,Instruction *>::iterator institer;
	map<BasicBlock *,Instruction *> lastinst;
	queue<BasicBlock *> myqueue;
	list<BasicBlock *> mylist;
 
	 for(auto& B : F)
     {
		 for(auto& I : B)
		 {
			 Instruction *instr6=&I;
			 int numop = instr6-> getNumOperands();
			 for(int i=0;i<numop;i++)
			 {
				 string opname = instr6->getOperand(i)->getName();
				 auto search=dict.find(opname);
				 if(search != dict.end())
				 {}
				 else
					{
						dict.insert(dict.end(),pair<string,int>(opname,count1));
						dictrev.insert(dictrev.end(),pair<int,string>(count1,opname));
						count1 = count1 +1;
					}
			 }
		 }
	 }
	 int numberofinstr=0;
	 for(auto& B : F)
	 {
		 BasicBlock *bb=&B;

		 for(auto& I : B)
		 {
			 Instruction *inst1=&I;
			 instructionmap.insert(instructionmap.end(),pair<Instruction *,int>(inst1,numberofinstr));
			 instructionmaprev.insert(instructionmaprev.end(),pair<int, Instruction *>(numberofinstr,inst1));
			 numberofinstr++;
			 if(lastinst.find(bb)==lastinst.end())
			 {
				 lastinst.insert(lastinst.begin(), pair<BasicBlock *,Instruction *>(bb,inst1));
			 }
			 else
			 {
				 lastinst.erase(bb);
				 lastinst.insert(lastinst.begin(), pair<BasicBlock *,Instruction *>(bb,inst1));
			 }
		 }
	 }

	int nblocks=0;
	for(auto& B : F)
	{
		
		BasicBlock *b1 = &B;
		mylist.insert(mylist.end(), b1);
		blockmap.insert(blockmap.begin(), pair<BasicBlock *,int>(b1,nblocks));
		nblocks=nblocks+1;
	}
	bool out[numberofinstr][count1]={ {false} };
	bool in[numberofinstr][count1]={{false}};
	bool use[numberofinstr][count1]={{false}};
	bool def[numberofinstr][count1]={{false}};
    bool result[count1]={false};
    bool temp1[count1]={false};
    bool temp2[count1]={false};
    bool temp3[count1]={false};
    
	for(int i=0;i<numberofinstr;i++)
	for(int j=0;j<count1;j++)
	{
		in[i][j] =0;
		def[i][j]=0;
		use[i][j]=0;
		out[i][j]=true;
	}    
	 
	 for (auto& A : F.getArgumentList()) 
		{ string var=A.getName();
		   int index=dict.find(var)->second;
		   in[0][index]=true;
		}
		
	for(auto& B : F)
	{
		BasicBlock *bb1=&B;
		int blockno = blockmap.find(bb1)->second;
		for(auto& I : B)
		{
			Instruction *instr = &I;
			if(isa<AllocaInst>(instr))
			{
				continue;
			}
			int instrno = instructionmap.find(instr)->second;
			if(isa<ReturnInst>(instr))
			{
			
				if(instr->getOperand(0)->getName()!="")
				{
					int index=dict.find(instr->getOperand(0)->getName())->second;
					use[instrno][index] = true;
				}
			}
			if(isa<BranchInst>(instr))
			{
				int numop=instr->getNumOperands();
				if(numop==3)
				{
					int index=dict.find(instr->getOperand(0)->getName())->second;
					use[instrno][index] = true;
				}
				continue;
			}
			if(isa<CallInst>(instr))
			{
				int index = dict.find(instr->getName().data())->second;
			    def[instrno][index] = true;
			    unsigned numop=cast<CallInst>(instr)->getNumArgOperands();
			    for(int opnum=0;opnum<numop;++opnum)
			    {
					string varname2=cast<CallInst>(instr)->getArgOperand(opnum)->getName();
					index=dict.find(varname2)->second;
					use[instrno][index] = true;
				}
			    continue;
			}
			if(isa<LoadInst>(instr))
			{
				int index = dict.find(instr->getName().data())->second;
			    def[instrno][index] = true;
			    string varname2=instr->getOperand(0)->getName();
			    string varname3=varname2.substr(0,varname2.find(".",0));
			    int index2= dict.find(varname2)->second;
			    use[instrno][index2]=true;
			    continue;
			}
			if(instr->getName() != "")
			{
				int index = dict.find(instr->getName().data())->second;
			    def[instrno][index] = true;
			    
			    unsigned numberofoperands=instr->getNumOperands();
				for(int opno=0;opno<numberofoperands;++opno)
				{
					if(instr->getOperand(opno)->getName() != "")
					{
						int index2=dict.find(instr->getOperand(opno)->getName())->second;
						use[instrno][index2]=true;
					} 
				}    	
			}
			if(isa<StoreInst>(instr))
			{
				string varname2=instr->getOperand(1)->getName();//.substr(0,instr->getName().find('.',0));
				string varname3=varname2.substr(0,varname2.find(".",0));
			//	errs()<<"first operand is : "<<varname3<<"\n";
				int index = dict.find(varname2)->second;
				def[instrno][index] = true;
				
				if(instr->getOperand(0)->getName() != "")
				{
					int index2 = dict.find(instr->getOperand(0)->getName())->second;
					use[instrno][index2] = true;
				}
			}						
						
		}
	}
	

	int block;
	int kk=0;
	bool firstinst=true;
	bool status = false;
	do
	{
		block=0;
		status = false;
		for(auto& B : F)
		{
			BasicBlock *bb2 = &B;
			firstinst=true;
			for(auto& I : B)
			{
				Instruction *inst2 = &I;
				int inst2no = instructionmap.find(inst2)->second;
		//		errs()<<"instruction is :"<<inst2no<<"\n";
				for(int i=0;i<count1;i++)
				{
					temp1[i] = in[inst2no][i];
					temp2[i] = out[inst2no][i];
					
				}
				if(firstinst)
				{
					if(block==0)
					{
						for(int i=0;i<count1;i++)
						{
							in[inst2no][i]=false;
						}
						firstinst=false;
						for (auto& A : F.getArgumentList()) 
						{ 
							string var=A.getName();
							int index=dict.find(var)->second;
							in[inst2no][index]=true;
						}
					}
					else
					{
						for(int i=0;i<count1;i++)
						{
							result[i] = true;
						}
						for(BasicBlock *Pred : predecessors(bb2))
						{
						//	errs()<<*Pred<<"\n";
							Instruction *predinst;
							predinst=lastinst.find(Pred)->second;
							int predindex=instructionmap.find(predinst)->second;
							for(int j=0;j<count1;j++)
							{
								if(out[predindex][j]==true && result[j]==true)
								{
									result[j]=true;
								}
								else
								{
									result[j]=false;
								}
							}
						
						}
						for(int i=0;i<count1;i++)
						{
							in[inst2no][i] = result[i];
						}
						firstinst = false;
					}
				}
				else
				{
					for(int i=0;i<count1;i++)
					{
						in[inst2no][i]=out[inst2no-1][i];
					}
				}
				
				// out=in union def
				for(int i=0;i<count1;i++)
				{
					if(in[inst2no][i] || def[inst2no][i])
					{
						out[inst2no][i] = true;
					}
					else
						out[inst2no][i] = false;
				}
				
				// stability
				for(int i=0;i<count1;i++)
				{
					if(temp1[i]!=in[inst2no][i] || temp2[i]!=out[inst2no][i])
						status=true;
				}
			}
			block++;
		}

	}while(status);
	


	for(int i=0;i<numberofinstr;i++)
	{
		for(int j=0;j<count1;j++)
		{
			if(in[i][j]==false && use[i][j]==true)
			{
				errs()<<dictrev.find(j)->second<<" is used in "<<*instructionmaprev.find(i)->second<<"\n";
			}
		}
	}
   }
 };
}

char UninitializedVariables::ID = 0;


static RegisterPass<UninitializedVariables> X("uninitvars", "uninitialized Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
// static void registerPass(const PassManagerBuilder &,
   //                      legacy::PassManagerBase &PM) {
 // PM.add(new SkeletonPass());
//  }
// static RegisterStandardPasses
 // RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
// registerSkeletonPass);
