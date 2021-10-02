#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;


const char * letters =  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const char * numbers =  "1234567890";
const char * hexNumbers =  "1234567890abcdefABCDEF";
const char * whitespace =  " \t\n\r";
const char * linebreak =  "\n\r";
const char * expressionBreak =  ",;\n";
const char * blockStart =  "([{";
const char * blockEnd =  ")]}";
const char * op =  "+*<>="; // '-' or '/' will be special cases

uint8_t space, name, reference, number, hex, minus, decimalNumber, slash, quote, comment, skipLineBreaks, escapeChar, hexByte, zero;


Array<uint8_t> tmp(4096);
Array<uint8_t> buffer(512);
Array<uint8_t> quoteBuffer(4096);

ByteAutomata* baPtr;
bool goBackwards, running, assignment;
int32_t index, lastStart, textCount = 0, lineNumber, characterNumber, quoteIndex, parsedSign, parsedNumber;
MNode* root;
MNode* currentBlock;
MNode* currentExpr;
MNode* currentToken;
TokenTree* tokenTree;

constexpr char const * nodeTypeName [] = { "root","expr","sub-call","struct","block","token" };


void next(uint8_t state) 
{
	lastStart = index;

	(*baPtr).next(state);
}

void nextCont(uint8_t state) 
{
	// continue with same token,
	// so don't reset the start index
	(*baPtr).next(state);
}

void bwd() 
{
	ASSERT(!goBackwards, "can't go backwards twice");
	goBackwards = true;
}

MSText* getNewName() 
{
	int32_t start = lastStart;
	int32_t length = index - start;
	CHECK(length < globalConfig.maxNameLength, EC_PARSE, "name is too long");
	
	int32_t i = 0;
	for (; i < length; i++)
	{
		tmp[i] = buffer[start++ % 512];
	}
	return new MSText (tmp.get(), 0, length);
}

MSText* getQuoteText() 
{
	int32_t start = 0;
	int32_t length = quoteIndex - start;
	return new MSText (quoteBuffer.get(), start, length);
}


void addExpr() 
{
	MNode* expr = new MNode(lineNumber, characterNumber, currentBlock, NT_EXPR, new MSText ("<EXPR>"));
	(*currentExpr).next = expr;
	currentExpr = expr;
	currentToken = 0;
}


void addToken(int32_t tokenType) 
{ 
	if (tokenType == NT_REFERENCE_TOKEN) lastStart++; // skip '#'
	
	MNode* token = 0;
	
	if (tokenType == NT_TEXT)
	{
		MSText* data = getQuoteText();
		// add text to the tree if same text is not already there
		if (!((*tokenTree).texts.find( (*data)) != (*tokenTree).texts.end()))
		{
			(*tokenTree).texts.insert(std::make_pair( MSText((*data)), textCount++));;
		}
		token = new MNode(lineNumber, characterNumber, currentExpr, NT_TEXT, data);
	}
	else
	{
		MSText* data = getNewName();
		DEBUG(VERBOSE("TOKEN: " CAT (*data)));
		token = new MNode(lineNumber, characterNumber, currentExpr, tokenType, data);
	}
	
	if (currentToken == 0) (*currentExpr).child = token;
	else (*currentToken).next = token;
	(*currentExpr).numChildren++;
	currentToken = token;
	lastStart = index;
}
void addOperator(int32_t tokenType, MSText* name)
{ 
	MNode* token = new MNode(lineNumber, characterNumber, currentExpr, tokenType, name);
	if (currentToken == 0) (*currentExpr).child = token;
	else (*currentToken).next = token;
	(*currentExpr).numChildren++;
	currentToken = token;
	lastStart = index;
}

void endBlock(int32_t blockType) 
{
	// check that block-end character is the right one
	CHECK((currentBlock != 0 && (*currentBlock).type == blockType), EC_PARSE, "invalid block end");

	lastStart = -1;
	currentToken = currentBlock;
	currentExpr = (*currentToken).parent;
	currentBlock = (*currentExpr).parent;
}

void exprBreak() 
{
	// check that comma is used properly
	if ((*baPtr).currentInput == ',')	
	{
		CHECK(currentBlock != 0 && 
			((*currentBlock).type == NT_ASSIGNMENT ||
			(*currentBlock).type == NT_SQUARE_BRACKETS ||
			(*currentBlock).type == NT_PARENTHESIS), EC_PARSE, "unexpected comma");
	}
	
	if (currentBlock != 0) (*currentBlock).numChildren ++;
	
	if (assignment)
	{
		if ((*baPtr).currentInput != ',')
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


void addBlock(int32_t blockType) 
{

	MSText* blockTypeName = 0;
	
	if (blockType == NT_PARENTHESIS) blockTypeName = new MSText("<PARENTHESIS>");
	else if (blockType == NT_SQUARE_BRACKETS) blockTypeName = new MSText("<SQUARE_BRACKETS>");
	else if (blockType == NT_ASSIGNMENT)
	{
		assignment = true;
		blockTypeName = new MSText("<ASSIGNMENT>");
	}
	else if (blockType == NT_CODE_BLOCK) blockTypeName = new MSText("<CODE_BLOCK>");
	else ASSERT(false, "invalid block type");
	
	DEBUG(VERBOSE("add block: " CAT blockType));
	
	lastStart = -1;

	ASSERT(blockTypeName != 0, "blockTypeName is null");

	MNode* block = new MNode(lineNumber, characterNumber, currentExpr, blockType, blockTypeName);
	
	if (currentToken == 0)
	{
		// expression starts with a block, eg. "[1,2]" in "[[1,2],34,56]"
		(*currentExpr).child = block;
		currentToken = block;
	}
	else
	{
		(*currentToken).next = block;
	}
	(*currentExpr).numChildren++;
	
	currentBlock = block;

	MNode* expr = new MNode(lineNumber, characterNumber, currentBlock, NT_EXPR, new MSText("<EXPR>"));
	(*currentBlock).child = expr;
	currentExpr = expr;
	currentToken = 0;
}

void parseError(const char * msg) 
{
	PRINTN("Parse error: ");
	PRINT(msg);
	(*baPtr).ok = false;
	running = false;
}

void addQuoteByte(int32_t i) 
{
	if (quoteIndex >= 4096)
	{
		parseError("text is too long");
		return;
	}
	quoteBuffer[quoteIndex++] = (uint8_t)i;
}

void addHexByte() 
{
	uint8_t high = buffer[(lastStart + 1) % 512];
	uint8_t low = buffer [(lastStart + 2) % 512];
	uint8_t b = (uint8_t)(((hexCharToByte(high)<<4) & 0xf0) | hexCharToByte(low));
	addQuoteByte(b);
}

void defineTransitions() 
{
	ByteAutomata & ba = (*baPtr);

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

	ba.transition(space, whitespace, 0);
	ba.transition(space, letters, []() { next(name); });
	ba.transition(space, "#", []() { next(reference); });
	ba.transition(space, "-", []() { next(minus); });
	ba.transition(space, "/", []() { next(slash); });
	ba.transition(space, "\"", []() { next(quote); quoteIndex = 0; });
	ba.transition(space, numbers, []() { next(number); });
	ba.transition(space, "0", []() { next(zero); }); // start hex
	ba.transition(space, expressionBreak, []() { exprBreak(); });
	ba.transition(space, "(", []() { addBlock(NT_PARENTHESIS); next(skipLineBreaks); });
	ba.transition(space, ")", []() { endBlock(NT_PARENTHESIS); });
	ba.transition(space, "[", []() { addBlock(NT_SQUARE_BRACKETS); next(skipLineBreaks); });
	ba.transition(space, "]", []() { endBlock(NT_SQUARE_BRACKETS); });
	ba.transition(space, "{", []() { addBlock(NT_CODE_BLOCK); next(skipLineBreaks); });
	ba.transition(space, "}", []() { endBlock(NT_CODE_BLOCK); });
	ba.transition(space, ":", []() { addBlock(NT_ASSIGNMENT); next(skipLineBreaks); });
	ba.transition(space, ".", []() { addOperator(NT_DOT,new MSText(".")); });

	ba.transition(name, letters, 0);
	ba.transition(name, numbers, 0);
	ba.transition(name, whitespace, []() { addToken(NT_NAME_TOKEN); next(space); });
	ba.transition(name, "#", []() { addToken(NT_REF_TYPE_TOKEN); next(space); });
	ba.transition(name, expressionBreak, []() { addToken(NT_NAME_TOKEN); exprBreak(); next(space); });
	ba.transition(name, blockStart, []() { addToken(NT_NAME_TOKEN); bwd(); next(space); });
	ba.transition(name, blockEnd, []() { addToken(NT_NAME_TOKEN); bwd(); next(space); });
	ba.transition(name, ":", []() { addToken(NT_NAME_TOKEN); addBlock(NT_ASSIGNMENT); next(skipLineBreaks); });
	ba.transition(name, ".", []() { addToken(NT_NAME_TOKEN); addOperator(NT_DOT,new MSText(".")); next(space); });
	
	ba.transition(reference, letters, 0);
	ba.transition(reference, whitespace, []() { addToken(NT_REFERENCE_TOKEN); next(space); });
	ba.transition(reference, expressionBreak, []() { addToken(NT_REFERENCE_TOKEN); exprBreak(); next(space); });
	ba.transition(reference, blockStart, []() { addToken(NT_REFERENCE_TOKEN); bwd(); next(space); });
	ba.transition(reference, blockEnd,   []() { addToken(NT_REFERENCE_TOKEN); bwd(); next(space); });

	ba.transition(number, numbers, 0);
	ba.transition(number, whitespace,       []() { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(number, expressionBreak,  []() { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(number, blockStart,       []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(number, blockEnd,         []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(number, ".", []() { nextCont(decimalNumber); });
	
	ba.transition(zero, numbers,          []() { nextCont(number); });
	ba.transition(zero, "x",              []() { next(hex); lastStart++; });
	ba.transition(zero, ".",              []() { next(decimalNumber); lastStart++; });
	ba.transition(zero, whitespace,       []() { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(zero, expressionBreak,  []() { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(zero, blockStart,       []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(zero, blockEnd,         []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	
	ba.transition(hex, hexNumbers, 0);
	ba.transition(hex, whitespace,       []() { addToken(NT_HEX_TOKEN); next(space); });
	ba.transition(hex, expressionBreak,  []() { addToken(NT_HEX_TOKEN); exprBreak(); next(space); });
	ba.transition(hex, blockStart,       []() { addToken(NT_HEX_TOKEN); bwd(); next(space); });
	ba.transition(hex, blockEnd,         []() { addToken(NT_HEX_TOKEN); bwd(); next(space); });

	ba.transition(minus, whitespace, []() { addToken(NT_MINUS); next(space); });
	ba.transition(minus, numbers, []() { parsedSign = -1; nextCont(number); }); // change state without reseting starting index
	ba.transition(minus, ".", []() { parsedSign = -1; nextCont(decimalNumber); });

 	ba.transition(decimalNumber, numbers, 0);
	ba.transition(decimalNumber, whitespace, []() { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(decimalNumber, expressionBreak, []() { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(decimalNumber, blockStart, []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(decimalNumber, blockEnd,   []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	
 /* decimalNumber = ba.addState("decimal number");

	expNumber = ba.addState("exp. number");*/
	
	ba.fillTransition(slash, []() { addToken(NT_DIV); bwd(); next(space); });
	ba.transition(slash, "/", []() { next(comment); });

	ba.fillTransition(quote, []() { addQuoteByte((*baPtr).currentInput); });
	ba.transition(quote, linebreak, []() { parseError("line break inside a quotation"); });
	ba.transition(quote, "\"", []() { lastStart++; addToken(NT_TEXT); next(space); });
	ba.transition(quote, "\\", []() { next(escapeChar); });
	
	ba.fillTransition(escapeChar, []() { parseError("invalid escape character in quotes"); });
	
	// standard escape character literals: https://en.cppreference.com/w/cpp/language/escape
	
	ba.transition(escapeChar, "'",  []() { addQuoteByte((uint8_t)0x27); next(quote); });
	ba.transition(escapeChar, "\"", []() { addQuoteByte((uint8_t)0x22); next(quote); });
	ba.transition(escapeChar, "?",  []() { addQuoteByte((uint8_t)0x3f); next(quote); });
	ba.transition(escapeChar, "\\", []() { addQuoteByte((uint8_t)0x5c); next(quote); });
	ba.transition(escapeChar, "a",  []() { addQuoteByte((uint8_t)0x07); next(quote); });
	ba.transition(escapeChar, "b",  []() { addQuoteByte((uint8_t)0x08); next(quote); });
	ba.transition(escapeChar, "f",  []() { addQuoteByte((uint8_t)0x0c); next(quote); });
	ba.transition(escapeChar, "n",  []() { addQuoteByte((uint8_t)0x0a); next(quote); });
	ba.transition(escapeChar, "r",  []() { addQuoteByte((uint8_t)0x0d); next(quote); });
	ba.transition(escapeChar, "t",  []() { addQuoteByte((uint8_t)0x09); next(quote); });
	ba.transition(escapeChar, "v",  []() { addQuoteByte((uint8_t)0x0b); next(quote); });
	
	ba.transition(escapeChar, "x", []() { next(hexByte); });
	
	ba.fillTransition(hexByte, []() { parseError("invalid hexadecimal byte"); });
	ba.transition(hexByte, hexNumbers, []() { if (index - lastStart >= 2) { addHexByte(); next(quote); } });
	
	ba.fillTransition(comment, 0);
	ba.transition(comment, linebreak, []() { exprBreak(); next(space); });
	
	ba.fillTransition(skipLineBreaks, []() { bwd(); next(space); });
	ba.transition(skipLineBreaks, whitespace, 0);
	ba.transition(skipLineBreaks, linebreak, 0);
}

Array<bool> validNameChars;
Array<bool> validNumberChars;
bool nameValidatorInitialized = false;

bool Parser:: isValidName (MSText* name) 
{
	// name validator
	// initialize if needed
	
	int32_t length, i;
	
	if (!nameValidatorInitialized) {
		uint8_t * letterBytes = (uint8_t *)letters;
		uint8_t * numberBytes = (uint8_t *)numbers;
		//uint8_t * numberBytes = (uint8_t *)numbers;
		validNameChars.reset( 256);
		validNumberChars.reset( 256);
		validNameChars.fill(false);
		validNumberChars.fill(false);
		
		std::string lettersString = letters;
		length = lettersString.size();
		for (i=0; i<length; i++) {
			validNameChars[letterBytes[i]] = true;
		}
		std::string numbersString = numbers;
		length = numbersString.size();
		for (i=0; i<length; i++) {
			validNumberChars[numberBytes[i]] = true;
		}
		
		nameValidatorInitialized = true;
	}
	
	length = (*name).numBytes();
	
	if (length < 1 || length > globalConfig.maxNameLength) return false;
	
	// first character must be a letter or under-score
	if (!validNameChars[(*name).byteAt(0)]) return false;
	
	for (i=1; i<length; i++) {
		int32_t b = (*name).byteAt(i);
		if (!validNameChars[b] && !validNumberChars[b]) return false;
	}

	return true;
}

bool Parser:: isValidName (const char * name) 
{
	MSText t (name);
	return isValidName((&(t)));
}

TokenTree* Parser:: Parse (MSInputStream & input) 
{
	VERBOSE("------------------------ START PARSING");

	tokenTree = new TokenTree();

	baPtr = new ByteAutomata();
	ByteAutomata & ba = (*baPtr);
	
	defineTransitions();

	ba.next((uint8_t)1);

	root = new MNode(1, 1, 0, NT_EXPR, new MSText("<ROOT>"));
	currentExpr = root;
	currentBlock = 0;
	currentToken = 0;
	
	textCount = 0;
	lastStart = 0;
	running = true;
	assignment = false;
	goBackwards = false;
	parsedSign = 0;
	parsedNumber = 0;
	
	// text ID 0: empty
	(*tokenTree).texts.insert(std::make_pair( MSText (""), textCount++));;
	
	lineNumber = 1;
	characterNumber = 1;
	int32_t inputByte = 0;
	index = 0;

	while ((!input.end() || goBackwards) && running && ba.ok)
	{
		if (!goBackwards)
		{
			index ++;
			inputByte = input.readByte();
			buffer[index % 512] = (uint8_t)inputByte;
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
		if (globalConfig.verboseOn())
		{
			PRINTN(" [").printCharSymbol(inputByte).print("]\n");
		}

		running = ba.step(inputByte);
	}

	if (!goBackwards) index++;
	if (ba.ok) ba.step((uint8_t)'\n'); // ended cleanly: last command break
	if (currentBlock != 0)
	{
		if (assignment) PRINT("unexpected end of file in assignment");
		else PRINT("unexpected end of file: closing parenthesis missing");
		ba.ok = false;
	}
	
	if (!running || !(ba.ok))
	{
		ERROR_PRINT("Parser state [" CAT ba.stateNames[ (int32_t)ba.currentState] CAT "]\n");
		ERROR_PRINT("Line " CAT lineNumber CAT ": \n        ");
		
		// print nearby code
		int32_t start = index-1;
		while (start > 0 && index - start < 512 && (char)buffer[start % 512] != '\n')
			start --;
		while (++start < index)
		{
			ERROR_PRINT("").printCharSymbol((((int) buffer[start % 512]) & 0xff));
		}
		ERROR_PRINT("\n");
		
		{ delete baPtr; baPtr = 0; };
		{ delete root; root = 0; };
		{ delete tokenTree; tokenTree = 0; };
		CHECK(false, EC_PARSE, 0);
	}

	if (globalConfig.verboseOn())
	{
		PRINT("------------------------ TOKEN TREE:");
		(*root).printTree(true);
		PRINT("------------------------ END PARSING");
	}	
	{ delete baPtr; baPtr = 0; };
	
	(*tokenTree).root = root;
	return tokenTree;
}


} // namespace meanscript(core)
