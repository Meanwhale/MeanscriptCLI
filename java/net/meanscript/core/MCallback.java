package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class MCallback extends MC {
 MJCallbackAction func;
 int returnType;
 StructDef argStruct;

public MCallback (MJCallbackAction _func, int _returnType, StructDef _argStruct)
{
	func=_func;
	returnType=_returnType;
	argStruct=_argStruct;
}

//;

}
