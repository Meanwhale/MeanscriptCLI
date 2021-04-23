package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;





public class MSData extends MC {
MeanMachine mm;
int tag,		// instruction code
    tagAddress, // address of the code
	dataIndex;  // address of the data
int [] structCode;	// where struct info is
int [] dataCode;	// where actual data is

// TODO: make a map if wanted

// variables to access from source code by name.
// value is the address of the variable tag.
// java.util.TreeMap<String, Integer> globalNames = new java.util.TreeMap<String, Integer>();

public MSData (MeanMachine _mm, int _tagAddress, int _dataIndex, boolean arrayItem) throws MException
{
	mm = _mm;
	
	structCode = mm.getStructCode();
	dataCode = mm.getDataCode();

	mm = _mm;
	tagAddress = _tagAddress;
	dataIndex = _dataIndex;
	tag = structCode[tagAddress];
	
	MSJava.assertion(arrayItem == isArrayItem(),   "array item error");
}

public int getType ()
{
	return (int)(tag & VALUE_TYPE_MASK);
}

public boolean isStruct ()
{
	return ((getType() == 0) || (getType() >= MAX_MS_TYPES));
}

public boolean hasData (String name) throws MException
{
	int memberTagAddress = getMemberTagAddress(name, false);
	return memberTagAddress >= 0;
}

public boolean hasArray (String name) throws MException
{
	int memberTagAddress = getMemberTagAddress(name, true);
	return memberTagAddress >= 0;
}

public String getText () throws MException
{
	MSJava.assertion(getType() >= MS_TYPE_TEXT,   "not a text");
	return getText(dataCode[dataIndex]);
}

public String getText (String name) throws MException
{
	int address = getMemberAddress(name, MS_TYPE_TEXT);
	MSJava.assertion(address >= 0, EC_DATA, "unknown name");
	return getText(dataCode[address]);
}
public String  getText (int id)
{
	if (id == 0) return "";
	int address = mm.texts[id];
	int numChars = structCode[address + 1];
	String s = new String(MSJava.intsToBytes(structCode, address + 2, numChars));
	return s;
}

public String getChars (String name) throws MException
{
	// TODO: check type
	int address = getMemberAddress(name);
	int numChars = dataCode[address];
	String s = new String(MSJava.intsToBytes(dataCode, address + 1, numChars));
	return s;
}

public float getFloat () throws MException
{
	MSJava.assertion(getType() != MS_TYPE_FLOAT,   "not a float");
	return MSJava.intFormatToFloat(dataCode[dataIndex]);
}

public float getFloat (String name) throws MException
{
	int address = getMemberAddress(name, MS_TYPE_FLOAT);
	MSJava.assertion(address >= 0, EC_DATA, "unknown name");
	return MSJava.intFormatToFloat(dataCode[address]);
}

public int getInt () throws MException
{
	MSJava.assertion(getType() >= MS_TYPE_INT,   "not an integer");
	return dataCode[dataIndex];
}

public int getInt (String name) throws MException
{
	int address = getMemberAddress(name, MS_TYPE_INT);
	MSJava.assertion(address >= 0, EC_DATA, "unknown name");
	return dataCode[address];
}

public boolean isArrayItem ()
{
	// NOTE: this should be true only if returned from MSDataArray 
	return (tag & OPERATION_MASK) == OP_ARRAY_MEMBER;
}

public MSDataArray getArray (String name) throws MException
{
	int arrayTagAddress = getMemberTagAddress(name, true);
	MSJava.assertion(arrayTagAddress >= 0, EC_DATA, "not found: " + name);
	int arrayTag = structCode[arrayTagAddress];
	MSJava.assertion((arrayTag & OPERATION_MASK) == OP_ARRAY_MEMBER, EC_DATA, "not an array");
	int dataAddress = dataIndex + structCode[arrayTagAddress + 1];
	return new MSDataArray (mm, arrayTagAddress, dataAddress);
}

public MSData getMember (String name) throws MException
{
	int memberTagAddress = getMemberTagAddress(name, false);
	MSJava.assertion(memberTagAddress >= 0, EC_DATA, "not found: " + name);
	int dataAddress = dataIndex + structCode[memberTagAddress + 1];
	return new MSData (mm, memberTagAddress, dataAddress, false);
}

public int getMemberAddress (String name, int type) throws MException
{
	int memberTagAddress = getMemberTagAddress(name, false);
	MSJava.assertion(memberTagAddress >= 0, EC_DATA, "not found: " + name);
	MSJava.assertion(((structCode[memberTagAddress]) & VALUE_TYPE_MASK) == type, EC_DATA, "wrong type");
	return dataIndex + structCode[memberTagAddress + 1];
}

public int getMemberAddress (String name) throws MException
{
	int memberTagAddress = getMemberTagAddress(name, false);
	MSJava.assertion(memberTagAddress >= 0, EC_DATA, "not found: " + name);
	// address of this data + offset of the member
	return dataIndex + structCode[memberTagAddress + 1];
}

public int getMemberTagAddress (String name, boolean isArray) throws MException
{
	MSJava.assertion(isStruct(),   "struct expected");
	
	int i = mm.types[getType()];
	int code = structCode[i];
	MSJava.assertion((code & OPERATION_MASK) == OP_STRUCT_DEF,   "struct def. expected");
	i += instrSize(code) + 1;
	code = structCode[i];
	
	// change 'name' to int array to compares names
	
	int [] nameIntsTmp = new int[MSJava.globalConfig.maxNameLength];
	
	stringToIntsWithSize(name, nameIntsTmp, 0, MSJava.globalConfig.maxNameLength);
	
	// go thru members
	
	while ((code & OPERATION_MASK) == OP_MEMBER_NAME)
	{
		//{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("check member name").endLine();};
		//{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("member: " + new String(MSJava.intsToBytes(structCode, i+2, structCode[i+1]))).endLine();};

		// compare names
		if (intStringsWithSizeEquals(nameIntsTmp, 0, structCode, i+1))
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("MATCH!").endLine();};
			i += instrSize(code) + 1;
			code = structCode[i];
			
			if (isArray)	{ MSJava.assertion((code & OPERATION_MASK) == OP_ARRAY_MEMBER,   "array expected"); }
			else			{ MSJava.assertion((code & OPERATION_MASK) == OP_STRUCT_MEMBER,   "array not expected"); };
			
			return i;
		}

		i += instrSize(code) + 1;
		code = structCode[i];
		
		MSJava.assertion((code & OPERATION_MASK) == OP_STRUCT_MEMBER || (code & OPERATION_MASK) == OP_ARRAY_MEMBER,   "struct member expected");

		i += instrSize(code) + 1;
		code = structCode[i];
	}	
	return -1;
}

public void  printData (int depth, String name) throws MException
{
		
	for (int x=0; x<depth; x++) MSJava.printOut.print("    ");
	MSJava.printOut.print(name);
	
	if (!isStruct())
	{
		if (getType() == MS_TYPE_FLOAT)
		{
			MSJava.printOut.print(MSJava.intFormatToFloat(dataCode[dataIndex])).endLine();
		}
		else if (getType() == MS_TYPE_TEXT)
		{
			MSJava.printOut.print(getText(dataCode[dataIndex])).endLine();
		}
		else
		{
			MSJava.printOut.print(dataCode[dataIndex]).endLine();
		}
	}
	else
	{
		// NOTE: similar to getMemberTagAddress()
		
		int i = mm.types[getType()];
		int code = structCode[i];
		
		if ((code & OPERATION_MASK) == OP_CHARS_DEF)
		{
			int numChars = dataCode[dataIndex + 0];
			String s = new String(MSJava.intsToBytes(dataCode, dataIndex + 1, numChars));
			MSJava.printOut.print(s).endLine();
			return;
		}
		MSJava.printOut.print("").endLine();
		MSJava.assertion((code & OPERATION_MASK) == OP_STRUCT_DEF,   "printData: struct def. expected");
		i += instrSize(code) + 1;
		code = structCode[i];
		while ((code & OPERATION_MASK) == OP_MEMBER_NAME)
		{
			
			String s = new String(MSJava.intsToBytes(structCode, i + 2, structCode[i+1]));
	
			i += instrSize(code) + 1;
			code = structCode[i];
			
			if ((code & OPERATION_MASK) == OP_STRUCT_MEMBER)
			{
				int dataAddress = dataIndex + structCode[i + 1];
				MSData d = new MSData (mm, i, dataAddress, false);
				s += ": ";
				d.printData(depth + 1, s);
			}
			else if ((code & OPERATION_MASK) == OP_ARRAY_MEMBER)
			{
				int dataAddress = dataIndex + structCode[i + 1];
				MSDataArray a = new MSDataArray (mm, i, dataAddress);
					
				for (int x=0; x<depth+1; x++) MSJava.printOut.print("    ");
				MSJava.printOut.print(s);
				MSJava.printOut.print(": ");
				
				// iterate thru array items

				MSJava.printOut.print("").endLine();
				for (int n=0; n<a.getArrayItemCount(); n++)
				{
					MSData d = a.getAt(n);
					for (int x=0; x<depth; x++) MSJava.printOut.print("  ");
					String indexText = "" +  n;;
					String tmp = "[";
					tmp += indexText;
					tmp += "] ";
					d.printData(depth + 1, tmp);
				}
			}
			else
			{
				MSJava.assertion(false,   "broken struct code");
			}

			i += instrSize(code) + 1;
			code = structCode[i];
		}	
	}
}

//;

}
