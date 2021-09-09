package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class Parser extends MC {


public static final String letters =  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
public static final String numbers =  "1234567890";
public static final String hexNumbers =  "1234567890abcdefABCDEF";
public static final String whitespace =  " \t\n\r";
public static final String linebreak =  "\n\r";
public static final String expressionBreak =  ",;\n";
public static final String blockStart =  "([{";
public static final String blockEnd =  ")]}";
public static final String op =  "+*<>="; // '-' or '/' will be special cases

private static byte space, name, reference, number, hex, minus, decimalNumber, slash, quote, comment, skipLineBreaks, escapeChar, hexByte, zero;


private static byte [] tmp = new byte[4096];
private static byte [] buffer = new byte[512];
private static byte [] quoteBuffer = new byte[4096];

private static ByteAutomata baPtr;
private static boolean goBackwards, running, assignment;
private static int index, lastStart, textCount = 0, lineNumber, characterNumber, quoteIndex, parsedSign, parsedNumber;
private static MNode root;
private static MNode currentBlock;
private static MNode currentExpr;
private static MNode currentToken;
private static TokenTree tokenTree;

public static final String [] nodeTypeName = new String[] { "root","expr","sub-call","struct","block","token" };


private static void next(byte state) throws MException
{
	lastStart = index;

	baPtr.next(state);
}

private static void nextCont(byte state) throws MException
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

private static MSText getNewName() throws MException
{
	int start = lastStart;
	int length = index - start;
	MSJava.assertion(length < MSJava.globalConfig.maxNameLength, EC_PARSE, "name is too long");
	
	int i = 0;
	for (; i < length; i++)
	{
		tmp[i] = buffer[start++ % 512];
	}
	return new MSText (tmp, 0, length);
}

private static MSText getQuoteText() throws MException
{
	int start = 0;
	int length = quoteIndex - start;
	return new MSText (quoteBuffer, start, length);
}


private static void addExpr() throws MException
{
	MNode expr = new MNode(lineNumber, characterNumber, currentBlock, NT_EXPR, new MSText ("<EXPR>"));
	currentExpr.next = expr;
	currentExpr = expr;
	currentToken = null;
}


private static void addToken(int tokenType) throws MException
{ 
	if (tokenType == NT_REFERENCE_TOKEN) lastStart++; // skip '#'
	
	MNode token = null;
	
	if (tokenType == NT_TEXT)
	{
		MSText data = getQuoteText();
		// add text to the tree if same text is not already there
		if (!(tokenTree.texts.containsKey( data)))
		{
			tokenTree.texts.put( new MSText(data), textCount++);
		}
		token = new MNode(lineNumber, characterNumber, currentExpr, NT_TEXT, data);
	}
	else
	{
		MSText data = getNewName();
		{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("TOKEN: " + data).endLine();};}};
		token = new MNode(lineNumber, characterNumber, currentExpr, tokenType, data);
	}
	
	if (currentToken == null) currentExpr.child = token;
	else currentToken.next = token;
	currentExpr.numChildren++;
	currentToken = token;
	lastStart = index;
}
private static void addOperator(int tokenType, MSText name)
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

	MSText blockTypeName = null;
	
	if (blockType == NT_PARENTHESIS) blockTypeName = new MSText("<PARENTHESIS>");
	else if (blockType == NT_SQUARE_BRACKETS) blockTypeName = new MSText("<SQUARE_BRACKETS>");
	else if (blockType == NT_ASSIGNMENT)
	{
		assignment = true;
		blockTypeName = new MSText("<ASSIGNMENT>");
	}
	else if (blockType == NT_CODE_BLOCK) blockTypeName = new MSText("<CODE_BLOCK>");
	else MSJava.assertion(false,   "invalid block type");
	
	{if (MSJava.debug) {{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("add block: " + blockType).endLine();};}};
	
	lastStart = -1;

	MSJava.assertion(blockTypeName != null,   "blockTypeName is null");

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

	MNode expr = new MNode(lineNumber, characterNumber, currentBlock, NT_EXPR, new MSText("<EXPR>"));
	currentBlock.child = expr;
	currentExpr = expr;
	currentToken = null;
}

private static void parseError(String msg) throws MException
{
	MSJava.printOut.print("Parse error: ");
	MSJava.printOut.print(msg).endLine();
	baPtr.ok = false;
	running = false;
}

private static void addQuoteByte(int i) throws MException
{
	if (quoteIndex >= 4096)
	{
		parseError("text is too long");
		return;
	}
	quoteBuffer[quoteIndex++] = (byte)i;
}

private static void addHexByte() throws MException
{
	byte high = buffer[(lastStart + 1) % 512];
	byte low = buffer [(lastStart + 2) % 512];
	byte b = (byte)(((hexCharToByte(high)<<4) & 0xf0) | hexCharToByte(low));
	addQuoteByte(b);
}

private static void defineTransitions() throws MException
{
	ByteAutomata ba = baPtr;

	space = ba.addState("space");
	name = ba.addState("name");
	//member = ba.addState("member");
	reference = ba.addState("reference");
	number = ba.addState("number");
	hex = ba.addState("hex");
	minus = ba.addState("minus");
	decimalNumber = ba.addState("decimal number");
	//expNumber = ba.addState("exp. number");
	slash = ba.addState("slash");
	quote = ba.addState("quote");
	comment = ba.addState("comment");
	skipLineBreaks = ba.addState("skip line breaks");
	escapeChar = ba.addState("escape character");
	hexByte = ba.addState("hex byte");
	zero = ba.addState("zero");

	ba.transition(space, whitespace, null);
	ba.transition(space, letters, () -> { next(name); });
	ba.transition(space, "#", () -> { next(reference); });
	ba.transition(space, "-", () -> { next(minus); });
	ba.transition(space, "/", () -> { next(slash); });
	ba.transition(space, "\"", () -> { next(quote); quoteIndex = 0; });
	ba.transition(space, numbers, () -> { next(number); });
	ba.transition(space, "0", () -> { next(zero); }); // start hex
	ba.transition(space, expressionBreak, () -> { exprBreak(); });
	ba.transition(space, "(", () -> { addBlock(NT_PARENTHESIS); next(skipLineBreaks); });
	ba.transition(space, ")", () -> { endBlock(NT_PARENTHESIS); });
	ba.transition(space, "[", () -> { addBlock(NT_SQUARE_BRACKETS); next(skipLineBreaks); });
	ba.transition(space, "]", () -> { endBlock(NT_SQUARE_BRACKETS); });
	ba.transition(space, "{", () -> { addBlock(NT_CODE_BLOCK); next(skipLineBreaks); });
	ba.transition(space, "}", () -> { endBlock(NT_CODE_BLOCK); });
	ba.transition(space, ":", () -> { addBlock(NT_ASSIGNMENT); next(skipLineBreaks); });
	ba.transition(space, ".", () -> { addOperator(NT_DOT,new MSText(".")); });

	ba.transition(name, letters, null);
	ba.transition(name, numbers, null);
	ba.transition(name, whitespace, () -> { addToken(NT_NAME_TOKEN); next(space); });
	ba.transition(name, "#", () -> { addToken(NT_REF_TYPE_TOKEN); next(space); });
	ba.transition(name, expressionBreak, () -> { addToken(NT_NAME_TOKEN); exprBreak(); next(space); });
	ba.transition(name, blockStart, () -> { addToken(NT_NAME_TOKEN); bwd(); next(space); });
	ba.transition(name, blockEnd, () -> { addToken(NT_NAME_TOKEN); bwd(); next(space); });
	ba.transition(name, ":", () -> { addToken(NT_NAME_TOKEN); addBlock(NT_ASSIGNMENT); next(skipLineBreaks); });
	ba.transition(name, ".", () -> { addToken(NT_NAME_TOKEN); addOperator(NT_DOT,new MSText(".")); next(space); });
	
	ba.transition(reference, letters, null);
	ba.transition(reference, whitespace, () -> { addToken(NT_REFERENCE_TOKEN); next(space); });
	ba.transition(reference, expressionBreak, () -> { addToken(NT_REFERENCE_TOKEN); exprBreak(); next(space); });
	ba.transition(reference, blockStart, () -> { addToken(NT_REFERENCE_TOKEN); bwd(); next(space); });
	ba.transition(reference, blockEnd,   () -> { addToken(NT_REFERENCE_TOKEN); bwd(); next(space); });

	ba.transition(number, numbers, null);
	ba.transition(number, whitespace,       () -> { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(number, expressionBreak,  () -> { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(number, blockStart,       () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(number, blockEnd,         () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(number, ".", () -> { nextCont(decimalNumber); });
	
	ba.transition(zero, numbers,          () -> { nextCont(number); });
	ba.transition(zero, "x",              () -> { next(hex); lastStart++; });
	ba.transition(zero, ".",              () -> { next(decimalNumber); lastStart++; });
	ba.transition(zero, whitespace,       () -> { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(zero, expressionBreak,  () -> { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(zero, blockStart,       () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(zero, blockEnd,         () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	
	ba.transition(hex, hexNumbers, null);
	ba.transition(hex, whitespace,       () -> { addToken(NT_HEX_TOKEN); next(space); });
	ba.transition(hex, expressionBreak,  () -> { addToken(NT_HEX_TOKEN); exprBreak(); next(space); });
	ba.transition(hex, blockStart,       () -> { addToken(NT_HEX_TOKEN); bwd(); next(space); });
	ba.transition(hex, blockEnd,         () -> { addToken(NT_HEX_TOKEN); bwd(); next(space); });

	ba.transition(minus, whitespace, () -> { addToken(NT_MINUS); next(space); });
	ba.transition(minus, numbers, () -> { parsedSign = -1; nextCont(number); }); // change state without reseting starting index
	ba.transition(minus, ".", () -> { parsedSign = -1; nextCont(decimalNumber); });

 	ba.transition(decimalNumber, numbers, null);
	ba.transition(decimalNumber, whitespace, () -> { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(decimalNumber, expressionBreak, () -> { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(decimalNumber, blockStart, () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(decimalNumber, blockEnd,   () -> { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	
 /* decimalNumber = ba.addState("decimal number");

	expNumber = ba.addState("exp. number");*/
	
	ba.fillTransition(slash, () -> { addToken(NT_DIV); bwd(); next(space); });
	ba.transition(slash, "/", () -> { next(comment); });

	ba.fillTransition(quote, () -> { addQuoteByte(baPtr.currentInput); });
	ba.transition(quote, linebreak, () -> { parseError("line break inside a quotation"); });
	ba.transition(quote, "\"", () -> { lastStart++; addToken(NT_TEXT); next(space); });
	ba.transition(quote, "\\", () -> { next(escapeChar); });
	
	ba.fillTransition(escapeChar, () -> { parseError("invalid escape character in quotes"); });
	
	// standard escape character literals: https://en.cppreference.com/w/cpp/language/escape
	
	ba.transition(escapeChar, "'",  () -> { addQuoteByte((byte)0x27); next(quote); });
	ba.transition(escapeChar, "\"", () -> { addQuoteByte((byte)0x22); next(quote); });
	ba.transition(escapeChar, "?",  () -> { addQuoteByte((byte)0x3f); next(quote); });
	ba.transition(escapeChar, "\\", () -> { addQuoteByte((byte)0x5c); next(quote); });
	ba.transition(escapeChar, "a",  () -> { addQuoteByte((byte)0x07); next(quote); });
	ba.transition(escapeChar, "b",  () -> { addQuoteByte((byte)0x08); next(quote); });
	ba.transition(escapeChar, "f",  () -> { addQuoteByte((byte)0x0c); next(quote); });
	ba.transition(escapeChar, "n",  () -> { addQuoteByte((byte)0x0a); next(quote); });
	ba.transition(escapeChar, "r",  () -> { addQuoteByte((byte)0x0d); next(quote); });
	ba.transition(escapeChar, "t",  () -> { addQuoteByte((byte)0x09); next(quote); });
	ba.transition(escapeChar, "v",  () -> { addQuoteByte((byte)0x0b); next(quote); });
	
	ba.transition(escapeChar, "x", () -> { next(hexByte); });
	
	ba.fillTransition(hexByte, () -> { parseError("invalid hexadecimal byte"); });
	ba.transition(hexByte, hexNumbers, () -> { if (index - lastStart >= 2) { addHexByte(); next(quote); } });
	
	ba.fillTransition(comment, null);
	ba.transition(comment, linebreak, () -> { exprBreak(); next(space); });
	
	ba.fillTransition(skipLineBreaks, () -> { bwd(); next(space); });
	ba.transition(skipLineBreaks, whitespace, null);
	ba.transition(skipLineBreaks, linebreak, null);
}

private static boolean validNameChars [];
private static boolean validNumberChars [];
private static boolean nameValidatorInitialized = false;

public static boolean  isValidName (MSText name) throws MException
{
	// name validator
	// initialize if needed
	
	int length, i;
	
	if (!nameValidatorInitialized) {
		byte[] letterBytes = letters.getBytes(java.nio.charset.StandardCharsets.UTF_8);
		byte[] numberBytes = numbers.getBytes(java.nio.charset.StandardCharsets.UTF_8);
		//byte[] numberBytes = numbers.getBytes(java.nio.charset.StandardCharsets.UTF_8);
		validNameChars = new  boolean[ 256];
		validNumberChars = new  boolean[ 256];
		java.util.Arrays.fill(validNameChars,false);
		java.util.Arrays.fill(validNumberChars,false);
		
		String lettersString = letters;
		length = lettersString.length();
		for (i=0; i<length; i++) {
			validNameChars[letterBytes[i]] = true;
		}
		String numbersString = numbers;
		length = numbersString.length();
		for (i=0; i<length; i++) {
			validNumberChars[numberBytes[i]] = true;
		}
		
		nameValidatorInitialized = true;
	}
	
	length = name.numBytes();
	
	if (length < 1 || length > MSJava.globalConfig.maxNameLength) return false;
	
	// first character must be a letter or under-score
	if (!validNameChars[name.byteAt(0)]) return false;
	
	for (i=1; i<length; i++) {
		int b = name.byteAt(i);
		if (!validNameChars[b] && !validNumberChars[b]) return false;
	}

	return true;
}

public static boolean  isValidName (String name) throws MException
{
	MSText t = new MSText (name);
	return isValidName(t);
}

public static TokenTree  Parse (MSInputStream input) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("------------------------ START PARSING").endLine();};

	tokenTree = new TokenTree();

	baPtr = new ByteAutomata();
	ByteAutomata ba = baPtr;
	
	defineTransitions();

	ba.next((byte)1);

	root = new MNode(1, 1, null, NT_EXPR, new MSText("<ROOT>"));
	currentExpr = root;
	currentBlock = null;
	currentToken = null;
	
	textCount = 0;
	lastStart = 0;
	running = true;
	assignment = false;
	goBackwards = false;
	parsedSign = 0;
	parsedNumber = 0;
	
	// text ID 0: empty
	tokenTree.texts.put( new MSText (""), textCount++);
	
	lineNumber = 1;
	characterNumber = 1;
	int inputByte = 0;
	index = 0;

	while ((!input.end() || goBackwards) && running && ba.ok)
	{
		if (!goBackwards)
		{
			index ++;
			inputByte = input.readByte();
			buffer[index % 512] = (byte)inputByte;
			if (inputByte == 10) // line break
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
		if (MSJava.globalConfig.verboseOn())
		{
			MSJava.printOut.print(" [").printCharSymbol(inputByte).print("]\n");
		}

		running = ba.step(inputByte);
	}

	if (!goBackwards) index++;
	if (ba.ok) ba.step((byte)'\n'); // ended cleanly: last command break
	if (currentBlock != null)
	{
		MSJava.printOut.print("closing parenthesis missing at the end").endLine();
		ba.ok = false;
	}
	
	if (!running || !(ba.ok))
	{
		MSJava.errorOut.print("Parser state [" + ba.stateNames.get( (int)ba.currentState) + "]\n");
		MSJava.errorOut.print("Line " + lineNumber + ": \"");
		
		// print nearby code
		int start = index-1;
		while (start > 0 && index - start < 512 && (char)buffer[start % 512] != '\n')
			start --;
		while (++start < index)
		{
			MSJava.errorOut.print("").printCharSymbol((((int) buffer[start % 512]) & 0xff));
		}
		MSJava.errorOut.print("\"\n");
		
		baPtr = null;
		root = null;
		tokenTree = null;
		MSJava.assertion(false, EC_PARSE, null);
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
