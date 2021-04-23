package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;





public class MCallback extends MC {
 String name;
 MJCallbackAction func;
 int returnType;
 StructDef argStruct;

public MCallback (String _name, MJCallbackAction _func, int _returnType, StructDef _argStruct)
{
	name=_name;
	func=_func;
	returnType=_returnType;
	argStruct=_argStruct;
}

//;

}
