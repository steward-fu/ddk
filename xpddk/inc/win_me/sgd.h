/*static char *SCCSID = "@(#)sgd.h   13.27 90/07/18";*/
/****************************************************************************\
*
*       Copyright (c) Microsoft Corporation 1990
*
\****************************************************************************/
/****************************************************************************\
*
* SGD - scatter/gather descriptor
*
\****************************************************************************/

/* equivalent names:                                                        */
/*                                                                          */
/* old rlh.h/strat2.h   old rlx.h               new sgd.h                   */
/* ==================   ==================      ==================          */
/* sg_bufferptr                                 sg_buffer_ptr               */
/* sg_buffersize                                sg_buffer_size              */

/* H2INCSWITCHES -t */

typedef struct _SGD { /* */

        ULONG  SG_buff_ptr;     /* 32 bit physcial pointer to the buffer    */
        ULONG  SG_buff_size;    /* size of the buffer in bytes              */

} SGD, *PSGD;

