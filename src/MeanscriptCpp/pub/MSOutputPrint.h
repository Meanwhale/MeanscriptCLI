namespace meanscript {
class MSOutputPrint
: public MSOutputStream
{
public:
MSOutputPrint();
virtual void writeByte(uint8_t) = 0;
MSOutputPrint & endLine();
// default printing functions 
virtual MSOutputPrint & print(uint8_t) = 0;
virtual MSOutputPrint & print(int32_t) = 0;
virtual MSOutputPrint & print(int64_t) = 0;
virtual MSOutputPrint & print(std::string) = 0;
virtual MSOutputPrint & print(float) = 0;
virtual MSOutputPrint & print(double) = 0;
virtual MSOutputPrint & print(const char *) = 0; // C++ only
MSOutputPrint & print(MSText & text);
MSOutputPrint & print(MSText* text);
MSOutputPrint & printHex(int32_t);
MSOutputPrint & printCharSymbol (uint8_t x);
MSOutputPrint & printIntsToChars (Array<int> & ints, int32_t start, int32_t numChars);
void close() override;
virtual ~MSOutputPrint () {};
};
} // namespace meanscript(core)
