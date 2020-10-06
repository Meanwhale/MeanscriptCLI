
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
	int32_t memberAddress = (*sd).getMemberAddress(name);
	ASSERT(((*sd).getMemberTag(name) & VALUE_TYPE_MASK) == MS_TYPE_INT, "not an integer type: " CAT name);
	(*builder).values[baseAddress + memberAddress] = value;
}

void MSWriter::setText (std::string name, std::string value) 
{
	int32_t memberAddress = (*sd).getMemberAddress(name);
	ASSERT(((*sd).getMemberTag(name) & VALUE_TYPE_MASK) == MS_TYPE_TEXT, "not a text type: " CAT name);
	int32_t textID = (*builder).createText(value);
	(*builder).values[baseAddress + memberAddress] = textID;
}

} // namespace meanscript(core)
// C++ END
