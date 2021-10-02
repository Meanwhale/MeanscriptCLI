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
bool getBool ();
bool getBool (std::string name);
int64_t getInt64 ();
int64_t getInt64 (std::string name);
int64_t getInt64At (int address);
double getFloat64 ();
double getFloat64 (std::string name);
double getFloat64At (int address);
float getFloat (std::string name);
std::string getText (std::string name);
std::string getText (int32_t textID);
std::string getChars (std::string name);
MSData getData (std::string name);
MSDataArray getArray (std::string name);
void printCode();
void printDetails();
void printData();
void dataOutputPrint(MSOutputPrint & output);
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
