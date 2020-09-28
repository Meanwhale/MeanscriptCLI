//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscriptcore {
class MArgs
{
public:
 ByteCode* byteCode;
 StructDef* structDef;
 int32_t baseIndex; // stack base where struct data start from
 bool valid; // become invalid when stack changes
MArgs (ByteCode* _byteCode, StructDef* _structDef, int32_t _base);
};
} // namespace meanscript(core)
// C++ header END
