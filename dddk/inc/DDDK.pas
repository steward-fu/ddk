//
// The DDDK stands for Delphi Driver Development Kit and is obtained from 
// http://www.delphibasics.info/home/delphibasicsprojects.
// All DDDK drivers should include this unit that exports
// all currently supported kernel function, structures and constants.
// This DDDK is used for tutorial purpose on Steward website only,
// not recommend for other purpose beacuse it might have the unexpected result.
//
// Last updated by Steward Fu <steward.fu@gmail.com>
// 01/30/2025
//

unit DDDK;

interface

const
NtKernel = 'ntoskrnl.exe';

STATUS_SUCCESS = $00000000;
STATUS_UNSUCCESSFUL = $C0000001;
STATUS_NOT_IMPLEMENTED = $C0000002;
STATUS_INVALID_INFO_CLASS = $C0000003;
STATUS_INFO_LENGTH_MISMATCH = $C0000004;
STATUS_ACCESS_VIOLATION = $C0000005;
STATUS_IN_PAGE_ERROR = $C0000006;
STATUS_PAGEFILE_QUOTA = $C0000007;
STATUS_INVALID_HANDLE = $C0000008;
STATUS_BAD_INITIAL_STACK = $C0000009;
STATUS_BAD_INITIAL_PC = $C000000A;
STATUS_INVALID_CID = $C000000B;
STATUS_TIMER_NOT_CANCELED = $C000000C;
STATUS_INVALID_PARAMETER = $C000000D;
STATUS_NO_SUCH_DEVICE = $C000000E;
STATUS_NO_SUCH_FILE = $C000000F;
STATUS_INVALID_DEVICE_REQUEST = $C0000010;

IRP_MJ_CREATE = $00;
IRP_MJ_CREATE_NAMED_PIPE = $01;
IRP_MJ_CLOSE = $02;
IRP_MJ_READ = $03;
IRP_MJ_WRITE = $04;
IRP_MJ_QUERY_INFORMATION = $05;
IRP_MJ_SET_INFORMATION = $06;
IRP_MJ_QUERY_EA = $07;
IRP_MJ_SET_EA = $08;
IRP_MJ_FLUSH_BUFFERS = $09;
IRP_MJ_QUERY_VOLUME_INFORMATION = $0A;
IRP_MJ_SET_VOLUME_INFORMATION = $0B;
IRP_MJ_DIRECTORY_CONTROL = $0C;
IRP_MJ_FILE_SYSTEM_CONTROL = $0D;
IRP_MJ_DEVICE_CONTROL = $0E;
IRP_MJ_INTERNAL_DEVICE_CONTROL = $0F;
IRP_MJ_SHUTDOWN = $10;
IRP_MJ_LOCK_CONTROL = $11;
IRP_MJ_CLEANUP = $12;
IRP_MJ_CREATE_MAILSLOT = $13;
IRP_MJ_QUERY_SECURITY = $14;
IRP_MJ_SET_SECURITY = $15;
IRP_MJ_POWER = $16;
IRP_MJ_SYSTEM_CONTROL = $17;
IRP_MJ_DEVICE_CHANGE = $18;
IRP_MJ_QUERY_QUOTA = $19;
IRP_MJ_SET_QUOTA = $1A;
IRP_MJ_PNP = $1B;
IRP_MJ_PNP_POWER = IRP_MJ_PNP;
IRP_MJ_MAXIMUM_FUNCTION = $1B;
IRP_MN_REMOVE_DEVICE = $02;

DO_BUFFERED_IO = $00000004;
DO_EXCLUSIVE = $00000008;
DO_DIRECT_IO = $00000010;
DO_MAP_IO_BUFFER = $00000020;
DO_DEVICE_INITIALIZING = $00000080;
DO_SHUTDOWN_REGISTERED = $00000800;
DO_BUS_ENUMERATED_DEVICE = $00001000;
DO_POWER_PAGABLE = $00002000;
DO_POWER_INRUSH = $00004000;

FILE_DEVICE_BEEP = $00000001;
FILE_DEVICE_CD_ROM = $00000002;
FILE_DEVICE_CD_ROM_FILE_SYSTEM = $00000003;
FILE_DEVICE_CONTROLLER = $00000004;
FILE_DEVICE_DATALINK = $00000005;
FILE_DEVICE_DFS = $00000006;
FILE_DEVICE_DISK = $00000007;
FILE_DEVICE_DISK_FILE_SYSTEM = $00000008;
FILE_DEVICE_FILE_SYSTEM = $00000009;
FILE_DEVICE_INPORT_PORT = $0000000A;
FILE_DEVICE_KEYBOARD = $0000000B;
FILE_DEVICE_MAILSLOT = $0000000C;
FILE_DEVICE_MIDI_IN = $0000000D;
FILE_DEVICE_MIDI_OUT = $0000000E;
FILE_DEVICE_MOUSE = $0000000F;
FILE_DEVICE_MULTI_UNC_PROVIDER = $00000010;
FILE_DEVICE_NAMED_PIPE = $00000011;
FILE_DEVICE_NETWORK = $00000012;
FILE_DEVICE_NETWORK_BROWSER = $00000013;
FILE_DEVICE_NETWORK_FILE_SYSTEM = $00000014;
FILE_DEVICE_NULL = $00000015;
FILE_DEVICE_PARALLEL_PORT = $00000016;
FILE_DEVICE_PHYSICAL_NETCARD = $00000017;
FILE_DEVICE_PRINTER = $00000018;
FILE_DEVICE_SCANNER = $00000019;
FILE_DEVICE_SERIAL_MOUSE_PORT = $0000001A;
FILE_DEVICE_SERIAL_PORT = $0000001B;
FILE_DEVICE_SCREEN = $0000001C;
FILE_DEVICE_SOUND = $0000001D;
FILE_DEVICE_STREAMS = $0000001E;
FILE_DEVICE_TAPE = $0000001F;
FILE_DEVICE_TAPE_FILE_SYSTEM = $00000020;
FILE_DEVICE_TRANSPORT = $00000021;
FILE_DEVICE_UNKNOWN = $00000022;
FILE_DEVICE_VIDEO = $00000023;
FILE_DEVICE_VIRTUAL_DISK = $00000024;
FILE_DEVICE_WAVE_IN = $00000025;
FILE_DEVICE_WAVE_OUT = $00000026;
FILE_DEVICE_8042_PORT = $00000027;
FILE_DEVICE_NETWORK_REDIRECTOR = $00000028;
FILE_DEVICE_BATTERY = $00000029;
FILE_DEVICE_BUS_EXTENDER = $0000002A;
FILE_DEVICE_MODEM = $0000002B;
FILE_DEVICE_VDM = $0000002C;
FILE_DEVICE_MASS_STORAGE = $0000002D;
FILE_DEVICE_SMB = $0000002E;
FILE_DEVICE_KS = $0000002F;
FILE_DEVICE_CHANGER = $00000030;
FILE_DEVICE_SMARTCARD = $00000031;
FILE_DEVICE_ACPI = $00000032;
FILE_DEVICE_DVD = $00000033;
FILE_DEVICE_FULLSCREEN_VIDEO = $00000034;
FILE_DEVICE_DFS_FILE_SYSTEM = $00000035;
FILE_DEVICE_DFS_VOLUME = $00000036;
FILE_DEVICE_SERENUM = $00000037;
FILE_DEVICE_TERMSRV = $00000038;
FILE_DEVICE_KSEC = $00000039;
FILE_DEVICE_FIPS = $0000003A;

EVENT_INCREMENT = 1;
IO_NO_INCREMENT = 0;
IO_CD_ROM_INCREMENT = 1;
IO_DISK_INCREMENT = 1;
IO_KEYBOARD_INCREMENT = 6;
IO_MAILSLOT_INCREMENT = 2;
IO_MOUSE_INCREMENT = 6;
IO_NAMED_PIPE_INCREMENT = 2;
IO_NETWORK_INCREMENT = 2;
IO_PARALLEL_INCREMENT = 1;
IO_SERIAL_INCREMENT = 2;
IO_SOUND_INCREMENT = 8;
IO_VIDEO_INCREMENT = 1;
SEMAPHORE_INCREMENT = 1;

MAXIMUM_FILENAME_LENGTH = 256;

FILE_REMOVABLE_MEDIA = $00000001;
FILE_READ_ONLY_DEVICE = $00000002;
FILE_FLOPPY_DISKETTE = $00000004;
FILE_WRITE_ONCE_MEDIA = $00000008;
FILE_REMOTE_DEVICE = $00000010;
FILE_DEVICE_IS_MOUNTED = $00000020;
FILE_VIRTUAL_VOLUME = $00000040;
FILE_AUTOGENERATED_DEVICE_NAME = $00000080;
FILE_DEVICE_SECURE_OPEN = $00000100;
FILE_CHARACTERISTIC_PNP_DEVICE = $00000800;

FileBasicInformation = 4;
FileStandardInformation = 5;
FilePositionInformation = 14;
FileEndOfFileInformation = 20;

FileFsVolumeInformation = 1;
FileFsLabelInformation = 2;
FileFsSizeInformation = 3;
FileFsDeviceInformation = 4;
FileFsAttributeInformation = 5;
FileFsControlInformation = 6;
FileFsFullSizeInformation = 7;
FileFsObjectIdInformation = 8;
FileFsDriverPathInformation = 9;
FileFsMaximumInformation = 10;

BusRelations = 0;
EjectionRelations = 1;
PowerRelations = 2;
RemovalRelations = 3;
TargetDeviceRelation = 4;
SingleBusRelations = 5;

BusQueryDeviceID = 0; // <Enumerator>\<Enumerator-specific device id>
BusQueryHardwareIDs = 1; // Hardware ids
BusQueryCompatibleIDs = 2; // compatible device ids
BusQueryInstanceID = 3; // persistent id for this instance of the device
BusQueryDeviceSerialNumber = 4; // serial number for this device

DeviceTextDescription = 0; // DeviceDesc property
DeviceTextLocationInformation = 1; // DeviceLocation property

DeviceUsageTypeUndefined = 0;
DeviceUsageTypePaging = 1;
DeviceUsageTypeHibernation = 2;
DeviceUsageTypeDumpFile = 3;

PowerSystemUnspecified = 0;
PowerSystemWorking = 1;
PowerSystemSleeping1 = 2;
PowerSystemSleeping2 = 3;
PowerSystemSleeping3 = 4;
PowerSystemHibernate = 5;
PowerSystemShutdown = 6;
PowerSystemMaximum = 7;

PowerActionNone = 0;
PowerActionReserved = 1;
PowerActionSleep = 2;
PowerActionHibernate = 3;
PowerActionShutdown = 4;
PowerActionShutdownReset = 5;
PowerActionShutdownOff = 6;
PowerActionWarmEject = 7;

PowerDeviceUnspecified = 0;
PowerDeviceD0 = 1;
PowerDeviceD1 = 2;
PowerDeviceD2 = 3;
PowerDeviceD3 = 4;
PowerDeviceMaximum = 5;

SystemPowerState = 0;
DevicePowerState = 1;

Executive = 0;
FreePage = 1; 
PageIn = 2; 
PoolAllocation = 3; 
DelayExecution = 4; 
Suspended = 5; 
UserRequest = 6; 
WrExecutive = 7; 
WrFreePage = 8; 
WrPageIn = 9; 
WrPoolAllocation = 10;
WrDelayExecution = 11; 
WrSuspended = 12; 
WrUserRequest = 13; 
WrEventPair = 14; 
WrQueue = 15; 
WrLpcReceive = 16; 
WrLpcReply = 17; 
WrVirtualMemory = 18; 
WrPageOut = 19; 
WrRendezvous = 20;
Spare2 = 21; 
Spare3 = 22; 
Spare4 = 23; 
Spare5 = 24; 
Spare6 = 25; 
WrKernel = 26; 
MaximumWaitReason = 27; 

KernelMode = 0;
UserMode = 1;
MaximumMode = 2;

NonPagedPool = 0;
PagedPool = 1;
NonPagedPoolMustSucceed = 2;
DontUseThisType = 3;
NonPagedPoolCacheAligned = 4;
PagedPoolCacheAligned = 5;
NonPagedPoolCacheAlignedMustS = 6;
MaxPoolType = 7;

FILE_ANY_ACCESS = 0;
FILE_READ_ACCESS = 1;
FILE_WRITE_ACCESS = 2;

METHOD_BUFFERED = 0;
METHOD_IN_DIRECT = 1;
METHOD_OUT_DIRECT = 2;
METHOD_NEITHER = 3;

MDL_MAPPED_TO_SYSTEM_VA = 1;
MDL_SOURCE_IS_NONPAGED_POOL = 4;
MmCached = 1;
LowPagePriority = 0;
STANDARD_RIGHTS_REQUIRED = $000F0000;
SYNCHRONIZE = $00100000;
THREAD_ALL_ACCESS = STANDARD_RIGHTS_REQUIRED or SYNCHRONIZE or $3FF;
SL_PENDING_RETURNED = 1;
STATUS_PENDING = $103;
STATUS_CANCELLED = $0C0000120;
DISPATCH_LEVEL = 2;

WDF_NO_HANDLE = Nil;
WDF_NO_OBJECT_ATTRIBUTES = Nil;

WdfChildListCreateTableIndex = 0;
WdfChildListGetDeviceTableIndex = 1;
WdfChildListRetrievePdoTableIndex = 2;
WdfChildListRetrieveAddressDescriptionTableIndex = 3;
WdfChildListBeginScanTableIndex = 4;
WdfChildListEndScanTableIndex = 5;
WdfChildListBeginIterationTableIndex = 6;
WdfChildListRetrieveNextDeviceTableIndex = 7;
WdfChildListEndIterationTableIndex = 8;
WdfChildListAddOrUpdateChildDescriptionAsPresentTableIndex = 9;
WdfChildListUpdateChildDescriptionAsMissingTableIndex = 10;
WdfChildListUpdateAllChildDescriptionsAsPresentTableIndex = 11;
WdfChildListRequestChildEjectTableIndex = 12;
WdfCollectionCreateTableIndex = 13;
WdfCollectionGetCountTableIndex = 14;
WdfCollectionAddTableIndex = 15;
WdfCollectionRemoveTableIndex = 16;
WdfCollectionRemoveItemTableIndex = 17;
WdfCollectionGetItemTableIndex = 18;
WdfCollectionGetFirstItemTableIndex = 19;
WdfCollectionGetLastItemTableIndex = 20;
WdfCommonBufferCreateTableIndex = 21;
WdfCommonBufferGetAlignedVirtualAddressTableIndex = 22;
WdfCommonBufferGetAlignedLogicalAddressTableIndex = 23;
WdfCommonBufferGetLengthTableIndex = 24;
WdfControlDeviceInitAllocateTableIndex = 25;
WdfControlDeviceInitSetShutdownNotificationTableIndex = 26;
WdfControlFinishInitializingTableIndex = 27;
WdfDeviceGetDeviceStateTableIndex = 28;
WdfDeviceSetDeviceStateTableIndex = 29;
WdfWdmDeviceGetWdfDeviceHandleTableIndex = 30;
WdfDeviceWdmGetDeviceObjectTableIndex = 31;
WdfDeviceWdmGetAttachedDeviceTableIndex = 32;
WdfDeviceWdmGetPhysicalDeviceTableIndex = 33;
WdfDeviceWdmDispatchPreprocessedIrpTableIndex = 34;
WdfDeviceAddDependentUsageDeviceObjectTableIndex = 35;
WdfDeviceAddRemovalRelationsPhysicalDeviceTableIndex = 36;
WdfDeviceRemoveRemovalRelationsPhysicalDeviceTableIndex = 37;
WdfDeviceClearRemovalRelationsDevicesTableIndex = 38;
WdfDeviceGetDriverTableIndex = 39;
WdfDeviceRetrieveDeviceNameTableIndex = 40;
WdfDeviceAssignMofResourceNameTableIndex = 41;
WdfDeviceGetIoTargetTableIndex = 42;
WdfDeviceGetDevicePnpStateTableIndex = 43;
WdfDeviceGetDevicePowerStateTableIndex = 44;
WdfDeviceGetDevicePowerPolicyStateTableIndex = 45;
WdfDeviceAssignS0IdleSettingsTableIndex = 46;
WdfDeviceAssignSxWakeSettingsTableIndex = 47;
WdfDeviceOpenRegistryKeyTableIndex = 48;
WdfDeviceSetSpecialFileSupportTableIndex = 49;
WdfDeviceSetCharacteristicsTableIndex = 50;
WdfDeviceGetCharacteristicsTableIndex = 51;
WdfDeviceGetAlignmentRequirementTableIndex = 52;
WdfDeviceSetAlignmentRequirementTableIndex = 53;
WdfDeviceInitFreeTableIndex = 54;
WdfDeviceInitSetPnpPowerEventCallbacksTableIndex = 55;
WdfDeviceInitSetPowerPolicyEventCallbacksTableIndex = 56;
WdfDeviceInitSetPowerPolicyOwnershipTableIndex = 57;
WdfDeviceInitRegisterPnpStateChangeCallbackTableIndex = 58;
WdfDeviceInitRegisterPowerStateChangeCallbackTableIndex = 59;
WdfDeviceInitRegisterPowerPolicyStateChangeCallbackTableIndex = 60;
WdfDeviceInitSetIoTypeTableIndex = 61;
WdfDeviceInitSetExclusiveTableIndex = 62;
WdfDeviceInitSetPowerNotPageableTableIndex = 63;
WdfDeviceInitSetPowerPageableTableIndex = 64;
WdfDeviceInitSetPowerInrushTableIndex = 65;
WdfDeviceInitSetDeviceTypeTableIndex = 66;
WdfDeviceInitAssignNameTableIndex = 67;
WdfDeviceInitAssignSDDLStringTableIndex = 68;
WdfDeviceInitSetDeviceClassTableIndex = 69;
WdfDeviceInitSetCharacteristicsTableIndex = 70;
WdfDeviceInitSetFileObjectConfigTableIndex = 71;
WdfDeviceInitSetRequestAttributesTableIndex = 72;
WdfDeviceInitAssignWdmIrpPreprocessCallbackTableIndex = 73;
WdfDeviceInitSetIoInCallerContextCallbackTableIndex = 74;
WdfDeviceCreateTableIndex = 75;
WdfDeviceSetStaticStopRemoveTableIndex = 76;
WdfDeviceCreateDeviceInterfaceTableIndex = 77;
WdfDeviceSetDeviceInterfaceStateTableIndex = 78;
WdfDeviceRetrieveDeviceInterfaceStringTableIndex = 79;
WdfDeviceCreateSymbolicLinkTableIndex = 80;
WdfDeviceQueryPropertyTableIndex = 81;
WdfDeviceAllocAndQueryPropertyTableIndex = 82;
WdfDeviceSetPnpCapabilitiesTableIndex = 83;
WdfDeviceSetPowerCapabilitiesTableIndex = 84;
WdfDeviceSetBusInformationForChildrenTableIndex = 85;
WdfDeviceIndicateWakeStatusTableIndex = 86;
WdfDeviceSetFailedTableIndex = 87;
WdfDeviceStopIdleTableIndex = 88;
WdfDeviceResumeIdleTableIndex = 89;
WdfDeviceGetFileObjectTableIndex = 90;
WdfDeviceEnqueueRequestTableIndex = 91;
WdfDeviceGetDefaultQueueTableIndex = 92;
WdfDeviceConfigureRequestDispatchingTableIndex = 93;
WdfDmaEnablerCreateTableIndex = 94;
WdfDmaEnablerGetMaximumLengthTableIndex = 95;
WdfDmaEnablerGetMaximumScatterGatherElementsTableIndex = 96;
WdfDmaEnablerSetMaximumScatterGatherElementsTableIndex = 97;
WdfDmaTransactionCreateTableIndex = 98;
WdfDmaTransactionInitializeTableIndex = 99;
WdfDmaTransactionInitializeUsingRequestTableIndex = 100;
WdfDmaTransactionExecuteTableIndex = 101;
WdfDmaTransactionReleaseTableIndex = 102;
WdfDmaTransactionDmaCompletedTableIndex = 103;
WdfDmaTransactionDmaCompletedWithLengthTableIndex = 104;
WdfDmaTransactionDmaCompletedFinalTableIndex = 105;
WdfDmaTransactionGetBytesTransferredTableIndex = 106;
WdfDmaTransactionSetMaximumLengthTableIndex = 107;
WdfDmaTransactionGetRequestTableIndex = 108;
WdfDmaTransactionGetCurrentDmaTransferLengthTableIndex = 109;
WdfDmaTransactionGetDeviceTableIndex = 110;
WdfDpcCreateTableIndex = 111;
WdfDpcEnqueueTableIndex = 112;
WdfDpcCancelTableIndex = 113;
WdfDpcGetParentObjectTableIndex = 114;
WdfDpcWdmGetDpcTableIndex = 115;
WdfDriverCreateTableIndex = 116;
WdfDriverGetRegistryPathTableIndex = 117;
WdfDriverWdmGetDriverObjectTableIndex = 118;
WdfDriverOpenParametersRegistryKeyTableIndex = 119;
WdfWdmDriverGetWdfDriverHandleTableIndex = 120;
WdfDriverRegisterTraceInfoTableIndex = 121;
WdfDriverRetrieveVersionStringTableIndex = 122;
WdfDriverIsVersionAvailableTableIndex = 123;
WdfFdoInitWdmGetPhysicalDeviceTableIndex = 124;
WdfFdoInitOpenRegistryKeyTableIndex = 125;
WdfFdoInitQueryPropertyTableIndex = 126;
WdfFdoInitAllocAndQueryPropertyTableIndex = 127;
WdfFdoInitSetEventCallbacksTableIndex = 128;
WdfFdoInitSetFilterTableIndex = 129;
WdfFdoInitSetDefaultChildListConfigTableIndex = 130;
WdfFdoQueryForInterfaceTableIndex = 131;
WdfFdoGetDefaultChildListTableIndex = 132;
WdfFdoAddStaticChildTableIndex = 133;
WdfFdoLockStaticChildListForIterationTableIndex = 134;
WdfFdoRetrieveNextStaticChildTableIndex = 135;
WdfFdoUnlockStaticChildListFromIterationTableIndex = 136;
WdfFileObjectGetFileNameTableIndex = 137;
WdfFileObjectGetFlagsTableIndex = 138;
WdfFileObjectGetDeviceTableIndex = 139;
WdfFileObjectWdmGetFileObjectTableIndex = 140;
WdfInterruptCreateTableIndex = 141;
WdfInterruptQueueDpcForIsrTableIndex = 142;
WdfInterruptSynchronizeTableIndex = 143;
WdfInterruptAcquireLockTableIndex = 144;
WdfInterruptReleaseLockTableIndex = 145;
WdfInterruptEnableTableIndex = 146;
WdfInterruptDisableTableIndex = 147;
WdfInterruptWdmGetInterruptTableIndex = 148;
WdfInterruptGetInfoTableIndex = 149;
WdfInterruptSetPolicyTableIndex = 150;
WdfInterruptGetDeviceTableIndex = 151;
WdfIoQueueCreateTableIndex = 152;
WdfIoQueueGetStateTableIndex = 153;
WdfIoQueueStartTableIndex = 154;
WdfIoQueueStopTableIndex = 155;
WdfIoQueueStopSynchronouslyTableIndex = 156;
WdfIoQueueGetDeviceTableIndex = 157;
WdfIoQueueRetrieveNextRequestTableIndex = 158;
WdfIoQueueRetrieveRequestByFileObjectTableIndex = 159;
WdfIoQueueFindRequestTableIndex = 160;
WdfIoQueueRetrieveFoundRequestTableIndex = 161;
WdfIoQueueDrainSynchronouslyTableIndex = 162;
WdfIoQueueDrainTableIndex = 163;
WdfIoQueuePurgeSynchronouslyTableIndex = 164;
WdfIoQueuePurgeTableIndex = 165;
WdfIoQueueReadyNotifyTableIndex = 166;
WdfIoTargetCreateTableIndex = 167;
WdfIoTargetOpenTableIndex = 168;
WdfIoTargetCloseForQueryRemoveTableIndex = 169;
WdfIoTargetCloseTableIndex = 170;
WdfIoTargetStartTableIndex = 171;
WdfIoTargetStopTableIndex = 172;
WdfIoTargetGetStateTableIndex = 173;
WdfIoTargetGetDeviceTableIndex = 174;
WdfIoTargetQueryTargetPropertyTableIndex = 175;
WdfIoTargetAllocAndQueryTargetPropertyTableIndex = 176;
WdfIoTargetQueryForInterfaceTableIndex = 177;
WdfIoTargetWdmGetTargetDeviceObjectTableIndex = 178;
WdfIoTargetWdmGetTargetPhysicalDeviceTableIndex = 179;
WdfIoTargetWdmGetTargetFileObjectTableIndex = 180;
WdfIoTargetWdmGetTargetFileHandleTableIndex = 181;
WdfIoTargetSendReadSynchronouslyTableIndex = 182;
WdfIoTargetFormatRequestForReadTableIndex = 183;
WdfIoTargetSendWriteSynchronouslyTableIndex = 184;
WdfIoTargetFormatRequestForWriteTableIndex = 185;
WdfIoTargetSendIoctlSynchronouslyTableIndex = 186;
WdfIoTargetFormatRequestForIoctlTableIndex = 187;
WdfIoTargetSendInternalIoctlSynchronouslyTableIndex = 188;
WdfIoTargetFormatRequestForInternalIoctlTableIndex = 189;
WdfIoTargetSendInternalIoctlOthersSynchronouslyTableIndex = 190;
WdfIoTargetFormatRequestForInternalIoctlOthersTableIndex = 191;
WdfMemoryCreateTableIndex = 192;
WdfMemoryCreatePreallocatedTableIndex = 193;
WdfMemoryGetBufferTableIndex = 194;
WdfMemoryAssignBufferTableIndex = 195;
WdfMemoryCopyToBufferTableIndex = 196;
WdfMemoryCopyFromBufferTableIndex = 197;
WdfLookasideListCreateTableIndex = 198;
WdfMemoryCreateFromLookasideTableIndex = 199;
WdfDeviceMiniportCreateTableIndex = 200;
WdfDriverMiniportUnloadTableIndex = 201;
WdfObjectGetTypedContextWorkerTableIndex = 202;
WdfObjectAllocateContextTableIndex = 203;
WdfObjectContextGetObjectTableIndex = 204;
WdfObjectReferenceActualTableIndex = 205;
WdfObjectDereferenceActualTableIndex = 206;
WdfObjectCreateTableIndex = 207;
WdfObjectDeleteTableIndex = 208;
WdfObjectQueryTableIndex = 209;
WdfPdoInitAllocateTableIndex = 210;
WdfPdoInitSetEventCallbacksTableIndex = 211;
WdfPdoInitAssignDeviceIDTableIndex = 212;
WdfPdoInitAssignInstanceIDTableIndex = 213;
WdfPdoInitAddHardwareIDTableIndex = 214;
WdfPdoInitAddCompatibleIDTableIndex = 215;
WdfPdoInitAddDeviceTextTableIndex = 216;
WdfPdoInitSetDefaultLocaleTableIndex = 217;
WdfPdoInitAssignRawDeviceTableIndex = 218;
WdfPdoMarkMissingTableIndex = 219;
WdfPdoRequestEjectTableIndex = 220;
WdfPdoGetParentTableIndex = 221;
WdfPdoRetrieveIdentificationDescriptionTableIndex = 222;
WdfPdoRetrieveAddressDescriptionTableIndex = 223;
WdfPdoUpdateAddressDescriptionTableIndex = 224;
WdfPdoAddEjectionRelationsPhysicalDeviceTableIndex = 225;
WdfPdoRemoveEjectionRelationsPhysicalDeviceTableIndex = 226;
WdfPdoClearEjectionRelationsDevicesTableIndex = 227;
WdfDeviceAddQueryInterfaceTableIndex = 228;
WdfRegistryOpenKeyTableIndex = 229;
WdfRegistryCreateKeyTableIndex = 230;
WdfRegistryCloseTableIndex = 231;
WdfRegistryWdmGetHandleTableIndex = 232;
WdfRegistryRemoveKeyTableIndex = 233;
WdfRegistryRemoveValueTableIndex = 234;
WdfRegistryQueryValueTableIndex = 235;
WdfRegistryQueryMemoryTableIndex = 236;
WdfRegistryQueryMultiStringTableIndex = 237;
WdfRegistryQueryUnicodeStringTableIndex = 238;
WdfRegistryQueryStringTableIndex = 239;
WdfRegistryQueryULongTableIndex = 240;
WdfRegistryAssignValueTableIndex = 241;
WdfRegistryAssignMemoryTableIndex = 242;
WdfRegistryAssignMultiStringTableIndex = 243;
WdfRegistryAssignUnicodeStringTableIndex = 244;
WdfRegistryAssignStringTableIndex = 245;
WdfRegistryAssignULongTableIndex = 246;
WdfRequestCreateTableIndex = 247;
WdfRequestCreateFromIrpTableIndex = 248;
WdfRequestReuseTableIndex = 249;
WdfRequestChangeTargetTableIndex = 250;
WdfRequestFormatRequestUsingCurrentTypeTableIndex = 251;
WdfRequestWdmFormatUsingStackLocationTableIndex = 252;
WdfRequestSendTableIndex = 253;
WdfRequestGetStatusTableIndex = 254;
WdfRequestMarkCancelableTableIndex = 255;
WdfRequestUnmarkCancelableTableIndex = 256;
WdfRequestIsCanceledTableIndex = 257;
WdfRequestCancelSentRequestTableIndex = 258;
WdfRequestIsFrom32BitProcessTableIndex = 259;
WdfRequestSetCompletionRoutineTableIndex = 260;
WdfRequestGetCompletionParamsTableIndex = 261;
WdfRequestAllocateTimerTableIndex = 262;
WdfRequestCompleteTableIndex = 263;
WdfRequestCompleteWithPriorityBoostTableIndex = 264;
WdfRequestCompleteWithInformationTableIndex = 265;
WdfRequestGetParametersTableIndex = 266;
WdfRequestRetrieveInputMemoryTableIndex = 267;
WdfRequestRetrieveOutputMemoryTableIndex = 268;
WdfRequestRetrieveInputBufferTableIndex = 269;
WdfRequestRetrieveOutputBufferTableIndex = 270;
WdfRequestRetrieveInputWdmMdlTableIndex = 271;
WdfRequestRetrieveOutputWdmMdlTableIndex = 272;
WdfRequestRetrieveUnsafeUserInputBufferTableIndex = 273;
WdfRequestRetrieveUnsafeUserOutputBufferTableIndex = 274;
WdfRequestSetInformationTableIndex = 275;
WdfRequestGetInformationTableIndex = 276;
WdfRequestGetFileObjectTableIndex = 277;
WdfRequestProbeAndLockUserBufferForReadTableIndex = 278;
WdfRequestProbeAndLockUserBufferForWriteTableIndex = 279;
WdfRequestGetRequestorModeTableIndex = 280;
WdfRequestForwardToIoQueueTableIndex = 281;
WdfRequestGetIoQueueTableIndex = 282;
WdfRequestRequeueTableIndex = 283;
WdfRequestStopAcknowledgeTableIndex = 284;
WdfRequestWdmGetIrpTableIndex = 285;
WdfIoResourceRequirementsListSetSlotNumberTableIndex = 286;
WdfIoResourceRequirementsListSetInterfaceTypeTableIndex = 287;
WdfIoResourceRequirementsListAppendIoResListTableIndex = 288;
WdfIoResourceRequirementsListInsertIoResListTableIndex = 289;
WdfIoResourceRequirementsListGetCountTableIndex = 290;
WdfIoResourceRequirementsListGetIoResListTableIndex = 291;
WdfIoResourceRequirementsListRemoveTableIndex = 292;
WdfIoResourceRequirementsListRemoveByIoResListTableIndex = 293;
WdfIoResourceListCreateTableIndex = 294;
WdfIoResourceListAppendDescriptorTableIndex = 295;
WdfIoResourceListInsertDescriptorTableIndex = 296;
WdfIoResourceListUpdateDescriptorTableIndex = 297;
WdfIoResourceListGetCountTableIndex = 298;
WdfIoResourceListGetDescriptorTableIndex = 299;
WdfIoResourceListRemoveTableIndex = 300;
WdfIoResourceListRemoveByDescriptorTableIndex = 301;
WdfCmResourceListAppendDescriptorTableIndex = 302;
WdfCmResourceListInsertDescriptorTableIndex = 303;
WdfCmResourceListGetCountTableIndex = 304;
WdfCmResourceListGetDescriptorTableIndex = 305;
WdfCmResourceListRemoveTableIndex = 306;
WdfCmResourceListRemoveByDescriptorTableIndex = 307;
WdfStringCreateTableIndex = 308;
WdfStringGetUnicodeStringTableIndex = 309;
WdfObjectAcquireLockTableIndex = 310;
WdfObjectReleaseLockTableIndex = 311;
WdfWaitLockCreateTableIndex = 312;
WdfWaitLockAcquireTableIndex = 313;
WdfWaitLockReleaseTableIndex = 314;
WdfSpinLockCreateTableIndex = 315;
WdfSpinLockAcquireTableIndex = 316;
WdfSpinLockReleaseTableIndex = 317;
WdfTimerCreateTableIndex = 318;
WdfTimerStartTableIndex = 319;
WdfTimerStopTableIndex = 320;
WdfTimerGetParentObjectTableIndex = 321;
WdfUsbTargetDeviceCreateTableIndex = 322;
WdfUsbTargetDeviceRetrieveInformationTableIndex = 323;
WdfUsbTargetDeviceGetDeviceDescriptorTableIndex = 324;
WdfUsbTargetDeviceRetrieveConfigDescriptorTableIndex = 325;
WdfUsbTargetDeviceQueryStringTableIndex = 326;
WdfUsbTargetDeviceAllocAndQueryStringTableIndex = 327;
WdfUsbTargetDeviceFormatRequestForStringTableIndex = 328;
WdfUsbTargetDeviceGetNumInterfacesTableIndex = 329;
WdfUsbTargetDeviceSelectConfigTableIndex = 330;
WdfUsbTargetDeviceWdmGetConfigurationHandleTableIndex = 331;
WdfUsbTargetDeviceRetrieveCurrentFrameNumberTableIndex = 332;
WdfUsbTargetDeviceSendControlTransferSynchronouslyTableIndex = 333;
WdfUsbTargetDeviceFormatRequestForControlTransferTableIndex = 334;
WdfUsbTargetDeviceIsConnectedSynchronousTableIndex = 335;
WdfUsbTargetDeviceResetPortSynchronouslyTableIndex = 336;
WdfUsbTargetDeviceCyclePortSynchronouslyTableIndex = 337;
WdfUsbTargetDeviceFormatRequestForCyclePortTableIndex = 338;
WdfUsbTargetDeviceSendUrbSynchronouslyTableIndex = 339;
WdfUsbTargetDeviceFormatRequestForUrbTableIndex = 340;
WdfUsbTargetPipeGetInformationTableIndex = 341;
WdfUsbTargetPipeIsInEndpointTableIndex = 342;
WdfUsbTargetPipeIsOutEndpointTableIndex = 343;
WdfUsbTargetPipeGetTypeTableIndex = 344;
WdfUsbTargetPipeSetNoMaximumPacketSizeCheckTableIndex = 345;
WdfUsbTargetPipeWriteSynchronouslyTableIndex = 346;
WdfUsbTargetPipeFormatRequestForWriteTableIndex = 347;
WdfUsbTargetPipeReadSynchronouslyTableIndex = 348;
WdfUsbTargetPipeFormatRequestForReadTableIndex = 349;
WdfUsbTargetPipeConfigContinuousReaderTableIndex = 350;
WdfUsbTargetPipeAbortSynchronouslyTableIndex = 351;
WdfUsbTargetPipeFormatRequestForAbortTableIndex = 352;
WdfUsbTargetPipeResetSynchronouslyTableIndex = 353;
WdfUsbTargetPipeFormatRequestForResetTableIndex = 354;
WdfUsbTargetPipeSendUrbSynchronouslyTableIndex = 355;
WdfUsbTargetPipeFormatRequestForUrbTableIndex = 356;
WdfUsbInterfaceGetInterfaceNumberTableIndex = 357;
WdfUsbInterfaceGetNumEndpointsTableIndex = 358;
WdfUsbInterfaceGetDescriptorTableIndex = 359;
WdfUsbInterfaceSelectSettingTableIndex = 360;
WdfUsbInterfaceGetEndpointInformationTableIndex = 361;
WdfUsbTargetDeviceGetInterfaceTableIndex = 362;
WdfUsbInterfaceGetConfiguredSettingIndexTableIndex = 363;
WdfUsbInterfaceGetNumConfiguredPipesTableIndex = 364;
WdfUsbInterfaceGetConfiguredPipeTableIndex = 365;
WdfUsbTargetPipeWdmGetPipeHandleTableIndex = 366;
WdfVerifierDbgBreakPointTableIndex = 367;
WdfVerifierKeBugCheckTableIndex = 368;
WdfWmiProviderCreateTableIndex = 369;
WdfWmiProviderGetDeviceTableIndex = 370;
WdfWmiProviderIsEnabledTableIndex = 371;
WdfWmiProviderGetTracingHandleTableIndex = 372;
WdfWmiInstanceCreateTableIndex = 373;
WdfWmiInstanceRegisterTableIndex = 374;
WdfWmiInstanceDeregisterTableIndex = 375;
WdfWmiInstanceGetDeviceTableIndex = 376;
WdfWmiInstanceGetProviderTableIndex = 377;
WdfWmiInstanceFireEventTableIndex = 378;
WdfWorkItemCreateTableIndex = 379;
WdfWorkItemEnqueueTableIndex = 380;
WdfWorkItemGetParentObjectTableIndex = 381;
WdfWorkItemFlushTableIndex = 382;
WdfCommonBufferCreateWithConfigTableIndex = 383;
WdfDmaEnablerGetFragmentLengthTableIndex = 384;
WdfDmaEnablerWdmGetDmaAdapterTableIndex = 385;
WdfUsbInterfaceGetNumSettingsTableIndex = 386;
WdfDeviceRemoveDependentUsageDeviceObjectTableIndex = 387;
WdfDeviceGetSystemPowerActionTableIndex = 388;
WdfInterruptSetExtendedPolicyTableIndex = 389;
WdfIoQueueAssignForwardProgressPolicyTableIndex = 390;
WdfPdoInitAssignContainerIDTableIndex = 391;
WdfPdoInitAllowForwardingRequestToParentTableIndex = 392;
WdfRequestMarkCancelableExTableIndex = 393;
WdfRequestIsReservedTableIndex = 394;
WdfRequestForwardToParentDeviceIoQueueTableIndex = 395;
WdfFunctionTableNumEntries = 396;
WDF_DRIVER_GLOBALS_NAME_LEN = 32;

WdfDeviceIoUndefined = 0;
WdfDeviceIoNeither = 1;
WdfDeviceIoBuffered = 2;
WdfDeviceIoDirect = 3;
WdfDeviceIoBufferedOrDirect = 4;
WdfDeviceIoMaximum = 5;

WdfFileObjectWdfCannotUseFsContexts = 4;
WdfUseDefault = 2;

WdfIoQueueDispatchInvalid = 0;
WdfIoQueueDispatchSequential = 1;
WdfIoQueueDispatchParallel = 2;
WdfIoQueueDispatchManual = 3;
WdfIoQueueDispatchMax = 4;

//
// types are very important, 
// because we want to code drivers in Delphi we use Delphi style of types,
// but also we want to have some code compatibilities
// so we implement also WinAPI (C) style of types 
//
type
LONG = Integer;
PLONG = ^LONG;
ULONG = Cardinal;
PULONG = ^ULONG;
NTSTATUS = ULONG;
LCID = ULONG;
TDeviceType = ULONG;
DEVICE_TYPE = TDeviceType;
TKProcessorMode = Byte;
KPROCESSOR_MODE = TKProcessorMode;
TKIrql = Byte;
KIRQL = TKIRQL;
PKIRQL = ^KIRQL;
PEThread = Pointer;
PEProcess = Pointer;
PKThread = Pointer; // PKTHREAD
PHandle = ^THandle;
Handle = THandle;
TAccessMask = ULONG;
PKSTART_ROUTINE = procedure(StartContext : Pointer); stdcall;
ACCESS_MASK = ULONG;
POBJECT_HANDLE_INFORMATION = Pointer;
WDFDRIVER = ULONG;
PWDFDRIVER = Pointer;
PWDFDEVICE_INIT = Pointer;
PPWDFDEVICE_INIT = ^PWDFDEVICE_INIT;
WDFDEVICE = ULONG;
PWDFDEVICE = Pointer;
WDFOBJECT = Pointer;
WDF_EXECUTION_LEVEL = ULONG;
WDF_SYNCHRONIZATION_SCOPE = ULONG;
WDF_DEVICE_IO_TYPE = ULONG;
WDFQUEUE = ULONG;
PWDFQUEUE = ^WDFQUEUE;
WDFREQUEST = ULONG;
WDFFILEOBJECT = ULONG;
WDFDPC = ULONG;
WDFTIMER = ULONG;
WDFWORKITEM = ULONG;
WDFINTERRUPT = ULONG;
WDFMEMORY = ULONG;
PWDFMEMORY = ^WDFMEMORY;
WDF_TRI_STATE = ULONG;
PWDF_TRI_STATE = ^WDF_TRI_STATE;
WDF_FILEOBJECT_CLASS = ULONG;
PWDF_FILEOBJECT_CLASS = ^WDF_FILEOBJECT_CLASS;
WDF_IO_QUEUE_DISPATCH_TYPE = ULONG;
DWORD = ULONG;
KPRIORITY = LONG;
EVENT_TYPE = (NotificationEvent = $00, SynchronizationEvent = $01);

PUnicodeString = ^TUnicodeString;
TUnicodeString = packed record
    Length : Word;
    MaximumLength : Word;
    Buffer : PWideChar;
end;
UNICODE_STRING = TUnicodeString;
PUNICODE_STRING = ^UNICODE_STRING;

PLargeInteger = ^TLargeInteger;
TLargeInteger = packed record
    LowPart : Cardinal;
    HighPart : Integer;
end;

PObjectAttributes = ^TObjectAttributes;
TObjectAttributes = packed record
    Length : Cardinal;
    RootDirectory : THandle;
    ObjectName : PUnicodeString;
    Attributes : Cardinal;
    SecurityDescriptor : Pointer;
    SecurityQualityOfService : Pointer;
end;
OBJECT_ATTRIBUTES = ^TObjectAttributes;
POBJECT_ATTRIBUTES = ^OBJECT_ATTRIBUTES;

PClientId = ^TClientId;
TClientId = packed record
    UniqueProcess : Cardinal;
    UniqueThread : Cardinal;
end;
CLIENT_ID = TClientId;
PCLIENT_ID = ^CLIENT_ID;

PDriverObject = ^TDriverObject;
PDeviceObject = ^TDeviceObject;
PDEVICE_OBJECT = PDeviceObject;
PIrp = ^TIrp;

PListEntry = ^TListEntry;
TListEntry = packed record
    Flink : PListEntry;
    BLink : PListEntry;
end;
LIST_ENTRY = TListEntry;
PLIST_ENTRY = ^LIST_ENTRY;
PRLIST_ENTRY = PLIST_ENTRY;

PKDeviceQueueEntry = ^TKDeviceQueueEntry;
TKDeviceQueueEntry = packed record
    DeviceListEntry : TListEntry;
    SortKey : Cardinal;
    Inserted : LongBool;
end;
KDEVICE_QUEUE_ENTRY = TKDeviceQueueEntry;
PKDEVICE_QUEUE_ENTRY = ^KDEVICE_QUEUE_ENTRY;
PRKDEVICE_QUEUE_ENTRY = PKDEVICE_QUEUE_ENTRY;

PWaitContextBlock = ^TWaitContextBlock;
TWaitContextBlock = packed record
    WaitQueueEntry : TKDeviceQueueEntry;
    DeviceRoutine : Pointer; // PDRIVER_CONTROL
    DeviceContext : Pointer;
    NumberOfMapRegisters : Cardinal;
    DeviceObject : Pointer;
    CurrentIrp : Pointer;
    BufferChainingDpc : Pointer; // PKDPC
end;
WAIT_CONTEXT_BLOCK = TWaitContextBlock;
PWAIT_CONTEXT_BLOCK = ^WAIT_CONTEXT_BLOCK;

TKSpinLock = ULONG; // ULONG_PTR
PKSpinLock = ^TKSpinLock;
KSPIN_LOCK = TKSpinLock;
PKSPIN_LOCK = ^KSPIN_LOCK;

TDeviceObjectUnionQueue = packed record
    case Byte of
    0 : (ListEntry : TListEntry);
    1 : (Wcb : TWaitContextBlock);
end;

PDevObjExtension = ^TDevObjExtension;
TDevObjExtension = packed record
    wType : Word;
    Size : Word;
    DeviceObject : PDeviceObject;
end;
DEVOBJ_EXTENSION = TDevObjExtension;
PDEVOBJ_EXTENSION = ^DEVOBJ_EXTENSION;

PKDeviceQueue = ^TKDeviceQueue;
TKDeviceQueue = packed record
    wType : Word;
    Size : Word;
    DeviceListHead : TListEntry;
    Lock : TKSpinLock;
    Busy : LongBool;
end;
KDEVICE_QUEUE = TKDeviceQueue;
PKDEVICE_QUEUE = ^KDEVICE_QUEUE;

PKApc = ^TKApc;
TKApc = packed record
    wType : Word;
    Size : Word;
    Spare0 : Cardinal;
    Thread : PKThread;
    ApcListEntry : TListEntry;
    KernelRoutine : Pointer; // PKKERNEL_ROUTINE
    RundownRoutine : Pointer; // PKRUNDOWN_ROUTINE
    NormalRoutine : Pointer; // KNORMAL_ROUTINE
    NormalContext : Pointer;
    SystemArgument1 : Pointer;
    SystemArgument2 : Pointer;
    ApcStateIndex : Byte;
    ApcMode : TKProcessorMode;
    Inserted : WordBool;
end;
KAPC = TKApc;
PRKAPC = PKApc;

PKDpc = ^TKDpc;
TKDpc = packed record
    wType : Word;
    Number : Byte;
    Importance : Byte;
    DpcListEntry : TListEntry;
    DeferredRoutine : Pointer; // PKDEFERRED_ROUTINE
    DeferredContext : Pointer;
    SystemArgument1 : Pointer;
    SystemArgument2 : Pointer;
    Lock : Pointer; // PULONG_PTR
end;
KDPC = TKDpc;
PRKDPC = PKDPC;

PDispatcherHeader = ^TDispatcherHeader;
TDispatcherHeader = packed record
    bType : Byte;
    bAbsolute : Byte;
    Size : Byte;
    Inserted : Byte;
    SignalState : Cardinal;
    WaitListHead : TListEntry
end;
DISPATCHER_HEADER = TDispatcherHeader;

TSemaphore = packed record
    Header : DISPATCHER_HEADER;
    Limit : LONG;
end;
KSEMAPHORE = TSemaphore;
PKSEMAPHORE = ^TSemaphore;
PRKSEMAPHORE = ^TSemaphore;

PKEvent = ^TKEvent;
TKEvent = packed record
    Header : TDispatcherHeader;
end;
KEVENT = TKEvent;
PRKEVENT = PKEVENT;

TDeviceObject = packed record
    wType : Word;
    Size : Word;
    ReferenceCount : Integer;
    DriverObject : PDriverObject;
    NextDevice : PDeviceObject;
    AttachedDevice : PDeviceObject;
    CurrentIrp : PIrp;
    Timer : Pointer; // PIO_TIMER
    Flags : Cardinal;
    Characteristics : Cardinal;
    DoNotUse1 : Pointer;
    DeviceExtension : Pointer;
    DeviceType : TDeviceType;
    StackSize : Byte;
    Queue : TDeviceObjectUnionQueue;
    AlignmentRequirement : Cardinal;
    DeviceQueue : TKDeviceQueue;
    Dpc : TKDpc;

    ActiveThreadCount : Cardinal;
    SecurityDescriptor : Pointer; // PSECURITY_DESCRIPTOR
    DeviceLock : TKEvent;

    SectorSize : Word;
    Spare1 : Word;

    DeviceObjectExtension : PDevObjExtension;
    Reserved : Pointer;
end;
DEVICE_OBJECT = TDeviceObject;

TIrpUnionAssociatedIrp = packed record
    case Byte of
    0 : (MasterIrp : PIrp);
    1 : (IrpCount : Cardinal);
    2 : (SystemBuffer : Pointer);
end;

PIoStatusBlock = ^TIoStatusBlock;
TIoStatusBlock = packed record
    Status : NTSTATUS;
    Information : Cardinal; // ULONG_PTR
end;
IO_STATUS_BLOCK = TIoStatusBlock;
PIO_STATUS_BLOCK = ^IO_STATUS_BLOCK;

TIrpUnionOverlayStructAsynchronousParameters = packed record
    UserApcRoutine : Pointer; // PIO_APC_ROUTINE
    UserApcContext : Pointer;
end;

TIrpUnionOverlay = packed record
    case Byte of 
    0 : (AsynchronousParameters : TIrpUnionOverlayStructAsynchronousParameters);
    1 : (AllocationSize : TLargeInteger);
end;

TIrpUnionTailStructOverlayUnion1 = packed record
    case Byte of 
    0 : (DeviceQueueEntry : TKDeviceQueueEntry);
    1 : (DriverContext : array[0..3] of Pointer);
end;

TIrpUnionTailStructOverlayStruct1Union1 = packed record
    case Byte of 
    0 : (CurrentStackLocation : Pointer); // PIO_STACK_LOCATION
    1 : (PacketType : Cardinal);
end;

TIrpUnionTailStructOverlayStruct1 = packed record
    ListEntry : TListEntry;
    u1 : TIrpUnionTailStructOverlayStruct1Union1;
end;

TIrpUnionTailStructOverlay = packed record
    u1 : TIrpUnionTailStructOverlayUnion1;
    Thread : PEThread;
    AuxiliaryBuffer : PChar;
    s1 : TIrpUnionTailStructOverlayStruct1;
    OriginalFileObject : Pointer; // PFILE_OBJECT
end;

TIrpUnionTail = packed record
    case Byte of 
    0 : (Overlay : TIrpUnionTailStructOverlay);
    1 : (Apc : TKApc);
    2 : (CompletionKey : Pointer);
end;

TIrp = packed record
    wType : Word;
    Size : Word;
    MdlAddress : Pointer; // PMDL
    Flags : Cardinal;
    AssociatedIrp : TIrpUnionAssociatedIrp;
    ThreadListEntry : TListEntry;
    IoStatus : TIoStatusBlock;
    RequestorMode : TKProcessorMode;
    PendingReturned : Boolean;
    StackCount : Byte;
    CurrentLocation : Byte;
    Cancel : Boolean;
    CancelIrql : TKIrql;
    ApcEnvironment : Byte;
    AllocationFlags : Byte;
    UserIosb : PIoStatusBlock;
    UserEvent : PKEvent;
    Overlay : TIrpUnionOverlay;
    CancelRoutine : Pointer; // PDRIVER_CANCEL
    UserBuffer : Pointer;
    Tail : TIrpUnionTail;
end;
IRP = TIrp;

PDRIVER_EXTENSION = ^DRIVER_EXTENSION;
DRIVER_EXTENSION = packed record
    DriverObject : Pointer;
    AddDevice : Pointer;
    Count : ULONG;
    ServiceKeyName : UNICODE_STRING;
    ClientDriverExtension : Pointer;
end;

TDriverObject = packed record
    wType : Word;
    Size : Word;
    DeviceObject : PDeviceObject;
    Flags : Cardinal;
    DriverStart : Pointer;
    DriverSize : Cardinal;
    DriverSection : Pointer;
    DriverExtension : PDRIVER_EXTENSION; // PDRIVER_EXTENSION
    DriverName : TUnicodeString;
    HardwareDatabase : PUnicodeString;
    FastIoDispatch : Pointer; // PFAST_IO_DISPATCH
    DriverInit : Pointer; // PDRIVER_INITIALIZE
    DriverStartIo : Pointer; // PDRIVER_STARTIO
    DriverUnload : Pointer; // PDRIVER_UNLOAD
    MajorFunction : array[0..IRP_MJ_MAXIMUM_FUNCTION] of Pointer; // PDRIVER_DISPATCH
end;
PDRIVER_OBJECT = PDriverObject;
DRIVER_OBJECT = TDriverObject;

PFileObject = ^TFileObject;
TFileObject = packed record 
    wType : Word;
    Size : Word;
    DeviceObject : PDeviceObject;
    DoNotUser1 : Pointer;
    FsContext : Pointer;
    FsContext2 : Pointer;
    SectionObjectPointer : Pointer; // PSECTION_OBJECT_POINTERS
    PrivateCacheMap : Pointer;
    FinalStatus : NTSTATUS;
    RelatedFileObject : PFileObject;
    LockOperation : Boolean;
    DeletePending : Boolean;
    ReadAccess : Boolean;
    WriteAccess : Boolean;
    DeleteAccess : Boolean;
    SharedRead : Boolean;
    SharedWrite : Boolean;
    SharedDelete : Boolean;
    Flags : Cardinal;
    FileName : TUnicodeString;
    CurrentByteOffset : TLargeInteger;
    Waiters : Cardinal;
    Busy : Cardinal;
    LastLock : Pointer;
    Lock : TKEvent;
    Event : TKEvent;
    CompletionContext : Pointer; // PIO_COMPLETION_CONTEXT
end;
FILE_OBJECT = TFileObject;
PFILE_OBJECT = ^FILE_OBJECT;

TFileInformationClass = Cardinal;
TFsInformationClass = Cardinal;
TDeviceRelationType = Cardinal;
TBusQueryIdType = Cardinal;
TDeviceTextType = Cardinal;
TDeviceUsageNotificationType = Cardinal;
TSystemPowerState = Cardinal;
TPowerAction = Cardinal;
TDevicePowerState = Cardinal;
TPowerStateType = Cardinal;
TKWaitReason = Cardinal;
TPoolType = Cardinal;

PPowerState = ^TPowerState;
TPowerState = packed record 
    case Byte of 
    0 : (SystemState : TSystemPowerState);
    1 : (DeviceState : TDevicePowerState);
end;
POWER_STATE = TPowerState;
PPOWER_STATE = ^POWER_STATE;

TIoStackLocationUnionParametersStructCreate = packed record
    SecurityContext : Pointer; // PIO_SECURITY_CONTEXT 
    Options : Cardinal;
    FileAttributes : Word;
    ShareAccess : Word;
    EaLength : Cardinal;
end;

TIoStackLocationUnionParametersStructRead = packed record
    Length : Cardinal;
    Key : Cardinal;
    ByteOffset : TLargeInteger;
end;

TIoStackLocationUnionParametersStructWrite = packed record
    Length : Cardinal;
    Key : Cardinal;
    ByteOffset : TLargeInteger;
end;

TIoStackLocationUnionParametersStructQueryFile = packed record
    Length : Cardinal;
    FileInformationClass : TFileInformationClass;
end;

TIoStackLocationUnionParametersStructSetFile = packed record
    Length : Cardinal;
    FileInformationClass : TFileInformationClass;
    FileObject : PFileObject;
    case Byte of
    0 : (ReplaceIfExists : Boolean;
    AdvanceOnly : Boolean);
    1 : (CluserCount : Cardinal);
    2 : (DeleteHandle : THandle);
end;

TIoStackLocationUnionParametersStructQueryVolume = packed record
    Length : Cardinal;
    FsInformationClass : TFsInformationClass;
end;

TIoStackLocationUnionParametersStructDeviceIoControl = packed record
    OutputBufferLength : Cardinal;
    InputBufferLength : Cardinal;
    IoControlCode : Cardinal;
    Type3InputBuffer : Pointer;
end;

TIoStackLocationUnionParametersStructMountVolume = packed record
    DoNotUse1 : Pointer;
    DeviceObject : PDeviceObject;
end;

TIoStackLocationUnionParametersStructVerifyVolume = packed record
    DoNotUse1 : Pointer;
    DeviceObject : PDeviceObject;
end;

TIoStackLocationUnionParametersStructScsi = packed record
    Srn : Pointer; // _SCSI_REQUEST_BLOCK *
end;

TIoStackLocationUnionParametersStructQueryDeviceRelations = packed record
    drType : TDeviceRelationType;
end;

TIoStackLocationUnionParametersStructQueryInterface = packed record
    InterfaceType : Pointer; // CONST GUID *
    Size : Word;
    Version : Word;
    pInterface : Pointer; // PINTERFACE
    InterfaceSpecificData : Pointer;
end;

TIoStackLocationUnionParametersStructDeviceCapabilities = packed record
    Capabilities : Pointer; // PDEVICE_CAPABILITIES
end;

TIoStackLocationUnionParametersStructFilterResourceRequirements = packed record
    IoResourceRequirementList : Pointer; // PIO_RESOURCE_REQUIREMENTS_LIST
end;

TIoStackLocationUnionParametersStructReadWriteConfig = packed record
    WhichSpace : Cardinal;
    Buffer : Pointer;
    Offset : Cardinal;
    Length : Cardinal;
end;

TIoStackLocationUnionParametersStructSetLock = packed record
    Lock : LongBool;
end;

TIoStackLocationUnionParametersStructQueryId = packed record
    IdType : TBusQueryIdType;
end;

TIoStackLocationUnionParametersStructQueryDeviceText = packed record
    DeviceTextType : TDeviceTextType;
    LocaleId : LCID;
end;

TIoStackLocationUnionParametersStructUsageNotification = packed record
    InPath : Boolean;
    Reserved : array[0..2] of Boolean;
    dunType : TDeviceUsageNotificationType;
end;

TIoStackLocationUnionParametersStructWaitWake = packed record
    PowerState : TSystemPowerState;
end;

TIoStackLocationUnionParametersStructPowerSequence = packed record
    PowerSequence : Pointer; // PPOWER_SEQUENCE
end;

TIoStackLocationUnionParametersStructPower = packed record
    SystemContext : Cardinal;
    psType : TPowerStateType;
    State : TPowerState;
    ShutdownType : TPowerAction;
end;

TIoStackLocationUnionParametersStructStartDevice = packed record
    AllocatedResources : Pointer; // PCM_RESOURCE_LIST
    AllocatedResourcesTranslated : Pointer; // PCM_RESOURCE_LIST
end;

TIoStackLocationUnionParametersStructWMI = packed record
    ProviderId : Pointer; // ULONG_PTR
    DataPath : Pointer;
    BufferSize : Cardinal;
    Buffer : Cardinal;
end;

TIoStackLocationUnionParametersStructOthers = packed record
    Argument1 : Pointer;
    Argument2 : Pointer;
    Argument3 : Pointer;
    Argument4 : Pointer;
end;

TIoStackLocationUnionParameters = packed record
    case Byte of
    00 : (Create : TIoStackLocationUnionParametersStructCreate);
    01 : (Read : TIoStackLocationUnionParametersStructRead);
    02 : (Write : TIoStackLocationUnionParametersStructWrite);
    03 : (QueryFile : TIoStackLocationUnionParametersStructQueryFile);
    04 : (SetFile : TIoStackLocationUnionParametersStructSetFile);
    05 : (QueryVolume : TIoStackLocationUnionParametersStructQueryVolume);
    06 : (DeviceIoControl : TIoStackLocationUnionParametersStructDeviceIoControl);
    07 : (MountVolume : TIoStackLocationUnionParametersStructMountVolume);
    08 : (VerifyVolume : TIoStackLocationUnionParametersStructVerifyVolume);
    09 : (Scsi : TIoStackLocationUnionParametersStructScsi);
    10 : (QueryDeviceRelations : TIoStackLocationUnionParametersStructQueryDeviceRelations);
    11 : (QueryInterface : TIoStackLocationUnionParametersStructQueryInterface);
    12 : (DeviceCapabilities : TIoStackLocationUnionParametersStructDeviceCapabilities);
    13 : (FilterResourceRequirements : TIoStackLocationUnionParametersStructFilterResourceRequirements);
    14 : (ReadWriteConfig : TIoStackLocationUnionParametersStructReadWriteConfig);
    15 : (SetLock : TIoStackLocationUnionParametersStructSetLock);
    16 : (QueryId : TIoStackLocationUnionParametersStructQueryId);
    17 : (QueryDeviceText : TIoStackLocationUnionParametersStructQueryDeviceText);
    18 : (UsageNotification : TIoStackLocationUnionParametersStructUsageNotification);
    19 : (WaitWake : TIoStackLocationUnionParametersStructWaitWake);
    20 : (PowerSequence : TIoStackLocationUnionParametersStructPowerSequence);
    21 : (Power : TIoStackLocationUnionParametersStructPower);
    22 : (StartDevice : TIoStackLocationUnionParametersStructStartDevice);
    23 : (WMI : TIoStackLocationUnionParametersStructWMI);
    24 : (Others : TIoStackLocationUnionParametersStructOthers);
end;

PIoStackLocation = ^TIoStackLocation;
PIO_STACK_LOCATION = PIoStackLocation;
TIoStackLocation = packed record
    MajorFunction : Byte;
    MinorFunction : Byte;
    Flags : Byte;
    Control : Byte;
    Parameters : TIoStackLocationUnionParameters;
    DeviceObject : PDeviceObject;
    FileObject : PFileObject;
    CompletionRoutine : Pointer; // PIO_COMPLETION_ROUTINE
    Context : Pointer;
end;
IO_STACK_LOCATION = TIoStackLocation;

MDL = packed record
    Next : ULONG;
    Size : WORD;
    MdlFlags : WORD;
    Process : ULONG;
    MappedSystemVa : Pointer;
    StartVa : Pointer;
    ByteCount : ULONG;
    ByteOffset : ULONG;
end;
PMDL = ^MDL;
PPMDL = ^PMDL;

PServiceDescriptorEntry = ^TServiceDescriptorEntry;
TServiceDescriptorEntry = packed record
    ServiceTableBase : PULONG;
    ServiceCounterTableBase : PULONG;
    NumberOfServices : ULONG;
    ParamTableBase : PByte;
end;
SERVICE_DESCRIPTOR_ENTRY = TServiceDescriptorEntry;
PSERVICE_DESCRIPTOR_ENTRY = ^SERVICE_DESCRIPTOR_ENTRY;

PKMutant = ^TKMutant;
TKMutant = packed record
    Header : TDispatcherHeader;
    MutantListEntry : TListEntry;
    OwnerThread : PKThread;
    Abandoned : Boolean;
    ApcDisable : Byte;
    Alignment0 : Word;
end;
TKMutex = TKMutant;
KMutex = TKMutex;
PKMutex = ^TKMutex;

LARGE_INTEGER = packed record
    case Integer of
    0 : (
    LowPart : ULONG;
    HighPart : Longint);
    1 : (
    QuadPart : Int64);
end;
PLARGE_INTEGER = ^LARGE_INTEGER;

ULARGE_INTEGER = packed record
    case Integer of
    0 : (
    LowPart : ULONG;
    HighPart : ULONG);
    1 : (
    QuadPart : Int64);
end;
PULARGE_INTEGER = ^ULARGE_INTEGER;
PIO_TIMER_ROUTINE = procedure(pOurDevice : PDeviceObject; pContext : Pointer); stdcall;

KTIMER = packed record
    Header : DISPATCHER_HEADER;
    DueTime : ULARGE_INTEGER;
    TimerListEntry : LIST_ENTRY;
    Dpc : Pointer;
    Period : LONG;
end;
PKTIMER = ^KTIMER;
PRKTIMER = ^KTIMER;
PKDEFERRED_ROUTINE = procedure(Dpc : KDPC; DeferredContext : Pointer; SystemArgument1 : Pointer; SystemArgument2 : Pointer); stdcall;
PDRIVER_CANCEL = procedure(DeviceObject : PDEVICE_OBJECT; Irp : PIRP); stdcall;

PIO_CSQ = ^IO_CSQ;
PIO_CSQ_INSERT_IRP = procedure(Csq : PIO_CSQ; Irp : PIRP); stdcall;
PIO_CSQ_REMOVE_IRP = procedure(Csq : PIO_CSQ; Irp : PIRP); stdcall;
PIO_CSQ_PEEK_NEXT_IRP = function(Csq : PIO_CSQ; Irp : PIRP; PeekContext : Pointer) : PIRP; stdcall;
PIO_CSQ_ACQUIRE_LOCK = procedure(Csq : PIO_CSQ; Irql : PKIRQL); stdcall;
PIO_CSQ_RELEASE_LOCK = procedure(Csq : PIO_CSQ; Irql : KIRQL); stdcall;
PIO_CSQ_COMPLETE_CANCELED_IRP = procedure(Csq : PIO_CSQ; Irp : PIRP); stdcall;
IO_CSQ = packed record
    _Type : ULONG;
    CsqInsertIrp : PIO_CSQ_INSERT_IRP;
    CsqRemoveIrp : PIO_CSQ_REMOVE_IRP;
    CsqPeekNextIrp : PIO_CSQ_PEEK_NEXT_IRP;
    CsqAcquireLock : PIO_CSQ_ACQUIRE_LOCK;
    CsqReleaseLock : PIO_CSQ_RELEASE_LOCK;
    CsqCompleteCanceledIrp : PIO_CSQ_COMPLETE_CANCELED_IRP;
    ReservePointer : Pointer;
end;

PIO_CSQ_IRP_CONTEXT = ^IO_CSQ_IRP_CONTEXT;
IO_CSQ_IRP_CONTEXT = packed record
    _Type : ULONG;
    IRP : PIRP;
    Csq : PIO_CSQ;
end;

WDFFUNC = Pointer;
PFN_WDF_DRIVER_DEVICE_ADD = function(Driver : WDFDRIVER; DeviceInit : PWDFDEVICE_INIT) : NTSTATUS; stdcall;
PFN_WDF_DRIVER_UNLOAD = procedure(Driver : WDFDRIVER); stdcall;
PWDF_DRIVER_CONFIG = ^WDF_DRIVER_CONFIG;
WDF_DRIVER_CONFIG = packed record
    Size : ULONG;
    EvtDriverDeviceAdd : PFN_WDF_DRIVER_DEVICE_ADD;
    EvtDriverUnload : PFN_WDF_DRIVER_UNLOAD;
    DriverInitFlags : ULONG;
    DriverPoolTag : ULONG;
end;

PWDF_DRIVER_GLOBALS = ^_WDF_DRIVER_GLOBALS;
_WDF_DRIVER_GLOBALS = packed record
    Driver : WDFDRIVER;
    DriverFlags : ULONG;
    DriverTag : ULONG;
    DriverName : array[0..WDF_DRIVER_GLOBALS_NAME_LEN] of BYTE;
    DisplaceDriverUnload : BOOLEAN;
end;

FN_WDFDEVICEINITASSIGNNAME = function(DriverGlobals : PWDF_DRIVER_GLOBALS; DeviceInit : PWDFDEVICE_INIT; DeviceName : PUNICODE_STRING) : NTSTATUS; stdcall;
PFN_WDFDEVICEINITASSIGNNAME = ^FN_WDFDEVICEINITASSIGNNAME;

FN_WDF_OBJECT_CONTEXT_CLEANUP = procedure(Obj : WDFOBJECT); stdcall;
PFN_WDF_OBJECT_CONTEXT_CLEANUP = ^FN_WDF_OBJECT_CONTEXT_CLEANUP;

FN_WDF_OBJECT_CONTEXT_DESTROY = procedure(Obj : WDFOBJECT); stdcall;
PFN_WDF_OBJECT_CONTEXT_DESTROY = ^FN_WDF_OBJECT_CONTEXT_DESTROY;

FN_GET_UNIQUE_CONTEXT_TYPE = procedure; cdecl;
PFN_GET_UNIQUE_CONTEXT_TYPE = ^FN_GET_UNIQUE_CONTEXT_TYPE;

PCWDF_OBJECT_CONTEXT_TYPE_INFO = ^WDF_OBJECT_CONTEXT_TYPE_INFO;
WDF_OBJECT_CONTEXT_TYPE_INFO = packed record
    Size : ULONG;
    ContextName : PCHAR;
    ContextSize : ULONG;
    UniqueType : Pointer;
    EvtDriverGetUniqueContextType : PFN_GET_UNIQUE_CONTEXT_TYPE;
end;

PWDF_OBJECT_ATTRIBUTES = ^WDF_OBJECT_ATTRIBUTES;
WDF_OBJECT_ATTRIBUTES = packed record
    Size : ULONG;
    EvtCleanupCallback : PFN_WDF_OBJECT_CONTEXT_CLEANUP;
    EvtDestroyCallback : PFN_WDF_OBJECT_CONTEXT_DESTROY;
    ExecutionLevel : WDF_EXECUTION_LEVEL;
    SynchronizationScope : WDF_SYNCHRONIZATION_SCOPE;
    ParentObject : WDFOBJECT;
    ContextSizeOverride : ULONG;
    ContextTypeInfo : PCWDF_OBJECT_CONTEXT_TYPE_INFO;
end;

FN_WDFDEVICECREATE = function(DriverGlobals : PWDF_DRIVER_GLOBALS; DeviceInit : PPWDFDEVICE_INIT; DeviceAttributes : PWDF_OBJECT_ATTRIBUTES; Device : PWDFDEVICE) : NTSTATUS; stdcall;
PFN_WDFDEVICECREATE = ^FN_WDFDEVICECREATE;

FN_WDFDRIVERCREATE = function(DriverGlobals : PWDF_DRIVER_GLOBALS; DriverObject : PDRIVER_OBJECT; RegistryPath : PUNICODE_STRING; DriverAttributes : PWDF_OBJECT_ATTRIBUTES; DriverConfig : PWDF_DRIVER_CONFIG; Driver : PWDFDRIVER) : NTSTATUS; stdcall;
PFN_WDFDRIVERCREATE = ^FN_WDFDRIVERCREATE;

FN_WDFDEVICEINITSETIOTYPE = procedure(DriverGlobals : PWDF_DRIVER_GLOBALS; DeviceInit : PWDFDEVICE_INIT; IoType : WDF_DEVICE_IO_TYPE); stdcall;
PFN_WDFDEVICEINITSETIOTYPE = ^FN_WDFDEVICEINITSETIOTYPE;

FN_WDF_DEVICE_FILE_CREATE = procedure(Device : WDFDEVICE; Request : WDFREQUEST; FileObject : WDFFILEOBJECT); stdcall;
PFN_WDF_DEVICE_FILE_CREATE = ^FN_WDF_DEVICE_FILE_CREATE;

FN_WDF_FILE_CLOSE = procedure(FileObject : WDFFILEOBJECT); stdcall;
PFN_WDF_FILE_CLOSE = ^FN_WDF_FILE_CLOSE;

FN_WDF_FILE_CLEANUP = procedure(FileObject : WDFFILEOBJECT); stdcall;
PFN_WDF_FILE_CLEANUP = ^FN_WDF_FILE_CLEANUP;

PWDF_FILEOBJECT_CONFIG = ^WDF_FILEOBJECT_CONFIG;
WDF_FILEOBJECT_CONFIG = packed record
    Size : ULONG;
    EvtDeviceFileCreate : PFN_WDF_DEVICE_FILE_CREATE;
    EvtFileClose : PFN_WDF_FILE_CLOSE;
    EvtFileCleanup : PFN_WDF_FILE_CLEANUP;
    AutoForwardCleanupClose : WDF_TRI_STATE;
    FileObjectClass : WDF_FILEOBJECT_CLASS;
end;

FN_WDFDEVICEINITSETFILEOBJECTCONFIG = procedure(DriverGlobals : PWDF_DRIVER_GLOBALS; DeviceInit : PWDFDEVICE_INIT; FileObjectConfig : PWDF_FILEOBJECT_CONFIG; FileObjectAttributes : PWDF_OBJECT_ATTRIBUTES); stdcall;
PFN_WDFDEVICEINITSETFILEOBJECTCONFIG = ^FN_WDFDEVICEINITSETFILEOBJECTCONFIG;

FN_WDFDEVICECREATESYMBOLICLINK = function(DriverGlobals : PWDF_DRIVER_GLOBALS; Device : WDFDEVICE; SymbolicLinkName : PUNICODE_STRING) : NTSTATUS; stdcall;
PFN_WDFDEVICECREATESYMBOLICLINK = ^FN_WDFDEVICECREATESYMBOLICLINK;

FN_WDF_IO_QUEUE_IO_DEFAULT = procedure(Queue : WDFQUEUE; Request : WDFREQUEST); stdcall;
PFN_WDF_IO_QUEUE_IO_DEFAULT = ^FN_WDF_IO_QUEUE_IO_DEFAULT;

FN_WDF_IO_QUEUE_IO_READ = procedure(Queue : WDFQUEUE; Request : WDFREQUEST; Length : ULONG); stdcall;
PFN_WDF_IO_QUEUE_IO_READ = ^FN_WDF_IO_QUEUE_IO_READ;

FN_WDF_IO_QUEUE_IO_WRITE = procedure(Queue : WDFQUEUE; Request : WDFREQUEST; Length : ULONG); stdcall;
PFN_WDF_IO_QUEUE_IO_WRITE = ^FN_WDF_IO_QUEUE_IO_WRITE;

FN_WDF_IO_QUEUE_IO_DEVICE_CONTROL = procedure(Queue : WDFQUEUE; Request : WDFREQUEST; OutputBufferLength : ULONG; InputBufferLength : ULONG; IoControlCode : ULONG); stdcall;
PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL = ^FN_WDF_IO_QUEUE_IO_DEVICE_CONTROL;

FN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL = procedure(Queue : WDFQUEUE; Request : WDFREQUEST; OutputBufferLength : ULONG; InputBufferLength : ULONG; IoControlCode : ULONG); stdcall;
PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL = ^FN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL;

FN_WDF_IO_QUEUE_IO_STOP = procedure(Queue : WDFQUEUE; Request : WDFREQUEST; ActionFlags : ULONG); stdcall;
PFN_WDF_IO_QUEUE_IO_STOP = ^FN_WDF_IO_QUEUE_IO_STOP;

FN_WDF_IO_QUEUE_IO_RESUME = procedure(Queue : WDFQUEUE; Request : WDFREQUEST); stdcall;
PFN_WDF_IO_QUEUE_IO_RESUME = ^FN_WDF_IO_QUEUE_IO_RESUME;

FN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE = procedure(Queue : WDFQUEUE; Request : WDFREQUEST); stdcall;
PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE = ^FN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE;

_Parallel = packed record
    NumberOfPresentedRequests : LONG;
end;

_Settings = packed record
    case Integer of
    0 : (Parallel : _Parallel);
end;

PWDF_IO_QUEUE_CONFIG = ^WDF_IO_QUEUE_CONFIG;
WDF_IO_QUEUE_CONFIG = packed record
    Size : ULONG;
    DispatchType : WDF_IO_QUEUE_DISPATCH_TYPE;
    PowerManaged : WDF_TRI_STATE;
    AllowZeroLengthRequests : BOOLEAN;
    DefaultQueue : BOOLEAN;
    wdf_io_queue_config_undefined1 : byte;
    wdf_io_queue_config_undefined2 : byte;
    EvtIoDefault : PFN_WDF_IO_QUEUE_IO_DEFAULT;
    EvtIoRead : PFN_WDF_IO_QUEUE_IO_READ;
    EvtIoWrite : PFN_WDF_IO_QUEUE_IO_WRITE;
    EvtIoDeviceControl : PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL;
    EvtIoInternalDeviceControl : PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL;
    EvtIoStop : PFN_WDF_IO_QUEUE_IO_STOP;
    EvtIoResume : PFN_WDF_IO_QUEUE_IO_RESUME;
    EvtIoCanceledOnQueue : PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE;
    Settings : _Settings;
end;

FN_WDFIOQUEUECREATE = function(DriverGlobals : PWDF_DRIVER_GLOBALS; Device : WDFDEVICE; Config : PWDF_IO_QUEUE_CONFIG; QueueAttributes : PWDF_OBJECT_ATTRIBUTES; Queue : PWDFQUEUE) : NTSTATUS; stdcall;
PFN_WDFIOQUEUECREATE = ^FN_WDFIOQUEUECREATE;

FN_WDFREQUESTCOMPLETE = procedure(DriverGlobals : PWDF_DRIVER_GLOBALS; Request : WDFREQUEST; Status : NTSTATUS); stdcall;
PFN_WDFREQUESTCOMPLETE = ^FN_WDFREQUESTCOMPLETE;

FN_WDFREQUESTCOMPLETEWITHINFORMATION = procedure(DriverGlobals : PWDF_DRIVER_GLOBALS; Request : WDFREQUEST; Status : NTSTATUS; Information : ULONG); stdcall;
PFN_WDFREQUESTCOMPLETEWITHINFORMATION = ^FN_WDFREQUESTCOMPLETEWITHINFORMATION;

FN_WDFREQUESTRETRIEVEOUTPUTMEMORY = function(DriverGlobals : PWDF_DRIVER_GLOBALS; Request : WDFREQUEST; Memory : PWDFMEMORY) : NTSTATUS; stdcall;
PFN_WDFREQUESTRETRIEVEOUTPUTMEMORY = ^FN_WDFREQUESTRETRIEVEOUTPUTMEMORY;

FN_WDFMEMORYCOPYFROMBUFFER = function(DriverGlobals : PWDF_DRIVER_GLOBALS; DestinationMemory : WDFMEMORY; DestinationOffset : ULONG; Buffer : Pointer; NumBytesToCopyFrom : ULONG) : NTSTATUS; stdcall;
PFN_WDFMEMORYCOPYFROMBUFFER = ^FN_WDFMEMORYCOPYFROMBUFFER;

FN_WDFREQUESTRETRIEVEINPUTMEMORY = function(DriverGlobals : PWDF_DRIVER_GLOBALS; Request : WDFREQUEST; Memory : PWDFMEMORY) : NTSTATUS; stdcall;
PFN_WDFREQUESTRETRIEVEINPUTMEMORY =^FN_WDFREQUESTRETRIEVEINPUTMEMORY;

FN_WDFMEMORYCOPYTOBUFFER = function(DriverGlobals : PWDF_DRIVER_GLOBALS; SourceMemory : WDFMEMORY; SourceOffset : ULONG; Buffer : Pointer; NumBytesToCopyTo : ULONG) : NTSTATUS; stdcall;
PFN_WDFMEMORYCOPYTOBUFFER = ^FN_WDFMEMORYCOPYTOBUFFER;

FN_WDFREQUESTRETRIEVEOUTPUTWDMMDL = function(DriverGlobals : PWDF_DRIVER_GLOBALS; Request : WDFREQUEST; Mdl : PPMDL) : NTSTATUS; stdcall;
PFN_WDFREQUESTRETRIEVEOUTPUTWDMMDL = ^FN_WDFREQUESTRETRIEVEOUTPUTWDMMDL;

FN_WDFREQUESTRETRIEVEINPUTWDMMDL = function(DriverGlobals : PWDF_DRIVER_GLOBALS; Request : WDFREQUEST; Mdl : PPMDL) : NTSTATUS; stdcall;
PFN_WDFREQUESTRETRIEVEINPUTWDMMDL = ^FN_WDFREQUESTRETRIEVEINPUTWDMMDL;


procedure IoCompleteRequest(Irp : PIrp; PriorityBoost : Integer); stdcall; 
procedure IoDeleteDevice(DeviceObject : PDeviceObject); stdcall; 
procedure RtlInitUnicodeString(DestinationString : PUnicodeString; SourceString : PWideChar); stdcall;
procedure ProbeForRead(Address : Pointer; Length : Cardinal; Alignment : Cardinal); stdcall; 
procedure ExFreePool(P : Pointer); stdcall; 
procedure KeInitializeMutex(Mutex : PKMutex; Level : Cardinal); stdcall; 
procedure memset(dst  : Pointer; val  : Char; len  : ULONG); stdcall;
procedure memcpy(dst  : Pointer; src  : Pointer; len  : ULONG); stdcall;
procedure krnlObDereferenceObject(handle : Handle); stdcall; external NtKernel name 'ObDereferenceObject';
procedure ObDereferenceObject(handle : Handle); stdcall;
procedure krnlIoStartTimer(DeviceObject : PDeviceObject); stdcall; external NtKernel name 'IoStartTimer';
procedure IoStartTimer(DeviceObject : PDeviceObject); stdcall;
procedure krnlIoStopTimer(DeviceObject : PDeviceObject); stdcall; external NtKernel name 'IoStopTimer';
procedure IoStopTimer(DeviceObject : PDeviceObject); stdcall;
procedure krnlKeInitializeTimer(Timer : PKTIMER); stdcall; external NtKernel name 'KeInitializeTimer';
procedure KeInitializeTimer(Timer : PKTIMER); stdcall;
procedure krnlKeInitializeDpc(Dpc : PRKDPC; DeferredRoutine : PKDEFERRED_ROUTINE; DeferredContext : Pointer); stdcall; external NtKernel name 'KeInitializeDpc';
procedure KeInitializeDpc(Dpc : PRKDPC; DeferredRoutine : PKDEFERRED_ROUTINE; DeferredContext : Pointer); stdcall;
procedure IoMarkIrpPending(pIrp : PIRP); stdcall;
procedure krnlIoStartPacket(DeviceObject : PDeviceObject; Irp : PIRP; Key : PULONG; CancelFunction : PDRIVER_CANCEL); stdcall; external NtKernel name 'IoStartPacket';
procedure IoStartPacket(DeviceObject : PDeviceObject; Irp : PIRP; Key : PULONG; CancelFunction : PDRIVER_CANCEL); stdcall;
procedure krnlIoStartNextPacket(DeviceObject : PDeviceObject; Cancelable : BOOLEAN); stdcall; external NtKernel name 'IoStartNextPacket';
procedure IoStartNextPacket(DeviceObject : PDeviceObject; Cancelable : BOOLEAN); stdcall;
procedure InitializeListHead(ListHead : PLIST_ENTRY); stdcall;
procedure InsertHeadList(ListHead : PLIST_ENTRY; Entry : PLIST_ENTRY); stdcall;
procedure InsertTailList(ListHead : PLIST_ENTRY; Entry : PLIST_ENTRY); stdcall;
function KfAcquireSpinLock(SpinLock : PKSPIN_LOCK) : KIRQL; stdcall;
procedure KefReleaseSpinLockFromDpcLevel(SpinLock : PKSPIN_LOCK); stdcall;
procedure KfReleaseSpinLock(SpinLock : PKSPIN_LOCK; irql : KIRQL); stdcall;
procedure IoCsqInsertIrp(Csq : PIO_CSQ; Irp : PIRP; Context : PIO_CSQ_IRP_CONTEXT); stdcall;
procedure KeInitializeSpinLock(SpinLock : PKSPIN_LOCK); stdcall;
procedure IoDetachDevice(TargetDevice : PDEVICE_OBJECT); stdcall;
procedure IoSkipCurrentIrpStackLocation(Irp : PIRP); stdcall;
procedure RtlZeroMemory(ptr : Pointer; size : ULONG); stdcall;
procedure WDF_DRIVER_CONFIG_INIT(Config : PWDF_DRIVER_CONFIG; EvtDriverDeviceAdd : PFN_WDF_DRIVER_DEVICE_ADD); stdcall;
procedure WdfDeviceInitSetIoType(DeviceInit : PWDFDEVICE_INIT; IoType : WDF_DEVICE_IO_TYPE); stdcall;
procedure WDF_FILEOBJECT_CONFIG_INIT(FileEventCallbacks : PWDF_FILEOBJECT_CONFIG; EvtDeviceFileCreate : PFN_WDF_DEVICE_FILE_CREATE; EvtFileClose : PFN_WDF_FILE_CLOSE; EvtFileCleanup : PFN_WDF_FILE_CLEANUP); stdcall;
procedure WdfDeviceInitSetFileObjectConfig(DeviceInit : PWDFDEVICE_INIT; FileObjectConfig : PWDF_FILEOBJECT_CONFIG; FileObjectAttributes : PWDF_OBJECT_ATTRIBUTES); stdcall;
procedure WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(Config : PWDF_IO_QUEUE_CONFIG; DispatchType : WDF_IO_QUEUE_DISPATCH_TYPE); stdcall;
procedure WdfRequestComplete(Request : WDFREQUEST; Status : NTSTATUS); stdcall;
procedure WdfRequestCompleteWithInformation(Request : WDFREQUEST; Status : NTSTATUS; Information : ULONG); stdcall;
procedure KeRaiseIrql(newIrql : KIRQL; oldIrql : PKIRQL); stdcall;
procedure KeAcquireSpinLock(SpinLock : PKSPIN_LOCK; oldIrql : PKIRQL); stdcall;
procedure KeReleaseSpinLock(SpinLock : PKSPIN_LOCK; oldIrql : KIRQL); stdcall;
procedure KeInitializeEvent(Event : PRKEVENT; nType : EVENT_TYPE; State : BOOLEAN); stdcall;
procedure KeInitializeSemaphore(Semaphore : PRKSEMAPHORE; Count : LONG; Limit : LONG); stdcall;

function KeServiceDescriptorTable : PServiceDescriptorEntry; 
function IoCreateDevice(DriverObject : PDriverObject; DeviceExtensionSize : Cardinal; DeviceName : PUnicodeString; DeviceType : TDeviceType; DeviceCharacteristics : Cardinal; Reserved : Boolean; var DeviceObject : PDeviceObject) : NTSTATUS; stdcall;
function IoCreateSymbolicLink(SymbolicLinkName : PUnicodeString; DeviceName : PUnicodeString) : NTSTATUS; stdcall;
function IoDeleteSymbolicLink(SymbolicLinkName : PUnicodeString) : NTSTATUS; stdcall;
function InterlockedExchange(Target : PLONG; Value : LONG) : LONG; stdcall; 
function ZwOpenProcess(ProcessHandle : PHandle; DesiredAccess : TAccessMask; ObjectAttributes : PObjectAttributes; ClientId : PClientId) : NTSTATUS; stdcall; 
function KeWaitForSingleObject(SObject : Pointer; WaitReason : TKWaitReason; WaitMode : TKProcessorMode; Alertable : LongBool; Timeout : PLargeInteger) : NTSTATUS; stdcall; 
function KeWaitForMutexObject(SObject : Pointer; WaitReason : TKWaitReason; WaitMode : TKProcessorMode; Alertable : LongBool; Timeout : PLargeInteger) : NTSTATUS; stdcall; 
function ExAllocatePool(PoolType : TPoolType; NumberOfBytes : Cardinal) : Pointer; stdcall; 
function ExAllocatePoolWithQuota(PoolType : TPoolType; NumberOfBytes : Cardinal) : Pointer; stdcall;
function ExAllocatePoolWithTag(PoolType : TPoolType; NumberOfBytes : Cardinal; Tag : ULONG) : Pointer; stdcall;
function KeReleaseMutex(Mutex : PKMutex; Wait : LongBool) : LONG; stdcall; 
function IoGetCurrentIrpStackLocation(pIrp : PIRP) : PIoStackLocation; stdcall;
function ZwOpenProcessAddr : Pointer;
function NT_SUCCESS(AStatus : NTSTATUS) : Boolean;
function DbgPrint(Format : PChar; Args : array of const) : NTSTATUS; stdcall;
function DbgMsg(Format : PChar; Args : array of const) : NTSTATUS; stdcall;
function MmGetMdlByteCount(pMDL : PMDL) : ULONG; stdcall;
function MmGetSystemAddressForMdlSafe(pMDL : PMDL; Priority : ULONG) : Pointer; stdcall;
function MmGetMdlVirtualAddress(pMDL : PMDL) : Pointer; stdcall;
function strlen(src : Pointer) : ULONG; stdcall;
function PsTerminateSystemThread(ExitStatus : NTSTATUS) : NTSTATUS; stdcall;
function PsCreateSystemThread(ThreadHandle : PHandle; DesiredAccess : ULONG; ObjectAttributes : Pointer; ProcessHandle : Handle; ClientId : Pointer; StartRoutine : PKSTART_ROUTINE; StartContext : Pointer) : NTSTATUS; stdcall;
function ObReferenceObjectByHandle(Handle : Handle; DesiredAccess : ACCESS_MASK; ObjectType : Pointer; AccessMode : KPROCESSOR_MODE; _Object : PPointer; HandleInformation : POBJECT_HANDLE_INFORMATION) : NTSTATUS; stdcall;
function ZwClose(Handle : Handle) : NTSTATUS; stdcall;
function IoGetCurrentProcess : Pointer; stdcall;
function KeDelayExecutionThread(WaitMode : KPROCESSOR_MODE; Alertable : Boolean; Interval : PLARGE_INTEGER) : NTSTATUS; stdcall;
function IoInitializeTimer(DeviceObject : PDeviceObject; TimerRoutine : PIO_TIMER_ROUTINE; Context : Pointer) : NTSTATUS; stdcall;
function krnlKeSetTimerEx(Timer : PKTIMER; LowPart : ULONG; HighPart : ULONG; Period : LONG; Dpc : PKDPC) : Boolean; stdcall; external NtKernel name 'KeSetTimerEx';
function KeSetTimerEx(Timer : PKTIMER; LowPart : ULONG; HighPart : ULONG; Period : LONG; Dpc : PKDPC) : Boolean; stdcall;
function krnlKeCancelTimer(Arg1 : PKTIMER) : Boolean; stdcall; external NtKernel name 'KeCancelTimer';
function KeCancelTimer(Arg1 : PKTIMER) : Boolean; stdcall;
function krnlKeSetTimer(Timer : PKTIMER; LowPart : ULONG; HighPart : ULONG; Dpc : PKDPC) : Boolean; stdcall; external NtKernel name 'KeSetTimer';
function KeSetTimer(Timer : PKTIMER; LowPart : ULONG; HighPart : ULONG; Dpc : PKDPC) : Boolean; stdcall;
function IsListEmpty(ListHead : PLIST_ENTRY) : Boolean; stdcall;
function RemoveEntryList(Entry : PLIST_ENTRY) : Boolean; stdcall;
function RemoveHeadList(ListHead : PLIST_ENTRY) : PLIST_ENTRY; stdcall;
function RemoveTailList(ListHead : PLIST_ENTRY) : PLIST_ENTRY; stdcall;
function IoCsqInitialize(Csq : PIO_CSQ; CsqInsertIrp : PIO_CSQ_INSERT_IRP; CsqRemoveIrp : PIO_CSQ_REMOVE_IRP; CsqPeekNextIrp : PIO_CSQ_PEEK_NEXT_IRP; CsqAcquireLock : PIO_CSQ_ACQUIRE_LOCK; CsqReleaseLock : PIO_CSQ_RELEASE_LOCK; CsqCompleteCanceledIrp : PIO_CSQ_COMPLETE_CANCELED_IRP) : NTSTATUS; stdcall;
function IoCallDriver(DeviceObject : PDEVICE_OBJECT; Irp : PIRP) : NTSTATUS; stdcall;
function IoAttachDeviceToDeviceStack(SourceDevice : PDEVICE_OBJECT; TargetDevice : PDEVICE_OBJECT) : PDEVICE_OBJECT; stdcall;
function WdfDeviceInitAssignName(DeviceInit : PWDFDEVICE_INIT; DeviceName : PUNICODE_STRING) : NTSTATUS; stdcall; assembler
function WdfDeviceCreate(DeviceInit : PPWDFDEVICE_INIT; DeviceAttributes : PWDF_OBJECT_ATTRIBUTES; Device : PWDFDEVICE) : NTSTATUS; stdcall; assembler
function WdfDriverCreate(DriverObject : PDRIVER_OBJECT; RegistryPath : PUNICODE_STRING; DriverAttributes : PWDF_OBJECT_ATTRIBUTES; DriverConfig : PWDF_DRIVER_CONFIG; Driver : PWDFDRIVER) : NTSTATUS; stdcall; assembler
function WdfDeviceCreateSymbolicLink(Device : WDFDEVICE; SymbolicLinkName : PUNICODE_STRING) : NTSTATUS; stdcall;
function WdfIoQueueCreate(Device : WDFDEVICE; Config : PWDF_IO_QUEUE_CONFIG; QueueAttributes : PWDF_OBJECT_ATTRIBUTES; Queue : PWDFQUEUE) : NTSTATUS; stdcall;
function WdfRequestRetrieveOutputMemory(Request : WDFREQUEST; Memory : PWDFMEMORY) : NTSTATUS; stdcall;
function WdfMemoryCopyFromBuffer(DestinationMemory : WDFMEMORY; DestinationOffset : ULONG; Buffer : Pointer; NumBytesToCopyFrom : ULONG) : NTSTATUS; stdcall;
function WdfRequestRetrieveInputMemory(Request : WDFREQUEST; Memory : PWDFMEMORY) : NTSTATUS; stdcall;
function WdfMemoryCopyToBuffer(SourceMemory : WDFMEMORY; SourceOffset : ULONG; Buffer : Pointer; NumBytesToCopyTo : ULONG) : NTSTATUS; stdcall;
function WdfRequestRetrieveOutputWdmMdl(Request : WDFREQUEST; Mdl : PPMDL) : NTSTATUS; stdcall;
function WdfRequestRetrieveInputWdmMdl(Request : WDFREQUEST; Mdl : PPMDL) : NTSTATUS; stdcall;
function KeSetEvent(Event : PRKEVENT; Increment : KPRIORITY; Wait : BOOLEAN) : LONG; stdcall;
function KeReadStateSemaphore(Semaphore : PRKSEMAPHORE) : LONG; stdcall;
function KeReleaseSemaphore(Semaphore : PRKSEMAPHORE; Increment : KPRIORITY; Adjustment : LONG; Wait : BOOLEAN) : LONG; stdcall;

implementation
var
    WdfFunctions : WDFFUNC;
    WdfDriverGlobals : PWDF_DRIVER_GLOBALS;
    
procedure krnlIoCompleteRequest(Irp : PIrp; PriorityBoost : Integer); stdcall; external NtKernel name 'IoCompleteRequest';
procedure krnlIoDeleteDevice(DeviceObject : PDeviceObject); stdcall; external NtKernel name 'IoDeleteDevice';
procedure krnlRtlInitUnicodeString(DestinationString : PUnicodeString; SourceString : PWideChar); stdcall; external NtKernel name 'RtlInitUnicodeString';
procedure krnlKeServiceDescriptorTable; external NtKernel name 'KeServiceDescriptorTable';
procedure krnlProbeForRead(Address : Pointer; Length : Cardinal; Alignment : Cardinal); stdcall; external NtKernel name 'ProbeForRead';
procedure krnlExFreePool(P : Pointer); stdcall; external NtKernel name 'ExFreePool';
procedure krnlKeInitializeMutex(Mutex : PKMutex; Level : Cardinal); stdcall; external NtKernel name 'KeInitializeMutex';
function krnlKfAcquireSpinLock(SpinLock : PKSPIN_LOCK) : KIRQL; register; external NtKernel name 'KfAcquireSpinLock';
procedure krnlKefReleaseSpinLockFromDpcLevel(SpinLock : PKSPIN_LOCK); register; external NtKernel name 'KefReleaseSpinLockFromDpcLevel';
procedure krnlKfReleaseSpinLock(SpinLock : PKSPIN_LOCK; irql : KIRQL); register; external NtKernel name 'KfReleaseSpinLock';
procedure krnlIoCsqInsertIrp(Csq : PIO_CSQ; Irp : PIRP; Context : PIO_CSQ_IRP_CONTEXT); stdcall; external NtKernel name 'IoCsqInsertIrp';
procedure krnlKeInitializeSpinLock(SpinLock : PKSPIN_LOCK); stdcall; external NtKernel name 'KeInitializeSpinLock';
procedure krnlIoDetachDevice(TargetDevice : PDEVICE_OBJECT); stdcall; external NtKernel name 'IoDetachDevice';
procedure krnlRtlZeroMemory(ptr : Pointer; size : ULONG); stdcall; external NtKernel name 'RtlZeroMemory';
procedure krnlWdfFunctions; external NtKernel name 'WdfFunctions';
procedure krnlWdfDriverGlobals; external NtKernel name 'WdfDriverGlobals';
procedure krnlKeRaiseIrql(newIrql : KIRQL; oldIrql : PKIRQL); stdcall; external NtKernel name 'KeRaiseIrql';
procedure krnlKeAcquireSpinLock(SpinLock : PKSPIN_LOCK; oldIrql : PKIRQL); stdcall; external NtKernel name 'KeAcquireSpinLock';
procedure krnlKeReleaseSpinLock(SpinLock : PKSPIN_LOCK; oldIrql : KIRQL); stdcall; external NtKernel name 'KeReleaseSpinLock';
procedure krnlKeInitializeEvent(Event : PRKEVENT; nType : EVENT_TYPE; State : BOOLEAN); stdcall; external NtKernel name 'KeInitializeEvent';
procedure krnlKeInitializeSemaphore(Semaphore : PRKSEMAPHORE; Count : LONG; Limit : LONG); stdcall; external NtKernel name 'KeInitializeSemaphore';

function krnlKeReadStateSemaphore(Semaphore : PRKSEMAPHORE) : LONG; stdcall; external NtKernel name 'KeReadStateSemaphore';
function krnlKeReleaseSemaphore(Semaphore : PRKSEMAPHORE; Increment : KPRIORITY; Adjustment : LONG; Wait : BOOLEAN) : LONG stdcall; external NtKernel name 'KeReleaseSemaphore';
function krnlKeSetEvent(Event : PRKEVENT; Increment : KPRIORITY; Wait : BOOLEAN) : LONG; stdcall; external NtKernel name 'KeSetEvent';
function krnlDbgPrint(Format : PChar; Args : array of const) : NTSTATUS; stdcall; external NtKernel name 'DbgPrint';
function krnlIoCreateDevice(DriverObject : PDriverObject; DeviceExtensionSize : Cardinal; DeviceName : PUnicodeString; DeviceType : TDeviceType; DeviceCharacteristics : Cardinal; Reserved : Boolean; var DeviceObject : PDeviceObject) : NTSTATUS; stdcall; external NtKernel name 'IoCreateDevice';
function krnlIoCreateSymbolicLink(SymbolicLinkName : PUnicodeString; DeviceName : PUnicodeString) : NTSTATUS; stdcall; external NtKernel name 'IoCreateSymbolicLink';
function krnlIoDeleteSymbolicLink(SymbolicLinkName : PUnicodeString) : NTSTATUS; stdcall; external NtKernel name 'IoDeleteSymbolicLink';
function krnlInterlockedExchange(Target : PLONG; Value : LONG) : LONG; register; external NtKernel name 'InterlockedExchange';
function krnlZwOpenProcess(ProcessHandle : PHandle; DesiredAccess : TAccessMask; ObjectAttributes : PObjectAttributes; ClientId : PClientId) : NTSTATUS; stdcall; external NtKernel name 'ZwOpenProcess';
function krnlKeWaitForSingleObject(SObject : Pointer; WaitReason : TKWaitReason; WaitMode : TKProcessorMode; Alertable : LongBool; Timeout : PLargeInteger) : NTSTATUS; stdcall; external NtKernel name 'KeWaitForSingleObject';
function krnlExAllocatePool(PoolType : TPoolType; NumberOfBytes : Cardinal) : Pointer; stdcall; external NtKernel name 'ExAllocatePool';
function krnlExAllocatePoolWithQuota(PoolType : TPoolType; NumberOfBytes : Cardinal) : Pointer; stdcall; external NtKernel name 'ExAllocatePoolWithQuota';
function krnlExAllocatePoolWithTag(PoolType : TPoolType; NumberOfBytes : Cardinal; Tag : ULONG) : Pointer; stdcall; external NtKernel name 'ExAllocatePoolWithTag';
function krnlKeReleaseMutex(Mutex : PKMutex; Wait : LongBool) : LONG; stdcall; external NtKernel name 'KeReleaseMutex';
function krnlPsTerminateSystemThread(ExitStatus : NTSTATUS) : NTSTATUS; stdcall; external NtKernel name 'PsTerminateSystemThread';
function krnlPsCreateSystemThread(ThreadHandle : PHandle; DesiredAccess : ULONG; ObjectAttributes : Pointer; ProcessHandle : Handle; ClientId : Pointer; StartRoutine : PKSTART_ROUTINE; StartContext : Pointer) : NTSTATUS; stdcall; external NtKernel name 'PsCreateSystemThread';
function krnlObReferenceObjectByHandle(Handle : Handle; DesiredAccess : ACCESS_MASK; ObjectType : Pointer; AccessMode : ULONG; _Object : PPointer; HandleInformation : POBJECT_HANDLE_INFORMATION) : NTSTATUS; stdcall; external NtKernel name 'ObReferenceObjectByHandle';
function krnlZwClose(Handle : Handle) : NTSTATUS; stdcall; external NtKernel name 'ZwClose';
function krnlIoGetCurrentProcess : Pointer; stdcall; external NtKernel name 'IoGetCurrentProcess';
function krnlKeDelayExecutionThread(WaitMode : KPROCESSOR_MODE; Alertable : Boolean; Interval : PLARGE_INTEGER) : NTSTATUS; stdcall; external NtKernel name 'KeDelayExecutionThread';
function krnlIoInitializeTimer(DeviceObject : PDeviceObject; TimerRoutine : PIO_TIMER_ROUTINE; Context : Pointer) : NTSTATUS; stdcall; external NtKernel name 'IoInitializeTimer';
function krnlIoCsqInitialize(Csq : PIO_CSQ; CsqInsertIrp : PIO_CSQ_INSERT_IRP; CsqRemoveIrp : PIO_CSQ_REMOVE_IRP; CsqPeekNextIrp : PIO_CSQ_PEEK_NEXT_IRP; CsqAcquireLock : PIO_CSQ_ACQUIRE_LOCK; CsqReleaseLock : PIO_CSQ_RELEASE_LOCK; CsqCompleteCanceledIrp : PIO_CSQ_COMPLETE_CANCELED_IRP) : NTSTATUS; stdcall; external NtKernel name 'IoCsqInitialize';
function krnlIoCallDriver(DeviceObject : PDEVICE_OBJECT; Irp : PIRP) : NTSTATUS; stdcall; external NtKernel name 'IoCallDriver';
function krnlIoAttachDeviceToDeviceStack(SourceDevice : PDEVICE_OBJECT; TargetDevice : PDEVICE_OBJECT) : PDEVICE_OBJECT; stdcall; external NtKernel name 'IoAttachDeviceToDeviceStack';
function MmMapLockedPagesSpecifyCache(MemoryDescriptorList : PMDL; AccessMode : ULONG; CacheType : ULONG; RequestedAddress : POINTER; BugCheckOnFailure : ULONG; Priority : ULONG) : POINTER; stdcall; external NtKernel name 'MmMapLockedPagesSpecifyCache';

procedure WdfRequestCompleteWithInformation(Request : WDFREQUEST; Status : NTSTATUS; Information : ULONG); stdcall;
var
    func : FN_WDFREQUESTCOMPLETEWITHINFORMATION;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfRequestCompleteWithInformationTableIndex * sizeof(ULONG))^;
    func(WdfDriverGlobals, Request, Status, Information);
end;

procedure WdfRequestComplete(Request : WDFREQUEST; Status : NTSTATUS); stdcall;
var
    func : FN_WDFREQUESTCOMPLETE;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfRequestCompleteTableIndex * sizeof(ULONG))^;
    func(WdfDriverGlobals, Request, Status);
end;

procedure WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(Config : PWDF_IO_QUEUE_CONFIG; DispatchType : WDF_IO_QUEUE_DISPATCH_TYPE); stdcall;
begin
    RtlZeroMemory(Config, sizeof(WDF_IO_QUEUE_CONFIG));
    Config^.Size := sizeof(WDF_IO_QUEUE_CONFIG);
    Config^.PowerManaged := WdfUseDefault;
    Config^.DefaultQueue := TRUE;
    Config^.DispatchType := DispatchType;
    if DispatchType = WdfIoQueueDispatchParallel then 
    begin
    Config^.Settings.Parallel.NumberOfPresentedRequests := -1;
    end;
end;

procedure WdfDeviceInitSetFileObjectConfig(DeviceInit : PWDFDEVICE_INIT; FileObjectConfig : PWDF_FILEOBJECT_CONFIG; FileObjectAttributes : PWDF_OBJECT_ATTRIBUTES); stdcall;
var
    func : FN_WDFDEVICEINITSETFILEOBJECTCONFIG;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfDeviceInitSetFileObjectConfigTableIndex * sizeof(ULONG))^;
    func(WdfDriverGlobals, DeviceInit, FileObjectConfig, FileObjectAttributes);
end;

procedure WDF_FILEOBJECT_CONFIG_INIT(FileEventCallbacks : PWDF_FILEOBJECT_CONFIG; EvtDeviceFileCreate : PFN_WDF_DEVICE_FILE_CREATE; EvtFileClose : PFN_WDF_FILE_CLOSE; EvtFileCleanup : PFN_WDF_FILE_CLEANUP); stdcall;
begin
    FileEventCallbacks^.Size := sizeof(WDF_FILEOBJECT_CONFIG);
    FileEventCallbacks^.EvtDeviceFileCreate := EvtDeviceFileCreate;
    FileEventCallbacks^.EvtFileClose := EvtFileClose;
    FileEventCallbacks^.EvtFileCleanup := EvtFileCleanup;
    FileEventCallbacks^.FileObjectClass := WdfFileObjectWdfCannotUseFsContexts;
    FileEventCallbacks^.AutoForwardCleanupClose := WdfUseDefault;
end;

procedure WdfDeviceInitSetIoType(DeviceInit : PWDFDEVICE_INIT; IoType : WDF_DEVICE_IO_TYPE); stdcall;
var
    func : FN_WDFDEVICEINITSETIOTYPE;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfDeviceInitSetIoTypeTableIndex * sizeof(ULONG))^;
    func(WdfDriverGlobals, DeviceInit, IoType);
end;

procedure RtlZeroMemory(ptr : Pointer; size : ULONG); stdcall;
begin
    krnlRtlZeroMemory(ptr, size);
end;

procedure WDF_DRIVER_CONFIG_INIT(Config : PWDF_DRIVER_CONFIG; EvtDriverDeviceAdd : PFN_WDF_DRIVER_DEVICE_ADD); stdcall;
begin
    RtlZeroMemory(Config, sizeof(WDF_DRIVER_CONFIG));
    Config^.Size := sizeof(WDF_DRIVER_CONFIG);
    Config^.EvtDriverDeviceAdd := EvtDriverDeviceAdd;
end;

procedure IoSkipCurrentIrpStackLocation(Irp : PIRP); stdcall;
begin
    Irp^.CurrentLocation := Irp^.CurrentLocation + 1;
    Irp^.Tail.Overlay.s1.u1.CurrentStackLocation := Pointer(Integer(Irp^.Tail.Overlay.s1.u1.CurrentStackLocation) + 36);
end;

procedure IoDetachDevice(TargetDevice : PDEVICE_OBJECT); stdcall;
begin
    krnlIoDetachDevice(TargetDevice);
end;

procedure KeInitializeSpinLock(SpinLock : PKSPIN_LOCK); stdcall;
begin
    krnlKeInitializeSpinLock(SpinLock);
end;

procedure IoCsqInsertIrp(Csq : PIO_CSQ; Irp : PIRP; Context : PIO_CSQ_IRP_CONTEXT); stdcall;
begin
    krnlIoCsqInsertIrp(Csq, Irp, Context);
end;

function KfAcquireSpinLock(SpinLock : PKSPIN_LOCK) : KIRQL; stdcall;
begin
    Result := krnlKfAcquireSpinLock(SpinLock);
end;

procedure KfReleaseSpinLock(SpinLock : PKSPIN_LOCK; irql : KIRQL); stdcall;
begin
    krnlKfReleaseSpinLock(SpinLock, irql);
end;

procedure KefReleaseSpinLockFromDpcLevel(SpinLock : PKSPIN_LOCK); stdcall;
begin
    krnlKefReleaseSpinLockFromDpcLevel(SpinLock);
end;

procedure InsertTailList(ListHead : PLIST_ENTRY; Entry : PLIST_ENTRY); stdcall;
var
    _EX_Blink : PLIST_ENTRY;
    _EX_ListHead : PLIST_ENTRY;
    
begin
    _EX_ListHead := Pointer(ListHead);
    _EX_Blink := Pointer(_EX_ListHead^.Blink);
    Entry^.Flink := Pointer(_EX_ListHead);
    Entry^.Blink := Pointer(_EX_Blink);
    _EX_Blink^.Flink := Pointer(Entry);
    _EX_ListHead^.Blink := Pointer(Entry);
end;

procedure InsertHeadList(ListHead : PLIST_ENTRY; Entry : PLIST_ENTRY); stdcall;
var
    _EX_Flink : PLIST_ENTRY;
    _EX_ListHead : PLIST_ENTRY;
    
begin
    _EX_ListHead := Pointer(ListHead);
    _EX_Flink := Pointer(_EX_ListHead^.Flink);
    Entry^.Flink := Pointer(_EX_Flink);
    Entry^.Blink := Pointer(_EX_ListHead);
    _EX_Flink^.Blink := Pointer(Entry);
    _EX_ListHead^.Flink := Pointer(Entry);
end;

procedure InitializeListHead(ListHead : PLIST_ENTRY); stdcall;
begin
    ListHead.Flink := Pointer(ListHead);
    ListHead.Blink := Pointer(ListHead);
end;

procedure IoStartNextPacket(DeviceObject : PDeviceObject; Cancelable : BOOLEAN); stdcall;
begin
    krnlIoStartNextPacket(DeviceObject, Cancelable);
end;

procedure IoStartPacket(DeviceObject : PDeviceObject; Irp : PIRP; Key : PULONG; CancelFunction : PDRIVER_CANCEL); stdcall;
begin
    krnlIoStartPacket(DeviceObject, Irp, Key, CancelFunction);
end;

procedure IoMarkIrpPending(pIrp : PIRP); stdcall;
var
    psk : PIoStackLocation;
    
begin
    psk := IoGetCurrentIrpStackLocation(pIrp);
    psk^.Control := psk^.Control or SL_PENDING_RETURNED;
end;

procedure KeInitializeDpc(Dpc : PRKDPC; DeferredRoutine : PKDEFERRED_ROUTINE; DeferredContext : Pointer); stdcall;
begin
    krnlKeInitializeDpc(Dpc, DeferredRoutine, DeferredContext);
end;

procedure KeInitializeTimer(Timer : PKTIMER); stdcall;
begin
    krnlKeInitializeTimer(Timer);
end;

procedure IoStartTimer(DeviceObject : PDeviceObject); stdcall;
begin
    krnlIoStartTimer(DeviceObject);
end;

procedure IoStopTimer(DeviceObject : PDeviceObject); stdcall;
begin
    krnlIoStopTimer(DeviceObject);
end;

procedure IoDeleteDevice(DeviceObject : PDeviceObject); stdcall; 
begin 
    krnlIoDeleteDevice(DeviceObject); 
end; 

procedure RtlInitUnicodeString(DestinationString : PUnicodeString; SourceString : PWideChar); stdcall; 
begin 
    krnlRtlInitUnicodeString(DestinationString, SourceString); 
end;

procedure ProbeForRead(Address : Pointer; Length : Cardinal; Alignment : Cardinal); stdcall; 
begin
    krnlProbeForRead(Address, Length, Alignment); 
end;

procedure ExFreePool(P : Pointer); stdcall; 
begin 
    krnlExFreePool(P); 
end;

procedure KeInitializeMutex(Mutex : PKMutex; Level : Cardinal); stdcall; 
begin 
    krnlKeInitializeMutex(Mutex, Level); 
end;

procedure IoCompleteRequest(Irp : PIrp; PriorityBoost : Integer); stdcall; 
begin 
    krnlIoCompleteRequest(Irp, PriorityBoost); 
end;

procedure ObDereferenceObject(handle : Handle); stdcall;
begin
    krnlObDereferenceObject(handle);
end;

procedure KeRaiseIrql(newIrql : KIRQL; oldIrql : PKIRQL); stdcall;
begin
    krnlKeRaiseIrql(newIrql, oldIrql);
end;

procedure KeAcquireSpinLock(SpinLock : PKSPIN_LOCK; oldIrql : PKIRQL); stdcall;
begin
    krnlKeAcquireSpinLock(SpinLock, oldIrql);
end;

procedure KeReleaseSpinLock(SpinLock : PKSPIN_LOCK; oldIrql : KIRQL); stdcall;
begin
    krnlKeReleaseSpinLock(SpinLock, oldIrql);
end;

procedure KeInitializeEvent(Event : PRKEVENT; nType : EVENT_TYPE; State : BOOLEAN); stdcall;
begin
    krnlKeInitializeEvent(Event, nType, State);
end;

procedure KeInitializeSemaphore(Semaphore : PRKSEMAPHORE; Count : LONG; Limit : LONG); stdcall;
begin
    krnlKeInitializeSemaphore(Semaphore, Count, Limit);
end;

function WdfDriverCreate(DriverObject : PDRIVER_OBJECT; RegistryPath : PUNICODE_STRING; DriverAttributes : PWDF_OBJECT_ATTRIBUTES; DriverConfig : PWDF_DRIVER_CONFIG; Driver : PWDFDRIVER) : NTSTATUS; stdcall;
var
    func : FN_WDFDRIVERCREATE;
    
begin
    WdfFunctions := Pointer(PPointer(@krnlWdfFunctions));
    WdfDriverGlobals := PPointer(@krnlWdfDriverGlobals)^; 

    func := PPointer(Integer(WdfFunctions) + WdfDriverCreateTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, DriverObject, RegistryPath, DriverAttributes, DriverConfig, Driver);
end;

function WdfRequestRetrieveInputWdmMdl(Request : WDFREQUEST; Mdl : PPMDL) : NTSTATUS; stdcall;
var
    func : FN_WDFREQUESTRETRIEVEINPUTWDMMDL;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfRequestRetrieveInputWdmMdlTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, Request, Mdl);
end;

function WdfRequestRetrieveOutputWdmMdl(Request : WDFREQUEST; Mdl : PPMDL) : NTSTATUS; stdcall;
var
    func : FN_WDFREQUESTRETRIEVEOUTPUTWDMMDL;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfRequestRetrieveOutputWdmMdlTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, Request, Mdl);
end;

function WdfMemoryCopyToBuffer(SourceMemory : WDFMEMORY; SourceOffset : ULONG; Buffer : Pointer; NumBytesToCopyTo : ULONG) : NTSTATUS; stdcall;
var
    func : FN_WDFMEMORYCOPYTOBUFFER;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfMemoryCopyToBufferTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, SourceMemory, SourceOffset, Buffer, NumBytesToCopyTo);
end;

function WdfRequestRetrieveInputMemory(Request : WDFREQUEST; Memory : PWDFMEMORY) : NTSTATUS; stdcall;
var
    func : FN_WDFREQUESTRETRIEVEINPUTMEMORY;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfRequestRetrieveInputMemoryTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, Request, Memory);
end;

function WdfMemoryCopyFromBuffer(DestinationMemory : WDFMEMORY; DestinationOffset : ULONG; Buffer : Pointer; NumBytesToCopyFrom : ULONG) : NTSTATUS; stdcall;
var
    func : FN_WDFMEMORYCOPYFROMBUFFER;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfMemoryCopyFromBufferTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, DestinationMemory, DestinationOffset, Buffer, NumBytesToCopyFrom);
end;

function WdfRequestRetrieveOutputMemory(Request : WDFREQUEST; Memory : PWDFMEMORY) : NTSTATUS; stdcall;
var
    func : FN_WDFREQUESTRETRIEVEOUTPUTMEMORY;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfRequestRetrieveOutputMemoryTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, Request, Memory);
end;

function WdfIoQueueCreate(Device : WDFDEVICE; Config : PWDF_IO_QUEUE_CONFIG; QueueAttributes : PWDF_OBJECT_ATTRIBUTES; Queue : PWDFQUEUE) : NTSTATUS; stdcall;
var
    func : FN_WDFIOQUEUECREATE;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfIoQueueCreateTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, Device, Config, QueueAttributes, Queue);
end;

function WdfDeviceCreateSymbolicLink(Device : WDFDEVICE; SymbolicLinkName : PUNICODE_STRING) : NTSTATUS; stdcall;
var
    func : FN_WDFDEVICECREATESYMBOLICLINK;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfDeviceCreateSymbolicLinkTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, Device, SymbolicLinkName);
end;

function WdfDeviceCreate(DeviceInit : PPWDFDEVICE_INIT; DeviceAttributes : PWDF_OBJECT_ATTRIBUTES; Device : PWDFDEVICE) : NTSTATUS; stdcall;
var
    func : FN_WDFDEVICECREATE;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfDeviceCreateTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, DeviceInit, DeviceAttributes, Device);
end;

function WdfDeviceInitAssignName(DeviceInit : PWDFDEVICE_INIT; DeviceName : PUNICODE_STRING) : NTSTATUS; stdcall;
var
    func : FN_WDFDEVICEINITASSIGNNAME;
    
begin
    func := PPointer(Integer(WdfFunctions) + WdfDeviceInitAssignNameTableIndex * sizeof(ULONG))^;
    Result := func(WdfDriverGlobals, DeviceInit, DeviceName);
end;

function IoCallDriver(DeviceObject : PDEVICE_OBJECT; Irp : PIRP) : NTSTATUS; stdcall;
begin
    Result := krnlIoCallDriver(DeviceObject, Irp);
end;

function IoCsqInitialize(Csq : PIO_CSQ; CsqInsertIrp : PIO_CSQ_INSERT_IRP; CsqRemoveIrp : PIO_CSQ_REMOVE_IRP; CsqPeekNextIrp : PIO_CSQ_PEEK_NEXT_IRP; CsqAcquireLock : PIO_CSQ_ACQUIRE_LOCK; CsqReleaseLock : PIO_CSQ_RELEASE_LOCK; CsqCompleteCanceledIrp : PIO_CSQ_COMPLETE_CANCELED_IRP) : NTSTATUS; stdcall;
begin
    Result := krnlIoCsqInitialize(Csq, CsqInsertIrp, CsqRemoveIrp, CsqPeekNextIrp, CsqAcquireLock, CsqReleaseLock, CsqCompleteCanceledIrp);
end;

function RemoveHeadList(ListHead : PLIST_ENTRY) : PLIST_ENTRY; stdcall;
begin
    Result := Pointer(ListHead^.Flink);
    RemoveEntryList(Pointer(ListHead^.Flink));
end;

function RemoveTailList(ListHead : PLIST_ENTRY) : PLIST_ENTRY; stdcall;
begin
    Result := Pointer(ListHead^.Blink);
    RemoveEntryList(Pointer(ListHead^.Blink));
end;

function IoDeleteSymbolicLink(SymbolicLinkName : PUnicodeString) : NTSTATUS; stdcall; 
begin
    Result := krnlIoDeleteSymbolicLink(SymbolicLinkName); 
end;

function IoAttachDeviceToDeviceStack(SourceDevice : PDEVICE_OBJECT; TargetDevice : PDEVICE_OBJECT) : PDEVICE_OBJECT; stdcall;
begin
    Result := krnlIoAttachDeviceToDeviceStack(SourceDevice, TargetDevice);
end;

function KeSetEvent(Event : PRKEVENT; Increment : KPRIORITY; Wait : BOOLEAN) : LONG; stdcall;
begin
    Result := krnlKeSetEvent(Event, Increment, Wait);
end;

function KeReadStateSemaphore(Semaphore : PRKSEMAPHORE) : LONG; stdcall;
begin
    Result := krnlKeReadStateSemaphore(Semaphore);
end;

function KeReleaseSemaphore(Semaphore : PRKSEMAPHORE; Increment : KPRIORITY; Adjustment : LONG; Wait : BOOLEAN) : LONG stdcall;
begin
    Result := krnlKeReleaseSemaphore(Semaphore, Increment, Adjustment, Wait);
end;

function InterlockedExchange(Target : PLONG; Value : LONG) : LONG; assembler;
asm 
    mov ecx, Target 
    mov edx, Value 
    call krnlInterlockedExchange 
end;

function IsListEmpty(ListHead : PLIST_ENTRY) : Boolean; stdcall;
begin
    Result := False;
    if ListHead^.Flink = Pointer(ListHead) then
    begin
    Result := True;
    end;
end;

function RemoveEntryList(Entry : PLIST_ENTRY) : Boolean; stdcall;
var
    _EX_Blink : PLIST_ENTRY;
    _EX_Flink : PLIST_ENTRY;

begin
    _EX_Flink := Pointer(Entry^.Flink);
    _EX_Blink := Pointer(Entry^.Blink);
    _EX_Blink^.Flink := Pointer(_EX_Flink);
    _EX_Flink^.Blink := Pointer(_EX_Blink);
    
    Result := False;
    if _EX_Flink = _EX_Blink then
    begin
    Result := True;
    end;
end;

function KeSetTimerEx(Timer : PKTIMER; LowPart : ULONG; HighPart : ULONG; Period : LONG; Dpc : PKDPC) : Boolean; stdcall;
begin
    Result := krnlKeSetTimerEx(Timer, LowPart, HighPart, Period, Dpc);
end;

function KeSetTimer(Timer : PKTIMER; LowPart : ULONG; HighPart : ULONG; Dpc : PKDPC) : Boolean; stdcall;
begin
    Result := krnlKeSetTimer(Timer, LowPart, HighPart, Dpc);
end;

function KeCancelTimer(Arg1 : PKTIMER) : Boolean; stdcall;
begin
    Result := krnlKeCancelTimer(Arg1);
end;

function ZwOpenProcess(ProcessHandle : PHandle; DesiredAccess : TAccessMask; ObjectAttributes : PObjectAttributes; ClientId : PClientId) : NTSTATUS; stdcall; 
begin 
    Result := krnlZwOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
end;

function KeWaitForSingleObject(SObject : Pointer; WaitReason : TKWaitReason; WaitMode : TKProcessorMode; Alertable : LongBool; Timeout : PLargeInteger) : NTSTATUS; stdcall; 
begin 
    Result := krnlKeWaitForSingleObject(SObject, WaitReason, WaitMode, Alertable, Timeout);
end;

function KeWaitForMutexObject(SObject : Pointer; WaitReason : TKWaitReason; WaitMode : TKProcessorMode; Alertable : LongBool; Timeout : PLargeInteger) : NTSTATUS; stdcall; 
begin 
    Result := krnlKeWaitForSingleObject(SObject, WaitReason, WaitMode, Alertable, Timeout);
end;

function ExAllocatePool(PoolType : TPoolType; NumberOfBytes : Cardinal) : Pointer; stdcall; 
begin 
    Result := krnlExAllocatePool(PoolType, NumberOfBytes); 
end;

function ExAllocatePoolWithQuota(PoolType : TPoolType; NumberOfBytes : Cardinal) : Pointer; stdcall; 
begin
    Result := krnlExAllocatePoolWithQuota(PoolType, NumberOfBytes); 
end;

function ExAllocatePoolWithTag(PoolType : TPoolType; NumberOfBytes : Cardinal; Tag : ULONG) : Pointer; stdcall; 
begin 
    Result := krnlExAllocatePoolWithTag(PoolType, NumberOfBytes, Tag); 
end;

function KeReleaseMutex(Mutex : PKMutex; Wait : LongBool) : LONG; stdcall; 
begin 
    Result :=krnlKeReleaseMutex(Mutex, Wait); 
end;

function PsTerminateSystemThread(ExitStatus : NTSTATUS) : NTSTATUS; stdcall; 
begin 
    Result := krnlPsTerminateSystemThread(ExitStatus); 
end;

function IoCreateSymbolicLink(SymbolicLinkName : PUnicodeString; DeviceName : PUnicodeString) : NTSTATUS; stdcall; 
begin 
    Result := krnlIoCreateSymbolicLink(SymbolicLinkName, DeviceName); 
end;

function IoCreateDevice(DriverObject : PDriverObject; DeviceExtensionSize : Cardinal; DeviceName : PUnicodeString; DeviceType : TDeviceType; DeviceCharacteristics : Cardinal; Reserved : Boolean; var DeviceObject : PDeviceObject) : NTSTATUS; stdcall; 
begin
    Result := krnlIoCreateDevice(DriverObject, DeviceExtensionSize, DeviceName, DeviceType, DeviceCharacteristics, Reserved, DeviceObject); 
end;

function PsCreateSystemThread(ThreadHandle : PHandle; DesiredAccess : ULONG; ObjectAttributes : Pointer; ProcessHandle : Handle; ClientId : Pointer; StartRoutine : PKSTART_ROUTINE; StartContext : Pointer) : NTSTATUS; stdcall; 
begin 
    Result := krnlPsCreateSystemThread(ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, ClientId, StartRoutine, StartContext); 
end;

function ObReferenceObjectByHandle(Handle : Handle; DesiredAccess : ACCESS_MASK; ObjectType : Pointer; AccessMode : KPROCESSOR_MODE; _Object : PPointer; HandleInformation : POBJECT_HANDLE_INFORMATION) : NTSTATUS; stdcall; 
begin 
    Result := krnlObReferenceObjectByHandle(Handle, DesiredAccess, ObjectType, AccessMode, _Object, HandleInformation); 
end;

//
// this is 3 times ugly (take care while reading):
// messy Zw* function addressing, @krnlZw* points to jmp ntoskrnl.Zw* instruction
// which is jmp [IAT.ntoskrnl.Zw*] and so that we move our pointer 2 bytes right (jmp instruction is 2 byte long)
// to the pointer itself and dereference it twice to make it real pointer to Zw* function
//
function ZwOpenProcessAddr : Pointer; 
begin 
    Result := PPointer(PPointer(Cardinal(@krnlZwOpenProcess)+2)^)^; 
end;

//
// this is just ugly (weird):
// somehow we don't know how to import data structure in Delphi, we need KeSDT import
// so we import it as a procedure krnlKeServiceDescriptorTable and make this function to return 
// a pointer to that table
//
function KeServiceDescriptorTable : PServiceDescriptorEntry; 
begin 
    Result := PPointer(@krnlKeServiceDescriptorTable)^; 
end;

//
// this is a must (but still ugly):
// we do not have #define in Delphi, we have to rewrite non-statical #define asa function
//
function IoGetCurrentIrpStackLocation(pIrp : PIRP) : PIoStackLocation; stdcall; 
begin 
    Result := pIrp^.Tail.Overlay.s1.u1.CurrentStackLocation; 
end;

function NT_SUCCESS(AStatus : NTSTATUS) : Boolean;
begin
    Result := Integer(AStatus) >= 0;
end;

function MmGetMdlByteCount(pMDL : PMDL) : ULONG; stdcall; 
begin 
    Result := pMDL^.ByteCount;
end;

function MmGetSystemAddressForMdlSafe(pMDL : PMDL;Priority : ULONG) : Pointer; stdcall; 
var
    flag : WORD;
    
begin 
    flag := pMDL^.MdlFlags;
    flag := flag and (MDL_MAPPED_TO_SYSTEM_VA or MDL_SOURCE_IS_NONPAGED_POOL);
    if flag > 0 then
    begin
    Result := pMDL^.MappedSystemVa;
    end else
    begin
    Result := MmMapLockedPagesSpecifyCache(pMDL, KernelMode, MmCached, Nil, 0, Priority);
    end;
end;

function MmGetMdlVirtualAddress(pMDL : PMDL) : Pointer; stdcall
begin
    Result := Pointer(ULONG(pMDL^.StartVa) + pMDL^.ByteOffset);
end;

procedure memset(dst  : Pointer; val  : Char; len  : ULONG); stdcall;
var
    i  : ULONG;
    d  : PChar;
    
begin
    d := dst;
    for i := 0 to (len - 1) do begin
    d[i] := val;
    end;
end;

procedure memcpy(dst  : Pointer; src  : Pointer; len  : ULONG); stdcall;
var
    i  : ULONG;
    s  : PChar;
    d  : PChar;
    
begin
    s := src;
    d := dst;
    for i := 0 to (len - 1) do begin
    d[i] := s[i];
    end;
end;

function strlen(src : Pointer) : ULONG; stdcall;
var
    s : PChar;
    
begin
    s := src;
    Result := 0;
    while Integer(s[Result]) > 0 do
    begin
    Result := Result + 1
    end;
end;

function ZwClose(Handle : Handle) : NTSTATUS; stdcall;
begin
    Result := krnlZwClose(Handle);
end;

function IoGetCurrentProcess() : Pointer; stdcall;
begin
    Result := krnlIoGetCurrentProcess();
end;

function KeDelayExecutionThread(WaitMode : KPROCESSOR_MODE; Alertable : Boolean; Interval : PLARGE_INTEGER) : NTSTATUS; stdcall;
begin
    Result := krnlKeDelayExecutionThread(WaitMode, Alertable, Interval);
end;

function IoInitializeTimer(DeviceObject : PDeviceObject; TimerRoutine : PIO_TIMER_ROUTINE; Context : Pointer) : NTSTATUS; stdcall;
begin
    Result := krnlIoInitializeTimer(DeviceObject, TimerRoutine, Context);
end;

//{$IFDEF DEBUG}
//
// our Delphi implementation of DbgPrint
// very useful function for coding kernel driver 
// and the must is to have variable number of arguments like in original
// DbgPrint call
// this implementation is very ugly, it is because we don't know how to 
// code similar convetion as C language has for this type of functions
//
function DbgPrint(Format : PChar; Args : array of const) : NTSTATUS;
var
    LI, LJ : Integer;
    LArgs : array[0..31] of Cardinal;
begin
    LJ := 0;
    
    //we fill our local arguments array
    for LI :=0 to High(Args) do
    begin
    with Args[LI] do
    begin
    case VType of
    vtInteger : LArgs[LJ] := VInteger;
    vtBoolean : LArgs[LJ] := Cardinal(VBoolean);
    vtChar : LArgs[LJ] := Cardinal(VChar);
    vtString : LArgs[LJ] := Cardinal(PChar(VString));
    vtPChar : LArgs[LJ] := Cardinal(VPChar);
    vtPointer : LArgs[LJ] := Cardinal(VPointer);
    vtAnsiString : LArgs[LJ] := Cardinal(PChar(VAnsiString));
    vtCurrency : LArgs[LJ] := Cardinal(VCurrency);
    vtVariant : LArgs[LJ] := Cardinal(VVariant);
    else LArgs[LJ] := $DEADBEEF;
    end;
    end;
    Inc(LJ);
    end;

    LJ := High(Args);
    //and we simulate the calling convetion using lowlevel
    asm
    lea eax, LArgs
    mov ecx, LJ
    jmp @cmp_args_end
    @args_loop:
    push dword ptr [eax+4*ecx]
    dec ecx
    @cmp_args_end:
    cmp ecx, -001h
    jnz @args_loop
    @make_call:
    push Format
    call krnlDbgPrint

    mov ecx, LJ
    shl ecx, 002h
    add ecx, 008h
    add esp, ecx
    mov Result, eax
    end
end;

function DbgMsg(Format : PChar; Args : array of const) : NTSTATUS; 
begin
    Result := DbgPrint(Format,Args);
end;
end.
