# Meanscript: Quick Refenrence

## Script syntax reference

<pre>int a                   // define integer 'a' with a default value (0)
int a: 5                // define with an initial value of 5
text name: "Jack"       // define immutable string
bool b                  // define boolean

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
