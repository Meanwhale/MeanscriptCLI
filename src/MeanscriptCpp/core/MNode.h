//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscriptcore {
class MNode
{
public:
 int32_t type;
 int32_t numChildren;
 int32_t lineNumber;
 int32_t characterNumber;
 std::string data;
 MNode* next = 0;
 MNode* child = 0;
 MNode* parent = 0;
MNode(int32_t line, int32_t ch, MNode* _parent, int32_t _type, const std::string & _data);
void printTree (bool deep);
void printTree (MNode* node, int32_t depth, bool deep);
~MNode();
};
} // namespace meanscript(core)
// C++ header END
