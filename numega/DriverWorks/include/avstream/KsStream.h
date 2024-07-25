// KsStreamPointer.h: KS Stream Access
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

#if !defined(AFX_KSSTREAM_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_)
#define AFX_KSSTREAM_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// fwd
class KsStreamPointer;

////////////////////////////////////////////////////////////////////
// KsStreamPointerBase
//
// Simple wrapper for PKSSTREAM_POINTER used as a base only
//
// KsStreamPointerBase exposes method Clone() producing "cloned" KsStreamPointer-s.
//
// There are 3 types of stream pointers used to point to actual data frames:
// KsStreamLeadingPointer, KsStreamTrailingPointer and KsStreamPointer - derived from
// KsStreamPointerBase.
//
// Leading and Tailing can be instantiated directly, while KsStreamPointer can
// only be "cloned" from one of them or another KsStreamPointer.
// After being locked, KsStreamPointer should be released (Release()) in order
// to dereference the pointed frame.
//
// The Clone() method is parametrized by the pin class P and its behavior is
// determined by the design of P.
//
// If KsPin-derived class P exposes P::OnStreamCancel() handler, Clone() method
// registers the cancel callback. The optional context of the stream pointer
// is determined by type P::StreamPointerContextType, which is PVOID by default.
//
class KsStreamPointerBase
{
public:

// Locking
	NTSTATUS Lock();
    void Unlock(BOOLEAN Eject=FALSE);

    // Advancing
    NTSTATUS Advance();
    NTSTATUS Advance(ULONG InUsed, ULONG OutUsed, BOOLEAN Eject=FALSE);
    NTSTATUS AdvanceOut(ULONG OutUsed);
    NTSTATUS AdvanceIn(ULONG InUsed);

    // Data offset accessors
    KSSTREAM_POINTER_OFFSET& OffsetOut() { return *&m_ptr->OffsetOut; }
    KSSTREAM_POINTER_OFFSET& OffsetIn() { return *&m_ptr->OffsetIn; }
    PKSSTREAM_HEADER& Header() { return *&m_ptr->StreamHeader; }

    void Invalidate() { m_ptr=NULL; }
    inline bool IsValid() const { return m_ptr!=NULL; }

    // Conversions
    inline operator PKSSTREAM_POINTER() const { return m_ptr; }
    inline operator PKSSTREAM_HEADER() const  { return m_ptr->StreamHeader; }
    inline PKSSTREAM_POINTER operator->()  const { return m_ptr; }

    // Cloning
    template <class P>
    KsStreamPointer Clone(P* /*Pin*/)
#if defined (_IA64_)
{
	ASSERT(IsValid());
	KsStreamPointer	ptr;
	NTSTATUS status = ::KsStreamPointerClone(m_ptr,
		(NOT_DEFAULT_HANDLER_P1(P, OnStreamCancel, PKSSTREAM_POINTER)) ?
		KsPinDispatch<P>::StreamPointerCancelHandler : NULL,
		NOT_DEFAULT_TYPE(P::StreamPointerContext_t) ?
		sizeof(P::StreamPointerContext_t) : 0,
		&ptr.m_ptr);
	ASSERT(status==STATUS_SUCCESS);
	return ptr;
}

#else
	;
#endif

// Comparisons
	bool operator==(PKSSTREAM_POINTER ptr) const { return m_ptr==ptr; }
	bool operator!=(PKSSTREAM_POINTER ptr) const { return m_ptr!=ptr; }

protected:

	PKSSTREAM_POINTER	m_ptr;

	KsStreamPointerBase() {m_ptr=0;}
	~KsStreamPointerBase(){}

};


////////////////////////////////////////////////////////////////////
// KsStreamLeadingPointer
// KsStreamTrailingPointer
// KsStreamPointer
//
// Stream Pointers types.
//
// Usage Example:
//
//	KsStreamLeadingPointer	Leading(*this);
//  KsStreamPointer			ptr = Leading.Clone(this);
//
class KsStreamLeadingPointer : public KsStreamPointerBase
{
public:
	KsStreamLeadingPointer(PKSPIN Pin, KSSTREAM_POINTER_STATE State=KSSTREAM_POINTER_STATE_LOCKED)
	{ m_ptr = ::KsPinGetLeadingEdgeStreamPointer(Pin, State); }
};

class KsStreamTrailingPointer : public KsStreamPointerBase
{
public:
	KsStreamTrailingPointer(PKSPIN Pin, KSSTREAM_POINTER_STATE State=KSSTREAM_POINTER_STATE_LOCKED)
	{ m_ptr = ::KsPinGetTrailingEdgeStreamPointer(Pin, State); }
};

class KsStreamPointer : public KsStreamPointerBase
{
public:
	// ctor-s
	KsStreamPointer() : KsStreamPointerBase() {}
	KsStreamPointer(const KsStreamPointer& src) { m_ptr = src.m_ptr; }
	KsStreamPointer(const PKSSTREAM_POINTER ptr) { m_ptr = ptr; }

	// releasing the locked one
	void Release()
	{ ::KsStreamPointerDelete(m_ptr); m_ptr=NULL; }

	// getting clone list
	KsStreamPointer GetNextClone();

	// Stream Pointer's Context accessor
	template <class P>
	typename P::StreamPointerContext_t* Context(P* /*Pin*/)
		{
			// don't call me if no context is defined
			STATIC_ASSERT(NOT_DEFAULT_TYPE(typename P::StreamPointerContext_t));
			return reinterpret_cast<typename P::StreamPointerContext_t*> (m_ptr->Context);
		}


};



/////////////////// Implementations /////////////////////

inline
NTSTATUS KsStreamPointerBase::Lock()
{
	return ::KsStreamPointerLock(m_ptr);
}

inline
void KsStreamPointerBase::Unlock(BOOLEAN Eject)
{
	::KsStreamPointerUnlock(m_ptr, Eject);
}

inline
NTSTATUS KsStreamPointerBase::Advance()
{ return ::KsStreamPointerAdvance(m_ptr);
}

inline
NTSTATUS KsStreamPointerBase::Advance(ULONG InUsed, ULONG OutUsed, BOOLEAN Eject)
{
 ASSERT(m_ptr && m_ptr->OffsetOut.Remaining >= OutUsed && m_ptr->OffsetIn.Remaining >= InUsed);
 return ::KsStreamPointerAdvanceOffsets(m_ptr, InUsed, OutUsed, Eject);
}

inline
NTSTATUS KsStreamPointerBase::AdvanceOut(ULONG OutUsed)
{
 ASSERT(m_ptr && m_ptr->OffsetOut.Remaining >= OutUsed);
 return ::KsStreamPointerAdvanceOffsets(m_ptr, 0, OutUsed, 	FALSE);
}
inline
NTSTATUS KsStreamPointerBase::AdvanceIn(ULONG InUsed)
{
 ASSERT(m_ptr && m_ptr->OffsetIn.Remaining >= InUsed);
 return ::KsStreamPointerAdvanceOffsets(m_ptr, InUsed, 0, FALSE);
}

#if !defined(_IA64_)
template <class P>
KsStreamPointer KsStreamPointerBase::Clone(P* /*Pin*/)
{
	ASSERT(IsValid());
	KsStreamPointer	ptr;
	NTSTATUS status = ::KsStreamPointerClone(m_ptr,
		(NOT_DEFAULT_HANDLER_P1(P, OnStreamCancel, PKSSTREAM_POINTER)) ?
		KsPinDispatch<P>::StreamPointerCancelHandler : NULL,
		NOT_DEFAULT_TYPE(P::StreamPointerContext_t) ?
		sizeof(P::StreamPointerContext_t) : 0,
		&ptr.m_ptr);
	ASSERT(status==STATUS_SUCCESS);
	return ptr;
}
#endif

inline
KsStreamPointer KsStreamPointer::GetNextClone()
{
  ASSERT(IsValid());
  KsStreamPointer ptr = ::KsStreamPointerGetNextClone(m_ptr);
  return ptr;
}

#endif // !defined(AFX_KSSTREAM_H__CFF0948C_885F_4C43_AA3D_D6E430780CD6__INCLUDED_)
