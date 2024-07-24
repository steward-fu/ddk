/****************************************************************************
 *                                                                          *
 *      ntverp.H        -- Version information for internal builds          *
 *                                      *
 *  This file is only modified by the official builder to update the    *
 *      VERSION, VER_PRODUCTVERSION, VER_PRODUCTVERSION_STR and             *
 *      VER_PRODUCTBETA_STR values.                                         *
 *                                                                          *
 ****************************************************************************/

 //
 // On win9x there is version file modified by official builders in another
 // location, so redirect to it VladS 04/18.
 //
//#include "..\..\sdk\inc16\version.h"
#include ".\inc16\version.h"

#ifndef VER_LEGALCOPYRIGHT_YEARS
#define VER_LEGALCOPYRIGHT_YEARS    "1981-" VER_COPYRIGHT_CURRENT_YEAR
#endif



