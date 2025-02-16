comment ^
Module Name:
    mountmgr.inc

Abstract:
    This file defines the external mount point interface for administering
    mount points.

Author:
    Four-F (four-f@mail.ru)

Last update:
	30-June-2004
^

IFNDEF _MOUNTMGR_
_MOUNTMGR_ equ 1

MOUNTMGRCONTROLTYPE  equ 0000006dh	; ((ULONG) 'm')
MOUNTDEVCONTROLTYPE  equ 0000004dh	; ((ULONG) 'M')


IFNDEF CTL_CODE
CTL_CODE MACRO DeviceType:=<0>, Function:=<0>, Method:=<0>, Access:=<0>
	EXITM %(((DeviceType) SHL 16) OR ((Access) SHL 14) OR ((Function) SHL 2) OR (Method))
ENDM
ENDIF

; These are the IOCTLs supported by the mount point manager.

IOCTL_MOUNTMGR_CREATE_POINT                 equ CTL_CODE(MOUNTMGRCONTROLTYPE, 0, METHOD_BUFFERED, FILE_READ_ACCESS or FILE_WRITE_ACCESS)
IOCTL_MOUNTMGR_DELETE_POINTS                equ CTL_CODE(MOUNTMGRCONTROLTYPE, 1, METHOD_BUFFERED, FILE_READ_ACCESS or FILE_WRITE_ACCESS)
IOCTL_MOUNTMGR_QUERY_POINTS                 equ CTL_CODE(MOUNTMGRCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
IOCTL_MOUNTMGR_DELETE_POINTS_DBONLY         equ CTL_CODE(MOUNTMGRCONTROLTYPE, 3, METHOD_BUFFERED, FILE_READ_ACCESS or FILE_WRITE_ACCESS)
IOCTL_MOUNTMGR_NEXT_DRIVE_LETTER            equ CTL_CODE(MOUNTMGRCONTROLTYPE, 4, METHOD_BUFFERED, FILE_READ_ACCESS or FILE_WRITE_ACCESS)
IOCTL_MOUNTMGR_AUTO_DL_ASSIGNMENTS          equ CTL_CODE(MOUNTMGRCONTROLTYPE, 5, METHOD_BUFFERED, FILE_READ_ACCESS or FILE_WRITE_ACCESS)
IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_CREATED   equ CTL_CODE(MOUNTMGRCONTROLTYPE, 6, METHOD_BUFFERED, FILE_READ_ACCESS or FILE_WRITE_ACCESS)
IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_DELETED   equ CTL_CODE(MOUNTMGRCONTROLTYPE, 7, METHOD_BUFFERED, FILE_READ_ACCESS or FILE_WRITE_ACCESS)
IOCTL_MOUNTMGR_CHANGE_NOTIFY                equ CTL_CODE(MOUNTMGRCONTROLTYPE, 8, METHOD_BUFFERED, FILE_READ_ACCESS)
IOCTL_MOUNTMGR_KEEP_LINKS_WHEN_OFFLINE      equ CTL_CODE(MOUNTMGRCONTROLTYPE, 9, METHOD_BUFFERED, FILE_READ_ACCESS or FILE_WRITE_ACCESS)
IOCTL_MOUNTMGR_CHECK_UNPROCESSED_VOLUMES    equ CTL_CODE(MOUNTMGRCONTROLTYPE, 10, METHOD_BUFFERED, FILE_READ_ACCESS)
IOCTL_MOUNTMGR_VOLUME_ARRIVAL_NOTIFICATION  equ CTL_CODE(MOUNTMGRCONTROLTYPE, 11, METHOD_BUFFERED, FILE_READ_ACCESS)

; Input structure for IOCTL_MOUNTMGR_CREATE_POINT.

MOUNTMGR_CREATE_POINT_INPUT STRUCT		; sizeof = 8
	SymbolicLinkNameOffset	WORD	?
	SymbolicLinkNameLength	WORD	?
	DeviceNameOffset		WORD	?
	DeviceNameLength		WORD	?
MOUNTMGR_CREATE_POINT_INPUT ENDS
PMOUNTMGR_CREATE_POINT_INPUT typedef ptr MOUNTMGR_CREATE_POINT_INPUT

; Input structure for IOCTL_MOUNTMGR_DELETE_POINTS,
; IOCTL_MOUNTMGR_QUERY_POINTS, and IOCTL_MOUNTMGR_DELETE_POINTS_DBONLY.

MOUNTMGR_MOUNT_POINT STRUCT		; sizeof = 18h
	SymbolicLinkNameOffset	DWORD	?
	SymbolicLinkNameLength	WORD	?
    						WORD	?
	UniqueIdOffset			DWORD	?
	UniqueIdLength			WORD	?
    						WORD	?
	DeviceNameOffset		DWORD	?
	DeviceNameLength		WORD	?
				    		WORD	?
MOUNTMGR_MOUNT_POINT ENDS
PMOUNTMGR_MOUNT_POINT typedef ptr MOUNTMGR_MOUNT_POINT

; Output structure for IOCTL_MOUNTMGR_DELETE_POINTS,
; IOCTL_MOUNTMGR_QUERY_POINTS, and IOCTL_MOUNTMGR_DELETE_POINTS_DBONLY.

MOUNTMGR_MOUNT_POINTS STRUCT		; sizeof = 20h
	_Size					DWORD	?
	NumberOfMountPoints		DWORD	?
	MountPoints				MOUNTMGR_MOUNT_POINT 1 dup(<>)
MOUNTMGR_MOUNT_POINTS ENDS
PMOUNTMGR_MOUNT_POINTS typedef ptr MOUNTMGR_MOUNT_POINTS

; Input structure for IOCTL_MOUNTMGR_NEXT_DRIVE_LETTER.

MOUNTMGR_DRIVE_LETTER_TARGET STRUCT		; sizeof = 4
	DeviceNameLength	WORD	?
	DeviceName			WORD 1 dup(?)	; WCHAR
MOUNTMGR_DRIVE_LETTER_TARGET ENDS
PMOUNTMGR_DRIVE_LETTER_TARGET typedef ptr MOUNTMGR_DRIVE_LETTER_TARGET

; Output structure for IOCTL_MOUNTMGR_NEXT_DRIVE_LETTER.

MOUNTMGR_DRIVE_LETTER_INFORMATION STRUCT		; sizeof = 2
	DriveLetterWasAssigned		BOOLEAN	?
	CurrentDriveLetter			BYTE	?
MOUNTMGR_DRIVE_LETTER_INFORMATION ENDS
PMOUNTMGR_DRIVE_LETTER_INFORMATION typedef ptr MOUNTMGR_DRIVE_LETTER_INFORMATION

; Input structure for IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_CREATED and
; IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_DELETED.

MOUNTMGR_VOLUME_MOUNT_POINT STRUCT		; sizeof = 8
	SourceVolumeNameOffset		WORD	?
	SourceVolumeNameLength		WORD	?
	TargetVolumeNameOffset		WORD	?
	TargetVolumeNameLength		WORD	?
MOUNTMGR_VOLUME_MOUNT_POINT ENDS
PMOUNTMGR_VOLUME_MOUNT_POINT typedef ptr MOUNTMGR_VOLUME_MOUNT_POINT

; Input structure for IOCTL_MOUNTMGR_CHANGE_NOTIFY.
; Output structure for IOCTL_MOUNTMGR_CHANGE_NOTIFY.

MOUNTMGR_CHANGE_NOTIFY_INFO STRUCT		; sizeof = 4
	EpicNumber	DWORD	?
MOUNTMGR_CHANGE_NOTIFY_INFO ENDS
PMOUNTMGR_CHANGE_NOTIFY_INFO typedef ptr MOUNTMGR_CHANGE_NOTIFY_INFO

; Input structure for IOCTL_MOUNTMGR_KEEP_LINKS_WHEN_OFFLINE and
; IOCTL_MOUNTMGR_VOLUME_ARRIVAL_NOTIFICATION.

MOUNTMGR_TARGET_NAME STRUCT		; sizeof = 4
	DeviceNameLength	WORD	?
	DeviceName			WORD 1 dup(?)	; WCHAR
MOUNTMGR_TARGET_NAME ENDS
PMOUNTMGR_TARGET_NAME typedef ptr MOUNTMGR_TARGET_NAME

; The following IOCTL is supported by mounted devices.

IOCTL_MOUNTDEV_QUERY_DEVICE_NAME    equ CTL_CODE(MOUNTDEVCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

; Output structure for IOCTL_MOUNTDEV_QUERY_DEVICE_NAME.

MOUNTDEV_NAME STRUCT		; sizeof = 4
	NameLength		WORD	?
	_Name			WORD 1 dup(?)	; WCHAR
MOUNTDEV_NAME ENDS
PMOUNTDEV_NAME typedef ptr MOUNTDEV_NAME

ENDIF ; _MOUNTMGR_
