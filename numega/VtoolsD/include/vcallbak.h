// Copyright (c) 1994, Compuware Corporation All Rights Reserved
//
// vcallbak.h - include file for callback classes
//

#include <vmachine.h>

#ifndef __vcallback_h_
#define __vcallback_h_

#define CALLBACKTHUNKSIZE 0x14

struct CallbackNode;

/*
class VCallback
---------------

  A VCallback object provides a means for an application to call a function
  in a VxD.

  Constructor
	VCallback(PVOID refData)

  Member functions
	handler		VxD level function invoked when
			application calls to the callback address

	getAddr		Returns the application level address that
			applications call to invoke the handler
			function.

	setReturn	Called from inside handler to modify client
			stack for proper return.

  Data members
	m_dwCBAddr	The application level address that
			applications call to invoke the handler
			function.
*/

class VCallback
{
public:
	virtual	BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);
	DWORD	getAddr() {return m_dwCBAddr;};
	virtual VOID setReturn()
#ifdef __BORLANDC__
		;
#else
		{
			Simulate_Far_Ret();
		}
#endif

protected:	
	static struct CallbackNode* AllocFromList(LISTHANDLE l);
	static struct CallbackNode* NewNode(LISTHANDLE* pl);
	DWORD	m_dwCBAddr;
	struct CallbackNode* m_Node;
};


typedef struct CallbackNode
{
	DWORD m_CBaddr;
	BYTE m_thunk[CALLBACKTHUNKSIZE];
} CALLBACKNODE;

/*
class VV86Callback
-------------------

  The class VV86Callback provides a means for a V86 mode application to
  directly invoke a function in a VxD.

  Constructor
	VV86Callback(PVOID refData)

  Destructor
	~VV86Callback(VOID)

  Usage
	Use a class derived from VV86Callback to cause a V86 mode
	application to invoke a function in your VxD.  Each
	VV86Callback that you create is associated with a unique
	segment:offset  in the V86 address space.  Calling or jumping
	to that address causes control to pass to the handler
	function of the class you derive from VV86Callback.
	The syntax of the handler is defined by the base class
	VCallback.  Create an instance of your class, and obtain the
	callback vector with the getAddr.  VxDs typically insert the
	callback vector at strategic locations in the V86 address
	space as a means to trap application level operations.

	You typically do not use a VV86Callback to provide a means to
	explicitly call your VxD.  Instead, you should use the V86
	mode API capability.  See class VDevice.

	Note that VV86Callback is a base class for class
	VInChainV86Int.  Use this class instead of inserting the
	callback vector into the V86 interrupt vector table of a
	virtual machine.

  Example
	class XMSTrap : public VV86Callback
	{
	public:
		virtual BOOL handler(VMHANDLE hVM,
			CLIENT_STRUCT* pRegs, PVOID refdata);
		DWORD m_savedXMSvector;
	};
*/

class VV86Callback : public VCallback
{
public:
	VV86Callback(PVOID refData=0);
	~VV86Callback(VOID);
	virtual VOID setReturn()
#ifdef __BORLANDC__
	;
#else
	{
		Simulate_Far_Ret();
	}
#endif
	virtual BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);

private:
	static LISTHANDLE m_list;
};

/*
class VProtModeCallback
-----------------------

  The class VProtModeCallback provides a means for a protected mode
  application to directly invoke a function in a VxD.

  Constructor
	VProtModeCallback(PVOID refData)

  Destructor
	~VProtModeCallback(VOID)

  Usage

	Use a class derived from VProtModeCallback to cause a protected mode
	application to invoke a function in your VxD.  Each VProtModeCallback
	that you create is associated with a unique selector:offset  in the
	protected mode address space.  Calling or jumping to that address
	causes control to pass to the handler function of the class you
	derive from VProtModeCallback.  The syntax of the handler is defined
	by the base class VCallback.  Create an instance of your class, and
	obtain the callback vector with the member function getAddr.  VxDs
	typically insert the callback vector at strategic locations in the
	application's address space as a means to trap application level
	operations.

	You typically do not use a VProtModeCallback to provide
	a means to explicitly call your VxD.  Instead, you should use the
	protected mode API capability.  See class VDevice.

	Note that VProtModeCallback is a base class for class VProtModeInt. 
	Use this class instead of inserting the interrupt vector to the
	callback vector.

*/
class VProtModeCallback : public VCallback
{
public:
	VProtModeCallback(PVOID refData=0);
	~VProtModeCallback(VOID);
	virtual BOOL handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData);

private:
	static LISTHANDLE m_list;
};

#endif

