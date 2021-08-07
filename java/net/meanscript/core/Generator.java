package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class Generator extends MC {
Context currentContext;
TokenTree tree;
Semantics sem;
Common common;
ByteCode bcPtr;
ByteCode bc;

public Generator (TokenTree _tree, Semantics _sem, Common _common)
{
	sem = _sem;
	common = _common;
	bc = new ByteCode(common);
	tree = _tree;
	currentContext = null;
}

//

public boolean inGlobal ()
{
	return currentContext == sem.contexts[0];
}

public static ByteCode  generate (TokenTree _tree, Semantics _sem, Common _common) throws MException
{
	Generator gen = new Generator(_tree, _sem, _common);
	gen.generate();
	return gen.bc;
}

public ByteCode generate () throws MException
{
	
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("------------------------ GENERATE GLOBAL CODE").endLine();};
	
	currentContext = sem.contexts[0];
	
	// start
	bc.addInstructionWithData(OP_START_INIT, 1, BYTECODE_EXECUTABLE, tree.texts.size());
	
	// add texts in numeral order, id = 0, 1, 2, 3, ...
	int numTexts = tree.texts.size();
	 MSText[] textArray = new  MSText[numTexts];
	
	for(java.util.Map.Entry<MSText,Integer>  entry :  tree.texts.entrySet())
	{
		// (key, value) = (text, id)
		int id = entry.getValue();
		MSJava.assertion(id >= 0 && id < numTexts,   "unexpected text ID");
		textArray[id] = entry.getKey();
	}
	// skip text 0: empty
	for (int i=1; i<numTexts; i++)
	{
		bc.codeTop = addTextInstruction(textArray[i], OP_ADD_TEXT, bc.code, bc.codeTop);
	}
	
	// define structure types
	sem.writeStructDefs(bc);
	
	// introduce functions
	for (int i=0; i<sem.maxContexts; i++)
	{
		if (sem.contexts[i] != null)
		{
			sem.contexts[i].tagAddress = bc.codeTop;
			bc.addInstruction(OP_FUNCTION, 5, 0);
			bc.addWord(sem.contexts[i].functionID);
			bc.addWord(-1); // add start address later
			bc.addWord(-1); // add struct size later (temp. variables may be added)
			bc.addWord(sem.contexts[i].variables.argsSize);
			bc.addWord(-1); // add end address later...
			// ...where the 'go back' instruction is, or in the future
			// some local un-initialization or something.
			// 'return' takes you there.
		}
	}
	bc.addInstruction(OP_END_INIT, 0 , 0);
	
	currentContext = sem.contexts[0]; // = global
	currentContext.codeStartAddress = bc.codeTop;
	
	NodeIterator it = new NodeIterator(tree.root);
	
	generateCodeBlock(it.copy());
	
	it = null;
	
	currentContext.codeEndAddress = bc.codeTop;
	bc.addInstruction(OP_GO_BACK, 0 , 0); // end of global code
	
	for (int i=1; i<sem.maxContexts; i++)
	{
		if (sem.contexts[i] != null)
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("------------------------ GENERATE FUNCTION CODE").endLine();};
			currentContext = sem.contexts[i];
			NodeIterator iter = new NodeIterator(currentContext.codeNode);
			generateFunctionCode(iter.copy());
			iter = null;
		}
	}
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("------------------------ write code addresses").endLine();};
	
	for (int i=0; i<sem.maxContexts; i++)
	{
		if (sem.contexts[i] != null)
		{
			bc.code[(sem.contexts[i].tagAddress) + 2] = sem.contexts[i].codeStartAddress;
			bc.code[(sem.contexts[i].tagAddress) + 3] = sem.contexts[i].variables.structSize;
			bc.code[(sem.contexts[i].tagAddress) + 5] = sem.contexts[i].codeEndAddress;
		}
	}
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("------------------------ END GENERATION").endLine();};
	return bc;
}

public void generateFunctionCode (NodeIterator it) throws MException
{
	it.toChild();
	currentContext.codeStartAddress = bc.codeTop;
	generateCodeBlock(it);
	currentContext.codeEndAddress = bc.codeTop;
	bc.addInstruction(OP_GO_BACK, 0, 0);
}

public void generateCodeBlock (NodeIterator it) throws MException
{
	while (true)
	{
		if (it.type() == NT_EXPR)
		{
			if (!it.hasChild())
			{
				{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("<EMPTY EXPR>").endLine();};
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
			MSJava.syntaxAssertion(false, it, "expression expected");
		}
	}
}


public void generateExpression (NodeIterator it) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("------------ read expr ------------").endLine();};
	if (MSJava.globalConfig.verboseOn()) it.printTree(false);

	if (it.type() == NT_NAME_TOKEN)
	{
		Context context = sem.findContext(it.data());
		
		if (context != null)
		{
			generateFunctionCall(it, context);
		}
		else if ((common.callbackIDs.containsKey( it.data())))
		{
			generateCallbackCall(it);
		}
		else if((currentContext.variables.memberNames.containsKey( it.data())))
		{
			generateAssignment(it);
		}
		else if (sem.hasType(it.data()))
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Initialize a variable").endLine();};
			it.toNext();
			
			if (it.type() == NT_SQUARE_BRACKETS)
			{
				// eg. "int [] numbers" or "person [5] team"
				it.toNext();
			}

			MSJava.syntaxAssertion((currentContext.variables.memberNames.containsKey( it.data())), it, "unknown variable: " + it.data());
			if (it.hasNext()) generateAssignment(it);
		}
		else if ((it.data()).match(keywords[KEYWORD_RETURN_ID]))
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Generate a return call").endLine();};
			MSJava.syntaxAssertion(it.hasNext(),it,  "'return' is missing a value"); // TODO: return from a void context
			it.toNext();
			MSJava.syntaxAssertion(!it.hasNext(),it,  "'return' can take only one value");
			MSJava.syntaxAssertion(currentContext.returnType >= 0,it,  "can't return");
			
			// TODO: return value could be an array, a reference, etc.
			singleArgumentPush(OP_STRUCT_MEMBER | currentContext.returnType, it, -1);
			
			bc.addInstruction(OP_POP_STACK_TO_REG, 1, currentContext.returnType);
			bc.addWord(sem.getType(currentContext.returnType, it).structSize);
			bc.addInstruction(OP_GO_END, 0 , 0);
		}
		else if ((it.data()).match(keywords[KEYWORD_STRUCT_ID]))
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Skip a struct definition").endLine();};
		}
		else if ((it.data()).match(keywords[KEYWORD_FUNC_ID]))
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Skip a function definition for now").endLine();};
		}
		else
		{
			MSJava.syntaxAssertion(false, it, "unknown word: " + it.data());
		}
	}
	else
	{
		MSJava.syntaxAssertion(false, it, "unexpected token");
	}
}

public void generateFunctionCall (NodeIterator it, Context funcContext) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Generate a function call").endLine();};
		
	bc.addInstruction(OP_SAVE_BASE, 0 , 0);
	
	if (funcContext.numArgs != 0)
	{
		MSJava.syntaxAssertion(it.hasNext(),it,  "function arguments expected");
		it.toNext();
		callArgumentPush(it.copy(), funcContext.variables, funcContext.numArgs);
	}
	bc.addInstructionWithData(OP_FUNCTION_CALL, 1, 0, funcContext.functionID);
	bc.addInstruction(OP_LOAD_BASE, 0 , 0);
}

public MCallback generateCallbackCall (NodeIterator it) throws MException
{
	int callbackID = common.callbackIDs.get( it.data());
	MCallback callback = common.callbacks[callbackID];
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Callback call, id " + callbackID).endLine();};
	if (callback.argStruct.numMembers > 0)
	{
		it.toNext();
		callArgumentPush(it.copy(), callback.argStruct, callback.argStruct.numMembers);
	}
	bc.addInstructionWithData(OP_CALLBACK_CALL, 1, 0, callbackID);
	return callback;
}

public void generateAssignment(NodeIterator it) throws MException
{
	// e.g. "int a:5" or "a:6"
	
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Add value assinging instructions").endLine();};
		
	// get assignment target 
	
	VarGen target = resolveMember(it);
	
	it.toNext();
	MSJava.assertion(it.type() == NT_ASSIGNMENT,   "assignment struct expected");

	if (((int)(target.tag & OPERATION_MASK)) == OP_ARRAY_MEMBER)
	{
		// assign array
		MSJava.syntaxAssertion(target.arraySize == it.numChildren(), it, "wrong number of arguments in array assignment");
		MSJava.syntaxAssertion(!target.isReference, it, "array reference can't be assigned");
		
		// assign children i.e. array items
		
		it.toChild();
		
		int arrayDataSize = arrayPush(it, target.tag, target.arraySize);
		
		bc.addInstruction(inGlobal()?OP_POP_STACK_TO_GLOBAL:OP_POP_STACK_TO_LOCAL, 2, MS_TYPE_VOID);
		bc.addWord(arrayDataSize);
		bc.addWord(target.address);

		it.toParent();
		
		return;
	}

	int targetType = (int)(target.tag & VALUE_TYPE_MASK);
	StructDef typeSD = sem.getType(targetType, it);

	// get value for assignment target

	it.toChild();
	if (it.hasNext())
	{
		// list of arguments to assign
		argumentStructPush(it.copy(), typeSD, typeSD.numMembers, true);
	}
	else
	{
		int numItems = -1;
		if (typeSD.isCharsDef())
		{
			numItems = typeSD.numCharsForCharsDef();
		}
		NodeIterator cp = new NodeIterator(it);
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
	bc.addWord(sem.getType(targetType, it).structSize);
	bc.addWord(target.address);
}

public int arrayPush (NodeIterator it, int targetTag, int arraySize) throws MException
{
	MSJava.assertion(((int)(targetTag & OPERATION_MASK)) == OP_ARRAY_MEMBER,   "array expected");
	StructDef itemType = sem.getType((int)(targetTag & VALUE_TYPE_MASK), it);
	int itemSize = itemType.structSize;
	int itemTag = makeInstruction(OP_STRUCT_MEMBER, 0, (int)(targetTag & VALUE_TYPE_MASK));

	for (int i=0; i<arraySize; i++)
	{
		it.toChild();
		NodeIterator cp = new NodeIterator(it);
		singleArgumentPush(itemTag, cp, -1);
		it.toParent();
		if (it.hasNext()) it.toNext();
	}
	return arraySize * itemSize;
}

public void squareBracketArgumentPush (NodeIterator it, StructDef sd, int numArgs) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Assign struct values in square brackets").endLine();};
	
	int argIndex = 0;
	it.toChild();
	MSJava.assertion(it.type() == NT_EXPR,   "expression expected");
	MSJava.assertion(it.hasChild(),   "argument expected");

	do
	{
		it.toChild();
		MSJava.syntaxAssertion(argIndex < numArgs, it,  "wrong number of arguments, expected " + numArgs);
		int memberTag = sd.getMemberTag(argIndex);
		int arrayItemCount = sd.getMemberArrayItemCountOrNegative(argIndex);
		StructDef memberType = sem.getType((int)(memberTag & VALUE_TYPE_MASK), it);
		if (memberType.isCharsDef())
		{
			MSJava.assertion(arrayItemCount < 0,   "chars is an array?");
			arrayItemCount = memberType.numCharsForCharsDef();
		}
		singleArgumentPush(memberTag, it, arrayItemCount);
		it.toParent();
		argIndex++;
	}
	while(it.toNextOrFalse());

	it.toParent();

	MSJava.syntaxAssertion(!(it.hasNext()) && argIndex == numArgs,it,  "wrong number of arguments");
}

public void callArgumentPush (NodeIterator it, StructDef sd, int numArgs) throws MException
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

public void argumentStructPush (NodeIterator it, StructDef sd, int numArgs, boolean commaSeparated) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Assign struct argument").endLine();};

	// HANDLE BOTH CASES:
	// 1)		func arg1 arg2
	// 2)		func (arg1, arg2)
	
	int argIndex = 0;
	do
	{		
		if (!commaSeparated)
		{
			MSJava.syntaxAssertion(!isFunctionOrCallback(it.data()),it, "function arguments must be in brackets or comma-separated");
		}
		else
		{
			it.toChild(); // comma-separated are expressions
		}
		
		MSJava.syntaxAssertion(sd.indexInRange(argIndex), it, "too many arguments");
		int memberTag = sd.getMemberTag(argIndex);
		int arrayItemCount = sd.getMemberArrayItemCountOrNegative(argIndex);
		singleArgumentPush(memberTag, it, arrayItemCount);
		
		if (commaSeparated)
		{
			it.toParent();
		}
		
		argIndex++;
	}
	while(it.toNextOrFalse());

	MSJava.syntaxAssertion(!(it.hasNext()) && argIndex == numArgs, it, "wrong number of arguments");
}

public boolean isFunctionOrCallback (MSText name)
{
	Context context = sem.findContext(name);
	if (context == null) return ((common.callbackIDs.containsKey( name)));
	return true;
}

public VarGen resolveMember (NodeIterator it) throws MException
{
	boolean isReference = false;
	int auxAddress = -1;
	int lastOffsetCodeIndex = -1;
	int arrayItemCount = -1;
	
	MSText data = it.data();
	
	StructDef currentStruct = currentContext.variables;
	int memberTag = currentStruct.getMemberTag(data);
	int size = currentStruct.getMemberSize(data);
	int srcAddress = currentStruct.getMemberAddress(data);

	if ((memberTag & OPERATION_MASK) == OP_ARRAY_MEMBER) {
		arrayItemCount = currentStruct.getMemberArrayItemCount(data);
	} else {
		arrayItemCount = -1;
	}

	while (true)
	{
		if (it.hasNext() && it.nextType() == NT_DOT)
		{
			// e.g. "age" in "group.person.age: 41"	
			it.toNext();
			MSJava.syntaxAssertion(it.hasNext() && it.nextType() == NT_NAME_TOKEN, it, "name expected after a dot");
			it.toNext();

			StructDef memberType = sem.getType((int)(memberTag & VALUE_TYPE_MASK), it);
			memberTag = memberType.getMemberTag(it.data());
			
			currentStruct = memberType;
		
			if (isReference)
			{
				bc.code[lastOffsetCodeIndex] += memberType.getMemberAddress(it.data());
			}
			else
			{
				size = memberType.getMemberSize(it.data());
				srcAddress += memberType.getMemberAddress(it.data()); // offset for the value
			}
		}
		else if (it.hasNext() && it.nextType() == NT_SQUARE_BRACKETS)
		{
			// e.g. "numbers[4]"
			
			MSJava.syntaxAssertion((memberTag & OPERATION_MASK) == OP_ARRAY_MEMBER, it, "array expected");
			
			if ((memberTag & OPERATION_MASK) == OP_ARRAY_MEMBER) {
				arrayItemCount = currentStruct.getMemberArrayItemCount(it.data());
			} else {
				arrayItemCount = -1;
			}
			it.toNext();
				
			// array index
			it.toChild();
			MSJava.syntaxAssertion(!it.hasNext(), it, "array index expected");
			it.toChild();
			
			// get array item type
			StructDef arrayItemType = sem.getType((int)(memberTag & VALUE_TYPE_MASK), it);
			int itemSize = arrayItemType.structSize;
			
			if (it.type() == NT_NUMBER_TOKEN)
			{
				MSJava.syntaxAssertion(!it.hasNext(), it, "array index expected");
				
				// array index (number) expected");
				int arrayIndex = MSJava.parseInt32(it.data().getString());
				// mul. size * index, and plus one as the array size is at [0]
				MSJava.syntaxAssertion(arrayIndex >= 0 && arrayIndex < arrayItemCount, it, "index out of range: " + arrayIndex + " of " + arrayItemCount);
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
				NodeIterator cp = new NodeIterator(it);
				singleArgumentPush(OP_STRUCT_MEMBER | MS_TYPE_INT, cp, -1);

				if (auxAddress < 0)
				{
					// create a auxiliar variable
					String auxAddressName = "~";
					auxAddressName += currentContext.variables.structSize;
					MSText tmp = new MSText (auxAddressName);
					auxAddress = currentStruct.addMember(tmp, MS_TYPE_INT, 1);
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
			memberTag = makeInstruction(OP_STRUCT_MEMBER, 0, arrayItemType.typeID);
			
			it.toParent();
			it.toParent();
		}
		else break;
	}
	
	if (isReference) { MSJava.assertion(size == 1,   ""); }		
				
	return new VarGen (size, memberTag, srcAddress, arrayItemCount, isReference);
}

public void singleArgumentPush (int targetTag, NodeIterator it, int arrayItemCount) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Assign an argument [" + it.data() + "]").endLine();};
	
	int targetType = (int)(targetTag & VALUE_TYPE_MASK);
	
	MSJava.assertion((targetTag & OPERATION_MASK) == OP_STRUCT_MEMBER || (targetTag & OPERATION_MASK) == OP_ARRAY_MEMBER,   "invalid tag");
	
	if (it.type() == NT_EXPR)
	{
		MSJava.syntaxAssertion(!it.hasNext(), it, "argument syntax error");
		it.toChild();
		NodeIterator cp = new NodeIterator(it);
		singleArgumentPush(targetTag, cp, arrayItemCount);
		it.toParent();
		return;
	}
	
	if (it.type() == NT_HEX_TOKEN)
	{
		if (targetType == MS_TYPE_INT)
		{
			long number = parseHex((it.data()).getString(), 8);
			bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_INT, int64lowBits(number));
			return;
		}
		else if (targetType == MS_TYPE_INT64)
		{
			long number = parseHex((it.data()).getString(), 16);
			bc.addInstruction(OP_PUSH_IMMEDIATE, 2, MS_TYPE_INT64);
			bc.addWord(int64highBits(number));
			bc.addWord(int64lowBits(number));
			return;
		}
		else
		{
			MSJava.syntaxAssertion(false, it, "number error");
		}
	}
	else if (it.type() == NT_NUMBER_TOKEN)
	{
		if (targetType == MS_TYPE_INT)
		{
			int number = MSJava.parseInt32((it.data()).getString());
			bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_INT, number);
			return;
		}
		else if (targetType == MS_TYPE_INT64)
		{
			long number = MSJava.parseInt64((it.data()).getString());
			bc.addInstruction(OP_PUSH_IMMEDIATE, 2, MS_TYPE_INT64);
			bc.addWord(int64highBits(number));
			bc.addWord(int64lowBits(number));
			return;
		}
		else if (targetType == MS_TYPE_FLOAT)
		{
			float f = MSJava.parseFloat32(it.data().getString());
			int floatToInt = MSJava.floatToIntFormat(f);
			bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_FLOAT, floatToInt);
			return;
		}
		else if (targetType == MS_TYPE_FLOAT64)
		{
			double f = MSJava.parseFloat64((it.data()).getString());
			long number = MSJava.float64ToInt64Format(f);
			bc.addInstruction(OP_PUSH_IMMEDIATE, 2, MS_TYPE_FLOAT64);
			bc.addWord(int64highBits(number));
			bc.addWord(int64lowBits(number));
			return;
		}
		else
		{
			MSJava.syntaxAssertion(false, it, "number error");
		}
	}
	else if (it.type() == NT_TEXT)
	{
		MSJava.assertion((tree.texts.containsKey( it.data())),   "text not found");
		int textID = tree.texts.get( it.data());
		if (targetType == MS_TYPE_TEXT)
		{
			// assign text id
			bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_TEXT, textID);
		}
		else
		{
			// copy chars
			int maxChars = arrayItemCount;
			StructDef sd = sem.typeStructDefs[targetType];
			bc.addInstructionWithData(OP_PUSH_CHARS, 3, MS_TYPE_TEXT, textID);
			bc.addWord(maxChars);
			bc.addWord(sd.structSize);
		}
		return;
	}
	else if (it.type() == NT_NAME_TOKEN)
	{
		Context functionContext = sem.findContext(it.data());
		if (functionContext != null)
		{
			// PUSH A FUNCTION ARGUMENT
			
			generateFunctionCall(it.copy(), functionContext);
			StructDef returnData = sem.getType(functionContext.returnType, it);
			MSJava.syntaxAssertion(targetType == returnData.typeID, it, "type mismatch");
			bc.addInstructionWithData(OP_PUSH_REG_TO_STACK, 1, MS_TYPE_VOID, returnData.structSize);
			return;
		}
		else if ((common.callbackIDs.containsKey( it.data())))
		{
			
			// PUSH A CALLBACK ARGUMENT
			
			MCallback callback = generateCallbackCall(it.copy());
			StructDef returnData = sem.getType(callback.returnType, it);
			MSJava.syntaxAssertion(targetType == returnData.typeID, it, "type mismatch");
			bc.addInstructionWithData(OP_PUSH_REG_TO_STACK, 1, MS_TYPE_VOID, returnData.structSize);
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
			
			int resolvedType = (int)(vg.tag & VALUE_TYPE_MASK);
			if (targetType == MS_TYPE_CHARS)
			{
				StructDef sd = sem.typeStructDefs[resolvedType];
				MSJava.syntaxAssertion(sd.isCharsDef(), it, "chars type expected");
			}
			else
			{
				MSJava.syntaxAssertion(resolvedType == targetType, it, "type mismatch");
			}
			return;
		}
	}
	else if (it.type() == NT_PARENTHESIS)
	{
		it.toChild();
		NodeIterator cp = new NodeIterator(it);
		singleArgumentPush(targetTag, cp, arrayItemCount);
		it.toParent();
		return;
	}
	else if (it.type() == NT_REFERENCE_TOKEN)
	{
		// TODO: SYNTAX type
		int memberTag = currentContext.variables.getMemberTag(it.data());
		int address = currentContext.variables.getMemberAddress(it.data());
		int memberType = (int)(memberTag & VALUE_TYPE_MASK);
		bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, memberType, address);
		return;
	}
	else if (it.type() == NT_CODE_BLOCK)
	{
		bc.addInstructionWithData(OP_JUMP,1,MS_TYPE_CODE_ADDRESS,-1); // -1 to be replaced after code block is done
		int jumpAddressPosition = bc.codeTop -1;
		int blockAddress = bc.codeTop;
			
		bc.addInstruction(OP_NOOP,0,0); // no-op in case that the block is empty

		// generate code block
		
		it.toChild();
		MSJava.assertion(it.type() == NT_EXPR,   "expression expected");
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(" - - - - start generating code block").endLine();};
		generateCodeBlock(it);
		bc.addInstruction(OP_GO_BACK, 0 , 0);
		{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(" - - - - end generating code block").endLine();};
		it.toParent();

		bc.code[jumpAddressPosition] = bc.codeTop;
		bc.addInstruction(OP_NOOP,0,0);
			
		bc.addInstructionWithData(OP_PUSH_IMMEDIATE, 1, MS_TYPE_CODE_ADDRESS, blockAddress); // push the argument: block address
		return;
	}
	else if (it.type() == NT_SQUARE_BRACKETS)
	{
		if (((int)(targetTag & OPERATION_MASK)) == OP_ARRAY_MEMBER)
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
			
			StructDef sd = sem.typeStructDefs[targetType];
			MSJava.syntaxAssertion(sd != null, it, "generator: unknown type: " + targetType);
			squareBracketArgumentPush(it.copy(), sd, sd.numMembers);
			return;
		}
	}
	else
	{
		MSJava.syntaxAssertion(false, it, "argument error");
	}
}


}
