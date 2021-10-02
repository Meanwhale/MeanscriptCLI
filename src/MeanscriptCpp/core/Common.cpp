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

void trueCallback(MeanMachine & mm, MArgs & args) 
{
	mm.callbackReturn(MS_TYPE_BOOL, 1);
}
void falseCallback(MeanMachine & mm, MArgs & args) 
{
	mm.callbackReturn(MS_TYPE_BOOL, 0);
}
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
	
	if (mm.stack[args.baseIndex] != 0) {	
		VERBOSE("do it!");	
		mm.gosub(mm.stack[args.baseIndex+1]);
	} else VERBOSE("don't do!");
}
void subCallback(MeanMachine & mm, MArgs & args) 
{
	VERBOSE("//////////////// SUBTRACTION ////////////////");
	int32_t a = mm.stack[args.baseIndex];
	int32_t b = mm.stack[args.baseIndex+1];
	VERBOSE("calculate " CAT a CAT " - " CAT b CAT " = " CAT (a-b));
	mm.callbackReturn(MS_TYPE_INT, a - b);
}

void printIntCallback(MeanMachine & mm, MArgs & args) 
{
	VERBOSE("//////////////// PRINT ////////////////");
	USER_PRINT(mm.stack[args.baseIndex]).endLine();
}

void printTextCallback(MeanMachine & mm, MArgs & args) 
{
	VERBOSE("//////////////// PRINT TEXT ////////////////");
	
	int32_t address = mm.texts[mm.stack[args.baseIndex]];
	int32_t numChars = (*mm.getStructCode())[address + 1];
	USER_PRINT("").printIntsToChars((*mm.getStructCode()), address + 2, numChars, false).endLine();
}

void printCharsCallback(MeanMachine & mm, MArgs & args) 
{
	VERBOSE("//////////////// PRINT CHARS  ////////////////");
	int32_t numChars = mm.stack[args.baseIndex];
	USER_PRINT("").printIntsToChars(mm.stack, args.baseIndex + 1, numChars, false);
}

void printFloatCallback(MeanMachine & mm, MArgs & args) 
{
	VERBOSE("//////////////// PRINT FLOAT ////////////////");
	USER_PRINT(((float&)(*(&mm.stack[args.baseIndex]))));
}

int32_t Common:: createCallback (MSText name, void (*func)(MeanMachine &, MArgs &), int32_t returnType, StructDef* argStruct) 
{
	VERBOSE("Add callback: " CAT name);
	
	MCallback* cb = new MCallback(func, returnType, argStruct);
	callbacks[callbackCounter] = cb;
	callbackIDs.insert(std::make_pair( name, callbackCounter));;
	return callbackCounter++;
}

void Common::initialize (Semantics & sem) 
{
	sem.addElementaryType(MS_TYPE_INT,     1);
	sem.addElementaryType(MS_TYPE_INT64,   2);
	sem.addElementaryType(MS_TYPE_FLOAT,   1);
	sem.addElementaryType(MS_TYPE_FLOAT64, 2);
	sem.addElementaryType(MS_TYPE_TEXT,    1);
	sem.addElementaryType(MS_TYPE_BOOL,    1);
	sem.addElementaryType(MS_TYPE_CHARS,  -1); // special, dynamic type
	
	createCallbacks(sem);
}

void Common::createCallbacks (Semantics & sem) 
{
	
	// add return value and parameter struct def.
	StructDef* trueArgs = new StructDef(0, callbackCounter);
	createCallback(MSText("true"), trueCallback, MS_TYPE_BOOL, trueArgs);
	
	StructDef* falseArgs = new StructDef(0, callbackCounter);
	createCallback(MSText("false"), falseCallback, MS_TYPE_BOOL, falseArgs);
	
	StructDef* sumArgs = new StructDef(0, callbackCounter)	;
	(*sumArgs).addMember(0, MS_TYPE_INT, 1);
	(*sumArgs).addMember(0, MS_TYPE_INT, 1);
	createCallback(MSText("sum"), sumCallback, MS_TYPE_INT, sumArgs);
	
	StructDef* subArgs = new StructDef(0, callbackCounter);
	(*subArgs).addMember(0, MS_TYPE_INT, 1);
	(*subArgs).addMember(0, MS_TYPE_INT, 1);
	createCallback(MSText("sub"), subCallback, MS_TYPE_INT, subArgs);
	
	StructDef* ifArgs = new StructDef(0, callbackCounter);
	(*ifArgs).addMember(0, MS_TYPE_BOOL, 1);
	(*ifArgs).addMember(0, MS_TYPE_CODE_ADDRESS, 1);
	createCallback(MSText("if"), ifCallback, MS_TYPE_VOID, ifArgs);
	
	StructDef* eqArgs = new StructDef(0, callbackCounter);
	(*eqArgs).addMember(0, MS_TYPE_INT, 1);
	(*eqArgs).addMember(0, MS_TYPE_INT, 1);
	createCallback(MSText("eq"), eqCallback, MS_TYPE_BOOL, eqArgs);
	
	StructDef* printArgs = new StructDef(0, callbackCounter);
	(*printArgs).addMember(0, MS_TYPE_INT, 1);
	createCallback(MSText("print"), printIntCallback, MS_TYPE_VOID, printArgs);
	
	StructDef* textPrintArgs = new StructDef(0, callbackCounter);
	(*textPrintArgs).addMember(0, MS_TYPE_TEXT, 1);
	createCallback(MSText("prints"), printTextCallback, MS_TYPE_VOID, textPrintArgs);
	
	StructDef* floatPrintArgs = new StructDef(0, callbackCounter);
	(*floatPrintArgs).addMember(0, MS_TYPE_FLOAT, 1);
	createCallback(MSText("printf"), printFloatCallback, MS_TYPE_VOID, floatPrintArgs);
	
	StructDef* charsPrintArgs = new StructDef(0, callbackCounter);
	(*charsPrintArgs).addMember(0, MS_TYPE_CHARS, 1);
	createCallback(MSText("printc"), printCharsCallback, MS_TYPE_VOID, charsPrintArgs);
}

Common::Common () 
{
	callbackCounter = MAX_MS_TYPES;
	callbacks = new MCallback*[globalConfig.maxCallbacks];
	for (int32_t i=0; i < globalConfig.maxCallbacks; i++)
	{
		callbacks[i] = 0;
	}
}
Common::~Common() { 	for (int32_t i=0; i < globalConfig.maxCallbacks; i++) 	{ 		delete callbacks[i]; 	} 	delete[] callbacks; }
} // namespace meanscript(core)
