namespace meanscriptcore {
using namespace meanscript;
class MCallback
{
public:
 void (*func)(MeanMachine &, MArgs &);
 int32_t returnType;
 StructDef* argStruct;
MCallback (void (*func)(MeanMachine &, MArgs &), int32_t _returnType, StructDef* _argStruct);
~MCallback();
};
} // namespace meanscript(core)
