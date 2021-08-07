namespace meanscriptcore {
using namespace meanscript;
class StructDef
{
public:
 int32_t typeID;
 Array<int> code;
 Array<int> memberOffset; // memberOffset[n] = code address of _n_th member
 Array<int> nameOffset; // nameOffset[n] = name address of _n_th member
 MSText* name;
 int32_t numMembers;
 int32_t argsSize;
 int32_t structSize;
 int32_t codeTop;
 std::map<MSText, int> memberNames;
bool isCharsDef();
int32_t numCharsForCharsDef();
int32_t addMember(Semantics* semantics, MSText* name, int32_t type);
int32_t addMember(MSText* name, int32_t type, int32_t size);
int32_t addArray (Semantics* semantics, MSText* name, int32_t arrayType, int32_t itemCount);
void addNameInstruction (MSText* text);
bool indexInRange(int32_t index);
int32_t getMemberTag(MSText* varName);
int32_t getMemberAddress(MSText* varName);
int32_t getMemberSize(MSText* varName);
int32_t getMemberTag(int32_t index);
int32_t getMemberAddress(int32_t index);
int32_t getMemberSize(int32_t index);
int32_t getMemberArrayItemCount (MSText* varName);
int32_t getMemberArrayItemCountOrNegative (MSText* varName);
int32_t getMemberArrayItemCountOrNegative (int32_t index);
std::string getMemberName (int32_t index);
void print();
StructDef(MSText* _name, int32_t _typeID);
StructDef (MSText* _name, int32_t _typeID, int32_t _size);
StructDef (MSText* _name, int32_t _typeID, int32_t data, int32_t size, int32_t op);
~StructDef();
};
} // namespace meanscript(core)
