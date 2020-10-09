# Meanscript Examples

Here are some examples of how to use Meanscript.

Check the Tutorial at [README.md](https://github.com/Meanwhale/MeanscriptCLI/blob/master/README.md) and the demo project for the basic setup.

Quick reference:<br>
https://github.com/Meanwhale/MeanscriptCLI/blob/master/REFERENCE.md


## Reading a Script

Read these values in `test.ms`

<pre>
text name: "Meanscript"
int year: 2020
float height: 123.456
</pre>

from your source code:

```cpp
#include <iostream>
#include "MS.h"
using namespace std;
using namespace meanscript;

int main()
{
  MSFileInStream input = getInput("test.ms", true);
  MSCode code (input, globalConfig.STREAM_SCRIPT);
  code.run();
  cout<<"name = "<<code.getText("name")<<endl;
  cout<<"year = "<<code.getInt("year")<<endl;
  cout<<"height = "<<code.getFloat("height")<<endl;

  return 0;
}
```


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

### Reading a Structure

You can read the structures above with MSData object:

```cpp
  MSFileInStream input = getInput("test.ms", true);
  MSCode code (input, globalConfig.STREAM_SCRIPT);
  code.run();

  MSData jack = code.getData("jackOne");
  MSData position = jack.getMember("pos");
  cout<<"pos.x = "<<position.getInt("x")<<endl;
  cout<<"pos.y = "<<position.getInt("y")<<endl;
  cout<<"name = "<<jack.getText("name")<<endl;
  cout<<"age = "<<jack.getInt("age")<<endl;
```


## Bytecode Builder

Use MSBuilder to create bytecode data in your source code.

Writing simple variables to a bytecode:

```cpp
MSBuilder builder ("test");

builder.addInt("aa", 123);
builder.addText("key","value");

MSFileOutStream output = getOutput("textcode.mb", true);
builder.write(output);
```

That results a similar data as scripting

<pre>
int aa: 123
text key: "value"
</pre>

You can create structures and structure variables with MSBuilder like this:

```cpp
int32_t personTypeID = builder.createStructDef("person");
builder.addMember(personTypeID, "age", MS_TYPE_INT);
builder.addMember(personTypeID, "name", MS_TYPE_TEXT);


MSWriter pw = builder.createStruct("person", "boss");
pw.setInt("age", 42);
pw.setText("name", "Jaska");
```

That creates a structure called `person` that has two members, `age` and `name`.
Then it creates a `person` variable called `boss`, and give the members values.
