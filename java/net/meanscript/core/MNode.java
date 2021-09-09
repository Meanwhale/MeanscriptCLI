package net.meanscript.core;
import net.meanscript.java.*;
import net.meanscript.*;
public class MNode {
 int type;
 int numChildren;
 int lineNumber;
 int characterNumber;
 MSText data;
 long numeralValue;
 MNode next = null;
 MNode child = null;
 MNode parent = null;

public MNode (int line, int ch, MNode _parent, int _type, MSText _data)
{
	data = _data;
	lineNumber = line;
	characterNumber = ch;

	parent = _parent;
	type = _type;
	numChildren = 0;
}
public MNode (int line, int ch, MNode _parent, int _type, long _numeralValue)
{
	data = null;
	numeralValue = _numeralValue;
	lineNumber = line;
	characterNumber = ch;

	parent = _parent;
	type = _type;
	numChildren = 0;
}

public int line ()
{
	return lineNumber;
}

public void printTree (boolean deep) throws MException
{
	printTree(this, 0, deep);
	if (!deep) MSJava.printOut.print("").endLine();
}

public void printTree (MNode _node, int depth, boolean deep) throws MException
{
	MSJava.assertion(_node != null,   "<printTree: empty node>");

	MNode node = _node;

	for (int i = 0; i < depth; i++) MSJava.printOut.print("  ");

	MSJava.printOut.print("[" + node.data + "]");

	// if (node.numChildren > 0) { {if(MSJava.globalConfig.verboseOn()) MSJava.printOut.print(" + " + node.numChildren).endLine();}; }

	if (deep) MSJava.printOut.print("").endLine();

	if (node.child != null && deep) printTree(node.child, depth + 1, deep);
	if (node.next != null) printTree(node.next, depth, deep);
}
//

}
