namespace meanscriptcore {
using namespace meanscript;
class Common
{
public:
 bool initialized = false;
 int32_t callbackCounter;
 MCallback** callbacks;
 std::map<MSText, int> callbackIDs;
void printCallbacks();
void initialize(Semantics & sem);
int32_t createCallback (MSText name, void (*func)(MeanMachine &, MArgs &), int32_t returnType, StructDef* argStruct);
void createCallbacks (Semantics & sem);
Common();
~Common();
};
} // namespace meanscript(core)
