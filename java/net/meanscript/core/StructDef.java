package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class StructDef extends MC {
public int typeID;
 int [] code;
 int [] memberOffset; // memberOffset[n] = code address of _n_th member
 int [] nameOffset; // nameOffset[n] = name address of _n_th member
 MSText name;
 int numMembers;
 int argsSize;
public int structSize;
 int codeTop;
 java.util.TreeMap<MSText, Integer> memberNames = new java.util.TreeMap<MSText, Integer>(MSJava.textComparator);


//	DEFINITION
//	
//	START, MEMBER_1, ADDRESS_1, MEMBER_2, ADDRESS_2, ... , MEMBER_n, ADDRESS_n
//		- numMembers = n
//		- structSize = sum of member types' sizes
//	
//	START:
//		instruction:	STRUCT_DEF
//		size:			0
//		type:			type ID
//	
//	MEMBER_i:
//		instruction:	STRUCT_MEMBER
//		size:			1
//		type:			type ID
//	
//	ADDRESS_i:
//		offset from structure data start, sum of previous member types' sizes
//	
//	For example:
//		[int foo, vec3 pos, int bar]
//			structSize = 5


public StructDef (MSText _name, int _typeID) throws MException
{
	if (_name != null) name = new MSText(_name);
	else name = null;
	typeID = _typeID;
	
	{code = new int[			MSJava.globalConfig.maxStructDefSize]; };
	{memberOffset = new int[	MSJava.globalConfig.maxStructMembers]; };
	{nameOffset = new int[		MSJava.globalConfig.maxStructMembers]; };
	
	numMembers = 0;
	argsSize = -1; // set structSize after all arguments are set
	structSize = 0;
	codeTop = 0;

	// save name
	if (name != null)
	{
		codeTop = addTextInstruction(name, OP_STRUCT_DEF, code, codeTop);
		// change type to start tag
		code[0] = makeInstruction(OP_STRUCT_DEF, codeTop-1, typeID);
	}
	else
	{
		code[0] = makeInstruction(OP_STRUCT_DEF, 1, typeID);
		code[1] = 0; // empty name
		codeTop = 2;
	}
	
}

public StructDef (MSText _name, int _typeID, int _size) throws MException
{
	// primitive: no need to initialize arrays (code, memberOffset, nameOffset)
	if (_name != null) name = new MSText(_name);
	else name = null;
	typeID = _typeID;
	numMembers = -1;
	argsSize = -1; // set structSize after all arguments are set
	structSize = _size;
	codeTop = -1;
}

public StructDef (MSText _name, int _typeID, int data, int size, int op) throws MException
{
	// generic struct: no need to initialize arrays memberOffset and nameOffset
	if (_name != null) name = new MSText(_name);
	else name = null;
	typeID = _typeID;
	
	{code = new int[ 2]; };
	code[0] = makeInstruction(op, 1, typeID);
	code[1] = data; // chars: number of characters
	codeTop = 2;
	
	numMembers = -1;
	argsSize = -1; // set structSize after all arguments are set
	structSize = size;
}

//

public boolean  isCharsDef ()
{
	return codeTop > 0 && (int)(code[0] & OPERATION_MASK) == OP_CHARS_DEF;
}

public int numCharsForCharsDef () throws MException
{
	MSJava.assertion(isCharsDef(),  "not a chars def.");
	return code[1];
}

public int  addArray (Semantics semantics, MSText name, int arrayType, int itemCount) throws MException
{	
	// array with it's item count at first, e.g.
	// 		vec2 [3] -> [3] [x0] [y0] [x1] [y1] [x2] [y2]
	//		"sizeof" being 7 here

	MSJava.assertion(itemCount > 0 && itemCount < MSJava.globalConfig.maxArraySize,   "invalid array size");
	StructDef sd = semantics.typeStructDefs[arrayType];
	MSJava.assertion(sd != null,   "struct missing");
	int singleMemberSize = sd.structSize;
	int dataSize = itemCount * singleMemberSize; // "sizeof"
	
	// name saving instruction
	nameOffset[numMembers] = codeTop;
	codeTop = addTextInstruction(name, OP_MEMBER_NAME, code, codeTop);
	
	// create tag
	memberOffset[numMembers] = codeTop;

	int memberTag = makeInstruction(OP_ARRAY_MEMBER, 3, arrayType);
	code[codeTop] = memberTag;
	int address = structSize;
	code[codeTop + 1] = address; // offset = sum of previous type sizes
	code[codeTop + 2] = dataSize;
	code[codeTop + 3] = itemCount;
	
	structSize += dataSize;
	
	memberNames.put( new MSText(name), codeTop);
	codeTop += 4;

	numMembers++;
	
	return address;
}

public int  addMember (Semantics semantics, MSText name, int type) throws MException
{	
	// get size

	StructDef sd = semantics.typeStructDefs[type];
	MSJava.assertion(sd != null,   "struct missing");
	int memberSize = sd.structSize;
	return addMember(name, type, memberSize);
}

public int  addMember (MSText name, int type, int memberSize) throws MException
{		
	// name saving instruction

	if (name != null)
	{
		nameOffset[numMembers] = codeTop;
		codeTop = addTextInstruction(name, OP_MEMBER_NAME, code, codeTop);
	}
	else
	{
		nameOffset[numMembers] = -1;
	}
	// create tag

	memberOffset[numMembers] = codeTop;

	int memberTag = makeInstruction(OP_STRUCT_MEMBER, 2, type);
	code[codeTop] = memberTag;
	int address = structSize;
	code[codeTop + 1] = address; // offset = sum of previous type sizes
	code[codeTop + 2] = memberSize; // "sizeof"

	structSize += memberSize;
	
	if (name != null) { memberNames.put( new MSText(name), codeTop); }
	
	codeTop += 3;

	numMembers++;
	
	return address;
}

public int  getMemberTag (MSText varName) throws MException
{
	MSJava.assertion((memberNames.containsKey( varName)),   "undefined variable: " + varName);
	int index = memberNames.get( varName);
	return code[index];
}
public int  getMemberAddress (MSText varName) throws MException
{
	MSJava.assertion((memberNames.containsKey( varName)),   "undefined variable: " + varName);
	int index = memberNames.get( varName);
	return code[index+1]; // see above for definition
}
public int  getMemberSize (MSText varName) throws MException
{
	MSJava.assertion((memberNames.containsKey( varName)),   "undefined variable: " + varName);
	int index = memberNames.get( varName);
	return code[index+2]; // see above for definition
}
public int  getMemberArrayItemCount (MSText varName) throws MException
{
	MSJava.assertion((memberNames.containsKey( varName)),   "undefined variable: " + varName);
	int index = memberNames.get( varName);
	int tag = code[index];
	MSJava.assertion((tag & OPERATION_MASK) == OP_ARRAY_MEMBER, EC_SYNTAX, "not an array: " + varName);
	return code[index + 3]; // see above for definition
}
public int  getMemberArrayItemCountOrNegative (MSText varName) throws MException
{
	MSJava.assertion((memberNames.containsKey( varName)),   "undefined variable: " + varName);
	int index = memberNames.get( varName);
	int tag = code[index];
	if ((tag & OPERATION_MASK) != OP_ARRAY_MEMBER) return -1;
	return code[index + 3]; // see above for definition
}
public int  getMemberArrayItemCountOrNegative (int index) throws MException
{
	MSJava.assertion(indexInRange(index),   "argument index out of range: " + index);
	int tag = code[memberOffset[index]];
	if ((tag & OPERATION_MASK) != OP_ARRAY_MEMBER) return -1;
	return code[memberOffset[index] + 3]; // see above for definition
}
public boolean  indexInRange(int index)
{
	return index >= 0 && index < numMembers;
}
public int  getMemberTag (int index) throws MException
{
	MSJava.assertion(indexInRange(index),   "argument index out of range: " + index);
	return code[memberOffset[index]];
}
public int  getMemberAddress (int index) throws MException
{
	MSJava.assertion(indexInRange(index),   "argument index out of range: " + index);
	return code[memberOffset[index] + 1]; // see above for definition
}
public int  getMemberSize (int index) throws MException
{
	MSJava.assertion(indexInRange(index),   "argument index out of range: " + index);
	return code[memberOffset[index] + 2]; // see above for definition
}
public String  getMemberName (int index) throws MException
{
	MSJava.assertion(indexInRange(index),   "argument index out of range: " + index);
	int offset = nameOffset[index];
	return new String(MSJava.intsToBytes(code, offset + 2, code[offset + 1]),java.nio.charset.StandardCharsets.UTF_8);
}
public void print () throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("STRUCT CODE: " + name).endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	if (numMembers == 0)
	{
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("        empty").endLine();};
	}
	else
	{
		if (MSJava.globalConfig.verboseOn()) printData(code, codeTop, -1, true);
	}
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
}

}
