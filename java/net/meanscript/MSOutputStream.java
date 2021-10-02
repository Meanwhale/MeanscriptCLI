package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;
public abstract class MSOutputStream extends MC {

public MSOutputStream ()
{
}
public abstract void  writeByte (byte b) throws MException;
public abstract void  close () throws MException;

public void  writeInt (int i) throws MException
{
	writeByte((byte)((i>>24) & 0xff));
	writeByte((byte)((i>>16) & 0xff));
	writeByte((byte)((i>>8) & 0xff));
	writeByte((byte)(i & 0xff));
}


}
