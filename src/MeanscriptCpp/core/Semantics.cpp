#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

Semantics::Semantics () 
{
	typeIDCounter = MAX_MS_TYPES;

	typeStructDefs = new StructDef*[MAX_TYPES];
	for (int32_t i=0; i<MAX_TYPES; i++)
	{
		typeStructDefs[i] = 0;
	}
	
	maxContexts = globalConfig.maxFunctions;
	contexts = new Context*[maxContexts];
	
	contexts[0] = new Context(0, 0, -1); // global context
	globalContext = contexts[0];
	currentContext = globalContext;
	for (int32_t i=1; i<maxContexts; i++)
	{
		contexts[i] = 0;
	}
	numContexts=1;
}
Semantics::~Semantics() { 	for (int32_t i=0; i<maxContexts; i++) { delete contexts[i]; } 	delete[] contexts; 	for (int32_t i=0; i<MAX_TYPES; i++) { delete typeStructDefs[i]; } 	delete[] typeStructDefs; };

void Semantics::addPrimitiveType (MSText* name, StructDef* sd, int32_t id) 
{
	VERBOSE("Add primitive type [" CAT id CAT "] " CAT name);
	types.insert(std::make_pair( MSText((*name)), id));;
	typeStructDefs[id] = sd;
	
}

void Semantics::addElementaryType (int32_t typeID, int32_t size) 
{
	std::string name = primitiveNames[typeID];
	VERBOSE("Add elementary type [" CAT typeID CAT "] " CAT name);
	types.insert(std::make_pair( MSText(name), typeID));;
	MSText tmp (name);
	StructDef* sd = new StructDef((&(tmp)), typeID, size);
	typeStructDefs[typeID] = sd;
}

StructDef* Semantics::addCharsType (int32_t numChars) 
{
	// words:
	//		0:		size in characters (start stringToIntsWithSize() from here)
	//		1...n:	characters + zero at the end, e.g. "mean\0" --> 2 words = (numChars / 4) + 1
	
	int32_t arraySize = (numChars / 4) + 2;
	SYNTAX(arraySize > 0 && arraySize < globalConfig.maxArraySize, 0, "invalid array size");
	int32_t typeID = typeIDCounter++;
	StructDef* sd = new StructDef(0, typeID, numChars, arraySize, OP_CHARS_DEF);
	typeStructDefs[typeID] = sd;
	return sd;
}

bool Semantics::hasType(MSText* name)
{
	return (types.find( (*name)) != types.end());
}

bool Semantics::hasType(int32_t id)
{
	return typeStructDefs[id] != 0;
}

StructDef* Semantics:: getType (int32_t id) 
{
	StructDef* userType = typeStructDefs[id];
	ASSERT(userType != 0, "Data type error");
	return userType;
}

StructDef* Semantics:: getType (MSText* name) 
{
	int32_t id = nameTreeGet(types, name);
	StructDef* userType = typeStructDefs[id];
	ASSERT(userType != 0, "Data type error: " CAT name);
	return userType;
}

StructDef* Semantics:: getType (int32_t id, NodeIterator* itPtr) 
{
	StructDef* userType = typeStructDefs[id];
	SYNTAX(userType != 0, (*itPtr), "Data type error: #" CAT id);
	return userType;
}

StructDef* Semantics:: getType (MSText* name, NodeIterator* itPtr) 
{
	int32_t id = nameTreeGet(types, name);
	StructDef* userType = typeStructDefs[id];
	SYNTAX(userType != 0, (*itPtr), "Data type error: " CAT name);
	return userType;
}

bool Semantics::inGlobal ()
{
	return currentContext == contexts[0];
}

Context* Semantics:: findContext (MSText* name)
{
	for (int32_t i=1; i<maxContexts; i++)
	{
		if (contexts[i] == 0) continue;
		if ((*name).match((*(*contexts[i]).variables.name))) return contexts[i];			
	}
	return 0;
}

bool Semantics:: isNameValidAndAvailable (const std::string & name) 
{
	MSText n (name);
	return isNameValidAndAvailable((&(n)));
}

bool Semantics:: isNameValidAndAvailable (MSText* name) 
{
	// check it has valid characters
	if (!Parser::isValidName(name)) {
		return false;
	}
	
	if((*name).numBytes() >= globalConfig.maxNameLength) {
		ERROR_PRINT("name is too long, max length: " CAT (globalConfig.maxNameLength) CAT " name: " CAT name);
		return false;
	}
	// return true if not reserved, otherwise print error message and return false
	
	if(findContext(name) != 0) {
		ERROR_PRINT("unexpected function name: " CAT name);
		return false;
	}
	if((types.find( (*name)) != types.end())) {
		ERROR_PRINT("unexpected type name: " CAT name);
		return false;
	}
	if(((*globalContext).variables.memberNames.find( (*name)) != (*globalContext).variables.memberNames.end())) {
		ERROR_PRINT("duplicate variable name: " CAT name);
		return false;
	}	
	if (currentContext != globalContext)
	{
		if(((*currentContext).variables.memberNames.find( (*name)) != (*currentContext).variables.memberNames.end())) {
			ERROR_PRINT("duplicate variable name: " CAT name);
			return false;
		}
	}
	for(int32_t i=0; i<NUM_KEYWORDS; i++)
	{
		if ((*name).match(keywords[i])) {
			ERROR_PRINT("unexpected keyword: " CAT name);
			return false;
		}
	}
	return true;
}

void Semantics::analyze (TokenTree* tree) 
{
	VERBOSE(HORIZONTAL_LINE);
	VERBOSE("SEMANTIC ANALYZE");
	VERBOSE(HORIZONTAL_LINE);

	currentContext = contexts[0];

	NodeIterator* it = new NodeIterator((*tree).root);
	
	analyzeNode((*it).copy()); // start from the first expression
	
	{ delete it; it = 0; };
	
	VERBOSE(HORIZONTAL_LINE);
	VERBOSE("CONTEXTS");
	VERBOSE(HORIZONTAL_LINE);
	for (int32_t i=0; i<maxContexts; i++)
	{
		if (contexts[i] != 0)
		{
			VERBOSE("-------- context ID: " CAT i);
			(*contexts[i]).variables.print();
		}
	}
	VERBOSE(HORIZONTAL_LINE);
	VERBOSE("END ANALYZING");
	VERBOSE(HORIZONTAL_LINE);

	print();
}

void Semantics::print ()
{
}

void Semantics::analyzeNode (NodeIterator it) 
{
	
	while (true)
	{
		if (it.type() == NT_EXPR)
		{
			if (!it.hasChild())
			{
				VERBOSE("<EMPTY EXPR>");
			}
			else
			{
				// make a new iterator for child
				it.toChild();
				analyzeExpr(it.copy());
				it.toParent();
			}
			
			if (!it.hasNext()) return;

			it.toNext();
		}
		else
		{
			ERROR("expression expected");
		}
	}
}

void Semantics::analyzeExpr (NodeIterator it) 
{
	if (globalConfig.verboseOn()) it.printTree(false);

	if (it.type() == NT_NAME_TOKEN)
	{
		Context* context = findContext(it.data());
		
		if (context != 0)
		{
			VERBOSE("-------- function call!!!");
		}
		else if ((*it.data()).match(keywords[KEYWORD_FUNC_ID]))
		{
			// get return type

			SYNTAX(it.hasNext(), it, "function return type expected");
			it.toNext();
			SYNTAX(hasType(it.data()), it, "unknown return type");
			int32_t returnType = nameTreeGet(types, it.data());

			// function name

			SYNTAX(it.hasNext(), it, "function name expected");
			it.toNext();
			SYNTAX(it.type() == NT_NAME_TOKEN, it, "function name expected");
			MSText* functionName = it.data();

			SYNTAX(isNameValidAndAvailable(functionName), it, "variable name error");
			
			VERBOSE("Create a new function: " CAT functionName);
			
			// create new context
			Context* funcContext = new Context(functionName, numContexts, returnType);
			contexts[numContexts++] = funcContext;

			// argument definition
			
			SYNTAX(it.hasNext(), it, "argument definition expected");
			it.toNext();
			createStructDef((*funcContext).variables, it.copy());
			(*funcContext).variables.argsSize = (*funcContext).variables.structSize;
			(*funcContext).numArgs = (*funcContext).variables.numMembers;
			
			// parse function body
			
			SYNTAX(it.hasNext(), it, "function body expected");
			it.toNext();
			SYNTAX(it.type() == NT_CODE_BLOCK, it, "code block expected");
			
			// save node to Context for Generator
			(*funcContext).codeNode = it.node;
			
			it.toChild();
			VERBOSE("-------- ANALYZE FUNCTION");
			currentContext = funcContext;
			analyzeNode(it.copy());
			currentContext = contexts[0];
			VERBOSE("-------- END ANALYZE FUNCTION");
			it.toParent();

			SYNTAX(!it.hasNext(), it, "unexpected token after code block");
		}
		else if ((*it.data()).match(keywords[KEYWORD_STRUCT_ID]))
		{
			// e.g. "struct Vec [int x, INT y, INT z]"

			SYNTAX(it.hasNext(), it, "struct name expected");
			it.toNext();
			MSText* structName = it.data();
			SYNTAX(isNameValidAndAvailable(structName), it, "variable name error");
			SYNTAX(it.hasNext(), it, "struct definition expected");
			it.toNext();
			SYNTAX(!it.hasNext(), it, "unexpected token after struct definition");
			VERBOSE("Create a new struct: " CAT structName);
			addStructDef(structName, it.copy());
		}
		else if (hasType(it.data()))
		{
			// expr. starts with a type name, eg. "int foo" OR "person [5] players"
			
			int32_t type = nameTreeGet(types, it.data());
			ASSERT(type == MS_TYPE_INT || type == MS_TYPE_INT64 || type == MS_TYPE_FLOAT || type == MS_TYPE_FLOAT64 || type == MS_TYPE_BOOL || type == MS_TYPE_TEXT || type == MS_TYPE_CHARS || type >= MAX_MS_TYPES, "semantics: unknown type: " CAT type);

			it.toNext();
			
			if (type == MS_TYPE_CHARS)
			{
				// get number of chars, eg. "chars [12] name"
				SYNTAX(it.type() == NT_SQUARE_BRACKETS, it, "chars size expected");
				it.toChild();
				SYNTAX(!it.hasNext(), it, "only the chars size expected");
				it.toChild();
				SYNTAX(!it.hasNext(), it, "only the chars size expected");
				SYNTAX(it.type() == NT_NUMBER_TOKEN, it, "chars size (number) expected");
				
				// parse size and calculate array size
				
				int32_t charsSize = std::stoi((*it.data()).getString());
				
				it.toParent();
				it.toParent();
				
				it.toNext();
				SYNTAX(it.type() == NT_NAME_TOKEN, it, "name expected");
				SYNTAX(isNameValidAndAvailable(it.data()), it, "variable name error");
				
				StructDef* charsType = addCharsType(charsSize);
				
				(*currentContext).variables.addMember(this, it.data(), (*charsType).typeID);				
			}
			else if (it.type() == NT_SQUARE_BRACKETS)
			{
				// eg. "person [5] players"
				
				SYNTAX(inGlobal(), it, "no arrays in functions");
				
				// array size
				it.toChild();
				SYNTAX(!it.hasNext(), it, "only the array size expected");
				
				int32_t arraySize = -1;
				
				if (!it.hasChild())
				{
					// array size is not specified, so argument count decide it
					// eg. if "int [] numbers: 1, 2, 3" then size is 3
				}
				else
				{
					it.toChild();
					SYNTAX(!it.hasNext(), it, "array size expected");
					SYNTAX(it.type() == NT_NUMBER_TOKEN, it, "array size (number) expected");
					arraySize = std::stoi((*it.data()).getString());
					SYNTAX(arraySize > 0 && arraySize < globalConfig.maxArraySize, it, "invalid array size");
					it.toParent();
				}
				it.toParent();
				
				// array name
				it.toNext();
				SYNTAX(it.type() == NT_NAME_TOKEN, it, "name expected");
				MSText* varName = it.data();
				SYNTAX(isNameValidAndAvailable(varName), it, "variable name error");
				
				if (arraySize == -1)
				{
					it.toNext();
					ASSERT(it.type() == NT_ASSIGNMENT, "array assignment expected as the size is not defined");
					arraySize = it.numChildren();
					SYNTAX(arraySize > 0 && arraySize < globalConfig.maxArraySize, it, "invalid array size");
				}
				
				VERBOSE("New array: " CAT varName CAT ", size " CAT arraySize);

				(*currentContext).variables.addArray(this, varName, type, arraySize);
			}
			else
			{
				// variable name
				SYNTAX(isNameValidAndAvailable(it.data()), it, "variable name error");
				VERBOSE("New variable: " CAT it.data() CAT " <" CAT (*currentContext).variables.name CAT ">");
				(*currentContext).variables.addMember(this, it.data(), type);
			}
		}
	}
	else
	{
		CHECK(false, EC_PARSE, "unexpected token");
	}
}

void Semantics:: addStructDef (MSText* name, NodeIterator it) 
{
	int32_t id = typeIDCounter++;
	

	StructDef* sd = new StructDef(name, id);
	createStructDef((*sd), it.copy());

	(*sd).print();

	addStructDef(name, id, sd);
}

void Semantics:: addStructDef (MSText* name, int32_t id, StructDef* sd) 
{
	ASSERT(!(types.find((*name)) != types.end()) && typeStructDefs[(*sd).typeID] == 0, "addStructDef: type ID reserved");
	types.insert(std::make_pair( MSText((*name)), (int32_t)(id & VALUE_TYPE_MASK)));;
	typeStructDefs[(*sd).typeID] = sd;
}

void Semantics::createStructDef (StructDef & sd, NodeIterator it) 
{	
	SYNTAX(it.type() == NT_SQUARE_BRACKETS, it, "struct definition expected");

	it.toChild();
	SYNTAX(it.type() == NT_EXPR, it, "exression expected");

	do
	{
		if (!it.hasChild()) continue; // skip an empty expression
		it.toChild();
		SYNTAX((types.find((*(it.data()))) != types.end()),it,  "createStructDef: unknown type: " CAT it.data());
		int32_t type = nameTreeGet(types, (it.data()));
		it.toNext();

		if (type == MS_TYPE_CHARS)
		{
			// get number of chars, eg. "chars [12] name"
			SYNTAX(it.type() == NT_SQUARE_BRACKETS, it, "chars size expected");
			it.toChild();
			SYNTAX(!it.hasNext(), it, "only the chars size expected");
			it.toChild();
			SYNTAX(!it.hasNext(), it, "only the chars size expected");
			SYNTAX(it.type() == NT_NUMBER_TOKEN, it, "chars size (number) expected");
			
			// parse size and calculate array size
			
			int32_t charsSize = std::stoi((*it.data()).getString());
			
			it.toParent();
			it.toParent();
			
			it.toNext();
			SYNTAX(isNameValidAndAvailable(it.data()), it, "variable name error");
			
			StructDef* charsType = addCharsType(charsSize);
			
			sd.addMember(this, it.data(), (*charsType).typeID);				
		}
		else if (it.type() == NT_SQUARE_BRACKETS)
		{
			// eg. "int [5] numbers"

			// NOTE: almost same as when defining variables...

			// array size
			it.toChild();
			SYNTAX(!it.hasNext(), it, "array size expected");
			it.toChild();
			SYNTAX(!it.hasNext(),it,  "array size expected");
			SYNTAX(it.type() == NT_NUMBER_TOKEN, it, "array size (number) expected");
			int32_t arraySize = std::stoi((*it.data()).getString());
			it.toParent();
			it.toParent();
			
			// array name
			it.toNext();
			VERBOSE("Member array: " CAT it.data() CAT ", size" CAT arraySize);
			
			sd.addArray(this, it.data(), type, arraySize);
		}
		else
		{
			SYNTAX(it.type() == NT_NAME_TOKEN,it,  "member name expected");
			SYNTAX(!(sd.memberNames.find( (*(it.data()))) != sd.memberNames.end()),it,  "duplicate name: " CAT it.data());
			VERBOSE("Add struct member: " CAT it.data());
			sd.addMember(this, it.data(), type);
		}
		SYNTAX(!it.hasNext(), it, "break expected");
		
		it.toParent();
	}
	while(it.toNextOrFalse());
}

void Semantics::writeStructDefs (ByteCode* bc)
{
	// write globals
	StructDef* sd = (&((*contexts[0]).variables));
	for (int32_t a=0; a<(*sd).codeTop; a++)
	{
		(*bc).addWord((*sd).code[a]);
	}
	
	// write user struct definitions to code
	for (int32_t i=MAX_MS_TYPES; i<MAX_TYPES; i++)
	{
		if (typeStructDefs[i] == 0) continue;
		sd = typeStructDefs[i];
		for (int32_t a=0; a<(*sd).codeTop; a++)
		{
			(*bc).addWord((*sd).code[a]);
		}
	}
}


} // namespace meanscript(core)
