#include <windows.h>
#include "imm.h"
#include "imedefs.h"

#pragma data_seg(".ShareDate")
IMEG      sImeG = {0};

CHAR SKLayout[NumsSK][48*2] = {
    "¡¡£°£±£²£³£´£µ£¶£·£¸£¹£á£â£ã£ä£å£æ£ç£è£é£ê£ë£ì£í£î£ï£ð£ñ£ò£ó£ô£õ£ö£÷£ø£ù£ú£»£½£¬£­£®£¯£à£Û£Ü£Ý£§",
	"¡¡                    ¦Ê¦Ö¦Ô¦Ì¦Ã¦Í¦Î¦Ï¦È¦Ð¦Ñ  ¦Ø¦×¦É  ¦Á¦Ä¦Ë¦Å¦Ç¦Õ¦Â¦Ó¦Æ¦Ò                      ",
	"¡¡                    §Ý§ì§ê§ß§Ó§à§á§â§Ø§ã§ä§å§î§í§Ù§Ú§Ñ§Ô§Þ§Õ§×§ë§Ò§é§Ö§è§æ  §ï  §ð§ñ  §Û  §Ü§ç",
	"¡¡¨æ¨É    ¨Ó    ¨Ú¨Þ¨â¨Ç¨Ö¨Ï¨Î¨Í¨Ñ¨Õ¨Ø¨Û¨è¨Ü¨à¨é¨Ù¨ß¨ã¨Æ¨Ð¨Ë¨Ô¨ç¨Ò¨Ê  ¨×¨È¨ä  ¨Ý  ¨á¨å¨Å        ",
	"¡¡                    ¨¥  ¨³¨§¨£¨¨  ¨©¨¯¨ª¨«¨¬¨¶¨µ¨°  ¨¡¨¤¨¦  ¨®¨´¨¢¨²¨­¨±    ¨·  ¨¸¨¹  ¨º      ",
	"¡¡  ¤£¤¥¤§¤©¤«¤­¤¯¤±¤³¤Ê¤â¤à¤Ì¤¹¤Í¤Î¤Ï¤Ä¤Ò¤Õ¤Ø¤å¤ã¤Ã¤Æ¤µ¤»¤Ë¤½¤Á¤á¤·¤ß¤¿¤Þ¤Û  ¤ç¤ó¤î¤ò¤¡¤È  ¤ð¤ñ",
	"¡¡¥±¥£¥¥¥ô¥§¥©¥«¥õ¥­¥¯¥Ê¥â¥à¥Ì¥¹¥Í¥Î¥Ï¥Ä¥Ò¥Õ¥Ø¥å¥ã¥Ã¥Æ¥µ¥»¥Ë¥½¥Á¥á¥·¥ß¥¿¥Þ¥Û¥³¥ç¥ö¥î¥ò¥¡¥È¥ó¥ð¥ñ",
	"¡¡¡¥£¬¡¢£»£º£¿£¡¡­¡ª¡¤¡²£¨¡¾¡´¡°¡µ¡¶¡·¡Ã¡¸¡¹¡º£Û£©£¢£§¡®¡±¡³¡©¡¬¡¿¡¯¡½¡«¡¼¡»¡§£Ý¡¦£û£ý¡££à¡¨£ü£®",
	"¡¡¢û¢ò¢ó¢ô¢õ¢ö¢÷¢ø¢ù¢ú¢å¢É¢Ç¢ç¢³¢è¢é¢ê¢¸¢ë¢ì¢í¢Ë¢Ê¢¹¢º¢±¢´¢æ¢µ¢·¢È¢²¢Æ¢¶¢Å¢î  ¢Ì¢ü¢Í¢Î¢ñ        ",
	"¡¡  ¡Ô¡Ù£½¡Ü¡Ý£¼£¾¡Ú¡Û¡Ä¡Ñ¡Ï¡Æ£­¡Ç¡È¡É¡Ò¡Ê  ¡ß¡×¡Õ¡Ó¡Ø¡À¡Á¡Å¡Â  ¡Ð£«¡Î£¯¡Í¡à    ¡Ë¡Ì  ¡Ö¡Þ      ",
	"¡¡¡è¡ã¡ä¡å¡ç¡ê£¤¡ë£¥¡æ°Ù  ºÁÍò¶þÒÚÕ×¼ªÆßÌ«ÅÄ°¬    °Ë¾Å¡ðÈýÇ§ËÄÁùÎ¢Ò»ÀåÎå·Ö      ¡é      Ê®      ",
	"¡¡©¨©±©²©³©´©µ©¶©·  ©¤©Ð©ä©â©Ò©Â©Ó©Ô©Õ©Ç©Ö©×  ©æ©å  ©¦©À©Ã©Ñ©Ä©Æ©ã©Á©á©Å©à    ©ç©¬    ©°©ª  ©®  ",
	"¡¡                    ¡ö£Þ£À¡ø¡î¡ù¡ú¡û¡ó¡ü¡ý¡þ£þ£ß¡ô¡õ¡ì¡ï¡÷¡ð¡ò£Ü¡í£¦¡ñ££                      ",
    };
CHAR SKLayoutS[NumsSK][48*2] = {
	"¡¡£©£¡£À££¡ç£¥£Þ£¦£ª£¨£Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï£Ð£Ñ£Ò£Ó£Ô£Õ£Ö£×£Ø£Ù£Ú£º£«£¼£ß£¾£¿¡«£û£ü£ý£¢",
	"                      ¦ª¦¶¦´¦¬¦£¦­¦®¦¯¦¨¦°¦±  ¦¸¦·¦©  ¦¡¦¤¦«¦¥¦§¦µ¦¢¦³¦¦¦²                      ",
	"                      §­§¼§º§¯§£§°§±§²§¨§³§´§µ§¾§½§©§ª§¡§¤§®§¥§§§»§¢§¹§¦§¸§¶  §¿  §À§Á  §«  §¬§·",
	"                                                                                                ",
	"                                                                                                ",
	"    ¤¤¤¦¤¨¤ª¤¬¤®¤°¤²¤´¤Ñ¤í¤ë¤×¤º¤Ú¤Ý¤Ü¤Å¤Ó¤Ö¤Ù¤æ¤ä  ¤Ç¤¶¤¼¤Ô¤¾¤Â¤ì¤¸¤ê¤À¤é¤Ü  ¤è  ¤ï  ¤¢¤É      ",
	"  ¥²¥¤¥¦  ¥¨¥ª¥¬  ¥®¥°¥Ñ¥í¥ë¥×¥º¥Ú¥Ý¥Ð¥Å¥Ó¥Ö¥Ù¥æ¥ä  ¥Ç¥¶¥¼¥Ô¥¾¥Â¥ì¥¸¥ê¥À¥é¥Ü¥´¥è  ¥ï  ¥¢¥É      ",
	"                                                                                                ",
	"                      ¢Ù¢Ó¢Ñ¢Û¢½¢Ü¢Ý¢Þ¢Â¢ß¢à¢á¢Õ¢Ô¢Ã¢Ä¢»¢¾¢Ú¢¿¢Á¢Ò¢¼¢Ð¢À¢Ï¢â  ¢Ö  ¢×¢Ø          ",
	"                                                                                                ",
	"                      °Û      ·¡      Æâ          °Æ¾ÁÁãÈþÇªËÁÂ½  Ò¼  Îé                Ê°      ",
	"  ©©©¹©º©»©¼©½©¾©¿  ©¥©Ø©ì©ê©Ú©Ê©Û©Ü©Ý©Ï©Þ©ß  ©î©í  ©§©È©Ë©Ù©Ì©Î©ë©É©é©Í©è    ©ï©­    ©¸©«  ©¯  ",
	"                                                                                                ",
	};
#pragma data_seg()

HDC   ST_UI_hDC;
UINT  uStartComp = 0;
UINT  uOpenCand = 0;
UINT  uCaps = 0;
DWORD SaTC_Trace;

HINSTANCE hInst;
IMEL      sImeL;
LPIMEL    lpImeL;
BYTE      szUIClassName[CLASS_LEN];
BYTE      szCompClassName[CLASS_LEN];
BYTE      szCandClassName[CLASS_LEN];
BYTE      szStatusClassName[CLASS_LEN];
BYTE      szCMenuClassName[CLASS_LEN];			
BYTE      szSoftkeyMenuClassName[CLASS_LEN];	
BYTE      szHandCursor[] = "Hand";
BYTE      szChinese[] = "Chinese";
BYTE      szEnglish[] = "English";
TCHAR		pszImeName[IMEINDEXNUM][MAX_PATH]={
			"ÇøÎ»",
			"GBKÄÚÂë",
			"UNICODE"};
TCHAR		*szImeName;
TCHAR		szImeRegName[]="ÄÚÂë";
BYTE      szCode[] = "Code";
BYTE      szEudc[] = "Eudc";
BYTE      szFullShape[] = "FullShape";
BYTE      szHalfShape[] = "HalfShape";
BYTE      szSymbol[] = "Symbol";
BYTE      szNoSymbol[] = "NoSymbol";
BYTE      szNone[] = "None";
BYTE      szSoftKBD[] = "SoftKBD";
BYTE      szNoSoftKBD[] = "NoSoftKBD";
BYTE      szDigit[] = "01234567890";
BYTE      szCandInf[] = "   Êý×Ö¼ü";
BYTE	  szTrace[]= "¹â±ê¸úËæ";
BYTE szRegRevKL[] = "±àÂë²éÑ¯";
BYTE szRegRevMaxKey[] = "±àÂë²éÑ¯Âë³¤";
BYTE	  szWarnTitle[]="¾¯¸æ";	
BYTE	  szErrorTitle[]="´íÎó";

// convert char to upper case
BYTE bUpper[] = {
// 0x20 - 0x27
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
// 0x28 - 0x2F
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
// 0x30 - 0x37
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
// 0x38 - 0x3F
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
// 0x40 - 0x47
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
// 0x48 - 0x4F
    0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
// 0x50 - 0x57
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
// 0x58 - 0x5F
    0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
//   '    a    b    c    d    e    f    g 
    '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
//   h    i    j    k    l    m    n    o
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
//   p    q    r    s    t    u    v    w
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
//   x    y    z    {    |    }    ~
    'X', 'Y', 'Z'
};

WORD fMask[] = {         // offset of bitfield
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
};

BYTE szRegIMESetting[] = "Control Panel\\Input Method";
BYTE szPara[] = "Parallel Distance";
BYTE szPerp[] = "Perpendicular Distance";
BYTE szParaTol[] = "Parallel Tolerance";
BYTE szPerpTol[] = "Perpendicular Tolerance";
BYTE szRegImeIndex[] = "IME Index";

const NEARCARET ncUIEsc[] = {
   // LogFontX  LogFontY  ParaX   PerpX   ParaY   PerpY
    { 0,        1,        1,      0,      0,      1},       // 0
    { 1,        0,        0,      1,      1,      0},       // 900
    { 0,        0,       -1,      0,      0,      1},       // 1800
    {-1,        0,        0,     -1,     -1,      0}        // 2700
};
// decide another UI offset base on escapement
const NEARCARET ncAltUIEsc[] = {
   // LogFontX  LogFontY  ParaX   PerpX   ParaY   PerpY
    { 0,        0,        1,      0,      0,     -1},       // 0
    { 0,        0,        0,     -1,      1,      0},       // 900
    { 0,        0,       -1,      0,      0,     -1},       // 1800
    { 0,        0,        0,      1,     -1,      0}        // 2700
};
// decide input rectangle base on escapement
const POINT ptInputEsc[] = {
    // LogFontWi   LogFontHi
    {1,            1},                                  // 0
    {1,           -1},                                  // 900
    {1,            1},                                  // 1800
    {-1,           1}                                   // 2700
};

// decide another input rectangle base on escapement
const POINT ptAltInputEsc[] = {
    // LogFontWi   LogFontHi
    {1,           -1},                                  // 0
    {-1,          -1},                                  // 900
    {1,           -1},                                  // 1800
    {1,            1}                                   // 2700
};


BYTE VirtKey48Map[48] = {
	0x20, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53,
	0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0xba, 0xbb, 0xbc,
	0xbd, 0xbe, 0xbf, 0xc0, 0xdb, 0xdc, 0xdd, 0xde
};
