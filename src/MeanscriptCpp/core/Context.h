namespace meanscriptcore {
using namespace meanscript;
class Context
{
public:
 int32_t functionID;
 int32_t returnType;
 int32_t tagAddress;
 int32_t codeStartAddress;
 int32_t codeEndAddress;
 StructDef variables;
 int32_t numArgs; // number of arguments in the beginning of 'variables' struct
 MNode* codeNode; // code block node where the function code is
Context(MSText* _name, int32_t _functionID, int32_t _returnType);
void print();
};
} // namespace meanscript(core)
