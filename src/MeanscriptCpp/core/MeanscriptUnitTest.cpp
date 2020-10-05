
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

// code: struct vec [int x, int32_t y]
constexpr int vecStructCode [] = {
    134225936,3,6514038,486547458,1,120,151003137,0,
    1,486547458,1,121,151003137,1,1,
};
  
const char * testStructs = "struct vec [int x, int y]; struct person [vec pos, text name, int age];";

void simpleVariable() 
{
	std::string s = "int a: 5; text b: \"x\";float c:-123.456";
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	TEST((*m).hasData("a"));
	TEST((*m).getInt("a") == 5);
	TEST(((*m).getText("b").compare( "x")==0));
	TEST((*m).getFloat("c") == -123.456f);
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
	s += "struct person [text name, vec [2] pos, int age];";
	s += "func int summa [int a, int b] {return (sum a b)};";
	s += "int a:2;int b;int c:1;";
	s += "person [5] team; team[a].name: \"Jaska\"; team[a].pos[c].x: 8888; team[a].age: 9999;";
	s += "b: team[a].pos[c].x; c: team[a].age; text t: team[a].name;";
	s += "person [] otherTeam: \n[\"A\", [[1,2], [3,4]], 34],\n [\"B\", [[5,6], [7,8]], 56],\n [\"C\", [[1,2], [9,0]], 78]";
		
	MSCode* m = new MSCode();
	(*m).compileAndRun(s);
	
	// variable test
	
	TEST((*m).getInt("a") == 2);
	TEST((*m).getInt("b") == 8888);
	TEST((*m).getInt("c") == 9999);
	TEST(((*m).getText("t").compare("Jaska")==0));
	
	// MSData access test
	
	MSDataArray arr = (*m).getArray("team");
	TEST(arr.getAt(2).getInt("age") == 9999);
	
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
	builder.addMember(personTypeID, "name", MS_TYPE_TEXT);
	
	// struct
	// TODO: make builder for other languages too
	
	// simple global values
	builder.addInt("aa", 123);
	builder.addText("key","value");
	builder.addInt("bb", 456);
	
	MSWriter pw = builder.createStruct("person", "boss");
	pw.setInt("age", 42);
	pw.setText("name", "Jaska");

//void MeanscriptUnitTest::addArray (int32_t typeID, std::string arrayName, int32_t arraySize) 
//MSWriter & MeanscriptUnitTest::arrayItem (std::string arrayName, int32_t arrayIndex)

	builder.addArray(personTypeID, "team", 3);
	MSWriter aw = builder.arrayItem("team", 1);
	aw.setInt("age", 67);

	builder.generate();
	MSCode* ms = builder.createMS();
	//(*ms).run();
	
	if (globalConfig.verboseOn()) (*ms).printData();
	
	TEST((*ms).getInt("bb") == 456);
	TEST(((*ms).getText("key").compare("value")==0));
	
	MSData bossData = (*ms).getData("boss");
	VERBOSE(bossData.getType());
	TEST(bossData.getInt("age") == 42);
	TEST(((*ms).getData("boss").getText("name").compare( "Jaska")==0));
	
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
		
	TEST_CATCH (E_UNEXPECTED_CHAR)
	return false;
}



void MeanscriptUnitTest:: runAll () 
{
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
// C++ END
