package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;





public class ByteCode extends MC {
public int codeTop;
public int code [];
 Common common;



public ByteCode (Common _common)
{
	common = _common;
	{code = new int[ MSJava.globalConfig.codeSize]; };
	codeTop = 0;
}

public ByteCode (Common _common, MSInputStream input) throws MException
{
	common = _common;
	int byteCount = input.getByteCount();
	MSJava.assertion(byteCount % 4 == 0,   "bytecode file size not divisible by 4");
	int size = byteCount / 4;
	{code = new int[ size]; };
	input.readArray(code, size);
	codeTop = size;
}

public ByteCode (ByteCode bc) throws MException
{
	codeTop = bc.codeTop;
	common = bc.common;
	
	{code = new int[ codeTop]; };	
	
	// copy array
	
	for (int i=0; i<codeTop; i++)
	{
		code[i] = bc.code[i];
	}
}

//;


public void addInstructionWithData (int operation, int size, int valueType, int data) throws MException
{
	addInstruction(operation,size,valueType);
	addWord(data);
}

public void addInstruction (int operation, int size, int valueType) throws MException
{
	int instruction = makeInstruction(operation, size, valueType);
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Add instruction: [" + getOpName(instruction) + "]").endLine();};
	code[codeTop++] = instruction;
}

public void addWord(int data)
{
	//{if (MSJava.debug) {MSJava.printOut.print("data: " + data).endLine();}};
	code[codeTop++] = data;
}


public void  addTextInstr (String text) throws MException
{
	codeTop = addTextInstruction(text, OP_ADD_TEXT, code, codeTop);
}

public void  writeCode (MSOutputStream output) throws MException
{
	for (int i=0; i < codeTop; i++)
	{
		output.writeInt(code[i]);
	}
}

public void  writeStructInit (MSOutputStream output) throws MException
{
	int i=0;

	// change init tag
	
	int op = (int)((code[i]) & OPERATION_MASK);
	MSJava.assertion(op == OP_START_INIT,   "writeStructInit: bytecode error");
	output.writeInt(makeInstruction(OP_START_INIT, 1, BYTECODE_READ_ONLY));
	i++;
	output.writeInt(code[i]);
	i++;
	
	int tagIndex = i;
	
	// copy necessary tags
	
	for (; i < codeTop; i++)
	{
		if (i == tagIndex)
		{
			tagIndex += instrSize(code[i]) + 1;
			op = (int)((code[i]) & OPERATION_MASK);
			if (op == OP_FUNCTION)
			{
				// skip function inits
				i += instrSize(code[i]);
				continue;
			}
			else if (op == OP_END_INIT)
			{
				return;
			}
		}
		output.writeInt(code[i]);
	}
	MSJava.assertion(false,  "bytecode init end tag not found");
}

}
