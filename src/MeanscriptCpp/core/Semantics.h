namespace meanscriptcore {
using namespace meanscript;
class Semantics
{
public:
 int32_t typeIDCounter;
 int32_t maxContexts;
 int32_t numContexts;
 std::map<MSText, int> types;
 StructDef** typeStructDefs;
 Context** contexts;
 Context* globalContext;
 Context* currentContext;
Semantics();
~Semantics();
Context* findContext(MSText* name);
void analyze(TokenTree* root);
void addPrimitiveType(MSText* name, StructDef* sd, int32_t id);
void addElementaryType (int32_t typeID, int32_t size);
StructDef* addCharsType (int32_t numChars);
bool hasType(MSText* name);
bool hasType(int32_t);
StructDef* getType (int32_t);
StructDef* getType (MSText*);
StructDef* getType (int32_t, NodeIterator*);
StructDef* getType (MSText*, NodeIterator*);
bool inGlobal();
bool isNameValidAndAvailable (const std::string & name);
bool isNameValidAndAvailable(MSText* name);
void print();
void analyzeNode(NodeIterator it);
void analyzeExpr(NodeIterator it);
void addStructDef (MSText* name, NodeIterator it);
void addStructDef (MSText* name, int, StructDef*);
void createStructDef (StructDef & sd, NodeIterator it);
void writeStructDefs (ByteCode* bc);
};
} // namespace meanscript(core)
