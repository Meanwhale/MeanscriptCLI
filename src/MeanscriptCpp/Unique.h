// Copyright (C) Juha Huotari, 2020


#ifndef _UNIQUE_H_
#define _UNIQUE_H_


template <class T> class Unique
{
protected:

	T* data;

public:

	explicit Unique(T* ptr) : data(ptr) {}
	explicit Unique() :	data(0) {}

	Unique<T>(Unique<T> & src) { take(src); }

	Unique<T> & operator = (Unique<T> & src) { take(src); }

	T* operator -> () const { return data; }

	T* get() { return data; }

	void set(T* ptr)
	{
		del();
		data = ptr;
	}
	bool dead() const
	{
		return data == 0;
	}
	~Unique()
	{
		del();
	}

private:

	// hide
	Unique<T> & operator & () {};
	Unique<T> * operator * () {};

	void take(Unique<T> & src)
	{
		// take ownership from 'src'
		msAssert(!src.dead(), "object not alive...");
		data = src.data;
		src.data = 0;
	}
	void del()
	{
		if (data != 0)
		{
			delete data;
			data = 0;
		}
	}
};
#endif
