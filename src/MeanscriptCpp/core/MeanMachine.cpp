
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;


int32_t currentStructID = 0;
int32_t textCounter = 0;

MeanMachine::MeanMachine (ByteCode* bc) 
{
	byteCode = bc;
	
	// INT_ARRAY_RESETtexts;
	{ types.reset(			MAX_TYPES); types.fill(0); };
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
	globals = 0;
	
	init();
}
MeanMachine::~MeanMachine() { 	{ delete byteCode; byteCode = 0; }; 	{ delete globals; globals = 0; }; }

bool MeanMachine::isInitialized ()
{
	return initialized;
}

bool MeanMachine::isDone ()
{
	return done;
}

void MeanMachine::initVMArrays () 
{	
	// initialize arrays for code execution only if needed.

	ASSERT(!initialized, "VM arrays must be initialized before code initialization is finished.");
	
	{ stack.reset(			globalConfig.stackSize); stack.fill(0); };
	{ ipStack.reset(		globalConfig.ipStackSize); ipStack.fill(0); };
	{ baseStack.reset(		globalConfig.baseStackSize); baseStack.fill(0); };
	{ functions.reset(		globalConfig.maxFunctions); functions.fill(0); };
	{ registerData.reset(	globalConfig.registerSize); registerData.fill(0); };
}

Array<int>* MeanMachine::getStructCode ()
{
	return (&((*byteCode).code));
}

Array<int>* MeanMachine::getDataCode () 
{
	if (byteCodeType == BYTECODE_EXECUTABLE) return (&(stack));
	else if (byteCodeType == BYTECODE_READ_ONLY) return (&((*byteCode).code));
	else ASSERT(false,"wrong bytecode type");
	return 0;
}

void MeanMachine::gosub (int32_t address) 
{
	int32_t instruction = (*byteCode).code[address];
	ASSERT((instruction & OPERATION_MASK) == OP_NOOP, "gosub: wrong address");
	pushIP(-instructionPointer); // gosub addresses (like when executing if's body) negative
	instructionPointer = address;
	jumped = true;
}

void MeanMachine::pushIP (int32_t ip) 
{
	ASSERT(ipStackTop < globalConfig.ipStackSize - 1, "call stack overflow");
	ipStack[ipStackTop] = ip;
	ipStackTop++;
}

int32_t MeanMachine:: popIP () 
{
	ASSERT(ipStackTop > 0, "pop empty call stack");
	ipStackTop--;
	int32_t rv = ipStack[ipStackTop];
	return rv < 0 ? -rv : rv; // return positive (gosub address is negative)
}

int32_t MeanMachine:: popEndIP () 
{
	ASSERT(ipStackTop > 0, "pop empty call stack");
	int32_t rv=0;
	do {
		ipStackTop--;
		rv = ipStack[ipStackTop];
	} while (rv < 0);
	return rv;
}

bool MeanMachine::running ()
{
	return instructionPointer < (*byteCode).codeTop && !done;
}

void MeanMachine::stepToFunction (int32_t id) 
{
	initFunctionCall(id);
}

void MeanMachine::initFunctionCall (int32_t id) 
{
	CHECK(initialized, EC_CODE, "Code not initialized.");
	VERBOSE("Initialize a user's function call");
	ByteCode & bc = (*byteCode);
	int32_t tagAddress = functions[id];
	instructionPointer = bc.code[tagAddress + 2];
	stackBase = globalsSize;
	stackTop = globalsSize;
	done = false;
}

void MeanMachine::callFunction (int32_t id) 
{
	ASSERT(byteCodeType == BYTECODE_EXECUTABLE, "bytecode is not executable");
	initFunctionCall(id);
	while (running())
	{
		step();
	}
}


void MeanMachine::init () 
{
	CHECK(!initialized, EC_CODE, "Code is already initialized.");
	CHECK((*byteCode).codeTop != 0, EC_CODE, "ByteCode is empty...");
	
	stackBase = 0;
	stackTop = 0;
	currentStructID = -1;
	textCounter = 1; // 0 is empty
	done = false;
	
	VERBOSE(HORIZONTAL_LINE);
	VERBOSE("START INITIALIZING");
	VERBOSE(HORIZONTAL_LINE);
	if (globalConfig.verboseOn()) printData((*byteCode).code, (*byteCode).codeTop, -1, true);

	while (running())
	{
		initStep();
	}
	ASSERT(numTexts == textCounter, "text count mismatch");
	VERBOSE(HORIZONTAL_LINE);
	VERBOSE("INITIALIZING FINISHED");
	VERBOSE(HORIZONTAL_LINE);
	
	if (byteCodeType == BYTECODE_EXECUTABLE)
	{
		ASSERT(globalsSize == -1, "EXEC. globals init error");
		int32_t mainFunctionAddress = functions[0];
		globalsSize = (*byteCode).code[mainFunctionAddress + 3];
	}
	else
	{
		ASSERT(byteCodeType == BYTECODE_READ_ONLY && globalsSize >= 0, "READ-ONLY globals init error");
	}
	
	initialized = true;
}


void MeanMachine::initStep () 
{
	ByteCode & bc = (*byteCode);
	
	// read instruction

	int32_t instruction = bc.code[instructionPointer];
	VERBOSE("INIT [" CAT getOpName(instruction) CAT "]");
	//jumped = false;

	int32_t op = (int32_t)(instruction & OPERATION_MASK);

	if (instructionPointer == 0)
	{
		CHECK(op == OP_START_INIT, EC_CODE, "bytecode starting tag missing");
		byteCodeType = (int32_t)(instruction & AUX_DATA_MASK);
		CHECK(
			byteCodeType == BYTECODE_READ_ONLY ||
			byteCodeType == BYTECODE_EXECUTABLE, EC_CODE, "unknown bytecode type"); 
		numTexts = bc.code[instructionPointer + 1];
		VERBOSE("start init! " CAT numTexts CAT " texts");
		{ texts.reset(numTexts+1); texts.fill(0); };
		
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
		int32_t id = bc.code[instructionPointer + 1];
		functions[id] = instructionPointer; // save address to this tag 
	}
	else if (op == OP_MEMBER_NAME) { }
	else if (op == OP_STRUCT_DEF)
	{
		currentStructID = (int32_t)(instruction & VALUE_TYPE_MASK);
		types[currentStructID] = instructionPointer;
		if (currentStructID == 0)
		{
			ASSERT(globals == 0, "EXEC. globals already initialized");
			globalsTagAddress = instructionPointer;
			if (byteCodeType == BYTECODE_EXECUTABLE)
			{
				globals = new MSData(this, globalsTagAddress, 0, false);
			}
			else
			{
				ASSERT(byteCodeType == BYTECODE_READ_ONLY, "unknown byteCodeType");
			}
		}
	}
	else if (op == OP_STRUCT_MEMBER) { }
	else if (op == OP_ARRAY_MEMBER) { }
	else if (op == 	OP_INIT_GLOBALS)
	{
		int32_t size = instrSize(instruction);
		
		if (byteCodeType == BYTECODE_EXECUTABLE)
		{
			// if read-only then the data is in bytecode, not stack
			stackBase = size;
			stackTop = size;
			for (int32_t i=1; i<=size; i++)
			{
				stack[i-1] = bc.code[instructionPointer + i];
			}
		}
		else
		{
			ASSERT(globals == 0, "READ-ONLY globals already initialized");
			// for read-only data array is the bytecode array and start index is next to OP_INIT_GLOBALS tag.
			globals = new MSData(this, globalsTagAddress, instructionPointer + 1, false);
			globalsSize = size;
		}
	}
	else if (op == OP_END_INIT)
	{
		VERBOSE("INIT DONE!");
		done = true;
	}
	else
	{
		ERROR("unknown op. code");
	}
	instructionPointer += 1 + instrSize(instruction);
}


void MeanMachine::step () 
{
	ByteCode & bc = (*byteCode);
	
	// read instruction

	int32_t instruction = bc.code[instructionPointer];
	VERBOSE("EXECUTE [" CAT getOpName(instruction) CAT "]");
	jumped = false;

	int32_t op = (int32_t)(instruction & OPERATION_MASK);

	if (op == OP_PUSH_IMMEDIATE)
	{
		int32_t size = instrSize(instruction);
		// push words after the instruction to stack 
		for (int32_t i=1; i<=size; i++)
		{
			push(bc.code[instructionPointer + i]);
		}
	}
	else if (op == OP_PUSH_GLOBAL)
	{
		int32_t address = bc.code[instructionPointer + 1];
		int32_t size    = bc.code[instructionPointer + 2];
		
		pushData(bc, stack, address, size);
	}
	else if (op == OP_PUSH_LOCAL)
	{
		int32_t address = bc.code[instructionPointer + 1];
		int32_t size    = bc.code[instructionPointer + 2];
		
		pushData(bc, stack, stackBase + address, size);
	}
	else if (op == OP_PUSH_GLOBAL_REF)
	{
		int32_t refAddress = bc.code[instructionPointer + 1];
		int32_t address = stack[refAddress];
		int32_t size    = bc.code[instructionPointer + 2];
		
		pushData(bc, stack, address, size);
	}
	else if (op == OP_PUSH_LOCAL_REF)
	{
		int32_t refAddress = bc.code[instructionPointer + 1];
		int32_t address = stack[stackBase + refAddress];
		int32_t size    = bc.code[instructionPointer + 2];
		
		pushData(bc, stack, address, size);
	}
	else if (op == OP_POP_STACK_TO_GLOBAL)
	{
		// write from stack to global target
		
		int32_t size = bc.code[instructionPointer + 1];
		int32_t address = bc.code[instructionPointer + 2];
		popStackToTarget(bc, stack, size, address);
	}
	else if (op == OP_POP_STACK_TO_LOCAL)
	{
		// write from stack to local target
		
		int32_t size = bc.code[instructionPointer + 1];
		int32_t address = bc.code[instructionPointer + 2];
		popStackToTarget(bc, stack, size, address + stackBase);
	}
	else if (op == OP_POP_STACK_TO_GLOBAL_REF)
	{
		// write from stack to global reference target
		
		int32_t size = bc.code[instructionPointer + 1];
		int32_t refAddress = bc.code[instructionPointer + 2];
		int32_t address = stack[refAddress];
		popStackToTarget(bc, stack, size, address);
	}
	else if (op == OP_POP_STACK_TO_LOCAL_REF)
	{
		// write from stack to local reference target
		
		int32_t size = bc.code[instructionPointer + 1];
		int32_t refAddress = bc.code[instructionPointer + 2];
		int32_t address = stack[stackBase + refAddress];
		popStackToTarget(bc, stack, size, address);
	}
	else if (op == OP_POP_STACK_TO_REG)
	{
		// when 'return' is called
		int32_t size = bc.code[instructionPointer + 1];
		popStackToTarget(bc, registerData, size, 0);
		registerType = (int32_t)(instruction & VALUE_TYPE_MASK);
	}
	else if (op == OP_MULTIPLY_GLOBAL_ARRAY_INDEX)
	{
		// array index is pushed to the stack before
		// NOTE: works only for global arrays (check at Generator)
				
		int32_t indexAddress		= bc.code[instructionPointer + 1];
		int32_t arrayItemSize		= bc.code[instructionPointer + 2];
		int32_t arrayDataAddress	= bc.code[instructionPointer + 3];
		int32_t arrayItemCount		= bc.code[instructionPointer + 4];
				
		// get the index from stack top
		stackTop--;
		int32_t arrayIndex = stack[stackTop];
		if (arrayIndex < 0 || arrayIndex >= arrayItemCount)
		{
			PRINT("ERROR: index " CAT arrayIndex CAT ", size" CAT arrayItemCount);
			CHECK(false, EC_SCRIPT, "index out of bounds");
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
		//MeanMachine::MeanMachine (ByteCode* _byteCode, StructDef* _structDef, int32_t _base)
		int32_t callbackIndex = bc.code[instructionPointer + 1];
		const MCallback & cb = (*(*bc.common).callbacks[callbackIndex]);
		ASSERT((&(cb)) != 0, "invalid callback");
		int32_t argsSize = (*cb.argStruct).structSize;
		
		MArgs* args = new MArgs((&(bc)), cb.argStruct, stackTop - argsSize);
		
		VERBOSE("-------- callback " CAT callbackIndex);
		cb.func( (*this), (*args));
		
		{ delete args; args = 0; };
		
		VERBOSE("Clear stack after call");
		// clear stack after callback is done
		stackTop -= (*cb.argStruct).structSize;
	}
	else if (op == OP_FUNCTION_CALL)
	{
		int32_t functionID = bc.code[instructionPointer + 1];
		int32_t tagAddress = functions[functionID];
		
		pushIP(instructionPointer); // save old IP
		instructionPointer = bc.code[tagAddress + 2];
		
		// args are already in stack. make room for locals
		int32_t functionContextStructSize = bc.code[tagAddress + 3];
		int32_t argsSize = bc.code[tagAddress + 4];
		int32_t delta = functionContextStructSize - argsSize;
		for (int32_t i=0; i<delta; i++) stack[stackTop+i] = -1;
		
		stackTop += delta;
		stackBase = stackTop - functionContextStructSize;
		
		VERBOSE("-------- function call! ID " CAT functionID CAT ", jump to " CAT instructionPointer);
		jumped = true;
		
	}
	else if (op == OP_SAVE_BASE)
	{
		VERBOSE("-------- OP_PUSH_STACK_BASE");
		baseStack[baseStackTop++] = stackBase;
	}
	else if (op == OP_LOAD_BASE)
	{
		VERBOSE("-------- OP_POP_STACK_BASE");
		baseStackTop--;
		stackTop = stackBase;
		stackBase = baseStack[baseStackTop];
	}
	else if(op == OP_PUSH_REG_TO_STACK)
	{
		ASSERT(registerType != -1, "register empty");
		int32_t size = bc.code[instructionPointer + 1];
		VERBOSE("push register content to stack, size " CAT size);
		for (int32_t i=0; i<size; i++) push(registerData[i]);
		registerType = -1;
	}
	else if (op == OP_JUMP)
	{
		int32_t address = bc.code[instructionPointer + 1];
		VERBOSE("jump: " CAT address);
		instructionPointer = address;
		jumped = true;
	}
	else if (op == OP_GO_BACK)
	{
		if (ipStackTop == 0)
		{
			VERBOSE("DONE!");
			done = true;
		}
		else
		{
			VERBOSE("Return from go-sub");
			// get the old IP and make a jump to the next instruction
			instructionPointer = popIP();
			instruction = bc.code[instructionPointer];
			instructionPointer += 1 + instrSize(instruction);
		}
		jumped = true;
	}
	else if (op == OP_GO_END)
	{
		VERBOSE("Go to end of function");
		instructionPointer = popEndIP();
		instruction = bc.code[instructionPointer];
		instructionPointer += 1 + instrSize(instruction);
		jumped = true;
	}
	else if (op == OP_NOOP)
	{
		VERBOSE(" . . .");
	}
	else
	{
		ERROR("unknown operation code");
	}

	//DEBUG(VERBOSE("STACK: base " CAT stackBase CAT ", top " CAT stackTop));
	//DEBUG(printData(stack, stackTop, stackBase, false));

	if (!jumped)
	{
		instructionPointer += 1 + instrSize(instruction);
	}
}


void MeanMachine::pushData (ByteCode & bc, Array<int> & source, int32_t address, int32_t size)
{
	// push words from the source
	for (int32_t i=0; i<size; i++)
	{
		VERBOSE("push from address " CAT address + i);
		push(source[address + i]);
	}
}

void MeanMachine::popStackToTarget (ByteCode & bc, Array<int> & target, int32_t size, int32_t address)
{
	for (int32_t i=0; i<size; i++)
	{
		target[address + i] = stack[stackTop - size + i];
		VERBOSE("write " CAT stack[stackTop - size + i] CAT " to address " CAT address + i);
	}
	VERBOSE("clean stack");
	stackTop -= size;
}

void MeanMachine::push (int32_t data)
{
	VERBOSE("push stack: " CAT data);
	stack[stackTop++] = data;
}

void MeanMachine::callbackReturn (int32_t type, int32_t value)
{
	VERBOSE(HORIZONTAL_LINE);
	     VERBOSE("        return " CAT value);
	VERBOSE(HORIZONTAL_LINE);
	// return value from a callback
	// parameter for some other function or call
	saveReg(type,value);
}

void MeanMachine::saveReg (int32_t type, int32_t value)
{
	registerType = type;
	registerData[0] = value;
}


void MeanMachine:: writeCode (MSOutputStream & output) 
{
	(*byteCode).writeCode(output);
}

void MeanMachine:: writeReadOnlyData (MSOutputStream & output) 
{
	// write struct definitions
	(*byteCode).writeStructInit(output);
	output.writeInt(makeInstruction(OP_INIT_GLOBALS, globalsSize, 0));
	for (int32_t i=0; i<globalsSize; i++)
	{
		output.writeInt(stack[i]);
	}
	output.writeInt(makeInstruction(OP_END_INIT,0,0));
}

void MeanMachine::printGlobals()
{
	PRINT("GLOBALS: ");
	if (globalsSize == 0)
	{
		PRINT("    <none>");
	}
	else
	{
		for (int32_t i=0; i<globalsSize; i++)
		{
			PRINT("    " CAT i CAT ":    " CAT stack[i]);
		}
		PRINT("");
	}
}

void MeanMachine::printDetails() 
{
	PRINT("DETAILS");
	PRINT(HORIZONTAL_LINE);
	PRINT("globals size: " CAT globalsSize);
	PRINT("stack base:   " CAT stackBase);
	PRINT("stack top:    " CAT stackTop);
	PRINT("call depth:   " CAT baseStackTop);
	PRINT("instruction pointer: " CAT instructionPointer);
	
	PRINT("\nSTACK");
	PRINT(HORIZONTAL_LINE);
	printData(stack, stackTop, -1, false);
}

void MeanMachine::printCode() 
{
	PRINT("BYTECODE CONTENTS");
	PRINT(HORIZONTAL_LINE);
	PRINT("index     code/data (32 bits)");
	PRINT(HORIZONTAL_LINE);
	printData((*byteCode).code, (*byteCode).codeTop, instructionPointer, true);
}


void MeanMachine::dataPrint() 
{
	PRINT(HORIZONTAL_LINE);
	(*globals).printData(0, "<globals>");
	PRINT(HORIZONTAL_LINE);
}

} // namespace meanscript(core)
// C++ END
