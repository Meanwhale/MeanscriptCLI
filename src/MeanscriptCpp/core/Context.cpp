
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

Context::Context (std::string _name, int32_t _functionID, int32_t _returnType) 
  :  variables(_name, _functionID)
{
	name = _name;
	functionID = _functionID;
	returnType = _returnType;
	tagAddress = -1;
	
	codeNode = 0;
	codeStartAddress = -1;
	codeEndAddress = -1;
}

void Context::print ()
{
	VERBOSE("Context.print(): TODO");
}

} // namespace meanscript(core)
// C++ END
