
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;


ByteCode::ByteCode (Common* _common)
{
	common = _common;
	{ code.reset( globalConfig.codeSize); code.fill(0); };
	codeTop = 0;
}

ByteCode::ByteCode (Common* _common, MSInputStream & input) 
{
	common = _common;
	int32_t byteCount = input.getByteCount();
	ASSERT(byteCount % 4 == 0, "bytecode file size not divisible by 4");
	int32_t size = byteCount / 4;
	{ code.reset( size); code.fill(0); };
	input.readArray(code, size);
	codeTop = size;
}

ByteCode::ByteCode (ByteCode & bc) 
{
	codeTop = bc.codeTop;
	common = bc.common;
	
	{ code.reset( codeTop); code.fill(0); };	
	
	// copy array
	
	for (int32_t i=0; i<codeTop; i++)
	{
		code[i] = bc.code[i];
	}
}

ByteCode::~ByteCode() { };


void ByteCode::addInstructionWithData (int32_t operation, int32_t size, int32_t valueType, int32_t data) 
{
	addInstruction(operation,size,valueType);
	addWord(data);
}

void ByteCode::addInstruction (int32_t operation, int32_t size, int32_t valueType) 
{
	int32_t instruction = makeInstruction(operation, size, valueType);
	VERBOSE("Add instruction: [" CAT getOpName(instruction) CAT "]");
	code[codeTop++] = instruction;
}

void ByteCode::addWord(int32_t data)
{
	//DEBUG(PRINT("data: " CAT data));
	code[codeTop++] = data;
}


void ByteCode:: addTextInstr (std::string text) 
{
	codeTop = addTextInstruction(text, OP_ADD_TEXT, code, codeTop);
}

void ByteCode:: writeCode (MSOutputStream & output) 
{
	for (int32_t i=0; i < codeTop; i++)
	{
		output.writeInt(code[i]);
	}
}

void ByteCode:: writeStructInit (MSOutputStream & output) 
{
	int32_t i=0;

	// change init tag
	
	int32_t op = (int32_t)((code[i]) & OPERATION_MASK);
	ASSERT(op == OP_START_INIT, "writeStructInit: bytecode error");
	output.writeInt(makeInstruction(OP_START_INIT, 1, BYTECODE_READ_ONLY));
	i++;
	output.writeInt(code[i]);
	i++;
	
	int32_t tagIndex = i;
	
	// copy necessary tags
	
	for (; i < codeTop; i++)
	{
		if (i == tagIndex)
		{
			tagIndex += instrSize(code[i]) + 1;
			op = (int32_t)((code[i]) & OPERATION_MASK);
			if (op == OP_FUNCTION)
			{
				// skip function inits
				i += instrSize(code[i]);
				continue;
			}
			else if (op == OP_END_INIT)
			{
				return;
			}
		}
		output.writeInt(code[i]);
	}
	ASSERT(false,"bytecode init end tag not found");
}

} // namespace meanscript(core)
// C++ END
