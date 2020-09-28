//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
// Auto-generated: do not edit.
namespace meanscript {
class MSWriter
{
public:
MSBuilder* builder;
meanscriptcore::StructDef* sd;
int32_t baseAddress;
MSWriter(MSBuilder* _builder, meanscriptcore::StructDef* _sd, int32_t _address);
void setInt (std::string name, int32_t value);
void setText (std::string name, std::string value);
private: // hide
MSWriter() = delete;
MSWriter & operator = (MSWriter &) = delete;
MSWriter & operator & () = delete;
MSWriter * operator * () = delete;
};
} // namespace meanscript(core)
// C++ header END
