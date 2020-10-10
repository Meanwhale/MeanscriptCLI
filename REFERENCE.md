# Meanscript: Quick Refenrence

Meanscript is still work-in-progress so the documentation is quite minimal.
To explore Meanscript capabilities, see header files in the public folder:

https://github.com/Meanwhale/MeanscriptCLI/tree/master/src/MeanscriptCpp/pub

Check out the unit tests for more information of library usage:

https://github.com/Meanwhale/MeanscriptCLI/blob/master/src/MeanscriptCpp/core/MeanscriptUnitTest.cpp

## Public interface classes

class | description
------|------------
MSBuilder | Create bytecode data and data structures from source code.
MSCode | Read and access script and bytecode data.
MSData | Any type of variable or structure.
MSDataArray | Array of data, i.e. _MSData_ objects of certain type.
MSGlobal | Global constant values for the engine.
MSInputStream, MSOutputStream | An abstract interfaces for input/output.
MSInputArray, MSOutputArray | Array holders for streaming data.
MSOutputPrint | An abstract _MSOutputStream_ interface to print data.
MSWriter | A writer class for the _MSBuilder_.

## Script syntax reference

<pre>// basic variable types:
int a                   // define an integer 'a' with a default value (0)
int a: 5                // define it with an initial value of 5
float f: 123.456        // define a floating-point (32-bit decimal) number
text name: "Jack"       // define an immutable string
bool b                  // define a boolean

// Function calls can be of two formats:
// 1. Argument list separated with spaces, like on command line:
//        function arg1 arg2 ...
// 2. Argument list in brackets, separated with commands:
//        function ( arg1 , arg2 , ... )
// If an argument is a function call (return value), it's in brackets:
//        1. function_a arg_a1 (function_b arg_b1 arg_b2 ...) arg_a3 ...
//        2. function_a (arg_a1, ( function_b ( arg_b1, arg_b2, ... ) ) , arg_a3 , ... )

// common functions:
sum (a, b)              // return a+b
sum a b                 // alternative format for the same call, without brackets
sub (a, b)              // return a-b
eq (a, b)               // return true if a=b or else return false
if (condition) {code}   // execute code if the condition is true
print a                 // prints an integer 
prints name             // prints a string

// define a struct with two members: name and id
struct person [text name, int id]     
person p                 // define struct variable
person p2: "John", 5432  // define struct variable with initial values
p.name: "Jack"           // assign struct variable member

// array
person [4] team          // define an array of size four
team[0].name: "Jane"     // modify the first 'person'
team[sum(1,2)].id: 5738  // modify the last person (index = 3)

// define a function that returns a value
func int increase [int foo] { return (sum foo 1) }
</pre>

<!--## MSCode quick reference

Use _MSCode_ to read script and bytecode, execute code, and access data.

C++ code: https://github.com/Meanwhale/MeanscriptCLI/blob/master/src/MeanscriptCpp/pub/MSCode.h

MSCode(MSInputStream & input, int32_t streamType);   // constructor. see MSGlobal.h for stream types
void compileAndRun (std::string s);                  // compile and run a string of script
int32_t getInt (std::string name);                   // get an integer by name
float getFloat (std::string name);                   // get a float by name
std::string getText (std::string name);              // get a text string by name
MSData getData (std::string name);                   // get any type of data (also struct) by name
MSDataArray getArray (std::string name);             // get a data array by name

MSBuilder quick reference-->

## Bytecode format

Bytecode consists of 32-bit words that can be code or data.

For example there 5 words define a text "Meanscript" constant:

<pre>0:   0x10040002      Text adding operation (hex)
1:   10              Number of characters
2:   1851876685      Text character in bytes
3:   1769104243
4:   29808</pre>

32-bit operation content:

* bits 0-7 from left: operation type (0x10 above: add text content)
* bits 8-15: operation size, i.e. offset to the next operation (0x04 above)
* bits 16-31: data type (0x0002 is text type)

At the beginning of a bytecode file is the initialization part, where text constants, structures, and functions are defined.
After the initialization follows function code, including assigning global values.

Use command `mean decode` to explore bytecode content further!


## List of bytecode operations:

C++ code: https://github.com/Meanwhale/MeanscriptCLI/blob/master/src/MeanscriptCpp/core/MC.h

<pre>START_INIT                  Start code initialization
END_INIT                    End code initialization
ADD_TEXT                    Add a constant text
STRUCT_DEF                  Define a variable structure
STRUCT_MEMBER               Define a structure member
ARRAY_MEMBER                Define an array member
FUNCTION                    Define a function
MEMBER_NAME                 Define a structure member

SYSTEM                      System calls, like errors (TBD)
NOOP                        Do nothing
SAVE_BASE                   Save the stack pointer
LOAD_BASE                   Load the stack pointer
CALLBACK_CALL               Call a source code function
FUNCTION_CALL               Call a script function
JUMP                        Jump to an address
GO_BACK                     Go back to previous address
GO_END                      Go to end of a code block

PUSH_IMMEDIATE              Push an immediate (constant) value to stack
PUSH_REG_TO_STACK           Push register content
PUSH_LOCAL                  Push a local value
PUSH_GLOBAL                 Push a global value
POP_STACK_TO_LOCAL          Pop the stack and copy to a local address
POP_STACK_TO_GLOBAL         Pop the stack and copy to a global address
POP_STACK_TO_REG            Pop the stack and copy to the register

POP_STACK_TO_LOCAL_REF      Pop the stack and copy to a local reference (address)
POP_STACK_TO_GLOBAL_REF     Pop the stack and copy to a global reference
PUSH_LOCAL_REF              Push a local reference
PUSH_GLOBAL_REF             Push a global reference

MULTIPLY_GLOBAL_ARRAY_INDEX Multiply to calculate an array item address
INIT_GLOBALS                Initialize all global values
</pre>
