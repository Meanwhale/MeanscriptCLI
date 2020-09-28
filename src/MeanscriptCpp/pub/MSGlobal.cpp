
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;
	
void MSGlobal::setVerbose (bool b)
{
	verbose = b;
}

} // namespace meanscript(core)
// C++ END
