#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;


MSOutputPrint::MSOutputPrint ()
{
}


void MSOutputPrint::close () 
{
}

MSOutputPrint & MSOutputPrint::print(int32_t x) 
{
	print((int64_t)x);
	return (*this);
}
MSOutputPrint & MSOutputPrint::print(int64_t x) 
{ 
	// TODO: make iterative instead of recursive
	if (x < 0) {
		print('-');
		x = -x;
	}
	if (x/10 > 0) print(x/10);
	print((char)('0' + (x%10)));
	return (*this);
}

// Floating-point number printing uses native string for now.
// For a 'proper' solution:
//		- https://www.cs.tufts.edu/~nr/cs257/archive/florian-loitsch/printf.pdf
//		- https://github.com/romange/Grisu

MSOutputPrint & MSOutputPrint::print(float x) 
{
	print(std::to_string(x));
	return (*this);
}
MSOutputPrint & MSOutputPrint::print(double x) 
{
	print(std::to_string(x));
	return (*this);
}
MSOutputPrint & MSOutputPrint::print(bool x) 
{
	print(x ? "true" : "false");
	return (*this);
}

constexpr char hexs [] =
{
	'0','1','2','3',
	'4','5','6','7',
	'8','9','a','b',
	'c','d','e','f'
};

// Character codes 0-255: ISO/IEC 8859-1
//     0-31    Code symbols
//     32-127  ASCII characters
//     128-159 Not defined for ISO/IEC 8859-1
//     160-255 Character descriptions (as their printed character can vary according to environment)
// source: https://en.wikipedia.org/wiki/ISO/IEC_8859-1

constexpr char const * ascii [] =
{
"[NUL]",       // null
"[SOH]",       // start of heading
"[STX]",       // start of text
"[ETX]",       // end of text
"[EOT]",       // end of transmission
"[ENQ]",       // enquiry
"[ACK]",       // acknowledge
"[BEL]",       // bell
"[BS]",        // backpace
"[HT]",        // horizontal tab
"[LF]",        // line feed, new line
"[VT]",        // vertical tab
"[FF]",        // form feed, new page
"[CR]",        // carriage return
"[SO]",        // shift out
"[SI]",        // shift in
"[DLE]",       // data link escape
"[DC1]",       // device control 1
"[DC2]",       // device control 2
"[DC3]",       // device control 3
"[DC4]",       // device control 4
"[NAK]",       // negative acknowledge
"[SYN]",       // synchonous idle
"[ETB]",       // end of transmission block
"[CAN]",       // cancel
"[EM]",        // end of medium
"[SUB]",       // substitute
"[ESC]",       // escape
"[FS]",        // file separator
"[GS]",        // group separator
"[RS]",        // record separator
"[US]"        // unit separator
};


MSOutputPrint & MSOutputPrint:: printHex (int32_t h) 
{
	for (int32_t i = 28; i >= 0; i -=4 )
	{
		int32_t index = (h>>i);
		index &= 0x0000000f;
		print(hexs[index]);
	}
	return (*this);
}

MSOutputPrint & MSOutputPrint::print (MSText* text)
{
	if (text == 0) return print("null");
	return print(*text);
}

MSOutputPrint & MSOutputPrint::print (MSText & text) 
{
	return printIntsToChars(text.getData(), 1, text.numBytes(), false);
}

MSOutputPrint & MSOutputPrint:: printCharSymbol (int32_t i) 
{
	// print an ASCII character, symbol, or description for it.
	if (i > 127) // 127 = [DEL]
	{
		print("[#");
		print(i);
		print("]");
	}
	else if (i < 32) print(ascii[i]);
	else if (i == 127) print("[DEL]");
	else print((char)i);
	return (*this);
}

MSOutputPrint & MSOutputPrint::printIntsToChars (Array<int> & ints, int32_t start, int32_t numChars, bool quote) 
{

	int32_t shift = 0;
	for (int32_t i = 0; i < numChars;)
	{
		int32_t b = ((ints[start + (i/4)] >> shift) & 0x000000FF);

		if (quote)
		{
			if (b < 32 || b >= 127)
			{
				print("\\x");
				int32_t index = (b >> 4) & 0x0000000f;
				print(hexs[index]);
				index = b & 0x0000000f;
				print(hexs[index]);
			}
			else print((char)b);
		}
		else if (b >= 32 && b < 128)
		{
			print((char)b); //print(ascii[b]);
		}
		else
		{
			print("[#");
			print(b);
			print("]");
		}

		i++;
		if (i % 4 == 0) shift = 0;
		else shift += 8;
	}
	return (*this);
}

MSOutputPrint & MSOutputPrint:: endLine () 
{
	print("\n");
	return (*this);
}
} // namespace meanscript(core)
