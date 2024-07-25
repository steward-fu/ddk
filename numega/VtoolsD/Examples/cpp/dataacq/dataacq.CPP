//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03060  USA
//
// Copyright (c) 1998 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
//=============================================================================

// DATAACQ.CPP - main module for the VxD DATAACQ sample
// Copyright (c) 1997 Compuware Corporation

// **********************************************************************
// Programmed by: Kevin van der Raad
//                Jobert Lommerse
//				  Hogeschool Haarlem
//				  Netherlands
// **********************************************************************

// This VxD is for win32 use ONLY

// Function: VxD for communication between the Data Acquisition Card 
// and a Data-acquisition application.
//
// use: Open handle to this VxD with create file (device name DATAACQ),
//
// We respond to the system control message W32DeviceIOControl, 
// we support I/O services with the following I/O control codes (dioc_IOCtlCode):
//      1 - get this VxD version number. size of buffer: WORD (2 BYTES)
//      2 - output one byte to DAC A. size: BYTE
//      3 - output one byte to DAC B. size: BYTE
//      4 - program timer for sampling from ADC
//              application should give a structure with the 
//              following information:
//
//              struct xsampling {      WORD dtime;             // time between samples in µs (65..8191)
//                                      WORD channel;                   // sampling channel (0..7)
//                                      WORD MsgID;                             // unique message-id, provided by application
//                                      HANDLE application;             // handle of application, where to post the samples to.
//                               } *sampling;
//
//      5 - change buffer from default. size: DWORD
//
// The VxD posts a message (with the given MSG ID) to the application 
// with the given handler which contains a pointer to the samplebuffer. 
// The VxD starts sampling (and posting messages) when the VxD-handle is
// opened (createfile) and stops when it is closed.
// So the application first have to supply its handler with 
// service nr. 4

#define DEVICE_MAIN
#include "dataacq.h"
Declare_Virtual_Device(DATAACQ)
#undef DEVICE_MAIN

// default size of data buffer
DWORD buffersize=256;

// Handle of application to send sample-buffer-pointer-messages to:
HANDLE oscilloscoop=NULL;

// MsgID, unique channel for sending messages to application:
// (provided by application)
UINT MsgID;

// devnode, created by OnNewPnpDevice:
DEVNODE devnode;
// Instance of End of Conversion interupt from the DAC:
XEOC_IRQ* pEOC_IRQ=NULL;
// Thunk for scheduling a global event
PEvent_THUNK pThunk;

// declare resource-variables
DWORD data_ac_base=1; //base address of data_ac device
int data_ac_irq=255;  //IRQ level of data_ac device

// declare pointers data buffer
PVOID pdatabase;
PCHAR pdatacursor;
DWORD dataindex=0;


CONFIGRET CM_HANDLER OnConfigure(CONFIGFUNC cf, SUBCONFIGFUNC scf, DEVNODE devnode, DWORD refdata, ULONG flags);

/********************************************************************************/
/********************************************************************************/

XEOC_IRQ::XEOC_IRQ() : VHardwareInt(data_ac_irq, 0,0,0) {}

/********************************************************************************/
/********************************************************************************/

EOC_GlobalEvent::EOC_GlobalEvent() : VGlobalEvent() {}

//********************************************************************************/
//********************************************************************************/
// function:            posts a message to the application. Message contains
//                                      a pointer to a sample-buffer located on the Heap.
// called by:           XEOC_IRQ::OnHardwareInt (scheduled a global event)
// return value(s): none

VOID EOC_GlobalEvent::handler(VMHANDLE hVM, CLIENT_STRUCT* pRegs, PVOID refData)
{
	dprintf("DATAACQ: Post Buffer @ %p",pdatabase);

	// send message with sample value to application
	if(_SHELL_PostMessage(oscilloscoop, MsgID, (WORD)buffersize, (DWORD)pdatabase, 0, 0))
		{
			
		}
	else 
		{
			dprintf("DATAACQ: IRQ-Event-handler, message NOT send!");
		}

}

//********************************************************************************/
//********************************************************************************/
// function:            gets a sample on a EOC_IRQ and writes it in a buffer
//                                      If buffer full, schedule a global event.
// called by:           Hardware interrupt callback. (VPICD)
// return value:        none

VOID XEOC_IRQ::OnHardwareInt(VMHANDLE hVM)
{
	
	//dprintf("DATAACQ: IRQ index %d",dataindex);

	*pdatacursor = _inp(data_ac_base+0);
	pdatacursor++;
	dataindex++;
	
	
	// schedule a global event
	if (dataindex==buffersize) 
	{
		EOC_GlobalEvent* pEvent = new EOC_GlobalEvent();
			if (pEvent)
			{
				pEvent->schedule();
				dataindex=0;
				pdatacursor=(PCHAR)pdatabase;
			}
			else
			{
				dataindex--;
				pdatacursor--;
			}
	}
	pEOC_IRQ->sendPhysicalEOI();
}


//********************************************************************************/
//********************************************************************************/
// function:            inits events
// called by:           OnSysDynamicDeviceInit control message
// return value:        none

BOOL DataAcqDevice::OnSysDynamicDeviceInit()
{
	
	dprintf("DATAACQ: dynamic device init");
	
	// initialisation to preform global events
	if (!VEvent::initEvents())
	{
		dprintf("DATAACQ: fatal error - initEvents()");
		return FALSE;
	}
	else return TRUE;
}

//********************************************************************************/
//********************************************************************************/
// function:            returns true
// called by:           OnSysDynamicDeviceExit control message
// return value:        true

BOOL DataAcqDevice::OnSysDynamicDeviceExit()
{
	dprintf("DATAACQ: is leaving the system");
	return TRUE;
}

//********************************************************************************/
//********************************************************************************/
// function:            registers this VxD
// called by:           OnPnpNewDevnode control message
// return value:        none

CONFIGRET DataAcqDevice::OnPnpNewDevnode(DEVNODE devnode, DWORD loadtype)
{
	dprintf("DATAACQ: PnP - new devnode:");
	switch (loadtype)
	{
	case DLVXD_LOAD_ENUMERATOR:
		{
			dprintf("DATAACQ: - loaded as an enumerator");
			return CR_DEFAULT;
		}
	case DLVXD_LOAD_DEVLOADER:
		{
			dprintf("DATAACQ: - loaded as a device loader");
			return CONFIGMG_Register_Device_Driver(devnode, OnConfigure, 0, CM_REGISTER_DEVICE_DRIVER_REMOVABLE | CM_REGISTER_DEVICE_DRIVER_DISABLEABLE);
		}
	case DLVXD_LOAD_DRIVER:
		{
			dprintf("DATAACQ: - loaded as a device driver");
			return CR_DEFAULT;
		}
	default:
		{
			dprintf("DATAACQ: - DLVXD not found");
			return CR_DLVXD_NOT_FOUND;
		}
	}
}

//********************************************************************************/
//********************************************************************************/
// function:        initialises allocated recources     
// called by:           configuration manager
// return value:        CR_SUCCESS, CR_FAILURE, CR_DEFAULT

CONFIGRET CM_HANDLER OnConfigure(CONFIGFUNC cf, SUBCONFIGFUNC scf, DEVNODE devnode, DWORD refdata, ULONG flags)
{
	CONFIGRET cr;
	CMCONFIG config;
	
	dprintf("DATAACQ: configuration-handler:");

	switch (cf)
	{
		case CONFIG_START:
			{
				dprintf("DATAACQ: Using allocated configuration");
				cr = CONFIGMG_Get_Alloc_Log_Conf(&config, devnode, CM_GET_ALLOC_LOG_CONF_ALLOC);
				{
					switch(cr)
					{
					case CR_INVALID_DEVNODE: 
						{
							dprintf("DATAACQ: invalid devnode");
							break;
						}
					case CR_INVALID_FLAG: 
						{
							dprintf("DATAACQ: invalid flag");
							break;
						}
					case CR_INVALID_POINTER: 
						{
							dprintf("DATAACQ: invalid pointer");
							break;
						}
					case CR_NO_MORE_LOG_CONF: 
						{
							dprintf("DATAACQ: no more log config");
							break;
						}
					case CR_SUCCESS: 
					case CR_DEFAULT: 
						{
							dprintf("IRQ    : %d",data_ac_irq  = config.bIRQRegisters[0]);
							dprintf("IOBASE : %3.3Xh",data_ac_base = config.wIOPortBase[0]);
							
							// create an instance of our interrupt class:
							pEOC_IRQ = new XEOC_IRQ();              
	
							// hook this interrupt
							if (!pEOC_IRQ->hook() || (pEOC_IRQ==NULL)) {
								dprintf("DATAACQ: IRQ NOT hooked. IRQ: %d",data_ac_irq);
								return CR_FAILURE;
							}       
							else {
								pEOC_IRQ->physicalMask(); // just in case
								dprintf("DATAACQ: IRQ succesfully hooked");
								dprintf("IRQ    : %d",data_ac_irq);
								dprintf("IOBASE : %3.3Xh",data_ac_base);                        
				
								return CR_SUCCESS;
							}       
						}
					
					default:
						{
							dprintf("DATAACQ: error");
							break;
						}
					}
					return cr ;
				}
				
				
			}
		case CONFIG_STOP:
		case CONFIG_REMOVE:
			{
				dprintf("DATAACQ: Stop using the current configuration");
				
				if (pEOC_IRQ->unhook()) {
					dprintf("DATAACQ: IRQ succesfully unhooked");
				    data_ac_irq=255;
					return CR_SUCCESS;
				}
				else {
					dprintf("DATAACQ: IRQ was NOT unhooked");
					return CR_FAILURE;
				}
			}
		default:
			{
				dprintf("DATAACQ: configfunction: %d",cf);
				return CR_DEFAULT;
			}
	}
}

//********************************************************************************/
//********************************************************************************/
// function:            responds to application requests
// called by:           indirectly by application.
// return value:        outputbuffer

DWORD DataAcqDevice::OnW32DeviceIoControl(PIOCTLPARAMS p)
{
	switch (p->dioc_IOCtlCode)
	{

	// by create file
	case DIOC_OPEN:
		dprintf("DATAACQ: dioc_openhandle");
		pdatabase = (PVOID) _HeapAllocate(buffersize,HEAPZEROINIT);
		pdatacursor = (PCHAR) pdatabase;
		dprintf("         - %d bytes buffer reserved.",buffersize);
		pEOC_IRQ->physicalUnmask(); 
		dprintf("         - IRQ %d unmasked",data_ac_irq);
		dprintf("         - Base Address: %3.3X",data_ac_base);
		return DEVIOCTL_NOERROR;

	// by close file
	case DIOC_CLOSEHANDLE:
		dprintf("DATAACQ: dioc_closehandle");
		pEOC_IRQ->physicalMask(); 
		dprintf("         - IRQ %d masked",data_ac_irq);
		HeapFree(pdatabase, 0) ;
		dprintf("         - %d bytes buffer freed.",buffersize);
		return DEVIOCTL_NOERROR;

/********************************************************************************/
	
	// give VxD-version
	case 1: {       // check outbut-buffer
				if(!p->dioc_OutBuf || p->dioc_cbOutBuf < 2) return ERROR_INVALID_PARAMETER;
				
				// version number in output-buffer
				*(WORD*)(p->dioc_OutBuf) = (WORD)((DATAACQ_Major << 8) + DATAACQ_Minor);

				// give size of output-buffer
				if (p->dioc_bytesret) *(PDWORD)(p->dioc_bytesret) = 2;

				dprintf("DATAACQ: gave version number: v%2.2X.%2.2X",(WORD)(DATAACQ_Major,DATAACQ_Minor));

				return 0;
			}

/********************************************************************************/

	// output one byte to ADC A
	case 2: {       // output byte in buffer
				_outp(data_ac_base+2,*(char*)p->dioc_InBuf);

				dprintf("DATAACQ: output byte %3.3u @ %4.4Xh (A)",*(UCHAR*)p->dioc_InBuf,data_ac_base+2);

				return 0;
			}

/********************************************************************************/

	// output one byte to ADC B
	case 3: {       // output byte in buffer
				_outp(data_ac_base+3,*(char*)p->dioc_InBuf);

				dprintf("DATAACQ: output byte %3.3u @ %4.4Xh (B)",*(UCHAR*)p->dioc_InBuf,data_ac_base+3);
				
				return 0;
			}

/********************************************************************************/

	// program timer (get application handler and message ID)
	case 4: {       UCHAR LSB, MSB;
				// expects this structure in the input-buffer, provided by application
				struct xsampling {      WORD dtime;                     // time between samples in µs
									WORD channel;           // sampling channel 0..7
									WORD MsgID;                     // unique message-id, provided by application
									HANDLE application;     // handle of application
								 } *sampling;

				// cast input-buffer to structure
				sampling = (xsampling *)p->dioc_InBuf;
		
				// new handle and message-id
				oscilloscoop = sampling->application;
				MsgID            = sampling->MsgID;
				LSB = (UCHAR)((sampling->dtime * 8) & 0x00FF);                  // write LSB } divide 8 MHz
				MSB = (UCHAR)(((sampling->dtime * 8) & 0xFF00)>>8);     // write MSB }  by this

				dprintf("DATAACQ: prog.timer; buffer: T=%4.4dµs, channel %d, MsgID=%4.4Xh, AppHandl=%d",sampling->dtime,sampling->channel,MsgID,oscilloscoop);
				dprintf("DATAACQ: MSB %2.2Xh, LSB %2.2Xh",MSB,LSB);
				dprintf("- config: IRQ: %d; BASE %3.3Xh",data_ac_irq,data_ac_base);
				
				// program the timer i82C54 on the data-acquisitioncard:
				
				// select ADC channel
				_outp(data_ac_base+0,(UCHAR)sampling->channel);
	
				// program sample-frequentie, time in microsecs (start sample)
				_outp(data_ac_base+7,(UCHAR)0x3C);      // 0011 1100: 00=counter0, 11=write LSB + MSB, X10=mode2, 0=bin (rate generator)
				_outp(data_ac_base+4,LSB);                      // write LSB } divide 8 MHz
				_outp(data_ac_base+4,MSB);                      // write MSB }  by this

				// program clock of the ADC: constant 8 MHz / 8 = 1 MHz         
				_outp(data_ac_base+7,(UCHAR)0x5C);      // 0101 1100: 01=counter1, 01=write LSB only, X11=mode3, 0=bin (squarewave generator)
				_outp(data_ac_base+5,(UCHAR)0x08);      // write LSB } divide by this
		
				return 0;
						
			}

/********************************************************************************/
	
	// change buffersize (old data lost!)
	case 5: {       dprintf("DATAACQ: changing buffersize...");
				// first mask the interrupt
				pEOC_IRQ->physicalMask(); 
				dprintf("         - IRQ %d masked",data_ac_irq);
				// reallocated
				buffersize = *(DWORD*)p->dioc_InBuf;
				pdatabase = HeapReAllocate(pdatabase, buffersize, HEAPZEROREINIT) ;
				pdatacursor = (PCHAR) pdatabase;
				dataindex=0;
				dprintf("         - %d bytes buffer reallocated.",buffersize);
				// unmask the interrupt
				pEOC_IRQ->physicalUnmask(); 
				dprintf("         - IRQ %d unmasked",data_ac_irq);
				// childs play!
				dprintf("DATAACQ: data-buffersize changed to %d bytes",*(DWORD*)p->dioc_InBuf);
			}

/********************************************************************************/


	// if service not available
	default: return ERROR_INVALID_PARAMETER;

/********************************************************************************/
	
	}
 
}

