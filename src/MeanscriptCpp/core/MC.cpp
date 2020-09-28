
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;




// error classes

const meanscript::MSError EC_CLASS( 0,  "-");

const meanscript::MSError 	EC_PARSE	(				EC_CLASS, 		"Parse error");		// when building the token tree
const meanscript::MSError 	EC_SYNTAX	(				EC_CLASS, 		"Syntax error");	// ...analyzing and generating code
const meanscript::MSError 	EC_SCRIPT	(				EC_CLASS, 		"Script error");	// ...executing script
const meanscript::MSError 	EC_CODE		(				EC_CLASS, 		"Code error");		// ...resolving bytecode
const meanscript::MSError 	EC_DATA		(				EC_CLASS, 		"Data error");		// ...accessing/creating data
const meanscript::MSError 	EC_TEST		(				EC_CLASS, 		"Test error");		// ...unit test
const meanscript::MSError 	EC_INTERNAL	(				EC_CLASS, 		"|||||||| INTERNAL ERROR ||||||||");

const meanscript::MSError 	E_UNEXPECTED_CHAR(			EC_PARSE, 		"Unexpected character");


 int32_t makeInstruction (int32_t operation, int32_t size, int32_t auxData) 
{
	ASSERT((operation | OPERATION_MASK) == OPERATION_MASK, "invalid operation");
	ASSERT(size >= 0 && size < 256, "wrong size");
	ASSERT((auxData | VALUE_TYPE_MASK) == VALUE_TYPE_MASK, "wrong aux. data");
	int32_t sizeBits = size << SIZE_SHIFT;
	int32_t instruction = operation | sizeBits | auxData;
	return instruction;
}
 std::string getOpName(int32_t instruction) 
{
	int32_t index = (int32_t)(instruction & OPERATION_MASK) >> OP_SHIFT;
	index &= 0x000000ff;
	CHECK(index < NUM_OP, EC_CODE, "unknown operation code");
	return opName[index];
}

 int32_t instrSize(int32_t instruction)
{
	return (int32_t)(instruction & SIZE_MASK) >> SIZE_SHIFT;
}
 int32_t instrValueTypeIndex(int32_t instruction)
{
	return (int32_t)(instruction & VALUE_TYPE_MASK) >> VALUE_TYPE_SHIFT;
}

 void printData(Array<int> & data, int32_t top, int32_t index, bool code) 
{
	int32_t tagIndex = 0;
	for (int32_t i=0; i<top; i++)
	{
		if (code)
		{
			if (i == index) PRINTN(">>> " CAT i);
			else PRINTN("    " CAT i);
			
			if (i == tagIndex)
			{
				PRINT(":    0x" CATHEX (data[i]) CAT "      " CAT getOpName(data[i]));
				tagIndex += instrSize(data[i]) + 1;
			}
			else PRINT(":    " CAT data[i]);
		}
		else
		{
			PRINT("    " CAT i CAT ":    " CAT data[i]);
		}
	}
}
 int32_t stringToIntsWithSize(std::string text, Array<int> & code, int32_t top)
{
	int32_t numChars = text.size();
	int32_t size32 = (numChars/4) + 1;
	Array<int> intArray;
	stringToIntBits(intArray, text);;
	
	code[top++]=numChars;
	for (int32_t i=0; i<size32; i++)
	{
		VERBOSE("        0x" CATHEX intArray[i]);
		code[top++] = intArray[i];
	}
	return top;
}

 bool intStringsWithSizeEquals(Array<int> & a, int32_t aOffset, Array<int> & b, int32_t bOffset)
{
	// first check that sizes match
	if (a[aOffset] == b[bOffset])
	{
		// check that ints match
		int32_t numChars = a[aOffset];
		int32_t size = (numChars / 4) + 1;
		for (int32_t i=1; i<=size; i++)
		{
			if (a[aOffset + i] != b[bOffset + i]) return false;
		}
		return true;
	}
	return false;
}

 int32_t addTextInstruction (std::string text, int32_t instructionCode, Array<int> & code, int32_t top) 
{
	// cast character array to int32_t array to copy it

	int32_t numChars = text.size();
	int32_t size32 = (numChars/4) + 1;
	
	VERBOSE("Add text: " CAT size32 CAT " x 4 bytes, " CAT numChars CAT " characters");
	
	int32_t instruction = makeInstruction(instructionCode, size32 + 1, MS_TYPE_TEXT);
	code[top++] = instruction;

	return stringToIntsWithSize(text, code, top);
}
}
