
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


StructDef::StructDef (std::string _name, int32_t _typeID) 
{
	name = _name;
	typeID = _typeID;
	
	{ code.reset(			globalConfig.maxStructDefSize); code.fill(0); code.description =  "StructDef: structure definitions"; };
	{ memberOffset.reset(	globalConfig.maxStructMembers); memberOffset.fill(0); memberOffset.description =  "StructDef: structure members"; };
	
	numMembers = 0;
	argsSize = -1; // set structSize after all arguments are set
	structSize = 0;
	codeTop = 0;

	// save name
	codeTop = addTextInstruction(name, OP_STRUCT_DEF, code, codeTop);
	
	// change type to start tag
	code[0] = makeInstruction(OP_STRUCT_DEF, codeTop-1, typeID);
}

StructDef::~StructDef() { }

int32_t StructDef:: addArray (Semantics* semantics, std::string name, int32_t arrayType, int32_t itemCount) 
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
	codeTop = addTextInstruction(name, OP_MEMBER_NAME, code, codeTop);
	
	// create tag
	memberOffset[numMembers] = codeTop;

	int32_t memberTag = makeInstruction(OP_ARRAY_MEMBER, 3, arrayType);
	code[codeTop] = memberTag;
	int32_t address = structSize;
	code[codeTop + 1] = address; // offset = sum of previous type sizes
	code[codeTop + 2] = dataSize;
	code[codeTop + 3] = itemCount;
	
	structSize += dataSize;
	
	memberNames.insert(std::make_pair( name, codeTop));;
	codeTop += 4;

	numMembers++;
	
	return address;
}

int32_t StructDef:: addMember (Semantics* semantics, std::string name, int32_t type) 
{	
	// get size

	// TODO: StructDef löytyy kaikille tyypeille joten if on turha ja huono jos koko > 1
	int32_t memberSize = 1;
	if (type >= MAX_MS_TYPES)
	{
		StructDef* sd = (*semantics).typeStructDefs[type];
		ASSERT(sd != 0, "struct missing");
		memberSize = (*sd).structSize;
	}
	
	return addMember(name, type, memberSize);
}

int32_t StructDef:: addMember (std::string name, int32_t type) 
{	
	ASSERT(type >= 0 && type < MAX_MS_TYPES, "invalid member type");
	return addMember(name, type, 1);
}

int32_t StructDef:: addMember (std::string name, int32_t type, int32_t memberSize) 
{		
	// name saving instruction

	codeTop = addTextInstruction(name, OP_MEMBER_NAME, code, codeTop);

	// create tag

	memberOffset[numMembers] = codeTop;

	int32_t memberTag = makeInstruction(OP_STRUCT_MEMBER, 2, type);
	code[codeTop] = memberTag;
	int32_t address = structSize;
	code[codeTop + 1] = address; // offset = sum of previous type sizes
	code[codeTop + 2] = memberSize; // "sizeof"

	structSize += memberSize;
	
	memberNames.insert(std::make_pair( name, codeTop));;
	codeTop += 3;

	numMembers++;
	
	return address;
}

int32_t StructDef:: getMemberTag (std::string varName) 
{
	ASSERT((memberNames.find( varName) != memberNames.end()), "member not found: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	return code[index];
}
int32_t StructDef:: getMemberAddress (std::string varName) 
{
	ASSERT((memberNames.find( varName) != memberNames.end()), "member not found: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	return code[index+1]; // see above for definition
}
int32_t StructDef:: getMemberSize (std::string varName) 
{
	ASSERT((memberNames.find( varName) != memberNames.end()), "member not found: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	return code[index+2]; // see above for definition
}
int32_t StructDef:: getMemberArrayItemCount (std::string varName) 
{
	ASSERT((memberNames.find( varName) != memberNames.end()), "member not found: " CAT varName);
	int32_t index = nameTreeGet(memberNames, varName);
	int32_t tag = code[index];
	CHECK((tag & OPERATION_MASK) == OP_ARRAY_MEMBER, EC_SYNTAX, "not an array: " CAT varName);
	return code[index + 3]; // see above for definition
}
int32_t StructDef:: getMemberArrayItemCountOrNegative (std::string varName) 
{
	ASSERT((memberNames.find( varName) != memberNames.end()), "member not found: " CAT varName);
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
		if (globalConfig.verboseOn()) printData(code, codeTop, -1, true);
	}
	VERBOSE(HORIZONTAL_LINE);
}

} // namespace meanscript(core)
// C++ END
