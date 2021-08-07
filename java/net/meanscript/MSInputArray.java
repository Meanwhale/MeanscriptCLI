package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;
public class MSInputArray extends MSInputStream {
byte buffer [];
int size;
int index;

public MSInputArray (MSOutputArray output)
{
	// copy array
	size = output.index;
	{ buffer = new  byte[ size];  };
	for (int i=0; i<size; i++) buffer[i] = output.buffer[i];
	index = 0;
}

public MSInputArray (String s)
{
	buffer =  s.getBytes(java.nio.charset.StandardCharsets.UTF_8);
	size = buffer.length;
	index = 0;
}

@Override
public int  getByteCount ()
{
	return size;
}

@Override
public byte  readByte () throws MException
{
	MSJava.assertion(!end(), EC_DATA, "readInt: buffer overflow");
	return buffer[index++];
}

@Override
public boolean  end ()
{
	return index >= size;
}

@Override
public void  close ()
{
}

}
