
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

MCallback::MCallback (std::string _name, void (*_func)(MeanMachine &, MArgs &), int32_t _returnType, StructDef* _argStruct)
{
	name=_name;
	func=_func;
	returnType=_returnType;
	argStruct=_argStruct;
}

MCallback::~MCallback() { delete argStruct;};

} // namespace meanscript(core)
// C++ END
