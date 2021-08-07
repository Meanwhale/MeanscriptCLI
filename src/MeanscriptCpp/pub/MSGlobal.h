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
const int32_t maxNameLength = 128; // TODO: not a configurable, but according to language spex
const int32_t codeSize = 65536; // 2^16
const int32_t stackSize = 65536;
const int32_t builderValuesSize = 65536;
const int32_t outputArraySize = 65536;
const int32_t maxCallbacks = 256;
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
