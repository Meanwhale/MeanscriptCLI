package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;





public class MArgs extends MC {
 ByteCode byteCode;
 StructDef structDef;
 int baseIndex; // stack base where struct data start from
 boolean valid; // become invalid when stack changes

public MArgs (ByteCode _byteCode, StructDef _structDef, int _base)
{
	byteCode = _byteCode;
	structDef = _structDef;
	baseIndex = _base;
	valid = true;
}

}
