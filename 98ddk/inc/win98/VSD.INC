;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	Virtual Sound Device Exported Services
;
;   Version:	1.00
;
;   Date:	12-Oct-1988
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   12-Oct-1988 RAL Original
;   16-Jun-1993 rjc New services for accessibility
;
;==============================================================================

Begin_Service_Table VSD

VSD_Service VSD_Get_Version, LOCAL
VSD_Service VSD_Bell, LOCAL
VSD_Service VSD_SoundOn, LOCAL
VSD_Service VSD_TakeSoundPort, LOCAL

End_Service_Table VSD
