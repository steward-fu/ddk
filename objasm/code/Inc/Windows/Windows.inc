;Include file created by h2incX, Version B.01.01, 01/09/2018 18:44
;(C) 2018-today by Biterider, 2005-2009 by Japheth
;Source file: '\ObjAsm\Projects\h2IncX\header\Windows.h', last modified: 26/08/2018 18:51
;Creation parameters: 

include WinAsm.inc
include winapifamily.inc
ifndef _WINDOWS_
  _WINDOWS_ equ <>
  include sdkddkver.inc
  ifndef _INC_WINDOWS
    _INC_WINDOWS equ <>
    if Defined(_MSC_VER) and (_MSC_VER ge 1020)
    endif
    if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP or WINAPI_PARTITION_SYSTEM)
      if Defined(RC_INVOKED) and  not Defined(NOWINRES)
        ;[...]
      else
        if Defined(RC_INVOKED)
          ;[...]
        endif
        if  not Defined(_68K_) and  not Defined(_MPPC_) and  not Defined(_X86_) and  not Defined(_IA64_) and  not Defined(_AMD64_) and  not Defined(_ARM_) and  not Defined(_ARM64_) and Defined(_M_IX86)
          _X86_ equ <>
          if  not Defined(_CHPE_X86_ARM64_) and Defined(_M_HYBRID)
            _CHPE_X86_ARM64_ equ <>
          endif
        endif
        if  not Defined(_68K_) and  not Defined(_MPPC_) and  not Defined(_X86_) and  not Defined(_IA64_) and  not Defined(_AMD64_) and  not Defined(_ARM_) and  not Defined(_ARM64_) and Defined(_M_AMD64)
          _AMD64_ equ <>
        endif
        if  not Defined(_68K_) and  not Defined(_MPPC_) and  not Defined(_X86_) and  not Defined(_IA64_) and  not Defined(_AMD64_) and  not Defined(_ARM_) and  not Defined(_ARM64_) and Defined(_M_ARM)
          ;[...]
        endif
        if  not Defined(_68K_) and  not Defined(_MPPC_) and  not Defined(_X86_) and  not Defined(_IA64_) and  not Defined(_AMD64_) and  not Defined(_ARM_) and  not Defined(_ARM64_) and Defined(_M_ARM64)
          ;[...]
        endif
        if  not Defined(_68K_) and  not Defined(_MPPC_) and  not Defined(_X86_) and  not Defined(_IA64_) and  not Defined(_AMD64_) and  not Defined(_ARM_) and  not Defined(_ARM64_) and Defined(_M_M68K)
          _68K_ equ <>
        endif
        if  not Defined(_68K_) and  not Defined(_MPPC_) and  not Defined(_X86_) and  not Defined(_IA64_) and  not Defined(_AMD64_) and  not Defined(_ARM_) and  not Defined(_ARM64_) and Defined(_M_MPPC)
          _MPPC_ equ <>
        endif
        if  not Defined(_68K_) and  not Defined(_MPPC_) and  not Defined(_X86_) and  not Defined(_M_IX86) and  not Defined(_AMD64_) and  not Defined(_ARM_) and  not Defined(_ARM64_) and Defined(_M_IA64)
          if  not Defined(_IA64_)
            _IA64_ equ <>
          endif
        endif
        ifndef _MAC
          if Defined(_68K_) or Defined(_MPPC_)
            ;[...]
          endif
        endif
        if Defined(_MSC_VER)
          if (_MSC_VER ge 800)
            ifndef __cplusplus
            endif
          endif
        endif
        ifndef RC_INVOKED
          if (_MSC_VER ge 800)
            ifndef __WINDOWS_DONT_DISABLE_PRAGMA_PACK_WARNING__
            endif
            if _MSC_VER ge 1200
            endif
          endif
          include excpt.inc
          include stdarg.inc
        endif
        include windef.inc
        include winbase.inc
        include wingdi.inc
        include winuser.inc
        if  not Defined(_MAC) or Defined(_WIN32NLS)
          include winnls.inc
        endif
        ifndef _MAC
          include wincon.inc
          include winver.inc
        endif
        if  not Defined(_MAC) or Defined(_WIN32REG)
          include winreg.inc
        endif
        ifndef _MAC
          include winnetwk.inc
        endif
        ifndef WIN32_LEAN_AND_MEAN
          include cderr.inc
          include dde.inc
          include ddeml.inc
          include dlgs.inc
          ifndef _MAC
            include lzexpand.inc
            include mmsystem.inc
            include nb30.inc
            include rpc.inc
          endif
          include shellapi.inc
          ifndef _MAC
            include winperf.inc
            include winsock.inc
          endif
          ifndef NOCRYPT
            include wincrypt.inc
            include winefs.inc
            include winscard.inc
          endif
          ifndef NOGDI
            ifndef _MAC
              include winspool.inc
              ifdef INC_OLE1
                include ole.inc
              else
                include ole2.inc
              endif
            endif
            include commdlg.inc
          endif
        endif
        include stralign.inc
        ifdef _MAC
          ;[...]
        endif
        ifdef INC_OLE2
          include ole2.inc
        endif
        ifndef _MAC
          ifndef NOSERVICE
            include winsvc.inc
          endif
          if (WINVER ge 0400h)
            ifndef NOMCX
              include mcx.inc
            endif
            ifndef NOIME
              include imm.inc
            endif
          endif
        endif
        ifndef RC_INVOKED
          if (_MSC_VER ge 800)
            if _MSC_VER ge 1200
            else
            endif
          endif
        endif
      endif
    endif
  endif
endif

