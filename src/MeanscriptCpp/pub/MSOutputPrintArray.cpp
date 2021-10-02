#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSOutputPrint& MSOutputPrintArray::print(const char * x)
{
	for (int i=0; x[i]!=0; i++)	print(x[i]); 
	return (*this);
}

MSOutputPrintArray::MSOutputPrintArray ()
{
	maxSize = globalConfig.outputArraySize;
	{ buffer.reset(maxSize); };
	index = 0;
}


void MSOutputPrintArray::close () 
{
	writeByte((uint8_t)0);
}


void MSOutputPrintArray:: writeByte (uint8_t b) 
{
	CHECK(index != -1, EC_DATA, "output closed");
	CHECK(index < maxSize, EC_DATA, "output: buffer overflow");
	buffer[index++] = b;
}


MSOutputPrint & MSOutputPrintArray:: print (char x) 
{
	writeByte((uint8_t)x);
	return (*this);
}


MSOutputPrint & MSOutputPrintArray:: print (std::string x) 
{
	Array<uint8_t> buffer;
	buffer.clone((uint8_t*)x.c_str(), x.length());
	for (int32_t i = 0; i < buffer.length(); i++)
	{
		writeByte(buffer[i]);
	}
	return (*this);
}

const char * MSOutputPrintArray::getString()
{
	return (const char *) buffer.get();
}

void MSOutputPrintArray:: print () 
{
	PRINT("[[[MSOutputPrint.print: TODO]]]");
}

} // namespace meanscript(core)
