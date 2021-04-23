package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;





public class Parser extends MC {


public static final String letters =  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
public static final String numbers =  "1234567890";
public static final String whitespace =  " \t\n\r";
public static final String linebreak =  "\n\r";
public static final String expressionBreak =  ",;\n";
public static final String blockStart =  "([{";
public static final String blockEnd =  ")]}";
public static final String op =  "+*<>="; // '-' or '/' will be special cases

private static byte space, name, reference, number, minus, decimalNumber, slash, quote, comment, skipLineBreaks;


private static byte [] tmp = new byte[512];
private static byte [] buffer = new byte[512];

private static ByteAutomata baPtr;
private static boolean goBackwards, running, assignment;
private static int index, lastStart, textCount = 0, lineNumber, characterNumber;
private static MNode root;
private static MNode currentBlock;
private static MNode currentExpr;
private static MNode currentToken;
private static TokenTree tokenTree;


public static final String [] nodeTypeName = new String[] { "root","expr","sub-call","struct","block","token" };


private static void next(byte state)
{
	lastStart = index;

	baPtr.next(state);
}

private static void nextCont(byte state)
{
	// continue with same token,
	// so don't reset the start index
	baPtr.next(state);
}

private static void bwd() throws MException
{
	MSJava.assertion(!goBackwards,   "can't go backwards twice");
	goBackwards = true;
}

//private static void copy(byte[] dst, byte[] src, int start, int end)
//{
//	for (int i = start, n = 0; i < end; i++, n++)
//	{
//		dst[n] = src[i];
//	}
//}


private static String getNewName() throws MException
{
	int start = lastStart;
	int length = index - start;
	MSJava.assertion(length < MSJava.globalConfig.maxNameLength, EC_PARSE, "name is too long");
	
	int i = 0;
	for (; i < length; i++)
	{
		tmp[i] = buffer[start++ % 512];
	}
	
	return new String(tmp,0,length);
}


private static void addExpr()
{
	MNode expr = new MNode(lineNumber, characterNumber, currentBlock, NT_EXPR, "<EXPR>");
	currentExpr.next = expr;
	currentExpr = expr;
	currentToken = null;
}


private static void addToken(int tokenType) throws MException
{ 
	if (tokenType == NT_REFERENCE_TOKEN) lastStart++; // skip '#'

	String data = getNewName();

	{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("TOKEN: " + data).endLine();};}};

	if (tokenType == NT_TEXT)
	{
		// add text to the tree if same text is not already there
		if (!(tokenTree.texts.containsKey( data)))
		{
			tokenTree.texts.put( data, textCount++);
		}
	}

	MNode token = new MNode(lineNumber, characterNumber, currentExpr, tokenType, data);

	if (currentToken == null) currentExpr.child = token;
	else currentToken.next = token;
	currentExpr.numChildren++;
	currentToken = token;
	lastStart = index;
}
private static void addOperator(int tokenType, String name)
{ 
	MNode token = new MNode(lineNumber, characterNumber, currentExpr, tokenType, name);
	if (currentToken == null) currentExpr.child = token;
	else currentToken.next = token;
	currentExpr.numChildren++;
	currentToken = token;
	lastStart = index;
}

private static void endBlock(int blockType) throws MException
{
	// check that block-end character is the right one
	MSJava.assertion((currentBlock != null && currentBlock.type == blockType), EC_PARSE, "invalid block end");

	lastStart = -1;
	currentToken = currentBlock;
	currentExpr = currentToken.parent;
	currentBlock = currentExpr.parent;
}

private static void exprBreak() throws MException
{
	// check that comma is used properly
	if (baPtr.currentInput == ',')	
	{
		MSJava.assertion(currentBlock != null &&  			(currentBlock.type == NT_ASSIGNMENT || 			currentBlock.type == NT_SQUARE_BRACKETS || 			currentBlock.type == NT_PARENTHESIS), EC_PARSE, "unexpected comma");
	}
	
	if (currentBlock != null) currentBlock.numChildren ++;
	
	if (assignment)
	{
		if (baPtr.currentInput != ',')
		{
			// end assignment block
			// hack solution to allow assignment args without brackets
			assignment = false;
			endBlock(NT_ASSIGNMENT);	
			addExpr();	
		}
		else
		{
			// allow line breaks after comma in assignment list
			lastStart = -1;
			addExpr();
			next(skipLineBreaks);
		}
	}
	else
	{
		lastStart = -1;
		addExpr();
	}	
}


private static void addBlock(int blockType) throws MException
{

	String blockTypeName = "???";
	
	if (blockType == NT_PARENTHESIS) blockTypeName = "<PARENTHESIS>";
	else if (blockType == NT_SQUARE_BRACKETS) blockTypeName = "<SQUARE_BRACKETS>";
	else if (blockType == NT_ASSIGNMENT)
	{
		assignment = true;
		blockTypeName = "<ASSIGNMENT>";
	}
	else if (blockType == NT_CODE_BLOCK) blockTypeName = "<CODE_BLOCK>";
	else MSJava.assertion(false,   "invalid block type");
	
	{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("add block: " + blockType).endLine();};}};
	
	lastStart = -1;
	MNode block = new MNode(lineNumber, characterNumber, currentExpr, blockType, blockTypeName);
	if (currentToken == null)
	{
		// expression starts with a block, eg. "[1,2]" in "[[1,2],34,56]"
		currentExpr.child = block;
		currentToken = block;
	}
	else
	{
		currentToken.next = block;
	}
	currentExpr.numChildren++;
	
	currentBlock = block;

	MNode expr = new MNode(lineNumber, characterNumber, currentBlock, NT_EXPR, "<EXPR>");
	currentBlock.child = expr;
	currentExpr = expr;
	currentToken = null;
}


private static void parseError(String msg)
{
	MSJava.printOut.print("Parse error: ");
	MSJava.printOut.print(msg).endLine();
	baPtr.ok = false;
	running = false;
}

private static void defineTransitions() throws MException
{
	ByteAutomata ba = baPtr;

	space = ba.addState("space");
	name = ba.addState("name");
	//member = ba.addState("member");
	reference = ba.addState("reference");
	number = ba.addState("number");
	minus = ba.addState("minus");
	decimalNumber = ba.addState("decimal number");
	//expNumber = ba.addState("exp. number");
	slash = ba.addState("slash");
	quote = ba.addState("quote");
	comment = ba.addState("comment");
	skipLineBreaks = ba.addState("skip line breaks");

	ba.transition(space, whitespace, null);
	ba.transition(space, letters, () -> { next(name); });
	ba.transition(space, "#", () -> { next(reference); });
	ba.transition(space, "-", () -> { next(minus); });
	ba.transition(space, "/", () -> { next(slash); });
	ba.transition(space, "\"", () -> { next(quote); });
	ba.transition(space, numbers, () -> { next(number); });
	ba.transition(space, expressionBreak, () -> { exprBreak(); });
	ba.transition(space, "(", () -> { addBlock(NT_PARENTHESIS); next(skipLineBreaks); });
	ba.transition(space, ")", () -> { endBlock(NT_PARENTHESIS); });
	ba.transition(space, "[", () -> { addBlock(NT_SQUARE_BRACKETS); next(skipLineBreaks); });
	ba.transition(space, "]", () -> { endBlock(NT_SQUARE_BRACKETS); });
	ba.transition(space, "{", () -> { addBlock(NT_CODE_BLOCK); next(skipLineBreaks); });
	ba.transition(space, "}", () -> { endBlock(NT_CODE_BLOCK); });
	ba.transition(space, ":", () -> { addBlock(NT_ASSIGNMENT); next(skipLineBreaks); });
	ba.transition(space, ".", () -> { addOperator(NT_DOT,"."); });

	ba.transition(name, letters, null);
	ba.transition(name, whitespace, () -> { addToken(NT_NAME_TOKEN); next(space); });
	ba.transition(name, "#", () -> { addToken(NT_REF_TYPE_TOKEN); next(space); });
	ba.transition(name, expressionBreak, () -> { addToken(NT_NAME_TOKEN); exprBreak(); next(space); });
	ba.transition(name, blockStart, () -> { addToken(NT_NAME_TOKEN); bwd(); next(space); });
	ba.transition(name, blockEnd, () -> { addToken(NT_NAME_TOKEN); bwd(); next(space); });
	ba.transition(name, ":", () -> { addToken(NT_NAME_TOKEN); addBlock(NT_ASSIGNMENT); next(skipLineBreaks); });
	ba.transition(name, ".", () -> { addToken(NT_NAME_TOKEN); addOperator(NT_DOT,"."); next(space); });
	
	//ba.fillTransition(member, () -> { addMember(); bwd(); next(space); });
	//ba.transition(member, letters, null);
	//ba.transition(member, ".", () -> { addMember(); next(member); });

	ba.transition(reference, letters, null);
	ba.transition(reference, whitespace, () -> { addToken(NT_REFERENCE_TOKEN); next(space); });
	ba.transition(reference, expressionBreak, () -> { addToken(NT_REFERENCE_TOKEN); exprBreak(); next(space); });
	ba.transition(reference, blockStart, () -> { addToken(NT_REFERENCE_TOKEN); bwd(); next(space); });
	ba.transition(reference, blockEnd,   () -> { addToken(NT_REFERENCE_TOKEN); bwd(); next(space); });

	ba.transition(number, numbers, null);
	ba.transition(number, whitespace, () -> { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(number, expressionBreak, () -> { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(number, blockStart, () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(number, blockEnd,   () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(number, ".", () -> { nextCont(decimalNumber); });

	ba.transition(minus, whitespace, () -> { addToken(NT_MINUS); next(space); });
	ba.transition(minus, numbers, () -> { nextCont(number); }); // change state without reseting starting index
	ba.transition(minus, ".", () -> { nextCont(decimalNumber); });

 	ba.transition(decimalNumber, numbers, null);
	ba.transition(decimalNumber, whitespace, () -> { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(decimalNumber, expressionBreak, () -> { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(decimalNumber, blockStart, () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(decimalNumber, blockEnd,   () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	
 /* decimalNumber = ba.addState("decimal number");

	expNumber = ba.addState("exp. number");*/
	
	ba.fillTransition(slash, () -> { addToken(NT_DIV); bwd(); next(space); });
	ba.transition(slash, "/", () -> { next(comment); });

	ba.fillTransition(quote, null);
	ba.transition(quote, linebreak, () -> { parseError("line break inside a quotation"); });
	ba.transition(quote, "\"", () -> { lastStart++; addToken(NT_TEXT); next(space); });
	
	ba.fillTransition(comment, null);
	ba.transition(comment, linebreak, () -> { exprBreak(); next(space); });
	
	ba.fillTransition(skipLineBreaks, () -> { bwd(); next(space); });
	ba.transition(skipLineBreaks, whitespace, null);
	ba.transition(skipLineBreaks, linebreak, null);
}


public static TokenTree  Parse (MSInputStream input) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("------------------------ START PARSING").endLine();};

	tokenTree = new TokenTree();

	baPtr = new ByteAutomata();
	ByteAutomata ba = baPtr;
	
	defineTransitions();

	ba.next((byte)1);

	root = new MNode(1, 1, null, NT_EXPR, "<ROOT>");
	currentExpr = root;
	currentBlock = null;
	currentToken = null;
	
	textCount = 0;
	lastStart = 0;
	running = true;
	assignment = false;
	goBackwards = false;
	
	// text ID 0: empty
	tokenTree.texts.put( "", textCount++);
	
	lineNumber = 1;
	characterNumber = 1;
	byte inputByte = 0;
	index = 0;

	while ((!input.end() || goBackwards) && running && ba.ok)
	{
		if (!goBackwards)
		{
			index ++;
			inputByte = input.readByte();
			buffer[index % 512] = inputByte;
			if (inputByte=='\n')
			{
				lineNumber ++;
				characterNumber = 1;
			}
			else
			{
				characterNumber ++;
			}
		}
		else
		{
			goBackwards = false;
		}
		{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("[ " + (char)(inputByte) + " ]").endLine();};}};

		running = ba.step(inputByte);
	}

	if (!goBackwards) index++;
	ba.step((byte)'\n'); // ended cleanly: last command break
	if (currentBlock != null)
	{
		MSJava.printOut.print("closing parenthesis missing at the end").endLine();
		ba.ok = false;
	}
	
	if (!running || !(ba.ok))
	{
		MSJava.printOut.print("Parser state [" + ba.stateNames.get( (int)ba.currentState) + "]").endLine();
		MSJava.printOut.print("Line " + lineNumber + ": \"").endLine();
		
		// print nearby code
		int start = index-1;
		while (start > 0 && index - start < 512 && (char)buffer[start % 512] != '\n')
			start --;
		while (++start < index)
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print((char)(buffer[start % 512])).endLine();};
		}
		MSJava.printOut.print("\"").endLine();
		
		baPtr = null;
		root = null;
		tokenTree = null;
		throw new MException(MC.EC_INTERNAL, "Parse error");
	}

	if (MSJava.globalConfig.verboseOn())
	{
		MSJava.printOut.print("------------------------ TOKEN TREE:").endLine();
		root.printTree(true);
		MSJava.printOut.print("------------------------ END PARSING").endLine();
	}	
	baPtr = null;
	
	tokenTree.root = root;
	return tokenTree;
}


}
