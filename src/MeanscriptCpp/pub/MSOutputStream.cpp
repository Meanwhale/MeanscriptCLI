
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSOutputStream::MSOutputStream ()
{
}

void MSOutputStream:: writeInt (int32_t i) 
{
	writeByte((uint8_t)((i>>24) & 0xff));
	writeByte((uint8_t)((i>>16) & 0xff));
	writeByte((uint8_t)((i>>8) & 0xff));
	writeByte((uint8_t)(i & 0xff));
}


} // namespace meanscript(core)
// C++ END
