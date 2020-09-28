#include <iostream>
#include "MS.h"
using namespace std;
using namespace meanscript;

int main()
{
	MSBuilder builder("demo");
	builder.addInt("foo", 123);
	builder.generate();

	MSFileOutStream output = getOutput("demo.mb", true);
	builder.write(output);

	MSFileInStream input = getInput("demo.mb", true);
	MSCode code (input, globalConfig.STREAM_BYTECODE);
	cout<<"FOO: "<<code.getInt("foo")<<endl;

	return 0;
}
