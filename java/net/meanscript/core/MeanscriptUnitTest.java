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

private static void msText() throws MException
{
	
	int [] ints;
	ints = new int[3];
	ints[0] = 0x00000005;
	ints[1] = 0x64636261;
	ints[2] = 0x00000065;
	
	// public static void intsToBytes(int [] ints, int intsOffset, byte bytes [], int bytesOffset, int bytesLength)
	// public static void bytesToInts(byte bytes [], int bytesOffset, int [] ints, int intsOffset, int bytesLength) 

	byte bytes2 [];
	bytes2 = new byte[10];
	intsToBytes(ints,1,bytes2,0,5);
	MSJava.assertion(bytes2[0] == 0x61,EC_TEST,"");
	MSJava.assertion(bytes2[2] == 0x63,EC_TEST,"");
	MSJava.assertion(bytes2[4] == 0x65,EC_TEST,"");

	byte[] cbytes = {0x61,0x62,0x63,0x64,0x65,0x00};
	int [] ints2;
	ints2 = new int[3];
	bytesToInts(cbytes,0,ints2,0,5);
	MSJava.assertion(ints2[0] == 0x64636261,EC_TEST,"");
	MSJava.assertion(ints2[1] == 0x00000065,EC_TEST,"");

	MSText t = new MSText (ints);
	String s = t.getString();
	MSJava.assertion((s.equals("abcde")),EC_TEST,"");
	
	
	byte[] cbytes2 = {'a','b','c'};
	
	MSText t2 = new MSText (cbytes2,0,3);
	s = t2.getString();
	MSJava.assertion((s.equals("abc")),EC_TEST,"");
	MSJava.assertion(s.length() == 3,EC_TEST,"");
	
	MSText t3 = new MSText (""); // {0, 0}
	MSJava.assertion(t3.numBytes() == 0,EC_TEST,"");
	MSJava.assertion(t3.dataSize() == 2,EC_TEST,"");
	s = t3.getString();
	MSJava.assertion((s.equals("")),EC_TEST,"");
}

private static void utils() throws MException
{
	// check utility functions
	
	// variable name validator
	MSJava.assertion(Parser.isValidName("abc"),EC_TEST,"");
	MSJava.assertion(Parser.isValidName("_a"),EC_TEST,"");
	MSJava.assertion(Parser.isValidName("a1"),EC_TEST,"");

	MSJava.assertion(!Parser.isValidName("123"),EC_TEST,"");
	MSJava.assertion(!Parser.isValidName("~"),EC_TEST,"");
	MSJava.assertion(!Parser.isValidName("a!"),EC_TEST,"");
	MSJava.assertion(!Parser.isValidName(""),EC_TEST,"");
	
	// int32 max = 2147483647, int64 max = 9223372036854775808
	MSJava.assertion(MSJava.parseInt32("2147483647") == 2147483647,EC_TEST,"");
	MSJava.assertion(MSJava.parseInt64("2147483648") == 2147483648l,EC_TEST,"");
	MSJava.assertion(MSJava.parseInt64("-9223372036854775808") == -9223372036854775808l,EC_TEST,"");
}

private static void consistency() throws MException
{
	// check that everything works similarly on all platforms
	
	String s = "A";
	MSJava.assertion(s.length() == 1,EC_TEST,"");
	
	// OLD: now msText test does this
	// converting string to bytes and bytes to string
	//int stringToIntsWithSize(String text, int [] code, int top, int maxSize)
	//int [] intArray;
	//{intArray = new int[3]; };
	//stringToIntsWithSize("abcde", intArray,0,3);
	////STRING_TO_INT_ARRAY(intArray, "abcde");
	//MSJava.assertion(intArray.length == 2,EC_TEST,"");
	//MSJava.assertion(intArray[0] == 1684234849,EC_TEST,"");
	//MSJava.assertion(intArray[1] == 101,EC_TEST,"");
	//s = new String(MSJava.intsToBytes(intArray, 0, 5),java.nio.charset.StandardCharsets.UTF_8);
	//MSJava.assertion((s.equals("abcde")),EC_TEST,"");
	
	// int64 conversions

	long max = -9023372036854775808l;
	int high = int64highBits(max);
	int low = int64lowBits(max);
	long max2 = intsToInt64(high,low);
	MSJava.assertion(max == max2,EC_TEST,"");
	
	double f64 = -12.123456789;
	long longBits = MSJava.float64ToInt64Format(f64);
	MSJava.assertion(longBits == -4600357519365344569l,EC_TEST,"");
	double f64x = MSJava.int64FormatToFloat64(longBits);
	MSJava.assertion(f64 == f64x,EC_TEST,"");
}

private static void simpleVariable() throws MException
{
	// long max: 9223372036854775807
	String s = "int a: 5; int64 short: -1; int64 long: 1234567891234; text b: \"x\";chars [12] ch: \"asds\";";
	s += "float c:-123.456; float64 d: 12.123456789; bool b1: true; bool b2: false";
	MSCode m = new MSCode();
	m.compileAndRun(s);
	MSJava.assertion(m.hasData("a"),EC_TEST,"");
	MSJava.assertion(m.getInt("a") == 5,EC_TEST,"");
	MSJava.assertion(m.getInt64("long") == 1234567891234l,EC_TEST,"");
	MSJava.assertion(m.getInt64("short") == -1l,EC_TEST,"");
	MSJava.assertion((m.getText("b").equals( "x")),EC_TEST,"");
	MSJava.assertion((m.getChars("ch").equals( "asds")),EC_TEST,"");
	MSJava.assertion(m.getFloat("c") == -123.456f,EC_TEST,"");
	MSJava.assertion(m.getFloat64("d") == 12.123456789,EC_TEST,"");
	MSJava.assertion(m.getBool("b1") == true,EC_TEST,"");
	MSJava.assertion(m.getBool("b2") == false,EC_TEST,"");
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
	builder.addMember(personTypeID, "i64", MS_TYPE_INT64);
	builder.addMember(personTypeID, "f", MS_TYPE_FLOAT);
	builder.addMember(personTypeID, "f64", MS_TYPE_FLOAT64);
	builder.addMember(personTypeID, "b", MS_TYPE_BOOL);
	builder.addMember(personTypeID, "name", MS_TYPE_TEXT);
	builder.addCharsMember(personTypeID, "code", 12);
	
	// struct
	// TODO: make builder for other languages too
	
	// simple global values
	builder.addInt("aa", 123);
	builder.addInt64("long", -1234567891234l);
	builder.addText("key","value");
	builder.addChars("cdata",13,"cdatavalue");
	builder.addInt("bb", 456);
	
	MSWriter pw = builder.createStruct("person", "boss");
	pw.setInt("age", 42);
	pw.setInt64("i64", -1);
	pw.setFloat("f", -1);
	pw.setFloat64("f64", -1);
	pw.setBool("b", true);
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
	
	MSJava.assertion(ms.getInt64("long") == -1234567891234l,EC_TEST,"");
	MSJava.assertion(ms.getInt("bb") == 456,EC_TEST,"");
	MSJava.assertion((ms.getText("key").equals("value")),EC_TEST,"");
	MSJava.assertion((ms.getChars("cdata").equals("cdatavalue")),EC_TEST,"");
	
	MSData bossData = ms.getData("boss");
	{if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(bossData.getType()).endLine();};
	MSJava.assertion(bossData.getInt("age") == 42,EC_TEST,"");
	MSJava.assertion(bossData.getInt64("i64") == -1,EC_TEST,"");
	MSJava.assertion(bossData.getFloat("f") == -1,EC_TEST,"");
	MSJava.assertion(bossData.getFloat64("f64") == -1,EC_TEST,"");
	MSJava.assertion(bossData.getBool("b") == true,EC_TEST,"");
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
		
	} catch(MException e) { return e.error == EC_PARSE; }
	return false;
}



public static void  runAll () throws MException
{
	MSJava.printOut.print("TEST " +  "NATIVE_TEST" ); MSJava.nativeTest(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "msText" ); msText(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "utils" ); utils(); MSJava.printOut.print(": OK").endLine();;
	MSJava.printOut.print("TEST " +  "consistency" ); consistency(); MSJava.printOut.print(": OK").endLine();;
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
