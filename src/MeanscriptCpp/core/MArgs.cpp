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
