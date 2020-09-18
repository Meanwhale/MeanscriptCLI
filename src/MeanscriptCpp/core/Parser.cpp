
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;


const char * letters =  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char * numbers =  "1234567890";
const char * whitespace =  " \t\n\r";
const char * linebreak =  "\n\r";
const char * expressionBreak =  ",;\n";
const char * blockStart =  "([{";
const char * blockEnd =  ")]}";
const char * op =  "+*<>="; // '-' or '/' will be special cases

uint8_t space, name, reference, number, minus, decimalNumber, slash, quote, comment, skipLineBreaks;


Array<uint8_t> tmp(512);
Array<uint8_t> buffer(512);

ByteAutomata* baPtr;
bool goBackwards, running, assignment;
int32_t index, lastStart, textCount = 0, lineNumber, characterNumber;
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

//void copy(uint8_t * dst, const uint8_t * src, int32_t start, int32_t end)
//{
//	for (int32_t i = start, n = 0; i < end; i++, n++)
//	{
//		dst[n] = src[i];
//	}
//}


std::string getNewName() 
{
	int32_t start = lastStart;
	int32_t length = index - start;
	CHECK(length < CFG_MAX_NAME_LENGTH, EC_PARSE, "name is too long");
	
	int32_t i = 0;
	for (; i < length; i++)
	{
		tmp[i] = buffer[start++ % 512];
	}
	
	tmp[i] = '\0';
	return std::string((char*)tmp.get());
}


void addExpr()
{
	MNode* expr = new MNode(lineNumber, characterNumber, currentBlock, NT_EXPR, "<EXPR>");
	(*currentExpr).next = expr;
	currentExpr = expr;
	currentToken = 0;
}


void addToken(int32_t tokenType) 
{ 
	if (tokenType == NT_REFERENCE_TOKEN) lastStart++; // skip '#'

	std::string data = getNewName();

	DEBUG(VR("TOKEN: ")X(data)XO);

	if (tokenType == NT_TEXT)
	{
		// add text to the tree if same text is not already there
		if (!((*tokenTree).texts.find( data) != (*tokenTree).texts.end()))
		{
			(*tokenTree).texts.insert(std::make_pair( data, textCount++));;
		}
	}

	MNode* token = new MNode(lineNumber, characterNumber, currentExpr, tokenType, data);

	if (currentToken == 0) (*currentExpr).child = token;
	else (*currentToken).next = token;
	(*currentExpr).numChildren++;
	currentToken = token;
	lastStart = index;
}
void addOperator(int32_t tokenType, std::string name)
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

	const char * blockTypeName = "???";
	
	if (blockType == NT_PARENTHESIS) blockTypeName = "<PARENTHESIS>";
	else if (blockType == NT_SQUARE_BRACKETS) blockTypeName = "<SQUARE_BRACKETS>";
	else if (blockType == NT_ASSIGNMENT)
	{
		assignment = true;
		blockTypeName = "<ASSIGNMENT>";
	}
	else if (blockType == NT_CODE_BLOCK) blockTypeName = "<CODE_BLOCK>";
	else ASSERT(false, "invalid block type");
	
	DEBUG(VR("add block: ")X(blockType)XO);
	
	lastStart = -1;
	std::string tmp123(blockTypeName);
	MNode* block = new MNode(lineNumber, characterNumber, currentExpr, blockType, tmp123);
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

	MNode* expr = new MNode(lineNumber, characterNumber, currentBlock, NT_EXPR, "<EXPR>");
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

void defineTransitions() 
{
	ByteAutomata & ba = (*baPtr);

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

	ba.transition(space, whitespace, 0);
	ba.transition(space, letters, []() { next(name); });
	ba.transition(space, "#", []() { next(reference); });
	ba.transition(space, "-", []() { next(minus); });
	ba.transition(space, "/", []() { next(slash); });
	ba.transition(space, "\"", []() { next(quote); });
	ba.transition(space, numbers, []() { next(number); });
	ba.transition(space, expressionBreak, []() { exprBreak(); });
	ba.transition(space, "(", []() { addBlock(NT_PARENTHESIS); next(skipLineBreaks); });
	ba.transition(space, ")", []() { endBlock(NT_PARENTHESIS); });
	ba.transition(space, "[", []() { addBlock(NT_SQUARE_BRACKETS); next(skipLineBreaks); });
	ba.transition(space, "]", []() { endBlock(NT_SQUARE_BRACKETS); });
	ba.transition(space, "{", []() { addBlock(NT_CODE_BLOCK); next(skipLineBreaks); });
	ba.transition(space, "}", []() { endBlock(NT_CODE_BLOCK); });
	ba.transition(space, ":", []() { addBlock(NT_ASSIGNMENT); next(skipLineBreaks); });
	ba.transition(space, ".", []() { addOperator(NT_DOT,"."); });

	ba.transition(name, letters, 0);
	ba.transition(name, whitespace, []() { addToken(NT_NAME_TOKEN); next(space); });
	ba.transition(name, "#", []() { addToken(NT_REF_TYPE_TOKEN); next(space); });
	ba.transition(name, expressionBreak, []() { addToken(NT_NAME_TOKEN); exprBreak(); next(space); });
	ba.transition(name, blockStart, []() { addToken(NT_NAME_TOKEN); bwd(); next(space); });
	ba.transition(name, blockEnd, []() { addToken(NT_NAME_TOKEN); bwd(); next(space); });
	ba.transition(name, ":", []() { addToken(NT_NAME_TOKEN); addBlock(NT_ASSIGNMENT); next(skipLineBreaks); });
	ba.transition(name, ".", []() { addToken(NT_NAME_TOKEN); addOperator(NT_DOT,"."); next(space); });
	
	//ba.fillTransition(member, []() { addMember(); bwd(); next(space); });
	//ba.transition(member, letters, 0);
	//ba.transition(member, ".", []() { addMember(); next(member); });

	ba.transition(reference, letters, 0);
	ba.transition(reference, whitespace, []() { addToken(NT_REFERENCE_TOKEN); next(space); });
	ba.transition(reference, expressionBreak, []() { addToken(NT_REFERENCE_TOKEN); exprBreak(); next(space); });
	ba.transition(reference, blockStart, []() { addToken(NT_REFERENCE_TOKEN); bwd(); next(space); });
	ba.transition(reference, blockEnd,   []() { addToken(NT_REFERENCE_TOKEN); bwd(); next(space); });

	ba.transition(number, numbers, 0);
	ba.transition(number, whitespace, []() { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(number, expressionBreak, []() { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(number, blockStart, []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(number, blockEnd,   []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(number, ".", []() { nextCont(decimalNumber); });

	ba.transition(minus, whitespace, []() { addToken(NT_MINUS); next(space); });
	ba.transition(minus, numbers, []() { nextCont(number); }); // change state without reseting starting index
	ba.transition(minus, ".", []() { nextCont(decimalNumber); });

 	ba.transition(decimalNumber, numbers, 0);
	ba.transition(decimalNumber, whitespace, []() { addToken(NT_NUMBER_TOKEN); next(space); });
	ba.transition(decimalNumber, expressionBreak, []() { addToken(NT_NUMBER_TOKEN); exprBreak(); next(space); });
	ba.transition(decimalNumber, blockStart, []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	ba.transition(decimalNumber, blockEnd,   []() { addToken(NT_NUMBER_TOKEN); bwd(); next(space); });
	
 /* decimalNumber = ba.addState("decimal number");

	expNumber = ba.addState("exp. number");*/
	
	ba.fillTransition(slash, []() { addToken(NT_DIV); bwd(); next(space); });
	ba.transition(slash, "/", []() { next(comment); });

	ba.fillTransition(quote, 0);
	ba.transition(quote, linebreak, []() { parseError("line break inside a quotation"); });
	ba.transition(quote, "\"", []() { lastStart++; addToken(NT_TEXT); next(space); });
	
	ba.fillTransition(comment, 0);
	ba.transition(comment, linebreak, []() { exprBreak(); next(space); });
	
	ba.fillTransition(skipLineBreaks, []() { bwd(); next(space); });
	ba.transition(skipLineBreaks, whitespace, 0);
	ba.transition(skipLineBreaks, linebreak, 0);
}


TokenTree* Parser:: Parse (MSInputStream & input) 
{
	VERBOSE("------------------------ START PARSING");

	tokenTree = new TokenTree();

	baPtr = new ByteAutomata();
	ByteAutomata & ba = (*baPtr);
	
	defineTransitions();

	ba.next((uint8_t)1);

	root = new MNode(1, 1, 0, NT_EXPR, "<ROOT>");
	currentExpr = root;
	currentBlock = 0;
	currentToken = 0;
	
	textCount = 0;
	lastStart = 0;
	running = true;
	assignment = false;
	goBackwards = false;
	
	// text ID 0: empty
	(*tokenTree).texts.insert(std::make_pair( "", textCount++));;
	
	lineNumber = 1;
	characterNumber = 1;
	uint8_t inputByte = 0;
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
		DEBUG(VR("[ ")X((char)(inputByte))X(" ]")XO);

		running = ba.step(inputByte);
	}

	if (!goBackwards) index++;
	ba.step((uint8_t)'\n'); // ended cleanly: last command break
	if (currentBlock != 0)
	{
		PRINT("closing parenthesis missing at the end");
		ba.ok = false;
	}
	
	if (!running || !(ba.ok))
	{
		PR("Parser state [")X(ba.stateNames[ (int32_t)ba.currentState])X("]")XO;
		PR("Line ")X(lineNumber)X(": \"");
		
		// print nearby code
		int32_t start = index-1;
		while (start > 0 && index - start < 512 && (char)buffer[start % 512] != '\n')
			start --;
		while (++start < index)
		{
			VR((char)(buffer[start % 512]));
		}
		PRINT("\"");
		
		{ delete baPtr; baPtr = 0; };
		{ delete root; root = 0; };
		{ delete tokenTree; tokenTree = 0; };
		EXIT("Parse error");
	}

	if (globalConfig.verboseOn)
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
// C++ END
