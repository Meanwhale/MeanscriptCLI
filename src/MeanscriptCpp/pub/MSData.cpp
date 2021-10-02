#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSData::MSData (MeanMachine* _mm, int32_t _tagAddress, int32_t _dataIndex, bool arrayItem) 
{
	mm = _mm;
	
	structCode = (*mm).getStructCode();
	dataCode = (*mm).getDataCode();

	mm = _mm;
	tagAddress = _tagAddress;
	dataIndex = _dataIndex;
	tag = (*structCode)[tagAddress];
	
	ASSERT(arrayItem == isArrayItem(), "array item error");
}

int32_t MSData::getType ()
{
	return (int32_t)(tag & VALUE_TYPE_MASK);
}

bool MSData::isStruct ()
{
	return ((getType() == 0) || (getType() >= MAX_MS_TYPES));
}

bool MSData::hasData (std::string name) 
{
	int32_t memberTagAddress = getMemberTagAddress(name, false);
	return memberTagAddress >= 0;
}

bool MSData::hasArray (std::string name) 
{
	int32_t memberTagAddress = getMemberTagAddress(name, true);
	return memberTagAddress >= 0;
}

std::string MSData::getText () 
{
	ASSERT(getType() >= MS_TYPE_TEXT, "not a text");
	return getText((*dataCode)[dataIndex]);
}

std::string MSData::getText (std::string name) 
{
	int32_t address = getMemberAddress(name, MS_TYPE_TEXT);
	CHECK(address >= 0, EC_DATA, "unknown name");
	return getText((*dataCode)[address]);
}
std::string MSData:: getText (int32_t id)
{
	if (id == 0) return "";
	int32_t address = (*mm).texts[id];
	int32_t numChars = (*structCode)[address + 1];
	std::string s = readStringFromIntArray((*structCode),  address + 2,  numChars);;
	return s;
}
MSText MSData:: getMSText (int32_t id) 
{
	if (id == 0) return MSText("");
	int32_t address = (*mm).texts[id]; // operation address
	return MSText((*structCode),address+1);
}

bool MSData::isChars (int32_t typeID)
{
	int32_t charsTypeAddress = (*mm).types[typeID];
	int32_t charsTypeTag = (*structCode)[charsTypeAddress];
	return (charsTypeTag & OPERATION_MASK) == OP_CHARS_DEF;
}

int32_t MSData::getCharsSize (int32_t typeID)
{
	int32_t index = (*mm).types[typeID];
	int32_t typeTagID = (*structCode)[index] & VALUE_TYPE_MASK;
	int32_t potentialCharsTag = (*structCode)[(*mm).types[typeTagID]];
	if ((potentialCharsTag & OPERATION_MASK) == OP_CHARS_DEF)
	{
		return (*structCode)[(*mm).types[typeTagID] + 1];
	}
	else return -1;
}

std::string MSData::getChars (std::string name) 
{
	int32_t memberTagAddress = getMemberTagAddress(name, false);
	
	int32_t charsTag = (*structCode)[memberTagAddress];
	int32_t typeID = instrValueTypeID(charsTag);
	CHECK(isChars(typeID), EC_DATA, "not chars");
	
	CHECK(memberTagAddress >= 0, EC_DATA, "not found: " CAT name);
	int32_t address = dataIndex + (*structCode)[memberTagAddress + 1];
	int32_t numChars = (*dataCode)[address];
	std::string s = readStringFromIntArray((*dataCode),  address + 1,  numChars);;
	return s;
}

float MSData::getFloat () 
{
	ASSERT(getType() != MS_TYPE_FLOAT, "not a float");
	return ((float&)(*(&(*dataCode)[dataIndex])));
}

float MSData::getFloat (std::string name) 
{
	int32_t address = getMemberAddress(name, MS_TYPE_FLOAT);
	CHECK(address >= 0, EC_DATA, "unknown name");
	return ((float&)(*(&(*dataCode)[address])));
}

int32_t MSData::getInt () 
{
	ASSERT(getType() >= MS_TYPE_INT, "not a 32-bit integer");
	return (*dataCode)[dataIndex];
}


int32_t MSData::getInt (std::string name) 
{
	int32_t address = getMemberAddress(name, MS_TYPE_INT);
	CHECK(address >= 0, EC_DATA, "unknown name");
	return (*dataCode)[address];
}

bool MSData::getBool () 
{
	ASSERT(getType() >= MS_TYPE_BOOL, "not a bool integer");
	return (*dataCode)[dataIndex] != 0;
}

bool MSData::getBool (std::string name) 
{
	int32_t address = getMemberAddress(name, MS_TYPE_BOOL);
	CHECK(address >= 0, EC_DATA, "unknown name");
	return (*dataCode)[address] != 0;
}

int64_t MSData::getInt64 () 
{
	return getInt64At(dataIndex);
}

int64_t MSData::getInt64 (std::string name) 
{
	int32_t address = getMemberAddress(name, MS_TYPE_INT64);
	return getInt64At(address);
}
int64_t MSData::getInt64At (int address) 
{
	int32_t a = (*dataCode)[address];
	int32_t b = (*dataCode)[address+1];
	int64_t i64 = intsToInt64(a,b);
	return i64;
}

double MSData::getFloat64 () 
{
	return getFloat64At(dataIndex);
}

double MSData::getFloat64 (std::string name) 
{
	int32_t address = getMemberAddress(name, MS_TYPE_FLOAT64);
	return getFloat64At(address);
}
double MSData::getFloat64At (int address) 
{
	int64_t i64 = getInt64At(address);
	return ((double&)(*(&i64)));
}

bool MSData::isArrayItem ()
{
	// NOTE: this should be true only if returned from MSDataArray 
	return (tag & OPERATION_MASK) == OP_ARRAY_MEMBER;
}

MSDataArray MSData::getArray (std::string name) 
{
	int32_t arrayTagAddress = getMemberTagAddress(name, true);
	CHECK(arrayTagAddress >= 0, EC_DATA, "not found: " CAT name);
	int32_t arrayTag = (*structCode)[arrayTagAddress];
	CHECK((arrayTag & OPERATION_MASK) == OP_ARRAY_MEMBER, EC_DATA, "not an array");
	int32_t dataAddress = dataIndex + (*structCode)[arrayTagAddress + 1];
	return MSDataArray (mm, arrayTagAddress, dataAddress);
}

MSData MSData::getMember (std::string name) 
{
	int32_t memberTagAddress = getMemberTagAddress(name, false);
	CHECK(memberTagAddress >= 0, EC_DATA, "not found: " CAT name);
	int32_t dataAddress = dataIndex + (*structCode)[memberTagAddress + 1];
	return MSData (mm, memberTagAddress, dataAddress, false);
}

int32_t MSData::getMemberAddress (std::string name, int32_t type) 
{
	int32_t memberTagAddress = getMemberTagAddress(name, false);
	CHECK(memberTagAddress >= 0, EC_DATA, "not found: " CAT name);
	CHECK((((*structCode)[memberTagAddress]) & VALUE_TYPE_MASK) == type, EC_DATA, "wrong type");
	return dataIndex + (*structCode)[memberTagAddress + 1];
}

int32_t MSData::getMemberAddress (std::string name) 
{
	int32_t memberTagAddress = getMemberTagAddress(name, false);
	CHECK(memberTagAddress >= 0, EC_DATA, "not found: " CAT name);
	// address of this data + offset of the member
	return dataIndex + (*structCode)[memberTagAddress + 1];
}

int32_t MSData::getMemberTagAddress (std::string name, bool isArray) 
{
	ASSERT(isStruct(), "struct expected");
	
	int32_t i = (*mm).types[getType()];
	int32_t code = (*structCode)[i];
	ASSERT((code & OPERATION_MASK) == OP_STRUCT_DEF, "struct def. expected");
	i += instrSize(code) + 1;
	code = (*structCode)[i];
	
	// change 'name' to int32_t array to compares names
	
	Array<int> nameIntsTmp(globalConfig.maxNameLength);
	
	stringToIntsWithSize(name, nameIntsTmp, 0, globalConfig.maxNameLength);
	
	// go thru members
	
	while ((code & OPERATION_MASK) == OP_MEMBER_NAME) // TODO: handle the case of "no name"
	{
		//VERBOSE("check member name");
		//VERBOSE("member: " + readStringFromIntArray((*structCode),  i+2,  (*structCode)[i+1]););

		// compare names
		if (intStringsWithSizeEquals(nameIntsTmp, 0, (*structCode), i+1))
		{
			VERBOSE("MATCH!");
			i += instrSize(code) + 1;
			code = (*structCode)[i];
			
			if (isArray)	{ ASSERT((code & OPERATION_MASK) == OP_ARRAY_MEMBER, "array expected"); }
			else			{ ASSERT((code & OPERATION_MASK) == OP_STRUCT_MEMBER, "array not expected"); };
			
			return i;
		}

		i += instrSize(code) + 1;
		code = (*structCode)[i];
		
		ASSERT((code & OPERATION_MASK) == OP_STRUCT_MEMBER || (code & OPERATION_MASK) == OP_ARRAY_MEMBER, "struct member expected");

		i += instrSize(code) + 1;
		code = (*structCode)[i];
	}	
	return -1;
}

void MSData:: printType (MSOutputPrint & op, int32_t typeID) 
{
	if (typeID < MAX_MS_TYPES) op.print(primitiveNames[typeID]);
	else
	{
		int32_t charsSizeOrNegative = getCharsSize(typeID);
		if (charsSizeOrNegative >= 0)
		{
			op.print("chars[");
			op.print(charsSizeOrNegative);
			op.print("]");
		}
		else
		{
			int32_t index = (*mm).types[typeID];
			MSText typeName ((*structCode),index+1);
			op.print(typeName);
		}
	}
}

void MSData:: printData (MSOutputPrint & op, int32_t depth, std::string name) 
{
	//for (int32_t x=0; x<depth; x++) op.print("    ");
	
	if (!isStruct())
	{
		if (depth != 1) op.print(name);
		op.print(": ");
		if (getType() == MS_TYPE_INT)
		{
			op.print((*dataCode)[dataIndex]);
		}
		else if (getType() == MS_TYPE_INT64)
		{
			op.print(intsToInt64((*dataCode)[dataIndex], (*dataCode)[dataIndex+1]));
		}
		else if (getType() == MS_TYPE_FLOAT)
		{
			op.print(((float&)(*(&(*dataCode)[dataIndex]))));
		}
		else if (getType() == MS_TYPE_FLOAT64)
		{
			op.print(getFloat64At(dataIndex));
		}
		else if (getType() == MS_TYPE_TEXT)
		{
			MSText tmp = getMSText((*dataCode)[dataIndex]);
			op.print("\"");
			op.printIntsToChars(tmp.getData(), 1, tmp.numBytes(), true);
			op.print("\"");
		}
		else if (getType() == MS_TYPE_BOOL)
		{
			if ((*dataCode)[dataIndex] == 0) op.print("false");
			else op.print("true");
		}
		else
		{
			CHECK(false, EC_DATA, "printData: unhandled data type: " CAT getType());
		}
		op.print("\n");
	}
	else
	{	
		// NOTE: similar to getMemberTagAddress()
		
		int32_t i = (*mm).types[getType()];
		int32_t code = (*structCode)[i];
		
		if ((code & OPERATION_MASK) == OP_CHARS_DEF)
		{
			int32_t numChars = (*dataCode)[dataIndex + 0];
			if (depth != 1) op.print(name);
			op.print(": \"");
			op.printIntsToChars((*dataCode), dataIndex + 1, numChars, true);
			op.print("\"\n");
			return;
		}
		if (depth == 1) op.print("\n");
					
		ASSERT((code & OPERATION_MASK) == OP_STRUCT_DEF, "printData: struct def. expected");
				
		i += instrSize(code) + 1;
		code = (*structCode)[i];
		while ((code & OPERATION_MASK) == OP_MEMBER_NAME)
		{
			std::string s = readStringFromIntArray((*structCode),  i + 2,  (*structCode)[i+1]);;
	
			i += instrSize(code) + 1;
			code = (*structCode)[i];
			
			// print type name
			
			if (depth == 0) 
			{
				int32_t typeID = (int32_t)(code & VALUE_TYPE_MASK);
				printType(op, typeID);
				op.print(" ");
				op.print(s);
			}
			
			if ((code & OPERATION_MASK) == OP_STRUCT_MEMBER)
			{
				int32_t dataAddress = dataIndex + (*structCode)[i + 1];
				MSData d = MSData (mm, i, dataAddress, false);
				if (depth > 0) d.printData(op, depth + 1, name + "." + s);
				else d.printData(op, depth + 1, s);
			}
			else if ((code & OPERATION_MASK) == OP_ARRAY_MEMBER)
			{
				int32_t dataAddress = dataIndex + (*structCode)[i + 1];
				MSDataArray a = MSDataArray (mm, i, dataAddress);
					
				//for (int32_t x=0; x<depth+1; x++) op.print("    ");
				//op.print(s);
				
				
				// iterate thru array items

				for (int32_t n=0; n<a.getArrayItemCount(); n++)
				{
					MSData d = a.getAt(n);
					//for (int32_t x=0; x<depth; x++) op.print("  ");


					std::string tmp = "";
					if (depth > 0) tmp = name + "." + s;
					else tmp = s;	
					
					std::string indexText = std::to_string( n);
					tmp += "[";
					tmp += indexText;
					tmp += "]";
					d.printData(op, depth + 1, tmp);
				}
			}
			else
			{
				ASSERT(false, "broken struct code");
			}

			i += instrSize(code) + 1;
			code = (*structCode)[i];
		}
	}
}

MSData::~MSData() { };

} // namespace meanscript(core)
