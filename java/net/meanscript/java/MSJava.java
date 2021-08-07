package net.meanscript.java;
import net.meanscript.core.*;
import net.meanscript.*;

public abstract class MSJava
{
	public static boolean debug = true;
	
	public static final MSGlobal globalConfig = new MSGlobal();
	public static final MSTextComparator textComparator = new MSTextComparator();
	
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

	public static void nativeTest() throws MException
	{
		// 안녕하세요 = good morning
		MSText t = new MSText("안녕하세요");
		t.check();
		assertion(t.toString().equals("안녕하세요"), MC.EC_TEST, "");
	}
	
	public static int parseInt32(String s) throws MException
	{
		try {
			return Integer.parseInt(s);
		} catch (NumberFormatException e) {
			assertion(false, MC.EC_SYNTAX, "malformed int: " + s);
			return 0;
		}
	}
	public static long parseInt64(String s) throws MException
	{
		try {
			return Long.parseLong(s);
		} catch (NumberFormatException e) {
			assertion(false, MC.EC_SYNTAX, "malformed int64: " + s);
			return 0;
		}		
	}
	public static float parseFloat32(String s) throws MException
	{
		try {
			return Float.parseFloat(s);
		} catch (NumberFormatException e) {
			assertion(false, MC.EC_SYNTAX, "malformed float: " + s);
			return Float.NaN;
		}
	}
	public static double parseFloat64(String s) throws MException
	{
		try {
			return Double.parseDouble(s);
		} catch (NumberFormatException e) {
			assertion(false, MC.EC_SYNTAX, "malformed float64: " + s);
			return Double.NaN;
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
	public static long float64ToInt64Format(double f)
	{
		return Double.doubleToLongBits(f);
	}
	public static double int64FormatToFloat64(long i)
	{
		return Double.longBitsToDouble(i);
	}
	public static byte[] intsToBytes(int [] ia, int iaOffset, int bytesLength)
	{
		byte [] bytes = new byte[bytesLength];
		MC.intsToBytes(ia, iaOffset, bytes, 0, bytesLength);
		return bytes;
	}
	public static String bytesToString(byte[] tmp, int i, int length) {
		return new String(tmp,i,length);
	}
	
	public static void test()
	{
		/*long max = Long.MAX_VALUE;
		int high = highBits(max);
		int low = lowBits(max);
		long max2 = toLong(high,low);

		max = -1;
		high = highBits(max);
		low = lowBits(max);
		max2 = toLong(high,low);

		max = Long.MIN_VALUE;
		high = highBits(max);
		low = lowBits(max);
		max2 = toLong(high,low);*/
		
		String s = "Toimii!";
		/*byte [] bytes = s.getBytes();
		int [] ia = bytesToInts(bytes);
		byte [] ba = intsToBytes(ia,0,7);
		String ns = new String(ba);
		
		System.out.println("Toimii? " + s.equals(ns));*/
	}
}