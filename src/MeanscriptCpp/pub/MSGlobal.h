//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscript {
class MSGlobal
{
public:
// configuration parameters
const int32_t maxStructDefSize = 4096;
const int32_t maxStructMembers = 1024;
const int32_t ipStackSize = 1024;
const int32_t baseStackSize = 1024;
const int32_t maxFunctions = 256;
const int32_t registerSize = 256;
const int32_t maxArraySize = 4096;
const int32_t maxNameLength = 128;
const int32_t codeSize = 16384; // 2^14
const int32_t builderValuesSize = 16384;
const int32_t outputArraySize = 16384;
const int32_t maxCallbacks = 256;
const int32_t stackSize = 4096;
bool verbose = false;
// stream types
const int32_t STREAM_TYPE_FIRST = 100001;
const int32_t STREAM_BYTECODE = 100001;
const int32_t STREAM_SCRIPT = 100002;
const int32_t STREAM_BYTECODE_READ_ONLY = 100003;
const int32_t STREAM_TYPE_LAST = 100003;
void setVerbose(bool);
bool verboseOn() const { return verbose; };
};
} // namespace meanscript(core)
// C++ header END
