#include <stddef.h>
#include <stdlib.h>
#include <windows.h>
#include <winddi.h>

#define _WINDEFP_NO_PDEVBRUSH
#include <winspool.h>
#include <plotgpc.h>
#include <plotdm.h>
#include <plotters.h>
#include <plotlib.h>
#include <string.h>

#include "enable.h"
#include "bitblt.h"
#include "pencolor.h"
#include "polygon.h"
#include "ropblt.h"
#include "output.h"
#include "htblt.h"
#include "plotform.h"
#include "escape.h"
#include "compress.h"
#include "htbmp1.h"
#include "htbmp4.h"
#include "transpos.h"
#include "brush.h"
#include "path.h"
#include "textout.h"

