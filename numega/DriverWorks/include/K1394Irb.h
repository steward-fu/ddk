// k1394Irb.h - Class KIrb<REQUEST_XXXX>
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
#ifndef _K1394_IRB_H_
#define _K1394_IRB_H_

///////////////////////////////////////////////////////////////////////////////
//Class KIrb<REQUEST_XXXX> provides wrappers for setting up IRBs, and
// for retrieving information from the IRB after it has been completed.

struct K1394Irb : public IRB
{
	K1394Irb() { RtlZeroMemory(this, sizeof(IRB)); }
	K1394Irb(ULONG fn) { RtlZeroMemory(this, sizeof(IRB)); FunctionNumber = fn;	}

	operator PIRB () const { return PIRB(this); }

	template<class MdlType> static ULONG MdlSize(MdlType Mdl)
	{
		return Mdl.Size();
	}

	template<> static ULONG MdlSize(PMDL Mdl)
	{
		return MmGetMdlByteCount(Mdl);
	}
};

template <const ULONG BusRequest> struct KIrb : public K1394Irb
{
	KIrb() : K1394Irb(BusRequest) {}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_ASYNC_READ
template <> struct KIrb<REQUEST_ASYNC_READ> : public K1394Irb
{
	KIrb<REQUEST_ASYNC_READ>() {}

	template<class MdlType>
	KIrb<REQUEST_ASYNC_READ>(
		ULONG			GenerationCount,
		USHORT			OffsetHigh,
		ULONG			OffsetLow,
		MdlType			Mdl,
		ULONG			BlockSize = 0,
		ULONG			Flags = 0)
	{
		Initialize(	GenerationCount,
					OffsetHigh,
					OffsetLow,
					Mdl,
					BlockSize,
					Flags);
	}

	template<class MdlType>
	VOID Initialize(
		ULONG			GenerationCount,
		USHORT			OffsetHigh,
		ULONG			OffsetLow,
		MdlType			Mdl,
		ULONG			BlockSize = 0,
		ULONG			Flags = 0)
	{
		FunctionNumber = REQUEST_ASYNC_READ;

		u.AsyncRead.ulGeneration = GenerationCount;
		u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_High
			= OffsetHigh;

		u.AsyncRead.DestinationAddress.IA_Destination_Offset.Off_Low
			= OffsetLow;

		SetupMdl(Mdl);

		u.AsyncRead.nBlockSize = BlockSize;
		u.AsyncRead.fulFlags = Flags;
	}

	template<class MdlType> VOID SetupMdl(MdlType Mdl)
	{
		u.AsyncRead.nNumberOfBytesToRead = MdlSize(Mdl);
		u.AsyncRead.Mdl = Mdl;
	}
};



///////////////////////////////////////////////////////////////////////////////
//REQUEST_BUS_RESET
template<> struct KIrb<REQUEST_BUS_RESET> : public K1394Irb
{
	KIrb<REQUEST_BUS_RESET>(BOOLEAN ForceRootNode = FALSE)
	{
		Initialize(ForceRootNode);
	}

	VOID Initialize(BOOLEAN ForceRootNode = FALSE)
	{
		FunctionNumber = REQUEST_BUS_RESET;
		u.BusReset.fulFlags = (ForceRootNode) ? BUS_RESET_FLAGS_FORCE_ROOT : 0;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_BUS_RESET_NOTIFICATION
template<> struct KIrb<REQUEST_BUS_RESET_NOTIFICATION> : public K1394Irb
{
	KIrb<REQUEST_BUS_RESET_NOTIFICATION>() {}

	KIrb<REQUEST_BUS_RESET_NOTIFICATION>(
		PBUS_BUS_RESET_NOTIFICATION		ResetRoutine,
		PVOID							ResetContext,
		BOOLEAN							Register = TRUE)
	{
		Initialize(ResetRoutine, ResetContext, Register);
	}

	VOID Initialize(
		PBUS_BUS_RESET_NOTIFICATION		ResetRoutine,
		PVOID							ResetContext,
		BOOLEAN							Register = TRUE)
	{
		FunctionNumber = REQUEST_BUS_RESET_NOTIFICATION;
		u.BusResetNotification.fulFlags = Register ?
			REGISTER_NOTIFICATION_ROUTINE : DEREGISTER_NOTIFICATION_ROUTINE;
		u.BusResetNotification.ResetRoutine = ResetRoutine;
		u.BusResetNotification.ResetContext = ResetContext;
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_CONTROL
// Sends a device control request to the bus driver
template<> struct KIrb<REQUEST_CONTROL> : public K1394Irb
{
	KIrb<REQUEST_CONTROL>() {}

	template<class MdlType>
	KIrb<REQUEST_CONTROL>(
		ULONG		ControlCode,
		MdlType		InputBuffer,
		MdlType		OutputBuffer)
	{
		Initialize(ControlCode, InputBuffer, OutputBuffer);
	}

	template<class MdlType>
	VOID Initialize(
		ULONG		ControlCode,
		MdlType		InputBuffer,
		MdlType		OutputBuffer)
	{
		FunctionNumber = REQUEST_CONTROL;
		u.Control.ulIoControlCode = ControlCode;
		u.Control.pInBuffer = InputBuffer;
		u.Control.ulInBufferLength = MdlSize(InputBuffer);
		u.Control.pOutBuffer = OutputBuffer;
		u.Control.ulOutBufferLength = MdlSize(OutputBuffer);
	}

	ULONG BytesReturned(){ return u.Control.BytesReturned; }
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_ASYNC_STREAM
// Sends an asynchronous packet to an isochronous listener
template<> struct KIrb<REQUEST_ASYNC_STREAM> : public K1394Irb
{
	KIrb<REQUEST_ASYNC_STREAM>() {}

	template<class MdlType>
	KIrb<REQUEST_ASYNC_STREAM>(
		MdlType		Mdl,
		ULONG		Channel,
		UCHAR		Speed,
		ULONG		Synch = 0,
		ULONG       Tag = 0)
	{
		Initialize(Mdl, Channel, Speed, Synch, Tag);
	}

	template<class MdlType>
	VOID Initialize(
		MdlType		Mdl,
		ULONG		Channel,
		UCHAR		Speed,
		ULONG		Synch = 0,
		ULONG		Tag = 0)
	{
		FunctionNumber = REQUEST_ASYNC_STREAM;

		u.AsyncStream.nChannel = Channel;
		u.AsyncStream.ulTag = Tag;
		u.AsyncStream.ulSynch = Synch;
		u.AsyncStream.nSpeed = Speed;

		SetupMdl(Mdl);
	}

	template <class MdlType> VOID SetupMdl(MdlType Mdl)
	{
		u.AsyncStream.Mdl = Mdl;
		u.AsyncStream.nNumberOfBytesToStream = MdlSize(Mdl);
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_ALLOCATE_RESOURCES
// Allocates a resource handle which may be used in setting up an
//  isochronous transfer
//
template<> struct KIrb<REQUEST_ISOCH_ALLOCATE_RESOURCES> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_ALLOCATE_RESOURCES>() {}

	KIrb<REQUEST_ISOCH_ALLOCATE_RESOURCES>(
		ULONG				Speed,
		ULONG				Flags,
		ULONG				Channel,
		ULONG				MaxBytesPerFrame,
		ULONG				NumberOfBuffers,
		ULONG				MaxBufferSize,
		ULONG				QuadletsToStrip)
	{
		Initialize( Speed,
				    Flags,
					Channel,
					MaxBytesPerFrame,
					NumberOfBuffers,
				    MaxBufferSize,
					QuadletsToStrip);
	}

	VOID Initialize(
		ULONG				Speed,
		ULONG				Flags,
		ULONG				Channel,
		ULONG				MaxBytesPerFrame,
		ULONG				NumberOfBuffers,
		ULONG				MaxBufferSize,
		ULONG				QuadletsToStrip)
	{
		FunctionNumber = REQUEST_ISOCH_ALLOCATE_RESOURCES;

		u.IsochAllocateResources.fulSpeed = Speed;
		u.IsochAllocateResources.fulFlags = Flags;
		u.IsochAllocateResources.nChannel = Channel;
		u.IsochAllocateResources.nMaxBytesPerFrame = MaxBytesPerFrame;
		u.IsochAllocateResources.nNumberOfBuffers = NumberOfBuffers;
		u.IsochAllocateResources.nMaxBufferSize = MaxBufferSize;
		u.IsochAllocateResources.nQuadletsToStrip = QuadletsToStrip;
	}

	HANDLE Handle() { return u.IsochAllocateResources.hResource; }
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_ALLOCATE_ADDRESS_RANGE
//
// Allocates an asynchronous address range with notification callbacks
//  to the driver whenever the device performs an operation on the address
//  range.
template<> struct KIrb<REQUEST_ALLOCATE_ADDRESS_RANGE> : public K1394Irb
{
	KIrb<REQUEST_ALLOCATE_ADDRESS_RANGE>() {}

	KIrb<REQUEST_ALLOCATE_ADDRESS_RANGE>(
		PVOID				Callback,
		PVOID				Context,
		ULONG				Length,
		ULONG				AccessFlags,
		ULONG				NotificationFlags,
		ADDRESS_OFFSET		*AddressOffset,
		PMDL				pMdl,
		ULONG				Flags,
		ULONG				MaxSegmentSize,
		PSLIST_HEADER		FifoListHead,
		PKSPIN_LOCK			FifoSpinLock,
		PADDRESS_RANGE		pAddressArray)
	{
		Initialize( Callback,
					Context,
					Length,
					AccessFlags,
					NotificationFlags,
					AddressOffset,
					pMdl,
					Flags,
					MaxSegmentSize,
					FifoListHead,
					FifoSpinLock,
					pAddressArray);
	}

	VOID Initialize(
		PVOID				Callback,
		PVOID				Context,
		ULONG				Length,
		ULONG				AccessFlags,
		ULONG				NotificationFlags,
		ADDRESS_OFFSET		*AddressOffset,
		PMDL				pMdl,
		ULONG				Flags,
		ULONG				MaxSegmentSize,
		PSLIST_HEADER		FifoListHead,
		PKSPIN_LOCK			FifoSpinLock,
		PADDRESS_RANGE		pAddressArray)
	{
		FunctionNumber = REQUEST_ALLOCATE_ADDRESS_RANGE;
		u.AllocateAddressRange.fulFlags = Flags;
		u.AllocateAddressRange.nLength = Length;
		u.AllocateAddressRange.MaxSegmentSize = MaxSegmentSize;
		u.AllocateAddressRange.fulAccessType = AccessFlags;
		u.AllocateAddressRange.fulNotificationOptions = NotificationFlags;

		if( AddressOffset )
		{
			u.AllocateAddressRange.Required1394Offset.Off_High =
				AddressOffset->Off_High;

			u.AllocateAddressRange.Required1394Offset.Off_Low =
				AddressOffset->Off_Low;
		}

		u.AllocateAddressRange.AddressesReturned = 0;
		u.AllocateAddressRange.p1394AddressRange = pAddressArray;
		u.AllocateAddressRange.hAddressRange = NULL;
		u.AllocateAddressRange.Mdl =  pMdl;

		u.AllocateAddressRange.Callback = Callback;
		u.AllocateAddressRange.Context = Context;

		u.AllocateAddressRange.FifoSListHead = FifoListHead;
		u.AllocateAddressRange.FifoSpinLock = FifoSpinLock;
	}

	HANDLE Handle(){ return u.AllocateAddressRange.hAddressRange; }
	ULONG AddressCount(){ return u.AllocateAddressRange.AddressesReturned; }
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_ASYNC_LOCK
//
// Performs an atomic operation on the 1394 address space.  Possible
//  operations are FetchAdd, MaskSwap, CompareSwap, BoundedAdd, and
//  WrapAdd
//
template<> struct KIrb<REQUEST_ASYNC_LOCK> : public K1394Irb
{
	KIrb<REQUEST_ASYNC_LOCK>() {}

	KIrb<REQUEST_ASYNC_LOCK>(
		ULONG		GenerationCount,
		USHORT		OffsetHigh,
		ULONG		OffsetLow,
		ULONG		TransactionType,
		ULONG		ArgumentHigh,
		ULONG		ArgumentLow,
		ULONG		ArgumentBytes,
		ULONG		DataHigh,
		ULONG		DataLow,
		ULONG		DataBytes,
		PVOID		pOriginalValue)
	{
		Initialize(	GenerationCount,
					OffsetHigh,
					OffsetLow,
					TransactionType,
					ArgumentHigh,
					ArgumentLow,
					ArgumentBytes,
					DataHigh,
					DataLow,
					DataBytes,
					pOriginalValue);
	}

	VOID Initialize(
		ULONG		GenerationCount,
		USHORT		OffsetHigh,
		ULONG		OffsetLow,
		ULONG		TransactionType,
		ULONG		ArgumentHigh,
		ULONG		ArgumentLow,
		ULONG		ArgumentBytes,
		ULONG		DataHigh,
		ULONG		DataLow,
		ULONG		DataBytes,
		PVOID		pOriginalValue)
	{
		FunctionNumber = REQUEST_ASYNC_LOCK;

		u.AsyncLock.ulGeneration = GenerationCount;
		u.AsyncLock.DestinationAddress.IA_Destination_Offset.Off_High
			= OffsetHigh;
		u.AsyncLock.DestinationAddress.IA_Destination_Offset.Off_Low
			= OffsetLow;
		u.AsyncLock.fulTransactionType = TransactionType;
		u.AsyncLock.nNumberOfArgBytes = ArgumentBytes;
		u.AsyncLock.nNumberOfDataBytes = DataBytes;

		if(ArgumentBytes != 0)
			u.AsyncLock.Arguments[0] = ArgumentHigh;
		u.AsyncLock.DataValues[0] = DataHigh;

		if(ArgumentBytes == 8)
			u.AsyncLock.Arguments[1] = ArgumentLow;
		if(DataBytes == 8)
			u.AsyncLock.DataValues[1] = DataLow;

		u.AsyncLock.pBuffer = pOriginalValue;
		u.AsyncLock.fulFlags = 0;
	}
};


///////////////////////////////////////////////////////////////////////////////
// REQUEST_ASYNC_WRITE
template<> struct KIrb<REQUEST_ASYNC_WRITE> : public K1394Irb
{
	KIrb<REQUEST_ASYNC_WRITE>() {}

	template<class MdlType>
	KIrb<REQUEST_ASYNC_WRITE>(
		ULONG			GenerationCount,
		USHORT			OffsetHigh,
		ULONG			OffsetLow,
		MdlType			Mdl,
		ULONG			BlockSize = 0,
		ULONG			Flags = 0)
	{
		Initialize( GenerationCount,
					OffsetHigh,
					OffsetLow,
					Mdl,
					BlockSize,
					Flags);
	}

	template<class MdlType>
	VOID Initialize(
		ULONG			GenerationCount,
		USHORT			OffsetHigh,
		ULONG			OffsetLow,
		MdlType			Mdl,
		ULONG			BlockSize = 0,
		ULONG			Flags = 0)
	{
		FunctionNumber = REQUEST_ASYNC_WRITE;

		u.AsyncWrite.ulGeneration = GenerationCount;
		u.AsyncWrite.DestinationAddress.IA_Destination_Offset.Off_High
			= OffsetHigh;

		u.AsyncWrite.DestinationAddress.IA_Destination_Offset.Off_Low
			= OffsetLow;

		SetupMdl(Mdl);

		u.AsyncWrite.nBlockSize = BlockSize;
		u.AsyncWrite.fulFlags = Flags;
	}

	template <class MdlType> VOID SetupMdl(MdlType Mdl)
	{
		u.AsyncWrite.nNumberOfBytesToWrite = MdlSize(Mdl);
		u.AsyncWrite.Mdl = Mdl;
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_GET_CONFIGURATION_INFO
//
// Returns parts of the config rom including the vendor name, model name,
//  and unit directories.
template<> struct KIrb<REQUEST_GET_CONFIGURATION_INFO> : public K1394Irb
{
	KIrb<REQUEST_GET_CONFIGURATION_INFO>() {}

	KIrb<REQUEST_GET_CONFIGURATION_INFO>(
		CONFIG_ROM			*pConfigRom,
		ULONG				UnitDirectoryBufferSize,
		PVOID				UnitDirectory,
		ULONG				UnitDependentDirectorySize,
		PVOID				UnitDependentDirectory,
		ULONG				VendorLeafBufferSize,
		PTEXTUAL_LEAF		VendorLeaf,
		ULONG				ModelLeafBufferSize,
		PTEXTUAL_LEAF		ModelLeaf)
	{
		Initialize(	pConfigRom,
					UnitDirectoryBufferSize,
					UnitDirectory,
					UnitDependentDirectorySize,
					UnitDependentDirectory,
					VendorLeafBufferSize,
					VendorLeaf,
					ModelLeafBufferSize,
					ModelLeaf);
	}

	VOID Initialize(
		CONFIG_ROM			*pConfigRom,
		ULONG				UnitDirectoryBufferSize,
		PVOID				UnitDirectory,
		ULONG				UnitDependentDirectorySize,
		PVOID				UnitDependentDirectory,
		ULONG				VendorLeafBufferSize,
		PTEXTUAL_LEAF		VendorLeaf,
		ULONG				ModelLeafBufferSize,
		PTEXTUAL_LEAF		ModelLeaf)
	{
		FunctionNumber = REQUEST_GET_CONFIGURATION_INFO;

		u.GetConfigurationInformation.ConfigRom = pConfigRom;

		u.GetConfigurationInformation.UnitDirectoryBufferSize =
			UnitDirectoryBufferSize;
		u.GetConfigurationInformation.UnitDirectory = UnitDirectory;
		u.GetConfigurationInformation.UnitDependentDirectoryBufferSize =
			UnitDependentDirectorySize;
		u.GetConfigurationInformation.UnitDependentDirectory =
			UnitDependentDirectory;
		u.GetConfigurationInformation.VendorLeafBufferSize =
			VendorLeafBufferSize;
		u.GetConfigurationInformation.VendorLeaf = VendorLeaf;
		u.GetConfigurationInformation.ModelLeafBufferSize =
			ModelLeafBufferSize;
		u.GetConfigurationInformation.ModelLeaf = ModelLeaf;
	}
};



///////////////////////////////////////////////////////////////////////////////
//REQUEST_GET_SPEED_BETWEEN_DEVICES
//
// Returns the maximum speed that may be used in communication between
//  the driver's device and any other devices on the bus that have
//  system DEVICE_OBJECTs.  (Note: there is a maximum of 63 devices,
//   the maximum that may be on the bus)
template<> struct KIrb<REQUEST_GET_SPEED_BETWEEN_DEVICES> : public K1394Irb
{
	KIrb<REQUEST_GET_SPEED_BETWEEN_DEVICES>() {}

	KIrb<REQUEST_GET_SPEED_BETWEEN_DEVICES>(
		ULONG			DeviceObjectCount,
		PDEVICE_OBJECT*	pDeviceObjects,
		ULONG			SourceDevice = 0)
	{
		Initialize(DeviceObjectCount, pDeviceObjects, SourceDevice);
	}

	VOID Initialize(
		ULONG			DeviceObjectCount,
		PDEVICE_OBJECT*	pDeviceObjects,
		ULONG			SourceDevice = 0)
	{
		FunctionNumber = REQUEST_GET_SPEED_BETWEEN_DEVICES;

		u.GetMaxSpeedBetweenDevices.fulFlags = SourceDevice;
		u.GetMaxSpeedBetweenDevices.ulNumberOfDestinations = DeviceObjectCount;
		for(ULONG i = 0; i < DeviceObjectCount; i++)
		{
			u.GetMaxSpeedBetweenDevices.hDestinationDeviceObjects[i] =
				pDeviceObjects[i];
		}
	}

	ULONG Speed() { return u.GetMaxSpeedBetweenDevices.fulSpeed; }
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_ALLOCATE_BANDWIDTH
//
//Reserves bandwidth for the device on the bus.
//
template<> struct KIrb<REQUEST_ISOCH_ALLOCATE_BANDWIDTH> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_ALLOCATE_BANDWIDTH>() {}

	KIrb<REQUEST_ISOCH_ALLOCATE_BANDWIDTH>(
				   ULONG Speed,
				   ULONG MaxBytesPerFrame)
	{
		Initialize(Speed, MaxBytesPerFrame);
	}

	VOID Initialize(
				   ULONG Speed,
				   ULONG MaxBytesPerFrame)
	{
		FunctionNumber = REQUEST_ISOCH_ALLOCATE_BANDWIDTH;
		u.IsochAllocateBandwidth.nMaxBytesPerFrameRequested = MaxBytesPerFrame;
		u.IsochAllocateBandwidth.fulSpeed = Speed;
	}

	HANDLE Handle() { return u.IsochAllocateBandwidth.hBandwidth; }

	ULONG Speed() { return u.IsochAllocateBandwidth.SpeedSelected; }

	ULONG AvailableBandwidth()
	{
		return u.IsochAllocateBandwidth.BytesPerFrameAvailable;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_ALLOCATE_CHANNEL
//
// Reserves a channel for the device on the bus
//
template<> struct KIrb<REQUEST_ISOCH_ALLOCATE_CHANNEL> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_ALLOCATE_CHANNEL>() {}

	KIrb<REQUEST_ISOCH_ALLOCATE_CHANNEL>(ULONG ChannelNumber)
	{
		Initialize(ChannelNumber);
	}

	VOID Initialize(ULONG ChannelNumber)
	{
		FunctionNumber = REQUEST_ISOCH_ALLOCATE_CHANNEL;
		u.IsochAllocateChannel.nRequestedChannel = ChannelNumber;
	}

	ULONG ChannelNumber() { return u.IsochAllocateChannel.Channel; }

	LARGE_INTEGER& AvailableChannels()
	{
		return u.IsochAllocateChannel.ChannelsAvailable;
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_ATTACH_BUFFERS
// Attach buffers to a resource handle to be transmitted over the bus,
//  or to receive data from the bus.
//
template<> struct KIrb<REQUEST_ISOCH_ATTACH_BUFFERS> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_ATTACH_BUFFERS>() {}

	KIrb<REQUEST_ISOCH_ATTACH_BUFFERS>(
		HANDLE						ResourceHandle,
		ULONG						DescriptorCount,
		PISOCH_DESCRIPTOR			pIsochDescriptor)
	{
		Initialize(ResourceHandle, DescriptorCount, pIsochDescriptor);
	}

	VOID Initialize(
		HANDLE						ResourceHandle,
		ULONG						DescriptorCount,
		PISOCH_DESCRIPTOR			pIsochDescriptor)
	{
		FunctionNumber = REQUEST_ISOCH_ATTACH_BUFFERS;
		u.IsochAttachBuffers.hResource = ResourceHandle;
		u.IsochAttachBuffers.nNumberOfDescriptors = DescriptorCount;
		u.IsochAttachBuffers.pIsochDescriptor = pIsochDescriptor;
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_DETACH_BUFFERS
//
// Remove attached buffers from the resource handle.  Either removes
//  completed buffers, or acts to cancel the operation on buffers in
//  progress before removing them.
//
template<> struct KIrb<REQUEST_ISOCH_DETACH_BUFFERS> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_DETACH_BUFFERS>() {}

	KIrb<REQUEST_ISOCH_DETACH_BUFFERS>(
		HANDLE						ResourceHandle,
		ULONG						DescriptorCount,
		PISOCH_DESCRIPTOR			pIsochDescriptor)
	{
		Initialize(ResourceHandle, DescriptorCount, pIsochDescriptor);
	}

	VOID Initialize(
		HANDLE						ResourceHandle,
		ULONG						DescriptorCount,
		PISOCH_DESCRIPTOR			pIsochDescriptor)
	{
		FunctionNumber = REQUEST_ISOCH_DETACH_BUFFERS;
		u.IsochDetachBuffers.hResource = ResourceHandle;
		u.IsochDetachBuffers.nNumberOfDescriptors = DescriptorCount;
		u.IsochDetachBuffers.pIsochDescriptor = pIsochDescriptor;
	}
};

///////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_FREE_BANDWIDTH
template<> struct KIrb<REQUEST_ISOCH_FREE_BANDWIDTH> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_FREE_BANDWIDTH>() {}

	KIrb<REQUEST_ISOCH_FREE_BANDWIDTH>(HANDLE Bandwidth)
	{
		Initialize(Bandwidth);
	}

	VOID Initialize(HANDLE Bandwidth)
	{
		FunctionNumber = REQUEST_ISOCH_FREE_BANDWIDTH;
		u.IsochFreeBandwidth.hBandwidth = Bandwidth;
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_FREE_CHANNEL
template<> struct KIrb<REQUEST_ISOCH_FREE_CHANNEL> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_FREE_CHANNEL>() {}

	KIrb<REQUEST_ISOCH_FREE_CHANNEL>(ULONG ChannelNumber)
	{
		Initialize(ChannelNumber);
	}

	VOID Initialize(ULONG ChannelNumber)
	{
		FunctionNumber = REQUEST_ISOCH_FREE_CHANNEL;
		u.IsochFreeChannel.nChannel = ChannelNumber;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_FREE_RESOURCES
template<> struct KIrb<REQUEST_ISOCH_FREE_RESOURCES> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_FREE_RESOURCES>() {}

	KIrb<REQUEST_ISOCH_FREE_RESOURCES>(HANDLE Resource)
	{
		Initialize(Resource);
	}

	VOID Initialize(HANDLE Resource)
	{
		FunctionNumber = REQUEST_ISOCH_FREE_RESOURCES;
		u.IsochFreeResources.hResource = Resource;
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_LISTEN
template<> struct KIrb<REQUEST_ISOCH_LISTEN> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_LISTEN>() {}

	KIrb<REQUEST_ISOCH_LISTEN>(
		HANDLE Resource,
		CYCLE_TIME *StartTime = NULL)
	{
		Initialize(Resource, StartTime);
	}

	VOID Initialize(
		HANDLE Resource,
		CYCLE_TIME* StartTime = NULL)
	{
		FunctionNumber = REQUEST_ISOCH_LISTEN;
		u.IsochListen.hResource = Resource;
		if( StartTime )
			u.IsochListen.StartTime = *StartTime;
		u.IsochListen.fulFlags = 0;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_TALK
template<> struct KIrb<REQUEST_ISOCH_TALK> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_TALK>() {}

	KIrb<REQUEST_ISOCH_TALK>(
		HANDLE Resource,
		CYCLE_TIME *StartTime = NULL)
	{
		Initialize(Resource, StartTime);
	}

	VOID Initialize(
		HANDLE Resource,
		CYCLE_TIME* StartTime = NULL)
	{
		FunctionNumber = REQUEST_ISOCH_TALK;
		u.IsochTalk.hResource = Resource;
		if( StartTime )
			u.IsochTalk.StartTime = *StartTime;
		u.IsochTalk.fulFlags = 0;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_STOP
template<> struct KIrb<REQUEST_ISOCH_STOP> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_STOP>() {}

	KIrb<REQUEST_ISOCH_STOP>(HANDLE Resource)
	{
		Initialize(Resource);
	}

	VOID Initialize(HANDLE Resource)
	{
		FunctionNumber = REQUEST_ISOCH_STOP;
		u.IsochStop.hResource = Resource;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_SET_CHANNEL_BANDWIDTH
//
// Changes the bytes per frame in an already allocated bandwidth handle.
//
template<> struct KIrb<REQUEST_ISOCH_SET_CHANNEL_BANDWIDTH> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_SET_CHANNEL_BANDWIDTH>() {}

	KIrb<REQUEST_ISOCH_SET_CHANNEL_BANDWIDTH>(
		HANDLE Bandwidth,
		ULONG MaxBytesPerFrame)
	{
		Initialize(Bandwidth, MaxBytesPerFrame);
	}

	VOID Initialize(
		HANDLE Bandwidth,
		ULONG MaxBytesPerFrame)
	{
		FunctionNumber = REQUEST_ISOCH_SET_CHANNEL_BANDWIDTH;
		u.IsochSetChannelBandwidth.hBandwidth = Bandwidth;
		u.IsochSetChannelBandwidth.nMaxBytesPerFrame = MaxBytesPerFrame;
	}
};

////////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_QUERY_CYCLE_TIME
template<> struct KIrb<REQUEST_ISOCH_QUERY_CYCLE_TIME> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_QUERY_CYCLE_TIME>()
	{
		Initialize();
	}

	VOID Initialize()
	{
		FunctionNumber = REQUEST_ISOCH_QUERY_CYCLE_TIME;
	}

	CYCLE_TIME& CycleTime()
	{
		return u.IsochQueryCurrentCycleTime.CycleTime;
	}
};


///////////////////////////////////////////////////////////////////////
//REQUEST_ISOCH_QUERY_RESOURCES
//
// Retreive the available channels and bandwidth
//
template<> struct KIrb<REQUEST_ISOCH_QUERY_RESOURCES> : public K1394Irb
{
	KIrb<REQUEST_ISOCH_QUERY_RESOURCES>() {}

	KIrb<REQUEST_ISOCH_QUERY_RESOURCES>(ULONG Speed)
	{
		Initialize(Speed);
	}

	VOID Initialize(ULONG Speed)
	{
		FunctionNumber = REQUEST_ISOCH_QUERY_RESOURCES;
		u.IsochQueryResources.fulSpeed = Speed;
	}

	LARGE_INTEGER& AvailableChannels()
	{
		return u.IsochQueryResources.ChannelsAvailable;
	}

	ULONG AvailableBandwidth()
	{
		return u.IsochQueryResources.BytesPerFrameAvailable;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_FREE_ADDRESS_RANGE
template<> struct KIrb<REQUEST_FREE_ADDRESS_RANGE> : public K1394Irb
{
	KIrb<REQUEST_FREE_ADDRESS_RANGE>() {}

	KIrb<REQUEST_FREE_ADDRESS_RANGE>(
		HANDLE*			pAddressRange,
		ULONG			AddressCount,
		PADDRESS_RANGE	pAddressArray)
	{
		Initialize(pAddressRange, AddressCount, pAddressArray);
	}

	VOID Initialize(
		HANDLE*			pAddressRange,
		ULONG			AddressCount,
		PADDRESS_RANGE	pAddressArray)
	{
		FunctionNumber = REQUEST_FREE_ADDRESS_RANGE;
		u.FreeAddressRange.nAddressesToFree = AddressCount;
		u.FreeAddressRange.p1394AddressRange = pAddressArray;
		u.FreeAddressRange.pAddressRange = pAddressRange;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_GET_ADDR_FROM_DEVICE_OBJECT
//
// Retreive the device's node number, and the 1394 bus number
//   If Device is USE_LOCAL_NODE, the host controller's node address
//    is received.
//
template<> struct KIrb<REQUEST_GET_ADDR_FROM_DEVICE_OBJECT> : public K1394Irb
{
	KIrb<REQUEST_GET_ADDR_FROM_DEVICE_OBJECT>(ULONG Device = 0)
	{
		Initialize(Device);
	}

	VOID Initialize(ULONG Device = 0)
	{
		FunctionNumber = REQUEST_GET_ADDR_FROM_DEVICE_OBJECT;
		u.Get1394AddressFromDeviceObject.fulFlags = Device;
	}

	USHORT NodeNumber()
	{
		return u.Get1394AddressFromDeviceObject.NodeAddress.NA_Node_Number;
	}

	ULONG BusNumber()
	{
		return u.Get1394AddressFromDeviceObject.NodeAddress.NA_Bus_Number;
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_GET_GENERATION_COUNT
//
// Retreive the generation count necessary for asynchronous operations
//  It is only valid until a bus reset occurs.
//
template<> struct KIrb<REQUEST_GET_GENERATION_COUNT> : public K1394Irb
{
	KIrb<REQUEST_GET_GENERATION_COUNT>() :
		K1394Irb(REQUEST_GET_GENERATION_COUNT) {}

	VOID Initialize() {	FunctionNumber = REQUEST_GET_GENERATION_COUNT; }

	ULONG GenerationCount() { return u.GetGenerationCount.GenerationCount; }
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_GET_LOCAL_HOST_INFO
//
// Used to retreive a variety of information about the device
//
template<> struct KIrb<REQUEST_GET_LOCAL_HOST_INFO> : public K1394Irb
{
	KIrb<REQUEST_GET_LOCAL_HOST_INFO>() {}

	KIrb<REQUEST_GET_LOCAL_HOST_INFO>(
		ULONG Level,
		PVOID Information)
	{
		Initialize(Level, Information);
	}

	VOID Initialize(
		ULONG Level,
		PVOID Information)
	{
		FunctionNumber = REQUEST_GET_LOCAL_HOST_INFO;
		u.GetLocalHostInformation.nLevel = Level;
		u.GetLocalHostInformation.Information = Information;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_GET_SPEED_TOPOLOGY_MAPS
//
// The speed map is an array containing the speed between any two devices
//  on the bus
//
// The topology map is contains an array of the self-ID packets each
//  node sent at the last bus reset.
//
template<> struct KIrb<REQUEST_GET_SPEED_TOPOLOGY_MAPS> : public K1394Irb
{
	KIrb<REQUEST_GET_SPEED_TOPOLOGY_MAPS>() :
		K1394Irb(REQUEST_GET_SPEED_TOPOLOGY_MAPS) {}

	VOID Initialize()
	{
		FunctionNumber = REQUEST_GET_SPEED_TOPOLOGY_MAPS;
	}

	PSPEED_MAP SpeedMap() { return u.GetSpeedTopologyMaps.SpeedMap; }
	PTOPOLOGY_MAP TopologyMap() { return u.GetSpeedTopologyMaps.TopologyMap; }
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_SEND_PHY_CONFIG_PACKET
//
// Sends a raw phy config packet to the bus driver.
//
// A phy config packet can either cause a bus reset, or set the gap count
//
// Both of these services are available in other IRBs.  The bus reset can
// be caused by REQUEST_BUS_REST, and the gap count may be set with
// REQUEST_SET_LOCAL_HOST_PROPERTIES
//
template<> struct KIrb<REQUEST_SEND_PHY_CONFIG_PACKET> : public K1394Irb
{
	KIrb<REQUEST_SEND_PHY_CONFIG_PACKET>() {}

	KIrb<REQUEST_SEND_PHY_CONFIG_PACKET>(PHY_CONFIGURATION_PACKET Packet)
	{
		Initialize(Packet);
	}

	VOID Initialize(PHY_CONFIGURATION_PACKET Packet)
	{
		FunctionNumber = REQUEST_SEND_PHY_CONFIG_PACKET;
		u.SendPhyConfigurationPacket.PhyConfigurationPacket = Packet;
	}
};


///////////////////////////////////////////////////////////////////////////////
//REQUEST_SET_DEVICE_XMIT_PROPERTIES
//
// Sets the maximum speed at which the device will transmit.  The default
//  maximum speed is the physical maximum for the device.  This function
//  may only lower that maximum.
//
template<> struct KIrb<REQUEST_SET_DEVICE_XMIT_PROPERTIES> : public K1394Irb
{
	KIrb<REQUEST_SET_DEVICE_XMIT_PROPERTIES>() {}

	KIrb<REQUEST_SET_DEVICE_XMIT_PROPERTIES>(ULONG Speed)
	{
		Initialize(Speed);
	}

	VOID Initialize(ULONG Speed)
	{
		FunctionNumber = REQUEST_SET_DEVICE_XMIT_PROPERTIES;
		u.SetDeviceXmitProperties.fulSpeed = Speed;
	}
};

///////////////////////////////////////////////////////////////////////////////
//REQUEST_SET_LOCAL_HOST_PROPERTIES
//
// Sets the gap count.  (The idle time on the bus between packets)
//
template<> struct KIrb<REQUEST_SET_LOCAL_HOST_PROPERTIES> : public K1394Irb
{
	KIrb<REQUEST_SET_LOCAL_HOST_PROPERTIES>() {}

	KIrb<REQUEST_SET_LOCAL_HOST_PROPERTIES>(
		ULONG Level,
		PVOID Information)
	{
		Initialize(Level, Information);
	}

	VOID Initialize(
		ULONG Level,
		PVOID Information)
	{
		FunctionNumber = REQUEST_SET_LOCAL_HOST_PROPERTIES;
		u.SetLocalHostProperties.nLevel = Level;
		u.SetLocalHostProperties.Information = Information;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Async lock

template <ULONG Operation> class KLockIrb32: public KIrb<REQUEST_ASYNC_LOCK>
{
public:
	VOID Initialize(
		ULONG	GenerationCount,
		USHORT OffsetHigh,
		ULONG  OffsetLow,
		PVOID  pOriginalValue,
		ULONG  Data,
		ULONG  Argument = 0); //ignored for fetch add
};

template<ULONG Operation>
inline VOID KLockIrb32<Operation>::
Initialize(
   		ULONG	GenerationCount,
		USHORT OffsetHigh,
		ULONG  OffsetLow,
		PVOID  pOriginalValue,
		ULONG  Data,
		ULONG  Argument)
{
	KIrb<REQUEST_ASYNC_LOCK>::Initialize(
		GenerationCount,
		OffsetHigh,OffsetLow,
		Operation,
		Argument,0,4,
		Data,0,4,
		pOriginalValue);
}

inline VOID KLockIrb32<LOCK_TRANSACTION_FETCH_ADD>::
Initialize(
   		ULONG	GenerationCount,
		USHORT OffsetHigh,
		ULONG  OffsetLow,
		PVOID  pOriginalValue,
		ULONG  Data,
		ULONG  Argument)
{
	KIrb<REQUEST_ASYNC_LOCK>::Initialize(
		GenerationCount,
		OffsetHigh,
		OffsetLow,
		LOCK_TRANSACTION_FETCH_ADD,
		0,0,0,
		Data,0,4,
		pOriginalValue);
	UNREFERENCED_PARAMETER(Argument);
}

inline VOID KLockIrb32<LOCK_TRANSACTION_LITTLE_ADD>::
Initialize(
   		ULONG	GenerationCount,
		USHORT OffsetHigh,
		ULONG  OffsetLow,
		PVOID  pOriginalValue,
		ULONG  Data,
		ULONG  Argument)
{
	KIrb<REQUEST_ASYNC_LOCK>::Initialize(
		GenerationCount,
		OffsetHigh,
		OffsetLow,
		LOCK_TRANSACTION_LITTLE_ADD,
		0,0,0,
		Data,0,4,
		pOriginalValue);
	UNREFERENCED_PARAMETER(Argument);
}

template<ULONG Operation> class KLockIrb64 : public KIrb<REQUEST_ASYNC_LOCK>
{
public:
	VOID Initialize(
		ULONG	GenerationCount,
		USHORT OffsetHigh,
		ULONG  OffsetLow,
		PVOID  pOriginalValue,
		ULONG  DataHigh,
		ULONG  DataLow,
		ULONG  ArgumentHigh = 0, // These last parameters are ignored
		ULONG  ArgumentLow = 0   // for either fetch add operation
		);
};


template<ULONG Operation>
inline VOID KLockIrb64<Operation>::
Initialize(
   		ULONG	GenerationCount,
		USHORT OffsetHigh,
		ULONG  OffsetLow,
		PVOID  pOriginalValue,
		ULONG  DataHigh,
		ULONG  DataLow,
		ULONG  ArgumentHigh,
		ULONG  ArgumentLow)
{
	KIrb<REQUEST_ASYNC_LOCK>::Initialize(
		GenerationCount,
		OffsetHigh, OffsetLow,      //Address
		Operation,
		ArgumentLow,ArgumentHigh,8,	//Argument
		DataLow,DataHigh,8,			//Data
		pOriginalValue);
}


inline VOID KLockIrb64<LOCK_TRANSACTION_FETCH_ADD>::
Initialize(
   		ULONG	GenerationCount,
		USHORT OffsetHigh,
		ULONG  OffsetLow,
		PVOID  pOriginalValue,
		ULONG  DataHigh,
		ULONG  DataLow,
		ULONG  ArgumentHigh,
		ULONG  ArgumentLow)
{
	KIrb<REQUEST_ASYNC_LOCK>::Initialize(
		GenerationCount,
		OffsetHigh, OffsetLow,      //Address
		LOCK_TRANSACTION_FETCH_ADD,
		0,0,0,						//Argument
		DataLow,DataHigh,8,			//Data
		pOriginalValue);
	UNREFERENCED_PARAMETER(ArgumentHigh);
	UNREFERENCED_PARAMETER(ArgumentLow);
}

inline VOID KLockIrb64<LOCK_TRANSACTION_LITTLE_ADD>::
Initialize(
   		ULONG	GenerationCount,
		USHORT OffsetHigh,
		ULONG  OffsetLow,
		PVOID  pOriginalValue,
		ULONG  DataHigh,
		ULONG  DataLow,
		ULONG  ArgumentHigh,
		ULONG  ArgumentLow)
{
	KIrb<REQUEST_ASYNC_LOCK>::Initialize(
		GenerationCount,
		OffsetHigh, OffsetLow,      //Address
		LOCK_TRANSACTION_LITTLE_ADD,
		0,0,0,						//Argument
		DataLow,DataHigh,8,			//Data
		pOriginalValue);
	UNREFERENCED_PARAMETER(ArgumentHigh);
	UNREFERENCED_PARAMETER(ArgumentLow);
}

///////////////////////////////////////////////////////////////////////////////
// It is quite common to allocate Irb submit it asynchronously (from DISPATCH_LEVEL)
// and retrieve the result in the completion routine. In this case the completion
// routine needs to have pointers to both Irb structure and user context. To avoid
// the problem with creating too many different context structures it is useful to
// define the KIrbWithContext structure.
template <const ULONG BusRequest, class T> struct KIrbWithContext : public KIrb<BusRequest>
{
	T	m_Context;
};


#endif
