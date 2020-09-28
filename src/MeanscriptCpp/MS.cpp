#include "MS.h"
#include <cstdarg>
#include <iostream>
#include <fstream>



namespace meanscript
{
	using namespace meanscriptcore;

	MSGlobal _globalConfig;
	const MSGlobal& globalConfig = _globalConfig;
	
	void setVerbose(bool b) { _globalConfig.verbose = b; }
	
	NullStream nullOut;

	MStdOutPrint verboseOut(std::cout);
	MStdOutPrint printOut(std::cout);
	MStdOutPrint errorOut(std::cerr);
	MSNullPrint nullPrint;
	
	MSOutputPrint& verbose() { if (globalConfig.verboseOn()) return verboseOut; return nullPrint; }
	MSOutputPrint& printer() { return printOut; }
	MSOutputPrint& errorPrinter() { return errorOut; }
	
	// Standard print

	MStdOutPrint::MStdOutPrint(std::ostream& _os) :
		os(_os)
	{
	}
	void MStdOutPrint::writeByte(uint8_t b) {
		os<<b;
	}
	MSOutputPrint& MStdOutPrint::print(std::string s) {
		os<<s;
		return *this;
	}
	MSOutputPrint& MStdOutPrint::print(std::int32_t i) {
		os<<i;
		return *this;
	}
	MSOutputPrint& MStdOutPrint::print(float f) {
		os<<f;
		return *this;
	}
	void MStdOutPrint::close() { }

	// Standard print

	MSNullPrint::MSNullPrint()
	{
	}
	void MSNullPrint::writeByte(uint8_t) {
	}
	MSOutputPrint& MSNullPrint::print(std::string) {
		return *this;
	}
	MSOutputPrint& MSNullPrint::print(std::int32_t) {
		return *this;
	}
	MSOutputPrint& MSNullPrint::print(float) {
		return *this;
	}
	void MSNullPrint::close() { }

	// file input/output
	MSFileInStream getInput(const char * fileName, bool usePathVar)
	{
		std::string inputFileName(fileName);
		if (usePathVar)
		{
			char * inputDir = std::getenv("MS_INPUT");
			if (inputDir == 0) ERROR("environmental variable MS_INPUT not set");
			inputFileName = inputDir + inputFileName;
			
		}
		MSPRINT("input: ").print(inputFileName).endLine();
		return MSFileInStream (inputFileName.c_str());
	}

	MSFileOutStream getOutput(const char * fileName, bool usePathVar)
	{
		std::string outputFileName(fileName);
		if (usePathVar)
		{
			char * outputDir = std::getenv("MS_OUTPUT");
			if (outputDir == 0) ERROR("environmental variable MS_OUTPUT not set");
			outputFileName = outputDir + outputFileName;
			
		}
		MSPRINT("output: ").print(outputFileName).endLine();
		return MSFileOutStream (outputFileName.c_str());
	}

	// MS file output stream

	MSFileOutStream::MSFileOutStream(const char * fileName) {
		fo = new std::ofstream();
		fo->open(fileName, std::ios::out | std::ios::binary);
		if (!fo->is_open())
		{
			printer().print("file not found: ").print(fileName).endLine();
			ERROR("MFileOutStream: exit");
		}
	}
	void MSFileOutStream::writeByte(uint8_t b) {
		if (!fo->is_open()) ERROR("file is closed");
		fo->write((const char *) &b, 1);
	}
	void MSFileOutStream::close() {
		if (!fo->is_open()) return;
		fo->flush();
		fo->close();
	}
	
	// MS file input stream
	
	MSFileInStream::MSFileInStream(const char * fileName){
		fi = new std::ifstream();
		fi->open(fileName,  std::ios::in | std::ios::binary | std::ios::ate);
		if (!fi->is_open())
		{
			printer().print("file not found: ").print(fileName).endLine();
			ERROR("MFileOutStream: exit");
		}
		fi->seekg (0, fi->end);
		size = (int)fi->tellg();
		fi->seekg (0, fi->beg);
	}
	int MSFileInStream::getByteCount() {
		return size;
	}
	uint8_t MSFileInStream::readByte() {
		char c;
		fi->read(&c, 1);
		return (uint8_t) c;
	}
	bool MSFileInStream::end() {
		return fi->peek() == EOF;
	}
	void MSFileInStream::close() {
		if (!fi->is_open()) return;
		fi->close();
	}

	// Utils
	

	int length(const char * c)
	{
		int i = 0;
		while (c[i] != 0)i++;
		return i + 1;
	};
	int32_t floatToIntBits(float f)
	{
		char * c = (char*)&f;
		int32_t i = 0;
		i |= (int32_t)((c[3] << 24)		& 0xff000000);
		i |= (int32_t)((c[2] << 16)		& 0x00ff0000);
		i |= (int32_t)((c[1] << 8)		& 0x0000ff00);
		i |= (int32_t)((c[0])			& 0x000000ff);
		return i;
	}
	unsigned char * copyToByteArray(const char * c, int size)
	{
		if (size < 0)
		{
			size = length(c);
		}
		unsigned char * b = new unsigned char[size];
		for (int i = 0; i < size; i++) b[i] = c[i];
		return b;
	}
	void println(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		putchar('\n');
	}
	void print(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
	}
	int nameTreeGet(std::map<std::string, int> & tree, std::string name)
	{
		if (tree.find(name) == tree.end())
		{
			msAssert(false, "nameTreeGet");
			return -1;
		}
		return tree[name];
	}
	std::string readStringFromIntArray(Array<int>& data, int offset, int count)
	{	
		char* bytes = new char[count];

	    int shift = 0;
	    for (int i = 0; i < count;)
	    {
	        bytes[i] = (byte)((data[offset + (i/4)] >> shift) & 0x000000FF);

	        i++;
	        if (i % 4 == 0) shift = 0;
	        else shift += 8;
	    }
		std::string s(bytes, count);
		delete[] bytes;
		return s;
	}
	void stringToIntBits(Array<int>& bits, std::string text)
	{
		int numChars = text.size();
		int size32 = (numChars/4) + 1;
		bits.reset(size32);

		for (int n=0; n<size32; n++) bits[n] = 0;
		
	    int shift = 0;
	    for (int i = 0; i < numChars;)
	    {
	        bits[i/4] += (text[i] & 0x000000ff) << shift;
	        
	        i++;
	        if (i % 4 == 0) shift = 0;
	        else shift += 8;
	    }
	}
	void printCompact(int * data, int top)
	{
		int tagIndex = 0;
		for (int i=0; i<top; i++)
		{
			if (i == tagIndex)
			{
				print("(%d)", (data[i] & OPERATION_MASK) >> OP_SHIFT); // TODO: print tag info
				tagIndex += instrSize(data[i]) + 1;
			}
			else
			{
				print("%x", data[i]);
				if (tagIndex > i+1) print(",");
			}
		}
		println("");
	}

	
	std::string toString(const char * s)
	{
		return std::string(s);
	}
	std::string toString(std::string s)
	{
		return s;
	}
	std::string toString(int i)
	{
		return std::to_string(i);
	}
	std::string toString(float f)
	{
		return std::to_string(f);
	}
}


void msAssert(bool b, const char* m)
{
	if (!b) msError(m,0,0,-1);
};

void msAssert(bool b, const char* m, const char* expression, const char* sourceFile, int lineNumber)
{
	if (!b) msError(m, expression, sourceFile, lineNumber);
};

void msError(const char* m, const char* expression, const char* sourceFile, int lineNumber)
{
	// TODO: use meanscript::errorPrinter()
	
	std::cerr<<"\n...ERROR... ";
	if (m!= 0) std::cerr<<m;
	if (sourceFile != 0) std::cerr<<"\nFILE: "<<sourceFile;
	if (lineNumber >= 0) std::cerr<<"\nLINE: "<<lineNumber;
	if (expression != 0) std::cerr<<"\nEXPR: "<<expression;
	std::cerr<<std::endl;
	std::exit(-1);
};