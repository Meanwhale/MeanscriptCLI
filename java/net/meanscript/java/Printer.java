package net.meanscript.java;
import net.meanscript.MSOutputPrint;
import net.meanscript.core.*;

public class Printer extends net.meanscript.MSOutputPrint
{

	@Override
	public void writeByte(byte x) throws MException {

		System.out.print((char)x);
	}
	@Override
	public MSOutputPrint print(String x) {

		System.out.print(x);
		return this;
	}
	@Override
	public MSOutputPrint print(char x) {

		System.out.print(x);
		return this;
	}


}
// Java END
