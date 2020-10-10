
<img src=nutshell.png>

## Command line interface (CLI) and C++ library for Meanscript

<!-- USE CASES -->

**Light-weight**, **stand-alone** tool to save and read data in both human-readable **text script** and **bytecode** formats.


Meanscript is <!-- a versatile scripting and bytecode language to create, save, and read data.
It is --> still in the **prototyping phase**, but the plan is to combine the best parts of existing techniques like XML, JSON, and Protocol Buffers,
while keeping it compact and easy to use.
 

## Featuring

* **Scripting language** to store and transfer data using variables, data structures, and functions.
* **Bytecode format** compiled from a script or generated from source code.
* **Command line interface** (CLI) to compile and run code from a terminal.
* **C++ library** to compile scripts, and read and write bytecode data from C++ source code.

<!-- * **Parser:** convert text script to bytecode
* Bytecode **interpreter:** execute bytecode instructions
* **MSCode:** access bytecode data from your source code
* **MSBuilder:** create data from your source code
* **Command line application:** compile and run scripts and view contents of bytecode files
* **C++ library:** compile and run code, and read and write bytecode data from your source code
* Integers, text strings, floating point numbers, structs, arrays, and functions
* Create custom input/output streams to read/write data
* _Web editor_ -->

### New features coming up next
* Support for C# and Java, in addition to C++
* Generate C++/C#/Java code to read and write data structures, like Protocol Buffers does
* Custom data types and callbacks (called from script or RPC)

Meanscript is implemented in syntax that is common to C++, C#, and Java, and a lot of macros (GCC).
That's how to generate code to multiple languages at the same time.
Same technique is used in Meanscript's side project **ByteAutomata**,
which is a toolset for a hard-coded parser in C++, C#, and Java. Check it out:<br>
https://github.com/Meanwhale/ByteAutomata

See [EXAMPLES.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/EXAMPLES.md) for some script and C++ examples.

More about script syntax, API, etc. in [REFERENCE.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/REFERENCE.md).

More about Meanscript design in [ABOUT.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/ABOUT.md).


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

* Requirement: Visual Studio 2017 with C++ (older versions might work but not tested)
* Open the solution file in _src_ folder
* Build the command line interface: _MeanscriptCmd_
* Build the demo application: _Demo_

### Linux

* Requirement: GCC 6.1 or higher
* Run `make` at project's root folder to build a command line tool
* `make demo`: demo project
* `make debug`: CLI debug build

Executables are built to `bin/` folder.
Add it to your `PATH` environment variable to run the executables from any folder.

## 5 Minutes Tutorial

<!--### Hello world-->

Create a text file `hello.ms` that contains:

```
prints "Hello world!"
```

Compile the script to a bytecode file `hello.mb` and run it:

```
mean cr hello.ms hello.mb
```

If everything goes well, you'll see the text `Hello world!` in your terminal.

You can check the content of the bytecode file:

```
mean decode hello.mb
```

### C++ API example

Examples use environment variables `MS_INPUT` and `MS_OUTPUT` to define input and output file directories, so set them up first, eg. on Linux

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

Then change the `main()` function in `src/Demo/DemoMain.cpp` so that it looks like this:

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

Meanscript is still work-in-progress, so stay connected for further updates by **Starring** this project and/or Twitter:

https://twitter.com/TheMeanwhale

Check out
[ABOUT.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/ABOUT.md)
and
[REFERENCE.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/REFERENCE.md)
for more information.

_Cheers, Juha H._
