# Class Maker

Class maker generates source code for serializing fixed-sized data structures (classes).

The structures can contain
 * ints and floats
 * fixed-leght text
 * other data structures
 * arrays of anything above

It's a work-in-progress and generates only Java for now.

## Simple example

Data structure `vec` with integer members `x` and `y`

```
struct vec [
  int x,
  int y
]
```

generates Java code like this:

```java
public class vec extends IData
{
  public vec (MSBuilder builder, String name) throws MException
  { super(builder,name,16); }

  public    vec (MSCode mc, String varName) throws MException
  { super(mc,varName,16); }

  protected vec (MSCode mc, int dataIndex) throws MException
  { super(mc,dataIndex,16); }

  @Override
  public String typeName() { return "vec"; }
  public static final int TYPE_ID = 16;
  public int         get_x() throws MException { return dataAt(index+0); }
  public void        set_x(int data) throws MException { write(index+0, data); }
  public int         get_y() throws MException { return dataAt(index+1); }
  public void        set_y(int data) throws MException { write(index+1, data); }
  public String      get_name() throws MException { return getChars(index+2); }
  public void        set_name(String data) throws MException { setChars(index+2, data, 7); }
  public int         get_name_max_chars() { return 7; }
}
```

There's two public constructors to access the data structure.
You can create a `vec` with a name using `MSBuilder` when writing data, or
you can get an access to a `vec` from `MSCode` when reading data.
The third constructor is to create a `vec` object from other generated class that has a `vec` as a member.

When reading or writing data with a generated class, the get and set functions access directly to the integer array where all bytecode is saved.
All access happens in a constant time (_O(1)_ time complexity) without any extra memory allocation.

## Advanced Example

Here's a group of data structures that has references to each other:

```
struct vec [
  int x,               // an integer
  int y]               // an integer
struct person [  
  chars[32] name,      // fixed-sized text, with 32 characters max
  vec [4] corner,      // array of four vec's
  vec pos,             // a single vec
  float age]           // a floating-point number
struct group [  
  text title,          // non-modifiable text
  person [3] member]   // array of three persons
```

Class maker genearates three Java classes from it. Direct accesses to member structures and arrays are generated recursively.

```java
public class group extends IData
{
  public group (MSBuilder builder, String name) throws MException
  { super(builder,name,20); }

  public    group (MSCode mc, String varName) throws MException
  { super(mc,varName,20); }

  protected group (MSCode mc, int dataIndex) throws MException
  { super(mc,dataIndex,20); }

  @Override
  public String typeName() { return "group"; }
  public static final int TYPE_ID = 20;
  public String      get_title() throws MException { return mc.getText(dataAt(index+0)); }
  public int         get_member_array_size() { return 3; }
  public MSDataArray get_member_array() throws MException { return new MSDataArray(mm,1,index+1); }
  public person get_member_at(int memberIndex) throws MException { indexCheck(memberIndex,3); return new person(mc,index+1); }
  public String      get_member_name_at(int memberIndex) throws MException { indexCheck(memberIndex,3); return getChars(index+1+(memberIndex*36)); }
  public void        set_member_name_at(int memberIndex, String data) throws MException { indexCheck(memberIndex,3); setChars(index+1+(memberIndex*36), data, 32); }
  public int         get_member_name_max_chars() { return 32; }
  public int         get_member_corner_array_size() { return 4; }
  public MSDataArray get_member_corner_array_at(int memberIndex) throws MException { indexCheck(memberIndex,3); return new MSDataArray(mm,1,index+11+(memberIndex*36)); }
  public vec get_member_corner_at(int memberIndex,int cornerIndex) throws MException { indexCheck(memberIndex,3); indexCheck(cornerIndex,4); return new vec(mc,index+11); }
  public int         get_member_corner_x_at(int memberIndex,int cornerIndex) throws MException { indexCheck(memberIndex,3); indexCheck(cornerIndex,4); return dataAt(index+11+(memberIndex*36+cornerIndex*5)); }
  public void        set_member_corner_x_at(int memberIndex,int cornerIndex, int data) throws MException { indexCheck(memberIndex,3); indexCheck(cornerIndex,4); write(index+11+(memberIndex*36+cornerIndex*5), data); }
  public int         get_member_corner_y_at(int memberIndex,int cornerIndex) throws MException { indexCheck(memberIndex,3); indexCheck(cornerIndex,4); return dataAt(index+12+(memberIndex*36+cornerIndex*5)); }
  public void        set_member_corner_y_at(int memberIndex,int cornerIndex, int data) throws MException { indexCheck(memberIndex,3); indexCheck(cornerIndex,4); write(index+12+(memberIndex*36+cornerIndex*5), data); }
  public String      get_member_corner_name_at(int memberIndex,int cornerIndex) throws MException { indexCheck(memberIndex,3); indexCheck(cornerIndex,4); return getChars(index+13+(memberIndex*36+cornerIndex*5)); }
  public void        set_member_corner_name_at(int memberIndex,int cornerIndex, String data) throws MException { indexCheck(memberIndex,3); indexCheck(cornerIndex,4); setChars(index+13+(memberIndex*36+cornerIndex*5), data, 7); }
  public int         get_member_corner_name_max_chars() { return 7; }
  // struct vec
  public vec get_member_pos_at(int memberIndex) throws MException { indexCheck(memberIndex,3); return new vec(mc,index+31+(memberIndex*36)); }
  public int         get_member_pos_x_at(int memberIndex) throws MException { indexCheck(memberIndex,3); return dataAt(index+31+(memberIndex*36)); }
  public void        set_member_pos_x_at(int memberIndex, int data) throws MException { indexCheck(memberIndex,3); write(index+31+(memberIndex*36), data); }
  public int         get_member_pos_y_at(int memberIndex) throws MException { indexCheck(memberIndex,3); return dataAt(index+32+(memberIndex*36)); }
  public void        set_member_pos_y_at(int memberIndex, int data) throws MException { indexCheck(memberIndex,3); write(index+32+(memberIndex*36), data); }
  public String      get_member_pos_name_at(int memberIndex) throws MException { indexCheck(memberIndex,3); return getChars(index+33+(memberIndex*36)); }
  public void        set_member_pos_name_at(int memberIndex, String data) throws MException { indexCheck(memberIndex,3); setChars(index+33+(memberIndex*36), data, 7); }
  public int         get_member_pos_name_max_chars() { return 7; }

  public float       get_member_age_at(int memberIndex) throws MException { indexCheck(memberIndex,3); return MSJava.intFormatToFloat(dataAt(index+36+(memberIndex*36))); }
  public void        set_member_age_at(int memberIndex, float data) throws MException { indexCheck(memberIndex,3); write(index+36+(memberIndex*36), MSJava.floatToIntFormat(data)); }
}
```

```java
public class person extends IData
{
  public person (MSBuilder builder, String name) throws MException
  { super(builder,name,18); }

  public    person (MSCode mc, String varName) throws MException
  { super(mc,varName,18); }

  protected person (MSCode mc, int dataIndex) throws MException
  { super(mc,dataIndex,18); }

  @Override
  public String typeName() { return "person"; }
  public static final int TYPE_ID = 18;
  public String      get_name() throws MException { return getChars(index+0); }
  public void        set_name(String data) throws MException { setChars(index+0, data, 32); }
  public int         get_name_max_chars() { return 32; }
  public int         get_corner_array_size() { return 4; }
  public MSDataArray get_corner_array() throws MException { return new MSDataArray(mm,1,index+10); }
  public vec get_corner_at(int cornerIndex) throws MException { indexCheck(cornerIndex,4); return new vec(mc,index+10); }
  public int         get_corner_x_at(int cornerIndex) throws MException { indexCheck(cornerIndex,4); return dataAt(index+10+(cornerIndex*5)); }
  public void        set_corner_x_at(int cornerIndex, int data) throws MException { indexCheck(cornerIndex,4); write(index+10+(cornerIndex*5), data); }
  public int         get_corner_y_at(int cornerIndex) throws MException { indexCheck(cornerIndex,4); return dataAt(index+11+(cornerIndex*5)); }
  public void        set_corner_y_at(int cornerIndex, int data) throws MException { indexCheck(cornerIndex,4); write(index+11+(cornerIndex*5), data); }
  public String      get_corner_name_at(int cornerIndex) throws MException { indexCheck(cornerIndex,4); return getChars(index+12+(cornerIndex*5)); }
  public void        set_corner_name_at(int cornerIndex, String data) throws MException { indexCheck(cornerIndex,4); setChars(index+12+(cornerIndex*5), data, 7); }
  public int         get_corner_name_max_chars() { return 7; }
  // struct vec
  public vec get_pos() throws MException { return new vec(mc,index+30); }
  public int         get_pos_x() throws MException { return dataAt(index+30); }
  public void        set_pos_x(int data) throws MException { write(index+30, data); }
  public int         get_pos_y() throws MException { return dataAt(index+31); }
  public void        set_pos_y(int data) throws MException { write(index+31, data); }
  public String      get_pos_name() throws MException { return getChars(index+32); }
  public void        set_pos_name(String data) throws MException { setChars(index+32, data, 7); }
  public int         get_pos_name_max_chars() { return 7; }

  public float       get_age() throws MException { return MSJava.intFormatToFloat(dataAt(index+35)); }
  public void        set_age(float data) throws MException { write(index+35, MSJava.floatToIntFormat(data)); }
}
```

```java
public class vec extends IData
{
  public vec (MSBuilder builder, String name) throws MException
  { super(builder,name,16); }

  public    vec (MSCode mc, String varName) throws MException
  { super(mc,varName,16); }

  protected vec (MSCode mc, int dataIndex) throws MException
  { super(mc,dataIndex,16); }

  @Override
  public String typeName() { return "vec"; }
  public static final int TYPE_ID = 16;
  public int         get_x() throws MException { return dataAt(index+0); }
  public void        set_x(int data) throws MException { write(index+0, data); }
  public int         get_y() throws MException { return dataAt(index+1); }
  public void        set_y(int data) throws MException { write(index+1, data); }
  public String      get_name() throws MException { return getChars(index+2); }
  public void        set_name(String data) throws MException { setChars(index+2, data, 7); }
  public int         get_name_max_chars() { return 7; }
}
```
