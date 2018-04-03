#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
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
#include <string>

using namespace std;
using namespace llvm;

namespace {
  struct LivenessAnalysis : public FunctionPass {
    static char ID;
    LivenessAnalysis() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
     
	int count1=0;
	map<string,int> dict;
	map<BasicBlock *,int> blockmap;
	map<Instruction *,int> instructionmap;
	map<int, Instruction *> instructionmaprev;
	map<int, string> dictrev;
	map<int, string>::iterator dictiter;
	map<int,Instruction *>::iterator institer;
	map<BasicBlock *,Instruction *> firstinst;
	queue<BasicBlock *> myqueue;
	list<BasicBlock *> mylist;

     for(auto& B : F)
     {
		 for(auto& I : B)
		 {
			 Instruction *instr6=&I;
			 std::string varname = instr6->getName().data();
	
			 if(instr6->getName().data() !=" ")
			 {
				 auto search=dict.find(varname);
				 if(search != dict.end()) {}
				 else
				 {
					dict.insert(dict.end(),pair<string,int>(instr6->getName().data(),count1));
					dictrev.insert(dictrev.end(),pair<int,string>(count1,instr6->getName().data()));
					count1++;
				 }
			 }	 
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
     bool allvars[count1];
     BasicBlock *bb;

	int numberofinstr=0;
	for(auto& B : F)
	{
		BasicBlock *bb1 = &B;
		for(auto& I : B)
		{
			auto search1 = firstinst.find(bb1);
			Instruction *inst3 = &I;
			
			if(search1 == firstinst.end())
			{
			//	errs()<<"inserting : "<<*inst3<<"\n";
				firstinst.insert(firstinst.begin(),pair<BasicBlock *,Instruction *>(bb1,inst3));
			}
			Instruction *inst1=&I;
		//	errs()<<*inst1<<":"<<numberofinstr<<"\n";
			instructionmap.insert(instructionmap.end(),pair<Instruction *,int>(inst1,numberofinstr));
			instructionmaprev.insert(instructionmaprev.end(),pair<int, Instruction *>(numberofinstr,inst1));
			numberofinstr++;
		//	errs()<<"instruction is : "<< *inst1<<"\n";
		}
		
    }

	int nblocks=0;
	for(auto& B : F)
	{
		
		BasicBlock *b1 = &B;
		mylist.insert(mylist.begin(), b1);
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
//	BasicBlock *b2 = mylist.front();
//	errs() << *b2;
	for(int i=0;i<numberofinstr;i++)
	for(int j=0;j<count1;j++)
	{
		def[i][j]=0;
		use[i][j]=0;
	}
	
   // finding the use arrays ----
 
	for(auto& B : F)
	{
	  BasicBlock *b4 = &B;
	  int blockno=blockmap.find(b4)->second;
		for(auto& I : B)
		{			
			Instruction *instr=&I;
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
			// finding definition arrays
		
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
				string varname2=instr->getOperand(1)->getName();
				string varname3=varname2.substr(0,varname2.find(".",0));
			
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
	

	bool status=false;
	
	int kk=0;
	do 
	{
		status=false;
	  for(auto& iter : mylist)
	  {
	     BasicBlock *b3=iter;
	     const TerminatorInst *TInst = b3->getTerminator();
		 int blocknob3=blockmap.find(b3)->second;
		
		// count no of instructions
		 int numberofinstthisblock=0;
		 for(auto& inst2 : *b3)
		 {   numberofinstthisblock++;  }
		
		stack<Instruction *> mystack;
		for(auto& inst3 : *b3)
		{  Instruction *inst4=&inst3;
			 mystack.push(inst4);
		}
		 while(!mystack.empty())
		 {
			 
			Instruction *inst1 = mystack.top();
			//b3->getInstList()[numberofinstthisblock];
		
			mystack.pop();	
			int inst1no=instructionmap.find(inst1)->second;
			
			for(int i=0;i<count1;i++)
			{
				temp1[i] = in[inst1no][i];
				temp2[i] = out[inst1no][i];
				result[i]=false;
			}
         if(inst1 == TInst)
         {
          for (unsigned I = 0, NSucc = TInst->getNumSuccessors(); I < NSucc; ++I) 
          {
			//  errs()<<NSucc;
            BasicBlock *Succ = TInst->getSuccessor(I);
            int blockno = blockmap.find(Succ)->second;
            Instruction *firstinstruction = firstinst.find(Succ)->second;
         //   errs()<<"first instructio is : "<<firstinstruction<<"\n"; 
            int instno = instructionmap.find(firstinstruction)->second;
          
            for(int j=0;j<count1;j++)
		    {
			  if(in[instno][j]==true || result[j]==true)
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
			 out[inst1no][i]=result[i];
		 }
	    }
	    else
	    {
		 for(int i=0;i<count1;i++)
         {
			 out[inst1no][i]=in[inst1no+1][i];
		 }
		}
	//	errs()<<"-----Done with use and def---\n";
		// condition for stability
		for(int i=0;i<count1;i++)
		{
			if(out[inst1no][i] != temp2[i])
			   status=true;
		}
		// to compute new IN
		 for(int i=0;i<count1;i++)
		 {
			 if(out[inst1no][i]==true && def[inst1no][i]==false)
			 {
				 temp3[i]=true;
			 }
			 else
				temp3[i]=false;
		 }
		 for(int i=0;i<count1;i++)
		 {
			 if(use[inst1no][i]==true || temp3[i]==true)
			 {
				 in[inst1no][i]=true;
			 }
			 else
				in[inst1no][i]=false;
		 }
		 // condition for stability
		 for(int i=0;i<count1;i++) 
		 {
		   if(in[inst1no][i] != temp1[i])
		      status=true;	 
		 }
	  }
	}
		

 }while(status);
	

	int livevalues[count1+1]={0};
	for(int i=0;i<numberofinstr;i++)
	{
		int live=0;
		for(int j=0;j<count1;j++)
		{
			if(out[i][j]==true)
			  live++;
		}
		livevalues[live]++;
	}

	 for(int i=0;i<numberofinstr;i++)
	 {
		 errs()<<*instructionmaprev.find(i)->second<<"\n";
		 for(int j=0;j<count1;j++)
		 {
			 if(out[i][j] == true)
			 {
				 errs()<<dictrev.find(j)->second<<", ";
			 }
		 }
		 errs()<<"\n";
	 }
	 int maxnonzero=0;
	 for(int i=0;i<count1+1;i++)
	 {
		 if(livevalues[i]!=0)
			maxnonzero=i;
	 }
	for(int i=0;i<=maxnonzero;i++)
	{
			errs()<<i << " : " <<livevalues[i]<<"\n";
		
	}
	// second part
	


	  
      return false;
    }
  };
}

char LivenessAnalysis::ID = 0;


static RegisterPass<LivenessAnalysis> X("liveness", "skeleton Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
// static void registerSkeletonPass(const PassManagerBuilder &,
   //                      legacy::PassManagerBase &PM) {
 // PM.add(new SkeletonPass());
//  }
// static RegisterStandardPasses
 // RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
// registerSkeletonPass);
