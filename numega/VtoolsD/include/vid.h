// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vid.h - include file for (and implementation of) class VId
//

#ifdef WIN40

#ifdef __BORLANDC__
#pragma warn -inl
#endif

class VId    
{
public:
	VOID Block(DWORD flags=BLOCK_SVC_INTS)
		{
			m_bBlocked = TRUE;
			while (m_bBlocked)
				_BlockOnID ( (DWORD)this, flags);
		}
	VOID Signal()
		{
			m_bBlocked = FALSE;
			_SignalID( (DWORD)this);
		}

protected:
	volatile BOOL m_bBlocked;
};


#ifdef __BORLANDC__
#pragma warn .inl
#endif

#endif
