#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSText::MSText (const std::string & src) 
{
	uint8_t * bytes = (uint8_t *)src.c_str();
	_init(bytes, 0, length((const char*)bytes));
}

MSText::MSText (const uint8_t src[], int32_t start, int32_t length) 
{
	_init(src, start, length);
}

void MSText::_init(const uint8_t src[], int32_t start, int32_t length)
{
	{ data.reset( (length / 4) + 2); data.fill(0); data.description =  "MSText"; };
	data[0] = length;
	bytesToInts(src,start,data,1,length);
}

MSText::MSText (const MSText & src) 
{
	makeCopy(src.data,0);
}

MSText::MSText (Array<int> & src) 
{
	makeCopy(src,0);
}

MSText::MSText (Array<int> & src, int32_t start) 
{
	makeCopy(src,start);
}


bool MSText::match(const MSText & t)
{
	return compare(t) == 0;
}

bool MSText::match(const char * s) 
{
	return (compareStrings(s,getString()));
}

Array<int> & MSText::getData()
{
	return data;
}

int32_t MSText::numBytes() const
{
	// count is without the ending character
	return data[0];
}
int32_t MSText::dataSize() const
{
	return data.length();
}
int32_t MSText::byteAt(int32_t index) const 
{
	ASSERT(index >= 0 && index <= data[0],"index overflow");
	return ((data[(index / 4) + 1]) >> ((index % 4) * 8) & 0x000000ff);
}
int32_t MSText::write(Array<int> & trg, int start) const 
{
	for (int32_t i=0; i<data.length(); i++)
	{
		trg[start + i] = data[i];
	}
	return 	start + data.length();
}
void MSText::makeCopy( const Array<int> & src, int start)
{
	int32_t numChars = src[start];
	int32_t size32 = (numChars/4) + 2;
	{ data.reset( size32); data.fill(0); data.description =  "MSText"; };
	for (int32_t i=0; i<size32; i++)
	{
		data[i] = src[i+start];
	}
}
int32_t MSText::compare (const MSText & text) const
{
	// returns -1 (less), 1 (greater), or 0 (equal)
	
	if (data.length() != text.data.length())
	{
		return data.length() > text.data.length() ? 1 : -1;
	}
	
	for (int32_t i=0; i<data.length(); i++)
	{
		if (data[i] != text.data[i])
		{
			return data[i] > text.data[i] ? 1 : -1;
		}
	}
	return 0; // equals
}
void MSText::check() 
{
	int32_t size32 = (data[0]/4) + 2;
	ASSERT(data.length() == size32,"corrupted MSText object (size don't match)");
	ASSERT(byteAt(data[0]) == 0, "corrupted MSText object (no zero byte at end)");
}
std::string MSText::getString() 
{
	check();
	return readStringFromIntArray(data, 1, data[0]);;
}

} // namespace meanscript(core)
