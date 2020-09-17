#ifndef _MS
#define _MS

//#include <stdio.h>
//#include <iostream>
#include <fstream>
#include <map>
#include <string>

// Meanscript source macros 

#define MS_NO_EXCEPTIONS

#define CAT ).print(
#define DEBUG(x) {x;}
#define PR(a) printer().print(toString(a))
#define VR(a) verbose().print(a)
#define X(a) .print(a)
#define XHEX(i) .printHex(i)
#define XO .endLine()
#define PRINT(a) PR(a).endLine()
#define PRINTN(a) PR(a)
#ifndef MS_NO_EXCEPTIONS
#define CHECK(b,error,msg) {if (!(b)) {PR(msg); throw MException(error);}}
#else
#define CHECK(b,error,msg) {if (!(b)) {PR(msg); EXIT("");}}
#endif
#define SYNTAX(b,node,msg) {if (!(b)) {PR(msg); EXIT("");}}
#define TEST(b) ASSERT(b,"test error")
#define TRY try {
#define TEST_CATCH(err) } catch(MException e) { return &e.error == &err; }
#define VERBOSE(a) verbose().print(a).endLine()
#define VERBOSEN(a) verbose().print(a)
#define USER_PRINT PRINT



void msAssert(bool b, std::string);
void msAssert(bool b, std::string, const char*, const char*, int);

namespace meanscript
{
	
	class MSBuilder;
	class MSCode;
	class MSData;
	class MSDataArray;
	class MSGlobal;
	class MSInputArray;
	class MSInputStream;
	class MSOutputArray;
	class MSOutputPrint;
	class MSOutputStream;
	class MSWriter;
	

}

namespace meanscriptcore
{
	class ByteAutomata;
	class ByteCode;
	class StructDef; // switched place
	class Context;
	class Common;
	class Generator;
	class MArgs;
	class MCallback;
	class MeanMachine;
	class Meanscript;
	class MNode;
	class MVarList;
	class NodeIterator;
	class Parser;
	class Semantics;
	class TokenTree;
	class VarGen;
}

#if defined MS_DEBUG && defined MS_RELEASE
#error either MS_DEBUG or MS_RELEASE must be defined, not both
#elif MS_DEBUG
#define MS_BUILD_INFO "version 0.1 DEBUG"
#define HALT {__debugbreak(); std::exit(-1);}
#elif MS_RELEASE
#define MS_BUILD_INFO "version 0.1 RELEASE"
#define HALT std::exit(-1)
#else
#error MS_DEBUG or MS_RELEASE must be defined
#endif

#ifdef _DEBUG
     #define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
     #define new new
#endif

#define STR(x) #x
//#define ASSERT(x,msg) { if (!(x)) { printf("FAIL: (%s), file %s, line %d.\n", STR(x), __FILE__, __LINE__); printer().print(msg).endLine(); HALT; }}
//#define ASSERT(x,msg) { if (!(x)) { meanscript::printer().print("ASSERTION FAILED: \"").print(STR(x)).print("\"\nFILE: ").print(__FILE__).print("\nLINE: ").print(__LINE__).print("\nMESSAGE: ").print(msg).endLine(); HALT; }}
//#define ASSERT(x,msg) msAssert(x,toString(msg), STR(x), __FILE__, __LINE__);
#define ASSERT(x,msg) msAssert(x,"assertion failed", STR(x), __FILE__, __LINE__);
//#define EXIT(msg) { meanscript::printer().print("\nERROR:").endLine().print(msg).endLine(); HALT;}
#define EXIT(msg) { HALT; }

#define MSPRINT printer().print

#include "Array.h"
#include "Unique.h"


#ifndef _INCLUDE_GENERATED_
#define _INCLUDE_GENERATED_
#include "ByteAutomata.h"
#include "ByteCode.h"
#include "StructDef.h" // switched place
#include "MSInputStream.h" // switched place
#include "MSOutputStream.h" // switched place
#include "Context.h"
#include "Common.h"
#include "Generator.h"
#include "MArgs.h"
#include "MCallback.h"
#include "MeanMachine.h"
#include "MeanscriptUnitTest.h"
#include "MSInputArray.h"
#include "MNode.h"
#include "MSBuilder.h"
#include "MSCode.h"
#include "MSData.h"
#include "MSDataArray.h"
#include "MSOutputArray.h"
#include "MSOutputPrint.h"
#include "MSWriter.h"
#include "MSGlobal.h"
//#include "MVarList.h"
#include "NodeIterator.h"
#include "Parser.h"
#include "Semantics.h"
// StructDef
#include "TokenTree.h"
#include "VarGen.h"
#endif


namespace meanscript
{
	// Silent stream for verbose etc.

	class NullStream : public std::ostream {
	public:
	  NullStream() : std::ostream(nullptr) {}
	  NullStream(const NullStream &) : std::ostream(nullptr) {}
	};

	template <class T>
	const NullStream &operator<<(NullStream &&os, const T &value) { 
	  return os;
	}

	// Global config. TODO: read configs from CLI or file.

	extern const MSGlobal& globalConfig;

	void setVerbose(bool);

	// Standard printers

	class MStdOutPrint : public MSOutputPrint {
	public:
		MStdOutPrint();
		virtual void writeByte(uint8_t) override;
		virtual MSOutputPrint& print(std::string) override;
		virtual MSOutputPrint& print(int32_t) override;
		virtual MSOutputPrint& print(float) override;
		virtual void close() override;
	};

	class MSNullPrint : public MSOutputPrint {
	public:
		MSNullPrint();
		virtual void writeByte(uint8_t) override;
		virtual MSOutputPrint& print(std::string) override;
		virtual MSOutputPrint& print(int32_t) override;
		virtual MSOutputPrint& print(float) override;
		virtual void close() override;
	};

	extern MStdOutPrint verboseOut;
	MSOutputPrint& verbose();

	extern MStdOutPrint printOut;
	MSOutputPrint& printer();

	// Meanscript file in/output 


	class MSFileOutStream : public MSOutputStream {
	private:
		std::ofstream* fo;
		
		MSFileOutStream() = delete;
		MSFileOutStream & operator = (const MSFileOutStream &) = delete;
		MSFileOutStream & operator & () = delete;
		MSFileOutStream * operator * () = delete;

	public:
		MSFileOutStream(const MSFileOutStream&) = default;
		MSFileOutStream(const char *);
		~MSFileOutStream() { close(); delete fo; }
		virtual void writeByte(uint8_t) override;
		virtual void close() override;
	};

	class MSFileInStream : public MSInputStream {
	private:
		int size;
		std::ifstream* fi;
		
		MSFileInStream() = delete;
		MSFileInStream & operator = (const MSFileInStream &) = delete;
		MSFileInStream & operator & () = delete;
		MSFileInStream * operator * () = delete;

	public:
		MSFileInStream(const MSFileInStream&) = default;
		MSFileInStream(const char *);
		~MSFileInStream() { close(); delete fi; }
		virtual int getByteCount() override;
		virtual uint8_t readByte() override;
		virtual bool end() override;
		virtual void close() override;
	};
	
	MSFileInStream getInput(const char * fileName, bool usePathVar = false);
	MSFileOutStream getOutput(const char * fileName, bool usePathVar = false);

	// Error class 

	class MSError
	{
	public:
		const MSError& type;
		const char * title;
		MSError (const char* _title) :
			type(*this),
			title(_title) {}
		MSError (const MSError& _type, const char* _title) :
			type(_type),
			title(_title) {}
	};

	// Exception

	class MException : std::exception
	{
	public:
		const MSError& error;
		MException(const MSError& e) : error(e) {};
	};

	// Utilities


	typedef unsigned char byte;

	int length(const char *);
	unsigned char * copyToByteArray(const char *, int length = -1);

	void println(const char* format, ...);
	void print(const char* format, ...);
	const char * hexStr(int h);
	void printCompact(int *, int top);
	int nameTreeGet(std::map<std::string, int> &, std::string);
	std::string readStringFromIntArray(Array<int> &, int offset, int count);
	void stringToIntBits(Array<int>&, std::string);
	
	std::string toString(const char *);
	std::string toString(std::string);
	std::string toString(int);
	std::string toString(float);
}

// constants
#ifndef _MC_H_
#define _MC_H_
#include "MC.h"
#endif

#endif