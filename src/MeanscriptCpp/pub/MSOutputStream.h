//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscript {
class MSOutputStream
{
public:
MSOutputStream();
virtual void writeByte(uint8_t) = 0;
virtual void close() = 0;
void writeInt(int32_t);
virtual ~MSOutputStream () {};
};
} // namespace meanscript(core)
// C++ header END
