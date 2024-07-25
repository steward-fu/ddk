#include <vdw.h>

#include "..\Chat1394Deviceinterface.h"

#include "Chat1394.h"
#include "ChatClient.h"
#include "Chat1394Device.h"
#include "..\Chat1394ioctl.h"


////////////////////////////////////////////////////////////////////////
//Chat1394Client::Initialize
//
// Parameters
//	LowerDevice - Pointer to the K1394LowerDevice object to initialize 
//		K1394AsyncClient.
//	DeviceObject - Pointer to our device object class so we can notify it
//      as writes come in.
//
// Returns:
//	 STATUS_SUCCESS if successful
//	 STATUS_INSUFFICIENT_RESOURCES if it couldn't allocate the fifo elements
//
// Comments:
//	Allocates the address range and a number of FIFO structures
//
NTSTATUS Chat1394Client::Initialize(K1394LowerDevice* LowerDevice, Chat1394Device * DeviceObject)
{
	m_pDeviceObject = DeviceObject;

	K1394AsyncClient::Initialize(LowerDevice);

	//Allocate some buffers and MDLs and push them on the FIFO
	// NOTE: this should be done before the address range is allocated
	//  as calls to the address range may be received any time after
	//  the call to allocate.
	K1394FifoElement *pFifoElement;
	
	for(ULONG next = 0; next < MAX_ADDRESS_RANGE_BUFFERS; next++)
	{
		pFifoElement = new K1394FifoElement(ADDRESS_RANGE_BUFFER_SIZE);
		if(pFifoElement && pFifoElement->IsValid())
			m_AddressFifo.Push(pFifoElement);
		else 
			break;
	}
	
	if(next < MAX_ADDRESS_RANGE_BUFFERS)
	{
		//At least one call failed.  Pop and free any successfully 
		// allocated fifo elements
		pFifoElement = (K1394FifoElement*)m_AddressFifo.Pop();
		while(pFifoElement)
		{
			pFifoElement->Free();
			pFifoElement = (K1394FifoElement*)m_AddressFifo.Pop();		
		}
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	

	//Allocate our address range 
	ADDRESS_OFFSET HardCodedOffset;
	HardCodedOffset.Off_High = 1;
	HardCodedOffset.Off_Low = 0;
	
	NTSTATUS status = Allocate(m_AddressFifo, ADDRESS_RANGE_BUFFER_SIZE, HardCodedOffset);
	
	if(!NT_SUCCESS(status))
	{ //Clean up the fifo elements we allocated if the call didn't succeed.
		K1394FifoElement * pFifoElement = (K1394FifoElement*)m_AddressFifo.Pop();
		while(pFifoElement)
		{
			pFifoElement->Free();
			pFifoElement = (K1394FifoElement*)m_AddressFifo.Pop();		
		}
	}
	

	return status;
}

////////////////////////////////////////////////////////////////////////
//Chat1394Client::Invalidate
//
// Frees the address range and the fifo elements allocated
//
// Parameters:
//	None.
//
// Comments:
//
//
VOID Chat1394Client::Invalidate()
{
	t << "Entering Chat1394Client::Invalidate\n";

	//Free our address range. (Stops any calls to OnWrite)
	Free(m_AddressFifo);

	//Free the FIFO elements we allocated
	K1394FifoElement *pFifoElement;

	pFifoElement = (K1394FifoElement*)m_AddressFifo.Pop();
	while(pFifoElement)
	{
		pFifoElement->Free();
		delete pFifoElement;
		pFifoElement = (K1394FifoElement*)m_AddressFifo.Pop();		
	}
}

///////////////////////////////////////////////////////////////////////
//Chat1394Client::OnWrite
//
// Called when our address range is written to.
//
// Parameters:
//	pMdl - The MDL describing the change to the address range.
//  Offset - The offset into the address range to which the write occured
//	Length - The number of bytes written
//  pAddressFifo - The address fifo element that was popped off the stack
//  
// Returns:
//	None.
//
// Comments:
//	
//
VOID Chat1394Client::OnWrite(PMDL pMdl, ULONG Offset, ULONG Length, PADDRESS_FIFO pAddressFifo)
{
	t << "Entering Chat1394Device::OnWrite ";

	// Use the Mdl to create a pointer to the buffer to be read
	KMemory Mem(pMdl);
	PUCHAR pBuffer = (PUCHAR) Mem.MapToSystemSpace() + Offset;

	//Notify the device object of the received data
	m_pDeviceObject->DataReceived(pBuffer, Length);

	// Return the ADDRESS_FIFO structure to the address range
	m_AddressFifo.Push(pAddressFifo);
}
