// Here I made a unique_ptr which itself is a smart pointer
#include<iostream>
using namespace std;
template <class T>
class my_unique_ptr
{
private:
	T * ptr = nullptr;

public:
	my_unique_ptr() : ptr(nullptr) // default constructor
	{
	}

	my_unique_ptr(T * ptr) : ptr(ptr)
	{
	}

	my_unique_ptr(const my_unique_ptr & obj) = delete; // copy constructor is deleted
	my_unique_ptr& operator=(const my_unique_ptr & obj) = delete; // copy assignment is deleted

	my_unique_ptr(my_unique_ptr && dyingObj) // move constructor
	{
		// Transfer ownership of the memory pointed by dyingObj to this object
		this->ptr = dyingObj.ptr;
		dyingObj.ptr = nullptr; 
	}

	void operator=(my_unique_ptr && dyingObj) // move assignment
	{
		__cleanup__(); // cleanup any existing data

		// Transfer ownership of the memory pointed by dyingObj to this object
		this->ptr = dyingObj.ptr;
		dyingObj.ptr = nullptr;
	}

	T* operator->() // deferencing arrow operator
	{
		return this->ptr;
	}

	T& operator*()
	{
		return *(this->ptr);
	}

	~my_unique_ptr() // destructor
	{
		__cleanup__();
	}

private:
	void __cleanup__()
	{
		if (ptr != nullptr)
			delete ptr;
	}
};

template <class T>
class my_unique_ptr<T[]>
{
private:
	T * ptr = nullptr;

public:
	my_unique_ptr() : ptr(nullptr) // default constructor
	{
	}

	my_unique_ptr(T * ptr) : ptr(ptr)
	{
	}

	my_unique_ptr(const my_unique_ptr & obj) = delete; // copy constructor is deleted
	my_unique_ptr& operator=(const my_unique_ptr & obj) = delete; // copy assignment is deleted

	my_unique_ptr(my_unique_ptr && dyingObj) // move constructor
	{
		__cleanup__(); // cleanup any existing data

		// Transfer ownership of the memory pointed by dyingObj to this object
		this->ptr = dyingObj.ptr;
		dyingObj.ptr = nullptr;
	}

	void operator=(my_unique_ptr && dyingObj) // move assignment
	{
		__cleanup__(); // cleanup any existing data

		// Transfer ownership of the memory pointed by dyingObj to this object
		this->ptr = dyingObj.ptr;
		dyingObj.ptr = nullptr;
	}

	T* operator->() // deferencing arrow operator
	{
		return this->ptr;
	}

	T& operator*()
	{
		return *(this->ptr);
	}

	T& operator[](int index)
	{
		if(index < 0)
		{
			// Throw negative index exception
			throw(new std::exception("Negative index exception"));
		}
		return this->ptr[index]; // doesn't check out of bounds
	}

	~my_unique_ptr() // destructor
	{
		__cleanup__();
	}

private:
	void __cleanup__()
	{
		if (ptr != nullptr)
			delete[] ptr;
	}
};
