namespace meanscript {
class MSOutputPrintArray
: public MSOutputPrint
{
public:
 Array<uint8_t> buffer;
 int32_t maxSize;
 int32_t index;
MSOutputPrintArray();
void writeByte(uint8_t) override;
virtual MSOutputPrint & print (char x);
virtual MSOutputPrint & print (std::string x);
virtual MSOutputPrint & print (const char * x); // C++ only
void close() override;
const char * getString();
void print();
virtual ~MSOutputPrintArray () {};
};
} // namespace meanscript(core)
