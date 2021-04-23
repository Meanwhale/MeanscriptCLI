package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;





public class MeanscriptUnitTest extends MC {


// code: struct vec [int x, int y]
public static final int [] vecStructCode = new int [] {
    134225936,3,6514038,486547458,1,120,151003137,0,
    1,486547458,1,121,151003137,1,1,
};
  
private static String testStructs = "struct vec [int x, int y]; struct person [vec pos, text name, int age];";

private static void simpleVariable() throws MException
{
	String s = "int a: 5; text b: \"x\";chars [12] ch: \"asds\";float c:-123.456";
	MSCode m = new MSCode();
	m.compileAndRun(s);
	MSJava.assertion(m.hasData("a"),EC_TEST,"");
	MSJava.assertion(m.getInt("a") == 5,EC_TEST,"");
	MSJava.assertion((m.getText("b").equals( "x")),EC_TEST,"");
	MSJava.assertion((m.getChars("ch").equals( "asds")),EC_TEST,"");
	MSJava.assertion(m.getFloat("c") == -123.456f,EC_TEST,"");
	m = null;
}

private static void structAssignment() throws MException
{
	String s = testStructs;
	s += "person p: [11,34], \"N\", 41\n";
	s += "int a: p.pos.x;";
	s += "p.pos.x: 12\n";
	MSCode m = new MSCode();
	m.compileAndRun(s);
	MSJava.assertion(m.getData("p").getMember("pos").getMember("y").getInt() == 34,EC_TEST,"");
	MSJava.assertion(m.getData("p").getMember("pos").getMember("x").getInt() == 12,EC_TEST,"");
	MSJava.assertion(m.getData("a").getInt() == 11,EC_TEST,"");
	m = null;
}

private static void argumentList() throws MException
{
	String s = testStructs;
	s += "int a: sum 2 3; int b: (sum 4 5); int c: sum (6, 7); int d: sum (sum 2 (sum (4,7))) 9";
	MSCode m = new MSCode();
	m.compileAndRun(s);
	MSJava.assertion(m.getInt("a") == 5,EC_TEST,"");
	MSJava.assertion(m.getInt("b") == 9,EC_TEST,"");
	MSJava.assertion(m.getInt("c") == 13,EC_TEST,"");
	MSJava.assertion(m.getInt("d") == 22,EC_TEST,"");
	m = null;
}

private static void simpleFunction() throws MException
{
	String s = "func int f [int x] { int n: sum (x, 3); return n }; int a: f 5";
	MSCode m = new MSCode();
	m.compileAndRun(s);
	MSJava.assertion(m.getInt("a") == 8,EC_TEST,"");
	m = null;
}

private static void structFunction() throws MException
{
	String s = testStructs;
	s += "func person p [int a] { person x: [12,34], \"N\", a; return x}; vec foo; person z: p (56)";
	MSCode m = new MSCode();
	m.compileAndRun(s);
	MSJava.assertion(m.getData("z").getInt("age") == 56,EC_TEST,"");
	MSJava.assertion(m.getData("z").getMember("pos").getInt("y") == 34,EC_TEST,"");
	m = null;
}

private static void varArray() throws MException
{
	String s = "struct person [text name, int [2] pos, int age];";
	s += "func int summa [int a, int b] {return (sum a b)};";
	s += "int a:2;int b;int c; int [5] numbers;";
	s += "numbers[a]:1002; numbers[0]:111111; numbers[1]:1001; numbers[4]:222222;";
	s += "a: numbers[a]; b: numbers[4]; c: numbers[summa(2,2)]";
		
	MSCode m = new MSCode();
	m.compileAndRun(s);
	MSJava.assertion(!(m.hasArray("xxyyzz")),EC_TEST,"");
	MSJava.assertion(m.hasArray("numbers"),EC_TEST,"");
	MSJava.assertion(m.getInt("a") == 1002,EC_TEST,"");
	MSJava.assertion(m.getInt("b") == 222222,EC_TEST,"");
	MSJava.assertion(m.getInt("c") == 222222,EC_TEST,"");
	m = null;
}

private static void structArray() throws MException
{
	String s = "struct vec [int x, int y];";
	s += "struct person [text name, chars[12] title, vec [2] pos, int age];";
	s += "func int summa [int a, int b] {return (sum a b)};";
	s += "int a:2;int b;int c:1;";
	s += "person [5] team; team[a].name: \"Jaska\"; team[a].title: \"boss\"; team[a].pos[c].x: 8888; team[a].age: 9999;";
	s += "b: team[a].pos[c].x; c: team[a].age; text t: team[a].name;";
	s += "person [] otherTeam: \n[\"A\", \"tA\", [[1,2], [3,4]], 34],\n [\"B\", \"Bt\", [[5,6], [7,8]], 56],\n [\"C\", \"tC\", [[1,2], [9,0]], 78]";
		
	MSCode m = new MSCode();
	m.compileAndRun(s);
	
	// variable test
	
	MSJava.assertion(m.getInt("a") == 2,EC_TEST,"");
	MSJava.assertion(m.getInt("b") == 8888,EC_TEST,"");
	MSJava.assertion(m.getInt("c") == 9999,EC_TEST,"");
	MSJava.assertion((m.getText("t").equals("Jaska")),EC_TEST,"");
	
	// MSData access test
	
	MSDataArray arr = m.getArray("team");
	MSJava.assertion(arr.getAt(2).getInt("age") == 9999,EC_TEST,"");
	MSJava.assertion((arr.getAt(2).getChars("title").equals("boss")),EC_TEST,"");
	
	// struct array assignment
	arr = m.getArray("otherTeam");
	MSJava.assertion(arr.getAt(2).getInt("age") == 78,EC_TEST,"");
	MSJava.assertion(arr.getAt(1).getArray("pos").getAt(1).getInt("y") == 8,EC_TEST,"");
	
	m = null;
	
}

private static void msBuilder() throws MException
{
	MSBuilder builder = new MSBuilder ("test");
	
	int personTypeID = builder.createStructDef("person");
	builder.addMember(personTypeID, "age", MS_TYPE_INT);
	builder.addMember(personTypeID, "name", MS_TYPE_TEXT);
	builder.addCharsMember(personTypeID, "code", 12);
	
	// struct
	// TODO: make builder for other languages too
	
	// simple global values
	builder.addInt("aa", 123);
	builder.addText("key","value");
	builder.addChars("cdata",13,"cdatavalue");
	builder.addInt("bb", 456);
	
	MSWriter pw = builder.createStruct("person", "boss");
	pw.setInt("age", 42);
	pw.setText("name", "Jaska");
	pw.setChars("code", "abcdefg");
	

//public void addArray (int typeID, String arrayName, int arraySize) throws MException
//public MSWriter arrayItem (String arrayName, int arrayIndex)

	builder.addArray(personTypeID, "team", 3);
	MSWriter aw = builder.arrayItem("team", 1);
	aw.setInt("age", 67);

	builder.generate();
	MSCode ms = builder.createMS();
	//ms.run();
	
	if (MSJava.globalConfig.verboseOn()) ms.printData();
	
	MSJava.assertion(ms.getInt("bb") == 456,EC_TEST,"");
	MSJava.assertion((ms.getText("key").equals("value")),EC_TEST,"");
	MSJava.assertion((ms.getChars("cdata").equals("cdatavalue")),EC_TEST,"");
	
	MSData bossData = ms.getData("boss");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(bossData.getType()).endLine();};
	MSJava.assertion(bossData.getInt("age") == 42,EC_TEST,"");
	MSJava.assertion((ms.getData("boss").getText("name").equals( "Jaska")),EC_TEST,"");
	MSJava.assertion((ms.getData("boss").getChars("code").equals( "abcdefg")),EC_TEST,"");
	
	MSDataArray arr = ms.getArray("team");
	MSJava.assertion(arr.getAt(1).getInt("age") == 67,EC_TEST,"");
	
	
	ms = null;
}

private static void inputOutputStream() throws MException
{
	String code = "int a: 5";
	MSInputArray input = new MSInputArray(code);
	MSCode m = new MSCode(input, MSJava.globalConfig.STREAM_SCRIPT);
	m.run();
	MSJava.assertion(m.getInt("a") == 5,EC_TEST,"");
	
	MSOutputArray output = new MSOutputArray();
	m.writeCode(output);
	
	input = null;
	input = new MSInputArray(output);
	m.initBytecode(input);
	m.run();
	MSJava.assertion(m.getInt("a") == 5,EC_TEST,"");
		
	input = null;
	output = null;
	m = null;
}


private static void readOnly() throws MException
{
	String code = "int a: 5";
	MSInputArray input = new MSInputArray(code);
	MSCode m = new MSCode(input, MSJava.globalConfig.STREAM_SCRIPT);
	m.run();
	MSJava.assertion(m.getInt("a") == 5,EC_TEST,"");
	
	MSOutputArray output = new MSOutputArray();
	m.writeReadOnlyData(output);
	
	input = null;
	input = new MSInputArray(output);
	m.initBytecode(input);
	MSJava.assertion(m.getInt("a") == 5,EC_TEST,"");
		
	input = null;
	output = null;
	m = null;
}


private static boolean parseError() throws MException
{
	try {
		MSCode m = new MSCode();
		String s = "a~";
		m.compileAndRun(s);
		
	} catch(MException e) { return e.error == E_UNEXPECTED_CHAR; }
	return false;
}



public static void  runAll () throws MException
{
	MSJava.printOut.print("TEST " +  "simpleVariable" ); simpleVariable(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "structAssignment" ); structAssignment(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "argumentList" ); argumentList(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "simpleFunction" ); simpleFunction(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "structFunction" ); structFunction(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "msBuilder" ); msBuilder(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "varArray" ); varArray(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "structArray" ); structArray(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "inputOutputStream" ); inputOutputStream(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "readOnly" ); readOnly(); MSJava.printOut.print(": OK").endLine();;


	MSJava.printOut.print("TEST ERROR " +  "parseError" ).endLine(); if (parseError()) MSJava.printOut.print(": OK").endLine(); else throw new MException(MC.EC_INTERNAL, "ERROR TEST FAIL");;
	
}

}
