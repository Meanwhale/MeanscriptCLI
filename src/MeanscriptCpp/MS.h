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
#define CATHEX ).printHex(
#define DEBUG(x) {x;}
#define ERROR_PRINT(a) errorPrinter().print(a).endLine()
#define PRINT(a) printer().print(a).endLine()
#define PRINTN(a) printer().print(a)
#define VERBOSE(a) {if(globalConfig.verboseOn()) printer().print(a).endLine();}
#define VERBOSEN(a) {if(globalConfig.verboseOn()) printer().print(a);}
#define USER_PRINT PRINT
#define PRINT_STREAM printer()

void msAssert(bool b, const char*);
void msAssert(bool b, const char*, const char*, const char*, int);
void msError(const char*, const char*, const char*, int);

namespace meanscriptcore
{
	class NodeIterator;
}
void msSyntaxAssert(bool b, meanscriptcore::NodeIterator* node, const char* msg);
void msSyntaxAssert(bool b, meanscriptcore::NodeIterator& node, const char* msg);

#if defined MS_DEBUG && defined MS_RELEASE
#error either MS_DEBUG or MS_RELEASE must be defined, not both
#elif MS_DEBUG
#define MS_BUILD_INFO "version 0.1 DEBUG"
#elif MS_RELEASE
#define MS_BUILD_INFO "version 0.1 RELEASE"
#else
#error MS_DEBUG or MS_RELEASE must be defined
#endif

#ifdef MS_VS_MEM_DEBUG 
#define USE_DEBUG_BREAK
#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif

#define STR(a) #a

// Error handling:
//		* SYNTAX: user script error. Print error cause and position in script, and exit.
//		* CHECK: user error when using MS library.
//		* ASSERT/ERROR: internal error that shouldn't normally happen.
//		* CHECK/ASSERT/ERROR:
//			* MS_DEBUG: trigger break.
//			* MS_RELEASE: Print info and source code location, and exit.

// NOTE: print in-line so that CAT works!

#ifdef USE_DEBUG_BREAK
#define ASSERT(x,msg) {if (!(x)) {__debugbreak(); std::exit(-1);}}
#define ERROR(msg) ASSERT(false,msg);
#else
#define ASSERT(x,msg) {if (!(x)){errorPrinter().print(msg).endLine(); msError("", STR(x), __FILE__, __LINE__);}}
#define ERROR(msg) {errorPrinter().print(msg).endLine(); msError(0, 0, __FILE__, __LINE__);}
#endif

#ifndef MS_NO_EXCEPTIONS
#define CHECK(b,error,msg) {if (!(b)) {PR(msg); throw MException(error);}}
#else
#define CHECK(b,error,msg) {if (!(b)) {errorPrinter().print(msg).endLine(); ERROR("");}}
#endif
#define SYNTAX(b,node,msg) {if (!(b)) {errorPrinter().print(msg).endLine(); ERROR("");}}
#define TEST(b) ASSERT(b,"test error")

#define TRY try {
#define TEST_CATCH(err) } catch(MException e) { return &e.error == &err; }

#define MSPRINT printer().print

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
	//class MSOutputPrint;
	class MSOutputStream;
	class MSText;
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


#include "Array.h"
#include "Unique.h"


#ifndef _INCLUDE_GENERATED_
#define _INCLUDE_GENERATED_
#include "core/ByteAutomata.h"
#include "core/ByteCode.h"
#include "core/StructDef.h"
#include "pub/MSInputStream.h"
#include "pub/MSOutputStream.h"
#include "pub/MSOutputPrint.h"
#include "pub/MSOutputPrintArray.h"
#include "core/ClassMaker.h"
#include "core/Context.h"
#include "core/Common.h"
#include "core/Generator.h"
#include "core/MArgs.h"
#include "core/MCallback.h"
#include "core/MeanMachine.h"
#include "core/MeanscriptUnitTest.h"
#include "pub/MSText.h"
#include "core/MNode.h"
#include "pub/MSInputArray.h"
#include "pub/MSBuilder.h"
#include "pub/MSCode.h"
#include "pub/MSData.h"
#include "pub/MSDataArray.h"
#include "pub/MSOutputArray.h"
#include "pub/MSWriter.h"
#include "pub/MSGlobal.h"
//#include "MVarList.h"
#include "core/NodeIterator.h"
#include "core/Parser.h"
#include "core/Semantics.h"
// StructDef
#include "core/TokenTree.h"
#include "core/VarGen.h"
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

	void setVerbose(bool); // access beyond public const

	// Standard printers

	class MStdOutPrint : public MSOutputPrint {
	private:
		MStdOutPrint() = delete;
		std::ostream& os;
	public:
		MStdOutPrint(std::ostream&);
		virtual void writeByte(uint8_t) override;
		virtual MSOutputPrint& print(const char*) override;
		virtual MSOutputPrint& print(std::string) override;
		virtual void close() override;

		// Inherited via MSOutputPrint
		virtual MSOutputPrint& print(char) override;
	};

	class MSNullPrint : public MSOutputPrint {
	public:
		MSNullPrint();
		virtual void writeByte(uint8_t) override;
		virtual MSOutputPrint& print(const char *) override;
		virtual MSOutputPrint& print(std::string) override;
		virtual void close() override;

		// Inherited via MSOutputPrint
		virtual MSOutputPrint& print(char) override;
	};

	extern MStdOutPrint verboseOut;
	MSOutputPrint& verbose();
	
	extern MStdOutPrint printOut;
	MSOutputPrint& printer();

	extern MStdOutPrint errorOut;
	MSOutputPrint& errorPrinter();

	// Meanscript file in/output 


	class MSFileOutStream : public MSOutputStream {
	private:
		std::ofstream* fo;

		MSFileOutStream() = delete;
		MSFileOutStream& operator = (const MSFileOutStream&) = delete;
		MSFileOutStream& operator & () = delete;
		MSFileOutStream* operator * () = delete;

	public:
		MSFileOutStream(const MSFileOutStream&) = default;
		MSFileOutStream(const char*);
		~MSFileOutStream() { close(); delete fo; }
		virtual void writeByte(uint8_t) override;
		virtual void close() override;
	};

	class MSFilePrint : public MSOutputPrint {
	private:
		std::ofstream* fo;

		MSFilePrint() = delete;
		MSFilePrint& operator = (const MSFilePrint&) = delete;
		MSFilePrint& operator & () = delete;
		MSFilePrint* operator * () = delete;
		MSFilePrint(const MSFilePrint&) = delete;

	public:
		MSFilePrint(std::string folderName, std::string className, std::string extension);
		~MSFilePrint() { close(); delete fo; }

		// Inherited via MSOutputPrint
		virtual void writeByte(uint8_t) override;
		virtual MSOutputPrint& print(const char *) override;
		virtual MSOutputPrint& print(std::string) override;
		virtual void close() override;

		// Inherited via MSOutputPrint
		virtual MSOutputPrint& print(char) override;
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
		virtual bool end() override;
		virtual void close() override;

		// Inherited via MSInputStream
		virtual int32_t readByte() override;
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
	
	char filePathSeparator();
	int length(const char *);
	int32_t floatToIntBits(float);
	unsigned char * copyToByteArray(const char *, int length = -1);
	bool compareStrings(const std::string&, const std::string&);

	void println(const char* format, ...);
	void print(const char* format, ...);
	void printCompact(int *, int top);
	int nameTreeGet(std::map<MSText, int>& tree, MSText* name);
	std::string readStringFromIntArray(Array<int> &, int offset, int count);
	void stringToIntBits(Array<int>&, std::string);
	
	std::string toString(const char *);
	std::string toString(std::string);
	std::string toString(int);
	std::string toString(float);

	void nativeTest();
}

// constants
#ifndef _MC_H_
#define _MC_H_
#include "core/MC.h"
#endif

#endif