PAGE 58,132
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	VNETBIOS.INC - Virtual NetBIOS Device Service Declarations
;
;   Version:	1.00
;
;   Date:	12-Dec-1989
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   12-Dec-1989 RAL Original
;
;==============================================================================

Begin_Service_Table VNETBIOS

VNETBIOS_Service VNETBIOS_Get_Version, LOCAL
VNETBIOS_Service VNETBIOS_Register, LOCAL
VNETBIOS_Service VNETBIOS_Submit, LOCAL
VNETBIOS_Service VNETBIOS_Enum, LOCAL
VNETBIOS_Service VNETBIOS_Deregister, LOCAL
VNETBIOS_Service VNETBIOS_Register2, LOCAL
VNETBIOS_Service VNETBIOS_Map, LOCAL
VNETBIOS_Service VNETBIOS_Enum2, LOCAL

End_Service_Table VNETBIOS
