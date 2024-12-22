# Question 2
So I am going to explain that ghow the definition.hpp file works

```
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

```
Stable containers basically keep references and iterators to the objects
valid until the object is erased from the container 

The template defined using stablecontainer is ideal for scenarios requiring 
ordwered storage or frequent insertions/removals.

```
template <typename T> class MPMCQueue
```
purpose of a wrappwer around moodyblock is to implement a thread safe queue 
with multiple producer, multiple consumer capabilities. Wrapper includes
ThreadSanitizer suppressions using _attribute__((no_sanitize("thread")))
to suppress TSAN warnings for specific methods.

```
__attribute__((no_sanitize("thread"))) bool enqueue(const T& t);
```
It adds an element to the queue by copying it, and it returns true if 
the element was successfully added. And TSAN suppression suppresses thread
sanitizer warnings for this method.
```
template <typename U>
__attribute__((no_sanitize("thread"))) void wait_dequeue(U& u);
```
It waits for an element to become available in the queue and removes it,
storing the result in u. Here the input is a reference 'u' where the dequed
element will be stored. And same way TSAN suppression suppresses thread
sanitizer warnings for this method.


