// Auto-generated: do not edit.
namespace meanscript {
class MSBuilder
{
public:
std::string packageName;
 Array<int> values;
 meanscriptcore::Semantics* semantics;
meanscriptcore::StructDef & variables;
meanscriptcore::ByteCode* byteCode;
meanscriptcore::Common* common;
std::map<std::string, int> texts;
int32_t textIDCounter;
bool structLock;
void lockCheck();
void addType (std::string typeName, meanscriptcore::StructDef* sd);
void addInt(std::string n, int32_t value);
void addText(std::string varName, std::string value);
int32_t createText(std::string value);
int32_t createStructDef (std::string name);
void addMember(int32_t sd, std::string varName, int32_t type);
MSWriter createStruct (std::string typeName, std::string varName);
MSWriter createStruct (int32_t typeID, std::string varName);
void addArray (int32_t typeID, std::string arrayName, int32_t arraySize);
MSWriter arrayItem (std::string arrayName, int32_t arrayIndex);
void generate();
MSCode* createMS ();
void write(MSOutputStream & output);
MSBuilder(std::string _packageName);
~MSBuilder();
};
} // namespace meanscript(core)
// C++ header END
