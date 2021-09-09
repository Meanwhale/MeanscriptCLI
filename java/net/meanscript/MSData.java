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
// MAP_STRING_TO_INT(globalNames);

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
	String s = new String(MSJava.intsToBytes(structCode, address + 2, numChars),java.nio.charset.StandardCharsets.UTF_8);
	return s;
}
public MSText  getMSText (int id) throws MException
{
	if (id == 0) return new MSText("");
	int address = mm.texts[id]; // operation address
	return new MSText(structCode,address+1);
}

public boolean isChars (int typeID)
{
	int charsTypeAddress = mm.types[typeID];
	int charsTypeTag = structCode[charsTypeAddress];
	return (charsTypeTag & OPERATION_MASK) == OP_CHARS_DEF;
}

public int getCharsSize (int typeID)
{
	int index = mm.types[typeID];
	int typeTagID = structCode[index] & VALUE_TYPE_MASK;
	int potentialCharsTag = structCode[mm.types[typeTagID]];
	if ((potentialCharsTag & OPERATION_MASK) == OP_CHARS_DEF)
	{
		return structCode[mm.types[typeTagID] + 1];
	}
	else return -1;
}

public String getChars (String name) throws MException
{
	int memberTagAddress = getMemberTagAddress(name, false);
	
	int charsTag = structCode[memberTagAddress];
	int typeID = instrValueTypeID(charsTag);
	MSJava.assertion(isChars(typeID), EC_DATA, "not chars");
	
	MSJava.assertion(memberTagAddress >= 0, EC_DATA, "not found: " + name);
	int address = dataIndex + structCode[memberTagAddress + 1];
	int numChars = dataCode[address];
	String s = new String(MSJava.intsToBytes(dataCode, address + 1, numChars),java.nio.charset.StandardCharsets.UTF_8);
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
	MSJava.assertion(getType() >= MS_TYPE_INT,   "not a 32-bit integer");
	return dataCode[dataIndex];
}


public int getInt (String name) throws MException
{
	int address = getMemberAddress(name, MS_TYPE_INT);
	MSJava.assertion(address >= 0, EC_DATA, "unknown name");
	return dataCode[address];
}

public boolean getBool () throws MException
{
	MSJava.assertion(getType() >= MS_TYPE_BOOL,   "not a bool integer");
	return dataCode[dataIndex] != 0;
}

public boolean getBool (String name) throws MException
{
	int address = getMemberAddress(name, MS_TYPE_BOOL);
	MSJava.assertion(address >= 0, EC_DATA, "unknown name");
	return dataCode[address] != 0;
}

public long getInt64 () throws MException
{
	return getInt64At(dataIndex);
}

public long getInt64 (String name) throws MException
{
	int address = getMemberAddress(name, MS_TYPE_INT64);
	return getInt64At(address);
}
public long getInt64At (int address) throws MException
{
	int a = dataCode[address];
	int b = dataCode[address+1];
	long i64 = intsToInt64(a,b);
	return i64;
}

public double getFloat64 () throws MException
{
	return getFloat64At(dataIndex);
}

public double getFloat64 (String name) throws MException
{
	int address = getMemberAddress(name, MS_TYPE_FLOAT64);
	return getFloat64At(address);
}
public double getFloat64At (int address) throws MException
{
	long i64 = getInt64At(address);
	return MSJava.int64FormatToFloat64(i64);
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
	
	while ((code & OPERATION_MASK) == OP_MEMBER_NAME) // TODO: handle the case of "no name"
	{
		//{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("check member name").endLine();};
		//{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("member: " + new String(MSJava.intsToBytes(structCode, i+2, structCode[i+1]),java.nio.charset.StandardCharsets.UTF_8)).endLine();};

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

public void  printType (MSOutputPrint op, int typeID) throws MException
{
	if (typeID < MAX_MS_TYPES) op.print(primitiveNames[typeID]);
	else
	{
		int charsSizeOrNegative = getCharsSize(typeID);
		if (charsSizeOrNegative >= 0)
		{
			op.print("chars[");
			op.print(charsSizeOrNegative);
			op.print("]");
		}
		else
		{
			int index = mm.types[typeID];
			MSText typeName = new MSText (structCode,index+1);
			op.print(typeName);
		}
	}
}

public void  printData (MSOutputPrint op, int depth, String name) throws MException
{
	//for (int x=0; x<depth; x++) op.print("    ");
	
	if (!isStruct())
	{
		if (depth != 1) op.print(name);
		op.print(": ");
		if (getType() == MS_TYPE_INT)
		{
			op.print(dataCode[dataIndex]);
		}
		else if (getType() == MS_TYPE_INT64)
		{
			op.print(intsToInt64(dataCode[dataIndex], dataCode[dataIndex+1]));
		}
		else if (getType() == MS_TYPE_FLOAT)
		{
			op.print(MSJava.intFormatToFloat(dataCode[dataIndex]));
		}
		else if (getType() == MS_TYPE_FLOAT64)
		{
			op.print(getFloat64At(dataIndex));
		}
		else if (getType() == MS_TYPE_TEXT)
		{
			MSText tmp = getMSText(dataCode[dataIndex]);
			op.print("\"");
			op.printIntsToChars(tmp.getData(), 1, tmp.numBytes(), true);
			op.print("\"");
		}
		else if (getType() == MS_TYPE_BOOL)
		{
			if (dataCode[dataIndex] == 0) op.print("false");
			else op.print("true");
		}
		else
		{
			MSJava.assertion(false, EC_DATA, "printData: unhandled data type: " + getType());
		}
		op.print("\n");
	}
	else
	{	
		// NOTE: similar to getMemberTagAddress()
		
		int i = mm.types[getType()];
		int code = structCode[i];
		
		if ((code & OPERATION_MASK) == OP_CHARS_DEF)
		{
			int numChars = dataCode[dataIndex + 0];
			if (depth != 1) op.print(name);
			op.print(": \"");
			op.printIntsToChars(dataCode, dataIndex + 1, numChars, true);
			op.print("\"\n");
			return;
		}
		if (depth == 1) op.print("\n");
					
		MSJava.assertion((code & OPERATION_MASK) == OP_STRUCT_DEF,   "printData: struct def. expected");
				
		i += instrSize(code) + 1;
		code = structCode[i];
		while ((code & OPERATION_MASK) == OP_MEMBER_NAME)
		{
			String s = new String(MSJava.intsToBytes(structCode, i + 2, structCode[i+1]),java.nio.charset.StandardCharsets.UTF_8);
	
			i += instrSize(code) + 1;
			code = structCode[i];
			
			// print type name
			
			if (depth == 0) 
			{
				int typeID = (int)(code & VALUE_TYPE_MASK);
				printType(op, typeID);
				op.print(" ");
				op.print(s);
			}
			
			if ((code & OPERATION_MASK) == OP_STRUCT_MEMBER)
			{
				int dataAddress = dataIndex + structCode[i + 1];
				MSData d = new MSData (mm, i, dataAddress, false);
				if (depth > 0) d.printData(op, depth + 1, name + "." + s);
				else d.printData(op, depth + 1, s);
			}
			else if ((code & OPERATION_MASK) == OP_ARRAY_MEMBER)
			{
				int dataAddress = dataIndex + structCode[i + 1];
				MSDataArray a = new MSDataArray (mm, i, dataAddress);
					
				//for (int x=0; x<depth+1; x++) op.print("    ");
				//op.print(s);
				
				
				// iterate thru array items

				for (int n=0; n<a.getArrayItemCount(); n++)
				{
					MSData d = a.getAt(n);
					//for (int x=0; x<depth; x++) op.print("  ");


					String tmp = "";
					if (depth > 0) tmp = name + "." + s;
					else tmp = s;	
					
					String indexText = "" +  n;;
					tmp += "[";
					tmp += indexText;
					tmp += "]";
					d.printData(op, depth + 1, tmp);
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
