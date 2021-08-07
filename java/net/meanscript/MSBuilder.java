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
java.util.TreeMap<MSText, Integer> texts = new java.util.TreeMap<MSText, Integer>(MSJava.textComparator);
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
	common.initialize(semantics);
	structLock = false;
	
	textIDCounter = 0;
	texts.put( new MSText(""), textIDCounter++);
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
	MSText tn = new MSText (typeName);
	semantics.addStructDef(tn, id, sd);
}

public void addInt (String name, int value) throws MException
{
	structLock = true;
	MSJava.assertion(semantics.isNameValidAndAvailable(name),   "variable name error");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("BUILDER: New int: " + name).endLine();};
	MSText tn = new MSText (name);
	int address = variables.addMember(semantics, tn, MS_TYPE_INT);
	values[address] = value;
}
public void addInt64 (String name, long value) throws MException
{
	structLock = true;
	MSText tn = new MSText (name);
	MSJava.assertion(semantics.isNameValidAndAvailable(tn),   "variable name error");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("BUILDER: New int64: " + name).endLine();};
	int address = variables.addMember(semantics, tn, MS_TYPE_INT64);
	values[address] = int64highBits(value);
	values[address+1] = int64lowBits(value);
}

public int createText (String value) throws MException
{
	MSText tmp = new MSText (value);
	structLock = true;
	if (!(texts.containsKey( tmp)))
	{
		texts.put( new MSText(value), textIDCounter++);
	}
	return texts.get( tmp);
}

public void addText (String name, String value) throws MException
{
	structLock = true;
	MSJava.assertion(semantics.isNameValidAndAvailable(name),   "variable name error");
	// add string to tree
	int textID = createText(value);
	MSText tn = new MSText (name);
	int address = variables.addMember(semantics, tn, MS_TYPE_TEXT);
	values[address] = textID;
}

public void addChars (String name, int numChars, String text) throws MException
{
	StructDef sd = semantics.addCharsType(numChars);
	MSJava.assertion(semantics.isNameValidAndAvailable(name),   "variable name error");
	MSText tn = new MSText (name);
	int address = variables.addMember(semantics, tn, sd.typeID);
	int maxSize = sd.structSize;
	
	stringToIntsWithSize(text, values, address, maxSize);
}

public int createStructDef (String name) throws MException
{
	lockCheck();
	int id = semantics.typeIDCounter++;
	MSText tn = new MSText (name);
	StructDef sd = new StructDef(tn, id);
	semantics.addStructDef(tn, id, sd);
	return id;
}

public void addCharsMember (int structTypeID, String name, int numChars) throws MException
{
	StructDef sd = semantics.getType(structTypeID);
	StructDef charsType = semantics.addCharsType(numChars);
	MSText tn = new MSText (name);
	sd.addMember(semantics, tn, charsType.typeID);
}

public int addMember (int structTypeID, String name, int memberType) throws MException
{
	StructDef sd = semantics.getType(structTypeID);
	MSText tn = new MSText (name);
	return sd.addMember(semantics, tn, memberType);
}

public void addArray (int typeID, String arrayName, int arraySize) throws MException
{
	MSText tn = new MSText (arrayName);
	variables.addArray(semantics, tn, typeID, arraySize);
}

public MSWriter arrayItem (String arrayName, int arrayIndex) throws MException
{
	MSText tn = new MSText (arrayName);
	int tag = variables.getMemberTag(tn);
	MSJava.assertion((tag & OPERATION_MASK) == OP_ARRAY_MEMBER,   "not an array");
	int itemCount = variables.getMemberArrayItemCount(tn);
	MSJava.assertion(arrayIndex >= 0 && arrayIndex < itemCount,   "index out of bounds: " + arrayIndex + " / " + itemCount);
	StructDef arrayItemType = semantics.getType((int)(tag & VALUE_TYPE_MASK));
	int itemSize = arrayItemType.structSize;
	int address = variables.getMemberAddress(tn);
	address += arrayIndex * itemSize;
	
	return new MSWriter( 		this, 		arrayItemType, 		address 		);
}

public MSWriter createStruct (String typeName, String varName) throws MException
{
	MSText tn = new MSText (typeName);
	StructDef sd = semantics.getType(tn);
	return createStruct(sd.typeID, varName);
}

public MSWriter createStruct (int typeID, String varName) throws MException
{
	MSText tn = new MSText(varName);
	structLock = true;
	MSJava.assertion(semantics.isNameValidAndAvailable(tn),   "variable name error");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("BUILDER: New struct: " + tn).endLine();};
	StructDef sd = semantics.getType(typeID);
	int address = variables.addMember(semantics, tn, typeID);
	tn = null;
	return new MSWriter( 		this, 		sd, 		address 		);
}

public int createGeneratedStruct (int typeID, String varName) throws MException
{
	MSText tn = new MSText (varName);
	return variables.addMember(semantics, tn, typeID);
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
	 MSText[] textArray = new  MSText[numTexts];
	
	for(java.util.Map.Entry<MSText,Integer>  entry :  texts.entrySet())
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
