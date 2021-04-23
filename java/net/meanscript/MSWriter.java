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
	int memberAddress = sd.getMemberAddress(name);
	MSJava.assertion((sd.getMemberTag(name) & VALUE_TYPE_MASK) == MS_TYPE_INT,   "not an integer type: " + name);
	builder.values[baseAddress + memberAddress] = value;
}

public void setText (String name, String value) throws MException
{
	int memberAddress = sd.getMemberAddress(name);
	MSJava.assertion((sd.getMemberTag(name) & VALUE_TYPE_MASK) == MS_TYPE_TEXT,   "not a text type: " + name);
	int textID = builder.createText(value);
	builder.values[baseAddress + memberAddress] = textID;
}
//pw.setChars("code", "abcdefg");
public void setChars (String name, String value) throws MException
{
	// get chars type
	int memberAddress = sd.getMemberAddress(name);
	int tag = sd.getMemberTag(name);
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
