#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSInputArray::MSInputArray (MSOutputArray* output)
{
	// copy array
	size = (*output).index;
	{ buffer.reset( size); };
	for (int32_t i=0; i<size; i++) buffer[i] = (*output).buffer[i];
	index = 0;
}

MSInputArray::MSInputArray (std::string & s)
{
	buffer.clone((uint8_t*) s.c_str(),  s.length());
	size = buffer.length();
	index = 0;
}


int32_t MSInputArray:: getByteCount ()
{
	return size;
}


int32_t MSInputArray:: readByte () 
{
	//CHECK(!end(), EC_DATA, "readInt: buffer overflow");
	if (end()) return -1;
	return (((int) buffer[index++]) & 0xff);
}


bool MSInputArray:: end ()
{
	return index >= size;
}


void MSInputArray:: close () 
{
}

} // namespace meanscript(core)
