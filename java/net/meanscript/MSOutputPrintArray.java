package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;
public class MSOutputPrintArray extends MSOutputPrint {
 byte  buffer [];
 int maxSize;
 int index;


public MSOutputPrintArray ()
{
	maxSize = MSJava.globalConfig.outputArraySize;
	{ buffer = new byte[maxSize];  };
	index = 0;
}

@Override
public void close () throws MException
{
	writeByte((byte)0);
}

@Override
public void  writeByte (byte b) throws MException
{
	MSJava.assertion(index != -1, EC_DATA, "output closed");
	MSJava.assertion(index < maxSize, EC_DATA, "output: buffer overflow");
	buffer[index++] = b;
}

@Override
public MSOutputPrint  print (char x) throws MException
{
	writeByte((byte)x);
	return this;
}

@Override
public MSOutputPrint  print (String x) throws MException
{
	byte buffer [];
	buffer = x.getBytes(java.nio.charset.StandardCharsets.UTF_8);
	for (int i = 0; i < buffer.length; i++)
	{
		writeByte(buffer[i]);
	}
	return this;
}

public String getString()
{
	return MSJava.bytesToString(buffer,0,index-1);
}

public void  print () throws MException
{
	MSJava.printOut.print("[[[MSOutputPrint.print: TODO]]]").endLine();
}

}
