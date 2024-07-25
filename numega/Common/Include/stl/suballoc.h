// suballoc.h - suballocator for use with STL / kernel mode
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

// Suballocator overview
//
// This is a simple, self-contained suballocator. The primary class is heap,
// and its primary methods are allocate, deallocate, and reallocate. The
// class was designed for use as an STL allocator.
//
// The ctor for heap takes as parameters the address and size of a memory
// region to be used as the heap area. Class heap uses class pool as the
// manager of the unallocated region. The pool can only allocate from the
// its current top or its current bottom.
//
// It is a goal of this implementation to preserve locality of the control
// information. To this end, all control information is allocated from the
// low end of the pool. Consequently, walking the free list touches the
// minimal number of pages.
//
// The heap only maintains information about free space. It does not keep
// a record of the currently allocated blocks, although it does count them.
// The free space information is maintained in set of structures of type
// free_node. The free_nodes are organized in two doubly linked lists,
// one by address and one by size. This enables efficient removal of
// free space as it is allocated. List heads are maintained in the heap
// class. The heap also maintains a list of free_nodes not currently in
// use.
//
// Allocation:
//
// When the heap receives an allocation request, it first searches the
// size ordered list of free space nodes. The heap can be configured for
// either a best fit or first fit strategy. If a free block of satisfactory
// size is available, the allocator uses it or a portion of it to satisfy
// the request. Otherwise, allocation of the block is attempted from the
// high end of the pool.
//
// Allocation requires a linear search of the size ordered free list.
// Performance could be optimized by using a binary tree rather than a
// linked list.
//
// At the time of allocation, the allocator allocates a free block from the
// low end of the pool to be used to describe the block when it is freed.
// Since the pool can be quickly consumed by allocations, the strategy is
// to maintain a count of free nodes that is double the number of active
// allocated blocks. This anticipates the point where no more free nodes
// will be available from the pool.
//
// Deallocation:
//
// When a block is deallocated, the heap first checks its list of available
// free nodes, and if one is available, it is used. Otherwise, it tries to take
// one off of the low end of the pool. It then initializes the free_node and
// inserts it into the doubly linked address list. The code that does this
// checks to see if the block being inserted is contiguous to another free
// block or blocks. If so, the blocks are merged. If not, the newly added
// block is added to the size ordered list as well.
//
// Reallocation:
//
// Reallocation to a smaller size always succeeds. It is effectively the same
// as freeing the excess area.
//
// Reallocation to a larger size succeeds only if there is a free block of
// adequate size directly above the block which must grow. The code takes into
// account that the adjacent free block could be exactly the same size as
// the requested increment, in which case the entire free_node is consumed
// and is removed from the address and size lists.

#ifndef __SUBALLOC_H__
#define __SUBALLOC_H__

#ifndef NULL
#define NULL 0
#endif

#define __round_up__(val,n) ( (((val) + (n) - 1)/(n))*(n) )

namespace suballocator {

enum __alignment__ { block_alignment = sizeof(_int64) };


//////////////////////////////////////////////////////////////////////////////
class pool
{
public:
	pool(void* base, size_t size);
	void init(void* base, size_t size);
	void* alloc_low(size_t size);
	void* alloc_high(size_t size);
	bool dealloc_low( void* p, size_t n );
	bool dealloc_high( void* p, size_t n );
	char*  base(void) { return m_base; }
	char*  original_base(void) { return m_original_base; }
	size_t size(void) { return m_size; }
	size_t original_size(void) { return m_original_size; }

	char* m_original_base;
	size_t m_original_size;

	char* m_base;
	size_t m_size;
};

//////////////////////////////////////////////////////////////////////////////
class free_node
{
public:
	void zero(void);
	void init(void* p, size_t size);
	char*  base(void) { return m_base; }
	size_t size(void) { return m_size; }
	void  grow(size_t n);
	void*  shrink(size_t n);

	char*			m_base;
	size_t			m_size;
	free_node* 		m_larger;
	free_node* 		m_smaller;
	free_node* 		m_nextaddr;
	free_node*		m_prevaddr;
};

//////////////////////////////////////////////////////////////////////////////
class heap
{
	union block_hdr
	{
		size_t m_size;
		char m_pad[__round_up__(sizeof(size_t),block_alignment)];
	};

public:
	enum heap_strategy { first_fit, best_fit };

	heap(
		void* region=0,
		size_t heap_size=0,
		heap_strategy strategy=best_fit
		);

	void init(
		void* region,
		size_t heap_size,
		heap_strategy strategy=best_fit
		);

	~heap(void) { }

	void* allocate(size_t n);
	void  deallocate(void* p);
	void* reallocate(void* p, size_t new_size);

	size_t largest_free(void);
	void* region(void) { return m_pool.original_base(); }
	size_t allocated_size(void) { return m_pool.original_size(); }

	void (*bad_deallocate)(heap*,void*);

protected:
	bool insert_node_address( free_node* node_to_insert );
	void insert_node_size( free_node* node_to_insert );
	void remove_node_address( free_node* node_to_remove );

	free_node* new_free_node(void);
	bool delete_free_node(free_node* pnode);
	void* new_block(size_t n);
	bool delete_block(void* pblock, size_t n);
	void insert_free_node( free_node* pnode );
	free_node* get_node(void);
	void insert_node_avail(free_node* pnode);
	void release_node( free_node* pnode );
	void remove_node_size(free_node* pnode);
	void* use_node(free_node* pnode, size_t n);

	static void default_bad_deallocate(heap*,void*) { }

	// data
	heap_strategy 	m_strategy;
	unsigned int	m_active_block_count;
	unsigned int	m_avail_node_count;

	pool 			m_pool;

	free_node* 		m_freeaddr;
	free_node* 		m_freesize_ascend;
	free_node* 		m_freesize_descend;
	free_node*		m_availnodes;
};

//////////////////////////////////////////////////////////////////////////////
// Inline members for class pool

inline
pool::pool(void* base, size_t size) :
		m_original_base((char*)base),
		m_original_size(size),
		m_base((char*)base),
		m_size(size)
	{}

inline
void pool::init(void* base, size_t size)
{
	m_original_base = (char*)base;
	m_original_size = size;
	m_base = (char*)base;
	m_size = size;
}

inline
void* pool::alloc_low(size_t size)
{
	if ( size <= m_size )
	{
		m_size -= size;
		m_base += size;
		return m_base - size;
	}
	else
		return NULL;
};

inline
void* pool::alloc_high(size_t size)
{
	if ( size <= m_size )
	{
		m_size -= size;
		return m_base + m_size;
	}
	else
		return NULL;
}

inline
bool pool::dealloc_low( void* p, size_t n )
{
	char* pblock = (char*)p;

	if ( (pblock + n) == m_base )
	{
		m_base = pblock;
		m_size += n;

		return true;
	}
	else
		return false;
}

inline
bool pool::dealloc_high( void* p, size_t n )
{
	char* pblock =  (char*)p;

	if ( pblock == (m_base + m_size) )
	{
		m_size += n;
		return true;
	}
	else
		return false;
}


//////////////////////////////////////////////////////////////////////////////
// inline members for class free_node

inline
void free_node::zero(void)
{
	m_base = NULL;
	m_size = 0;
	m_larger = NULL;
	m_smaller = NULL;
	m_nextaddr = NULL;
	m_prevaddr = NULL;
}

inline
void free_node::init(void* p, size_t size)
{
	m_base = (char*)p;
	m_size = size;
}

inline
void free_node::grow(size_t n)
{
	m_size += n;
}

inline
void* free_node::shrink(size_t n)
{
	void* block = m_base;
	m_size -= n;
	m_base += n;
	return block;
}

//////////////////////////////////////////////////////////////////////////////
// inline members for class heap

inline
heap::heap(
	void* region,
	size_t heap_size,
	heap_strategy strategy
	) :
	bad_deallocate(default_bad_deallocate),
	m_strategy(strategy),
	m_active_block_count(0),
	m_avail_node_count(0),
	m_pool(region, heap_size),
	m_freeaddr(0),
	m_freesize_ascend(0),
	m_freesize_descend(0),
	m_availnodes(0)
	{}

inline
void heap::init(
	void* region,
	size_t heap_size,
	heap_strategy strategy
	)
{
	bad_deallocate = default_bad_deallocate;
	m_strategy = strategy;
	m_active_block_count = 0;
	m_avail_node_count = 0;
	m_pool.init(region, heap_size);
	m_freeaddr = 0;
	m_freesize_ascend = 0;
	m_freesize_descend = 0;
	m_availnodes = 0;
}

inline
free_node* heap::new_free_node(void)
{
	free_node* pnode = (free_node*)m_pool.alloc_low(sizeof(free_node));
	if ( pnode != NULL )
	{
		pnode->zero();
	}

	return pnode;
}

inline
bool heap::delete_free_node(free_node* pnode)
{
	return m_pool.dealloc_low(pnode, sizeof(free_node));
}

inline
void* heap::new_block(size_t n)
{
	return m_pool.alloc_high(n);
}

inline
bool heap::delete_block(void* pblock, size_t n)
{
	return m_pool.dealloc_high(pblock, n);
}

inline
free_node* heap::get_node(void)
{
	free_node* pnode;

	if ( m_availnodes != NULL )
	{
		m_avail_node_count--;
		pnode = m_availnodes;
		m_availnodes = pnode->m_nextaddr;
		pnode->zero();

		return pnode;
	}
	else
	{
		pnode = new_free_node();
		return pnode;
	}
}

inline
void heap::insert_node_avail(free_node* pnode)
{
	m_avail_node_count++;
	pnode->m_nextaddr = m_availnodes;
	m_availnodes = pnode;
}

inline
void heap::release_node( free_node* pnode )
{
	if ( !delete_free_node(pnode) )
	{
		insert_node_avail(pnode);
	}
}

inline
void* heap::use_node(free_node* pnode, size_t n)
{
	block_hdr* head;

	remove_node_size(pnode);
	head = (block_hdr*)pnode->shrink(n);
	head->m_size = n;
	insert_node_size(pnode);
	return &head[1];
}

inline
size_t heap::largest_free(void)
{
	size_t block;
	size_t poolsize;

	if ( m_freesize_descend != NULL )
		block = m_freesize_descend->size();
	else
		block = 0;

	poolsize = m_pool.m_size;
	if ( poolsize > (sizeof(free_node)*2 + sizeof(block_hdr)) )
		poolsize = poolsize - (sizeof(free_node)*2+sizeof(block_hdr));
	else
		poolsize = 0;

	return ( block > poolsize ) ? block : poolsize;
}

}; // namespace suballocator


#endif // __SUBALLOC_H__
