// Auto-generated: do not edit.
namespace meanscriptcore {
using namespace meanscript;
class MCallback
{
public:
 std::string name;
 void (*func)(MeanMachine &, MArgs &);
 int32_t returnType;
 StructDef* argStruct;
MCallback (std::string _name, void (*func)(MeanMachine &, MArgs &), int32_t _returnType, StructDef* _argStruct);
~MCallback();
};
} // namespace meanscript(core)
// C++ header END
