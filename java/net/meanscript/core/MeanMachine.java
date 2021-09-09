package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class MeanMachine extends MC {
 int stackTop;
 int ipStackTop;
 int baseStackTop;
 int stackBase;
 int instructionPointer;
 int globalsSize;
 int numTexts;
 int registerType;
 int byteCodeType;
 boolean initialized;
 boolean done;
 boolean jumped;
public int [] stack;
 int [] ipStack;
 int [] baseStack;
 int [] functions;
 int [] registerData;
public int [] texts;
public int [] types; // typeID -> STRUCT_DEF tag address
public ByteCode byteCode;
public MSData globals;
 int globalsTagAddress;



private static int currentStructID = 0;
private static int textCounter = 0;

public MeanMachine (ByteCode bc) throws MException
{
	byteCode = bc;
	
	// INT_ARRAY_RESETtexts;
	{types = new int[ MAX_TYPES]; };
	registerType = -1;
	byteCodeType = -1;
	ipStackTop = 0;
	baseStackTop = 0;
	stackBase = 0;
	globalsSize = -1;
	instructionPointer = 0; // instruction pointer
	globalsTagAddress = -1;
	initialized = false;
	done = false;
	jumped = false;
	globals = null;
	
	init();
}
//

public boolean isInitialized ()
{
	return initialized;
}

public boolean isDone ()
{
	return done;
}

public void initVMArrays () throws MException
{	
	// initialize arrays for code execution only if needed.

	MSJava.assertion(!initialized,   "VM arrays must be initialized before code initialization is finished.");
	
	{stack = new int[			MSJava.globalConfig.stackSize]; };
	{ipStack = new int[		MSJava.globalConfig.ipStackSize]; };
	{baseStack = new int[		MSJava.globalConfig.baseStackSize]; };
	{functions = new int[		MSJava.globalConfig.maxFunctions]; };
	{registerData = new int[	MSJava.globalConfig.registerSize]; };
}

public int [] getStructCode ()
{
	return byteCode.code;
}

public int [] getDataCode () throws MException
{
	if (byteCodeType == BYTECODE_EXECUTABLE) return stack;
	else if (byteCodeType == BYTECODE_READ_ONLY) return byteCode.code;
	else MSJava.assertion(false,  "wrong bytecode type");
	return null;
}

public void gosub (int address) throws MException
{
	int instruction = byteCode.code[address];
	MSJava.assertion((instruction & OPERATION_MASK) == OP_NOOP,   "gosub: wrong address");
	pushIP(-instructionPointer); // gosub addresses (like when executing if's body) negative
	instructionPointer = address;
	jumped = true;
}

public void pushIP (int ip) throws MException
{
	MSJava.assertion(ipStackTop < MSJava.globalConfig.ipStackSize - 1,   "call stack overflow");
	ipStack[ipStackTop] = ip;
	ipStackTop++;
}

public int  popIP () throws MException
{
	MSJava.assertion(ipStackTop > 0,   "pop empty call stack");
	ipStackTop--;
	int rv = ipStack[ipStackTop];
	return rv < 0 ? -rv : rv; // return positive (gosub address is negative)
}

public int  popEndIP () throws MException
{
	MSJava.assertion(ipStackTop > 0,   "pop empty call stack");
	int rv=0;
	do {
		ipStackTop--;
		rv = ipStack[ipStackTop];
	} while (rv < 0);
	return rv;
}

public boolean running ()
{
	return instructionPointer < byteCode.codeTop && !done;
}

public void stepToFunction (int id) throws MException
{
	initFunctionCall(id);
}

public void initFunctionCall (int id) throws MException
{
	MSJava.assertion(initialized, EC_CODE, "Code not initialized.");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Initialize a user's function call").endLine();};
	ByteCode bc = byteCode;
	int tagAddress = functions[id];
	instructionPointer = bc.code[tagAddress + 2];
	stackBase = globalsSize;
	stackTop = globalsSize;
	done = false;
}

public void callFunction (int id) throws MException
{
	MSJava.assertion(byteCodeType == BYTECODE_EXECUTABLE,   "bytecode is not executable");
	initFunctionCall(id);
	while (running())
	{
		step();
	}
}


public void init () throws MException
{
	MSJava.assertion(!initialized, EC_CODE, "Code is already initialized.");
	MSJava.assertion(byteCode.codeTop != 0, EC_CODE, "ByteCode is empty...");
	
	stackBase = 0;
	stackTop = 0;
	currentStructID = -1;
	textCounter = 1; // 0 is empty
	done = false;
	
	if (MSJava.globalConfig.verboseOn())
	{
		MSJava.printOut.print(HORIZONTAL_LINE).endLine();
		MSJava.printOut.print("START INITIALIZING").endLine();
		MSJava.printOut.print(HORIZONTAL_LINE).endLine();
		printBytecode(byteCode.code, byteCode.codeTop, -1, true);
	}

	while (running())
	{
		initStep();
	}
	MSJava.assertion(numTexts == textCounter,   "text count mismatch");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("INITIALIZING FINISHED").endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	
	if (byteCodeType == BYTECODE_EXECUTABLE)
	{
		MSJava.assertion(globalsSize == -1,   "EXEC. globals init error");
		int mainFunctionAddress = functions[0];
		globalsSize = byteCode.code[mainFunctionAddress + 3];
	}
	else
	{
		MSJava.assertion(byteCodeType == BYTECODE_READ_ONLY && globalsSize >= 0,   "READ-ONLY globals init error");
	}
	
	initialized = true;
}


public void initStep () throws MException
{
	ByteCode bc = byteCode;
	
	// read instruction

	int instruction = bc.code[instructionPointer];
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("INIT [" + getOpName(instruction) + "]").endLine();};
	//jumped = false;

	int op = (int)(instruction & OPERATION_MASK);

	if (instructionPointer == 0)
	{
		MSJava.assertion(op == OP_START_INIT, EC_CODE, "bytecode starting tag missing");
		byteCodeType = (int)(instruction & AUX_DATA_MASK);
		MSJava.assertion( 			byteCodeType == BYTECODE_READ_ONLY || 			byteCodeType == BYTECODE_EXECUTABLE, EC_CODE, "unknown bytecode type"); 
		numTexts = bc.code[instructionPointer + 1];
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("start init! " + numTexts + " texts").endLine();};
		{texts = new int[numTexts+1]; };
		
		if (byteCodeType == BYTECODE_EXECUTABLE) initVMArrays();
	}
	else if (op == OP_ADD_TEXT)
	{
		// save address to this tag so the text can be found by its ID
		texts[textCounter++] = instructionPointer;
	}
	else if (op == OP_FUNCTION)
	{
		// FORMAT: | OP_FUNCTION | type | code address |
		int id = bc.code[instructionPointer + 1];
		functions[id] = instructionPointer; // save address to this tag 
	}
	else if (op == OP_MEMBER_NAME) { }
	else if (op == OP_CHARS_DEF)
	{
		currentStructID = (int)(instruction & VALUE_TYPE_MASK);
		types[currentStructID] = instructionPointer;
	}
	else if (op == OP_STRUCT_DEF)
	{
		currentStructID = (int)(instruction & VALUE_TYPE_MASK);
		types[currentStructID] = instructionPointer;
		if (currentStructID == 0)
		{
			MSJava.assertion(globals == null,   "EXEC. globals already initialized");
			globalsTagAddress = instructionPointer;
			if (byteCodeType == BYTECODE_EXECUTABLE)
			{
				globals = new MSData(this, globalsTagAddress, 0, false);
			}
			else
			{
				MSJava.assertion(byteCodeType == BYTECODE_READ_ONLY,   "unknown byteCodeType");
			}
		}
	}
	else if (op == OP_STRUCT_MEMBER) { }
	else if (op == OP_ARRAY_MEMBER) { }
	else if (op == 	OP_INIT_GLOBALS)
	{
		int size = instrSize(instruction);
		
		if (byteCodeType == BYTECODE_EXECUTABLE)
		{
			// if read-only then the data is in bytecode, not stack
			stackBase = size;
			stackTop = size;
			for (int i=1; i<=size; i++)
			{
				stack[i-1] = bc.code[instructionPointer + i];
			}
		}
		else
		{
			MSJava.assertion(globals == null,   "READ-ONLY globals already initialized");
			// for read-only data array is the bytecode array and start index is next to OP_INIT_GLOBALS tag.
			globals = new MSData(this, globalsTagAddress, instructionPointer + 1, false);
			globalsSize = size;
		}
	}
	else if (op == OP_END_INIT)
	{
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("INIT DONE!").endLine();};
		done = true;
	}
	else
	{
		MSJava.assertion(false, EC_CODE, "unknown op. code");
	}
	instructionPointer += 1 + instrSize(instruction);
}


public void step () throws MException
{
	ByteCode bc = byteCode;
	
	// read instruction

	int instruction = bc.code[instructionPointer];
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("EXECUTE [" + getOpName(instruction) + "]").endLine();};
	jumped = false;

	int op = (int)(instruction & OPERATION_MASK);

	if (op == OP_PUSH_IMMEDIATE)
	{
		int size = instrSize(instruction);
		// push words after the instruction to stack 
		for (int i=1; i<=size; i++)
		{
			push(bc.code[instructionPointer + i]);
		}
	}
	else if (op == OP_PUSH_GLOBAL)
	{
		int address = bc.code[instructionPointer + 1];
		int size    = bc.code[instructionPointer + 2];
		
		pushData(stack, address, size);
	}
	else if (op == OP_PUSH_LOCAL)
	{
		int address = bc.code[instructionPointer + 1];
		int size    = bc.code[instructionPointer + 2];
		
		pushData(stack, stackBase + address, size);
	}
	else if (op == OP_PUSH_GLOBAL_REF)
	{
		int refAddress = bc.code[instructionPointer + 1];
		int address = stack[refAddress];
		int size    = bc.code[instructionPointer + 2];
		
		pushData(stack, address, size);
	}
	else if (op == OP_PUSH_LOCAL_REF)
	{
		int refAddress = bc.code[instructionPointer + 1];
		int address = stack[stackBase + refAddress];
		int size    = bc.code[instructionPointer + 2];
		
		pushData(stack, address, size);
	}
	else if (op == OP_PUSH_CHARS)
	{
		int textID = bc.code[instructionPointer + 1];
		int maxChars = bc.code[instructionPointer + 2];
		int structSize = bc.code[instructionPointer + 3];
		int textDataSize = 0;
		if (textID != 0)
		{
			int textIndex = texts[textID];
			int textChars = bc.code[textIndex + 1];
			textDataSize = instrSize(bc.code[textIndex]);
			MSJava.assertion(textChars <= maxChars, EC_CODE, "text too long");
			MSJava.assertion(textDataSize <= structSize, EC_CODE, "text data too long");
			//{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("SIZE: " + textChars + " MAX: " + maxChars).endLine();};
			
			pushData(bc.code, textIndex + 1, textDataSize);
		}
		// fill the rest
		for (int i=0; i < (structSize - textDataSize); i++) push(0);
	}
	else if (op == OP_POP_STACK_TO_GLOBAL)
	{
		// write from stack to global target
		
		int size = bc.code[instructionPointer + 1];
		int address = bc.code[instructionPointer + 2];
		popStackToTarget(bc, stack, size, address);
	}
	else if (op == OP_POP_STACK_TO_LOCAL)
	{
		// write from stack to local target
		
		int size = bc.code[instructionPointer + 1];
		int address = bc.code[instructionPointer + 2];
		popStackToTarget(bc, stack, size, address + stackBase);
	}
	else if (op == OP_POP_STACK_TO_GLOBAL_REF)
	{
		// write from stack to global reference target
		
		int size = bc.code[instructionPointer + 1];
		int refAddress = bc.code[instructionPointer + 2];
		int address = stack[refAddress];
		popStackToTarget(bc, stack, size, address);
	}
	else if (op == OP_POP_STACK_TO_LOCAL_REF)
	{
		// write from stack to local reference target
		
		int size = bc.code[instructionPointer + 1];
		int refAddress = bc.code[instructionPointer + 2];
		int address = stack[stackBase + refAddress];
		popStackToTarget(bc, stack, size, address);
	}
	else if (op == OP_POP_STACK_TO_REG)
	{
		// when 'return' is called
		int size = bc.code[instructionPointer + 1];
		popStackToTarget(bc, registerData, size, 0);
		registerType = (int)(instruction & VALUE_TYPE_MASK);
	}
	else if (op == OP_MULTIPLY_GLOBAL_ARRAY_INDEX)
	{
		// array index is pushed to the stack before
		// NOTE: works only for global arrays (check at Generator)
				
		int indexAddress		= bc.code[instructionPointer + 1];
		int arrayItemSize		= bc.code[instructionPointer + 2];
		int arrayDataAddress	= bc.code[instructionPointer + 3];
		int arrayItemCount		= bc.code[instructionPointer + 4];
				
		// get the index from stack top
		stackTop--;
		int arrayIndex = stack[stackTop];
		if (arrayIndex < 0 || arrayIndex >= arrayItemCount)
		{
			MSJava.errorOut.print("ERROR: index " + arrayIndex + ", size" + arrayItemCount);
			MSJava.assertion(false, EC_SCRIPT, "index out of bounds");
		}
		if (arrayDataAddress < 0)
		{
			// add address as this is not the first variable index of the chain,
			// e.g. "team[foo].position[bar]"
			stack[indexAddress] += (arrayIndex * arrayItemSize);
		}
		else
		{
			// save address to local variable for later use
			stack[indexAddress] = arrayDataAddress + (arrayIndex * arrayItemSize);
		}
	}
	else if (op == OP_CALLBACK_CALL)
	{
		//public MeanMachine (ByteCode _byteCode, StructDef _structDef, int _base)
		int callbackIndex = bc.code[instructionPointer + 1];
		MCallback cb = bc.common.callbacks[callbackIndex];
		MSJava.assertion(cb != null,   "invalid callback");
		int argsSize = cb.argStruct.structSize;
		
		MArgs args = new MArgs(bc, cb.argStruct, stackTop - argsSize);
		
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- callback " + callbackIndex).endLine();};
		cb.func.action( this, args);
		
		args = null;
		
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Clear stack after call").endLine();};
		// clear stack after callback is done
		stackTop -= cb.argStruct.structSize;
	}
	else if (op == OP_FUNCTION_CALL)
	{
		int functionID = bc.code[instructionPointer + 1];
		int tagAddress = functions[functionID];
		
		pushIP(instructionPointer); // save old IP
		instructionPointer = bc.code[tagAddress + 2];
		
		// args are already in stack. make room for locals
		int functionContextStructSize = bc.code[tagAddress + 3];
		int argsSize = bc.code[tagAddress + 4];
		int delta = functionContextStructSize - argsSize;
		for (int i=0; i<delta; i++) stack[stackTop+i] = -1;
		
		stackTop += delta;
		stackBase = stackTop - functionContextStructSize;
		
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- function call! ID " + functionID + ", jump to " + instructionPointer).endLine();};
		jumped = true;
		
	}
	else if (op == OP_SAVE_BASE)
	{
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- OP_PUSH_STACK_BASE").endLine();};
		baseStack[baseStackTop++] = stackBase;
	}
	else if (op == OP_LOAD_BASE)
	{
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- OP_POP_STACK_BASE").endLine();};
		baseStackTop--;
		stackTop = stackBase;
		stackBase = baseStack[baseStackTop];
	}
	else if(op == OP_PUSH_REG_TO_STACK)
	{
		MSJava.assertion(registerType != -1,   "register empty");
		int size = bc.code[instructionPointer + 1];
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("push register content to stack, size " + size).endLine();};
		for (int i=0; i<size; i++) push(registerData[i]);
		registerType = -1;
	}
	else if (op == OP_JUMP)
	{
		int address = bc.code[instructionPointer + 1];
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("jump: " + address).endLine();};
		instructionPointer = address;
		jumped = true;
	}
	else if (op == OP_GO_BACK)
	{
		if (ipStackTop == 0)
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("DONE!").endLine();};
			done = true;
		}
		else
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Return from go-sub").endLine();};
			// get the old IP and make a jump to the next instruction
			instructionPointer = popIP();
			instruction = bc.code[instructionPointer];
			instructionPointer += 1 + instrSize(instruction);
		}
		jumped = true;
	}
	else if (op == OP_GO_END)
	{
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Go to end of function").endLine();};
		instructionPointer = popEndIP();
		instruction = bc.code[instructionPointer];
		instructionPointer += 1 + instrSize(instruction);
		jumped = true;
	}
	else if (op == OP_NOOP)
	{
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(" . . . ").endLine();};
	}
	else
	{
		throw new MException(MC.EC_INTERNAL, "unknown operation code");
	}

	//{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("STACK: base " + stackBase + ", top " + stackTop).endLine();};}};
	//{if (MSJava.debug) {printData(stack, stackTop, stackBase, false);}};

	if (!jumped)
	{
		instructionPointer += 1 + instrSize(instruction);
	}
}


public void pushData (int [] source, int address, int size) throws MException
{
	// push words from the source
	for (int i=0; i<size; i++)
	{
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("push from address " + (address + i)).endLine();};
		push(source[address + i]);
	}
}

public void popStackToTarget (ByteCode bc, int [] target, int size, int address) throws MException
{
	for (int i=0; i<size; i++)
	{
		target[address + i] = stack[stackTop - size + i];
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("write " + stack[stackTop - size + i] + " to address " + (address + i)).endLine();};
	}
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("clean stack").endLine();};
	stackTop -= size;
}

public void push (int data) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("push stack: " + data).endLine();};
	stack[stackTop++] = data;
}

public void callbackReturn (int type, int value) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	     {if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("        return " + value).endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	// return value from a callback
	// parameter for some other function or call
	saveReg(type,value);
}

public void saveReg (int type, int value)
{
	registerType = type;
	registerData[0] = value;
}


public void  writeCode (MSOutputStream output) throws MException
{
	byteCode.writeCode(output);
}

public void  writeReadOnlyData (MSOutputStream output) throws MException
{
	// write struct definitions
	byteCode.writeStructInit(output);
	output.writeInt(makeInstruction(OP_INIT_GLOBALS, globalsSize, 0));
	for (int i=0; i<globalsSize; i++)
	{
		output.writeInt(stack[i]);
	}
	output.writeInt(makeInstruction(OP_END_INIT,0,0));
}

public void printGlobals() throws MException
{
	MSJava.printOut.print("GLOBALS: ").endLine();
	if (globalsSize == 0)
	{
		MSJava.printOut.print("    <none>").endLine();
	}
	else
	{
		for (int i=0; i<globalsSize; i++)
		{
			MSJava.printOut.print("    " + i + ":    " + stack[i]).endLine();
		}
		MSJava.printOut.print("").endLine();
	}
}

public void printDetails() throws MException
{
	MSJava.printOut.print("DETAILS").endLine();
	MSJava.printOut.print(HORIZONTAL_LINE).endLine();
	MSJava.printOut.print("globals size: " + globalsSize).endLine();
	MSJava.printOut.print("stack base:   " + stackBase).endLine();
	MSJava.printOut.print("stack top:    " + stackTop).endLine();
	MSJava.printOut.print("call depth:   " + baseStackTop).endLine();
	MSJava.printOut.print("instruction pointer: " + instructionPointer).endLine();
	
	MSJava.printOut.print("\nSTACK").endLine();
	MSJava.printOut.print(HORIZONTAL_LINE).endLine();
	printBytecode(stack, stackTop, -1, false);
}

public void printCode() throws MException
{
	MSJava.printOut.print("BYTECODE CONTENTS").endLine();
	MSJava.printOut.print(HORIZONTAL_LINE).endLine();
	MSJava.printOut.print("index     code/data (32 bits)").endLine();
	MSJava.printOut.print(HORIZONTAL_LINE).endLine();
	printBytecode(byteCode.code, byteCode.codeTop, instructionPointer, true);
}


public void dataPrint() throws MException
{
	MSJava.printOut.print(HORIZONTAL_LINE).endLine();
	globals.printData(MSJava.printOut, 0, "");
	MSJava.printOut.print(HORIZONTAL_LINE).endLine();
}

}
