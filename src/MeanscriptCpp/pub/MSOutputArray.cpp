
#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSOutputArray::MSOutputArray ()
{
	maxSize = CFG_OUTPUT_ARRAY_SIZE;
	{ buffer.reset(maxSize); };
	index = 0;
}


void MSOutputArray::close ()
{
	index = -1;
}


void MSOutputArray:: writeByte (uint8_t b) 
{
	CHECK(index != -1, EC_DATA, "output closed");
	CHECK(index < maxSize, EC_DATA, "output: buffer overflow");
	buffer[index++] = b;
}

void MSOutputArray:: print ()
{
	PRINT("TODO");
}

MSOutputArray::~MSOutputArray() {  };

} // namespace meanscript(core)
// C++ END
