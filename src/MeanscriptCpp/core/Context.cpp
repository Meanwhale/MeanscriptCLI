#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

Context::Context (MSText* _name, int32_t _functionID, int32_t _returnType) 
  :  variables(_name, _functionID)
{
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
