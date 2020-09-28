//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscriptcore {
class StructDef
{
public:
 int32_t typeID;
 Array<int32_t> code;
 Array<int32_t> memberOffset; // memberOffset[n] = code address of _n_th member
 std::string name;
 int32_t numMembers;
 int32_t argsSize;
 int32_t structSize;
 int32_t codeTop;
 std::map<std::string, int> memberNames;
int32_t addMember(Semantics* semantics, std::string name, int32_t type);
int32_t addMember(std::string name, int32_t type, int32_t size);
int32_t addMember(std::string name, int32_t type);
int32_t addArray (Semantics* semantics, std::string name, int32_t arrayType, int32_t itemCount);
void addNameInstruction (std::string text);
bool indexInRange(int32_t index);
int32_t getMemberTag(std::string varName);
int32_t getMemberAddress(std::string varName);
int32_t getMemberSize(std::string varName);
int32_t getMemberTag(int32_t index);
int32_t getMemberAddress(int32_t index);
int32_t getMemberSize(int32_t index);
int32_t getMemberArrayItemCount (std::string varName);
int32_t getMemberArrayItemCountOrNegative (std::string varName);
int32_t getMemberArrayItemCountOrNegative (int32_t index);
void print();
StructDef(std::string _name, int32_t _typeID);
~StructDef();
};
} // namespace meanscript(core)
// C++ header END
