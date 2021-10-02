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
const meanscript::MSError 	EC_NATIVE	(				EC_CLASS, 		"Native error");	// ...executing native code
const meanscript::MSError 	EC_INTERNAL	(				EC_CLASS, 		"Error");			// General error when executing script, accessing data, etc.

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
 int32_t instrValueTypeID(int32_t instruction)
{
	return (int32_t)(instruction & VALUE_TYPE_MASK) >> VALUE_TYPE_SHIFT;
}

 int32_t int64highBits(int64_t x) {
	return (int32_t)(x>>32);
}
 int32_t int64lowBits(int64_t x) {
	return (int32_t)x;
}
 int64_t intsToInt64(int32_t high, int32_t low) {
	int64_t x = ((int64_t)high)<<32;
	x |= ((int64_t)low) & 0x00000000ffffffffl;
	return x;
}

 void printBytecode(Array<int> & data, int32_t top, int32_t index, bool code) 
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
				PRINTN(":    0x").printHex(data[i]).print("      ").print(getOpName(data[i])).endLine();
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
 int32_t stringToIntsWithSize(const std::string & text, Array<int> & code, int32_t top, int32_t maxSize) 
{
	MSText t (text);
	int32_t size32 = t.dataSize();
	CHECK(size32 <= maxSize, EC_CODE, "text is too long, max 32-bit size: " CAT maxSize CAT ", text: " CAT text);
	return t.write(code, top);
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

 void intsToBytes(Array<int> & ints, int32_t intsOffset, Array<uint8_t> & bytes, int32_t bytesOffset, int32_t bytesLength)
{
	// order: 0x04030201
	
	int32_t shift = 0;
	for (int32_t i = 0; i < bytesLength;)
	{
		bytes[i + bytesOffset] = (byte)((ints[intsOffset + (i/4)] >> shift) & 0x000000FF);

		i++;
		if (i % 4 == 0) shift = 0;
		else shift += 8;
	}
}

 void bytesToInts(const uint8_t bytes[], int32_t bytesOffset, Array<int> & ints, int32_t intsOffset, int32_t bytesLength) 
{
	// order: 0x04030201

	// bytes:	b[3] b[2] b[1] b[0] b[7] b[6] b[5] b[4]...
	// ints:	_________i[0]______|_________i[1]______...

	int32_t shift = 0;
	ints[intsOffset] = 0;
	for (int32_t i = 0; i < bytesLength;)
	{
		ints[(i / 4) + intsOffset] += (bytes[i] & 0x000000FF) << shift;
		
		i++;
		if (i % 4 == 0)
		{
			shift = 0;
			if (i < bytesLength)
			{
				ints[(i / 4) + intsOffset] = 0;
			}
		}
		else shift += 8;
	}
}

 int32_t addTextInstruction (const MSText & text, int32_t instructionCode, Array<int> & code, int32_t top) 
{
	//VERBOSE("Add text: " CAT size32 CAT " x 4 bytes, " CAT numChars CAT " characters");
	int32_t instruction = makeInstruction(instructionCode, text.dataSize(), MS_TYPE_TEXT);
	code[top++] = instruction;
	return text.write(code, top);
}

 int64_t parseHex(std::string text, int32_t maxChars) 
{
	int32_t numChars = text.size();
	CHECK(numChars > 0, EC_PARSE, "empty hexadecimal literal");
	CHECK(numChars <= maxChars, EC_PARSE, "hexadecimal literal is too long: " CAT text);
	uint8_t * hexes = (uint8_t *)text.c_str();
	int64_t x = 0;
	for (int32_t i=0; i<numChars; i++)
	{
		x = (x<<4) | hexCharToByte(hexes[i]);
	}
	return x;
}

 uint8_t hexCharToByte(uint8_t c) 
{
	int32_t code = (((int) c) & 0xff);
	if (code >= '0' && code <= '9') return (uint8_t)(code - '0');
	if (code >= 'a' && code <= 'f') return (uint8_t)(0xa + code - 'a');
	if (code >= 'A' && code <= 'F') return (uint8_t)(0xa + code - 'A');
	ERROR_PRINT("invalid literal: ").printCharSymbol((((int) c) & 0xff)).endLine();
	CHECK(false, EC_PARSE, "wrong hex character");
	return 0;
}

}
