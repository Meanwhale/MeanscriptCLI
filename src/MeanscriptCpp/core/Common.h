//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscriptcore {
class Common
{
public:
 bool initialized = false;
 int32_t callbackCounter;
 MCallback** callbacks;
 std::map<std::string, int> callbackIDs;
void printCallbacks();
void includePrimitives(Semantics & sem);
int32_t createCallback (std::string name, void (*func)(MeanMachine &, MArgs &), int32_t returnType, StructDef * argStruct);
Common();
~Common();
};
} // namespace meanscript(core)
// C++ header END
