namespace meanscriptcore {
using namespace meanscript;
class Generator
{
public:
Context* currentContext;
TokenTree* tree;
Semantics & sem;
Common & common;
ByteCode* bcPtr;
ByteCode & bc;
static ByteCode* generate(TokenTree* tree, Semantics & sem, Common & _common);
Generator(TokenTree* _tree, Semantics & sem, Common & _common);
ByteCode* generate();
void generateCodeBlock(NodeIterator);
void generateFunctionCode(NodeIterator it);
void generateExpression(NodeIterator);
void generateAssignment(NodeIterator);
void generateFunctionCall(NodeIterator it, Context* context);
MCallback* generateCallbackCall(NodeIterator it);
void singleArgumentPush(int32_t type, NodeIterator & it, int32_t arrayItemCount);
void squareBracketArgumentPush(NodeIterator it, StructDef* sd, int32_t numArgs);
void argumentStructPush(NodeIterator it, StructDef* targetType, int32_t numArgs, bool commaSeparated);
void callArgumentPush(NodeIterator it, StructDef* targetType, int32_t numArgs);
VarGen resolveMember (NodeIterator & it);
int32_t arrayPush (NodeIterator it, int32_t targetTag, int32_t arraySize);
bool isFunctionOrCallback (MSText* name);
bool inGlobal();
~Generator();
};
} // namespace meanscript(core)
