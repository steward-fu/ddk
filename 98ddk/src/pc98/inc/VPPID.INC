;******************************************************************************
;
;   (C) Copyright NEC Corp., 1994
;
;   Title:	Virtual PPI Exported Services
;
;   Version:	1.00
;
;   Date:	21-Sep-1994
;
;   Author:	TAK
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   21-Sep-1994 000 Original
;
;==============================================================================

Begin_Service_Table VPPID

VPPID_Service VPPID_Get_Version, LOCAL
VPPID_Service VPPID_Install_IO_Handle, LOCAL
VPPID_Service VPPID_Remove_IO_Handle, LOCAL

End_Service_Table VPPID
