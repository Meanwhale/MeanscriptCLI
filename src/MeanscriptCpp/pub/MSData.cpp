
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

float MSData::getFloat () 
{
	ASSERT(getType() != MS_TYPE_FLOAT, "not a float");
	return (*reinterpret_cast<float*>(&((*dataCode)[dataIndex])));
}

float MSData::getFloat (std::string name) 
{
	int32_t address = getMemberAddress(name, MS_TYPE_FLOAT);
	CHECK(address >= 0, EC_DATA, "unknown name");
	return (*reinterpret_cast<float*>(&((*dataCode)[address])));
}

int32_t MSData::getInt () 
{
	ASSERT(getType() >= MS_TYPE_INT, "not an integer");
	return (*dataCode)[dataIndex];
}

int32_t MSData::getInt (std::string name) 
{
	int32_t address = getMemberAddress(name, MS_TYPE_INT);
	CHECK(address >= 0, EC_DATA, "unknown name");
	return (*dataCode)[address];
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

Array<int> nameIntsTmp(CFG_MAX_NAME_LENGTH);

int32_t MSData::getMemberTagAddress (std::string name, bool isArray) 
{
	ASSERT(isStruct(), "struct expected");
	
	int32_t i = (*mm).types[getType()];
	int32_t code = (*structCode)[i];
	ASSERT((code & OPERATION_MASK) == OP_STRUCT_DEF, "struct def. expected");
	i += instrSize(code) + 1;
	code = (*structCode)[i];
	
	// change 'name' to int32_t array to compares names
	
	stringToIntsWithSize(name, nameIntsTmp, 0);
	
	// go thru members
	
	while ((code & OPERATION_MASK) == OP_MEMBER_NAME)
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

void MSData:: printData (int32_t depth, std::string name) 
{
		
	for (int32_t x=0; x<depth; x++) PRINTN("    ");
	PRINTN(name);
	
	if (!isStruct())
	{
		if (getType() == MS_TYPE_FLOAT)
		{
			PRINT((*reinterpret_cast<float*>(&((*dataCode)[dataIndex]))));
		}
		else if (getType() == MS_TYPE_TEXT)
		{
			PRINT(getText((*dataCode)[dataIndex]));
		}
		else
		{
			PRINT((*dataCode)[dataIndex]);
		}
	}
	else
	{
		PRINT("");
		// NOTE: similar to getMemberTagAddress()
		
		int32_t i = (*mm).types[getType()];
		int32_t code = (*structCode)[i];
		ASSERT((code & OPERATION_MASK) == OP_STRUCT_DEF, "printData: struct def. expected");
		i += instrSize(code) + 1;
		code = (*structCode)[i];
		while ((code & OPERATION_MASK) == OP_MEMBER_NAME)
		{
			
			std::string s = readStringFromIntArray((*structCode),  i + 2,  (*structCode)[i+1]);;
	
			i += instrSize(code) + 1;
			code = (*structCode)[i];
			
			if ((code & OPERATION_MASK) == OP_STRUCT_MEMBER)
			{
				int32_t dataAddress = dataIndex + (*structCode)[i + 1];
				MSData d = MSData (mm, i, dataAddress, false);
				s += ": ";
				d.printData(depth + 1, s);
			}
			else if ((code & OPERATION_MASK) == OP_ARRAY_MEMBER)
			{
				int32_t dataAddress = dataIndex + (*structCode)[i + 1];
				MSDataArray a = MSDataArray (mm, i, dataAddress);
					
				for (int32_t x=0; x<depth+1; x++) PRINTN("    ");
				PRINTN(s);
				PRINTN(": ");
				
				// iterate thru array items

				PRINT("");
				for (int32_t n=0; n<a.getArrayItemCount(); n++)
				{
					MSData d = a.getAt(n);
					for (int32_t x=0; x<depth; x++) PRINTN("  ");
					std::string indexText = std::to_string( n);
					std::string tmp = "[";
					tmp += indexText;
					tmp += "] ";
					d.printData(depth + 1, tmp);
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
// C++ END
