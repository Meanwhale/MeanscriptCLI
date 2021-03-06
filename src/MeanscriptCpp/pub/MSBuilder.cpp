
#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSBuilder::MSBuilder (std::string _packageName) 
 : 
	semantics(new Semantics()),
	variables((*(*semantics).globalContext).variables) {
	packageName = _packageName;
	{ values.reset( globalConfig.builderValuesSize); values.fill(0); values.description =  "MSBuilder: values"; };
	common = new Common();
	byteCode = new ByteCode(common);
	(*common).includePrimitives((*semantics));
	structLock = false;
	
	textIDCounter = 0;
	texts.insert(std::make_pair( "", textIDCounter++));;
}

MSBuilder::~MSBuilder() { delete semantics; delete byteCode; delete common; };

void MSBuilder::lockCheck () 
{
	ASSERT(!structLock, "Structs can't be defined after data is added");
}

Array<int> MSBuilder::getValueArray ()
{
	return values;
}

void MSBuilder::addType (std::string typeName, StructDef* sd) 
{
	lockCheck();
	int32_t id = (*semantics).typeIDCounter++;
	(*sd).typeID = id;
	(*semantics).addStructDef(typeName, id, sd);
}

void MSBuilder::addInt (std::string name, int32_t value) 
{
	structLock = true;
	ASSERT((*semantics).assumeNotReserved(name), "variable name error");
	VERBOSE("BUILDER: New int: " CAT name);
	int32_t address = variables.addMember(semantics, name, MS_TYPE_INT);
	values[address] = value;
}

int32_t MSBuilder::createText (std::string value) 
{
	structLock = true;
	if (!(texts.find( value) != texts.end()))
	{
		texts.insert(std::make_pair( value, textIDCounter++));;
	}
	return nameTreeGet(texts, value);
}

void MSBuilder::addText (std::string varName, std::string value) 
{
	structLock = true;
	ASSERT((*semantics).assumeNotReserved(varName), "variable name error");
	// add string to tree
	int32_t textID = createText(value);
	int32_t address = variables.addMember(semantics, varName, MS_TYPE_TEXT);
	values[address] = textID;
}

void MSBuilder::addChars (std::string varName, int32_t numChars, std::string text) 
{
	StructDef* sd = (*semantics).addCharsType(numChars);
	int32_t address = variables.addMember(semantics, varName, (*sd).typeID);
	int32_t maxSize = (*sd).structSize;
	
	stringToIntsWithSize(text, values, address, maxSize);
}

int32_t MSBuilder::createStructDef (std::string name) 
{
	lockCheck();
	int32_t id = (*semantics).typeIDCounter++;
	StructDef* sd = new StructDef(name, id);
	(*semantics).addStructDef(name, id, sd);
	return id;
}

void MSBuilder::addCharsMember (int32_t structTypeID, std::string varName, int32_t numChars) 
{
	StructDef* sd = (*semantics).getType(structTypeID);
	StructDef* charsType = (*semantics).addCharsType(numChars);
	(*sd).addMember(semantics, varName, (*charsType).typeID);
}

int32_t MSBuilder::addMember (int32_t structTypeID, std::string varName, int32_t memberType) 
{
	StructDef* sd = (*semantics).getType(structTypeID);
	return (*sd).addMember(varName, memberType);
}

void MSBuilder::addArray (int32_t typeID, std::string arrayName, int32_t arraySize) 
{
	variables.addArray(semantics, arrayName, typeID, arraySize);
}

MSWriter MSBuilder::arrayItem (std::string arrayName, int32_t arrayIndex) 
{
	int32_t tag = variables.getMemberTag(arrayName);
	ASSERT((tag & OPERATION_MASK) == OP_ARRAY_MEMBER, "not an array");
	int32_t itemCount = variables.getMemberArrayItemCount(arrayName);
	ASSERT(arrayIndex >= 0 && arrayIndex < itemCount, "index out of bounds: " CAT arrayIndex CAT " / " CAT itemCount);
	StructDef* arrayItemType = (*semantics).getType((int32_t)(tag & VALUE_TYPE_MASK));
	int32_t itemSize = (*arrayItemType).structSize;
	int32_t address = variables.getMemberAddress(arrayName);
	address += arrayIndex * itemSize;
	
	return MSWriter( 		this, 		arrayItemType, 		address 		);
}

MSWriter MSBuilder::createStruct (std::string typeName, std::string varName) 
{
	StructDef* sd = (*semantics).getType(typeName);
	return createStruct((*sd).typeID, varName);
}

MSWriter MSBuilder::createStruct (int32_t typeID, std::string varName) 
{
	structLock = true;
	ASSERT((*semantics).assumeNotReserved(varName), "variable name error");
	VERBOSE("BUILDER: New struct: " CAT varName);
	StructDef* sd = (*semantics).getType(typeID);
	int32_t address = variables.addMember(semantics, varName, typeID);
	
	return MSWriter( 		this, 		sd, 		address 		);
}

int32_t MSBuilder::createGeneratedStruct (int32_t typeID, std::string varName) 
{
	return variables.addMember(semantics, varName, typeID);
}

void MSBuilder:: readStructCode (Array<int> code) 
{
	ClassMaker::findTypes((*semantics), code);
	ClassMaker::createStructDefs((*semantics), code);
}

void MSBuilder::generate () 
{
	(*byteCode).addInstructionWithData(OP_START_INIT, 1, BYTECODE_READ_ONLY, texts.size());
	
	// write global variable and other structure definitions
	(*semantics).writeStructDefs(byteCode);
	
	// save immutable texts
	// TODO: same as in Generator = make a function (?)
	
	int32_t numTexts = texts.size();
	Array<std::string>textArray(numTexts);
	
	for (const auto&  entry :  texts)
	{
		// (key, value) = (text, id)
		int32_t id = entry.second;
		ASSERT(id >= 0 && id < numTexts, "unexpected text ID");
		textArray[id] = entry.first.c_str();
	}
	for (int32_t i=1; i<numTexts; i++)
	{
		(*byteCode).codeTop = addTextInstruction(textArray[i], OP_ADD_TEXT, (*byteCode).code, (*byteCode).codeTop);
	}

	// initialize  values

	int32_t globalsSize = variables.structSize;
	(*byteCode).addWord(makeInstruction(OP_INIT_GLOBALS, globalsSize, 0));
	for (int32_t i=0; i<globalsSize; i++)
	{
		(*byteCode).addWord(values[i]);
	}
	(*byteCode).addInstruction(OP_END_INIT, 0 , 0);
}

MSCode* MSBuilder:: createMS () 
{
	MSCode* ms = new MSCode();
	(*ms).initBytecode(byteCode);
	return ms;
}

void MSBuilder::write (MSOutputStream & output) 
{
	(*byteCode).writeCode(output);
	output.close();
}


} // namespace meanscript(core)
// C++ END
