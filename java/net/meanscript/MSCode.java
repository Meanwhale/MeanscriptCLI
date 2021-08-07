package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;
public class MSCode extends MC {
Common common;

MeanMachine mm;

boolean initialized;

public MSCode () throws MException
{
	common = new Common();
	mm = null;
	initialized = false;
	
	reset();
}
public MSCode (MSInputStream input, int streamType) throws MException
{
	MSJava.assertion(streamType >= MSJava.globalConfig.STREAM_TYPE_FIRST && streamType <= MSJava.globalConfig.STREAM_TYPE_LAST,   "unknown stream type");
	
	common = new Common();
	mm = null;
	initialized = false;
	
	if (streamType == MSJava.globalConfig.STREAM_BYTECODE)
	{
		initBytecode(input);
	}
	else if (streamType == MSJava.globalConfig.STREAM_SCRIPT)
	{
		mm = new MeanMachine(compile(input));
	}
	else
	{
		MSJava.assertion(false,   "unknown stream type");
	}
}

public MeanMachine getMM ()
{
	return mm;
}

//;

public void reset ()
{
	mm = null;
	initialized = false;
}

public boolean isInitialized ()
{
	return initialized;
}

public void checkInit () throws MException
{
	MSJava.assertion(initialized,   "MSCode is not initialized");
}

public void initBytecode (MSInputStream input) throws MException
{
	reset();
	
	ByteCode byteCode = new ByteCode(common, input);
	mm = new MeanMachine(byteCode);
	
	initialized = true;
}

public void initBytecode (ByteCode bc) throws MException
{
	reset();
	
	ByteCode byteCode = new ByteCode(bc);
	mm = new MeanMachine(byteCode);
	
	initialized = true;
}

public boolean hasData (String name) throws MException
{
	return mm.globals.hasData(name);
}

public boolean hasArray (String name) throws MException
{
	return mm.globals.hasArray(name);
}

public boolean getBool (String name) throws MException
{
	checkInit();
	return mm.globals.getBool(name);
}

public int getInt (String name) throws MException
{
	checkInit();
	return mm.globals.getInt(name);
}

public long getInt64 (String name) throws MException
{
	checkInit();
	return mm.globals.getInt64(name);
}

public float getFloat (String name) throws MException
{
	checkInit();
	return mm.globals.getFloat(name);
}
public double getFloat64 (String name) throws MException
{
	checkInit();
	return mm.globals.getFloat64(name);
}

public String getText (String name) throws MException
{
	checkInit();
	return mm.globals.getText(name);
}

public String getChars (String name) throws MException
{
	checkInit();
	return mm.globals.getChars(name);
}

public String getText (int textID) throws MException
{
	checkInit();
	return mm.globals.getText(textID);
}

public MSData getData (String name) throws MException
{
	checkInit();
	return mm.globals.getMember(name);
}

public MSDataArray getArray (String name) throws MException
{
	checkInit();
	return mm.globals.getArray(name);
}

public void writeReadOnlyData (MSOutputStream output) throws MException
{
	mm.writeReadOnlyData(output);
}

public void writeCode (MSOutputStream output) throws MException
{
	mm.writeCode(output);
}

public void printCode () throws MException
{
	if (initialized) mm.printCode();
	else MSJava.printOut.print("printCode: MSCode is not initialized").endLine();
}

public void printDetails () throws MException
{
	if (initialized) mm.printDetails();
	else MSJava.printOut.print("printDetails: MSCode is not initialized").endLine();
}

public void printData () throws MException
{
	if (initialized) mm.dataPrint();
	else MSJava.printOut.print("printDetails: MSCode is not initialized").endLine();
}

public ByteCode compile (MSInputStream input) throws MException
{
	reset();
	
	TokenTree tree = Parser.Parse (input);
	Semantics semantics = new Semantics();
	common.initialize(semantics);
	semantics.analyze(tree);
	ByteCode bc = Generator.generate (tree, semantics, common);
	tree = null;	
	
	initialized = true;
	return bc;
}

public void run () throws MException
{
	MSJava.assertion(mm != null,   "not initialized");
	mm.callFunction(0);
}

public void step () throws MException
{
	MSJava.assertion(false,   "TODO");
}

public void compileAndRun (String s) throws MException
{
	reset();
	MSInputArray ia = new MSInputArray(s);
	ByteCode bc = compile(ia);
	ia = null;
	mm = new MeanMachine(bc);
	run();
}

//public void compileAndRun (byte[] script) throws MException
//{
//	compile(script);
//	run();
//}


}
