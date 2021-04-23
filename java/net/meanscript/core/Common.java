package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;





public class Common extends MC {
 boolean initialized = false;
 int callbackCounter;
 MCallback [] callbacks;
 java.util.TreeMap<String, Integer> callbackIDs = new java.util.TreeMap<String, Integer>();

// private static data. common for all MeanScript objects

public void printCallbacks ()
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- CALLBACKS:").endLine();};
	for (int i=0; i < MSJava.globalConfig.maxCallbacks; i++)
	{
		if (callbacks[i] != null) MSJava.printOut.print("" + i + "").endLine();
	}
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("").endLine();};
}

//void integerCallback(MeanMachine mm, MArgs args) throws MException
//{
//	MSJava.printOut.print("//////////////// INT!!! ////////////////").endLine();
//}

private static void sumCallback(MeanMachine mm, MArgs args)
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
	
	if (mm.stack[args.baseIndex] == 1) {	
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("do it!").endLine();};	
		mm.gosub(mm.stack[args.baseIndex+1]);
	} else {if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("don't do!").endLine();};
}
private static void subCallback(MeanMachine mm, MArgs args)
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// SUBTRACTION ////////////////").endLine();};
	int a = mm.stack[args.baseIndex];
	int b = mm.stack[args.baseIndex+1];
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("calculate " + a + " - " + b + " = " + (a-b)).endLine();};
	mm.callbackReturn(MS_TYPE_INT, a - b);
}

private static void printIntCallback(MeanMachine mm, MArgs args)
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// PRINT ////////////////").endLine();};
	MSJava.userOut.print(mm.stack[args.baseIndex]);
}

private static void printTextCallback(MeanMachine mm, MArgs args)
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// PRINT TEXT ////////////////").endLine();};
	MSJava.userOut.print(mm.globals.getText(mm.stack[args.baseIndex]));
}

private static void printCharsCallback(MeanMachine mm, MArgs args)
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// PRINT CHARS  ////////////////").endLine();};
	int numChars = mm.stack[args.baseIndex];
	String s = new String(MSJava.intsToBytes(mm.stack, args.baseIndex + 1, numChars));
	MSJava.userOut.print(s);
}

private static void printFloatCallback(MeanMachine mm, MArgs args)
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("//////////////// PRINT FLOAT ////////////////").endLine();};
	MSJava.userOut.print(MSJava.intFormatToFloat(mm.stack[args.baseIndex]));
}

public int  createCallback (String name, MJCallbackAction func, int returnType, StructDef argStruct)
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Add callback: " + name).endLine();};
	
	MCallback cb = new MCallback(name, func, returnType, argStruct);
	callbacks[callbackCounter] = cb;
	callbackIDs.put( name, callbackCounter);
	return callbackCounter++;
}

public void includePrimitives (Semantics sem) throws MException
{
	sem.addElementaryType("int",   MS_TYPE_INT,   1);
	sem.addElementaryType("float", MS_TYPE_FLOAT, 1);
	sem.addElementaryType("text",  MS_TYPE_TEXT,  1);
	sem.addElementaryType("bool",  MS_TYPE_BOOL,  1);
	sem.addElementaryType("chars", MS_TYPE_CHARS,  -1); // special, dynamic type
}
// Meanscript core types and callbacks

public Common () throws MException
{
	callbackCounter = MAX_MS_TYPES;
	callbacks = new MCallback[MSJava.globalConfig.maxCallbacks];
	for (int i=0; i < MSJava.globalConfig.maxCallbacks; i++)
	{
		callbacks[i] = null;
	}
	
	// add return value and parameter struct def.
	StructDef sumArgs = new StructDef("two ints", callbackCounter);
	sumArgs.addMember("a", MS_TYPE_INT);
	sumArgs.addMember("b", MS_TYPE_INT);
	createCallback("sum", (MeanMachine mm, MArgs args) -> {sumCallback(mm,args);}, MS_TYPE_INT, sumArgs);
	
	StructDef subArgs = new StructDef("two ints", callbackCounter);
	subArgs.addMember("a", MS_TYPE_INT);
	subArgs.addMember("b", MS_TYPE_INT);
	createCallback("sub", (MeanMachine mm, MArgs args) -> {subCallback(mm,args);}, MS_TYPE_INT, subArgs);

//	StructDef incArgs = new StructDef(sem, "one int", callbackCounter);
//	incArgs.addMember("a", MS_TYPE_INT);
//	createCallback("inc", (MeanMachine mm, MArgs args) -> {incCallback(mm,args);}, MS_TYPE_VOID, incArgs);
	
	StructDef ifArgs = new StructDef("if args", callbackCounter);
	ifArgs.addMember("a", MS_TYPE_BOOL);
	ifArgs.addMember("b", MS_TYPE_CODE_ADDRESS);
	createCallback("if", (MeanMachine mm, MArgs args) -> {ifCallback(mm,args);}, MS_TYPE_VOID, ifArgs);
	
	StructDef eqArgs = new StructDef("eq args", callbackCounter);
	eqArgs.addMember("a", MS_TYPE_INT);
	eqArgs.addMember("b", MS_TYPE_INT);
	createCallback("eq", (MeanMachine mm, MArgs args) -> {eqCallback(mm,args);}, MS_TYPE_BOOL, eqArgs);
	
	StructDef printArgs = new StructDef("one int", callbackCounter);
	printArgs.addMember("a", MS_TYPE_INT);
	createCallback("print", (MeanMachine mm, MArgs args) -> {printIntCallback(mm,args);}, MS_TYPE_VOID, printArgs);
	
	StructDef textPrintArgs = new StructDef("one text", callbackCounter);
	textPrintArgs.addMember("a", MS_TYPE_TEXT);
	createCallback("prints", (MeanMachine mm, MArgs args) -> {printTextCallback(mm,args);}, MS_TYPE_VOID, textPrintArgs);
	
	StructDef floatPrintArgs = new StructDef("one float", callbackCounter);
	floatPrintArgs.addMember("a", MS_TYPE_FLOAT);
	createCallback("printf", (MeanMachine mm, MArgs args) -> {printFloatCallback(mm,args);}, MS_TYPE_VOID, floatPrintArgs);
	
	StructDef charsPrintArgs = new StructDef("one chars", callbackCounter);
	charsPrintArgs.addMember("a", MS_TYPE_CHARS);
	createCallback("printc", (MeanMachine mm, MArgs args) -> {printCharsCallback(mm,args);}, MS_TYPE_VOID, charsPrintArgs);
}
//
}
