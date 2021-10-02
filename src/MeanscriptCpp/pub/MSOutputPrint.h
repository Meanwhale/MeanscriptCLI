namespace meanscript {
class MSOutputPrint
: public MSOutputStream
{
public:
MSOutputPrint();
MSOutputPrint & endLine();
//void writeByte(BYTE) override; // <- MSOutputStream
// default printing functions 
virtual MSOutputPrint & print(char) = 0; // e.g. print('a');
virtual MSOutputPrint & print(std::string) = 0;
virtual MSOutputPrint & print(const char *) = 0; // C++ only
MSOutputPrint & print(int32_t);
MSOutputPrint & print(int64_t);
MSOutputPrint & print(float);
MSOutputPrint & print(double);
MSOutputPrint & print(bool);
MSOutputPrint & print(MSText & text);
MSOutputPrint & print(MSText* text);
MSOutputPrint & printHex(int32_t);
MSOutputPrint & printCharSymbol (int32_t i);
MSOutputPrint & printIntsToChars (Array<int> & ints, int32_t start, int32_t numChars, bool quote);
void close() override;
virtual ~MSOutputPrint () {};
};
} // namespace meanscript(core)
