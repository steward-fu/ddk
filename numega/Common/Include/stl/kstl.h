// kstl.h - DriverWorks support for STL
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================
//
// Assumes that ntddk.h or wdm.h has been #include'd

#ifndef __KSTL_H__
#define __KSTL_H__

#undef  _CRTIMP
#define _CRTIMP
#define tolower(ch) (( ('A' <= (ch)) && ( (ch) <= 'Z' ) ) ? ( (ch)+('a'-'A') ) : (ch) )
#define toupper(ch) (( ('a' <= (ch)) && ( (ch) <= 'z' ) ) ? ( (ch)-('a'-'A') ) : (ch) )

#if !defined(_WDM_)
#define _WDM_ (NTVERSION=='WDM')
#endif

#ifndef __KNDISCPP_H__

#include <cpprt.h>
#include <util.h>
#include <kspin.h>
#include <kustring.h>
#include <kdispobj.h>
#include <kevent.h>
#include <kmutex.h>
#include <ksemaphr.h>
#include <kgenlock.h>

#endif	// __KNDISCPP_H__

#define __THROW_BAD_ALLOC

#include <suballoc.h>
#include <lockable.h>

#undef tolower
#undef toupper

typedef ULONG POOLTAG;
extern POOLTAG DefaultPoolTag;

///////////////////////////////////////////////////////////////////////////////////
// Pool traits types
//
// Template class km_alloc (see below) has a template parameter that specifies the
// characteristics of the system memory pool for the class. These types must specify
// (1) the pool type (e.g. PagedPool), and the lock type (e.g. KGenericLockMutex).
// The lock type must implement the DriverWorks generic lock interface.
//
// There are two predefined types for such pool traits: paged_pool and non_paged_pool.

///////////////////////////////////////////////////////////////////////////////////
// Allocators
//
// Allocators control allocation of memory for the STL classes. Template class
// km_alloc implements an allocator suited for kernel mode drivers. Following
// the definition of this template class are typedefs for two specializations
// of this template: paged_alloc and non_paged_alloc.
//
// When a driver declares an instance of an STL container, the declaration should
// include a template parameter that specifies which allocator type to use. For example,
// consider vector. Like many of the STL containers, vector has two template parameters:
// one that specifies the value type (i.e. which type of objects the vector contains),
// and one that specifies the type of the allocator.
//
// Here is an example of how to declare a vector of 100 ULONGs that uses the paged pool
// for allocations:
//
//		vector<ULONG, paged_alloc> ulvect(100);
//
// Drivers that use the STLport STL should #define _STLP_USE_MALLOC. From STPport
// documentation: "This makes allocator<> to do plain malloc() calls instead of using SGI
// optimized node allocator engine."
// The malloc-based allocator is mapped to non_paged_alloc, so the call to member
// function init at DriverEntry is still required.
//
// The default value for the template parameter that specifies the allocator type is
// std::malloc_alloc.  It is preferable to make the allocator type explicit, rather
// than to rely on the default. As stated above, the two predefined allocator types are
// paged_alloc and non_paged_alloc.
//
// In the STLport implementation of STL, this STL assumes the allocators are stateless,
// i.e. there is no unique data associated with any given instance of an allocator.
// Therefore, there is never a need to create an instance of an allocator. Template
// class km_alloc (and its specializations paged_alloc and non_paged_alloc) are
// implemented entirely as static member functions.
//
// -Setting the heap sizes
//
// To set the size of the paged heap or the non-paged heap, simply add to your driver
// definitions and initializations for the following data objects:
//
//			stl_traits::non_paged_heap_size
//			stl_traits::paged_heap_size
//
// For example, if you want to allocate 32 KB for the non_paged_heap and 1 MB for the
// paged heap, add these lines to your driver (in the global scope) and the #include
// of kstl.h:
//
//			size_t stl_traits::non_paged_heap_size = 0x8000;
//			size_t stl_traits::paged_heap_size = 0x100000;
//
// Note: The default size for each heap is 32 KB.
// Note: These values are used only at initialization time.
//
// -Suballocators
//
// Template class km_alloc uses a suballocator that is compatible with kernel mode
// drivers. A description of the suballocator is found in suballoc.h.
//
// -Out of Memory Condition
//
// The STLport STL can be configured such that it will not throw C++ exceptions. This is the
// desirable mode for kernel mode drivers because (1) there is significant overhead
// required to support throwing C++ exceptions, (2) the necessary run time support
// would have to be customized for the kernel environment, and (3) the behavior the
// exception mechanism across different IRQLs could be problematic. Consequently, the
// STL is compiled to avoid throwing exceptions. However, this does not answer the
// question of what to do when a memory allocation request fails. The default malloc-based
// allocator calls a client defined out-of-memory handler if memory is exhausted
// and exceptions are not in use, but the assumption here is that the handler will
// terminate the "application". For drivers, the analogous operation would be to call
// KeBugCheck, which crashes the system. This is clearly not an acceptable course of action.
// Furthermore, simply returning zero from the allocator is not sufficient, because the
// STL assumes that if memory were not available, then the allocator would have thrown
// an exception or terminated.  Consequently, returning zero from the allocator causes
// an immediate page fault.
//
// The simple solution is to not run out of memory. The kernel allocators are designed
// to allow preallocation of memory so that availability of memory is predictable and
// deterministic. To this end, class km_alloc provides the following:
//
//	(1) Static member function init, which takes as a parameter the number of bytes
//	    of memory to preallocate for the type of allocator, and the pointer to a
//	    function to be called if memory becomes scarce.
//
//	(2) Static member function destroy, which releases the memory allocated by init.
//
//  (3) Static member function largest_free, which returns the size in bytes of the
//	    largest available free block. The allocator guarantees that if the next
//		allocation attempt that it receives is for a block of that size or smaller,
//		then it will succeed.
//
// When km_alloc is unable to satisfy an allocation request, either because the preallocated
// size was inadequate or there is excessive fragmentation, it does not crash the system.
// Instead, it takes the following actions:
//
//	(1) It calls the low memory callback function passed to member function init. The
//	    request size is passed to this function, along with the pool type. The callback
//		function is not required to take any particular action, and it must NOT make
//		any STL allocation or deallocation calls (doing so will result in a deadlock).
//		The call to the callback serves to notify the driver that the preallocated heap
//		is running low, or that fragmentation is preventing allocations of large blocks.
//
//	(2) It then calls ExAllocatePool to attempt to allocate the required memory from the
//	    the system. If this succeeds, the allocator returns a pointer to the allocated
//		memory.
//
//	(3) If the allocator fails to allocate memory either from the preallocated heap or
//		from the system, it calls ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES). This can
//		be caught in a __try/__except statement of the calling driver. If there is no
//		__try/__except statement active, a blue screen results.
//
// -IRQL Guidelines
//
// The suballocator is locked with a mutex for allocations and deallocations of objects that
// use paged_alloc. Therefore, when the driver is running at IRQL >= DISPATCH_LEVEL, it must
// not access any object that relies on paged_alloc.
//
// The suballocator is locked with a spin lock for allocations and deallocations of objects that
// use non_paged_alloc. Therefore, when the driver is running at IRQL > DISPATCH_LEVEL (as in an ISR),
// it must not construct or destroy any STL object that relies on non_paged_alloc (not even on the
// stack), nor may it call any member function that might require allocation or deallocation.
// The debug version of km_alloc will ASSERT if this occurs.
//

//////////////////////////////////////////////////////////////////////////////////////////
// Examples
//////////////////////////////////////////////////////////////////////////////////////////

// --------------------------------
// Initialization and Preallocation
// --------------------------------
// This code belongs in DriverEntry, or before you access any STL objet.
//
//		// allocate 64 kb for the STL non_paged_pool
//		bool non_paged_alloc_ok = non_paged_alloc::init(65536, LowMemCallback);
//		// allocate 1 MB for the STL paged_pool
//		bool paged_alloc_ok = paged_alloc::init(0x100000, LowMemCallback);
//
//		if ( ! (non_paged_alloc_ok && paged_alloc_ok) )
//			return STATUS_INSUFFICIENT_RESOURCES;
// --------------------------------
// Low memory handler
// --------------------------------
//
//		void LowMemCallback(POOL_TYPE pooltype, size_t requested_size)
//		{
//			Low_On_Memory = TRUE;
//			DbgPrint("Failed to allocate %x bytes from pool of type %x\n", requested_size, pooltype);
//		}
//
// --------------------------------
// Destruction
// --------------------------------
// This code belongs in Unload, or when all STL objects are no longer in use.
//
//		paged_alloc::destroy();
//		non_paged_alloc::destroy();
//
// --------------------------------
// Small allocations
// --------------------------------
//		vector<int, paged_alloc> vint;
//		vint.push_back(0);
//
// --------------------------------
// Larger allocations
// --------------------------------
//		if ( non_paged_pool::largest_free() < sizeof(int)*0x100000 )
//			return;
//
//		vector<int, non_paged_pool> big_array(0x100000);
//

//////////////////////////////////////////////////////////////////////////////////////////
// Pool traits types

#ifndef __KNDISCPP_H__

struct paged_pool
{
	enum _pooltype { _type = PagedPool };
	typedef KGenericLockMutex lock_type;
	static void* __AllocateSystemMemory(size_t size)
	{
#if DBG
		return ExAllocatePoolWithTag(PagedPool, size, DefaultPoolTag);
#else
		return ExAllocatePool(POOL_TYPE(PagedPool), size);
#endif
	}
	static void __DeallocateSystemMemory(void* p, size_t /*size*/)
	{
		ExFreePool(p);
	}
	static void panic(void)
	{
		ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES);
	}
};

struct non_paged_pool
{
	enum _pooltype { _type = NonPagedPool };
	typedef KGenericLockSpinLock lock_type;
	static void* __AllocateSystemMemory(size_t size)
	{
#if DBG
		return ExAllocatePoolWithTag(NonPagedPool, size, DefaultPoolTag);
#else
		return ExAllocatePool(NonPagedPool, size);
#endif
	}
	static void __DeallocateSystemMemory(void* p, size_t /*size*/)
	{
		ExFreePool(p);
	}
	static void panic(void)
	{
		ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES);
	}
};

#else // __KNDISCPP_H__

struct kndis_pool	// KNDIS: use default non-paged new/delete
{
	enum _pooltype { _type = NonPagedPool };
	typedef KNdisSpinLock lock_type;
	static void* __AllocateSystemMemory(size_t size)
	{
		return (void*) new char[size];
	}
	static void __DeallocateSystemMemory(void* p, size_t /*size*/)
	{
		delete reinterpret_cast<char*>(p);
	}
	static void panic(void)
	{
#if _X86_
#if DBG
		_asm	int 3	// what else?
#endif
#else
		ExRaiseStatus(STATUS_INSUFFICIENT_RESOURCES);
#endif
	}
};

#endif // __KNDISCPP_H__


//////////////////////////////////////////////////////////////////////////////////////////
// class km_alloc
//
template <class _pool_traits> class km_alloc
{
protected:
	typedef typename ntkm::lockable<suballocator::heap, typename _pool_traits::lock_type> lockable_heap;

public:
	typedef void (*low_memory_callback)(POOL_TYPE pooltype, size_t request);
	template <class _Tp1> struct rebind {
		typedef std::__allocator<_Tp1, km_alloc<_pool_traits> > other;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	// init
	static inline bool init(size_t heapsize, low_memory_callback plmcb = 0)
	{
		low_memory(0, plmcb);

		if (heapsize != 0)
		{
			void* p = _pool_traits::__AllocateSystemMemory(heapsize);

			if (p != NULL)
			{
				lockable_heap& h = _heap_instance();
				h().init(p, heapsize);
				h().bad_deallocate = free_from_system;
				return true;
			}
			else
				return false;
		}
		else
			return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// destroy
	static inline void destroy(void)
	{
		static lockable_heap& h=_heap_instance();

		void* p = h().region();
		if (p != 0)
			_pool_traits::__DeallocateSystemMemory(p, h().allocated_size());

		h().init(0, 0);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// allocate
	static inline void* allocate(size_t __n)
	{
		static lockable_heap& h=_heap_instance();

#ifndef __KNDISCPP_H__
		ASSERT( KeGetCurrentIrql() <= DISPATCH_LEVEL);
		ASSERT(((KeGetCurrentIrql() < DISPATCH_LEVEL) || (_pool_traits::_type == NonPagedPool)));
#endif	// __KNDISCPP_H__
		ASSERT( h().region() && "Heap was not initialized.");

		h.lock();
		void* p = h().allocate(__n);
		h.unlock();

		if ( p == 0 )
			p = low_memory(__n);

		return p;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// deallocate
	static inline void deallocate(void* __p, size_t __n)
	{
		static lockable_heap& h=_heap_instance();

#ifndef __KNDISCPP_H__
		ASSERT( KeGetCurrentIrql() <= DISPATCH_LEVEL);
		ASSERT (((KeGetCurrentIrql() < DISPATCH_LEVEL) || (_pool_traits::_type == NonPagedPool)));
#endif	// __KNDISCPP_H__
		ASSERT( h().region() && "Heap was not initialized.");

		h.lock();
		h().deallocate(__p);
		h.unlock();

		UNREFERENCED_PARAMETER(__n);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// reallocate
	static inline void* reallocate(void* __p, size_t  __old_sz , size_t __new_sz)
	{
		static lockable_heap& h =_heap_instance();

		ASSERT( h().region() && "Heap was not initialized.");

		h.lock();
		void* p = h().reallocate(__p, __new_sz);
		h.unlock();
		return p;
		UNREFERENCED_PARAMETER(__old_sz);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// largest_free
	static size_t largest_free(void)
	{
		static lockable_heap& h=_heap_instance();

		ASSERT( h().region() && "Heap was not initialized.");

		return h().largest_free();
	}

/////////////////////////////////////////////////////////////////////////////////////////
// protected members

protected:
	static lockable_heap& _heap_instance(void)
	{
		static suballocator::heap h;
		static lockable_heap m_heap(h);
		return m_heap;
	}

	static void* low_memory(size_t request, low_memory_callback plmcb=0)
	{
		static low_memory_callback m_lmcb=0;
		if ( m_lmcb == 0 )	// first time, initialization call
		{
			m_lmcb = plmcb;
			return 0;
		}
		else if ( request > 0 )
		{
			void* p;

			if (m_lmcb != 0)
			{
				m_lmcb(POOL_TYPE(_pool_traits::_type), request);
			}

			p = _pool_traits::__AllocateSystemMemory(request);

			if ( p == 0 )
				_pool_traits::panic();

			return p;
		}
		else
			return 0;
	}

	static void free_from_system(suballocator::heap* pheap, void* block)
	{
		if (block != 0)
			_pool_traits::__DeallocateSystemMemory(block, 0); // size unknown????

		UNREFERENCED_PARAMETER(pheap);
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
// Specializations of km_alloc
//
#ifndef __KNDISCPP_H__

typedef km_alloc<paged_pool> paged_alloc;
typedef km_alloc<non_paged_pool> non_paged_alloc;

#else	// __KNDISCPP_H__

typedef km_alloc<kndis_pool> non_paged_alloc;

#endif	// __KNDISCPP_H__

//////////////////////////////////////////////////////////////////////////////////////////
// Pool initializer struct
//
struct stl_traits
{
	static size_t paged_heap_size;
	static size_t non_paged_heap_size;
	static void (*low_memory_callback)(POOL_TYPE, size_t);
};

//////////////////////////////////////////////////////////////////////////////////////////
// malloc and free
//
// This code maps the malloc-based allocator to non_paged_alloc
//
extern "C" void* __cdecl malloc(size_t __n);
extern "C" void  __cdecl free(void* __p);

inline void* __cdecl malloc(size_t __n)
{
	return non_paged_alloc::allocate(__n);
}

inline void __cdecl free(void* __p)
{
	non_paged_alloc::deallocate(__p, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Utility classes for exception safe heap allocation

template <class T> T* allocate_object(POOL_TYPE pool)
{
	_try
	{
		T* pT = new (pool) T;
		return pT;
	}
	_except (EXCEPTION_EXECUTE_HANDLER)
	{
		return NULL;
	}
}

template <class T> T* allocate_object(POOL_TYPE pool, size_t n)
{
	_try
	{
		T* pT = new (pool) T(n);
		return pT;
	}
	_except (EXCEPTION_EXECUTE_HANDLER)
	{
		return NULL;
	}
}

// This pragma forces in kstl.cpp, which includes special data segments recognized in
// the C++ start up code for the driver (see cpprt.cpp and kstl.cpp).

#if defined(_IA64_) || defined(_AMD64_)
#pragma comment(linker, "/include:__stl_init__")
#else
#pragma comment(linker, "/include:___stl_init__")
#endif

#endif // __KSTL_H__
