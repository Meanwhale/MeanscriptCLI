//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscript {
class MSOutputArray
: public MSOutputStream
{
public:
 Array<uint8_t> buffer;
 int32_t maxSize;
 int32_t index;
MSOutputArray();
void writeByte(uint8_t) override;
void close() override;
void print();
virtual ~MSOutputArray () {};
};
} // namespace meanscript(core)
// C++ header END
