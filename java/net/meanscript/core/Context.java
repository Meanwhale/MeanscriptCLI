package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class Context extends MC {
 int functionID;
 int returnType;
 int tagAddress;
 int codeStartAddress;
 int codeEndAddress;
public StructDef variables;
 int numArgs; // number of arguments in the beginning of 'variables' struct
 MNode codeNode; // code block node where the function code is

public Context (MSText _name, int _functionID, int _returnType) throws MException
{
	variables = new StructDef(_name, _functionID);
	functionID = _functionID;
	returnType = _returnType;
	tagAddress = -1;
	
	codeNode = null;
	codeStartAddress = -1;
	codeEndAddress = -1;
}

public void print ()
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Context.print(): TODO").endLine();};
}

}
