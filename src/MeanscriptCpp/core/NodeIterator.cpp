
//#define FLOAT_TO_INT_FORMAT(f) (*reinterpret_cast<int*>(&(f)))
//#define INT_FORMAT_TO_FLOAT(i) (*reinterpret_cast<float*>(&(i)))
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

NodeIterator::NodeIterator (MNode* _node)
{
	node = _node;
}


NodeIterator NodeIterator:: copy ()
{
	return NodeIterator(node);
}
int32_t NodeIterator:: type ()
{
	return (*node).type;
}
std::string NodeIterator:: data ()
{
	return (*node).data;
}
MNode* NodeIterator:: getChild()
{
	return (*node).child;
}
MNode* NodeIterator:: getNext()
{
	return (*node).next;
}
MNode* NodeIterator:: getParent()
{
	return (*node).parent;
}
int32_t NodeIterator:: numChildren ()
{
	return (*node).numChildren;
}
int32_t NodeIterator:: line ()
{
	return (*node).lineNumber;
}
bool NodeIterator::hasNext()
{
	return (*node).next != 0;
}
bool NodeIterator::hasChild()
{
	return (*node).child != 0;
}
bool NodeIterator::hasParent()
{
	return (*node).parent != 0;
}
int32_t NodeIterator::nextType() 
{
	ASSERT(hasNext(), "nextType: no next");
	return (*(*node).next).type;
}
void NodeIterator::toNext() 
{
	ASSERT(hasNext(), "toNext: no next");
	node = (*node).next;
}
bool NodeIterator::toNextOrFalse()
{
	if (!hasNext()) return false;
	node = (*node).next;
	return true;
}
void NodeIterator::toChild() 
{
	ASSERT(hasChild(), "toChild: no child");
	node = (*node).child;
}
void NodeIterator::toParent() 
{
	ASSERT(hasParent(), "toParent: no parent");
	node = (*node).parent;
}
void NodeIterator::printTree(bool deep) 
{
	(*node).printTree(deep);
}
} // namespace meanscript(core)
// C++ END
