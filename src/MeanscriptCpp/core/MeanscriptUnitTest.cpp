#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

// code: struct vec [int x, int32_t y]
constexpr int vecStructCode [] = {
    134225936,3,6514038,486547458,1,120,151003137,0,
    1,486547458,1,121,151003137,1,1,
};
  
const char * testStructs = "struct vec [int x, int y]; struct person [vec pos, text name, int age];";

void msText() 
{
	
	Array<int> ints;
	ints.reset(3);
	ints[0] = 0x00000005;
	ints[1] = 0x64636261;
	ints[2] = 0x00000065;
	
	//  void intsToBytes(Array<int> & ints, int32_t intsOffset, Array<uint8_t> bytes &, int32_t bytesOffset, int32_t bytesLength)
	//  void bytesToInts(Array<uint8_t> bytes &, int32_t bytesOffset, Array<int> & ints, int32_t intsOffset, int32_t bytesLength) 

	Array<uint8_t> bytes2;
	bytes2.reset(10);
	intsToBytes(ints,1,bytes2,0,5);
	TEST(bytes2[0] == 0x61);
	TEST(bytes2[2] == 0x63);
	TEST(bytes2[4] == 0x65);

	const uint8_t cbytes[] = {0x61,0x62,0x63,0x64,0x65,0x00};
	Array<int> ints2;
	ints2.reset(3);
	bytesToInts(cbytes,0,ints2,0,5);
	TEST(ints2[0] == 0x64636261);
	TEST(ints2[1] == 0x00000065);

	MSText t (ints);
	std::string s = t.getString();
	TEST((compareStrings(s,"abcde")));
	
	
	const uint8_t cbytes2[] = {'a','b','c'};
	
	MSText t2 (cbytes2,0,3);
	s = t2.getString();
	TEST((compareStrings(s,"abc")));
	TEST(s.size() == 3);
	
	MSText t3 (""); // {0, 0}
	TEST(t3.numBytes() == 0);
	TEST(t3.dataSize() == 2);
	s = t3.getString();
	TEST((compareStrings(s,"")));
}

void utils() 
{
	// check utility functions
	
	// variable name validator
	TEST(Parser::isValidName("abc"));
	TEST(Parser::isValidName("_a"));
	TEST(Parser::isValidName("a1"));

	TEST(!Parser::isValidName("123"));
	TEST(!Parser::isValidName("~"));
	TEST(!Parser::isValidName("a!"));
	TEST(!Parser::isValidName(""));
	
	// int32 max = 2147483647, int64 max = 9223372036854775808
	TEST(std::stoi("2147483647") == 2147483647);
}

void consistency() 
{
	// check that everything works similarly on all platforms
	
	std::string s = "A";
	TEST(s.size() == 1);
	
	// OLD: now msText test does this
	// converting string to bytes and bytes to string
	//int32_t stringToIntsWithSize(const std::string & text, Array<int> & code, int32_t top, int32_t maxSize)
	//Array<int> intArray;
	//{ intArray.reset(3); intArray.fill(0); intArray.description = ""; };
	//stringToIntsWithSize("abcde", intArray,0,3);
	////STRING_TO_INT_ARRAY(intArray, "abcde");
	//TEST(intArray.length() == 2);
	//TEST(intArray[0] == 1684234849);
	//TEST(intArray[1] == 101);
	//s = readStringFromIntArray(intArray,  0,  5);;
	//TEST((compareStrings(s,"abcde")));
	
	// int64 conversions

	int64_t max = -9023372036854775808l;
	int32_t high = int64highBits(max);
	int32_t low = int64lowBits(max);
	int64_t max2 = intsToInt64(high,low);
	TEST(max == max2);
	
	double f64 = -12.123456789;
	int64_t longBits = ((int64_t&)(*(&f64)));
	TEST(longBits == -4600357519365344569l);
	double f64x = ((double&)(*(&longBits)));
	TEST(f64 == f64x);
}

void simpleVariable() 
{
	// long max: 9223372036854775807
	std::string s = "int a: 5; int64 short: -1; int64 long: 1234567891234; text b: \"x\";chars [12] ch: \"asds\";";
	s += "float c:-123.456; float64 d: 12.123456789; bool b1: true; bool b2: false";
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	TEST((*m).hasData("a"));
	TEST((*m).getInt("a") == 5);
	TEST((*m).getInt64("long") == 1234567891234l);
	TEST((*m).getInt64("short") == -1l);
	TEST((compareStrings((*m).getText("b"), "x")));
	TEST((compareStrings((*m).getChars("ch"), "asds")));
	TEST((*m).getFloat("c") == -123.456f);
	TEST((*m).getBool("b1") == true);
	TEST((*m).getBool("b2") == false);
	{ delete m; m = 0; };
}

void structAssignment() 
{
	std::string s = testStructs;
	s += "person p: [11,34], \"N\", 41\n";
	s += "int a: p.pos.x;";
	s += "p.pos.x: 12\n";
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	TEST((*m).getData("p").getMember("pos").getMember("y").getInt() == 34);
	TEST((*m).getData("p").getMember("pos").getMember("x").getInt() == 12);
	TEST((*m).getData("a").getInt() == 11);
	{ delete m; m = 0; };
}

void argumentList() 
{
	std::string s = testStructs;
	s += "int a: sum 2 3; int b: (sum 4 5); int c: sum (6, 7); int d: sum (sum 2 (sum (4,7))) 9";
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	TEST((*m).getInt("a") == 5);
	TEST((*m).getInt("b") == 9);
	TEST((*m).getInt("c") == 13);
	TEST((*m).getInt("d") == 22);
	{ delete m; m = 0; };
}

void simpleFunction() 
{
	std::string s = "func int f [int x] { int n: sum (x, 3); return n }; int a: f 5";
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	TEST((*m).getInt("a") == 8);
	{ delete m; m = 0; };
}

void structFunction() 
{
	std::string s = testStructs;
	s += "func person p [int a] { person x: [12,34], \"N\", a; return x}; vec foo; person z: p (56)";
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	TEST((*m).getData("z").getInt("age") == 56);
	TEST((*m).getData("z").getMember("pos").getInt("y") == 34);
	{ delete m; m = 0; };
}

void varArray() 
{
	std::string s = "struct person [text name, int [2] pos, int age];";
	s += "func int summa [int a, int b] {return (sum a b)};";
	s += "int a:2;int b;int c; int [5] numbers;";
	s += "numbers[a]:1002; numbers[0]:111111; numbers[1]:1001; numbers[4]:222222;";
	s += "a: numbers[a]; b: numbers[4]; c: numbers[summa(2,2)]";
		
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	TEST(!((*m).hasArray("xxyyzz")));
	TEST((*m).hasArray("numbers"));
	TEST((*m).getInt("a") == 1002);
	TEST((*m).getInt("b") == 222222);
	TEST((*m).getInt("c") == 222222);
	{ delete m; m = 0; };
}

void structArray() 
{
	std::string s = "struct vec [int x, int y];";
	s += "struct person [text name, chars[12] title, vec [2] pos, int age];";
	s += "func int summa [int a, int b] {return (sum a b)};";
	s += "int a:2;int b;int c:1;";
	s += "person [5] team; team[a].name: \"Jaska\"; team[a].title: \"boss\"; team[a].pos[c].x: 8888; team[a].age: 9999;";
	s += "b: team[a].pos[c].x; c: team[a].age; text t: team[a].name;";
	s += "person [] otherTeam: \n[\"A\", \"tA\", [[1,2], [3,4]], 34],\n [\"B\", \"Bt\", [[5,6], [7,8]], 56],\n [\"C\", \"tC\", [[1,2], [9,0]], 78]";
		
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	
	// variable test
	
	TEST((*m).getInt("a") == 2);
	TEST((*m).getInt("b") == 8888);
	TEST((*m).getInt("c") == 9999);
	TEST((compareStrings((*m).getText("t"),"Jaska")));
	
	// MSData access test
	
	MSDataArray arr = (*m).getArray("team");
	TEST(arr.getAt(2).getInt("age") == 9999);
	TEST((compareStrings(arr.getAt(2).getChars("title"),"boss")));
	
	// struct array assignment
	arr = (*m).getArray("otherTeam");
	TEST(arr.getAt(2).getInt("age") == 78);
	TEST(arr.getAt(1).getArray("pos").getAt(1).getInt("y") == 8);
	
	{ delete m; m = 0; };
	
}

void msBuilder() 
{
	MSBuilder builder ("test");
	
	int32_t personTypeID = builder.createStructDef("person");
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
	pw.setBool("b", true);
	pw.setText("name", "Jaska");
	pw.setChars("code", "abcdefg");
	

//void MeanscriptUnitTest::addArray (int32_t typeID, std::string arrayName, int32_t arraySize) 
//MSWriter & MeanscriptUnitTest::arrayItem (std::string arrayName, int32_t arrayIndex)

	builder.addArray(personTypeID, "team", 3);
	MSWriter aw = builder.arrayItem("team", 1);
	aw.setInt("age", 67);

	builder.generate();
	MSCode* ms = builder.createMS();
	//(*ms).run();
	
	if (globalConfig.verboseOn()) (*ms).printData();
	
	TEST((*ms).getInt64("long") == -1234567891234l);
	TEST((*ms).getInt("bb") == 456);
	TEST((compareStrings((*ms).getText("key"),"value")));
	TEST((compareStrings((*ms).getChars("cdata"),"cdatavalue")));
	
	MSData bossData = (*ms).getData("boss");
	VERBOSE(bossData.getType());
	TEST(bossData.getInt("age") == 42);
	TEST(bossData.getInt64("i64") == -1);
	TEST(bossData.getFloat("f") == -1);
	TEST(bossData.getBool("b") == true);
	TEST((compareStrings((*ms).getData("boss").getText("name"), "Jaska")));
	TEST((compareStrings((*ms).getData("boss").getChars("code"), "abcdefg")));
	
	MSDataArray arr = (*ms).getArray("team");
	TEST(arr.getAt(1).getInt("age") == 67);
	
	
	{ delete ms; ms = 0; };
}

void inputOutputStream() 
{
	std::string code = "int a: 5";
	MSInputArray* input = new MSInputArray(code);
	MSCode* m = new MSCode((*input), globalConfig.STREAM_SCRIPT);
	(*m).run();
	TEST((*m).getInt("a") == 5);
	
	MSOutputArray* output = new MSOutputArray();
	(*m).writeCode((*output));
	
	{ delete input; input = 0; };
	input = new MSInputArray(output);
	(*m).initBytecode((*input));
	(*m).run();
	TEST((*m).getInt("a") == 5);
		
	{ delete input; input = 0; };
	{ delete output; output = 0; };
	{ delete m; m = 0; };
}


void readOnly() 
{
	std::string code = "int a: 5";
	MSInputArray* input = new MSInputArray(code);
	MSCode* m = new MSCode((*input), globalConfig.STREAM_SCRIPT);
	(*m).run();
	TEST((*m).getInt("a") == 5);
	
	MSOutputArray* output = new MSOutputArray();
	(*m).writeReadOnlyData((*output));
	
	{ delete input; input = 0; };
	input = new MSInputArray(output);
	(*m).initBytecode((*input));
	TEST((*m).getInt("a") == 5);
		
	{ delete input; input = 0; };
	{ delete output; output = 0; };
	{ delete m; m = 0; };
}


bool parseError() 
{
	TRY
		MSCode m = MSCode();
		std::string s = "a~";
		m.compileAndRun(s);
		
	TEST_CATCH (EC_PARSE)
	return false;
}



void MeanscriptUnitTest:: runAll () 
{
	PRINTN("TEST " CAT  "NATIVE_TEST" ); nativeTest(); PRINT(": OK");;
	PRINTN("TEST " CAT  "msText" ); msText(); PRINT(": OK");;
	PRINTN("TEST " CAT  "utils" ); utils(); PRINT(": OK");;
	PRINTN("TEST " CAT  "consistency" ); consistency(); PRINT(": OK");;
	PRINTN("TEST " CAT  "simpleVariable" ); simpleVariable(); PRINT(": OK");;
	PRINTN("TEST " CAT  "structAssignment" ); structAssignment(); PRINT(": OK");;
	PRINTN("TEST " CAT  "argumentList" ); argumentList(); PRINT(": OK");;
	PRINTN("TEST " CAT  "simpleFunction" ); simpleFunction(); PRINT(": OK");;
	PRINTN("TEST " CAT  "structFunction" ); structFunction(); PRINT(": OK");;
	PRINTN("TEST " CAT  "msBuilder" ); msBuilder(); PRINT(": OK");;
	PRINTN("TEST " CAT  "varArray" ); varArray(); PRINT(": OK");;
	PRINTN("TEST " CAT  "structArray" ); structArray(); PRINT(": OK");;
	PRINTN("TEST " CAT  "inputOutputStream" ); inputOutputStream(); PRINT(": OK");;
	PRINTN("TEST " CAT  "readOnly" ); readOnly(); PRINT(": OK");;

}

} // namespace meanscript(core)
