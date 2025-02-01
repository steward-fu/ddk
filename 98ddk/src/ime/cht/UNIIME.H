DWORD   WINAPI UniSearchPhrasePredictionW(UINT, LPCTSTR, DWORD,
               LPCTSTR, DWORD, DWORD, DWORD, DWORD,
               LPCANDIDATELIST);                                // uniphrs.c

DWORD   WINAPI UniSearchPhrasePredictionA(UINT, LPCTSTR, DWORD,
               LPCTSTR, DWORD, DWORD, DWORD, DWORD,
               LPCANDIDATELIST);                                // uniphrs.c

#ifdef UNICODE
#define UniSearchPhrasePrediction   UniSearchPhrasePredictionW
#else
#define UniSearchPhrasePrediction   UniSearchPhrasePredictionA
#endif
