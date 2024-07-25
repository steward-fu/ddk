#pragma once

#include once "crt/long.bi"
#include once "crt/ctype.bi"
#include once "basetsd.bi"

extern "C"

#define _NTDEF_
#define IN
#define OUT
#define OPTIONAL
#define NOTHING
#define CRITICAL
const ANYSIZE_ARRAY = 1
#define RESTRICTED_POINTER
#define UNALIGNED
#define POINTER_64
type POINTER_64_INT as culong
#define POINTER_32

#if (defined(__FB_64BIT__) and (defined(__FB_WIN32__) or defined(__FB_UNIX__))) or ((not defined(__FB_64BIT__)) and defined(__FB_ARM__) and (defined(__FB_LINUX__) or defined(__FB_FREEBSD__) or defined(__FB_OPENBSD__) or defined(__FB_NETBSD__)))
	#define DECLSPEC_IMPORT
#else
	#define DECLSPEC_IMPORT __declspec(dllimport)
#endif

#define DECLSPEC_NORETURN
type PVOID as any ptr
type PVOID64 as any ptr

#if (defined(__FB_64BIT__) and (defined(__FB_WIN32__) or defined(__FB_UNIX__))) or ((not defined(__FB_64BIT__)) and defined(__FB_ARM__) and (defined(__FB_LINUX__) or defined(__FB_FREEBSD__) or defined(__FB_OPENBSD__) or defined(__FB_NETBSD__)))
	#define FASTCALL
#else
	#define FASTCALL _fastcall
#endif

#define _cdecl
#define NTAPI
#define NTSYSAPI DECLSPEC_IMPORT

type VOID as any
type CHAR as zstring
type SHORT as short
type LONG as clong
type WCHAR as wstring
type PWCHAR as WCHAR ptr
type LPWCH as WCHAR ptr
type PWCH as WCHAR ptr
type LPCWCH as const WCHAR ptr
type PCWCH as const WCHAR ptr
type NWPSTR as WCHAR ptr
type LPWSTR as WCHAR ptr
type PWSTR as WCHAR ptr
type LPCWSTR as const WCHAR ptr
type PCWSTR as const WCHAR ptr
type PCHAR as CHAR ptr
type LPCH as CHAR ptr
type PCH as CHAR ptr
type LPCCH as const CHAR ptr
type PCCH as const CHAR ptr
type NPSTR as CHAR ptr
type LPSTR as CHAR ptr
type PSTR as CHAR ptr
type LPCSTR as const CHAR ptr
type PCSTR as const CHAR ptr
type TCHAR as zstring
type PTCHAR as zstring ptr
type TUCHAR as ubyte
type PTUCHAR as ubyte ptr
#define _TCHAR_DEFINED
type LPTCH as LPSTR
type PTCH as LPSTR
type PTSTR as LPSTR
type LPTSTR as LPSTR
type LPCTSTR as LPCSTR
#define __TEXT(quote) quote
#define TEXT(quote) __TEXT(quote)
type DOUBLE as double

type _QUAD
	DoNotUseThisField as double
end type

type QUAD as _QUAD
type PSHORT as SHORT ptr
type PLONG as LONG ptr
type PQUAD as QUAD ptr
#define BASETYPES
type UCHAR as ubyte
type USHORT as ushort
type ULONG as culong
type UQUAD as QUAD
type PUCHAR as UCHAR ptr
type PUSHORT as USHORT ptr
type PULONG as ULONG ptr
type PUQUAD as UQUAD ptr
type SCHAR as byte
type PSCHAR as SCHAR ptr
const STRICT = 1
type HANDLE as any ptr
'' TODO: #define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
type PHANDLE as HANDLE ptr
type FCHAR as UCHAR
type FSHORT as USHORT
type FLONG as ULONG
#define _HRESULT_DEFINED
type HRESULT as LONG
'' TODO: #define EXTERN_C extern

#ifdef __FB_WIN32__
	#define STDMETHODCALLTYPE __stdcall
	#define STDMETHODVCALLTYPE __cdecl
	#define STDAPICALLTYPE __stdcall
	#define STDAPIVCALLTYPE __cdecl
#else
	#define STDMETHODCALLTYPE __export __stdcall
	#define STDMETHODVCALLTYPE __export __cdecl
	#define STDAPICALLTYPE __export __stdcall
	#define STDAPIVCALLTYPE __export __cdecl
#endif

#define STDAPI EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type) EXTERN_C type STDAPICALLTYPE
'' TODO: #define STDMETHODIMP HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type) type STDMETHODCALLTYPE
#define STDAPIV EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type) EXTERN_C type STDAPIVCALLTYPE
'' TODO: #define STDMETHODIMPV HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_(type) type STDMETHODVCALLTYPE
const OBJ_HANDLE_TAGBITS = cast(clong, &h00000003)

type CCHAR as zstring
type CSHORT as short
type CLONG as ULONG
type PCCHAR as CCHAR ptr
type PCSHORT as CSHORT ptr
type PCLONG as CLONG ptr
type LCID as ULONG
type PLCID as PULONG
type LANGID as USHORT
type LOGICAL as ULONG
type PLOGICAL as ULONG ptr
type NTSTATUS as LONG
type PNTSTATUS as NTSTATUS ptr

#define NT_SUCCESS(Status) (cast(NTSTATUS, (Status)) >= 0)
#define NT_INFORMATION(Status) ((cast(ULONG, (Status)) shr 30) = 1)
#define NT_WARNING(Status) ((cast(ULONG, (Status)) shr 30) = 2)
#define NT_ERROR(Status) ((cast(ULONG, (Status)) shr 30) = 3)
const APPLICATION_ERROR_MASK = &h20000000
const ERROR_SEVERITY_SUCCESS = &h00000000
const ERROR_SEVERITY_INFORMATIONAL = &h40000000
const ERROR_SEVERITY_WARNING = &h80000000
const ERROR_SEVERITY_ERROR = &hC0000000
type TIME as LARGE_INTEGER
type PTIME as PLARGE_INTEGER
#define LowTime LowPart
#define HighTime HighPart

type _FLOAT128
	LowPart as longint
	HighPart as longint
end type

type FLOAT128 as _FLOAT128
type PFLOAT128 as FLOAT128 ptr
#define _ULONGLONG_

#if defined(__FB_DOS__) or ((not defined(__FB_64BIT__)) and (defined(__FB_DARWIN__) or defined(__FB_CYGWIN__) or ((not defined(__FB_ARM__)) and (defined(__FB_LINUX__) or defined(__FB_FREEBSD__) or defined(__FB_OPENBSD__) or defined(__FB_NETBSD__)))))
	type LONGLONG as double
	type ULONGLONG as double
#else
	type LONGLONG as longint
	'' TODO: typedef unsigned __int64 ULONGLONG;
	const MAXLONGLONG = &h7fffffffffffffff
#endif

type PLONGLONG as LONGLONG ptr
type PULONGLONG as ULONGLONG ptr
type USN as LONGLONG

type _LARGE_INTEGER_u
	LowPart as ULONG
	HighPart as LONG
end type

union _LARGE_INTEGER
	type
		LowPart as ULONG
		HighPart as LONG
	end type

	u as _LARGE_INTEGER_u
	QuadPart as LONGLONG
end union

type _TIME as _LARGE_INTEGER
type LARGE_INTEGER as _LARGE_INTEGER
type PLARGE_INTEGER as LARGE_INTEGER ptr

type _ULARGE_INTEGER_u
	LowPart as ULONG
	HighPart as ULONG
end type

union _ULARGE_INTEGER
	type
		LowPart as ULONG
		HighPart as ULONG
	end type

	u as _ULARGE_INTEGER_u
	QuadPart as ULONGLONG
end union

type ULARGE_INTEGER as _ULARGE_INTEGER
type PULARGE_INTEGER as ULARGE_INTEGER ptr

type _LUID
	LowPart as ULONG
	HighPart as LONG
end type

type LUID as _LUID
type PLUID as _LUID ptr
#define _DWORDLONG_
type DWORDLONG as ULONGLONG
type PDWORDLONG as DWORDLONG ptr
type PHYSICAL_ADDRESS as LARGE_INTEGER
type PPHYSICAL_ADDRESS as LARGE_INTEGER ptr

#if defined(__FB_DOS__) or ((not defined(__FB_64BIT__)) and (defined(__FB_DARWIN__) or defined(__FB_WIN32__) or defined(__FB_CYGWIN__) or ((not defined(__FB_ARM__)) and (defined(__FB_LINUX__) or defined(__FB_FREEBSD__) or defined(__FB_OPENBSD__) or defined(__FB_NETBSD__)))))
	#define Int32x32To64(a, b) cast(LONGLONG, cast(LONGLONG, cast(LONG, (a))) * cast(LONG, (b)))
#endif

#if defined(__FB_DOS__) or ((not defined(__FB_64BIT__)) and (defined(__FB_DARWIN__) or defined(__FB_CYGWIN__) or ((not defined(__FB_ARM__)) and (defined(__FB_LINUX__) or defined(__FB_FREEBSD__) or defined(__FB_OPENBSD__) or defined(__FB_NETBSD__)))))
	#define UInt32x32To64(a, b) cast(ULONGLONG, cast(ULONGLONG, cast(ULONG, (a))) * cast(ULONG, (b)))
#elseif defined(__FB_WIN32__) and (not defined(__FB_64BIT__))
	'' TODO: #define UInt32x32To64( a, b ) (ULONGLONG)((ULONGLONG)(ULONG)(a) * (ULONG)(b))
#endif

#if defined(__FB_DOS__) or ((not defined(__FB_64BIT__)) and (defined(__FB_DARWIN__) or defined(__FB_WIN32__) or defined(__FB_CYGWIN__) or ((not defined(__FB_ARM__)) and (defined(__FB_LINUX__) or defined(__FB_FREEBSD__) or defined(__FB_OPENBSD__) or defined(__FB_NETBSD__)))))
	declare function Int64ShllMod32(byval Value as ULONGLONG, byval ShiftCount as ULONG) as ULONGLONG
	declare function Int64ShraMod32(byval Value as LONGLONG, byval ShiftCount as ULONG) as LONGLONG
	declare function Int64ShrlMod32(byval Value as ULONGLONG, byval ShiftCount as ULONG) as ULONGLONG

	private function Int64ShllMod32(byval Value as ULONGLONG, byval ShiftCount as ULONG) as ULONGLONG
		'' TODO: __asm { mov ecx, ShiftCount mov eax, dword ptr [Value] mov edx, dword ptr [Value+4] shld edx, eax, cl shl eax, cl }
	end function

	private function Int64ShraMod32(byval Value as LONGLONG, byval ShiftCount as ULONG) as LONGLONG
		'' TODO: __asm { mov ecx, ShiftCount mov eax, dword ptr [Value] mov edx, dword ptr [Value+4] shrd eax, edx, cl sar edx, cl }
	end function

	private function Int64ShrlMod32(byval Value as ULONGLONG, byval ShiftCount as ULONG) as ULONGLONG
		'' TODO: __asm { mov ecx, ShiftCount mov eax, dword ptr [Value] mov edx, dword ptr [Value+4] shrd eax, edx, cl shr edx, cl }
	end function
#endif

type _EVENT_TYPE as long
enum
	NotificationEvent
	SynchronizationEvent
end enum

type EVENT_TYPE as _EVENT_TYPE

type _TIMER_TYPE as long
enum
	NotificationTimer
	SynchronizationTimer
end enum

type TIMER_TYPE as _TIMER_TYPE

type _WAIT_TYPE as long
enum
	WaitAll
	WaitAny
end enum

type WAIT_TYPE as _WAIT_TYPE
type PSZ as CHAR ptr
type PCSZ as const zstring ptr

type _STRING
	Length as USHORT
	MaximumLength as USHORT
	Buffer as PCHAR
end type

type STRING as _STRING
type PSTRING as STRING ptr
type ANSI_STRING as STRING
type PANSI_STRING as PSTRING
type OEM_STRING as STRING
type POEM_STRING as PSTRING

type _CSTRING
	Length as USHORT
	MaximumLength as USHORT
	Buffer as const zstring ptr
end type

type CSTRING as _CSTRING
type PCSTRING as CSTRING ptr
type CANSI_STRING as STRING
type PCANSI_STRING as PSTRING

type _UNICODE_STRING
	Length as USHORT
	MaximumLength as USHORT
	Buffer as PWSTR
end type

type UNICODE_STRING as _UNICODE_STRING
type PUNICODE_STRING as UNICODE_STRING ptr
type PCUNICODE_STRING as const UNICODE_STRING ptr
const UNICODE_NULL = cast(wchar_t, 0)
type BOOLEAN as UCHAR
type PBOOLEAN as BOOLEAN ptr

type _LIST_ENTRY
	Flink as _LIST_ENTRY ptr
	Blink as _LIST_ENTRY ptr
end type

type LIST_ENTRY as _LIST_ENTRY
type PLIST_ENTRY as _LIST_ENTRY ptr
type PRLIST_ENTRY as _LIST_ENTRY ptr

type _SINGLE_LIST_ENTRY
	Next as _SINGLE_LIST_ENTRY ptr
end type

type SINGLE_LIST_ENTRY as _SINGLE_LIST_ENTRY
type PSINGLE_LIST_ENTRY as _SINGLE_LIST_ENTRY ptr

type LIST_ENTRY32
	Flink as ULONG
	Blink as ULONG
end type

type PLIST_ENTRY32 as LIST_ENTRY32 ptr

type LIST_ENTRY64
	Flink as ULONG64
	Blink as ULONG64
end type

type PLIST_ENTRY64 as LIST_ENTRY64 ptr

type _UNICODE_STRING32
	Length as USHORT
	MaximumLength as USHORT
	Buffer as ULONG
end type

type UNICODE_STRING32 as _UNICODE_STRING32
type PUNICODE_STRING32 as UNICODE_STRING32 ptr

type _UNICODE_STRING64
	Length as USHORT
	MaximumLength as USHORT
	Buffer as ULONG64
end type

type UNICODE_STRING64 as _UNICODE_STRING64
type PUNICODE_STRING64 as UNICODE_STRING64 ptr
const OBJ_INHERIT = cast(clong, &h00000002)
const OBJ_PERMANENT = cast(clong, &h00000010)
const OBJ_EXCLUSIVE = cast(clong, &h00000020)
const OBJ_CASE_INSENSITIVE = cast(clong, &h00000040)
const OBJ_OPENIF = cast(clong, &h00000080)
const OBJ_OPENLINK = cast(clong, &h00000100)
const OBJ_VALID_ATTRIBUTES = cast(clong, &h000001F2)

type _OBJECT_ATTRIBUTES
	Length as ULONG
	RootDirectory as HANDLE
	ObjectName as PUNICODE_STRING
	Attributes as ULONG
	SecurityDescriptor as PVOID
	SecurityQualityOfService as PVOID
end type

type OBJECT_ATTRIBUTES as _OBJECT_ATTRIBUTES
type POBJECT_ATTRIBUTES as OBJECT_ATTRIBUTES ptr
#macro InitializeObjectAttributes(p, n, a, r, s)
	scope
		(p)->Length = sizeof(OBJECT_ATTRIBUTES)
		(p)->RootDirectory = r
		(p)->Attributes = a
		(p)->ObjectName = n
		(p)->SecurityDescriptor = s
		(p)->SecurityQualityOfService = NULL
	end scope
#endmacro
const FALSE = 0
const TRUE = 1
const NULL = cptr(any ptr, 0)
'' TODO: #define NULL64 ((void * POINTER_64)0)
#define GUID_DEFINED

type _GUID
	Data1 as ULONG
	Data2 as USHORT
	Data3 as USHORT
	Data4(0 to 7) as UCHAR
end type

type GUID as _GUID
#define __OBJECTID_DEFINED

type _OBJECTID
	Lineage as GUID
	Uniquifier as ULONG
end type

type OBJECTID as _OBJECTID
const MINCHAR = &h80
const MAXCHAR = &h7f
const MINSHORT = &h8000
const MAXSHORT = &h7fff
const MINLONG = &h80000000
const MAXLONG = &h7fffffff
const MAXUCHAR = &hff
const MAXUSHORT = &hffff
const MAXULONG = &hffffffff
#define ARGUMENT_PRESENT(ArgumentPointer) (cptr(CHAR ptr, (ArgumentPointer)) <> cptr(CHAR ptr, NULL))
#define FIELD_OFFSET(type, field) (cast(LONG, INT_PTR) and cptr(type ptr, 0)->field)
#define CONTAINING_RECORD(address, type, field) cptr(type ptr, cast(PCHAR, (address)) - UINT_PTR(@cptr(type ptr, 0)->field))

type PEXCEPTION_ROUTINE as function(byval ExceptionRecord as _EXCEPTION_RECORD ptr, byval EstablisherFrame as PVOID, byval ContextRecord as _CONTEXT ptr, byval DispatcherContext as PVOID) as EXCEPTION_DISPOSITION
type KIRQL as UCHAR
type PKIRQL as KIRQL ptr

type _NT_PRODUCT_TYPE as long
enum
	NtProductWinNt = 1
	NtProductLanManNt
	NtProductServer
end enum

type NT_PRODUCT_TYPE as _NT_PRODUCT_TYPE
type PNT_PRODUCT_TYPE as _NT_PRODUCT_TYPE ptr

type _SUITE_TYPE as long
enum
	SmallBusiness
	Enterprise
	BackOffice
	CommunicationServer
	TerminalServer
	MaxSuiteType
end enum

type SUITE_TYPE as _SUITE_TYPE
const LANG_NEUTRAL = &h00
const LANG_AFRIKAANS = &h36
const LANG_ALBANIAN = &h1c
const LANG_ARABIC = &h01
const LANG_ARMENIAN = &h2b
const LANG_ASSAMESE = &h4d
const LANG_AZERI = &h2c
const LANG_BASQUE = &h2d
const LANG_BELARUSIAN = &h23
const LANG_BENGALI = &h45
const LANG_BULGARIAN = &h02
const LANG_CATALAN = &h03
const LANG_CHINESE = &h04
const LANG_CROATIAN = &h1a
const LANG_CZECH = &h05
const LANG_DANISH = &h06
const LANG_DUTCH = &h13
const LANG_ENGLISH = &h09
const LANG_ESTONIAN = &h25
const LANG_FAEROESE = &h38
const LANG_FARSI = &h29
const LANG_FINNISH = &h0b
const LANG_FRENCH = &h0c
const LANG_GEORGIAN = &h37
const LANG_GERMAN = &h07
const LANG_GREEK = &h08
const LANG_GUJARATI = &h47
const LANG_HEBREW = &h0d
const LANG_HINDI = &h39
const LANG_HUNGARIAN = &h0e
const LANG_ICELANDIC = &h0f
const LANG_INDONESIAN = &h21
const LANG_ITALIAN = &h10
const LANG_JAPANESE = &h11
const LANG_KANNADA = &h4b
const LANG_KAZAK = &h3f
const LANG_KONKANI = &h57
const LANG_KOREAN = &h12
const LANG_LATVIAN = &h26
const LANG_LITHUANIAN = &h27
const LANG_MACEDONIAN = &h2f
const LANG_MALAY = &h3e
const LANG_MALAYALAM = &h4c
const LANG_MARATHI = &h4e
const LANG_NORWEGIAN = &h14
const LANG_ORIYA = &h48
const LANG_POLISH = &h15
const LANG_PORTUGUESE = &h16
const LANG_PUNJABI = &h46
const LANG_ROMANIAN = &h18
const LANG_RUSSIAN = &h19
const LANG_SANSKRIT = &h4f
const LANG_SERBIAN = &h1a
const LANG_SLOVAK = &h1b
const LANG_SLOVENIAN = &h24
const LANG_SPANISH = &h0a
const LANG_SWAHILI = &h41
const LANG_SWEDISH = &h1d
const LANG_TAMIL = &h49
const LANG_TATAR = &h44
const LANG_TELUGU = &h4a
const LANG_THAI = &h1e
const LANG_TURKISH = &h1f
const LANG_UKRAINIAN = &h22
const LANG_UZBEK = &h43
const LANG_VIETNAMESE = &h2a
const SUBLANG_NEUTRAL = &h00
const SUBLANG_DEFAULT = &h01
const SUBLANG_SYS_DEFAULT = &h02
const SUBLANG_ARABIC_SAUDI_ARABIA = &h01
const SUBLANG_ARABIC_IRAQ = &h02
const SUBLANG_ARABIC_EGYPT = &h03
const SUBLANG_ARABIC_LIBYA = &h04
const SUBLANG_ARABIC_ALGERIA = &h05
const SUBLANG_ARABIC_MOROCCO = &h06
const SUBLANG_ARABIC_TUNISIA = &h07
const SUBLANG_ARABIC_OMAN = &h08
const SUBLANG_ARABIC_YEMEN = &h09
const SUBLANG_ARABIC_SYRIA = &h0a
const SUBLANG_ARABIC_JORDAN = &h0b
const SUBLANG_ARABIC_LEBANON = &h0c
const SUBLANG_ARABIC_KUWAIT = &h0d
const SUBLANG_ARABIC_UAE = &h0e
const SUBLANG_ARABIC_BAHRAIN = &h0f
const SUBLANG_ARABIC_QATAR = &h10
const SUBLANG_AZERI_LATIN = &h01
const SUBLANG_AZERI_CYRILLIC = &h02
const SUBLANG_CHINESE_TRADITIONAL = &h01
const SUBLANG_CHINESE_SIMPLIFIED = &h02
const SUBLANG_CHINESE_HONGKONG = &h03
const SUBLANG_CHINESE_SINGAPORE = &h04
const SUBLANG_CHINESE_MACAU = &h05
const SUBLANG_DUTCH = &h01
const SUBLANG_DUTCH_BELGIAN = &h02
const SUBLANG_ENGLISH_US = &h01
const SUBLANG_ENGLISH_UK = &h02
const SUBLANG_ENGLISH_AUS = &h03
const SUBLANG_ENGLISH_CAN = &h04
const SUBLANG_ENGLISH_NZ = &h05
const SUBLANG_ENGLISH_EIRE = &h06
const SUBLANG_ENGLISH_SOUTH_AFRICA = &h07
const SUBLANG_ENGLISH_JAMAICA = &h08
const SUBLANG_ENGLISH_CARIBBEAN = &h09
const SUBLANG_ENGLISH_BELIZE = &h0a
const SUBLANG_ENGLISH_TRINIDAD = &h0b
const SUBLANG_ENGLISH_ZIMBABWE = &h0c
const SUBLANG_ENGLISH_PHILIPPINES = &h0d
const SUBLANG_FRENCH = &h01
const SUBLANG_FRENCH_BELGIAN = &h02
const SUBLANG_FRENCH_CANADIAN = &h03
const SUBLANG_FRENCH_SWISS = &h04
const SUBLANG_FRENCH_LUXEMBOURG = &h05
const SUBLANG_FRENCH_MONACO = &h06
const SUBLANG_GERMAN = &h01
const SUBLANG_GERMAN_SWISS = &h02
const SUBLANG_GERMAN_AUSTRIAN = &h03
const SUBLANG_GERMAN_LUXEMBOURG = &h04
const SUBLANG_GERMAN_LIECHTENSTEIN = &h05
const SUBLANG_ITALIAN = &h01
const SUBLANG_ITALIAN_SWISS = &h02
const SUBLANG_KOREAN = &h01
const SUBLANG_LITHUANIAN = &h01
const SUBLANG_LITHUANIAN_CLASSIC = &h02
const SUBLANG_MALAY_MALAYSIA = &h01
const SUBLANG_MALAY_BRUNEI_DARUSSALAM = &h02
const SUBLANG_NORWEGIAN_BOKMAL = &h01
const SUBLANG_NORWEGIAN_NYNORSK = &h02
const SUBLANG_PORTUGUESE = &h02
const SUBLANG_PORTUGUESE_BRAZILIAN = &h01
const SUBLANG_SERBIAN_LATIN = &h02
const SUBLANG_SERBIAN_CYRILLIC = &h03
const SUBLANG_SPANISH = &h01
const SUBLANG_SPANISH_MEXICAN = &h02
const SUBLANG_SPANISH_MODERN = &h03
const SUBLANG_SPANISH_GUATEMALA = &h04
const SUBLANG_SPANISH_COSTA_RICA = &h05
const SUBLANG_SPANISH_PANAMA = &h06
const SUBLANG_SPANISH_DOMINICAN_REPUBLIC = &h07
const SUBLANG_SPANISH_VENEZUELA = &h08
const SUBLANG_SPANISH_COLOMBIA = &h09
const SUBLANG_SPANISH_PERU = &h0a
const SUBLANG_SPANISH_ARGENTINA = &h0b
const SUBLANG_SPANISH_ECUADOR = &h0c
const SUBLANG_SPANISH_CHILE = &h0d
const SUBLANG_SPANISH_URUGUAY = &h0e
const SUBLANG_SPANISH_PARAGUAY = &h0f
const SUBLANG_SPANISH_BOLIVIA = &h10
const SUBLANG_SPANISH_EL_SALVADOR = &h11
const SUBLANG_SPANISH_HONDURAS = &h12
const SUBLANG_SPANISH_NICARAGUA = &h13
const SUBLANG_SPANISH_PUERTO_RICO = &h14
const SUBLANG_SWEDISH = &h01
const SUBLANG_SWEDISH_FINLAND = &h02
const SUBLANG_UZBEK_LATIN = &h01
const SUBLANG_UZBEK_CYRILLIC = &h02
const SORT_DEFAULT = &h0
const SORT_JAPANESE_XJIS = &h0
const SORT_JAPANESE_UNICODE = &h1
const SORT_CHINESE_BIG5 = &h0
const SORT_CHINESE_PRCP = &h0
const SORT_CHINESE_UNICODE = &h1
const SORT_CHINESE_PRC = &h2
const SORT_CHINESE_BOPOMOFO = &h3
const SORT_KOREAN_KSC = &h0
const SORT_KOREAN_UNICODE = &h1
const SORT_GERMAN_PHONE_BOOK = &h1
const SORT_HUNGARIAN_DEFAULT = &h0
const SORT_HUNGARIAN_TECHNICAL = &h1
const SORT_GEORGIAN_TRADITIONAL = &h0
const SORT_GEORGIAN_MODERN = &h1
#define MAKELANGID(p, s) ((cast(USHORT, (s)) shl 10) or cast(USHORT, (p)))
#define PRIMARYLANGID(lgid) (cast(USHORT, (lgid)) and &h3ff)
#define SUBLANGID(lgid) (cast(USHORT, (lgid)) shr 10)
const NLS_VALID_LOCALE_MASK = &h000fffff
#define MAKELCID(lgid, srtid) cast(ULONG, (cast(ULONG, cast(USHORT, (srtid))) shl 16) or cast(ULONG, cast(USHORT, (lgid))))
#define MAKESORTLCID(lgid, srtid, ver) cast(ULONG, MAKELCID(lgid, srtid) or (cast(ULONG, cast(USHORT, (ver))) shl 20))
#define LANGIDFROMLCID(lcid) cast(USHORT, (lcid))
#define SORTIDFROMLCID(lcid) cast(USHORT, (cast(ULONG, (lcid)) shr 16) and &hf)
#define SORTVERSIONFROMLCID(lcid) cast(USHORT, (cast(ULONG, (lcid)) shr 20) and &hf)
#define LANG_SYSTEM_DEFAULT MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT)
#define LANG_USER_DEFAULT MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
#define LOCALE_SYSTEM_DEFAULT MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT)
#define LOCALE_USER_DEFAULT MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT)
#define LOCALE_NEUTRAL MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT)
#define UNREFERENCED_PARAMETER(P) (P)
#define DBG_UNREFERENCED_PARAMETER(P) (P)
#define DBG_UNREFERENCED_LOCAL_VARIABLE(V) (V)

end extern
