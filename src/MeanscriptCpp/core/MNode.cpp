#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

MNode::MNode (int32_t line, int32_t ch, MNode* _parent, int32_t _type, MSText* _data)
{
	data = _data;
	lineNumber = line;
	characterNumber = ch;

	parent = _parent;
	type = _type;
	numChildren = 0;
}

int32_t MNode::line ()
{
	return lineNumber;
}

void MNode::printTree (bool deep) 
{
	printTree(this, 0, deep);
	if (!deep) PRINT("");
}

void MNode::printTree (MNode* _node, int32_t depth, bool deep) 
{
	ASSERT(_node != 0, "<printTree: empty node>");

	MNode & node = (*_node);

	for (int32_t i = 0; i < depth; i++) PRINTN("  ");

	PRINTN("[" CAT node.data CAT "]");

	// if (node.numChildren > 0) { VERBOSE(" + " CAT node.numChildren); }

	if (deep) PRINT("");

	if (node.child != 0 && deep) printTree(node.child, depth + 1, deep);
	if (node.next != 0) printTree(node.next, depth, deep);
}
MNode::~MNode() { delete data; delete next; delete child; }

} // namespace meanscript(core)
