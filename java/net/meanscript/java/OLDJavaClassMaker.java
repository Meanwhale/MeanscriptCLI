package net.meanscript.java;
import net.meanscript.core.*;
import net.meanscript.java.*;
import net.meanscript.*;

public class OLDJavaClassMaker extends MC
{
/*
package com.meanscript;
public class TestVec extends IData
{
	public static TestVec get(Meanscript ms, String s) throws MException
	{
		return new TestVec(ms.mm, ms.mm.getIndex(s));
	}
	// TODO: type ID
	
	private TestVec(MeanMachine _mm, int _index) { super(_mm, _index); }
	@Override public String typeName() { return "vec"; }
	public int getX() { return mm.stack[index]; }
	public int getY() {	return mm.stack[index+1]; }
}
*/
	public static void make(MSCode ms, String packageName, MSOutputStream output) throws MException
	{
		make(ms.getMM(), packageName, output);
	}
	public static void make(MeanMachine mm, String packageName, MSOutputStream output) throws MException
	{
		
		for (int i=0; i<MAX_TYPES; i++)
		{
			if (mm.types[i] != 0) make(mm,i,packageName, output);
		}
	}
	
	public static void make(MeanMachine mm, int type, String packageName, MSOutputStream output) throws MException
	{
		/*ByteCode bc = mm.byteCode;
		
		
		int i = mm.types[type];
		int codeStart = i;
		StringBuilder sdFunc = new StringBuilder(2048);
		
		String typeName = new String(MSJava.intsToBytes(bc.code, i+2, bc.code[i+1]));
		String className = "MS_" + typeName;

		output.write("\n\n");
		output.write("package " + packageName + ";\n");
		output.write("import com.meanscript.*;\n");
		output.write("public class "  + className + " extends IData\n{\n");
		output.write("  public " + className + " (Meanscript ms, String s) throws MException\n  { super(ms,s,"+type+"); }\n\n");
		output.write("  @Override\n  public String typeName() { return \"" + typeName + "\"; }\n");
		output.write("  public static final int TYPE_ID = " + type + ";\n");
		
		// iterate members

		i += instrSize(bc.code[i]) + 1;
		while (i < bc.codeTop)
		{
			int instruction = bc.code[i];
			if ((instruction & OPERATION_MASK) != OP_MEMBER_NAME) break;
			
			String memberName = new String(MSJava.intsToBytes(bc.code, i+2, bc.code[i+1]));
			
			i += instrSize(bc.code[i]) + 1;
			
			instruction = bc.code[i];
			MSJava.assertion(
					(instruction & OPERATION_MASK) == OP_STRUCT_MEMBER || 
					(instruction & OPERATION_MASK) == OP_ARRAY_MEMBER, "member expected");
			
			// tag (type) | address | size
			int memberTypeID = (instruction & VALUE_TYPE_MASK);
			
			if ((instruction & OPERATION_MASK) == OP_ARRAY_MEMBER)
			{
				output.write(
					"  public MSDataArray get_" + memberName + "() throws MException { " +
					"return new MSDataArray(mm,"+i+",index+"+(bc.code[i+1])+",true); }\n");
			}
			else if (memberTypeID == MS_TYPE_INT)
			{
				output.write(
					"  public int get_" + memberName + "() throws MException { " +
					"return stackAt(index+" + (bc.code[i+1]) + "); }\n");
			}
			else if (memberTypeID == MS_TYPE_FLOAT)
			{
				output.write(
					"  public float get_" + memberName + "() throws MException { " +
					"return stackAt(MSJava.intFormatToFloat(index+" + (bc.code[i+1]) + ")); }\n");
			}
			else if (memberTypeID == MS_TYPE_TEXT)
			{
				output.write(
					"  public String get_" + memberName + "() throws MException { " +
					"return stackAt(mm.getText(index+" + (bc.code[i+1]) + ")); }\n");
			}
			else
			{
				output.write(
					"  public MSData get_" + memberName + "() throws MException { " +
					"return new MSData(mm,"+i+",index+"+(bc.code[i+1])+",false); }\n");
			}
			
			// add function call for StructDef getter
			
			sdFunc
				.append("\n    sd.addMember(\"")
				.append(memberName)
				.append("\",")
				.append(memberTypeID)
				.append(");");
			

			i += instrSize(bc.code[i]) + 1;
		}
		
		// StructDef getter
		
		output.write("\n  public static StructDef getStructDef() throws MException {");
		output.write("\n    StructDef sd = new StructDef(\"");
		output.write(typeName + "\"," + type + ");");
		output.write(sdFunc.toString());
		output.write("\n    return sd;\n  }");
		
		// write structure definition bytecode
		//		- use when build
		//		- check that StructDef's match
		
		output.write("\n  protected static int[] code={");
		int foo = 0;
		for (int n=codeStart; n<i; n++)
		{
			if (((foo++)%8)==0) output.write("\n    "); 
			output.write("" + bc.code[n] + ",");
		}
		output.write("\n  };\n");
		
		// END
		output.write("}\n");*/
		output.close();
	}

	private static void print(String s) {
		System.out.println(s);
	}
}
