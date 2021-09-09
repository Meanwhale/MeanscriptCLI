package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;
public abstract class MSOutputPrint extends MSOutputStream {

public abstract void  writeByte (byte x) throws MException; // see up about output
public abstract MSOutputPrint  print (char x) throws MException;
public abstract MSOutputPrint  print (String x) throws MException;

public MSOutputPrint ()
{
}

@Override
public void close ()
{
}

public MSOutputPrint print(int x) throws MException
{
	print((long)x);
	return this;
}
public MSOutputPrint print(long x) throws MException
{ 
	// TODO: make iterative instead of recursive
	if (x < 0) {
		print('-');
		x = -x;
	}
	if (x/10 > 0) print(x/10);
	print((char)('0' + (x%10)));
	return this;
}

// Floating-point number printing uses native string for now.
// For a 'proper' solution:
//		- https://www.cs.tufts.edu/~nr/cs257/archive/florian-loitsch/printf.pdf
//		- https://github.com/romange/Grisu

public MSOutputPrint print(float x) throws MException
{
	String s = x;
	print(s);
	return this;
}
public MSOutputPrint print(double x) throws MException
{
	String s = x;
	print(s);
	return this;
}
public MSOutputPrint print(boolean x) throws MException
{
	print(x ? "true" : "false");
	return this;
}

public static final char [] hexs = new char[]
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

public static final String [] ascii = new String[]
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


public MSOutputPrint  printHex (int h) throws MException
{
	for (int i = 28; i >= 0; i -=4 )
	{
		int index = (h>>i);
		index &= 0x0000000f;
		print(hexs[index]);
	}
	return this;
}


public MSOutputPrint print (MSText text) throws MException
{
	return printIntsToChars(text.getData(), 1, text.numBytes(), false);
}

public MSOutputPrint  printCharSymbol (int i) throws MException
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
	return this;
}

public MSOutputPrint printIntsToChars (int [] ints, int start, int numChars, boolean quote) throws MException
{

	int shift = 0;
	for (int i = 0; i < numChars;)
	{
		int b = ((ints[start + (i/4)] >> shift) & 0x000000FF);

		if (quote)
		{
			if (b < 32 || b >= 127)
			{
				print("\\x");
				int index = (b >> 4) & 0x0000000f;
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
	return this;
}

public MSOutputPrint  endLine () throws MException
{
	print("\n");
	return this;
}
}
