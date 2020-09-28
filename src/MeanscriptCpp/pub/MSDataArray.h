//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscript {
class MSDataArray
{
public:
meanscriptcore::MeanMachine* mm;
int32_t tag, // instruction code
    tagAddress, // address of the code
 dataIndex; // address of the data
Array<int>* structCode; // where struct info is
Array<int>* dataCode; // where actual data is
MSDataArray(meanscriptcore::MeanMachine*, int32_t, int32_t);
int32_t getItemType ();
int32_t getArrayItemCount ();
MSData getAt (int32_t i);
~MSDataArray();
//void operator= (const CLASS_NAME & da) { mm = da.mm; /* etc. */ };
private: // hide
MSDataArray() = delete;
MSDataArray & operator & () = delete;
MSDataArray * operator * () = delete;
friend class MSData;
friend class MSCode;
};
} // namespace meanscript(core)
// C++ header END
