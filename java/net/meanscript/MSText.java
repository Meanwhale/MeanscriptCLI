package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;
public class MSText extends MC {
// Text bytes in an integer array. first int is the number of bytes. Bytes go from right to left (to be convinient on C++).
// Specification:

//		{0x00000000}{0x00000000}				= empty text, ie. just the terminating '\0' character
//		{0x00000002}{0x00006261}				= 2 chars ("ab") and '\0', from right to left
//		{0x00000005}{0x64636261}{0x00000065}	= 5 chars ("abcde") and '\0'

// Number of ints after the first int 'i' is '(int)i / 4 + 1' if 'i > 0', and 0 otherwise.
// Can't be modified. TODO: C++ reference counter for smart memory handling.


int [] data;

public MSText (String src) throws MException
{
	byte[] bytes = src.getBytes(java.nio.charset.StandardCharsets.UTF_8);
	_init(bytes, 0, bytes.length);
}

public MSText (byte[] src, int start, int length) throws MException
{
	_init(src, start, length);
}

public void _init(byte[] src, int start, int length)
{
	{data = new int[ (length / 4) + 2]; };
	data[0] = length;
	bytesToInts(src,start,data,1,length);
}

public MSText (MSText src) throws MException
{
	makeCopy(src.data,0);
}

public MSText (int [] src) throws MException
{
	makeCopy(src,0);
}

public MSText (int [] src, int start) throws MException
{
	makeCopy(src,start);
}


public boolean match(MSText t)
{
	return compare(t) == 0;
}

public boolean match(String s) throws MException
{
	return (s.equals(getString()));
}

public int [] getData()
{
	return data;
}

public int numBytes() 
{
	// count is without the ending character
	return data[0];
}
public int dataSize() 
{
	return data.length;
}
public byte byteAt(int index)  throws MException
{
	MSJava.assertion(index >= 0 && index <= data[0],  "index overflow");
	return (byte)((data[(index / 4) + 1]) >> ((index % 4) * 8) & 0x000000ff);
}
public int write(int [] trg, int start)  throws MException
{
	for (int i=0; i<data.length; i++)
	{
		trg[start + i] = data[i];
	}
	return 	start + data.length;
}
public void makeCopy(  int [] src, int start)
{
	int numChars = src[start];
	int size32 = (numChars/4) + 2;
	{data = new int[ size32]; };
	for (int i=0; i<size32; i++)
	{
		data[i] = src[i+start];
	}
}
public int compare (MSText text) 
{
	// returns -1 (less), 1 (greater), or 0 (equal)
	
	if (data.length != text.data.length)
	{
		return data.length > text.data.length ? 1 : -1;
	}
	
	for (int i=0; i<data.length; i++)
	{
		if (data[i] != text.data[i])
		{
			return data[i] > text.data[i] ? 1 : -1;
		}
	}
	return 0; // equals
}
public void check() throws MException
{
	int size32 = (data[0]/4) + 2;
	MSJava.assertion(data.length == size32,  "corrupted MSText object (size don't match)");
	MSJava.assertion(byteAt(data[0]) == 0,   "corrupted MSText object (no zero byte at end)");
}
public String getString() throws MException
{
	check();
	return new String(MSJava.intsToBytes(data,1,data[0]),java.nio.charset.StandardCharsets.UTF_8);
}
public String toString()
{
	try {
		check();
		return new String(MSJava.intsToBytes(data,1,data[0]),java.nio.charset.StandardCharsets.UTF_8);
	} catch (Exception e) {
		return "?";
	}
}

}
