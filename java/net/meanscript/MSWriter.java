package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;
public class MSWriter extends MC {
MSBuilder builder;
StructDef sd;
int baseAddress;

public MSWriter (MSBuilder _builder, StructDef _sd, int _address)
{
	builder = _builder;
	sd = _sd;
	baseAddress = _address;
}

public void setInt (String name, int value) throws MException
{
	MSText tn = new MSText (name);
	int memberAddress = sd.getMemberAddress(tn);
	MSJava.assertion((sd.getMemberTag(tn) & VALUE_TYPE_MASK) == MS_TYPE_INT,   "not an integer type: " + name);
	builder.values[baseAddress + memberAddress] = value;
}

public void setInt64 (String name, long value) throws MException
{
	MSText tn = new MSText (name);
	int memberAddress = sd.getMemberAddress(tn);
	MSJava.assertion((sd.getMemberTag(tn) & VALUE_TYPE_MASK) == MS_TYPE_INT64,   "not a 64-bit integer type: " + tn);
	builder.values[baseAddress + memberAddress]     = int64highBits(value);
	builder.values[baseAddress + memberAddress + 1] = int64lowBits(value);
}
public void setFloat (String name, float value) throws MException
{
	MSText tn = new MSText (name);
	int memberAddress = sd.getMemberAddress(tn);
	MSJava.assertion((sd.getMemberTag(tn) & VALUE_TYPE_MASK) == MS_TYPE_FLOAT,   "not a float type: " + tn);
	builder.values[baseAddress + memberAddress] = MSJava.floatToIntFormat(value);
}
public void setFloat64 (String name, double value) throws MException
{
	MSText tn = new MSText (name);
	int memberAddress = sd.getMemberAddress(tn);
	MSJava.assertion((sd.getMemberTag(tn) & VALUE_TYPE_MASK) == MS_TYPE_FLOAT64,   "not a 64-bit float type: " + tn);
	long tmp = MSJava.float64ToInt64Format(value);
	builder.values[baseAddress + memberAddress]     = int64highBits(tmp);
	builder.values[baseAddress + memberAddress + 1] = int64lowBits(tmp);
}
public void setBool (String name, boolean value) throws MException
{
	MSText tn = new MSText (name);
	int memberAddress = sd.getMemberAddress(tn);
	MSJava.assertion((sd.getMemberTag(tn) & VALUE_TYPE_MASK) == MS_TYPE_BOOL,   "not a boolean type: " + tn);
	builder.values[baseAddress + memberAddress] = (value ? 1 : 0);
}

public void setText (String name, String value) throws MException
{
	MSText tn = new MSText (name);
	int memberAddress = sd.getMemberAddress(tn);
	MSJava.assertion((sd.getMemberTag(tn) & VALUE_TYPE_MASK) == MS_TYPE_TEXT,   "not a text type: " + tn);
	int textID = builder.createText(value);
	builder.values[baseAddress + memberAddress] = textID;
}
//pw.setChars("code", "abcdefg");
public void setChars (String name, String value) throws MException
{
	MSText tn = new MSText (name);
	// get chars type
	int memberAddress = sd.getMemberAddress(tn);
	int tag = sd.getMemberTag(tn);
	int type = (int)(tag & VALUE_TYPE_MASK);
	StructDef charsDef = builder.semantics.getType(type);
	MSJava.assertion(charsDef.isCharsDef(),   "not a chars def.");
	//int maxChars = charsDef.numCharsForCharsDef();
	int size32 = charsDef.structSize;
	
	// TODO: check sizes
	// copy array
	stringToIntsWithSize(value, builder.values, baseAddress + memberAddress, size32);

}

}
