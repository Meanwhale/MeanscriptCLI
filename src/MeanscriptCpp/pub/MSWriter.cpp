#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSWriter::MSWriter (MSBuilder* _builder, StructDef* _sd, int32_t _address)
{
	builder = _builder;
	sd = _sd;
	baseAddress = _address;
}

void MSWriter::setInt (std::string name, int32_t value) 
{
	MSText tn (name);
	int32_t memberAddress = (*sd).getMemberAddress((&(tn)));
	ASSERT(((*sd).getMemberTag((&(tn))) & VALUE_TYPE_MASK) == MS_TYPE_INT, "not an integer type: " CAT name);
	(*builder).values[baseAddress + memberAddress] = value;
}

void MSWriter::setInt64 (std::string name, int64_t value) 
{
	MSText tn (name);
	int32_t memberAddress = (*sd).getMemberAddress((&(tn)));
	ASSERT(((*sd).getMemberTag((&(tn))) & VALUE_TYPE_MASK) == MS_TYPE_INT64, "not a 64-bit integer type: " CAT tn);
	(*builder).values[baseAddress + memberAddress]     = int64highBits(value);
	(*builder).values[baseAddress + memberAddress + 1] = int64lowBits(value);
}
void MSWriter::setFloat (std::string name, float value) 
{
	MSText tn (name);
	int32_t memberAddress = (*sd).getMemberAddress((&(tn)));
	ASSERT(((*sd).getMemberTag((&(tn))) & VALUE_TYPE_MASK) == MS_TYPE_FLOAT, "not a float type: " CAT tn);
	(*builder).values[baseAddress + memberAddress] = floatToIntBits(value);
}
void MSWriter::setFloat64 (std::string name, double value) 
{
	MSText tn (name);
	int32_t memberAddress = (*sd).getMemberAddress((&(tn)));
	ASSERT(((*sd).getMemberTag((&(tn))) & VALUE_TYPE_MASK) == MS_TYPE_FLOAT64, "not a 64-bit float type: " CAT tn);
	int64_t tmp = ((int64_t&)(*(&value)));
	(*builder).values[baseAddress + memberAddress]     = int64highBits(tmp);
	(*builder).values[baseAddress + memberAddress + 1] = int64lowBits(tmp);
}
void MSWriter::setBool (std::string name, bool value) 
{
	MSText tn (name);
	int32_t memberAddress = (*sd).getMemberAddress((&(tn)));
	ASSERT(((*sd).getMemberTag((&(tn))) & VALUE_TYPE_MASK) == MS_TYPE_BOOL, "not a boolean type: " CAT tn);
	(*builder).values[baseAddress + memberAddress] = (value ? 1 : 0);
}

void MSWriter::setText (std::string name, std::string value) 
{
	MSText tn (name);
	int32_t memberAddress = (*sd).getMemberAddress((&(tn)));
	ASSERT(((*sd).getMemberTag((&(tn))) & VALUE_TYPE_MASK) == MS_TYPE_TEXT, "not a text type: " CAT tn);
	int32_t textID = (*builder).createText(value);
	(*builder).values[baseAddress + memberAddress] = textID;
}
//pw.setChars("code", "abcdefg");
void MSWriter::setChars (std::string name, std::string value) 
{
	MSText tn (name);
	// get chars type
	int32_t memberAddress = (*sd).getMemberAddress((&(tn)));
	int32_t tag = (*sd).getMemberTag((&(tn)));
	int32_t type = (int32_t)(tag & VALUE_TYPE_MASK);
	StructDef* charsDef = (*(*builder).semantics).getType(type);
	ASSERT((*charsDef).isCharsDef(), "not a chars def.");
	//int32_t maxChars = (*charsDef).numCharsForCharsDef();
	int32_t size32 = (*charsDef).structSize;
	
	// TODO: check sizes
	// copy array
	stringToIntsWithSize(value, (*builder).values, baseAddress + memberAddress, size32);

}

} // namespace meanscript(core)
