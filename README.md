# MeanscriptCLI
Command line interface (CLI) for Meanscript, a versatile scripting and bytecode language (prototype).

The idea is to combine best parts of existing scripting languages like JavaScript, JSON, and Protocol Buffers, while keeping it compact and easy to use.
 
Meanscript is currently featuring
* This **command line tool** to compile and run scripts and view contents of bytecode files
* **C++ library** to compile and run code, and read and write bytecode data from your source code
* Integers, text strings, floating point numbers, structs, and arrays
* Script functions
* Create custom input/output streams to read/write data
* _Web editor_

New features coming up next
* Support for C# and Java, in addition to C++
* Generate C++/C#/Java code to read and write data structures
* Custom data types and callbacks (called from script or RPC)

More about Meanscript _ABOUT.md_.

## Project Content

This project contains C++ source code for Meanscript library, command line tool, and a demo program.
Visual Studio (2017) solution file is in *src* folder.

A *Makefile* for Linux build is in root folder.
Run `make` to build a command line tool (GCC 6.1 or higher required).
After a successful build run `bin/mean` to see command line arguments.

## 5 (?) Minutes Tutorial
