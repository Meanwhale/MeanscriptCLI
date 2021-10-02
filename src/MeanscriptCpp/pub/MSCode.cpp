#include "MS.h"
namespace meanscript {
using namespace meanscriptcore;

MSCode::MSCode () 
{
	common = new Common();
	mm = 0;
	initialized = false;
	
	reset();
}
MSCode::MSCode (MSInputStream & input, int32_t streamType) 
{
	ASSERT(streamType >= globalConfig.STREAM_TYPE_FIRST && streamType <= globalConfig.STREAM_TYPE_LAST, "unknown stream type");
	
	common = new Common();
	mm = 0;
	initialized = false;
	
	if (streamType == globalConfig.STREAM_BYTECODE)
	{
		initBytecode(input);
	}
	else if (streamType == globalConfig.STREAM_SCRIPT)
	{
		mm = new MeanMachine(compile(input));
	}
	else
	{
		ASSERT(false, "unknown stream type");
	}
}

MeanMachine* MSCode::getMM ()
{
	return mm;
}

MSCode::~MSCode() { reset(); delete common; };

void MSCode::reset ()
{
	{ delete mm; mm = 0; };
	initialized = false;
}

bool MSCode::isInitialized ()
{
	return initialized;
}

void MSCode::checkInit () 
{
	ASSERT(initialized, "MSCode is not initialized");
}

void MSCode::initBytecode (MSInputStream & input) 
{
	reset();
	
	ByteCode* byteCode = new ByteCode(common, input);
	mm = new MeanMachine(byteCode);
	
	initialized = true;
}

void MSCode::initBytecode (ByteCode* bc) 
{
	reset();
	
	ByteCode* byteCode = new ByteCode((*bc));
	mm = new MeanMachine(byteCode);
	
	initialized = true;
}

bool MSCode::hasData (std::string name) 
{
	return (*(*mm).globals).hasData(name);
}

bool MSCode::hasArray (std::string name) 
{
	return (*(*mm).globals).hasArray(name);
}

bool MSCode::getBool (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getBool(name);
}

int32_t MSCode::getInt (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getInt(name);
}

int64_t MSCode::getInt64 (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getInt64(name);
}

float MSCode::getFloat (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getFloat(name);
}
double MSCode::getFloat64 (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getFloat64(name);
}

std::string MSCode::getText (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getText(name);
}

std::string MSCode::getChars (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getChars(name);
}

std::string MSCode::getText (int32_t textID) 
{
	checkInit();
	return (*(*mm).globals).getText(textID);
}

MSData MSCode::getData (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getMember(name);
}

MSDataArray MSCode::getArray (std::string name) 
{
	checkInit();
	return (*(*mm).globals).getArray(name);
}

void MSCode::writeReadOnlyData (MSOutputStream & output) 
{
	(*mm).writeReadOnlyData(output);
}

void MSCode::writeCode (MSOutputStream & output) 
{
	(*mm).writeCode(output);
}

void MSCode::printCode () 
{
	if (initialized) (*mm).printCode();
	else PRINT("printCode: MSCode is not initialized");
}

void MSCode::printDetails () 
{
	if (initialized) (*mm).printDetails();
	else PRINT("printDetails: MSCode is not initialized");
}

void MSCode::printData () 
{
	if (initialized) (*mm).dataPrint();
	else PRINT("printDetails: MSCode is not initialized");
}

void MSCode::dataOutputPrint (MSOutputPrint & output) 
{
	(*(*mm).globals).printData(output,0,"");
	output.close();
}

ByteCode* MSCode::compile (MSInputStream & input) 
{
	reset();
	
	TokenTree* tree = Parser::Parse (input);
	Semantics semantics = Semantics();
	(*common).initialize(semantics);
	semantics.analyze(tree);
	ByteCode* bc = Generator::generate (tree, semantics, (*common));
	{ delete tree; tree = 0; };	
	
	initialized = true;
	return bc;
}

void MSCode::run () 
{
	ASSERT(mm != 0, "not initialized");
	(*mm).callFunction(0);
}

void MSCode::step () 
{
	ASSERT(false, "TODO");
}

void MSCode::compileAndRun (std::string s) 
{
	reset();
	MSInputArray* ia = new MSInputArray(s);
	ByteCode* bc = compile((*ia));
	{ delete ia; ia = 0; };
	mm = new MeanMachine(bc);
	run();
}

//void MSCode::compileAndRun (uint8_t * script) 
//{
//	compile(script);
//	run();
//}


} // namespace meanscript(core)
