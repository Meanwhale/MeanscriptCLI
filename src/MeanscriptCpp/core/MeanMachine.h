namespace meanscriptcore {
using namespace meanscript;
class MeanMachine
{
public:
 int32_t stackTop;
 int32_t ipStackTop;
 int32_t baseStackTop;
 int32_t stackBase;
 int32_t instructionPointer;
 int32_t globalsSize;
 int32_t numTexts;
 int32_t registerType;
 int32_t byteCodeType;
 bool initialized;
 bool done;
 bool jumped;
 Array<int> stack;
 Array<int> ipStack;
 Array<int> baseStack;
 Array<int> functions;
 Array<int> registerData;
 Array<int> texts;
 Array<int> types; // typeID -> STRUCT_DEF tag address
 ByteCode* byteCode;
 MSData* globals;
 int32_t globalsTagAddress;
void callFunction(int32_t id);
void stepToFunction(int32_t id);
void initFunctionCall(int32_t id);
bool running();
bool isInitialized();
bool isDone();
void initVMArrays();
Array<int>* getStructCode();
Array<int>* getDataCode();
void step();
void initStep();
void pushData (Array<int> & source, int32_t address, int32_t size);
void popStackToTarget (ByteCode &, Array<int> & target, int32_t size, int32_t offset);
void push(int32_t data);
void callbackReturn(int32_t type, int32_t value);
void saveReg(int32_t type, int32_t value);
void gosub(int32_t address);
void pushIP(int32_t ip);
int32_t popIP();
int32_t popEndIP();
void writeCode (MSOutputStream & output);
void writeReadOnlyData (MSOutputStream & output);
void printGlobals();
void printDetails();
void printCode();
void dataPrint();
~MeanMachine();
MeanMachine(ByteCode* bc);
private:
void init();
MeanMachine() = delete;
};
} // namespace meanscript(core)
