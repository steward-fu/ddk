// Help file identifiers - #include in .HPJ file for TTY.HLP.

#define IDD_MODELNAME_LABEL         101
#define IDD_MODELNAME               102

// Device code group identifiers

#define IDD_PRINTERCODE_GROUP       110
#define IDD_BEGINJOB_LABEL          111  
#define IDD_BEGINJOB_ESC            112
#define IDD_ENDJOB_LABEL            113
#define IDD_ENDJOB_ESC              114
#define IDD_PAPERSIZE_LABEL         115
#define IDD_PAPERSIZE_ESC           116
#define IDD_PAPERSRC_LABEL          117
#define IDD_PAPERSRC_ESC            118

// Extended character group identifiers

#define IDD_EXTENDEDCHAR_GROUP      120
#define IDD_SCREENCHAR_LABEL        121
#define IDD_SCREENCHAR              122
#define IDD_PRINTCHAR_LABEL         123
#define IDD_PRINTCHAR               124

#define IDD_CHARMAP_LABEL           140
#define IDD_CUSTOMCHARSET           141
#define IDD_437CHARSET              142
#define IDD_850CHARSET              143

// Identifiers from Add Model dialog
#define TTY_ADDMODEL_DLG            150
#define IDD_NEW_MODELNAME           151
#define IDD_NEW_MODEL_LABEL         152

// Font code group identifiers

#define IDD_FONTCODE_GROUP          210
#define IDD_10CPI_LABEL             211  
#define IDD_10CPI_ESC               212
#define IDD_12CPI_LABEL             213 
#define IDD_12CPI_ESC               214
#define IDD_17CPI_LABEL             215 
#define IDD_17CPI_ESC               216
#define IDD_DBLON_LABEL             217 
#define IDD_DBLON_ESC               218
#define IDD_DBLOFF_LABEL            219 
#define IDD_DBLOFF_ESC              220
#define IDD_BOLDON_LABEL            221
#define IDD_BOLDON_ESC              222
#define IDD_BOLDOFF_LABEL           223      
#define IDD_BOLDOFF_ESC             224 
#define IDD_ULINEON_LABEL           225
#define IDD_ULINEON_ESC             226
#define IDD_ULINEOFF_LABEL          227
#define IDD_ULINEOFF_ESC            228
                                     
// Device options propertysheet identifiers

#define IDD_ADD                     250
#define IDD_DELETE                  251

// Help file identifiers for the above elements in the propertysheet UI.

#define IDH_TTY_MODELNAME       0x00007001 
#define IDH_TTY_PRINTERCODES    0x00007002
#define IDH_TTY_BEGINJOB        0x00007003
#define IDH_TTY_ENDJOB          0x00007004
#define IDH_TTY_PAPERSIZE       0x00007005
#define IDH_TTY_PAPERSRC        0x00007006
#define IDH_TTY_ADD_BUTTON      0x00007007 
#define IDH_TTY_DELETE_BUTTON   0x00007008
#define IDH_TTY_FONTCODES       0x00007009
#define IDH_TTY_10CPI           0x0000700A
#define IDH_TTY_12CPI           0x0000700B
#define IDH_TTY_17CPI           0x0000700C
#define IDH_TTY_2X_ON           0x0000700D
#define IDH_TTY_2X_OFF          0x0000700E
#define IDH_TTY_BOLD_ON         0x0000700F
#define IDH_TTY_BOLD_OFF        0x00007010
#define IDH_TTY_ULINE_ON        0x00007011
#define IDH_TTY_ULINE_OFF       0x00007012
#define IDH_TTY_EXTENDEDCHAR    0x00007013
#define IDH_TTY_CUSTOMCHARSET   0x00007014
#define IDH_TTY_437CHARSET      0x00007015
#define IDH_TTY_850CHARSET      0x00007016 
#define IDH_TTY_ON_SCREEN_ON_PRINTER  0x00007017
#define IDH_TTY_ADD_PRINTER     0x00007018
                                
// Help control id array.

static DWORD far aKeyWordIds[] =
{
       IDD_MODELNAME_LABEL,    IDH_TTY_MODELNAME,
       IDD_MODELNAME,          IDH_TTY_MODELNAME,
       IDD_PRINTERCODE_GROUP,  IDH_TTY_PRINTERCODES,
       IDD_BEGINJOB_LABEL,     IDH_TTY_BEGINJOB,
       IDD_BEGINJOB_ESC,       IDH_TTY_BEGINJOB,
       IDD_ENDJOB_LABEL,       IDH_TTY_ENDJOB,
       IDD_ENDJOB_ESC,         IDH_TTY_ENDJOB,
       IDD_PAPERSIZE_LABEL,    IDH_TTY_PAPERSIZE,
       IDD_PAPERSIZE_ESC,      IDH_TTY_PAPERSIZE,
       IDD_PAPERSRC_LABEL,     IDH_TTY_PAPERSRC,
       IDD_PAPERSRC_ESC,       IDH_TTY_PAPERSRC,
       IDD_EXTENDEDCHAR_GROUP, IDH_TTY_EXTENDEDCHAR,
       IDD_SCREENCHAR_LABEL,   IDH_TTY_ON_SCREEN_ON_PRINTER,
       IDD_SCREENCHAR,         IDH_TTY_ON_SCREEN_ON_PRINTER,
       IDD_PRINTCHAR_LABEL,    IDH_TTY_ON_SCREEN_ON_PRINTER,
       IDD_PRINTCHAR,          IDH_TTY_ON_SCREEN_ON_PRINTER,
       IDD_CHARMAP_LABEL,      IDH_TTY_EXTENDEDCHAR,
       IDD_CUSTOMCHARSET,      IDH_TTY_CUSTOMCHARSET,
       IDD_437CHARSET,         IDH_TTY_437CHARSET,
       IDD_850CHARSET,         IDH_TTY_850CHARSET,
       IDD_ADD,                IDH_TTY_ADD_BUTTON,
       IDD_DELETE,             IDH_TTY_DELETE_BUTTON,
       IDD_FONTCODE_GROUP,     IDH_TTY_FONTCODES,       
       IDD_10CPI_LABEL,        IDH_TTY_10CPI,
       IDD_10CPI_ESC,          IDH_TTY_10CPI,
       IDD_12CPI_LABEL,        IDH_TTY_12CPI,
       IDD_12CPI_ESC,          IDH_TTY_12CPI,
       IDD_17CPI_LABEL,        IDH_TTY_17CPI,
       IDD_17CPI_ESC,          IDH_TTY_17CPI,
       IDD_DBLON_LABEL,        IDH_TTY_2X_ON,
       IDD_DBLON_ESC,          IDH_TTY_2X_ON,
       IDD_DBLOFF_LABEL,       IDH_TTY_2X_OFF,
       IDD_DBLOFF_ESC,         IDH_TTY_2X_OFF,
       IDD_BOLDON_LABEL,       IDH_TTY_BOLD_ON,
       IDD_BOLDON_ESC,         IDH_TTY_BOLD_ON,
       IDD_BOLDOFF_LABEL,      IDH_TTY_BOLD_OFF,
       IDD_BOLDOFF_ESC,        IDH_TTY_BOLD_OFF,
       IDD_ULINEON_LABEL,      IDH_TTY_ULINE_ON,
       IDD_ULINEON_ESC,        IDH_TTY_ULINE_ON,
       IDD_ULINEOFF_LABEL,     IDH_TTY_ULINE_OFF,
       IDD_ULINEOFF_ESC,       IDH_TTY_ULINE_OFF,
       IDD_NEW_MODELNAME,      IDH_TTY_ADD_PRINTER,
       IDD_NEW_MODEL_LABEL,    IDH_TTY_ADD_PRINTER,
       0,0
};

