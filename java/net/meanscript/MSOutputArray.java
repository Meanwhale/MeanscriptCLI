package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;
public class MSOutputArray extends MSOutputStream {
 byte  buffer [];
 int maxSize;
 int index;

public MSOutputArray ()
{
	maxSize = MSJava.globalConfig.outputArraySize;
	{ buffer = new byte[maxSize];  };
	index = 0;
}

@Override
public void close ()
{
	index = -1;
}

@Override
public void  writeByte (byte b) throws MException
{
	MSJava.assertion(index != -1, EC_DATA, "output closed");
	MSJava.assertion(index < maxSize, EC_DATA, "output: buffer overflow");
	buffer[index++] = b;
}

public void  print ()
{
	MSJava.printOut.print("TODO").endLine();
}

}
