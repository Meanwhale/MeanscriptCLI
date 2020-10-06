// Auto-generated: do not edit.
namespace meanscriptcore {
class Semantics
{
public:
 int32_t typeIDCounter;
 int32_t maxContexts;
 int32_t numContexts;
 std::map<std::string, int> types;
 StructDef** typeStructDefs;
 Context** contexts;
 Context* globalContext;
 Context* currentContext;
Semantics();
~Semantics();
Context* findContext(std::string name);
void analyze(TokenTree* root);
void addPrimitiveType(std::string name, StructDef* sd, int32_t id);
bool hasType(std::string name);
StructDef* getType (int32_t);
StructDef* getType (std::string);
StructDef* getType (int32_t, NodeIterator*);
StructDef* getType (std::string, NodeIterator*);
bool inGlobal();
bool assumeNotReserved(std::string name);
void print();
void analyzeNode(NodeIterator it);
void analyzeExpr(NodeIterator it);
void addStructDef (std::string name, NodeIterator it);
void addStructDef (std::string name, int, StructDef*);
void createStructDef (StructDef & sd, NodeIterator it);
void writeStructDefs (ByteCode* bc);
};
} // namespace meanscript(core)
// C++ header END
