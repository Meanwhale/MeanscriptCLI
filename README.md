<img src=http://www.meanscript.net/logo.png width=50%>

_Command line interface (CLI) and C++ library for Meanscript, a versatile scripting and bytecode language._

<img src=nutshell.png width=75%>

Meanscript is a tool to create, save, and read data in a software, by using binary bytecode, human-readable script, and scripted functions.
It is still in the prototyping phase, but the plan is to combine the best parts of existing scripting languages like JavaScript, JSON, and Protocol Buffers,
while keeping it compact and easy to use.
 

Current features:
* **Parser** to convert text script to bytecode
* Bytecode **interpreter** to execute bytecode instructions
* **MSCode** object to access bytecode data
* **MSBuilder** to create data from your source code
* Abstract interfaces to make your own input/output streams
* **Command line tool** to compilie and run scripts and view contents of bytecode files
* **C++ library** to compile and run code, and read and write bytecode data from your source code
* Integers, text strings, floating point numbers, structs, and arrays
* Script functions
<!-- * Create custom input/output streams to read/write data
* _Web editor_ -->

New features coming up next
* Support for C# and Java, in addition to C++
* Generate C++/C#/Java code to read and write data structures
* Custom data types and callbacks (called from script or RPC)

More about Meanscript design in [ABOUT.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/ABOUT.md).

More about script syntax and bytecode format in [REFERENCE.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/REFERENCE.md).


**Contact**:<br>
meanwhale@gmail.com<br>
https://twitter.com/TheMeanwhale

## Project Content

This project contains C++ source code for Meanscript library, command line tool, and a demo program.
Visual Studio (2017) solution file is in the *src* folder. A *Makefile* for Linux build is in the root folder.

## 5 Minutes Tutorial

* **Microsoft Visual C++:** Open the solution file in _src_ folder and build the _MeanscriptCmd_ project.
* **Linux:** Run `make` at project's root folder to build a command line tool (GCC 6.1 or higher required).

After a successful build run `bin/mean` to see the command line arguments.

### Hello world

Crete a text file `hello.ms` that contains:

```
prints "Hello world!"
```

Then compile the script to a bytecode file `hello.mb` and run it with a terminal command:

```
bin/mean cr hello.ms hello.mb
```

If everything goes well, you see the text `Hello world!` in your terminal.

You can check the content of the bytecode file by command

```
bin/mean decode hello.mb
```

### C++ example

C++ code examples use environment variables `MS_INPUT` and `MS_OUTPUT` to point input and output directories, so set up them first, eg. on Linux

```
export MS_OUTPUT=~/meanscriptCLI
export MS_INPUT=~/meanscriptCLI
```

Build the demo project in Visual Studio project or run `make demo` in Linux. Execute the demo by running `bin/meandemo`.
File `src/Demo/DemoMain.cpp` shows how the demo program writes and reads a simple Meanscript bytecode file.

Next, try to read a script file from your source code. Create a file `test.ms` that contains

```
text name: "Meanscript"
int year: 2020
```

Then change the `main` function in `src/Demo/DemoMain.cpp` so that it looks like this:

```
int main()
{
  MSFileInStream input = getInput("test.ms", true);
  MSCode code (input, globalConfig.STREAM_SCRIPT);
  code.run(); // execute code to assign variable values
  cout<<"name is "<<code.getText("name")<<endl;
  cout<<"and the year is "<<code.getInt("year")<<endl;
  return 0;
}
```

Compile and run `bin/meandemo` to see the result.

## To be continued...

Meanscript is still work-in-progress, so stay connected for further updates.

Check out
[ABOUT.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/ABOUT.md)
and
[REFERENCE.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/REFERENCE.md)
for more information.

_Cheers, Juha H._
