#include <math.h>

#define PI 3.1415926

/* definitions */
#define DD_MIN_BRIGHT	  0L
#define DD_MAX_BRIGHT 10000L
#define DD_DEF_BRIGHT   750L
#define HW_MIN_BRIGHT  -128L  /* SBBBBBBB */
#define HW_MAX_BRIGHT	127L
#define HW_DEF_BRIGHT	  0L

#define DD_MIN_CONT		   0L
#define DD_MAX_CONT	   20000L
#define DD_DEF_CONT	   10000L
#define HW_MIN_CONT		   0L  /* Multiplied C.CCCC range 0..1.9375 by 16 */
#define HW_MAX_CONT	      31L  /* in order to shift decimal right */
#define HW_DEF_CONT		  16L  /* by 4 binary places */

#define DD_MIN_HUE		-180L  /* Hue is in degrees with cos ranging -1..1 */
#define DD_MAX_HUE		 180L
#define DD_DEF_HUE		   0L
#define HW_MIN_HUE		-180L
#define HW_MAX_HUE		 180L
#define HW_DEF_HUE		   0L

#define DD_MIN_SAT		   0L
#define DD_MAX_SAT	   20000L
#define DD_DEF_SAT	   10000L
#define HW_MIN_SAT		 -16L  /* Multiplied SF.FFF range -2..1.875 by 8 */
#define HW_MAX_SAT	      15L  /* in order to shift decimal right */
#define HW_DEF_SAT		   8L  /* by 3 binary places */


/* prototypes */
LONG HWtoDD_Bright(DWORD dwReg);
LONG HWtoDD_Cont(DWORD dwReg);
LONG HWtoDD_Hue(DWORD dwReg);
LONG HWtoDD_Sat(DWORD dwReg);
DWORD DDtoHW_BrightCont(LONG dwDDBright, LONG dwDDCont);
DWORD DDtoHW_HueSat(LONG dwDDHue, LONG dwDDSat);







