
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

MArgs::MArgs (ByteCode* _byteCode, StructDef* _structDef, int32_t _base)
{
	byteCode = _byteCode;
	structDef = _structDef;
	baseIndex = _base;
	valid = true;
}

} // namespace meanscript(core)
// C++ END
