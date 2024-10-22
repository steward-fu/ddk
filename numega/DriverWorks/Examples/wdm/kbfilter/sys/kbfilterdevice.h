// KbfilterDevice.h
//
// Generated by DriverWizard version DriverStudio 2.0.0 (Build 473)
// Requires Compuware's DriverWorks classes
//

#ifndef __KbfilterDevice_h__
#define __KbfilterDevice_h__
#include "kbdmou.h"

// TODO:	The wizard creates a single driver managed queue class.
//			You may choose to create additional classes derived from
//			KDriverManagedQueueCsq to help manage Irp processing.
/*
class KbfilterDevice_DriverManagedQueue : public KDriverManagedQueueCsq
{
public:
	SAFE_DESTRUCTORS;
	VOID StartIo(KIrp I);
};
*/
typedef
VOID
(*PMY_SERVICE_CALLBACK) (
    PDEVICE_OBJECT pDeviceObject,
    PKEYBOARD_INPUT_DATA InputDataStart,
    PKEYBOARD_INPUT_DATA InputDataEnd,
    PULONG InputDataConsumed
    );


#define DEVMEMBER_SERVICE_CALLBACK(classname, function)	\
	VOID function(PKEYBOARD_INPUT_DATA InputDataStart,\
	PKEYBOARD_INPUT_DATA InputDataEnd,              \
	PULONG InputDataConsumed);						\
	static VOID LinkTo(function)(						\
    	PDEVICE_OBJECT pDevObj,					\
	    PKEYBOARD_INPUT_DATA InputDataStart,\
		PKEYBOARD_INPUT_DATA InputDataEnd,\
		PULONG InputDataConsumed)							\
	{												\
		((classname*)KDevicePTR(pDevObj))			\
			->function(InputDataStart,InputDataEnd,InputDataConsumed);						\
	}


class KbfilterDevice : public KWdmFilterDevice
{
	// Constructors
public:
	SAFE_DESTRUCTORS;
	KbfilterDevice(PDEVICE_OBJECT Pdo, ULONG Unit);
	~KbfilterDevice();
	char ConvertScancodeToLetter(ULONG scancode);
	// Member Functions
public:
	DEVMEMBER_SERVICE_CALLBACK(KbfilterDevice, KBServiceCallback);
	MEMBER_PASSTHROUGH_COMPLETE(KbfilterDevice, InternalDeviceCompletion);
	virtual NTSTATUS InternalDeviceControl(KIrp I);

	// Member Data
protected:
		// Unit number for this device (0-9)
	ULONG					m_Unit;
	PDEVICE_OBJECT			m_RealDeviceObject;
	PMY_SERVICE_CALLBACK	m_RealClassService;
};

#endif
