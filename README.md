# Live-Variables-analysis---Compiler
File LivenessAnalysis.cpp instruments the byte-code of input C programm to calculate the number of live variables at each program point.
File UninitializedVariables.cpp instruments the byte-code of input C program to find any uninitialized variables at each program point.
test1.c, test2.c, test3.c, test4.c are the testcases where each is a C program and test1.ll, test2.ll, test3.ll, test4.ll are LLVM instrumented byte code of the respective C programs.
