package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class Common extends MC {
 boolean initialized = false;
 int callbackCounter;
 MCallback [] callbacks;
 java.util.TreeMap<MSText, Integer> callbackIDs = new java.util.TreeMap<MSText, Integer>(MSJava.textComparator);

// private static data. common for all MeanScript objects

public void printCallbacks () throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- CALLBACKS:").endLine();};
	for (int i=0; i < MSJava.globalConfig.maxCallbacks; i++)
	{
		if (callbacks[i] != null) MSJava.printOut.print("" + i + "").endLine();
	}
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("").endLine();};
}

private static void trueCallback(MeanMachine mm, MArgs args) throws MException
{
	mm.callbackReturn(MS_TYPE_BOOL, 1);
}
private static void falseCallback(MeanMachine mm, MArgs args) throws MException
{
	mm.callbackReturn(MS_TYPE_BOOL, 0);
}
private static void sumCallback(MeanMachine mm, MArgs args) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// SUM ////////////////").endLine();};
	mm.callbackReturn(MS_TYPE_INT, mm.stack[args.baseIndex] + mm.stack[args.baseIndex+1]);
}
private static void eqCallback(MeanMachine mm, MArgs args) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// EQ ////////////////").endLine();};
	
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("compare " + mm.stack[args.baseIndex] + " and " + mm.stack[args.baseIndex+1]).endLine();};
	boolean result = (mm.stack[args.baseIndex] == mm.stack[args.baseIndex+1]);
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("result: " + (result ? "true" : "false")).endLine();};
	mm.callbackReturn(MS_TYPE_INT, result ? 1 : 0);
}
private static void ifCallback(MeanMachine mm, MArgs args) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// IF ////////////////").endLine();};
	
	if (mm.stack[args.baseIndex] != 0) {	
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("do it!").endLine();};	
		mm.gosub(mm.stack[args.baseIndex+1]);
	} else {if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("don't do!").endLine();};
}
private static void subCallback(MeanMachine mm, MArgs args) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// SUBTRACTION ////////////////").endLine();};
	int a = mm.stack[args.baseIndex];
	int b = mm.stack[args.baseIndex+1];
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("calculate " + a + " - " + b + " = " + (a-b)).endLine();};
	mm.callbackReturn(MS_TYPE_INT, a - b);
}

private static void printIntCallback(MeanMachine mm, MArgs args) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// PRINT ////////////////").endLine();};
	MSJava.userOut.print(mm.stack[args.baseIndex]).endLine();
}

private static void printTextCallback(MeanMachine mm, MArgs args) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// PRINT TEXT ////////////////").endLine();};
	
	int address = mm.texts[mm.stack[args.baseIndex]];
	int numChars = mm.getStructCode()[address + 1];
	MSJava.userOut.print("").printIntsToChars(mm.getStructCode(), address + 2, numChars, false).endLine();
}

private static void printCharsCallback(MeanMachine mm, MArgs args) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// PRINT CHARS  ////////////////").endLine();};
	int numChars = mm.stack[args.baseIndex];
	MSJava.userOut.print("").printIntsToChars(mm.stack, args.baseIndex + 1, numChars, false);
}

private static void printFloatCallback(MeanMachine mm, MArgs args) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// PRINT FLOAT ////////////////").endLine();};
	MSJava.userOut.print(MSJava.intFormatToFloat(mm.stack[args.baseIndex]));
}

public int  createCallback (MSText name, MJCallbackAction func, int returnType, StructDef argStruct) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Add callback: " + name).endLine();};
	
	MCallback cb = new MCallback(func, returnType, argStruct);
	callbacks[callbackCounter] = cb;
	callbackIDs.put( name, callbackCounter);
	return callbackCounter++;
}

public void initialize (Semantics sem) throws MException
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

public void createCallbacks (Semantics sem) throws MException
{
	
	// add return value and parameter struct def.
	StructDef trueArgs = new StructDef(null, callbackCounter);
	createCallback(new MSText("true"), (MeanMachine mm, MArgs args) -> {trueCallback(mm,args);}, MS_TYPE_BOOL, trueArgs);
	
	StructDef falseArgs = new StructDef(null, callbackCounter);
	createCallback(new MSText("false"), (MeanMachine mm, MArgs args) -> {falseCallback(mm,args);}, MS_TYPE_BOOL, falseArgs);
	
	StructDef sumArgs = new StructDef(null, callbackCounter)	;
	sumArgs.addMember(null, MS_TYPE_INT, 1);
	sumArgs.addMember(null, MS_TYPE_INT, 1);
	createCallback(new MSText("sum"), (MeanMachine mm, MArgs args) -> {sumCallback(mm,args);}, MS_TYPE_INT, sumArgs);
	
	StructDef subArgs = new StructDef(null, callbackCounter);
	subArgs.addMember(null, MS_TYPE_INT, 1);
	subArgs.addMember(null, MS_TYPE_INT, 1);
	createCallback(new MSText("sub"), (MeanMachine mm, MArgs args) -> {subCallback(mm,args);}, MS_TYPE_INT, subArgs);
	
	StructDef ifArgs = new StructDef(null, callbackCounter);
	ifArgs.addMember(null, MS_TYPE_BOOL, 1);
	ifArgs.addMember(null, MS_TYPE_CODE_ADDRESS, 1);
	createCallback(new MSText("if"), (MeanMachine mm, MArgs args) -> {ifCallback(mm,args);}, MS_TYPE_VOID, ifArgs);
	
	StructDef eqArgs = new StructDef(null, callbackCounter);
	eqArgs.addMember(null, MS_TYPE_INT, 1);
	eqArgs.addMember(null, MS_TYPE_INT, 1);
	createCallback(new MSText("eq"), (MeanMachine mm, MArgs args) -> {eqCallback(mm,args);}, MS_TYPE_BOOL, eqArgs);
	
	StructDef printArgs = new StructDef(null, callbackCounter);
	printArgs.addMember(null, MS_TYPE_INT, 1);
	createCallback(new MSText("print"), (MeanMachine mm, MArgs args) -> {printIntCallback(mm,args);}, MS_TYPE_VOID, printArgs);
	
	StructDef textPrintArgs = new StructDef(null, callbackCounter);
	textPrintArgs.addMember(null, MS_TYPE_TEXT, 1);
	createCallback(new MSText("prints"), (MeanMachine mm, MArgs args) -> {printTextCallback(mm,args);}, MS_TYPE_VOID, textPrintArgs);
	
	StructDef floatPrintArgs = new StructDef(null, callbackCounter);
	floatPrintArgs.addMember(null, MS_TYPE_FLOAT, 1);
	createCallback(new MSText("printf"), (MeanMachine mm, MArgs args) -> {printFloatCallback(mm,args);}, MS_TYPE_VOID, floatPrintArgs);
	
	StructDef charsPrintArgs = new StructDef(null, callbackCounter);
	charsPrintArgs.addMember(null, MS_TYPE_CHARS, 1);
	createCallback(new MSText("printc"), (MeanMachine mm, MArgs args) -> {printCharsCallback(mm,args);}, MS_TYPE_VOID, charsPrintArgs);
}

public Common () throws MException
{
	callbackCounter = MAX_MS_TYPES;
	callbacks = new MCallback[MSJava.globalConfig.maxCallbacks];
	for (int i=0; i < MSJava.globalConfig.maxCallbacks; i++)
	{
		callbacks[i] = null;
	}
}
//
}
