//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscript {
class MSInputArray
: public MSInputStream
{
public:
Array<uint8_t> buffer;
int32_t size;
int32_t index;
MSInputArray(MSOutputArray*);
MSInputArray(std::string & s);
//CLASS_NAME(INT_ARRAY_REF arr);
int32_t getByteCount() override;
uint8_t readByte() override;
bool end() override;
void close() override;
virtual ~MSInputArray () {};
};
} // namespace meanscript(core)
// C++ header END
