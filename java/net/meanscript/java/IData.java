package net.meanscript.java;
import net.meanscript.core.*;
import net.meanscript.*;

public abstract class IData {

	protected MeanMachine mm;
	protected MSCode mc;
	protected int[] code;
	protected int index;
	private int typeID;

	public IData (MSCode _mc, String s, int _typeID) throws MException
	{
		MSJava.assertion(_mc != null, "IData: MSCode is null");
		typeID = _typeID;
		mc = _mc;
		mm = mc.getMM();
		code = mm.byteCode.code;
		index = mm.globals.getMemberAddress(s, typeID);
	}
	public IData (MSBuilder _builder, String s, int _typeID) throws MException
	{
		mm = null;
		mc = null;
		typeID = _typeID;
		index = _builder.createGeneratedStruct(typeID, s);
		code = _builder.getValueArray();
	}
	protected IData (MSCode _mc, int _index, int _typeID) throws MException
	{
		MSJava.assertion(_mc != null, "IData: MSCode is null");
		typeID = _typeID;
		mc = _mc;
		mm = mc.getMM();
		code = mm.byteCode.code;
		index = _index;
	}
	public abstract String typeName();
	public int typeID() {return typeID;}
	protected int dataAt(int index)
	{
		return code[index];
	}
	protected void write(int index, int data)
	{
		code[index] = data;
	}
	protected void writeText(int index, String text, int max) throws MException
	{
		MC.stringToIntsWithSize(text, code, index, max);
	}
	protected String getChars(int index) throws MException
	{
		return new String(MSJava.intsToBytes(code, index+1, code[index]));
	}
	protected void setChars(int index, String text, int max) throws MException
	{
		MC.stringToIntsWithSize(text, code, index, max);
	}
	protected void indexCheck(int index, int arraySize) throws MException
	{
		if (index < 0 || index >= arraySize)
			throw new MException(MC.EC_DATA, "array index out of bounds: " + index + " (" + arraySize + ")");
	}
}
