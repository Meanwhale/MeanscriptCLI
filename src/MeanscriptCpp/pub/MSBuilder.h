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
std::map<MSText, int> texts;
int32_t textIDCounter;
bool structLock;
void lockCheck();
Array<int> getValueArray();
void addType (std::string typeName, meanscriptcore::StructDef* sd);
void addInt(std::string n, int32_t value);
void addInt64(std::string n, int64_t value);
void addText(std::string varName, std::string value);
void addChars (std::string varName, int32_t numChars, std::string text);
int32_t createText(std::string value);
int32_t createStructDef (std::string name);
void addCharsMember (int32_t structTypeID, std::string varName, int32_t numChars);
int32_t addMember(int32_t sd, std::string varName, int32_t type);
MSWriter createStruct (std::string typeName, std::string varName);
MSWriter createStruct (int32_t typeID, std::string varName);
int32_t createGeneratedStruct (int32_t typeID, std::string varName);
void readStructCode (Array<int> code);
void addArray (int32_t typeID, std::string arrayName, int32_t arraySize);
MSWriter arrayItem (std::string arrayName, int32_t arrayIndex);
void generate();
MSCode* createMS ();
void write(MSOutputStream & output);
MSBuilder(std::string _packageName);
~MSBuilder();
};
} // namespace meanscript(core)
