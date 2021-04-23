package net.meanscript;
import net.meanscript.core.*;
import net.meanscript.java.*;





public class MSGlobal extends MC {

// configuration parameters

public final int maxStructDefSize		= 4096;
public final int maxStructMembers		= 1024;
public final int ipStackSize			= 1024;
public final int baseStackSize			= 1024;
public final int maxFunctions			= 256;
public final int registerSize			= 256;
public final int maxArraySize			= 4096;
public final int maxNameLength			= 128;
public final int codeSize				= 65536; // 2^16
public final int stackSize				= 65536;
public final int builderValuesSize		= 65536;
public final int outputArraySize		= 65536;
public final int maxCallbacks			= 256;

boolean verbose = false;

// stream types

public final int STREAM_TYPE_FIRST			= 100001;
public final int STREAM_BYTECODE			= 100001;
public final int STREAM_SCRIPT				= 100002;
public final int STREAM_BYTECODE_READ_ONLY	= 100003;
public final int STREAM_TYPE_LAST			= 100003;
	
public void setVerbose (boolean b)
{
	verbose = b;
}
public boolean verboseOn ()
{
	return verbose;
}

}
