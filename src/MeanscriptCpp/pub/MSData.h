// Auto-generated: do not edit.
namespace meanscript {
class MSData
{
public:
meanscriptcore::MeanMachine* mm;
int32_t tag, // instruction code
    tagAddress, // address of the code
 dataIndex; // address of the data
Array<int>* structCode; // where struct info is
Array<int>* dataCode; // where actual data is
// TODO: make a map if wanted
// variables to access from source code by name.
// value is the address of the variable tag.
// MAP_STRING_TO_INT(globalNames);
MSData(meanscriptcore::MeanMachine*, int32_t, int32_t, bool);
int32_t getType();
bool isStruct ();
bool hasData (std::string);
bool hasArray (std::string);
std::string getText();
std::string getText(std::string);
std::string getText(int32_t);
float getFloat ();
float getFloat (std::string name);
int32_t getInt ();
int32_t getInt(std::string);
bool isArrayItem();
MSDataArray getArray (std::string name);
MSData getMember (std::string name);
int32_t getMemberAddress (std::string name, int32_t type);
int32_t getMemberAddress (std::string name);
int32_t getMemberTagAddress (std::string name, bool isArray);
void printData(int32_t depth, std::string name);
~MSData();
private: // hide
MSData & operator = (const MSData &) = default;
MSData() = delete;
MSData & operator & () = delete;
MSData * operator * () = delete;
friend class MSDataArray;
friend class MSCode;
};
} // namespace meanscript(core)
// C++ header END
