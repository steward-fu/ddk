
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FILTERDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FILTERDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef FILTERDLL_EXPORTS
#define FILTERDLL_API __declspec(dllexport)
#else
#define FILTERDLL_API __declspec(dllimport)
#endif


extern "C" {

FILTERDLL_API DWORD Install(void);

FILTERDLL_API DWORD Uninstall(void);

}

