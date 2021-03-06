package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;





public class Semantics extends MC {
public int typeIDCounter;
 int maxContexts;
 int numContexts;
 java.util.TreeMap<String, Integer> types = new java.util.TreeMap<String, Integer>();
 StructDef [] typeStructDefs;
 Context [] contexts;
public Context globalContext;
 Context currentContext;

public Semantics () throws MException
{
	typeIDCounter = MAX_MS_TYPES;

	typeStructDefs = new StructDef[MAX_TYPES];
	for (int i=0; i<MAX_TYPES; i++)
	{
		typeStructDefs[i] = null;
	}
	
	maxContexts = MSJava.globalConfig.maxFunctions;
	contexts = new Context[maxContexts];
	
	contexts[0] = new Context("global", 0, -1);
	globalContext = contexts[0];
	currentContext = globalContext;
	for (int i=1; i<maxContexts; i++)
	{
		contexts[i] = null;
	}
	numContexts=1;
}
//;

public void addPrimitiveType (String name, StructDef sd, int id)
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Add primitive type [" + id + "] " + name).endLine();};
	types.put( name, id);
	typeStructDefs[id] = sd;
	
}

public void addElementaryType (String name, int typeID, int size) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Add elementary type [" + typeID + "] " + name).endLine();};
	types.put( name, typeID);
	StructDef sd = new StructDef(name, typeID, size);
	typeStructDefs[typeID] = sd;
}

public StructDef addCharsType (int numChars) throws MException
{
	// words:
	//		0:		size in characters (start stringToIntsWithSize() from here)
	//		1...n:	characters + zero at the end, e.g. "mean\0" --> 2 words = (numChars / 4) + 1
	
	int arraySize = (numChars / 4) + 2;
	MSJava.syntaxAssertion(arraySize > 0 && arraySize < MSJava.globalConfig.maxArraySize, null, "invalid array size");
	int typeID = typeIDCounter++;
	StructDef sd = new StructDef("", typeID, numChars, arraySize, OP_CHARS_DEF);
	typeStructDefs[typeID] = sd;
	return sd;
}

public boolean hasType(String name)
{
	return (types.containsKey( name));
}

public boolean hasType(int id)
{
	return typeStructDefs[id] != null;
}

public StructDef  getType (int id) throws MException
{
	StructDef userType = typeStructDefs[id];
	MSJava.assertion(userType != null,   "Unkown type");
	return userType;
}

public StructDef  getType (String name) throws MException
{
	int id = types.get( name);
	StructDef userType = typeStructDefs[id];
	MSJava.assertion(userType != null,   "Unkown type: " + name);
	return userType;
}

public StructDef  getType (int id, NodeIterator itPtr) throws MException
{
	StructDef userType = typeStructDefs[id];
	MSJava.syntaxAssertion(userType != null, itPtr, "Unkown type: #" + id);
	return userType;
}

public StructDef  getType (String name, NodeIterator itPtr) throws MException
{
	int id = types.get( name);
	StructDef userType = typeStructDefs[id];
	MSJava.syntaxAssertion(userType != null, itPtr, "Unkown type: " + name);
	return userType;
}

public boolean inGlobal ()
{
	return currentContext == contexts[0];
}

public Context  findContext (String name)
{
	for (int i=1; i<maxContexts; i++)
	{
		if (contexts[i] == null) continue;
		if ((contexts[i].name.equals( name))) return contexts[i];			
	}
	return null;
}

public boolean  assumeNotReserved (String name) throws MException
{
	// return true if not reserved, otherwise print error message and return false
	
	if(findContext(name) != null) {
		MSJava.errorOut.print("unexpected function name: " + name);
		return false;
	}
	if((types.containsKey( name))) {
		MSJava.errorOut.print("unexpected type name: " + name);
		return false;
	}
	if((globalContext.variables.memberNames.containsKey( name))) {
		MSJava.errorOut.print("duplicate variable name: " + name);
		return false;
	}	
	if (currentContext != globalContext)
	{
		if((currentContext.variables.memberNames.containsKey( name))) {
			MSJava.errorOut.print("duplicate variable name: " + name);
			return false;
		}
	}
	for(int i=0; i<NUM_KEYWORDS; i++)
	{
		if ((name.equals( keywords[i]))) {
			MSJava.errorOut.print("unexpected keyword: " + name);
			return false;
		}
	}
	return true;
}

public void analyze (TokenTree tree) throws MException
{
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("SEMANTIC ANALYZE").endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};

	currentContext = contexts[0];

	NodeIterator it = new NodeIterator(tree.root);
	
	analyzeNode(it.copy()); // start from the first expression
	
	it = null;
	
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("CONTEXTS").endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	for (int i=0; i<maxContexts; i++)
	{
		if (contexts[i] != null)
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- context ID: " + i).endLine();};
			contexts[i].variables.print();
		}
	}
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("END ANALYZING").endLine();};
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(HORIZONTAL_LINE).endLine();};

	print();
}

public void print ()
{
}

public void analyzeNode (NodeIterator it) throws MException
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
				analyzeExpr(it.copy());
				it.toParent();
			}
			
			if (!it.hasNext()) return;

			it.toNext();
		}
		else
		{
			throw new MException(MC.EC_INTERNAL, "expression expected");
		}
	}
}

public void analyzeExpr (NodeIterator it) throws MException
{
	if (MSJava.globalConfig.verboseOn()) it.printTree(false);

	if (it.type() == NT_NAME_TOKEN)
	{
		Context context = findContext(it.data());
		
		if (context != null)
		{
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- function call!!!").endLine();};
		}
		else if ((it.data().equals( keywords[KEYWORD_FUNC_ID])))
		{
			// get return type

			MSJava.syntaxAssertion(it.hasNext(), it, "function return type expected");
			it.toNext();
			MSJava.syntaxAssertion(hasType(it.data()), it, "unknown return type");
			int returnType = types.get( it.data());

			// function name

			MSJava.syntaxAssertion(it.hasNext(), it, "function name expected");
			it.toNext();
			MSJava.syntaxAssertion(it.type() == NT_NAME_TOKEN, it, "function name expected");
			String functionName = it.data();

			MSJava.syntaxAssertion(assumeNotReserved(functionName), it, "variable name error");
			
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Create a new function: " + functionName).endLine();};
			
			// create new context
			Context funcContext = new Context(functionName, numContexts, returnType);
			contexts[numContexts++] = funcContext;

			// argument definition
			
			MSJava.syntaxAssertion(it.hasNext(), it, "argument definition expected");
			it.toNext();
			createStructDef(funcContext.variables, it.copy());
			funcContext.variables.argsSize = funcContext.variables.structSize;
			funcContext.numArgs = funcContext.variables.numMembers;
			
			// parse function body
			
			MSJava.syntaxAssertion(it.hasNext(), it, "function body expected");
			it.toNext();
			MSJava.syntaxAssertion(it.type() == NT_CODE_BLOCK, it, "code block expected");
			
			// save node to Context for Generator
			funcContext.codeNode = it.node;
			
			it.toChild();
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- ANALYZE FUNCTION").endLine();};
			currentContext = funcContext;
			analyzeNode(it.copy());
			currentContext = contexts[0];
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("-------- END ANALYZE FUNCTION").endLine();};
			it.toParent();

			MSJava.syntaxAssertion(!it.hasNext(), it, "unexpected token after code block");
		}
		else if ((it.data().equals( keywords[KEYWORD_STRUCT_ID])))
		{
			// e.g. "struct Vec [int x, INT y, INT z]"

			MSJava.syntaxAssertion(it.hasNext(), it, "struct name expected");
			it.toNext();
			String structName = it.data();
			MSJava.syntaxAssertion(assumeNotReserved(structName), it, "variable name error");
			MSJava.syntaxAssertion(it.hasNext(), it, "struct definition expected");
			it.toNext();
			MSJava.syntaxAssertion(!it.hasNext(), it, "unexpected token after struct definition");
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Create a new struct: " + structName).endLine();};
			addStructDef(structName, it.copy());
		}
		else if (hasType(it.data()))
		{
			// expr. starts with a type name, eg. "int foo" OR "person [5] players"
			
			int type = types.get( it.data());
			MSJava.assertion(type == MS_TYPE_INT || type == MS_TYPE_FLOAT || type == MS_TYPE_TEXT || type == MS_TYPE_CHARS || type >= MAX_MS_TYPES,   "semantics: unknown type: " + type);

			it.toNext();
			
			if (type == MS_TYPE_CHARS)
			{
				// get number of chars, eg. "chars [12] name"
				MSJava.syntaxAssertion(it.type() == NT_SQUARE_BRACKETS, it, "chars size expected");
				it.toChild();
				MSJava.syntaxAssertion(!it.hasNext(), it, "only the chars size expected");
				it.toChild();
				MSJava.syntaxAssertion(!it.hasNext(), it, "only the chars size expected");
				MSJava.syntaxAssertion(it.type() == NT_NUMBER_TOKEN, it, "chars size (number) expected");
				
				// parse size and calculate array size
				
				int charsSize = Integer.parseInt(it.data());
				
				it.toParent();
				it.toParent();
				
				it.toNext();
				String varName = it.data();
				MSJava.syntaxAssertion(assumeNotReserved(varName), it, "variable name error");
				
				StructDef charsType = addCharsType(charsSize);
				
				currentContext.variables.addMember(this, varName, charsType.typeID);				
			}
			else if (it.type() == NT_SQUARE_BRACKETS)
			{
				// eg. "person [5] players"
				
				MSJava.syntaxAssertion(inGlobal(), it, "no arrays in functions");
				
				// array size
				it.toChild();
				MSJava.syntaxAssertion(!it.hasNext(), it, "only the array size expected");
				
				int arraySize = -1;
				
				if (!it.hasChild())
				{
					// array size is not specified, so argument count decide it
					// eg. if "int [] numbers: 1, 2, 3" then size is 3
				}
				else
				{
					it.toChild();
					MSJava.syntaxAssertion(!it.hasNext(), it, "array size expected");
					MSJava.syntaxAssertion(it.type() == NT_NUMBER_TOKEN, it, "array size (number) expected");
					arraySize = Integer.parseInt(it.data());
					MSJava.syntaxAssertion(arraySize > 0 && arraySize < MSJava.globalConfig.maxArraySize, it, "invalid array size");
					it.toParent();
				}
				it.toParent();
				
				// array name
				it.toNext();
				String varName = it.data();
				MSJava.syntaxAssertion(assumeNotReserved(varName), it, "variable name error");
				
				if (arraySize == -1)
				{
					it.toNext();
					MSJava.assertion(it.type() == NT_ASSIGNMENT,   "array assignment expected as the size is not defined");
					arraySize = it.numChildren();
					MSJava.syntaxAssertion(arraySize > 0 && arraySize < MSJava.globalConfig.maxArraySize, it, "invalid array size");
				}
				
				{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("New array: " + varName + ", size " + arraySize).endLine();};

				currentContext.variables.addArray(this, varName, type, arraySize);
			}
			else
			{
				// variable name
				MSJava.syntaxAssertion(assumeNotReserved(it.data()), it, "variable name error");
				{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("New variable: " + it.data() + " <" + currentContext.name + ">").endLine();};
				currentContext.variables.addMember(this, it.data(), type);
			}
		}
	}
	else
	{
		throw new MException(MC.EC_INTERNAL, "unexpected token");
	}
}

public void  addStructDef (String name, NodeIterator it) throws MException
{
	int id = typeIDCounter++;
	

	StructDef sd = new StructDef(name, id);
	createStructDef(sd, it.copy());

	sd.print();

	addStructDef(name, id, sd);
}

public void  addStructDef (String name, int id, StructDef sd) throws MException
{
	MSJava.assertion(!(types.containsKey(name)) && typeStructDefs[sd.typeID] == null,   "addStructDef: type ID reserved");
	types.put( name, (int)(id & VALUE_TYPE_MASK));
	typeStructDefs[sd.typeID] = sd;
}

public void createStructDef (StructDef sd, NodeIterator it) throws MException
{	
	MSJava.syntaxAssertion(it.type() == NT_SQUARE_BRACKETS, it, "struct definition expected");

	it.toChild();
	MSJava.syntaxAssertion(it.type() == NT_EXPR, it, "exression expected");

	do
	{
		if (!it.hasChild()) continue; // skip an empty expression
		it.toChild();
		MSJava.syntaxAssertion((types.containsKey(it.data())),it,  "createStructDef: unknown type: " + it.data());
		int type = types.get( it.data());
		it.toNext();

		if (type == MS_TYPE_CHARS)
		{
			// get number of chars, eg. "chars [12] name"
			MSJava.syntaxAssertion(it.type() == NT_SQUARE_BRACKETS, it, "chars size expected");
			it.toChild();
			MSJava.syntaxAssertion(!it.hasNext(), it, "only the chars size expected");
			it.toChild();
			MSJava.syntaxAssertion(!it.hasNext(), it, "only the chars size expected");
			MSJava.syntaxAssertion(it.type() == NT_NUMBER_TOKEN, it, "chars size (number) expected");
			
			// parse size and calculate array size
			
			int charsSize = Integer.parseInt(it.data());
			
			it.toParent();
			it.toParent();
			
			it.toNext();
			String varName = it.data();
			MSJava.syntaxAssertion(assumeNotReserved(varName), it, "variable name error");
			
			StructDef charsType = addCharsType(charsSize);
			
			sd.addMember(this, varName, charsType.typeID);				
		}
		else if (it.type() == NT_SQUARE_BRACKETS)
		{
			// eg. "int [5] numbers"

			// NOTE: almost same as when defining variables...

			// array size
			it.toChild();
			MSJava.syntaxAssertion(!it.hasNext(), it, "array size expected");
			it.toChild();
			MSJava.syntaxAssertion(!it.hasNext(),it,  "array size expected");
			MSJava.syntaxAssertion(it.type() == NT_NUMBER_TOKEN, it, "array size (number) expected");
			int arraySize = Integer.parseInt(it.data());
			it.toParent();
			it.toParent();
			
			// array name
			it.toNext();
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Member array: " + it.data() + ", size" + arraySize).endLine();};
			
			sd.addArray(this, it.data(), type, arraySize);
		}
		else
		{
			MSJava.syntaxAssertion(it.type() == NT_NAME_TOKEN,it,  "member name expected");
			String memberName = it.data();
			MSJava.syntaxAssertion(!(sd.memberNames.containsKey( memberName)),it,  "duplicate name: " + memberName);
			{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print("Add struct member: " + memberName).endLine();};
			sd.addMember(this, memberName, type);
		}
		MSJava.syntaxAssertion(!it.hasNext(), it, "break expected");
		
		it.toParent();
	}
	while(it.toNextOrFalse());
}

public void writeStructDefs (ByteCode bc)
{
	// write globals
	StructDef sd = contexts[0].variables;
	for (int a=0; a<sd.codeTop; a++)
	{
		bc.addWord(sd.code[a]);
	}
	
	// write user struct definitions to code
	for (int i=MAX_MS_TYPES; i<MAX_TYPES; i++)
	{
		if (typeStructDefs[i] == null) continue;
		sd = typeStructDefs[i];
		for (int a=0; a<sd.codeTop; a++)
		{
			bc.addWord(sd.code[a]);
		}
	}
}


}
