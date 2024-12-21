// Here I made a unique_ptr which itself is a smart pointer
#include<iostream>
using namespace std;
template <class T>   // Template declaration
class my_unique_ptr  
{
private:
	T * ptr = nullptr;    // raw pointer initialised to null pointer so as to avoid issues

public:
	my_unique_ptr() : ptr(nullptr) // default constructor allows the creation of a my_unique_ptr object without associating it with any memory initially
	{
	}

	my_unique_ptr(T * ptr) : ptr(ptr)   // parametrised constructor, takes a raw pointer and assigns it to the internal pointer using this keyword
	{
	}

	my_unique_ptr(const my_unique_ptr & obj) = delete; // copy constructor is deleted to enforce exclusive ownership
	my_unique_ptr& operator=(const my_unique_ptr & obj) = delete; // copy assignment is deleted

	my_unique_ptr(my_unique_ptr && dyingObj) // move constructor it enables transfering ownership from a temporary pointer(my_unique_ptr) to a new one
	{
		// Transfer ownership of the memory pointed by dyingObj to this object
		this->ptr = dyingObj.ptr;
		dyingObj.ptr = nullptr; 
	}

	void operator=(my_unique_ptr && dyingObj) // move assignment transfers ownership from dying0bj to current object
	{
		__cleanup__(); // cleanup any existing data

		// Transfer ownership of the memory pointed by dyingObj to this object
		this->ptr = dyingObj.ptr;
		dyingObj.ptr = nullptr;  // set to null pointer to ensure no dangling pointer 
	}
         // Operator overloading
	T* operator->() // overloading the arrow operator to allow access to methods of object being pointed to.
	{
		return this->ptr;  // returning the raw pointer 
	}

	T& operator*()  // here we overloaded the dereference operator, reason is the same as above arrow operator 
	{
		return *(this->ptr);
	}

        // destructor called when my_unique_ptr goes out of scope 
	~my_unique_ptr() 
	{
		__cleanup__();  // calling the private cleanup funcn to free the dynamically allocated memory and ensuring no memory leaks.
	}

private:
	void __cleanup__() // prevents memory leak by ensuring that the dynamically allocated object is freed.
	{
		if (ptr != nullptr) // deletes the managed resource if ptr is not equal to null ptr
			delete ptr;
	}
};

// template specialisation for arrays. allows my_unique_ptr to manage dynamically allocated arrays separately
template <class T> 
class my_unique_ptr<T[]>  // instead of delete keyword, a specialisation is necessary
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

 // overloading the subscript operator to access elements of array 
	T& operator[](int index) 
	{
		if(index < 0)
		{
			// Throw negative index exception
			throw(new std::exception("Negative index exception"));
		}
		return this->ptr[index]; // doesn't check out of bounds
	}

       // destructor
	~my_unique_ptr()  
	{
		__cleanup__(); // funcn called to free up the memory 
	}

private:
	void __cleanup__()
	{
		if (ptr != nullptr)
			delete[] ptr;  // delete the array using the delete keyword 
	}
};
