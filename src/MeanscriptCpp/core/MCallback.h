//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscriptcore {
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
