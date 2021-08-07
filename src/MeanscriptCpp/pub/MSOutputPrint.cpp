#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;


MSOutputPrint::MSOutputPrint ()
{
}


void MSOutputPrint::close ()
{
}


constexpr char const * hexs [] =
{
	"0","1","2","3",
	"4","5","6","7",
	"8","9","a","b",
	"c","d","e","f"
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
"[US]",        // unit separator
" ",           // space
"!",
"\"",
"#",
"$",
"%",
"&",
"'",
"(",
")",
"*",
"+",
",",
"-",
".",
"/",
"0",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
":",
";",
"<",
"=",
">",
"?",
"@",
"A",
"B",
"C",
"D",
"E",
"F",
"G",
"H",
"I",
"J",
"K",
"L",
"M",
"N",
"O",
"P",
"Q",
"R",
"S",
"T",
"U",
"V",
"W",
"X",
"Y",
"Z",
"[",
"\\",
"]",
"^",
"_",
"`",
"a",
"b",
"c",
"d",
"e",
"f",
"g",
"h",
"i",
"j",
"k",
"l",
"m",
"n",
"o",
"p",
"q",
"r",
"s",
"t",
"u",
"v",
"w",
"x",
"y",
"z",
"{",
"|",
"}",
"~",
"[DEL]",  // #127, delete
};
//"[#128]", // #128 - #159 are not defined for ISO/IEC 8859-1
//"[#129]",
//"[#130]",
//"[#131]",
//"[#132]",
//"[#133]",
//"[#134]",
//"[#135]",
//"[#136]",
//"[#137]",
//"[#138]",
//"[#139]",
//"[#140]",
//"[#141]",
//"[#142]",
//"[#143]",
//"[#144]",
//"[#145]",
//"[#146]",
//"[#147]",
//"[#148]",
//"[#149]",
//"[#150]",
//"[#151]",
//"[#152]",
//"[#153]",
//"[#154]",
//"[#155]",
//"[#156]",
//"[#157]",
//"[#158]",
//"[#159]",
//"[NBSP]", //"[Non-breaking space]",                                // #160
//"[_!]", //"[Inverted exclamation mark]",                         // ¡
//"[cent]", //"[Cent sign]",                                         // ¢
//"[pound]", //"[Pound sign]",                                        // £
//"[currency]", //"[Currency sign]",                                     // ¤
//"[yen]", //"[Yen sign]",                                          // ¥
//"[|]", //ALT: [||][pipes] "[Pipe, Broken vertical bar]",                         // ¦
//"[section]", //"[Section sign]",                                      // §
//"[umlauts]", // ALT: [uml][..][^..][^:] "[Spacing diaeresis - umlaut]",                        // ¨
//"[copyright]", // ALT: [C][(C)] "[Copyright sign]",                                    // ©
//"[^a]", // ALT: [-a] "[Feminine ordinal indicator]",                        // ª
//"[<<]", //"[Left double angle quotes]",                          // «
//"[NOT]", //"[Not sign]",                                          // ¬
//"[-]", //"[Soft hyphen]",                                       // ­
//"[reg]", // ALT: [(R)] "[Registered trade mark sign]",                        // ®
//"[^-]", //"[Spacing macron - overline]",                         // ¯
//"[degree]", //"[Degree sign]",                                       // °
//"[+-]", //"[Plus-or-minus sign]",                                // ±
//"[^2]", //"[Superscript two - squared]",                         // ²
//"[^3]", //"[Superscript three - cubed]",                         // ³
//"[']", // ALT: [acute] NOW IT'S WRONG FOR ALL TO BE READABLE "[Acute accent - spacing acute]",                      // ´
//"[micro]", //ALT: [m] "[Micro sign]",                                        // µ
//"[paragraph]", // ALT: [q] [P] "[Pilcrow sign - paragraph sign]",                     // ¶
//"[dot]", // ALT: [middle dot] "[Middle dot - Georgian comma]",                       // ·
//"[cedilla]", // ALT: [,] "[Spacing cedilla]",                                   // ¸
//"[^1]", //"[Superscript one]",                                   // ¹
//"[^o]", //"[Masculine ordinal indicator]",                       // º
//"[>>]", //"[Right double angle quotes]",                         // »
//"[1/4]", //"[Fraction one quarter]",                              // ¼
//"[1/2]", //"[Fraction one half]",                                 // ½
//"[3/4]", //"[Fraction three quarters]",                           // ¾
//"[_?]", //"[Inverted question mark]",                            // ¿
//"[A`]", //"[Latin capital letter A with grave]",                 // À
//"[A']", //"[Latin capital letter A with acute]",                 // Á
//"[A^]", //"[Latin capital letter A with circumflex]",            // Â
//"[A~]", //"[Latin capital letter A with tilde]",                 // Ã
//"[A:]", //"[Latin capital letter A with diaeresis]",             // Ä
//"[Ao]", //"[Latin capital letter A with ring above]",            // Å
//"[AE]", //"[Latin capital letter AE]",                           // Æ
//"[C,]", //"[Latin capital letter C with cedilla]",               // Ç
//"[E`]", //"[Latin capital letter E with grave]",                 // È
//"[E']", //"[Latin capital letter E with acute]",                 // É
//"[E^]", //"[Latin capital letter E with circumflex]",            // Ê
//"[E:]", //"[Latin capital letter E with diaeresis]",             // Ë
//"[I`]", //"[Latin capital letter I with grave]",                 // Ì
//"[I']", //"[Latin capital letter I with acute]",                 // Í
//"[I^]", //"[Latin capital letter I with circumflex]",            // Î
//"[I:]", //"[Latin capital letter I with diaeresis]",             // Ï
//"[Eth]", //"[Latin capital letter ETH]",                          // Ð
//"[N~]", //"[Latin capital letter N with tilde]",                 // Ñ
//"[O`]", //"[Latin capital letter O with grave]",                 // Ò
//"[O']", //"[Latin capital letter O with acute]",                 // Ó
//"[O^]", //"[Latin capital letter O with circumflex]",            // Ô
//"[O~]", //"[Latin capital letter O with tilde]",                 // Õ
//"[O:]", //"[Latin capital letter O with diaeresis]",             // Ö
//"[x]", //"[Multiplication sign]",                               // ×
//"[O/]", //"[Latin capital letter O with slash]",                 // Ø
//"[U`]", //"[Latin capital letter U with grave]",                 // Ù
//"[U']", //"[Latin capital letter U with acute]",                 // Ú
//"[U^]", //"[Latin capital letter U with circumflex]",            // Û
//"[U:]", //"[Latin capital letter U with diaeresis]",             // Ü
//"[Y']", //"[Latin capital letter Y with acute]",                 // Ý
//"[Thorn]", //"[Latin capital letter THORN]",                        // Þ
//"[ss]", //"[Latin small letter sharp s - ess-zed]",              // ß
//"[a`]", //"[Latin small letter a with grave]",                   // à
//"[a']", //"[Latin small letter a with acute]",                   // á
//"[a^]", //"[Latin small letter a with circumflex]",              // â
//"[a~]", //"[Latin small letter a with tilde]",                   // ã
//"[a:]", //"[Latin small letter a with diaeresis]",               // ä
//"[ao]", //"[Latin small letter a with ring above]",              // å
//"[ae]", //"[Latin small letter ae]",                             // æ
//"[c,]", //"[Latin small letter c with cedilla]",                 // ç
//"[e`]", //"[Latin small letter e with grave]",                   // è
//"[e']", //"[Latin small letter e with acute]",                   // é
//"[e^]", //"[Latin small letter e with circumflex]",              // ê
//"[e:]", //"[Latin small letter e with diaeresis]",               // ë
//"[i`]", //"[Latin small letter i with grave]",                   // ì
//"[i']", //"[Latin small letter i with acute]",                   // í
//"[i^]", //"[Latin small letter i with circumflex]",              // î
//"[i:]", //"[Latin small letter i with diaeresis]",               // ï
//"[eth]", //"[Latin small letter eth]",                            // ð
//"[n~]", //"[Latin small letter n with tilde]",                   // ñ
//"[o`]", //"[Latin small letter o with grave]",                   // ò
//"[o']", //"[Latin small letter o with acute]",                   // ó
//"[o^]", //"[Latin small letter o with circumflex]",              // ô
//"[o~]", //"[Latin small letter o with tilde]",                   // õ
//"[o:]", //"[Latin small letter o with diaeresis]",               // ö
//"[DIV]", //"[Division sign]",                                     // ÷
//"[o/]", //"[Latin small letter o with slash]",                   // ø
//"[u`]", //"[Latin small letter u with grave]",                   // ù
//"[u']", //"[Latin small letter u with acute]",                   // ú
//"[u^]", //"[Latin small letter u with circumflex]",              // û
//"[u:]", //"[Latin small letter u with diaeresis]",               // ü
//"[y']", //"[Latin small letter y with acute]",                   // ý
//"[thorn]", //"[Latin small letter thorn]",                          // þ
//"[y:]", //"[Latin small letter y with diaeresis]"                // ÿ
//};

MSOutputPrint & MSOutputPrint:: printHex (int32_t h)
{
	for (int i = 28; i >= 0; i -=4 )
	{
		int index = (h>>i);
		index &= 0x0000000f;
		print(hexs[index]);
	}
	return (*this);
}

MSOutputPrint & MSOutputPrint::print (MSText* text)
{
	return MSOutputPrint::print(*text);
}
MSOutputPrint & MSOutputPrint::print (MSText & text)
{
	return printIntsToChars(text.getData(), 1, text.numBytes());
}

MSOutputPrint & MSOutputPrint:: printCharSymbol (uint8_t b)
{
	// print an ASCII character, symbol, or description for it.
	print(ascii[(((int) b) & 0xff)]);
	return (*this);
}

MSOutputPrint & MSOutputPrint::printIntsToChars (Array<int> & ints, int32_t start, int32_t numChars)
{

	int32_t shift = 0;
	for (int32_t i = 0; i < numChars;)
	{
		int32_t b = ((ints[start + (i/4)] >> shift) & 0x000000FF);

		if (b >= 0 && b < 128)
		{
			print(ascii[b]);
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
