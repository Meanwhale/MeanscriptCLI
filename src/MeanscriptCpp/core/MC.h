namespace meanscriptcore
{
// node types
constexpr int32_t NT_ROOT = 0;
constexpr int32_t NT_EXPR = 1;
constexpr int32_t NT_PARENTHESIS = 2;
constexpr int32_t NT_ASSIGNMENT = 3;
constexpr int32_t NT_SQUARE_BRACKETS = 4;
constexpr int32_t NT_CODE_BLOCK = 5;
constexpr int32_t NT_NAME_TOKEN = 6;
constexpr int32_t NT_NUMBER_TOKEN = 7;
constexpr int32_t NT_REFERENCE_TOKEN = 8; // e.g. "#foo", as in "increase #foo"
constexpr int32_t NT_REF_TYPE_TOKEN = 9; // e.g. "int#", as INT "func void increase [int# value] { value += 1 }
constexpr int32_t NT_DOT = 10;
constexpr int32_t NT_PLUS = 11;
constexpr int32_t NT_MINUS = 12;
constexpr int32_t NT_DIV = 13;
constexpr int32_t NT_MUL = 14;
constexpr int32_t NT_TEXT = 15;
constexpr int32_t NT_MEMBER = 16;
constexpr int32_t NT_COMMA = 17;
constexpr int32_t NT_HEX_TOKEN = 18;
// bytecode types
constexpr int BYTECODE_READ_ONLY = 0x101;
constexpr int BYTECODE_EXECUTABLE = 0x102;
// instructions
constexpr int32_t OP_SYSTEM = 0x00000000; // system calls (ERROR, assert, exception, etc. TBD)
constexpr int32_t OP_CALLBACK_CALL = 0x03000000;
constexpr int32_t OP_JUMP = 0x04000000;
constexpr int32_t OP_GO_BACK = 0x05000000; // return to previous block. named to be less confusing
constexpr int32_t OP_GO_END = 0x06000000; // go to end of the function (context's end address)
constexpr int32_t OP_CHARS_DEF = 0x07000000;
constexpr int32_t OP_STRUCT_DEF = 0x08000000;
constexpr int32_t OP_STRUCT_MEMBER = 0x09000000;
constexpr int32_t OP_SAVE_BASE = 0x0a000000; // save/load stack base index
constexpr int32_t OP_LOAD_BASE = 0x0b000000;
constexpr int32_t OP_NOOP = 0x0c000000;
constexpr int32_t OP_ADD_TEXT = 0x10000000; // add immutable text to text map and add index to register
constexpr int32_t OP_PUSH_IMMEDIATE = 0x11000000; // push immediate value to stack
constexpr int32_t OP_PUSH_REG_TO_STACK = 0x13000000; // push content of register to stack
constexpr int32_t OP_FUNCTION = 0x14000000; // introduce a function
constexpr int32_t OP_START_INIT = 0x15000000;
constexpr int32_t OP_END_INIT = 0x16000000;
constexpr int32_t OP_FUNCTION_CALL = 0x17000000;
constexpr int32_t OP_PUSH_LOCAL = 0x18000000;
constexpr int32_t OP_PUSH_GLOBAL = 0x19000000;
constexpr int32_t OP_POP_STACK_TO_LOCAL = 0x1a000000;
constexpr int32_t OP_POP_STACK_TO_GLOBAL = 0x1b000000;
constexpr int32_t OP_POP_STACK_TO_REG = 0x1c000000;
constexpr int32_t OP_MEMBER_NAME = 0x1d000000;
constexpr int32_t OP_INIT_GLOBALS = 0x1e000000;
constexpr int32_t OP_ARRAY_MEMBER = 0x1f000000;
constexpr int32_t OP_MULTIPLY_GLOBAL_ARRAY_INDEX= 0x20000000;
constexpr int32_t OP_POP_STACK_TO_LOCAL_REF = 0x21000000;
constexpr int32_t OP_POP_STACK_TO_GLOBAL_REF = 0x22000000;
constexpr int32_t OP_PUSH_LOCAL_REF = 0x23000000;
constexpr int32_t OP_PUSH_GLOBAL_REF = 0x24000000;
constexpr int32_t OP_PUSH_CHARS = 0x25000000;
constexpr int32_t OP_MAX = 0x30000000;
constexpr int32_t NUM_OP = 0x30;
constexpr char const * opName [] = {
 "system", "---OLD---", "---OLD---", "call",
 "jump", "go back", "go end", "chars definition",
 "struct definition", "struct member", "save base", "load base",
 "no operation", "---OLD---", "---OLD---", "---OLD---",
 "text", "push immediate", "---OLD---", "push from reg.",
 "function data", "start init", "end init", "function call",
 "push local", "push global", "pop to local", "pop to global",
 "pop to register", "member name", "init globals", "array member",
 "multiply array index", "pop to local ref.", "pop to global ref.", "push local ref.",
 "push global ref.", "push chars", "---ERROR---", "---ERROR---",
 "---ERROR---", "---ERROR---", "---ERROR---", "---ERROR---",
 "---ERROR---", "---ERROR---", "---ERROR---", "---ERROR---",
 };
constexpr int32_t KEYWORD_FUNC_ID = 0;
constexpr int32_t KEYWORD_STRUCT_ID = 1;
constexpr int32_t KEYWORD_RETURN_ID = 2;
constexpr int32_t KEYWORD_GLOBAL_ID = 3;
constexpr int32_t NUM_KEYWORDS = 4;
constexpr char const * keywords [] = {
 "func",
 "struct",
 "return",
 "global"
 };
  constexpr char const * HORIZONTAL_LINE = "------------------------------------------";
//INITIALIZED_CONST_STRING(HORIZONTAL_LINE,"__________________________________________\n");
constexpr uint32_t OPERATION_MASK = 0xff000000;
constexpr uint32_t SIZE_MASK = 0x00ff0000; // NOTE: erikoistapauksissa voisi käyttää 0x00FFFFFF
constexpr uint32_t VALUE_TYPE_MASK= 0x0000ffff; // max. 64K
constexpr uint32_t AUX_DATA_MASK = 0x0000ffff; // same size as VALUE_TYPE_MASK for commands to use other data than value type.
constexpr int32_t OP_SHIFT = 24;
constexpr int32_t SIZE_SHIFT = 16;
constexpr int32_t VALUE_TYPE_SHIFT = 0;
constexpr int32_t MS_TYPE_VOID = 0;
constexpr int32_t MS_TYPE_INT = 1;
constexpr int32_t MS_TYPE_INT64 = 2;
constexpr int32_t MS_TYPE_FLOAT = 3;
constexpr int32_t MS_TYPE_FLOAT64 = 4;
constexpr int32_t MS_TYPE_TEXT = 5;
constexpr int32_t MS_TYPE_BOOL = 6;
constexpr int32_t MS_TYPE_CODE_ADDRESS = 7;
constexpr int32_t MS_TYPE_ARGS = 8;
constexpr int32_t MS_TYPE_CHARS = 9;
constexpr int32_t MAX_MS_TYPES = 16;
constexpr int32_t MAX_TYPES = 256;
// error classes
extern const meanscript::MSError EC_CLASS;
extern const meanscript::MSError EC_PARSE; // when building the token tree
extern const meanscript::MSError EC_SYNTAX; // ...analyzing and generating code
extern const meanscript::MSError EC_SCRIPT; // ...executing script
extern const meanscript::MSError EC_CODE; // ...resolving bytecode
extern const meanscript::MSError EC_DATA; // ...accessing/creating data
extern const meanscript::MSError EC_TEST; // ...unit test
extern const meanscript::MSError EC_NATIVE; // ...executing native code
extern const meanscript::MSError EC_INTERNAL; // General error when executing script, accessing data, etc.
extern const meanscript::MSError E_UNEXPECTED_CHAR;
int32_t makeInstruction (int32_t operation, int32_t size, int32_t valueType);
std::string getOpName(int32_t instruction);
int32_t instrSize(int32_t instruction);
int32_t instrValueTypeIndex(int32_t instruction);
int32_t int64highBits(int64_t x);
int32_t int64lowBits(int64_t x);
int64_t intsToInt64(int32_t high, int32_t low);
void printData(Array<int> & data, int32_t top, int32_t index, bool code);
int32_t stringToIntsWithSize(const std::string & text, Array<int> & code, int32_t top, int32_t maxSize);
bool intStringsWithSizeEquals(Array<int> & a, int32_t aOffset, Array<int> & b, int32_t bOffset);
void intsToBytes(Array<int> & ints, int32_t intsOffset, Array<uint8_t> & bytes, int32_t bytesOffset, int32_t bytesLength);
void bytesToInts(const uint8_t bytes[], int32_t bytesOffset, Array<int> & ints, int32_t intsOffset, int32_t bytesLength);
int32_t addTextInstruction (const MSText & text, int32_t instructionCode, Array<int> & code, int32_t top);
int64_t parseHex(std::string text, int32_t maxChars);
uint8_t hexCharToByte(uint8_t c);
}
