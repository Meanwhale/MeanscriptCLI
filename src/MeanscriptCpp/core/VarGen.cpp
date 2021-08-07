#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

VarGen::VarGen (int32_t _size, int32_t _tag, int32_t _address, int32_t _arraySize, bool _isReference)
{
	size = _size;
	tag = _tag;
	address = _address;
	arraySize = _arraySize;
	isReference = _isReference;
}

} // namespace meanscript(core)
