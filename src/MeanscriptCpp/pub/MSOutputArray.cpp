#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSOutputArray::MSOutputArray ()
{
	maxSize = globalConfig.outputArraySize;
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

} // namespace meanscript(core)
