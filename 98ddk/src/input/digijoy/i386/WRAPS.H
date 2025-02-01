/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1998 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       wraps.h
 *  Content:    Wrapper inlines for external VxD calls
 *
 ***************************************************************************/

#pragma warning (disable:4035)  /* turn off no return code warning */

PVOID VXDINLINE _HeapReAllocate(PVOID pMem, ULONG Len, ULONG Flags)
{
    Touch_Register(eax)
    __asm push Flags
    Touch_Register(ecx)
    __asm push Len
    Touch_Register(edx)
    __asm push pMem
    VMMCall(_HeapAllocate)
    __asm add esp, 8
}

ULONG VXDINLINE __cdecl _lmemcpy( PVOID pDst, const void *pSrc, DWORD cb )
{
    __asm push cb
    Touch_Register(eax)
    __asm push pSrc
    Touch_Register(ecx)
    __asm push pDst
    Touch_Register(edx)
    VMMCall( _lmemcpy );
    __asm add esp,3*4
}

ULONG VXDINLINE __stdcall SprintfOne( PCHAR pszBuf, PCHAR pszFmt, DWORD dwNum )
{
    __asm push dwNum
    Touch_Register(eax)
    __asm push pszFmt
    Touch_Register(ecx)
    __asm push pszBuf
    Touch_Register(edx)
    VMMCall( _Sprintf );
    __asm add esp,3*4
}

CONFIGRET VXDINLINE __cdecl CM_Get_Alloc_Log_Conf( PCMCONFIG pccBuffer, DEVNODE dnDevNode, ULONG ulFlags )
{
    Touch_Register(eax)
    __asm push ulFlags
    Touch_Register(ecx)
    __asm push dnDevNode
    Touch_Register(edx)
    __asm push pccBuffer
    VMMCall(_CONFIGMG_Get_Alloc_Log_Conf);
    __asm add  esp, 3*4
}

CONFIGRET VXDINLINE __cdecl CM_Get_Device_ID_Size(PFARULONG pulLen, DEVNODE dnDevNode, ULONG ulFlags)
{
    Touch_Register(eax)
    __asm push ulFlags
    Touch_Register(ecx)
    __asm push dnDevNode
    Touch_Register(edx)
    __asm push pulLen
    VMMCall(_CONFIGMG_Get_Device_ID_Size);
    __asm add  esp, 3*4
}

CONFIGRET VXDINLINE __cdecl CM_Get_Device_ID(DEVNODE dnDevNode, PFARVOID Buffer, ULONG BufferLen, ULONG ulFlags)
{
    __asm push ulFlags
    Touch_Register(eax)
    __asm push BufferLen
    Touch_Register(ecx)
    __asm push Buffer
    Touch_Register(edx)
    __asm push dnDevNode
    VMMCall(_CONFIGMG_Get_Device_ID);
    __asm add  esp, 4*4
}

#ifndef RETAIL
ULONG VXDINLINE __cdecl _lstrlen( const char *psz )
{
    Touch_Register(eax)
    __asm push psz
    Touch_Register(ecx)
    Touch_Register(edx)
    VMMCall( _lstrlen );
    __asm add esp,1*4
}

ULONG VXDINLINE _HeapGetSize( PVOID pMem, ULONG Flags )
{
    Touch_Register(eax)
    __asm push Flags
    Touch_Register(ecx)
    __asm push pMem
    Touch_Register(edx)
    VMMCall(_HeapAllocate)
    __asm add esp, 8
}
#endif
#pragma warning (default:4035)  /* return no return code warning */

