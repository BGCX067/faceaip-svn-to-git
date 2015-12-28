/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * PoolAllocator.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: PoolAllocator.h 5884 2012-05-16 09:10:12Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __EZPOOLALLOCATOR_H__
#define __EZPOOLALLOCATOR_H__

#include <assert.h>
#include <exception>
#include <algorithm>
#include <stddef.h>/*ptrdiff_t*/

#include "EZMutex.h"
#ifdef EZLIBS_NAMESPACE
namespace ezlibs {
#endif

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// CEZPool based on Chunk
class CEZPool
{
	struct Link
	{
		Link* next;
	};
	struct Chunk
	{
		char *mem;					// allocation area first to get stringent alignment
		Chunk *next;
	};

	enum
	{
		ecount_default = 1024,		//defaut element count in a chunk
	};

	Chunk *chunks;
	const unsigned int esize;
	unsigned int ecount;
	Link* head;
	CEZMutex mutex;

	CEZPool(CEZPool&);					// copy protection
	void operator=(CEZPool&);			// copy protection
	void grow();					// make pool larger

public:
	CEZPool(unsigned int sz);			// sz is the size of elements
	~CEZPool();

	void* alloc();					// allocate one element
	void free(void *b);				// put an element back into the pool
	void setec(unsigned int n);		//set max element count in a chunk
	unsigned int getec();			//get max element count in a chunk
};

inline void *CEZPool::alloc()
{
	//mutex.CEZLock();
	mutex.Lock();
	if(head == 0) grow();
	Link *p = head;					// return first element
	head = p->next;
	//mutex.Unlock();
	mutex.Unlock();
	return p;
}

inline void CEZPool::free(void *b)
{
	mutex.Lock();
	Link *p = static_cast<Link*>(b);
	p->next = head;					// put b back as first element
	head = p;
	mutex.Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// pool_allocator
template <class T> class pool_allocator;

template<> class pool_allocator<void>
{
public:
	typedef void        value_type;
	typedef void*       pointer;
	typedef const void* const_pointer;
	
	template <class U> 
		struct rebind { typedef pool_allocator<U> other; };
};

template <class T> class pool_allocator
{
private:
	static CEZPool mem;				// pool of elements of sizeof(T)

public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
	
	template <class U> 
		struct rebind { typedef pool_allocator<U> other; };
	
	
	pool_allocator() {}
	template <class U> 
		pool_allocator(const pool_allocator<U>&) {}
	~pool_allocator() {}

	
	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const
	{ 
		return x;
	}

	static void setchunksize(unsigned int n)
	{
		mem.setec(n);
	}

	static unsigned int getchunksize()
	{
		return mem.getec();
	}

	pointer allocate(size_type n, pool_allocator<void>::const_pointer hint = 0)
	{
		assert(n == 1);
		return static_cast<T*>(mem.alloc());
	}

	void deallocate(pointer p, size_type n)
	{
		assert(n == 1);
		mem.free(p);
	}
	
	size_type max_size() const
	{ 
		return static_cast<size_type>(-1) / sizeof(T);
	}
	
	void construct(pointer p, const value_type& x)
	{ 
		new(p) value_type(x); 
	}
	void destroy(pointer p) { p->~T(); }
};

	template<class T>CEZPool pool_allocator<T>::mem(sizeof(T));

template <class T>
inline bool operator==(const pool_allocator<T>&, 
                       const pool_allocator<T>&)
{
	return true;
}


template <class T>
inline bool operator!=(const pool_allocator<T>&, 
                       const pool_allocator<T>&)
{
	return false;
}

#ifdef EZLIBS_NAMESPACE
}// namespace ezlibs
#endif

#endif // __EZPOOLALLOCATOR_H__
