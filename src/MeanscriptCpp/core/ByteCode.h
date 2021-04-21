// Auto-generated: do not edit.
namespace meanscriptcore {
using namespace meanscript;
class ByteCode
{
public:
 int32_t codeTop;
 Array<int32_t> code;
 Common* common;
void addInstruction(int32_t operation, int32_t size, int32_t valueType);
void addInstructionWithData(int32_t operation, int32_t size, int32_t valueType, int32_t data);
void addWord(int32_t data);
void addTextInstr (std::string text);
void writeCode (meanscript::MSOutputStream & output);
void writeStructInit (meanscript::MSOutputStream & output);
ByteCode (Common* _common);
ByteCode (Common* _common, meanscript::MSInputStream & input);
ByteCode (ByteCode &);
~ByteCode();
};
} // namespace meanscript(core)
// C++ header END
