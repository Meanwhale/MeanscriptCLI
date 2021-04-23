package net.meanscript.core;
import net.meanscript.java.*;





public class MC {



// node types

public static final int NT_ROOT						= 0;
public static final int NT_EXPR						= 1;
public static final int NT_PARENTHESIS				= 2;
public static final int NT_ASSIGNMENT				= 3;
public static final int NT_SQUARE_BRACKETS			= 4;
public static final int NT_CODE_BLOCK				= 5;
public static final int NT_NAME_TOKEN				= 6;
public static final int NT_NUMBER_TOKEN				= 7;
public static final int NT_REFERENCE_TOKEN			= 8;	// e.g. "#foo", as in "increase #foo"
public static final int NT_REF_TYPE_TOKEN			= 9;	// e.g. "int#", as int "func void increase [int# value] { value += 1 }
public static final int NT_DOT						= 10;
public static final int NT_PLUS						= 11;
public static final int NT_MINUS						= 12;
public static final int NT_DIV						= 13;
public static final int NT_MUL						= 14;
public static final int NT_TEXT						= 15;
public static final int NT_MEMBER					= 16;
public static final int NT_COMMA						= 17;

// bytecode types

public static final int BYTECODE_READ_ONLY			= 0x101;
public static final int BYTECODE_EXECUTABLE			= 0x102;


// instructions

public static final int OP_SYSTEM					= 0x00000000; // system calls (ERROR, assert, exception, etc. TBD)
public static final int OP_CALLBACK_CALL				= 0x03000000;
public static final int OP_JUMP						= 0x04000000;
public static final int OP_GO_BACK					= 0x05000000; // return to previous block. named to be less confusing
public static final int OP_GO_END					= 0x06000000; // go to end of the function (context's end address)
public static final int OP_CHARS_DEF					= 0x07000000;
public static final int OP_STRUCT_DEF				= 0x08000000;
public static final int OP_STRUCT_MEMBER				= 0x09000000;
public static final int OP_SAVE_BASE					= 0x0a000000; // save/load stack base index
public static final int OP_LOAD_BASE					= 0x0b000000;
public static final int OP_NOOP						= 0x0c000000;
public static final int OP_ADD_TEXT					= 0x10000000; // add immutable text to text map and add index to register
public static final int OP_PUSH_IMMEDIATE			= 0x11000000; // push immediate value to stack
public static final int OP_PUSH_REG_TO_STACK			= 0x13000000; // push content of register to stack
public static final int OP_FUNCTION					= 0x14000000; // introduce a function
public static final int OP_START_INIT				= 0x15000000;
public static final int OP_END_INIT					= 0x16000000;
public static final int OP_FUNCTION_CALL				= 0x17000000;
public static final int OP_PUSH_LOCAL				= 0x18000000;
public static final int OP_PUSH_GLOBAL				= 0x19000000;
public static final int OP_POP_STACK_TO_LOCAL		= 0x1a000000;
public static final int OP_POP_STACK_TO_GLOBAL		= 0x1b000000;
public static final int OP_POP_STACK_TO_REG			= 0x1c000000;
public static final int OP_MEMBER_NAME				= 0x1d000000;
public static final int OP_INIT_GLOBALS				= 0x1e000000;
public static final int OP_ARRAY_MEMBER				= 0x1f000000;
public static final int OP_MULTIPLY_GLOBAL_ARRAY_INDEX= 0x20000000;
public static final int OP_POP_STACK_TO_LOCAL_REF	= 0x21000000;
public static final int OP_POP_STACK_TO_GLOBAL_REF	= 0x22000000;
public static final int OP_PUSH_LOCAL_REF			= 0x23000000;
public static final int OP_PUSH_GLOBAL_REF			= 0x24000000;
public static final int OP_PUSH_CHARS				= 0x25000000;

public static final int OP_MAX						= 0x30000000;
public static final int NUM_OP						= 0x30;

public static final String [] opName = new String[] {
	"system",				"---OLD---",			"---OLD---",			"call",
	"jump",					"go back",				"go end",				"chars definition",
	"struct definition",	"struct member",		"save base",			"load base",
	"no operation",			"---OLD---",			"---OLD---",			"---OLD---",
	"text",					"push immediate",		"---OLD---",			"push from reg.",
	"function data",		"start init",			"end init",				"function call",
	"push local",			"push global",			"pop to local",			"pop to global",
	"pop to register", 		"member name",			"init globals",			"array member",
	"multiply array index",	"pop to local ref.",	"pop to global ref.",	"push local ref.",
	"push global ref.",		"push chars",			"---ERROR---",			"---ERROR---",
	"---ERROR---",			"---ERROR---",			"---ERROR---",			"---ERROR---",
	"---ERROR---",			"---ERROR---",			"---ERROR---",			"---ERROR---",
	};

public static final int KEYWORD_FUNC_ID		= 0;
public static final int KEYWORD_STRUCT_ID	= 1;
public static final int KEYWORD_RETURN_ID	= 2;
public static final int KEYWORD_GLOBAL_ID	= 3;
public static final int NUM_KEYWORDS			= 4;

public static final String [] keywords = new String[] {
	"func",
	"struct",
	"return",
	"global"
	};

  public static final String HORIZONTAL_LINE = "------------------------------------------";
//public static final String HORIZONTAL_LINE = "__________________________________________\n";

public static final int OPERATION_MASK	= 0xff000000;
//public static final int SCOPE_MASK			= 0x00f00000;
public static final int SIZE_MASK		= 0x00ff0000; // NOTE: erikoistapauksissa voisi käyttää 0x00FFFFFF
public static final int VALUE_TYPE_MASK= 0x0000ffff; // max. 64K

public static final int AUX_DATA_MASK	= 0x0000ffff; // same size as VALUE_TYPE_MASK for commands to use other data than value type.

public static final int OP_SHIFT				= 24;
//public static final int SCOPE_SHIFT		= 20;
public static final int SIZE_SHIFT			= 16;
public static final int VALUE_TYPE_SHIFT		= 0;

public static final int MS_TYPE_VOID			= 0;
public static final int MS_TYPE_INT			= 1;
public static final int MS_TYPE_TEXT			= 2;
public static final int MS_TYPE_BOOL			= 3;
public static final int MS_TYPE_CODE_ADDRESS	= 4;
public static final int MS_TYPE_ARGS			= 5;
public static final int MS_TYPE_FLOAT		= 6;
public static final int MS_TYPE_CHARS		= 7;
public static final int MAX_MS_TYPES			= 16;
public static final int MAX_TYPES			= 256;




// error classes

public static final MSJavaError EC_CLASS = new MSJavaError( null, "-");

public static final MSJavaError 	EC_PARSE	 = new MSJavaError(				EC_CLASS,		"Parse error");		// when building the token tree
public static final MSJavaError 	EC_SYNTAX	 = new MSJavaError(				EC_CLASS,		"Syntax error");	// ...analyzing and generating code
public static final MSJavaError 	EC_SCRIPT	 = new MSJavaError(				EC_CLASS,		"Script error");	// ...executing script
public static final MSJavaError 	EC_CODE		 = new MSJavaError(				EC_CLASS,		"Code error");		// ...resolving bytecode
public static final MSJavaError 	EC_DATA		 = new MSJavaError(				EC_CLASS,		"Data error");		// ...accessing/creating data
public static final MSJavaError 	EC_TEST		 = new MSJavaError(				EC_CLASS,		"Test error");		// ...unit test
public static final MSJavaError 	EC_NATIVE	 = new MSJavaError(				EC_CLASS,		"Native error");	// ...executing native code
public static final MSJavaError 	EC_INTERNAL	 = new MSJavaError(				EC_CLASS,		"|||||||| INTERNAL ERROR ||||||||");

public static final MSJavaError 	E_UNEXPECTED_CHAR = new MSJavaError(			EC_PARSE,		"Unexpected character");


public static int makeInstruction (int operation, int size, int auxData) throws MException
{
	MSJava.assertion((operation | OPERATION_MASK) == OPERATION_MASK,   "invalid operation");
	MSJava.assertion(size >= 0 && size < 256,   "wrong size");
	MSJava.assertion((auxData | VALUE_TYPE_MASK) == VALUE_TYPE_MASK,   "wrong aux. data");
	int sizeBits = size << SIZE_SHIFT;
	int instruction = operation | sizeBits | auxData;
	return instruction;
}
public static String getOpName(int instruction) throws MException
{
	int index = (int)(instruction & OPERATION_MASK) >> OP_SHIFT;
	index &= 0x000000ff;
	MSJava.assertion(index < NUM_OP, EC_CODE, "unknown operation code");
	return opName[index];
}

public static int instrSize(int instruction)
{
	return (int)(instruction & SIZE_MASK) >> SIZE_SHIFT;
}
public static int instrValueTypeIndex(int instruction)
{
	return (int)(instruction & VALUE_TYPE_MASK) >> VALUE_TYPE_SHIFT;
}

public static void printData(int [] data, int top, int index, boolean code) throws MException
{
	int tagIndex = 0;
	for (int i=0; i<top; i++)
	{
		if (code)
		{
			if (i == index) MSJava.printOut.print(">>> " + i);
			else MSJava.printOut.print("    " + i);
			
			if (i == tagIndex)
			{
				MSJava.printOut.print(":    0x").endLine().printHex(data[i]).print("      ").print(getOpName(data[i]));
				tagIndex += instrSize(data[i]) + 1;
			}
			else MSJava.printOut.print(":    " + data[i]).endLine();
		}
		else
		{
			MSJava.printOut.print("    " + i + ":    " + data[i]).endLine();
		}
	}
}
public static int stringToIntsWithSize(String text, int [] code, int top, int maxSize) throws MException
{
	int numChars = text.length();
	int size32 = (numChars/4) + 1;
	MSJava.assertion(size32 <= maxSize, EC_CODE, "text is too long, max 32-bit size: " + maxSize + ", text: " + text);
	int [] intArray;
	intArray = MSJava.bytesToInts( text.getBytes());
	
	code[top++]=numChars;
	for (int i=0; i<size32; i++)
	{
		//{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("        0x").endLine();}.printHex(String.format("%08X",intArray[i]));
		code[top++] = intArray[i];
	}
	return top;
}

public static boolean intStringsWithSizeEquals(int [] a, int aOffset, int [] b, int bOffset)
{
	// first check that sizes match
	if (a[aOffset] == b[bOffset])
	{
		// check that ints match
		int numChars = a[aOffset];
		int size = (numChars / 4) + 1;
		for (int i=1; i<=size; i++)
		{
			if (a[aOffset + i] != b[bOffset + i]) return false;
		}
		return true;
	}
	return false;
}

public static int addTextInstruction (String text, int instructionCode, int [] code, int top) throws MException
{
	// cast character array to int array to copy it

	int numChars = text.length();
	int size32 = (numChars/4) + 1;
	
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Add text: " + size32 + " x 4 bytes, " + numChars + " characters").endLine();};
	
	int instruction = makeInstruction(instructionCode, size32 + 1, MS_TYPE_TEXT);
	code[top++] = instruction;

	return stringToIntsWithSize(text, code, top, size32 + 1);
}
}
