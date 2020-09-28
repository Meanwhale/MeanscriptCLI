//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscript {
class MSOutputPrint
: public MSOutputStream
{
public:
MSOutputPrint();
virtual void writeByte(uint8_t) = 0;
MSOutputPrint & endLine();
// default printing functions 
virtual MSOutputPrint & print(int32_t) = 0;
virtual MSOutputPrint & print(std::string) = 0;
virtual MSOutputPrint & print(float) = 0;
MSOutputPrint & printHex(int32_t);
void close() override;
virtual ~MSOutputPrint () {};
};
} // namespace meanscript(core)
// C++ header END
