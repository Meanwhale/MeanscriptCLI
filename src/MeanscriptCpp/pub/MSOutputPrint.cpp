
#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;


MSOutputPrint::MSOutputPrint ()
{
}


void MSOutputPrint::close ()
{
}

constexpr char const * hexs [] =
{
	"0","1","2","3",
	"4","5","6","7",
	"8","9","a","b",
	"c","d","e","f"
};

MSOutputPrint & MSOutputPrint:: printHex (int32_t h)
{
	for (int i = 28; i >= 0; i -=4 )
	{
		int index = (h>>i);
		index &= 0x0000000f;
		print(hexs[index]);
	}
	return (*this);
}

MSOutputPrint & MSOutputPrint:: endLine ()
{
	print("\n");
	return (*this);
}

} // namespace meanscript(core)
// C++ END
