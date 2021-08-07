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
