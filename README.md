
<img src=nutshell.png>

## Command line interface (CLI) and C++ library for Meanscript

Meanscript is a versatile scripting and bytecode language to create, save, and read data.
It is still **in the prototyping phase**, but the plan is to combine the best parts of existing scripting languages like JavaScript, JSON, and Protocol Buffers,
while keeping it compact and easy to use.
 

### Current features:

* **Parser:** convert text script to bytecode
* Bytecode **interpreter:** execute bytecode instructions
* **MSCode:** a C++ class to access bytecode data from your source code
* **MSBuilder:** create data from your source code
* Abstract interfaces to make your own input/output streams
* **Command line tool:** compile and run scripts and view contents of bytecode files
* **C++ library:** compile and run code, and read and write bytecode data from your source code
* Integers, text strings, floating point numbers, structs, arrays, and functions
<!-- * Create custom input/output streams to read/write data
* _Web editor_ -->

### New features coming up next
* Support for C# and Java, in addition to C++
* Generate C++/C#/Java code to read and write data structures
* Custom data types and callbacks (called from script or RPC)

Meanscript is implemented in syntax that is common to C++, C#, and Java, and a lot of macros (gcc).
That's how to generate code to multiple languages at the same time.
Same technique is used in Meanscript's side project **ByteAutomata**,
that is a tool for making a hard-coded parser. Check it out:<br>
https://github.com/Meanwhale/ByteAutomata

More about Meanscript design in [ABOUT.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/ABOUT.md).

More about script syntax, API, etc. in [REFERENCE.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/REFERENCE.md).


**Contact**:<br>
meanwhale@gmail.com<br>
https://twitter.com/TheMeanwhale

<!--

## Project Content

This project contains

* C++ source code for Meanscript library
* Command line tool
* Demo program
* Visual Studio (2017) solution (_src_ folder)
* _Makefile_ for Linux build is in the root folder

-->

## Build

### Microsoft Visual C++

* Open the solution file in _src_ folder
* _MeanscriptCmd_: command line interface
* _Demo_: demo program

###Linux

* Requirement: GCC 6.1 or higher
* Run `make` at project's root folder to build a command line tool
* `make demo`: demo project
* `make debug`: CLI debug build

Executables are built to `bin/` folder.
Add it to your `PATH` to execute them from any folder.

## 5 Minutes Tutorial

<!--### Hello world-->

Create a text file `hello.ms` that contains:

```
prints "Hello world!"
```

Then compile the script to a bytecode file `hello.mb` and run it:

```
mean cr hello.ms hello.mb
```

If everything goes well, you see the text `Hello world!` in your terminal.

You can check the content of the bytecode file:

```
mean decode hello.mb
```

### C++ example

C++ code examples use environment variables `MS_INPUT` and `MS_OUTPUT` to define input and output file directories, so set up them first, eg. on Linux

```
export MS_OUTPUT=/home/johndoe/meanscript
export MS_INPUT=/home/johndoe/meanscript
```

Build the demo and execute it by running `meandemo`.
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

Compile and run `meandemo` to see the result!

## To be continued...

Meanscript is still work-in-progress, so stay connected for further updates.

Check out
[ABOUT.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/ABOUT.md)
and
[REFERENCE.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/REFERENCE.md)
for more information.

_Cheers, Juha H._
