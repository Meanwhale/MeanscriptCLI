package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;





public class ByteAutomata extends MC {
 boolean ok;
 byte[] tr;
 byte currentInput;
 byte currentState;
 java.util.TreeMap<Integer, String> stateNames = new java.util.TreeMap<Integer, String>();
 MJAction actions[]=new MJAction[64];
 byte stateCounter;
 byte actionCounter; // 0 = end

// running:
 byte inputByte = 0;
 int  index = 0;
 int  lineNumber = 0;
 boolean stayNextStep = false;
 boolean running = false;

 byte[] buffer;
 byte[] tmp;


public static final int MAX_STATES = 32;
public static final int BUFFER_SIZE = 512;

public ByteAutomata()
{
	ok = true;
	currentInput = 0;
	currentState = 0;
	stateCounter = 0;
	actionCounter = 0;
	tr = new  byte[ MAX_STATES * 256];
	for (int i=0; i<MAX_STATES * 256; i++) tr[i] = (byte)0xff;

	inputByte = 0;
	index = 0;
	lineNumber = 0;	
	stayNextStep = false;
	running = false;	
	buffer = new  byte[ BUFFER_SIZE];
	tmp = new  byte[ BUFFER_SIZE];
}

//

public void print ()
{
	for (int i = 0; i <= stateCounter; i++)
	{
		MSJava.printOut.print("state: " + i).endLine();

		for (int n = 0; n < 256; n++)
		{
			byte foo = tr[(i * 256) + n];
			if (foo == 0xff) MSJava.printOut.print(".");
			else MSJava.printOut.print(foo);
		}
		MSJava.printOut.print("").endLine();
	}
}

public byte  addState (String stateName)
{
	stateCounter++;
	stateNames.put( (int)stateCounter, stateName);
	return stateCounter;
}

public void transition (byte state, String input, MJAction action)
{
	byte actionIndex = 0;
	if (action != null)
	{
		actionIndex = addAction(action);
	}

	byte[] bytes = input.getBytes();

	int i = 0;
	while (i<input.length())
	{
		tr[(state * 256) + bytes[i]] = actionIndex;
		i++;
	}
	//{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("New Transition added: id " + actionIndex).endLine();};}};
}

public void fillTransition (byte state, MJAction action)
{
	byte actionIndex = 0;
	if (action != null) actionIndex = addAction(action);

	for (int i=0; i<256; i++)
	{
		tr[(state * 256) + i] = actionIndex;
	}
	//{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("New Transition filled: id " + actionIndex).endLine();};}};
}

public byte  addAction (MJAction action)
{
	actionCounter++;
	actions[actionCounter] = action;
	return actionCounter;
}


public void next (byte nextState)
{
	currentState = nextState;

	{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Next state: " + stateNames.get( (int)currentState)).endLine();};}};
}


public boolean step (byte input) throws MException
{
	currentInput = input;
	int index = (currentState * 256) + input;
	byte actionIndex = tr[index];

	if (actionIndex == 0) return true; // stay on same state and do nothing else
	if (actionIndex == 0xff||actionIndex < 0)
	{
		MSJava.assertion(false, E_UNEXPECTED_CHAR, "unexpected char: '" + (input) + "'" + " #" + ((int)input));
		ok = false;
		return false; // end
	}

	MJAction act = actions[actionIndex];

	if (act == null)
	{
		MSJava.assertion(false,   "invalid action index");
	}
	act.action();
	return true;
}

public int getIndex ()
{
	return index;
}

public byte getInputByte ()
{
	return inputByte;
}

public void stay () throws MException
{
	// same input byte on next step
	MSJava.assertion(!stayNextStep,   "'stay' is called twice");
	stayNextStep = true;
}

public String getString (int start, int length) throws MException
{
	MSJava.assertion(length < MSJava.globalConfig.maxNameLength, null, "name is too long");
	
	int i = 0;
	for (; i < length; i++)
	{
		tmp[i] = buffer[start++ % BUFFER_SIZE];
	}

	return new String(tmp,0,length);
}

public void run (MSInputStream input) throws MException
{
	inputByte = 0;
	index = 0;
	lineNumber = 1;
	stayNextStep = false;
	running = true;

	while ((!input.end() || stayNextStep) && running && ok)
	{
		if (!stayNextStep)
		{
			index ++;
			inputByte = input.readByte();
			buffer[index % BUFFER_SIZE] = inputByte;
			if (inputByte == '\n') lineNumber++;
		}
		else
		{
			stayNextStep = false;
		}
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("[ " + (char)(inputByte) + " ]").endLine();};

		running = step(inputByte);
	}
	
	if (!stayNextStep) index++;
}

public void printError ()
{
	MSJava.printOut.print("ERROR: parser state [" + stateNames.get( (int)currentState) + "]").endLine();
	MSJava.printOut.print("Line " + lineNumber + ": \"").endLine();
	
	// print nearby code
	int start = index-1;
	while (start > 0 && index - start < BUFFER_SIZE && (char)buffer[start % BUFFER_SIZE] != '\n')
	{
		start --;
	}
	while (++start < index)
	{
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print((char)(buffer[start % BUFFER_SIZE])).endLine();};
	}
	MSJava.printOut.print("\"").endLine();
}

}
