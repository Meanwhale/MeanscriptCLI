
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;


ClassMaker::ClassMaker () 
{
	semantics = new Semantics();
	Common* common = new Common();
	(*common).includePrimitives((*semantics));
	{ delete common; common = 0; };
	
	{ offsetStack.reset( 8); offsetStack.fill(0); offsetStack.description =  "offsetStack"; };
	{ arrayItemSizeStack.reset( 8); arrayItemSizeStack.fill(0); arrayItemSizeStack.description =  "arrayItemSizeStack"; };
	{ arrayItemCountStack.reset( 8); arrayItemCountStack.fill(0); arrayItemCountStack.description =  "arrayItemCountStack"; };
	{ memberStack.reset(8); };
	{ arrayStack.reset(8); };
}

ClassMaker::~ClassMaker() { { delete semantics; semantics = 0; }; }

int codeStart = -1;
int codeEnd = -1;
	
void ClassMaker:: findTypes (Semantics & semantics, Array<int> & code) 
{
	int32_t i = 0;
	int32_t instr = code[0];
	//int32_t codeSize = code.length();
	
	codeStart = -1;
	codeEnd = -1;
	
	while ((instr & OPERATION_MASK) != OP_END_INIT)
	{
		if ((instr & OPERATION_MASK) == OP_STRUCT_DEF)
		{
			if (codeStart < 0) codeStart = i;
			std::string name = readStringFromIntArray(code,  i + 2,  code[i+1]);;
			int32_t id = (instr & VALUE_TYPE_MASK);
			
			// add struct to Semantics
			
			StructDef* sd = new StructDef(name, id);
			semantics.addStructDef(name, id, sd);
		}
		else if ((instr & OPERATION_MASK) == OP_CHARS_DEF)
		{
			if (codeStart < 0) codeStart = i;
			// NOTE: duplicate code. same as in Semantics
			int32_t numChars = code[i+1];
			int32_t arraySize = (numChars / 4) + 2;
			SYNTAX(arraySize > 0 && arraySize < globalConfig.maxArraySize, 0, "invalid array size");
			int32_t typeID = (instr & VALUE_TYPE_MASK);
			StructDef* sd = new StructDef("", typeID, numChars, arraySize, OP_CHARS_DEF);
			semantics.typeStructDefs[typeID] = sd;
		}
		i += instrSize(instr) + 1;
		//if (i >= codeSize) break;
		instr = code[i];
	}
	codeEnd = i;
}

void ClassMaker:: createStructDefs (Semantics & semantics, Array<int> & code) 
{
		
	int32_t i = 0;
	int32_t instr = code[0];
	
	while ((instr & OPERATION_MASK) != OP_END_INIT)
	{
		if ((instr & OPERATION_MASK) == OP_STRUCT_DEF && (instr & VALUE_TYPE_MASK) != 0)
		{
			int32_t id = (instr & VALUE_TYPE_MASK);
			StructDef* structDef = semantics.getType(id);
			// PRINT("// generate: " CAT ((*structDef).name));
			
			// read [OP_MEMBER_NAME, OP_STRUCT_MEMBER] pairs
			
			i += instrSize(instr) + 1;
			instr = code[i];
		
			while ((instr & OPERATION_MASK) == OP_MEMBER_NAME)
			{
				// get member name
				
				std::string memberName = readStringFromIntArray(code,  i + 2,  code[i+1]);;
				
				i += instrSize(instr) + 1;
				instr = code[i];
				
				int32_t type = (instr & VALUE_TYPE_MASK);
				int32_t dataAddress = code[i + 1];
				
				if ((instr & OPERATION_MASK) == OP_STRUCT_MEMBER)
				{
					// PRINT("// member: " CAT memberName CAT ", type: " CAT type CAT ", addr.:" CAT dataAddress);
					(*structDef).addMember((&(semantics)), memberName, type);
				}
				else if ((instr & OPERATION_MASK) == OP_ARRAY_MEMBER)
				{
					//PRINT("// array: " CAT memberName CAT ", type: " CAT type CAT ", addr.:" CAT dataAddress CAT ", count: " CAT code[i+3]);
					(*structDef).addArray((&(semantics)), memberName, type, code[i+3]);
				}			
				else
				{
					ERROR("invalid tag: " CATHEX instr);
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

void ClassMaker:: pushName (int32_t offset, std::string memberName, std::string arrayName, int32_t arrayItemSize, int32_t arrayItemCount, int32_t depth) 
{
	//ASSERT(!(compare(memberName,"")), "pushName: memberName is empty");
	//ASSERT(!(compare(arrayName,"")), "pushName: arrayName is empty");
	ASSERT((compare(memberName,"")) || (compare(arrayName,"")), "pushName error");
	offsetStack[depth] = offset;
	memberStack[depth] = memberName;
	arrayStack[depth] = arrayName;
	arrayItemSizeStack[depth] = arrayItemSize;
	arrayItemCountStack[depth] = arrayItemCount;
}

const int32_t ACCESS_BASIC = 100;
const int32_t ACCESS_ARRAY = 101;
const int32_t ACCESS_ARRAY_SIZE = 102;
const int32_t ACCESS_CHARS_SIZE = 103;

void ClassMaker:: makeJavaAcc (MSOutputPrint & os, std::string setterData, int32_t depth, int32_t accessType) 
{
	// make accessors i.e. setter and getter
	
	ASSERT(depth < 8, "makeJavaAcc: too deep");
	
	if ((compare(setterData,""))) os.print("get");
	else os.print("set");
	
	bool array = false;
	// print member/array name hierarchy
	for (int32_t i=0; i <= depth; i++)
	{
		os.print("_");
		// NOTE: other one is empty
		os.print(memberStack[i]);
		if (!(compare(arrayStack[i],"")))
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
		bool first = true;
		for (int32_t i=0; i <= depth; i++)
		{
			if (!(compare(arrayStack[i],"")))
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
	
	if (!(compare(setterData,"")))
	{
		if (array) os.print(", ");
		os.print(setterData);
	}
	os.print(")");
}

void ClassMaker:: makeIndexedAddress (MSOutputPrint & os, int32_t baseAddress, int32_t depth) 
{
	os.print("index+" CAT baseAddress);
	bool array = false;
	for (int32_t i=0; i <= depth; i++)
	{
		if (!(compare(arrayStack[i],"")))
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

void ClassMaker:: makeIndexCheck (MSOutputPrint & os, int32_t depth) 
{
	for (int32_t i=0; i <= depth; i++)
	{
		if (!(compare(arrayStack[i],"")))
		{
			os.print("indexCheck(" CAT (arrayStack[i]) CAT "Index," CAT (arrayItemCountStack[i]) CAT "); ");
		}
	}
}

void ClassMaker:: makeJavaMembers (MSOutputPrint & os, StructDef* sd, int32_t addressOffset, int32_t depth) 
{
	for (int32_t memberIndex = 0; memberIndex < (*sd).numMembers; memberIndex ++)
	{
		int32_t memTag =  			(*sd).getMemberTag(memberIndex);
		int32_t memAddress =		(*sd).getMemberAddress(memberIndex) + addressOffset;
		std::string memberName =		(*sd).getMemberName(memberIndex);
		int32_t memberTypeID =		memTag & VALUE_TYPE_MASK;

		PRINT("  lvl" CAT depth CAT ": " CAT "make member: " CAT memberName);

		if ((memTag & OPERATION_MASK) == OP_ARRAY_MEMBER)
		{

			StructDef* memberStruct = (*semantics).getType(memberTypeID);
			int arrayItemCount = (*sd).getMemberArrayItemCountOrNegative(memberIndex);
			int arrayItemSize = (*memberStruct).structSize;

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
			os.print("return new MSDataArray(mm," CAT memberIndex CAT ",");
			makeIndexedAddress(os, memAddress, depth);
			os.print("); }\n");
			
			// getter for an array items
			
			pushName(memAddress, "", memberName, arrayItemSize, arrayItemCount, depth);
			
			// get the object
			
			os.print("  public " CAT (*memberStruct).name CAT " ");
			makeJavaAcc(os, "", depth, ACCESS_BASIC);
			os.print(" throws MException { ");
			makeIndexCheck(os, depth);
			os.print("return new " CAT (*memberStruct).name CAT "(mc,index+" CAT memAddress CAT "); }\n");
			
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
			StructDef* memberStruct = (*semantics).getType(memberTypeID);
			std::string memberStructName = (*memberStruct).name;
			
			if ((*memberStruct).isCharsDef())
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
				os.print(", data, " CAT ((*memberStruct).numCharsForCharsDef()) CAT "); }\n");
				
				// get size
				
				os.print("  public int         ");
				makeJavaAcc(os, "", depth, ACCESS_CHARS_SIZE);
				os.print(" { return ");
				os.print((*memberStruct).numCharsForCharsDef());
				os.print("; }\n");
			}
			else
			{
				pushName(memAddress, memberName, "", 0, 0, depth);
				
				os.print("  // struct " CAT memberStructName CAT "\n");
				os.print("  public " CAT memberStructName CAT " ");
				
				makeJavaAcc(os, "", depth, ACCESS_BASIC);
						
				os.print(" throws MException { ");
				makeIndexCheck(os, depth);
				os.print("return new " CAT memberStructName CAT "(mc,");
				makeIndexedAddress(os, memAddress, depth);
				os.print("); }\n");
					
				makeJavaMembers(os, memberStruct, memAddress, depth + 1);
				os.print("\n");
			}
		}
	}
}

void ClassMaker:: makeJava (Array<int> & code, std::string packageName, std::string folderName) 
{
	findTypes((*semantics), code);
	createStructDefs((*semantics), code);
	
	
	// create top class
	
	std::string moduleName = "ModuleMS";
	
	MSFilePrint mods( folderName, moduleName, ".java");;
	
	mods.print("package " CAT packageName CAT ";\n");
	mods.print("import net.meanscript.*;\n");
	mods.print("import net.meanscript.java.*;\n");
	mods.print("public class " CAT moduleName CAT " {\n");
	
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
		CAT moduleName
		CAT "\");\n  builder.readStructCode(code);\n");
	mods.print("  return builder;\n}}\n");
	mods.close();
		
	// module code
	
	mods.print("  builder.readStructCode(code);\n");
	
	// go thru all structs and make .java class files of them
	
	for (int32_t i=MAX_MS_TYPES; i < MAX_TYPES; i++)
	{
		if (!(*semantics).hasType(i)) continue;
		
		StructDef* sd = (*semantics).getType(i);
		
		if ((*sd).isCharsDef()) continue;
		
		std::string typeName = (*sd).name;  // Meanscript type
		std::string className = (*sd).name; // Java class
		int32_t typeID = ((*sd).typeID);
		
		MSFilePrint os( folderName, className, ".java");;
				
		// PRINT("// create class " CAT className);

		os.print("package " CAT packageName CAT ";\n");
		os.print("import net.meanscript.*;\n");
		os.print("import net.meanscript.java.*;\n");
		
		// constructors
		
		os.print("public class "  CAT className CAT " extends IData\n{\n");
		os.print("  public " CAT className CAT " (MSBuilder builder, String name) throws MException\n  { super(builder,name," CAT typeID CAT "); }\n\n");
		os.print("  public    " CAT className CAT " (MSCode mc, String varName) throws MException\n  { super(mc,varName," CAT typeID CAT "); }\n\n");
		os.print("  protected " CAT className CAT " (MSCode mc, int dataIndex) throws MException\n  { super(mc,dataIndex," CAT typeID CAT "); }\n\n");
		
		os.print("  @Override\n  public String typeName() { return \"" CAT typeName CAT "\"; }\n");
		os.print("  public static final int TYPE_ID = " CAT typeID CAT ";\n");

		// make getters and setters
		
		makeJavaMembers(os, sd, 0, 0);
				
		// END
		os.print("}\n");
		
		os.close();
	}
	
}
} // namespace meanscript(core)
// C++ END
