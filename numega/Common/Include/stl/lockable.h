// lockable.h - lockable and related templates for kernel STL
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

#ifndef __LOCKABLE__H_
#define __LOCKABLE__H_

#include <memory>

namespace ntkm {

////////////////////////////////////////////////////////////////////////////////////
// class lockable
//
//	Template class lockable embeds a reference to an existing instance of any class,
//	and inherits from a generic lock class (see kgenlock.h).
//
// Template parameters
//		T	the object type which is to be subject to locking/unlocking
//		L	the lock type (see kgenlock.h)
//
// Usage
//	This template enables locking of arbitrary types using any lock class that
//	conforms to the generic lock interface.
//
//	The object to be locked must be created and passed as a parameter to the ctor
//	of lockable.
//
//	operator()(void) returns embedded reference.
//
//	Members lock() and unlock() are used to restrict access to the object.
//
// Example:
//
//		struct sensitive {
//			int m_a;
//			int m_b;
//		};
//
//		sensitve x;
//		lockable<sensitive, KGenericLockSpinLock> lx(x);
//
//		lx.lock();
//		lx().m_a++;
//		lx().m_b++;
//		lx.unlock();
//
template <class T, class L> class lockable : public L
{
public:
	explicit lockable(T& Obj) : m_ref(Obj) {}

	void lock(void) { Lock();}
	void unlock(void) { Unlock();}

	T& operator()() { return m_ref; }
	T& operator=(const T& rval) { m_ref = rval; return m_ref; }
	operator T&() { return m_ref; }

protected:
	T&	m_ref;

private:
	lockable( const lockable<T,L>& );
};

////////////////////////////////////////////////////////////////////////////////////
// class lockable_container
//
//	Template class lockable_container inherits from any class that has matching ctor
//	forms (see below), and inherits from a generic lock class (see kgenlock.h).
//
// Template parameters
//		T	the object type which is to be subject to locking/unlocking
//		L	the lock type (see kgenlock.h)
//
// Usage
//	Type T must have ctors of the following forms:
//		T(void);			// default ctor
//		T(size_t size);		// common container ctor w/ size parameter
//
//	The advantage of this class is that the driver can call members of class T or
//	class L directory. This is really just a simple composition of the two classes
//	that assumes certain ctor forms and renames the Lock/Unlock functions of
//	generic lock interface.
//
// Example
//
//		lockable_container< vector<int>, KGenericLockSpinLock> lvec;
//
//		lvec.lock();
//		lvec[0] = 0;
//		lvec[1] = 1;
//		lvec.unlock();
//

template <class T, class L> class lockable_container : public T, public L
{
public:
	lockable_container(void) : T(), L() {}
	lockable_container( typename T::size_type s ) : T(s), L() {}

	void lock(void) { Lock();}
	void unlock(void) { Unlock();}

private:
	lockable_container( const lockable_container<T,L> & ); // do not allow
	lockable_container( T& ); // do not allow
};

////////////////////////////////////////////////////////////////////////////////////
// class lockable_ptr
//
// Usage
//
//	This class is simply a composition of auto_ptr and a generic lock class. The ctor
//	takes as a parameter the pointer that comes under the ownership of the class.
//	Generic lock member functions Lock and Unlock are renamed to lock and unlock.
//
// Example
//		lockable_ptr< vector<int>, KGenericLockMutex > lptr(new vector<int>(300));
//		lptr.lock();
//		lptr[0]++;
//		lptr.unlock();

template <class T, class L >
class lockable_ptr : public std::auto_ptr<T>, public L
{
public:
	lockable_ptr(T* pt) : auto_ptr<T>(pt), L() {}

	void lock(void) { Lock();}
	void unlock(void) { Unlock();}
};

}; // namespace ntkm

#endif // __LOCKABLE__H_
