package net.meanscript.java;
import java.io.FileWriter;
import java.io.IOException;

import net.meanscript.MSOutputPrint;
import net.meanscript.core.MC;

public class MSFilePrint extends net.meanscript.MSOutputPrint
{
	FileWriter fw;
	
	public MSFilePrint (String fileName) throws MException
	{
		try {
			fw = new FileWriter(fileName);
		} catch (IOException e) {
			throw new MException(MC.EC_NATIVE, "file open error");
		}
	}

	@Override
	public void close()
	{
		try { fw.flush(); fw.close(); }
		catch (IOException e) { e.printStackTrace(); }
	}
	
	@Override
	public void writeByte(byte x) throws MException
	{
		try {
			fw.write(x);
		} catch (IOException e) {
			throw new MException(MC.EC_NATIVE, "file write error");
		}
	}

	@Override
	public MSOutputPrint print(byte x)
{
		try {
			fw.write(x);
		} catch (IOException e) {
			MSJava.errorOut.print("file write error");
			return null;
		}
		return this;
	}

	@Override
	public MSOutputPrint print(int x)
{
		try {
			fw.write(String.valueOf(x));
		} catch (IOException e) {
			MSJava.errorOut.print("file write error");
			return null;
		}
		return this;
	}


	@Override
	public MSOutputPrint print(String x)
{
		try {
			fw.write(x);
		} catch (IOException e) {
			MSJava.errorOut.print("file write error");
			return null;
		}
		return this;
	}


	@Override
	public MSOutputPrint print(float x)
{
		try {
			fw.write(String.valueOf(x));
		} catch (IOException e) {
			MSJava.errorOut.print("file write error");
			return null;
		}
		return this;
	}


}
// Java END