#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;


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
//		[int foo, vec3 pos, int32_t bar]
//			structSize = 5


StructDef::StructDef (MSText* _name, int32_t _typeID) 
{
	if (_name != 0) name = new MSText((*_name));
	else name = 0;
	typeID = _typeID;
	
	{ code.reset(			globalConfig.maxStructDefSize); code.fill(0); code.description =  "StructDef: structure definitions"; };
	{ memberOffset.reset(	globalConfig.maxStructMembers); memberOffset.fill(0); memberOffset.description =  "StructDef: structure members"; };
	{ nameOffset.reset(		globalConfig.maxStructMembers); nameOffset.fill(0); nameOffset.description =  "StructDef: member names"; };
	
	numMembers = 0;
	argsSize = -1; // set structSize after all arguments are set
	structSize = 0;
	codeTop = 0;

	// save name
	if (name != 0)
	{
		codeTop = addTextInstruction((*name), OP_STRUCT_DEF, code, codeTop);
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

StructDef::StructDef (MSText* _name, int32_t _typeID, int32_t _size) 
{
	// primitive: no need to initialize arrays (code, memberOffset, nameOffset)
	if (_name != 0) name = new MSText((*_name));
	else name = 0;
	typeID = _typeID;
	numMembers = -1;
	argsSize = -1; // set structSize after all arguments are set
	structSize = _size;
	codeTop = -1;
}

StructDef::StructDef (MSText* _name, int32_t _typeID, int32_t data, int32_t size, int32_t op) 
{
	// generic struct: no need to initialize arrays memberOffset and nameOffset
	if (_name != 0) name = new MSText((*_name));
	else name = 0;
	typeID = _typeID;
	
	{ code.reset( 2); code.fill(0); code.description =  "generic struct"; };
	code[0] = makeInstruction(op, 1, typeID);
	code[1] = data; // chars: number of characters
	codeTop = 2;
	
	numMembers = -1;
	argsSize = -1; // set structSize after all arguments are set
	structSize = size;
}

StructDef::~StructDef() { delete name; }

bool StructDef:: isCharsDef ()
{
	return codeTop > 0 && (int32_t)(code[0] & OPERATION_MASK) == OP_CHARS_DEF;
}

int32_t StructDef::numCharsForCharsDef () 
{
	ASSERT(isCharsDef(),"not a chars def.");
	return code[1];
}

int32_t StructDef:: addArray (Semantics* semantics, MSText* name, int32_t arrayType, int32_t itemCount) 
{	
	// array with it's item count at first, e.g.
	// 		vec2 [3] -> [3] [x0] [y0] [x1] [y1] [x2] [y2]
	//		"sizeof" being 7 here

	ASSERT(itemCount > 0 && itemCount < globalConfig.maxArraySize, "invalid array size");
	StructDef* sd = (*semantics).typeStructDefs[arrayType];
	ASSERT(sd != 0, "struct missing");
	int32_t singleMemberSize = (*sd).structSize;
	int32_t dataSize = itemCount * singleMemberSize; // "sizeof"
	
	// name saving instruction
	nameOffset[numMembers] = codeTop;
	codeTop = addTextInstruction((*name), OP_MEMBER_NAME, code, codeTop);
	
	// create tag
	memberOffset[numMembers] = codeTop;

	int32_t memberTag = makeInstruction(OP_ARRAY_MEMBER, 3, arrayType);
	code[codeTop] = memberTag;
	int32_t address = structSize;
	code[codeTop + 1] = address; // offset = sum of previous type sizes
	code[codeTop + 2] = dataSize;
	code[codeTop + 3] = itemCount;
	
	structSize += dataSize;
	
	memberNames.insert(std::make_pair( MSText((*name)), codeTop));;
	codeTop += 4;

	numMembers++;
	
	return address;
}

int32_t StructDef:: addMember (Semantics* semantics, MSText* name, int32_t type) 
{	
	// get size

	StructDef* sd = (*semantics).typeStructDefs[type];
	ASSERT(sd != 0, "struct missing");
	int32_t memberSize = (*sd).structSize;
	return addMember(name, type, memberSize);
}

int32_t StructDef:: addMember (MSText* name, int32_t type, int32_t memberSize) 
{		
	// name saving instruction

	if (name != 0)
	{
		nameOffset[numMembers] = codeTop;
		codeTop = addTextInstruction((*name), OP_MEMBER_NAME, code, codeTop);
	}
	else
	{
		nameOffset[numMembers] = -1;
	}
	// create tag

	memberOffset[numMembers] = codeTop;

	int32_t memberTag = makeInstruction(OP_STRUCT_MEMBER, 2, type);
	code[codeTop] = memberTag;
	int32_t address = structSize;
	code[codeTop + 1] = address; // offset = sum of previous type sizes
	code[codeTop + 2] = memberSize; // "sizeof"

	structSize += memberSize;
	
	if (name != 0) { memberNames.insert(std::make_pair( MSText((*name)), codeTop));; }
	
	codeTop += 3;

	numMembers++;
	
	return address;
}

int32_t StructDef:: getMemberTag (MSText* varName) 
{
	ASSERT((memberNames.find( (*varName)) != memberNames.end()), "undefined variable: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	return code[index];
}
int32_t StructDef:: getMemberAddress (MSText* varName) 
{
	ASSERT((memberNames.find( (*varName)) != memberNames.end()), "undefined variable: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	return code[index+1]; // see above for definition
}
int32_t StructDef:: getMemberSize (MSText* varName) 
{
	ASSERT((memberNames.find( (*varName)) != memberNames.end()), "undefined variable: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	return code[index+2]; // see above for definition
}
int32_t StructDef:: getMemberArrayItemCount (MSText* varName) 
{
	ASSERT((memberNames.find( (*varName)) != memberNames.end()), "undefined variable: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	int32_t tag = code[index];
	CHECK((tag & OPERATION_MASK) == OP_ARRAY_MEMBER, EC_SYNTAX, "not an array: " CAT varName);
	return code[index + 3]; // see above for definition
}
int32_t StructDef:: getMemberArrayItemCountOrNegative (MSText* varName) 
{
	ASSERT((memberNames.find( (*varName)) != memberNames.end()), "undefined variable: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	int32_t tag = code[index];
	if ((tag & OPERATION_MASK) != OP_ARRAY_MEMBER) return -1;
	return code[index + 3]; // see above for definition
}
int32_t StructDef:: getMemberArrayItemCountOrNegative (int32_t index) 
{
	ASSERT(indexInRange(index), "argument index out of range: " CAT index);
	int32_t tag = code[memberOffset[index]];
	if ((tag & OPERATION_MASK) != OP_ARRAY_MEMBER) return -1;
	return code[memberOffset[index] + 3]; // see above for definition
}
bool StructDef:: indexInRange(int32_t index)
{
	return index >= 0 && index < numMembers;
}
int32_t StructDef:: getMemberTag (int32_t index) 
{
	ASSERT(indexInRange(index), "argument index out of range: " CAT index);
	return code[memberOffset[index]];
}
int32_t StructDef:: getMemberAddress (int32_t index) 
{
	ASSERT(indexInRange(index), "argument index out of range: " CAT index);
	return code[memberOffset[index] + 1]; // see above for definition
}
int32_t StructDef:: getMemberSize (int32_t index) 
{
	ASSERT(indexInRange(index), "argument index out of range: " CAT index);
	return code[memberOffset[index] + 2]; // see above for definition
}
std::string StructDef:: getMemberName (int32_t index) 
{
	ASSERT(indexInRange(index), "argument index out of range: " CAT index);
	int32_t offset = nameOffset[index];
	return readStringFromIntArray(code,  offset + 2,  code[offset + 1]);;
}
void StructDef::print () 
{
	VERBOSE(HORIZONTAL_LINE);
	VERBOSE("STRUCT CODE: " CAT name);
	VERBOSE(HORIZONTAL_LINE);
	if (numMembers == 0)
	{
		VERBOSE("        empty");
	}
	else
	{
		if (globalConfig.verboseOn()) printBytecode(code, codeTop, -1, true);
	}
	VERBOSE(HORIZONTAL_LINE);
}

} // namespace meanscript(core)
