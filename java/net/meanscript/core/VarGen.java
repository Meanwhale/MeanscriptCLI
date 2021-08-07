package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class VarGen extends MC {
 int size;
 int tag;
 int address;
 int arraySize;
 boolean isReference;

public VarGen (int _size, int _tag, int _address, int _arraySize, boolean _isReference)
{
	size = _size;
	tag = _tag;
	address = _address;
	arraySize = _arraySize;
	isReference = _isReference;
}

}
