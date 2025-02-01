/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       reglog.c
 *  Content:    write current driver info into registry
 *      \HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\Class\Display      
 *      Registry Logging Contents:
 *              ChipType        string
 *              DACType         string
 *              VideoMemorySize ulong
 *              MMIO            bool
 *              LinearAccess    bool
 *              HWCursor        bool
 *
 **X*************************************************************************/
#include        <windows.h>

#pragma code_seg("_INIT")

#pragma optimize("gle",off)
#define Not_VxD
#include        <vmm.h>
#include        <configmg.h>
#pragma optimize("",on)

#define MMIO_BIT        0x01
#define LINEAR_BIT      0x02
#define HWCURSOR_BIT    0x04

static char _based(_segname("_CODE")) szsubkey[]    = "INFO";
static char _based(_segname("_CODE")) szchiptype[]  = "ChipType";
static char _based(_segname("_CODE")) szrevision[]  = "Revision";
static char _based(_segname("_CODE")) szdactype[]   = "DACType";
static char _based(_segname("_CODE")) szvidmem[]    = "VideoMemory";
static char _based(_segname("_CODE")) szmmio[]      = "MMIO";
static char _based(_segname("_CODE")) szlinear[]    = "Linear";
static char _based(_segname("_CODE")) szhwcursor[]  = "HWCursor";
static char _based(_segname("_CODE")) szunknown[]   = "";

void FAR PASCAL RegistryLog(DEVNODE     dnDevNode,
                        PFARVOID        pszChipType,
                        WORD            wRevision,
                        PFARVOID        pszDACType,
                        ULONG           ulVideoMemorySize,
                        WORD            flags
                     )
{
    CONFIGRET   result;
    BYTE        yes=1;
    BYTE        no=0;

    if (dnDevNode == 0) {
#ifdef DEBUG
            OutputDebugString("INVALID DEVNODE Registry Not Written\r\n");
#endif
            return;
    }

    //
    //  write the Chiptype
    //
    if ((DWORD)pszChipType & 0xFFFF0000) {
        result=CM_Write_Registry_Value(dnDevNode,
                szsubkey,
                szchiptype,
                REG_SZ,
                pszChipType,
                strlen(pszChipType),
                CM_REGISTRY_SOFTWARE);
    }
    else {
        result=CM_Write_Registry_Value(dnDevNode,
                szsubkey,
                szchiptype,
                REG_SZ,
                szunknown,
                sizeof(szunknown),
                CM_REGISTRY_SOFTWARE);
    }

#ifdef DEBUG
    if (result != CR_SUCCESS) {
        switch (result) {
            case    CR_INVALID_DEVNODE: OutputDebugString("INVALID DEVNODE\r\n");            break;
            case    CR_INVALID_FLAG:    OutputDebugString("INVALID FLAG\r\n");               break;
            case    CR_INVALID_POINTER: OutputDebugString("INVALID POINTER\r\n");            break;
            case    CR_REGISTRY_ERROR:  OutputDebugString("UNSPECIFIED REGISTRY ERROR\r\n"); break;
            default:                    OutputDebugString("UNKNOWN ERROR CODE\r\n");         break;
        }
    }
#endif

    if (result != CR_SUCCESS) {
        return;
    }

    //
    //  write the chip rev
    //
    result=CM_Write_Registry_Value(dnDevNode,
            szsubkey,
            szrevision,
            REG_SZ,
            &wRevision,
            1,
            CM_REGISTRY_SOFTWARE);

    //
    // write the DAC type
    //
    if ((DWORD)pszDACType & 0xFFFF0000) {
        result=CM_Write_Registry_Value(dnDevNode,
                szsubkey,
                szdactype,
                REG_SZ,
                pszDACType,
                strlen(pszDACType),
                CM_REGISTRY_SOFTWARE);
    }
    else {
        result=CM_Write_Registry_Value(dnDevNode,
                szsubkey,
                szdactype,
                REG_SZ,
                szunknown,
                sizeof(szunknown),
                CM_REGISTRY_SOFTWARE);
    }

    //
    //  write the amount of vidmemory
    //
    if (ulVideoMemorySize) {
            result=CM_Write_Registry_Value(dnDevNode,
                    szsubkey,
                    szvidmem,
                    REG_DWORD,
                    &ulVideoMemorySize,
                    sizeof(ULONG),
                    CM_REGISTRY_SOFTWARE);
    }

    //
    // write the flags (MMIO, LINEAR, HWCURSOR)
    //
    if (flags & MMIO_BIT) {
            result=CM_Write_Registry_Value(dnDevNode,
                    szsubkey,
                    szmmio,
                    REG_BINARY,
                    &yes,
                    1,
                    CM_REGISTRY_SOFTWARE);
    }
    else {
            result=CM_Write_Registry_Value(dnDevNode,
                    szsubkey,
                    szmmio,
                    REG_BINARY,
                    &no,
                    1,
                    CM_REGISTRY_SOFTWARE);
    }

    if (flags & LINEAR_BIT) {
            result=CM_Write_Registry_Value(dnDevNode,
                    szsubkey,
                    szlinear,
                    REG_BINARY,
                    &yes,
                    1,
                    CM_REGISTRY_SOFTWARE);
    }
    else {
            result=CM_Write_Registry_Value(dnDevNode,
                    szsubkey,
                    szlinear,
                    REG_BINARY,
                    &no,
                    1,
                    CM_REGISTRY_SOFTWARE);
    }

    if (flags & HWCURSOR_BIT) {
            result=CM_Write_Registry_Value(dnDevNode,
                    szsubkey,
                    szhwcursor,
                    REG_BINARY,
                    &yes,
                    1,
                    CM_REGISTRY_SOFTWARE);
    }
    else {
          result=CM_Write_Registry_Value(dnDevNode,
                    szsubkey,
                    szhwcursor,
                    REG_BINARY,
                    &no,
                    1,
                    CM_REGISTRY_SOFTWARE);
    }

#ifdef DEBUG
        OutputDebugString(szchiptype);
        OutputDebugString("     :");
        if ((DWORD)pszChipType>>16)
                OutputDebugString(pszChipType);
        else
                OutputDebugString("UNKNOWN");
        OutputDebugString("\r\n");

        OutputDebugString(szrevision);
        OutputDebugString("     :");
        OutputDebugString((LPVOID)&wRevision);
        OutputDebugString("\r\n");

        OutputDebugString(szdactype);
        OutputDebugString("     :");
        if ((DWORD)pszDACType>>16)
                OutputDebugString(pszDACType);
        else
                OutputDebugString("UNKNOWN");
        OutputDebugString("\r\n");

        OutputDebugString("VideoMemory  : See Registry");
        OutputDebugString("\r\n");

        OutputDebugString(szmmio);
        OutputDebugString("     :");
        if (flags & MMIO_BIT)
                OutputDebugString("YES");
        else
                OutputDebugString("NO");
        OutputDebugString("\r\n");

        OutputDebugString(szlinear);
        OutputDebugString("     :");
        if (flags & LINEAR_BIT)
                OutputDebugString("YES");
        else
                OutputDebugString("NO");
        OutputDebugString("\r\n");

        OutputDebugString(szhwcursor);
        OutputDebugString("     :");
        if (flags & HWCURSOR_BIT)
                OutputDebugString("YES");
        else
                OutputDebugString("NO");
        OutputDebugString("\r\n");
#endif
}
