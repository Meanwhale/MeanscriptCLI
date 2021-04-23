package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;





public abstract class MSOutputPrint extends MSOutputStream {

public abstract void  writeByte (byte x) throws MException;
public abstract MSOutputPrint  print (byte x);
public abstract MSOutputPrint  print (int x);
public abstract MSOutputPrint  print (String x);
public abstract MSOutputPrint  print (float x);

public MSOutputPrint ()
{
}

@Override
public void close ()
{
}

public static final String [] hexs = new String[]
{
	"0","1","2","3",
	"4","5","6","7",
	"8","9","a","b",
	"c","d","e","f"
};

public MSOutputPrint  printHex (int h)
{
	for (int i = 28; i >= 0; i -=4 )
	{
		int index = (h>>i);
		index &= 0x0000000f;
		print(hexs[index]);
	}
	return this;
}

public MSOutputPrint  endLine ()
{
	print("\n");
	return this;
}

}
