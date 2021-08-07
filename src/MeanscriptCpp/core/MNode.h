namespace meanscriptcore {
using namespace meanscript;
class MNode
{
public:
 int32_t type;
 int32_t numChildren;
 int32_t lineNumber;
 int32_t characterNumber;
 MSText* data;
 MNode* next = 0;
 MNode* child = 0;
 MNode* parent = 0;
MNode(int32_t line, int32_t ch, MNode* _parent, int32_t _type, MSText* _data);
int32_t line();
void printTree (bool deep);
void printTree (MNode* node, int32_t depth, bool deep);
~MNode();
};
} // namespace meanscript(core)
