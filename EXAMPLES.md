# Meanscript Examples

Here are some examples of how to use Meanscript.

Quick reference:<br>
https://github.com/Meanwhale/MeanscriptCLI/blob/master/REFERENCE.md

## Scripted Structures

The script below defines a structure `vec` that has two integer members `x` and `y`.

<pre>
struct vec [
  int x,
  int y
]
</pre>

Structure `person` has a member of type `vec` called `pos` in addition to text `name` and integer `age`.

<pre>
struct person [
  vec pos,
  text name,
  int age
]
</pre>

Assign values for a structure variable separated with comman, in their respective order.

<pre>
vec position: 12, 34
person jackOne: position, "Jack", 45
person jackTwo: [12,34], "Jack", 45
</pre>

In the script above `jackOne` and `jackTwo` have equal values.

## Bytecode Builder

Use MSBuilder to create bytecode data in your source code.

Writing simple variables to a bytecode:

<pre>
MSBuilder builder ("test");

builder.addInt("aa", 123);
builder.addText("key","value");

MSFileOutStream output = getOutput("textcode.mb", true);
builder.write(output);
</pre>

That results a similar data as scripting

<pre>
int aa: 123
text key: "value"
</pre>

You can create structures and structure variables with MSBuilder like this:

<pre>
int32_t personTypeID = builder.createStructDef("person");
builder.addMember(personTypeID, "age", MS_TYPE_INT);
builder.addMember(personTypeID, "name", MS_TYPE_TEXT);


MSWriter pw = builder.createStruct("person", "boss");
pw.setInt("age", 42);
pw.setText("name", "Jaska");
</pre>

That creates a structure called `person` that has two members, `age` and `name`.
Then it creates a `person` variable called `boss`, and give the members values.
