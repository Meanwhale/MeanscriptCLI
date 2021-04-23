package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;





public class MSBuilder extends MC {
String packageName;
 int [] values;
 Semantics semantics;
StructDef variables;
ByteCode byteCode;
Common common;
java.util.TreeMap<String, Integer> texts = new java.util.TreeMap<String, Integer>();
int textIDCounter;
boolean structLock;

public MSBuilder (String _packageName) throws MException
{
	semantics = new Semantics();
	variables = semantics.globalContext.variables;
	packageName = _packageName;
	{values = new int[ MSJava.globalConfig.builderValuesSize]; };
	common = new Common();
	byteCode = new ByteCode(common);
	common.includePrimitives(semantics);
	structLock = false;
	
	textIDCounter = 0;
	texts.put( "", textIDCounter++);
}

//;

public void lockCheck () throws MException
{
	MSJava.assertion(!structLock,   "Structs can't be defined after data is added");
}

public int [] getValueArray ()
{
	return values;
}

public void addType (String typeName, StructDef sd) throws MException
{
	lockCheck();
	int id = semantics.typeIDCounter++;
	sd.typeID = id;
	semantics.addStructDef(typeName, id, sd);
}

public void addInt (String name, int value) throws MException
{
	structLock = true;
	MSJava.assertion(semantics.assumeNotReserved(name),   "variable name error");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("BUILDER: New int: " + name).endLine();};
	int address = variables.addMember(semantics, name, MS_TYPE_INT);
	values[address] = value;
}

public int createText (String value) throws MException
{
	structLock = true;
	if (!(texts.containsKey( value)))
	{
		texts.put( value, textIDCounter++);
	}
	return texts.get( value);
}

public void addText (String varName, String value) throws MException
{
	structLock = true;
	MSJava.assertion(semantics.assumeNotReserved(varName),   "variable name error");
	// add string to tree
	int textID = createText(value);
	int address = variables.addMember(semantics, varName, MS_TYPE_TEXT);
	values[address] = textID;
}

public void addChars (String varName, int numChars, String text) throws MException
{
	StructDef sd = semantics.addCharsType(numChars);
	int address = variables.addMember(semantics, varName, sd.typeID);
	int maxSize = sd.structSize;
	
	stringToIntsWithSize(text, values, address, maxSize);
}

public int createStructDef (String name) throws MException
{
	lockCheck();
	int id = semantics.typeIDCounter++;
	StructDef sd = new StructDef(name, id);
	semantics.addStructDef(name, id, sd);
	return id;
}

public void addCharsMember (int structTypeID, String varName, int numChars) throws MException
{
	StructDef sd = semantics.getType(structTypeID);
	StructDef charsType = semantics.addCharsType(numChars);
	sd.addMember(semantics, varName, charsType.typeID);
}

public int addMember (int structTypeID, String varName, int memberType) throws MException
{
	StructDef sd = semantics.getType(structTypeID);
	return sd.addMember(varName, memberType);
}

public void addArray (int typeID, String arrayName, int arraySize) throws MException
{
	variables.addArray(semantics, arrayName, typeID, arraySize);
}

public MSWriter arrayItem (String arrayName, int arrayIndex) throws MException
{
	int tag = variables.getMemberTag(arrayName);
	MSJava.assertion((tag & OPERATION_MASK) == OP_ARRAY_MEMBER,   "not an array");
	int itemCount = variables.getMemberArrayItemCount(arrayName);
	MSJava.assertion(arrayIndex >= 0 && arrayIndex < itemCount,   "index out of bounds: " + arrayIndex + " / " + itemCount);
	StructDef arrayItemType = semantics.getType((int)(tag & VALUE_TYPE_MASK));
	int itemSize = arrayItemType.structSize;
	int address = variables.getMemberAddress(arrayName);
	address += arrayIndex * itemSize;
	
	return new MSWriter( 		this, 		arrayItemType, 		address 		);
}

public MSWriter createStruct (String typeName, String varName) throws MException
{
	StructDef sd = semantics.getType(typeName);
	return createStruct(sd.typeID, varName);
}

public MSWriter createStruct (int typeID, String varName) throws MException
{
	structLock = true;
	MSJava.assertion(semantics.assumeNotReserved(varName),   "variable name error");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("BUILDER: New struct: " + varName).endLine();};
	StructDef sd = semantics.getType(typeID);
	int address = variables.addMember(semantics, varName, typeID);
	
	return new MSWriter( 		this, 		sd, 		address 		);
}

public int createGeneratedStruct (int typeID, String varName) throws MException
{
	return variables.addMember(semantics, varName, typeID);
}

public void  readStructCode (int [] code) throws MException
{
	ClassMaker.findTypes(semantics, code);
	ClassMaker.createStructDefs(semantics, code);
}

public void generate () throws MException
{
	byteCode.addInstructionWithData(OP_START_INIT, 1, BYTECODE_READ_ONLY, texts.size());
	
	// write global variable and other structure definitions
	semantics.writeStructDefs(byteCode);
	
	// save immutable texts
	// TODO: same as in Generator = make a function (?)
	
	int numTexts = texts.size();
	String[] textArray = new String[numTexts];
	
	for(java.util.Map.Entry<String,Integer>  entry :  texts.entrySet())
	{
		// (key, value) = (text, id)
		int id = entry.getValue();
		MSJava.assertion(id >= 0 && id < numTexts,   "unexpected text ID");
		textArray[id] = entry.getKey();
	}
	for (int i=1; i<numTexts; i++)
	{
		byteCode.codeTop = addTextInstruction(textArray[i], OP_ADD_TEXT, byteCode.code, byteCode.codeTop);
	}

	// initialize  values

	int globalsSize = variables.structSize;
	byteCode.addWord(makeInstruction(OP_INIT_GLOBALS, globalsSize, 0));
	for (int i=0; i<globalsSize; i++)
	{
		byteCode.addWord(values[i]);
	}
	byteCode.addInstruction(OP_END_INIT, 0 , 0);
}

public MSCode  createMS () throws MException
{
	MSCode ms = new MSCode();
	ms.initBytecode(byteCode);
	return ms;
}

public void write (MSOutputStream output) throws MException
{
	byteCode.writeCode(output);
	output.close();
}


}
