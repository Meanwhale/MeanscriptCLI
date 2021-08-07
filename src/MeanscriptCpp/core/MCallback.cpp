#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

MCallback::MCallback (void (*_func)(MeanMachine &, MArgs &), int32_t _returnType, StructDef* _argStruct)
{
	func=_func;
	returnType=_returnType;
	argStruct=_argStruct;
}

MCallback::~MCallback() { delete argStruct;};

} // namespace meanscript(core)
