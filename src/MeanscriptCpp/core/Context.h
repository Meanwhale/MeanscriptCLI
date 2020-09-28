//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscriptcore {
class Context
{
public:
 std::string name;
 int32_t functionID;
 int32_t returnType;
 int32_t tagAddress;
 int32_t codeStartAddress;
 int32_t codeEndAddress;
 StructDef variables;
 int32_t numArgs; // number of arguments in the beginning of 'variables' struct
 MNode* codeNode; // code block node where the function code is
Context(std::string _name, int32_t _functionID, int32_t _returnType);
void print();
};
} // namespace meanscript(core)
// C++ header END
