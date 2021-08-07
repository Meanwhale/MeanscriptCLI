package net.meanscript.java;
import net.meanscript.core.*;

public class MException extends Exception
{
	private final static String begin = "---------------- EXCEPTION ----------------\n";
	private final static String end   = "\n-------------------------------------------";
	public final MSJavaError error;
	private String info = "Meanscript exception";
	public MException()
	{
		error = null;
	}
	public MException(MSJavaError _error, String msg) {
		error = _error;

		if (error == null)
		{
			if (msg != null && !msg.isEmpty()) info = msg;
			else info = "<no message>";
		}
		else if (error.errorClass == MC.EC_CLASS)
		{
			info = error.title;
			if (msg != null && !msg.isEmpty()) info += "\n" + msg;
		}
		else 
		{
			info = error.errorClass.title + ": " + error.title;
			if (msg != null && !msg.isEmpty()) info += "\n" + msg;
		}
		
		/*
		// NOTE: GWT doesn't support Java Threads
		StackTraceElement[] st = Thread.currentThread().getStackTrace();
		if (st.length > 4)
		{
			info += "\n\n" + st[1].toString();
			info += "\n> " + st[2].toString();
			info += "\n> " + st[3].toString();
			info += "\n> " + st[4].toString();
			info += "\n...";
		}
		*/
	}
	public String toString()
	{
		return begin + info + end;
	}
}

// Java END
