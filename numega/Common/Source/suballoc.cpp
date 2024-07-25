// suballoc.cpp - suballocator for use with STL / kernel mode
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

#include <suballoc.h>

using namespace suballocator;

///////////////////////////////////////////////////////////////////////////////
//
//	heap::allocate
//		Allocate a block
//
//	Parameters:
//		request
//			size of block to allocate
//
//	Return Value:
//		Pointer to allocated block, or NULL
//
//	Comments:
//		A request size of zero returns NULL.
//
void* heap::allocate(size_t request)
{
	if ( request == 0 )
		return NULL;

	// If the count of available free nodes is less than twice
	// the number of active blocks, try to allocate a new one
	// from pool. This anticipates the point when the pool is
	// eventually exhausted, and deallocate will need to rely
	// on previously allocated free_nodes.

	if ( m_avail_node_count < m_active_block_count*2 )
	{
		free_node* pnew = new_free_node();
		if (pnew != NULL)
			insert_node_avail(pnew);
	}

	// Add to the request size the size of the block header, and
	// round up the result to the block alignment.

	size_t n = request + sizeof(block_hdr);
	n = __round_up__(n, (unsigned)block_alignment);

	// Now search the free space for a satisfactory block. If the
	// strategy is first_fit, use the first satisfactory block. For
	// best fit, search the descending size list until exhausted or
	// until a smaller block is found.

	free_node* pnode;
	free_node* best;

	void* block;
	block_hdr* phead;

	for (pnode=m_freesize_descend, best=NULL; pnode != NULL; pnode = pnode->m_smaller)
	{
		size_t freeblock_size = pnode->size();

		if ( freeblock_size == n )
		{
			block = pnode->base();
			remove_node_size(pnode);
			remove_node_address(pnode);
			release_node(pnode);
			phead = (block_hdr*)block;
			phead->m_size = n;
			m_active_block_count++;
			return phead+1;
		}

		if ( freeblock_size > n)
		{
			if ( m_strategy == first_fit )
			{
				m_active_block_count++;
				return use_node(pnode, n);
			}
			else
			{
				best = pnode;
				continue;
			}
		}

		if ( freeblock_size < n )
			break;
	}

	if ( best != NULL )
	{
		m_active_block_count++;
		return use_node(best, n);
	}

	// If there is nothing in the free list, try to allocate the block
	// from the pool.

	void* pnew = new_block(n);
	if ( pnew != NULL )
	{
		block_hdr* pblock = (block_hdr*)pnew;
		pblock->m_size = n;
		m_active_block_count++;
		return pblock+1;
	}
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
//	heap::deallocate
//		Deallocate a block.
//
//	Parameters:
//		p
//			pointer to the block to be deallocated
//
//	Return Value:
//		None
//
//	Comments:
//		This code verifies that the start of the block is within the bounds
//		of the heap before attempting to delete it. If not, it calls the
//		function pointed to by public data member bad_deallocate. This function
//		has the following prototype:
//
//			void bad_deallocate_callback(heap* ph, void* pb)
//
//		where ph is a pointer to this heap, and pb is a pointer to the block
//		that was deallocated.
//
void heap::deallocate(void* p)
{
	if ( ! ( (m_pool.base() <= p) && (p <= m_pool.base()+m_pool.original_size()) ) )
	{
		bad_deallocate(this, p);
		return;
	}

	// Get a free_node, initialize it, and put it into the address and size lists.

	free_node* pnode = get_node();

	if ( pnode != NULL )
	{
		block_hdr* phead = (block_hdr*)p - 1;
		pnode->init(phead, phead->m_size);
		insert_free_node(pnode);
	}

	m_active_block_count--;
}

///////////////////////////////////////////////////////////////////////////////
//
//	heap::reallocate
//		Grow or shrink a previously allocated block
//
//	Parameters:
//		pblock
//			Block to be grown or shrunk
//
//		new_size
//			New size (in bytes) of block after modification
//
//	Return Value:
//		Returns a pointer to the resized block if successful, or NULL if
//		not successful. If not successful, the block is unchanged.
//
//	Comments:
//		If new_size is zero, the block is freed, and the function returns NULL.
//
//		Currently does not attempt to move the block to a new address.
//
void* heap::reallocate(void* pblock, size_t new_size)
{
	// validate the block

	if ( pblock == NULL )
		return NULL;

	if ( ! ( (m_pool.base() <= pblock) && (pblock <= m_pool.base()+m_pool.original_size()) ) )
		return NULL;

	// reallocation to size zero is the same as deallocation

	if ( new_size == 0 )
	{
		deallocate(pblock);
		return NULL;
	}

	block_hdr* phead = (block_hdr*)pblock - 1;
	size_t current_size = phead->m_size;

	if ( current_size == new_size )
		return pblock;

	else if ( current_size > new_size ) // shrink
	{
		free_node* pnode = get_node();

		if ( pnode == NULL )
			return pblock;

		pnode->init( (char*)phead + new_size, current_size-new_size );
		insert_free_node(pnode);
		phead->m_size = new_size;
		return pblock;
	}
	else // try to grow
	{
		free_node* pnode;

		// Search the free space by ascending address to find the first
		// block above the block to grow. If it is directly adjacent, check
		// its size to determine if it is adequate to meet the request.

		for (pnode = m_freeaddr; pnode != NULL; pnode = pnode->m_nextaddr)
		{
			if ( (pnode->base() == ((char*)phead+current_size) ) )
			{
				if ( pnode->size() > (new_size - current_size) )
				{
					phead->m_size = new_size;
					remove_node_size(pnode);
					pnode->shrink( new_size - current_size );
					insert_node_size(pnode);
					phead->m_size = new_size;
					return pblock;
				}
				else if ( pnode->size() == ( new_size - current_size ) )
				{
					phead->m_size = new_size;
					remove_node_address(pnode);
					remove_node_size(pnode);
					release_node(pnode);
					return pblock;
				}
			}

			if ( pnode->base() > (char*)phead )
				break;
		}

		return NULL;
	}
}

//////////////////// Protected member functions //////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//	heap::insert_free_node
//		Insert a node into the free space lists.
//
//	Parameters:
//		node_to_insert
//			Pointer to the initialized node to be inserted
//
//	Return Value:
//		None
//
//	Comments:
//		First tries to put it back into the pool. If that fails, it gets
//		put on the address list. Unless it gets merged with another block,
//		it is then put onto the size list.
//
void heap::insert_free_node( free_node* node_to_insert )
{
	if ( delete_block( node_to_insert->base(), node_to_insert->size() ) )
	{
		release_node( node_to_insert );
	}
	else
	{
		if ( insert_node_address( node_to_insert ) )
			insert_node_size( node_to_insert );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	heap::insert_node_size
//		Insert a free_node into the doubly linked size list.
//
//	Parameters:
//		node_to_insert
//			Pointer to the initialized free_node to be inserted.
//
//	Return Value:
//		None.
//
//	Comments:
//		Walks down the descending size list and inserts at appropriate point,
//		maintaining list heads.
//
void heap::insert_node_size( free_node* node_to_insert )
{
	size_t ins_size = node_to_insert->size();

	for (free_node* pnode=m_freesize_descend; pnode != NULL;  pnode=pnode->m_smaller)
	{
		if ( ins_size > pnode->size() )
			break;
	}

	if ( pnode == NULL ) // new node is smallest
	{
		node_to_insert->m_smaller = NULL;
		node_to_insert->m_larger = m_freesize_ascend;

		if ( m_freesize_ascend != NULL )
			m_freesize_ascend->m_smaller = node_to_insert;

		m_freesize_ascend = node_to_insert;

		if ( m_freesize_descend == NULL )
			m_freesize_descend = node_to_insert;
	}
	else // new node is bigger than pnode
	{
		node_to_insert->m_smaller = pnode;
		node_to_insert->m_larger = pnode->m_larger;

		if ( pnode->m_larger != NULL )
			pnode->m_larger->m_smaller = node_to_insert;
		else
			m_freesize_descend = node_to_insert;

		pnode->m_larger = node_to_insert;

		if ( m_freesize_ascend == NULL )
			m_freesize_ascend = node_to_insert;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	heap::remove_node_size
//		Removes a free_node from the doubly linked size list.
//
//	Parameters:
//		pnode
//			Points to the node to be unlinked
//
//	Return Value:
//		None.
//
//	Comments:
//		After unlinking, zeroes out the size list pointers in the node.
//
void heap::remove_node_size(free_node* pnode)
{
	free_node* larger = pnode->m_larger;
	free_node* smaller = pnode->m_smaller;

	if ( larger != NULL )
		larger->m_smaller = smaller;
	else
		m_freesize_descend = smaller;

	if ( smaller != NULL )
		smaller->m_larger = larger;
	else
		m_freesize_ascend = larger;

	pnode->m_larger = NULL;
	pnode->m_smaller = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
//	heap::insert_node_address
//		Insert a free_node into the address list
//
//	Parameters:
//		node_to_insert
//			Points to the initialized node to insert
//
//	Return Value:
//		Returns true if the node was inserted, or false if the node was merged.
//
//	Comments:
//		Attempts to merge the block with existing free_nodes.
//
bool heap::insert_node_address( free_node* node_to_insert )
{
	char*  ins_addr = node_to_insert->base();
	size_t ins_size = node_to_insert->size();

	free_node* pnode = m_freeaddr;

	// This loop walks the ascending address list.

	while ( pnode != NULL )
	{
		if (ins_addr < pnode->base()) // block to insert is below pnode
		{
			// Check if top of block to insert is base of pnode. If so,
			// merge it. This case returns true to force insertion of
			// the block into the size list.

			if ( (ins_addr + ins_size) == pnode->base() )
			{
				node_to_insert->grow( pnode->size() );
				node_to_insert->m_nextaddr = pnode->m_nextaddr;
				node_to_insert->m_prevaddr = pnode->m_prevaddr;
				if ( pnode->m_nextaddr != NULL )
					pnode->m_nextaddr->m_prevaddr = node_to_insert;
				if (pnode->m_prevaddr != NULL)
					pnode->m_prevaddr->m_nextaddr = node_to_insert;
				else
					m_freeaddr = node_to_insert;

				remove_node_size(pnode);
				release_node(pnode);
			}

			// Not contiguous, but below. Link into list and return true.

			else
			{
				node_to_insert->m_nextaddr = pnode;
				node_to_insert->m_prevaddr = pnode->m_prevaddr;

				if ( pnode->m_prevaddr != NULL )
					pnode->m_prevaddr->m_nextaddr = node_to_insert;
				else
					m_freeaddr = node_to_insert;

				pnode->m_prevaddr = node_to_insert;
			}
			return true;
		}

		// Check if base of block to insert is top of pnode. If so,
		// do the merge.

		else if ( (pnode->base() + pnode->size()) == ins_addr )
		{
			remove_node_size(pnode);
			pnode->grow(ins_size);
			release_node(node_to_insert);

			free_node* pnext = pnode->m_nextaddr;
			if ( pnext != NULL)
			{
				// Check for "double merge" case, where block to insert
				// exactly fills gap between two existing free blocks.

				if ( (ins_addr+ins_size) == pnext->base() )
				{
					pnode->grow(pnext->size());
					pnode->m_nextaddr = pnext->m_nextaddr;
					if (pnext->m_nextaddr != NULL)
						pnext->m_nextaddr->m_prevaddr = pnode;
					remove_node_size(pnext);
					release_node(pnext);
				}
			}

			insert_node_size(pnode);
			return false;
		}

		// Check if we are at the last node, which means that block to
		// insert is new end of the list. Tack it on and return true.

		else if ( pnode->m_nextaddr == NULL )
		{
			pnode->m_nextaddr = node_to_insert;
			node_to_insert->m_nextaddr = NULL;
			node_to_insert->m_prevaddr = pnode;
			return true;
		}

		// Go to next node.

		pnode = pnode->m_nextaddr;
	}

	// This executes only if the list was empty initially.

	m_freeaddr = node_to_insert;
	node_to_insert->m_nextaddr = node_to_insert->m_prevaddr = NULL;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//	heap::remove_node_address
//		Remove a node from the address list.
//
//	Parameters:
//		node_to_remove
//			Pointer to the node to be removed.
//
//	Return Value:
//		None.
//
//	Comments:
//		Unlink it.
//
void heap::remove_node_address( free_node* node_to_remove )
{
	free_node* next = node_to_remove->m_nextaddr;
	free_node* prev = node_to_remove->m_prevaddr;

	if ( prev != NULL )
	{
		prev->m_nextaddr = next;
	}
	else
	{
		m_freeaddr = next;
	}

	if ( next != NULL )
	{
		next->m_prevaddr = prev;
	}
}
