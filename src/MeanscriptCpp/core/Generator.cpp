
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

Generator::Generator (TokenTree* _tree, Semantics & _sem, Common & _common)
	: sem(_sem), common(_common), bcPtr(new ByteCode((&(_common)))), bc(*bcPtr) {
	tree = _tree;
	currentContext = 0;
}

Generator::~Generator() { /* don't delete bcPtr as it will be passed on */ }

bool Generator::inGlobal ()
{
	return currentContext == sem.contexts[0];
}

ByteCode* Generator:: generate (TokenTree* _tree, Semantics & _sem, Common & _common) 
{
	Generator gen(_tree, _sem, _common);
	gen.generate();
	return gen.bcPtr;
}

ByteCode* Generator::generate () 
{
	
	VERBOSE("------------------------ GENERATE GLOBAL CODE");
	
	currentContext = sem.contexts[0];
	
	// start
	bc.addInstructionWithData(OP_START_INIT, 1, BYTECODE_EXECUTABLE, (*tree).texts.size());
	
	// add texts in numeral order, id = 0, 1, 2, 3, ...
	int32_t numTexts = (*tree).texts.size();
	Array<std::string>textArray(numTexts);
	
	for (const auto&  entry :  (*tree).texts)
	{
		// (key, value) = (text, id)
		int32_t id = entry.second;
		ASSERT(id >= 0 && id < numTexts, "unexpected text ID");
		textArray[id] = entry.first.c_str();
	}
	// skip text 0: empty
	for (int32_t i=1; i<numTexts; i++)
	{
		bc.codeTop = addTextInstruction(textArray[i], OP_ADD_TEXT, bc.code, bc.codeTop);
	}
	
	// define structure types
	sem.writeStructDefs((&(bc)));
	
	// introduce functions
	for (int32_t i=0; i<sem.maxContexts; i++)
	{
		if (sem.contexts[i] != 0)
		{
			(*sem.contexts[i]).tagAddress = bc.codeTop;
			bc.addInstruction(OP_FUNCTION, 5, 0);
			bc.addWord((*sem.contexts[i]).functionID);
			bc.addWord(-1); // add start address later
			bc.addWord(-1); // add struct size later (temp. variables may be added)
			bc.addWord((*sem.contexts[i]).variables.argsSize);
			bc.addWord(-1); // add end address later...
			// ...where the 'go back' instruction is, or in the future
			// some local un-initialization or something.
			// 'return' takes you there.
		}
	}
	bc.addInstruction(OP_END_INIT, 0 , 0);
	
	currentContext = sem.contexts[0]; // = global
	(*currentContext).codeStartAddress = bc.codeTop;
	
	NodeIterator* it = new NodeIterator((*tree).root);
	
	generateCodeBlock((*it).copy());
	
	{ delete it; it = 0; };
	
	(*currentContext).codeEndAddress = bc.codeTop;
	bc.addInstruction(OP_GO_BACK, 0 , 0); // end of global code
	
	for (int32_t i=1; i<sem.maxContexts; i++)
	{
		if (sem.contexts[i] != 0)
		{
			VERBOSE("------------------------ GENERATE FUNCTION CODE");
			currentContext = sem.contexts[i];
			NodeIterator* iter = new NodeIterator((*currentContext).codeNode);
			generateFunctionCode((*iter).copy());
			{ delete iter; iter = 0; };
		}
	}
	VERBOSE("------------------------ write code addresses");
	
	for (int32_t i=0; i<sem.maxContexts; i++)
	{
		if (sem.contexts[i] != 0)
		{
			bc.code[((*sem.contexts[i]).tagAddress) + 2] = (*sem.contexts[i]).codeStartAddress;
			bc.code[((*sem.contexts[i]).tagAddress) + 3] = (*sem.contexts[i]).variables.structSize;
			bc.code[((*sem.contexts[i]).tagAddress) + 5] = (*sem.contexts[i]).codeEndAddress;
		}
	}
	VERBOSE("------------------------ END GENERATION");
	return (&(bc));
}

void Generator::generateFunctionCode (NodeIterator it) 
{
	it.toChild();
	(*currentContext).codeStartAddress = bc.codeTop;
	generateCodeBlock(it);
	(*currentContext).codeEndAddress = bc.codeTop;
	bc.addInstruction(OP_GO_BACK, 0, 0);
}

void Generator::generateCodeBlock (NodeIterator it) 
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
				generateExpression(it.copy());
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


void Generator::generateExpression (NodeIterator it) 
{
	VERBOSE("------------ read expr ------------");
	if (globalConfig.verboseOn()) it.printTree(false);

	if (it.type() == NT_NAME_TOKEN)
	{
		Context* context = sem.findContext(it.data());
		
		if (context != 0)
		{
			generateFunctionCall(it, context);
		}
		else if ((common.callbackIDs.find( it.data()) != common.callbackIDs.end()))
		{
			generateCallbackCall(it);
		}
		else if(((*currentContext).variables.memberNames.find( it.data()) != (*currentContext).variables.memberNames.end()))
		{
			generateAssignment(it);
		}
		else if (sem.hasType(it.data()))
		{
			VERBOSE("Initialize a variable");
			it.toNext();
			
			if (it.type() == NT_SQUARE_BRACKETS)
			{
				// eg. "int [] numbers" or "person [5] team"
				it.toNext();
			}

			SYNTAX(((*currentContext).variables.memberNames.find( it.data()) != (*currentContext).variables.memberNames.end()), it, "unknown variable: " CAT it.data());
			if (it.hasNext()) generateAssignment(it);
		}
		else if ((compare(it.data(), keywords[KEYWORD_RETURN_ID])))
		{
			VERBOSE("Generate a return call");
			SYNTAX(it.hasNext(),it,  "'return' is missing a value"); // TODO: return from a void context
			it.toNext();
			SYNTAX(!it.hasNext(),it,  "'return' can take only one value");
			SYNTAX((*currentContext).returnType >= 0,it,  "can't return");
			
			// TODO: return value could be an array, a reference, etc.
			singleArgumentPush(OP_STRUCT_MEMBER | (*currentContext).returnType, it, -1);
			
			bc.addInstruction(OP_POP_STACK_TO_REG, 1, (*currentContext).returnType);
			bc.addWord((*sem.getType((*currentContext).returnType, (&(it)))).structSize);
			bc.addInstruction(OP_GO_END, 0 , 0);
		}
		else if ((compare(it.data(), keywords[KEYWORD_STRUCT_ID])))
		{
			VERBOSE("Skip a struct definition");
		}
		else if ((compare(it.data(), keywords[KEYWORD_FUNC_ID])))
		{
			VERBOSE("Skip a function definition for now");
		}
		else
		{
			SYNTAX(false, it, "unknown word: " CAT it.data().c_str());
		}
	}
	else
	{
		SYNTAX(false, it, "unexpected token");
	}
}

void Generator::generateFunctionCall (NodeIterator it, Context* funcContext) 
{
	VERBOSE("Generate a function call");
		
	bc.addInstruction(OP_SAVE_BASE, 0 , 0);
	
	if ((*funcContext).numArgs != 0)
	{
		SYNTAX(it.hasNext(),it,  "function arguments expected");
		it.toNext();
		callArgumentPush(it.copy(), (&((*funcContext).variables)), (*funcContext).numArgs);
	}
	bc.addInstructionWithData(OP_FUNCTION_CALL, 1, 0, (*funcContext).functionID);
	bc.addInstruction(OP_LOAD_BASE, 0 , 0);
}

MCallback* Generator::generateCallbackCall (NodeIterator it) 
{
	int32_t callbackID = nameTreeGet(common.callbackIDs, it.data());
	MCallback* callback = common.callbacks[callbackID];
	VERBOSE("Callback call, id " CAT callbackID);
	it.toNext();
	callArgumentPush(it.copy(), (*callback).argStruct, (*(*callback).argStruct).numMembers);
	bc.addInstructionWithData(OP_CALLBACK_CALL, 1, 0, callbackID);
	return callback;
}

void Generator::generateAssignment(NodeIterator it) 
{
	// e.g. "int a:5" or "a:6"
	
	VERBOSE("Add value assinging instructions");
		
	// get assignment target 
	
	VarGen target = resolveMember(it);
	
	it.toNext();
	ASSERT(it.type() == NT_ASSIGNMENT, "assignment struct expected");

	if (((int32_t)(target.tag & OPERATION_MASK)) == OP_ARRAY_MEMBER)
	{
		// assign array
		SYNTAX(target.arraySize == it.numChildren(), it, "wrong number of arguments in array assignment");
		SYNTAX(!target.isReference, it, "array reference can't be assigned");
		
		// assign children i.e. array items
		
		it.toChild();
		
		int32_t arrayDataSize = arrayPush(it, target.tag, target.arraySize);
		
		bc.addInstruction(inGlobal()?OP_POP_STACK_TO_GLOBAL:OP_POP_STACK_TO_LOCAL, 2, MS_TYPE_VOID);
		bc.addWord(arrayDataSize);
		bc.addWord(target.address);

		it.toParent();
		
		return;
	}

	int32_t targetType = (int32_t)(target.tag & VALUE_TYPE_MASK);
	StructDef* typeSD = sem.getType(targetType, (&(it)));

	// get value for assignment target

	it.toChild();
	if (it.hasNext())
	{
		// list of arguments to assign
		argumentStructPush(it.copy(), typeSD, (*typeSD).numMembers, true);
	}
	else
	{
		int32_t numItems = -1;
		if ((*typeSD).isCharsDef())
		{
			numItems = (*typeSD).numCharsForCharsDef();
		}
		NodeIterator cp = NodeIterator(it);
		singleArgumentPush(target.tag, cp, numItems);
	}

	// WRITE values. This works like a callback call.
	// Actually here we could call overridden assignment callback for the type.

	// local or global?
	if (target.isReference)
	{
		bc.addInstruction(inGlobal()?OP_POP_STACK_TO_GLOBAL_REF:OP_POP_STACK_TO_LOCAL_REF, 2, MS_TYPE_VOID);
	}
	else
	{
		bc.addInstruction(inGlobal()?OP_POP_STACK_TO_GLOBAL:OP_POP_STACK_TO_LOCAL, 2, MS_TYPE_VOID);
	}
	bc.addWord((*sem.getType(targetType, (&(it)))).structSize);
	bc.addWord(target.address);
}

int32_t Generator::arrayPush (NodeIterator it, int32_t targetTag, int32_t arraySize) 
{
	ASSERT(((int32_t)(targetTag & OPERATION_MASK)) == OP_ARRAY_MEMBER, "array expected");
	StructDef* itemType = sem.getType((int32_t)(targetTag & VALUE_TYPE_MASK), (&(it)));
	int32_t itemSize = (*itemType).structSize;
	int32_t itemTag = makeInstruction(OP_STRUCT_MEMBER, 0, (int32_t)(targetTag & VALUE_TYPE_MASK));

	for (int32_t i=0; i<arraySize; i++)
	{
		it.toChild();
		NodeIterator cp = NodeIterator(it);
		singleArgumentPush(itemTag, cp, -1);
		it.toParent();
		if (it.hasNext()) it.toNext();
	}
	return arraySize * itemSize;
}

void Generator::squareBracketArgumentPush (NodeIterator it, StructDef* sd, int32_t numArgs) 
{
	VERBOSE("Assign struct values in square brackets");
	
	int32_t argIndex = 0;
	it.toChild();
	ASSERT(it.type() == NT_EXPR, "expression expected");
	ASSERT(it.hasChild(), "argument expected");

	do
	{
		it.toChild();
		SYNTAX(argIndex < numArgs, it,  "wrong number of arguments, expected " CAT numArgs);
		int32_t memberTag = (*sd).getMemberTag(argIndex);
		int32_t arrayItemCount = (*sd).getMemberArrayItemCountOrNegative(argIndex);
		StructDef* memberType = sem.getType((int32_t)(memberTag & VALUE_TYPE_MASK), (&(it)));
		if ((*memberType).isCharsDef())
		{
			ASSERT(arrayItemCount < 0, "chars is an array?");
			arrayItemCount = (*memberType).numCharsForCharsDef();
		}
		singleArgumentPush(memberTag, it, arrayItemCount);
		it.toParent();
		argIndex++;
	}
	while(it.toNextOrFalse());

	it.toParent();

	SYNTAX(!(it.hasNext()) && argIndex == numArgs,it,  "wrong number of arguments");
}

void Generator::callArgumentPush (NodeIterator it, StructDef* sd, int32_t numArgs) 
{
	if ((it.type() == NT_PARENTHESIS && !it.hasNext()))
	{
		// F2 (a1, a2)
		
		it.toChild();
		argumentStructPush(it, sd, numArgs, true);
	}
	else
	{
		// F1 a1
		// F2 a1 a2
		// F2 (F3 x) a2
		
		argumentStructPush(it, sd, numArgs, false);
	}
}

void Generator::argumentStructPush (NodeIterator it, StructDef* sd, int32_t numArgs, bool commaSeparated) 
{
	VERBOSE("Assign struct argument");

	// HANDLE BOTH CASES:
	// 1)		func arg1 arg2
	// 2)		func (arg1, arg2)
	
	int32_t argIndex = 0;
	do
	{		
		if (!commaSeparated)
		{
			SYNTAX(!isFunctionOrCallback(it.data()),it, "function arguments must be in brackets or comma-separated");
		}
		else
		{
			it.toChild(); // comma-separated are expressions
		}
		
		SYNTAX((*sd).indexInRange(argIndex), it, "too many arguments");
		int32_t memberTag = (*sd).getMemberTag(argIndex);
		int32_t arrayItemCount = (*sd).getMemberArrayItemCountOrNegative(argIndex);
		singleArgumentPush(memberTag, it, arrayItemCount);
		
		if (commaSeparated)
		{
			it.toParent();
		}
		
		argIndex++;
	}
	while(it.toNextOrFalse());

	SYNTAX(!(it.hasNext()) && argIndex == numArgs, it, "wrong number of arguments");
}

bool Generator::isFunctionOrCallback (std::string name)
{
	Context* context = sem.findContext(name);
	if (context == 0) return ((common.callbackIDs.find( name) != common.callbackIDs.end()));
	return true;
}

VarGen Generator::resolveMember (NodeIterator & it) 
{
	bool isReference = false;
	int32_t auxAddress = -1;
	int32_t lastOffsetCodeIndex = -1;
	int32_t arrayItemCount = -1;
	
	std::string data = it.data();
	
	StructDef* currentStruct = (&((*currentContext).variables));
	int32_t memberTag = (*currentStruct).getMemberTag(data);
	int32_t size = (*currentStruct).getMemberSize(data);
	int32_t srcAddress = (*currentStruct).getMemberAddress(data);

	if ((memberTag & OPERATION_MASK) == OP_ARRAY_MEMBER) {
		arrayItemCount = (*currentStruct).getMemberArrayItemCount(data);
	} else {
		arrayItemCount = -1;
	}

	while (true)
	{
		if (it.hasNext() && it.nextType() == NT_DOT)
		{
			// e.g. "age" in "group.person.age: 41"	
			it.toNext();
			SYNTAX(it.hasNext() && it.nextType() == NT_NAME_TOKEN, it, "name expected after a dot");
			it.toNext();

			StructDef* memberType = sem.getType((int32_t)(memberTag & VALUE_TYPE_MASK), (&(it)));
			memberTag = (*memberType).getMemberTag(it.data());
			
			currentStruct = memberType;
		
			if (isReference)
			{
				bc.code[lastOffsetCodeIndex] += (*memberType).getMemberAddress(it.data());
			}
			else
			{
				size = (*memberType).getMemberSize(it.data());
				srcAddress += (*memberType).getMemberAddress(it.data()); // offset for the value
			}
		}
		else if (it.hasNext() && it.nextType() == NT_SQUARE_BRACKETS)
		{
			// e.g. "numbers[4]"
			
			SYNTAX((memberTag & OPERATION_MASK) == OP_ARRAY_MEMBER, it, "array expected");
			
			if ((memberTag & OPERATION_MASK) == OP_ARRAY_MEMBER) {
				arrayItemCount = (*currentStruct).getMemberArrayItemCount(it.data());
			} else {
				arrayItemCount = -1;
			}
			it.toNext();
				
			// array index
			it.toChild();
			SYNTAX(!it.hasNext(), it, "array index expected");
			it.toChild();
			
			// get array item type
			StructDef* arrayItemType = sem.getType((int32_t)(memberTag & VALUE_TYPE_MASK), (&(it)));
			int32_t itemSize = (*arrayItemType).structSize;
			
			if (it.type() == NT_NUMBER_TOKEN)
			{
				SYNTAX(!it.hasNext(), it, "array index expected");
				
				// array index (number) expected");
				int32_t arrayIndex = std::stoi(it.data());
				// mul. size * index, and plus one as the array size is at [0]
				SYNTAX(arrayIndex >= 0 && arrayIndex < arrayItemCount, it, "index out of range: " CAT arrayIndex CAT " of " CAT arrayItemCount);
				size = itemSize;
			
				if (isReference)
				{
					bc.code[lastOffsetCodeIndex] += itemSize * arrayIndex;
				}
				else
				{
					srcAddress += itemSize * arrayIndex;
				}
			}
			else
			{
				// handle variable (or other expression) array index
				// push index value
				NodeIterator cp = NodeIterator(it);
				singleArgumentPush(OP_STRUCT_MEMBER | MS_TYPE_INT, cp, -1);

				if (auxAddress < 0)
				{
					// create a auxiliar variable
					std::string auxAddressName = "~";
					auxAddressName += (*currentContext).variables.structSize;
					auxAddress = (*currentStruct).addMember(auxAddressName, MS_TYPE_INT);
				}
				
				// write index value to variable
				
				bc.addInstruction(OP_MULTIPLY_GLOBAL_ARRAY_INDEX, 4, MS_TYPE_INT);
				bc.addWord(auxAddress);				// address to array index
				bc.addWord(itemSize);				// size of one array item
				
				lastOffsetCodeIndex = bc.codeTop;	// save the address to add offset later if needed
				
				if (isReference)
				{
					// tell MeanMachine that we want too add to the previous address as
					// this is not the first variable index of the chain,
					// e.g. "team[foo].position[bar]"
					bc.addWord(-1);					// tell MeanMachine that we want too add to the previous address
				}
				else
				{
					bc.addWord(srcAddress);			// address of the array data (size first)
				}
				bc.addWord(arrayItemCount);			// save item count to SYNTAX for array out-of-bounds
				
				// index will be save in the auxAddress now and it won't be changed here now.
				// instead add ADD operations to change the index, e.g. in "arr[foo].bar" would "bar" do.
				srcAddress = auxAddress;
				
				isReference = true;
				size = 1; // address size
			}
			
			// change the current struct
			currentStruct = arrayItemType;
			
			// current member is not an array, so change the tag
			// TODO: needs some new design and refactoring
			memberTag = makeInstruction(OP_STRUCT_MEMBER, 0, (*arrayItemType).typeID);
			
			it.toParent();
			it.toParent();
		}
		else break;
	}
	
	if (isReference) { ASSERT(size == 1, ""); }		
				
	return VarGen (size, memberTag, srcAddress, arrayItemCount, isReference);
}

void Generator::singleArgumentPush (int32_t targetTag, NodeIterator & it, int32_t arrayItemCount) 
{
	VERBOSE("Assign an argument [" CAT it.data() CAT "]");
	
	int32_t targetType = (int32_t)(targetTag & VALUE_TYPE_MASK);
	
	ASSERT((targetTag & OPERATION_MASK) == OP_STRUCT_MEMBER || (targetTag & OPERATION_MASK) == OP_ARRAY_MEMBER, "invalid tag");
	
	if (it.type() == NT_EXPR)
	{
		SYNTAX(!it.hasNext(), it, "argument syntax error");
		it.toChild();
		NodeIterator cp = NodeIterator(it);
		singleArgumentPush(targetTag, cp, arrayItemCount);
		it.toParent();
		return;
	}
	
	if (it.type() == NT_NUMBER_TOKEN)
	{
		if (targetType == MS_TYPE_INT)
		{
			int32_t number = std::stoi(it.data());
			bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_INT, number);
			return;
		}
		else 
		{
			SYNTAX(targetType == MS_TYPE_FLOAT, it, "number expected");
			float f = std::stof(it.data());
			int32_t floatToInt = floatToIntBits(f);
			bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_FLOAT, floatToInt);
			return;
		}
	}
	else if (it.type() == NT_TEXT)
	{
		ASSERT(((*tree).texts.find( it.data()) != (*tree).texts.end()), "text not found");
		int32_t textID = nameTreeGet((*tree).texts, it.data());
		if (targetType == MS_TYPE_TEXT)
		{
			// assign text id
			bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_TEXT, textID);
		}
		else
		{
			// copy chars
			int32_t maxChars = arrayItemCount;
			StructDef* sd = sem.typeStructDefs[targetType];
			bc.addInstructionWithData(OP_PUSH_CHARS, 3, MS_TYPE_TEXT, textID);
			bc.addWord(maxChars);
			bc.addWord((*sd).structSize);
		}
		return;
	}
	else if (it.type() == NT_NAME_TOKEN)
	{
		Context* functionContext = sem.findContext(it.data());
		if (functionContext != 0)
		{
			// PUSH A FUNCTION ARGUMENT
			
			generateFunctionCall(it.copy(), functionContext);
			StructDef* returnData = sem.getType((*functionContext).returnType, (&(it)));
			SYNTAX(targetType == (*returnData).typeID, it, "type mismatch");
			bc.addInstructionWithData(OP_PUSH_REG_TO_STACK, 1, MS_TYPE_VOID, (*returnData).structSize);
			return;
		}
		else if ((common.callbackIDs.find( it.data()) != common.callbackIDs.end()))
		{
			
			// PUSH A CALLBACK ARGUMENT
			
			MCallback* callback = generateCallbackCall(it.copy());
			StructDef* returnData = sem.getType((*callback).returnType, (&(it)));
			SYNTAX(targetType == (*returnData).typeID, it, "type mismatch");
			bc.addInstructionWithData(OP_PUSH_REG_TO_STACK, 1, MS_TYPE_VOID, (*returnData).structSize);
			return;
		}
		else 
		{
			// PUSH A VARIABLE ARGUMENT
			
			VarGen vg = resolveMember(it);

			// write the address or its reference from where to push

			if (vg.isReference)
			{
				bc.addInstruction(inGlobal() ? OP_PUSH_GLOBAL_REF : OP_PUSH_LOCAL_REF, 2, MS_TYPE_INT);
			}
			else
			{
				bc.addInstruction(inGlobal() ? OP_PUSH_GLOBAL : OP_PUSH_LOCAL, 2, MS_TYPE_INT);
			}
			
			bc.addWord(vg.address);
			bc.addWord(vg.size);
			
			int32_t resolvedType = (int32_t)(vg.tag & VALUE_TYPE_MASK);
			if (targetType == MS_TYPE_CHARS)
			{
				StructDef* sd = sem.typeStructDefs[resolvedType];
				SYNTAX((*sd).isCharsDef(), it, "chars type expected");
			}
			else
			{
				SYNTAX(resolvedType == targetType, it, "type mismatch");
			}
			return;
		}
	}
	else if (it.type() == NT_PARENTHESIS)
	{
		it.toChild();
		NodeIterator cp = NodeIterator(it);
		singleArgumentPush(targetTag, cp, arrayItemCount);
		it.toParent();
		return;
	}
	else if (it.type() == NT_REFERENCE_TOKEN)
	{
		// TODO: SYNTAX type
		int32_t memberTag = (*currentContext).variables.getMemberTag(it.data());
		int32_t address = (*currentContext).variables.getMemberAddress(it.data());
		int32_t memberType = (int32_t)(memberTag & VALUE_TYPE_MASK);
		bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, memberType, address);
		return;
	}
	else if (it.type() == NT_CODE_BLOCK)
	{
		bc.addInstructionWithData(OP_JUMP,1,MS_TYPE_CODE_ADDRESS,-1); // -1 to be replaced after code block is done
		int32_t jumpAddressPosition = bc.codeTop -1;
		int32_t blockAddress = bc.codeTop;
			
		bc.addInstruction(OP_NOOP,0,0); // no-op in case that the block is empty

		// generate code block
		
		it.toChild();
		ASSERT(it.type() == NT_EXPR, "expression expected");
		VERBOSE(" - - - - start generating code block");
		generateCodeBlock(it);
		bc.addInstruction(OP_GO_BACK, 0 , 0);
		VERBOSE(" - - - - end generating code block");
		it.toParent();

		bc.code[jumpAddressPosition] = bc.codeTop;
		bc.addInstruction(OP_NOOP,0,0);
			
		bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_CODE_ADDRESS, blockAddress); // push the argument: block address
		return;
	}
	else if (it.type() == NT_SQUARE_BRACKETS)
	{
		if (((int32_t)(targetTag & OPERATION_MASK)) == OP_ARRAY_MEMBER)
		{
			// push a list of array values
			
			it.toChild();
			arrayPush(it.copy(), targetTag, arrayItemCount);
			it.toParent();
			return;
		}
		else
		{
			// push array values
			
			StructDef* sd = sem.typeStructDefs[targetType];
			SYNTAX(sd != 0, it, "generator: unknown type: " CAT targetType);
			squareBracketArgumentPush(it.copy(), sd, (*sd).numMembers);
			return;
		}
	}
	else
	{
		SYNTAX(false, it, "argument error");
	}
}


} // namespace meanscript(core)
// C++ END
