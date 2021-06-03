# Meanscript: Quick Refenrence

Meanscript is still work-in-progress so the documentation is quite minimal.
To explore its current features more explore the public interface in header files:

https://github.com/Meanwhale/MeanscriptCLI/tree/master/src/MeanscriptCpp/pub

Check out the unit tests for use examples:

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

## Script syntax quick reference

<pre>// basic variable types:
int a                   // define an integer 'a' with a default value (0)
int a: 5                // define it with an initial value of 5
float f: 123.456        // define a floating-point (32-bit decimal) number
text name: "Jack"       // define an immutable string
chars [12] ch: "Jill"   // define a mutable, fixed-sized string (max. 21 bytes)
bool b                  // define a boolean

// Function calls can be of two formats:
// 1. Argument list separated with spaces, like on command line:
//        function arg1 arg2 ...
// 2. Argument list in brackets, separated with commands:
//        function ( arg1 , arg2 , ... )
// If an argument is a function call (return value), it's in brackets:
//        1. function_a arg_a1 (function_b arg_b1 arg_b2 ...) arg_a3 ...
//        2. function_a (arg_a1, ( function_b ( arg_b1, arg_b2, ... ) ) , arg_a3 , ... )

// basic functions:
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

## Bytecode format

Bytecode consists of 32-bit words that can be an operation or data.

32-bit operation content:

bits  | mask         | description
------|--------------|------------
0-7   | `0xff000000` | Operation code. See list of operations below.
8-15  | `0x00ff0000` | Operation size, i.e. offset to the next operation.
16-31 | `0x0000ffff` | Data type of the operation target.

For example, here's an instruction to define a text "Meanscript" constant:

<pre>0:   0x10040002      Text adding operation (hex)
1:   10              Number of characters
2:   1851876685      Text characters in ASCII codes
3:   1769104243
4:   29808</pre>

The operation code above is `0x10` for adding a constant text.
The instruction size is `0x04` i.e. 4 32-bit words (excluding the operation).
The data type is `0x0004`, a constant text.

## Bytecode structure

At the beginning of a bytecode file is the initialization part, where text constants, structures, and functions are defined.
After the initialization follows function code, including assigning global values.

For example, compiling this script

```
int a: 5
text foo: "bar"
struct person [text name, int id] 
func int increase [int foo] {
  return (sum foo 1)
}
```

results this bytecode (hex codes, starting with `0x`, are operation codes (`OP`)and others are operation data):

```
address   operation or data   description
--------  ------------------  ------------------------------------
    0:    0x15010102          OP: start init
    1:    2                   
    2:    0x10020002          OP: text
    3:    3                   constant text "foo", length 3
    4:    7496034             ASCII bytes
    5:    0x08020000          OP: struct definition
    6:    0                   struct for global variables
    7:    0    
    8:    0x1d020002          OP: member name
    9:    1                   variable "a" name
    10:   97    
    11:   0x09020001          OP: struct member
    12:   0    
    13:   1    
    14:   0x1d020002          OP: member name
    15:   3    
    16:   7303014    
    17:   0x09020002          struct member
    18:   1    
    19:   1    
    20:   0x08030010          struct definition
    21:   6    
    22:   1936876912    
    23:   28271    
    24:   0x1d030002          OP: member name
    25:   4    
    26:   1701667182    
    27:   0    
    28:   0x09020002          OP: struct member
    29:   0    
    30:   1    
    31:   0x1d020002          OP: member name
    32:   2    
    33:   25705    
    34:   0x09020001          OP: struct member
    35:   1    
    36:   1    
    37:   0x14050000          OP: function data
    38:   0    
    39:   50    
    40:   2    
    41:   -1    
    42:   60    
    43:   0x14050000          OP: function data
    44:   1    
    45:   61    
    46:   1    
    47:   1    
    48:   73    
    49:   0x16000000          OP: end init
    50:   0x11010001          OP: push immediate
    51:   5    
    52:   0x1b020000          OP: pop to global
    53:   1    
    54:   0    
    55:   0x11010002          OP: push immediate
    56:   1    
    57:   0x1b020000          OP: pop to global
    58:   1    
    59:   1    
    60:   0x05000000          OP: go back
    61:   0x18020001          OP: push local
    62:   0    
    63:   1    
    64:   0x11010001          OP: push immediate
    65:   1    
    66:   0x03010000          OP: call
    67:   16    
    68:   0x13010000          OP: push from reg.
    69:   1    
    70:   0x1c010001          OP: pop to register
    71:   1    
    72:   0x06000000          OP: go end
    73:   0x05000000          OP: go back
```

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
