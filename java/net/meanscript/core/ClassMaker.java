package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class ClassMaker extends MC {

 Semantics semantics;
 int [] offsetStack;
 int [] arrayItemSizeStack;
 int [] arrayItemCountStack;
 String [] memberStack;
 String [] arrayStack;



public ClassMaker () throws MException
{
	semantics = new Semantics();
	Common common = new Common();
	common.initialize(semantics);
	common = null;
	
	{offsetStack = new int[ 8]; };
	{arrayItemSizeStack = new int[ 8]; };
	{arrayItemCountStack = new int[ 8]; };
	{ memberStack = new String[8];  };
	{ arrayStack = new String[8];  };
}

//

private static int codeStart = -1;
private static int codeEnd = -1;
	
public static void  findTypes (Semantics semantics, int [] code) throws MException
{
	int i = 0;
	int instr = code[0];
	//int codeSize = code.length;
	
	codeStart = -1;
	codeEnd = -1;
	
	while ((instr & OPERATION_MASK) != OP_END_INIT)
	{
		if ((instr & OPERATION_MASK) == OP_STRUCT_DEF)
		{
			if (codeStart < 0) codeStart = i;
			//String name = new String(MSJava.intsToBytes(code, i + 2, code[i+1]),java.nio.charset.StandardCharsets.UTF_8);
			MSText name = new MSText(code, i + 1);
			int id = (instr & VALUE_TYPE_MASK);
			
			// add struct to Semantics
			
			StructDef sd = new StructDef(name, id);
			semantics.addStructDef(name, id, sd);
		}
		else if ((instr & OPERATION_MASK) == OP_CHARS_DEF)
		{
			if (codeStart < 0) codeStart = i;
			// NOTE: duplicate code. same as in Semantics
			int numChars = code[i+1];
			int arraySize = (numChars / 4) + 2;
			MSJava.syntaxAssertion(arraySize > 0 && arraySize < MSJava.globalConfig.maxArraySize, null, "invalid array size");
			int typeID = (instr & VALUE_TYPE_MASK);
			StructDef sd = new StructDef(new MSText(""), typeID, numChars, arraySize, OP_CHARS_DEF);
			semantics.typeStructDefs[typeID] = sd;
		}
		i += instrSize(instr) + 1;
		//if (i >= codeSize) break;
		instr = code[i];
	}
	codeEnd = i;
}

public static void  createStructDefs (Semantics semantics, int [] code) throws MException
{
		
	int i = 0;
	int instr = code[0];
	
	while ((instr & OPERATION_MASK) != OP_END_INIT)
	{
		if ((instr & OPERATION_MASK) == OP_STRUCT_DEF && (instr & VALUE_TYPE_MASK) != 0)
		{
			int id = (instr & VALUE_TYPE_MASK);
			StructDef structDef = semantics.getType(id);
			// MSJava.printOut.print("// generate: " + (structDef.name)).endLine();
			
			// read [OP_MEMBER_NAME, OP_STRUCT_MEMBER] pairs
			
			i += instrSize(instr) + 1;
			instr = code[i];
		
			while ((instr & OPERATION_MASK) == OP_MEMBER_NAME)
			{
				// get member name
				
				MSText memberName = new MSText(code, i + 1);
				
				i += instrSize(instr) + 1;
				instr = code[i];
				
				int type = (instr & VALUE_TYPE_MASK);
				
				if ((instr & OPERATION_MASK) == OP_STRUCT_MEMBER)
				{
					structDef.addMember(semantics, memberName, type);
				}
				else if ((instr & OPERATION_MASK) == OP_ARRAY_MEMBER)
				{
					structDef.addArray(semantics, memberName, type, code[i+3]);
				}			
				else
				{
					throw new MException(MC.EC_INTERNAL, "invalid tag");
				}
				i += instrSize(instr) + 1;
				instr = code[i];
			}
		}
		else
		{
			i += instrSize(instr) + 1;
			instr = code[i];
		}
	}
}

public void  pushName (int offset, String memberName, String arrayName, int arrayItemSize, int arrayItemCount, int depth) throws MException
{
	//MSJava.assertion(!(memberName.equals("")),   "pushName: memberName is empty");
	//MSJava.assertion(!(arrayName.equals("")),   "pushName: arrayName is empty");
	MSJava.assertion((memberName.equals("")) || (arrayName.equals("")),   "pushName error");
	offsetStack[depth] = offset;
	memberStack[depth] = memberName;
	arrayStack[depth] = arrayName;
	arrayItemSizeStack[depth] = arrayItemSize;
	arrayItemCountStack[depth] = arrayItemCount;
}

final static int ACCESS_BASIC = 100;
final static int ACCESS_ARRAY = 101;
final static int ACCESS_ARRAY_SIZE = 102;
final static int ACCESS_CHARS_SIZE = 103;

public void  makeJavaAcc (MSOutputPrint os, String setterData, int depth, int accessType) throws MException
{
	// make accessors i.e. setter and getter
	
	MSJava.assertion(depth < 8,   "makeJavaAcc: too deep");
	
	if ((setterData.equals(""))) os.print("get");
	else os.print("set");
	
	boolean array = false;
	// print member/array name hierarchy
	for (int i=0; i <= depth; i++)
	{
		os.print("_");
		// NOTE: other one is empty
		os.print(memberStack[i]);
		if (!(arrayStack[i].equals("")))
		{
			array = true;
			os.print(arrayStack[i]);
		}
	}
	
	if (accessType == ACCESS_ARRAY)	{
		os.print("_array");
	} else if (accessType == ACCESS_ARRAY_SIZE) {
		os.print("_array_size");
	} else if (accessType == ACCESS_CHARS_SIZE) {
		os.print("_max_chars");
	}
	if (array && accessType != ACCESS_ARRAY_SIZE && accessType != ACCESS_CHARS_SIZE)
	{
		// generate list of index parameters
		os.print("_at(");
		boolean first = true;
		for (int i=0; i <= depth; i++)
		{
			if (!(arrayStack[i].equals("")))
			{
				if (!first) os.print(",");
				os.print("int ");
				os.print(arrayStack[i]);
				os.print("Index");
				first = false;
			}
		}
	}		
	else os.print("(");
	
	if (!(setterData.equals("")))
	{
		if (array) os.print(", ");
		os.print(setterData);
	}
	os.print(")");
}

public void  makeIndexedAddress (MSOutputPrint os, int baseAddress, int depth) throws MException
{
	os.print("index+" + baseAddress);
	boolean array = false;
	for (int i=0; i <= depth; i++)
	{
		if (!(arrayStack[i].equals("")))
		{
			if (array) os.print("+");
			else os.print("+(");
			array = true;
			os.print(arrayStack[i]);
			os.print("Index*");
			os.print(arrayItemSizeStack[i]);
		}
	}
	if (array) os.print(")");
}

public void  makeIndexCheck (MSOutputPrint os, int depth) throws MException
{
	for (int i=0; i <= depth; i++)
	{
		if (!(arrayStack[i].equals("")))
		{
			os.print("indexCheck(" + (arrayStack[i]) + "Index," + (arrayItemCountStack[i]) + "); ");
		}
	}
}

public void  makeJavaMembers (MSOutputPrint os, StructDef sd, int addressOffset, int depth) throws MException
{
	for (int memberIndex = 0; memberIndex < sd.numMembers; memberIndex ++)
	{
		int memTag =  			sd.getMemberTag(memberIndex);
		int memAddress =		sd.getMemberAddress(memberIndex) + addressOffset;
		String memberName =		sd.getMemberName(memberIndex);
		int memberTypeID =		memTag & VALUE_TYPE_MASK;

		MSJava.printOut.print("  lvl" + depth + ": " + "make member: " + memberName).endLine();

		if ((memTag & OPERATION_MASK) == OP_ARRAY_MEMBER)
		{

			StructDef memberStruct = semantics.getType(memberTypeID);
			int arrayItemCount = sd.getMemberArrayItemCountOrNegative(memberIndex);
			int arrayItemSize = memberStruct.structSize;

			// getter for an array object
			
			pushName(memAddress, memberName, "", 0, 0, depth);
			
			// get size
			
			os.print("  public int         ");
			makeJavaAcc(os, "", depth, ACCESS_ARRAY_SIZE);
			os.print(" { return ");
			os.print(arrayItemCount);
			os.print("; }\n");
			
			// get the object array
			
			os.print("  public MSDataArray ");
			makeJavaAcc(os, "", depth, ACCESS_ARRAY);
			os.print(" throws MException { ");
			makeIndexCheck(os, depth);
			os.print("return new MSDataArray(mm," + memberIndex + ",");
			makeIndexedAddress(os, memAddress, depth);
			os.print("); }\n");
			
			// getter for an array items
			
			pushName(memAddress, "", memberName, arrayItemSize, arrayItemCount, depth);
			
			// get the object
			
			os.print("  public " + memberStruct.name + " ");
			makeJavaAcc(os, "", depth, ACCESS_BASIC);
			os.print(" throws MException { ");
			makeIndexCheck(os, depth);
			os.print("return new " + memberStruct.name + "(mc,index+" + memAddress + "); }\n");
			
			// if it's a struct, create getters for array item members
			makeJavaMembers(os, memberStruct, memAddress, depth + 1);
		}
		else if (memberTypeID == MS_TYPE_INT)
		{
			pushName(memAddress, memberName, "", 0, 0, depth);
			
			os.print("  public int         ");
			makeJavaAcc(os, "", depth, ACCESS_BASIC);
			os.print(" throws MException { ");
			makeIndexCheck(os, depth);
			os.print("return dataAt(");
			makeIndexedAddress(os, memAddress, depth);
			os.print("); }\n");
			
			os.print("  public void        ");
			makeJavaAcc(os, "int data", depth, ACCESS_BASIC);
			os.print(" throws MException { ");
			makeIndexCheck(os, depth);
			os.print("write(");
			makeIndexedAddress(os, memAddress, depth);
			os.print(", data); }\n");
		}
		else if (memberTypeID == MS_TYPE_FLOAT)
		{
			pushName(memAddress, memberName, "", 0, 0, depth);
			
			os.print("  public float       ");
			makeJavaAcc(os, "", depth, ACCESS_BASIC);
			os.print(" throws MException { ");
			makeIndexCheck(os, depth);
			os.print("return MSJava.intFormatToFloat(dataAt(");
			makeIndexedAddress(os, memAddress, depth);
			os.print(")); }\n");
			
			os.print("  public void        ");
			makeJavaAcc(os, "float data", depth, ACCESS_BASIC);
			os.print(" throws MException { ");
			makeIndexCheck(os, depth);
			os.print("write(");
			makeIndexedAddress(os, memAddress, depth);
			os.print(", MSJava.floatToIntFormat(data)); }\n");
		}
		else if (memberTypeID == MS_TYPE_TEXT)
		{
			pushName(memAddress, memberName, "", 0, 0, depth);
			
			os.print("  public String      ");
			makeJavaAcc(os, "", depth, ACCESS_BASIC);
			os.print(" throws MException { ");
			makeIndexCheck(os, depth);
			os.print("return mc.getText(dataAt(");
			makeIndexedAddress(os, memAddress, depth);
			os.print(")); }\n");
		}
		else
		{
			StructDef memberStruct = semantics.getType(memberTypeID);
			MSText memberStructName = memberStruct.name;
			
			if (memberStruct.isCharsDef())
			{
				pushName(memAddress, memberName, "", 0, 0, depth);
				
				os.print("  public String      ");
				makeJavaAcc(os, "", depth, ACCESS_BASIC);
				os.print(" throws MException { ");
				makeIndexCheck(os, depth);
				os.print("return getChars(");
				makeIndexedAddress(os, memAddress, depth);
				os.print("); }\n");
				
				os.print("  public void        ");
				makeJavaAcc(os, "String data", depth, ACCESS_BASIC);
				os.print(" throws MException { ");
				makeIndexCheck(os, depth);
				os.print("setChars(");
				makeIndexedAddress(os, memAddress, depth);
				os.print(", data, " + (memberStruct.numCharsForCharsDef()) + "); }\n");
				
				// get size
				
				os.print("  public int         ");
				makeJavaAcc(os, "", depth, ACCESS_CHARS_SIZE);
				os.print(" { return ");
				os.print(memberStruct.numCharsForCharsDef());
				os.print("; }\n");
			}
			else
			{
				pushName(memAddress, memberName, "", 0, 0, depth);
				
				os.print("  // struct " + memberStructName + "\n");
				os.print("  public " + memberStructName + " ");
				
				makeJavaAcc(os, "", depth, ACCESS_BASIC);
						
				os.print(" throws MException { ");
				makeIndexCheck(os, depth);
				os.print("return new " + memberStructName + "(mc,");
				makeIndexedAddress(os, memAddress, depth);
				os.print("); }\n");
					
				makeJavaMembers(os, memberStruct, memAddress, depth + 1);
				os.print("\n");
			}
		}
	}
}

public void  makeJava (int [] code, String packageName, String moduleName, String folderName) throws MException
{
	findTypes(semantics, code);
	createStructDefs(semantics, code);
	
	
	// create top class
		
	MSFilePrint mods = new MSFilePrint( folderName +  moduleName +  ".java");;
	
	mods.print("package " + packageName + ";\n");
	mods.print("import net.meanscript.*;\n");
	mods.print("import net.meanscript.java.*;\n");
	mods.print("public class " + moduleName + " {\n");
	
	// bytecode for semantics (builder) generation
	
	mods.print("\npublic static final int[] code = {");
	int foo = 0;
	for (int n=codeStart; n <= codeEnd; n++)
	{
		if (((foo++)%8)==0) mods.print("\n  "); 
		mods.print("0x");
		mods.printHex(code[n]);
		mods.print(", ");
	}
	mods.print("\n};\n");
		
	mods.print("public static MSBuilder getBuilder()  throws MException {\n  MSBuilder builder = new MSBuilder (\""
		+ moduleName
		+ "\");\n  builder.readStructCode(code);\n");
	mods.print("  return builder;\n}}\n");
	mods.close();
		
	// module code
	
	mods.print("  builder.readStructCode(code);\n");
	
	// go thru all structs and make .java class files of them
	
	for (int i=MAX_MS_TYPES; i < MAX_TYPES; i++)
	{
		if (!semantics.hasType(i)) continue;
		
		StructDef sd = semantics.getType(i);
		
		if (sd.isCharsDef()) continue;
		
		MSText typeName = sd.name;  // Meanscript type
		MSText className = sd.name; // Java class
		int typeID = (sd.typeID);
		
		MSFilePrint os = new MSFilePrint( folderName +  className.getString() +  ".java");;
				
		// MSJava.printOut.print("// create class " + className).endLine();

		os.print("package " + packageName + ";\n");
		os.print("import net.meanscript.*;\n");
		os.print("import net.meanscript.java.*;\n");
		
		// constructors
		
		os.print("public class "  + className + " extends IData\n{\n");
		os.print("  public " + className + " (MSBuilder builder, String name) throws MException\n  { super(builder,name," + typeID + "); }\n\n");
		os.print("  public    " + className + " (MSCode mc, String varName) throws MException\n  { super(mc,varName," + typeID + "); }\n\n");
		os.print("  protected " + className + " (MSCode mc, int dataIndex) throws MException\n  { super(mc,dataIndex," + typeID + "); }\n\n");
		
		os.print("  @Override\n  public String typeName() { return \"" + typeName + "\"; }\n");
		os.print("  public static final int TYPE_ID = " + typeID + ";\n");

		// make getters and setters
		
		makeJavaMembers(os, sd, 0, 0);
				
		// END
		os.print("}\n");
		
		os.close();
	}
	
}
}
