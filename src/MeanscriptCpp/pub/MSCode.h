// Auto-generated: do not edit.
namespace meanscript {
class MSCode
{
public:
meanscriptcore::Common* common;
meanscriptcore::MeanMachine* mm;
bool initialized;
MSCode();
MSCode(MSInputStream & input, int32_t streamType);
meanscriptcore::MeanMachine* getMM();
void reset();
void compileAndRun (std::string s);
//void compileAndRun(BYTES script);
meanscriptcore::ByteCode* compile(MSInputStream & input);
void run();
void step();
bool isInitialized();
void initBytecode(MSInputStream & input);
void initBytecode (meanscriptcore::ByteCode* bc);
bool hasData(std::string);
bool hasArray(std::string);
int32_t getInt (std::string name);
float getFloat (std::string name);
std::string getText (std::string name);
std::string getText (int32_t textID);
std::string getChars (std::string name);
MSData getData (std::string name);
MSDataArray getArray (std::string name);
void printCode();
void printDetails();
void printData();
void writeReadOnlyData(MSOutputStream & output);
void writeCode(MSOutputStream & output);
void checkInit();
~MSCode();
private: // hide
//CLASS_NAME(const CLASS_NAME&) = default;
MSCode & operator = (const MSCode &) = delete;
MSCode & operator & () = delete;
MSCode * operator * () = delete;
};
} // namespace meanscript(core)
// C++ header END
