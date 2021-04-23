package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;





public class MSDataArray extends MC {
MeanMachine mm;
int tag,		// instruction code
    tagAddress, // address of the code
	dataIndex;  // address of the data
int [] structCode;	// where struct info is
int [] dataCode;	// where actual data is

public MSDataArray (MeanMachine _mm, int _tagAddress, int _dataIndex) throws MException
{
	mm = _mm;
	
	structCode = mm.getStructCode();
	dataCode = mm.getDataCode();

	tagAddress = _tagAddress;
	dataIndex = _dataIndex;
	tag = structCode[tagAddress];
	
	MSJava.assertion((tag & OPERATION_MASK) == OP_ARRAY_MEMBER,   "not an array");
}
public int getItemType ()
{
	return (int)(tag & VALUE_TYPE_MASK);
}

public int getArrayItemCount () throws MException
{
	return structCode[tagAddress + 3];
}

// TODO: getIntAt(i), getTextAt(i), etc.

public MSData getAt (int i) throws MException
{
	int itemCount = getArrayItemCount();
	MSJava.assertion(i >=0 && i < itemCount, EC_DATA, "index out of bounds");
	int arrayDataSize = structCode[tagAddress + 2];
	int arrayItemSize = arrayDataSize / itemCount;
	int itemAddress = dataIndex + (i * arrayItemSize);
	return new MSData (mm,tagAddress, itemAddress, true);
}

//;

}
