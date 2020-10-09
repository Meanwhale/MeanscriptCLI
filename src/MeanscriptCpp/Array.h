// Copyright (C) Juha Huotari, 2006


#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "MS.h"

template <class T> class Array
{
protected:

	T*				data;
	int				size; // size of reserved array; 0 if not allocated (, negative on error?)


public:

	const char * description = 0; // print array description on overflow to identify the problem better

	explicit Array() :
		data(0),
		size(0)
	{
	}
	explicit Array(int s) :
		data(new T[s]),
		size(s)
	{
	}
	explicit Array(T * a, int s) :
		data(a),
		size(s)
	{
	}

	~Array()
	{
		clear();
	}

	void clear()
	{
		if (data != 0)
		{
			delete[] data;
			data = 0;
			size = 0;
		}
	}

	void clone(T * a, int s)
	{
		delete[] data;
		data = new T[s];
		size = s;
		for (int i=0; i<s; i++) data[i] = a[i];
	}

	void reset(int s)
	{
		delete[] data;
		data = new T[s];
		size = s;
	}
	bool inRange(int i)
	{
		return i >= 0 && i < size;
	}

	// MANAGING FUNCTIONS

	void fill(T a)
	{
		for (int i = 0; i < size; i++)
			data[i] = a;
	}

	// overwrite: copy "ptr[i] <- src[0] ... ptr[i+num-1] <- src[num-1]
	/*void overwrite(const T * src, int i, int num)
	{
		ASSERT(inRange(i + num - 1));

		for (int n = 0; n < num; n++)
			data[i + n] = src[n];
	}
	void insert(const T * src, int i, int num)
	{
		// shift, make room to insert
		for (int n = size - 1; n >= i + num; n--)
		{
			get(n) = get(n - num);
		}
		overwrite(src, i, num);
	}
	bool equals(const T * src, int num)
	{
		ASSERT(num < size);
		for (int i = 0; i < num; i++)
			if (base.ptr[i] != src[i]) return false;
		return true;
	}
	//void append (T * src, int num)
	//{
	//}

	// MANAGING FUNCTIONS WITH ARRAY CLASS OBJECT AS A PARAMETER

	void overwrite(Array<T> & src, int i)
	{
		overwrite(src.base.ptr, i, src.size);
	}
	void insert(Array<T> & src, int i)
	{
		insert(src.base.ptr, i, src.size);
	}
	void copy(Array<T> & src)
	{
		reset(src.size);
		overwrite(src.base.ptr, 0, src.size);
	}
	bool equals(Array<T> & src)
	{
		if (src.size != size) return false;
		return equals(src.base.ptr, size);
	}*/


	// ACCESSORS

	T * get() const
	{
		return data;
	}
	T & get(int i)
	{
		if (!inRange(i)) msError("out of range", description, 0, -1);
		return data[i];
	}
	T & operator[] (int i)
	{
		if (!inRange(i)) msError("out of range", description, 0, -1);
		return data[i];
	}
	int length()
	{
		return size;
	}

private:

	//Array<T> & operator = (Array<T> &) = default;
	//Array<T> (const Array<T> &) = default;
	//Array<T> & operator & () {};
	//Array<T> * operator * () = delete;
};
#endif // _ARRAY_H_
