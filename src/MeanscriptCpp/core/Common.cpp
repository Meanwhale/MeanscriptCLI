
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

// private static data. common for all MeanScript objects

void Common::printCallbacks ()
{
	VERBOSE("-------- CALLBACKS:");
	for (int32_t i=0; i < globalConfig.maxCallbacks; i++)
	{
		if (callbacks[i] != 0) PRINT("" CAT i CAT "");
	}
	VERBOSE("");
}

//void integerCallback(MeanMachine & mm, MArgs & args) 
//{
//	PRINT("//////////////// INT!!! ////////////////");
//}

void sumCallback(MeanMachine & mm, MArgs & args)
{
	VERBOSE("//////////////// SUM ////////////////");
	mm.callbackReturn(MS_TYPE_INT, mm.stack[args.baseIndex] + mm.stack[args.baseIndex+1]);
}
void eqCallback(MeanMachine & mm, MArgs & args) 
{
	VERBOSE("//////////////// EQ ////////////////");
	
	VERBOSE("compare " CAT mm.stack[args.baseIndex] CAT " and " CAT mm.stack[args.baseIndex+1]);
	bool result = (mm.stack[args.baseIndex] == mm.stack[args.baseIndex+1]);
	VERBOSE("result: " CAT (result ? "true" : "false"));
	mm.callbackReturn(MS_TYPE_INT, result ? 1 : 0);
}
void ifCallback(MeanMachine & mm, MArgs & args) 
{
	VERBOSE("//////////////// IF ////////////////");
	
	if (mm.stack[args.baseIndex] == 1) {	
		VERBOSE("do it!");	
		mm.gosub(mm.stack[args.baseIndex+1]);
	} else VERBOSE("don't do!");
}
void subCallback(MeanMachine & mm, MArgs & args)
{
	VERBOSE("//////////////// SUBTRACTION ////////////////");
	int32_t a = mm.stack[args.baseIndex];
	int32_t b = mm.stack[args.baseIndex+1];
	VERBOSE("calculate " CAT a CAT " - " CAT b CAT " = " CAT a-b);
	mm.callbackReturn(MS_TYPE_INT, a - b);
}

void printIntCallback(MeanMachine & mm, MArgs & args)
{
	VERBOSE("//////////////// PRINT ////////////////");
	USER_PRINT(mm.stack[args.baseIndex]);
}

void printTextCallback(MeanMachine & mm, MArgs & args)
{
	VERBOSE("//////////////// PRINT TEXT ////////////////");
	USER_PRINT((*mm.globals).getText(mm.stack[args.baseIndex]));
}

void printFloatCallback(MeanMachine & mm, MArgs & args)
{
	VERBOSE("//////////////// PRINT FLOAT ////////////////");
	USER_PRINT(((float&)(*(&mm.stack[args.baseIndex]))));
}

int32_t Common:: createCallback (std::string name, void (*func)(MeanMachine &, MArgs &), int32_t returnType, StructDef* argStruct)
{
	VERBOSE("Add callback: " CAT name);
	
	MCallback* cb = new MCallback(name, func, returnType, argStruct);
	callbacks[callbackCounter] = cb;
	callbackIDs.insert(std::make_pair( name, callbackCounter));;
	return callbackCounter++;
}

void Common::includePrimitives (Semantics & sem) 
{
	// add primitive types
	StructDef* sd = new StructDef("int", MS_TYPE_INT);
	(*sd).addMember("x", MS_TYPE_INT);
	sem.addPrimitiveType("int", sd, MS_TYPE_INT);
	
	sd = new StructDef("float", MS_TYPE_FLOAT);
	(*sd).addMember("x", MS_TYPE_FLOAT);
	sem.addPrimitiveType("float", sd, MS_TYPE_FLOAT);
	
	sd = new StructDef("text", MS_TYPE_TEXT);
	(*sd).addMember("x", MS_TYPE_TEXT);
	sem.addPrimitiveType("text", sd, MS_TYPE_TEXT);
	
	sd = new StructDef("bool", MS_TYPE_BOOL);
	(*sd).addMember("x", MS_TYPE_BOOL);
	sem.addPrimitiveType("bool", sd, MS_TYPE_BOOL);
}
// Meanscript core types and callbacks

Common::Common () 
{
	callbackCounter = MAX_MS_TYPES;
	callbacks = new MCallback*[globalConfig.maxCallbacks];
	for (int32_t i=0; i < globalConfig.maxCallbacks; i++)
	{
		callbacks[i] = 0;
	}
	
	// add return value and parameter struct def.
	StructDef* sumArgs = new StructDef("two ints", callbackCounter);
	(*sumArgs).addMember("a", MS_TYPE_INT);
	(*sumArgs).addMember("b", MS_TYPE_INT);
	createCallback("sum", sumCallback, MS_TYPE_INT, sumArgs);
	
	StructDef* subArgs = new StructDef("two ints", callbackCounter);
	(*subArgs).addMember("a", MS_TYPE_INT);
	(*subArgs).addMember("b", MS_TYPE_INT);
	createCallback("sub", subCallback, MS_TYPE_INT, subArgs);

//	StructDef* incArgs = new StructDef((&(sem)), "one int", callbackCounter);
//	(*incArgs).addMember("a", MS_TYPE_INT);
//	createCallback("inc", incCallback, MS_TYPE_VOID, incArgs);
	
	StructDef* ifArgs = new StructDef("if args", callbackCounter);
	(*ifArgs).addMember("a", MS_TYPE_BOOL);
	(*ifArgs).addMember("b", MS_TYPE_CODE_ADDRESS);
	createCallback("if", ifCallback, MS_TYPE_VOID, ifArgs);
	
	StructDef* eqArgs = new StructDef("eq args", callbackCounter);
	(*eqArgs).addMember("a", MS_TYPE_INT);
	(*eqArgs).addMember("b", MS_TYPE_INT);
	createCallback("eq", eqCallback, MS_TYPE_BOOL, eqArgs);
	
	StructDef* printArgs = new StructDef("one int", callbackCounter);
	(*printArgs).addMember("a", MS_TYPE_INT);
	createCallback("print", printIntCallback, MS_TYPE_VOID, printArgs);
	
	StructDef* textPrintArgs = new StructDef("one text", callbackCounter);
	(*textPrintArgs).addMember("a", MS_TYPE_TEXT);
	createCallback("prints", printTextCallback, MS_TYPE_VOID, textPrintArgs);
	
	StructDef* floatPrintArgs = new StructDef("one float", callbackCounter);
	(*floatPrintArgs).addMember("a", MS_TYPE_FLOAT);
	createCallback("printf", printFloatCallback, MS_TYPE_VOID, floatPrintArgs);
}
Common::~Common() { 	for (int32_t i=0; i < globalConfig.maxCallbacks; i++) 	{ 		delete callbacks[i]; 	} 	delete[] callbacks; }
} // namespace meanscript(core)
// C++ END
