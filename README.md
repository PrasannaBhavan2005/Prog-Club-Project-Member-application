# Question 4 (After this question there is question 2 also)
# What exactly smart pointer is
Smart pointers are basically like the 'delete' keyword which we use 
when we allocate an object/array in heap memory so as to destroy the 
object/array otherwise there will be high chances of memory leak.

Smart pointers are basically advanced pointers provided by languages 
like C++ to manage the lifetime and ownership of dynamically allocated objects.
Smart pointers automate memory management to prevent issues like memory leaks 
and dangling pointers.

# Question 2
So I am going to explain that ghow the definition.hpp file works

'''

#pragma once

#include <blockingconcurrentqueue.h>
#include <list>
#include <utility>

template <typename T> using StableContainer = std::list<T>;

// wrapper class for tsan suppressions
template <typename T> class MPMCQueue
{
    moodycamel::BlockingConcurrentQueue<T> mpmcQueue;

public:
    __attribute__((no_sanitize("thread"))) bool enqueue(const T& t)
    {
        return mpmcQueue.enqueue(t);
    }
    __attribute__((no_sanitize("thread"))) bool enqueue(T&& t)
    {
        return mpmcQueue.enqueue(std::move(t));
    }

    template <typename U>
    __attribute__((no_sanitize("thread"))) void wait_dequeue(U& u)
    {
        return mpmcQueue.wait_dequeue(u);
    }
};

'''
