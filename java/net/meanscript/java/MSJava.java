package net.meanscript.java;
import net.meanscript.core.*;
import net.meanscript.*;

public abstract class MSJava
{
	public static boolean debug = true;
	
	public static final MSGlobal globalConfig = new MSGlobal();
	
	public static MSOutputPrint printOut = new Printer();
	public static MSOutputPrint errorOut = new Printer();
	public static MSOutputPrint userOut = new Printer();

	public static void assertion(boolean b, String msg) throws MException
	{
		if (!b)
		{
			throw new MException(MC.EC_INTERNAL, msg);
		}
	}
	public static void assertion(boolean b, MSJavaError error, String msg) throws MException
	{
		if (!b)
		{
			throw new MException(error, msg);
		}
	}
	public static void syntaxAssertion(boolean b, NodeIterator nit, String msg) throws MException
	{
		if (!b)
		{
			if (nit != null)
			{
				msg += "\nLine: " + nit.line();
			}
			throw new MException(MC.EC_SYNTAX, msg);
		}
	}

	public static float parseFloat(String s) throws MException
	{
		try {
			return Float.parseFloat(s);
		} catch (NumberFormatException e) {
			assertion(false, MC.EC_SYNTAX, "malformed float: " + s);
			return Float.NaN;
		}
	}
	public static int floatToIntFormat(float f)
	{
		return Float.floatToIntBits(f);
	}
	public static float intFormatToFloat(int i)
	{
		return Float.intBitsToFloat(i);
	}
	public static byte[] intsToBytes(int [] ia, int iaOffset, int bytesLength)
	{
		byte [] bytes = new byte[bytesLength];

	    int shift = 0;
	    for (int i = 0; i < bytesLength;)
	    {
	        bytes[i] = (byte)((ia[iaOffset + (i/4)] >> shift) & 0x000000FF);

	        i++;
	        if (i % 4 == 0) shift = 0;
	        else shift += 8;
	    }
		return bytes;
	}
	
	public static int[] bytesToInts(byte[] ba) 
	{
		int bytesLength = ba.length;
		int intsLength = (bytesLength / 4) + 1;
	    int[] ints = new int[intsLength];

	    // bytes:	b[0] b[1] b[2] b[3] b[4] b[5]...
	    // ints:	_________i[0]______|_________i[1]__...

	    int shift = 0;
	    for (int i = 0; i < bytesLength;)
	    {
	        ints[i/4] += (ba[i] & 0x000000FF) << shift;
	        
	        i++;
	        if (i % 4 == 0) shift = 0;
	        else shift += 8;
	    }
	    return ints;
	}
	public static void test()
	{
		String s = "Toimii!";
		byte [] bytes = s.getBytes();
		int [] ia = bytesToInts(bytes);
		byte [] ba = intsToBytes(ia,0,7);
		String ns = new String(ba);
		
		System.out.println("Toimii? " + s.equals(ns));
	}
}