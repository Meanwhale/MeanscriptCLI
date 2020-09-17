// Copyright (C) Juha Huotari, 2019

#ifndef _Link_H_
#define _Link_H_

#include "MS.h"

/*
	Form a two-way link for an object.
	If both get deleted, delete the data

	Sketch:


	Example:

	class Node {
		Unit unit;
		Link<Node> link;
		Node() : link(this) {}
	}
	Node * tail;
	void addNext(Node * node)
	{
		tail->link.connect(node);
		tail = node;
	}
	// ...
		
		addNext(new Node()); // will be deleted automatically

*/

template <class T> class Link
{
protected:

	T*		data;
	Link*	other;

public:

	explicit Link(T* ptr) :
		data(ptr)
	{
	}

	void connect(Link * l)
	{
		ASSERT(data != 0, "");
		ASSERT(other = 0, "");
		ASSERT(l->other = 0, "");
		other = l;
		other->other = this;
	}

	Link* getOther()
	{
		return other;
	}

	~Link()
	{
	}

private:

	// never needed?
	explicit Link()
	{
		data = 0;
	}

	// hide
	Link<T> & operator & () {};
	Link<T> * operator * () {};
};
#endif
