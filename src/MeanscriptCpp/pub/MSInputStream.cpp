
#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSInputStream::MSInputStream ()
{
}


int32_t MSInputStream:: readInt () 
{
	// bytes:	b[0] b[1] b[2] b[3] b[4] b[5] b[6] b[7]   ...
	// ints:	_________i[0]______|_________i[1]______|_ ...
	
	int32_t i = 0;
	i |= (int32_t)((readByte() << 24)		& 0xff000000);
	i |= (int32_t)((readByte() << 16)		& 0x00ff0000);
	i |= (int32_t)((readByte() << 8)			& 0x0000ff00);
	i |= (int32_t)((readByte())				& 0x000000ff);
	return i;
}

void MSInputStream:: readArray (Array<int> & trg, int32_t numInts) 
{
	CHECK(numInts <=  (getByteCount() * 4) + 1, EC_DATA, "readArray: buffer overflow");
	for (int32_t i=0; i < numInts; i++)
	{
		trg[i] = readInt();
	}
	ASSERT(end(), "all bytes not read");
}
} // namespace meanscript(core)
// C++ END
