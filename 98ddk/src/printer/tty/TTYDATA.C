// TTYDATA.C
// Arrays for printable TTY characters, TTY help file ID's.

#pragma code_seg("_DATASEG")

#include "tty.h"

BYTE DefaultTTYChars[] = {

    (BYTE)'_', /* undefined      128  0x80 */
    (BYTE)'_', /* undefined      129  0x81 */
    (BYTE)'_', /* undefined      130  0x82 */
    (BYTE)'_', /* undefined      131  0x83 */
    (BYTE)'_', /* undefined      132  0x84 */
    (BYTE)'_', /* undefined      133  0x85 */
    (BYTE)'_', /* undefined      134  0x86 */
    (BYTE)'_', /* undefined      135  0x87 */
    (BYTE)'_', /* undefined      136  0x88 */
    (BYTE)'_', /* undefined      138  0x89 */
    (BYTE)'_', /* undefined      138  0x8A */
    (BYTE)'_', /* undefined      139  0x8B */
    (BYTE)'_', /* undefined      140  0x8C */
    (BYTE)'_', /* undefined      141  0x8D */
    (BYTE)'_', /* undefined      142  0x8E */
    (BYTE)'_', /* undefined      143  0x8F */

    (BYTE)'_', /* undefined      144  0x90 */
    (BYTE) 96, /* open sing. qt. 145  0x91 use grave accent*/
    (BYTE) 39, /* close sing qt. 146  0x92 use apostrophe */
    (BYTE)'_', /* undefined      147  0x93 */
    (BYTE)'_', /* undefined      148  0x94 */
    (BYTE)'_', /* undefined      149  0x95 */
    (BYTE)'_', /* undefined      150  0x96 */
    (BYTE)'_', /* undefined      151  0x97 */
    (BYTE)'_', /* undefined      152  0x98 */
    (BYTE)'_', /* undefined      153  0x99 */
    (BYTE)'_', /* undefined      154  0x9A */
    (BYTE)'_', /* undefined      155  0x9B */
    (BYTE)'_', /* undefined      156  0x9C */
    (BYTE)'_', /* undefined      157  0x9D */
    (BYTE)'_', /* undefined      158  0x9E */
    (BYTE)'_', /* undefined      159  0x9F */

    (BYTE)' ', /* ' '            160  0xA0 */
    (BYTE)'!', /* !              161  0xA1 */
    (BYTE)'c', /* c|             162  0xA2 */
    (BYTE)'#', /* #              163  0xA3 */
    (BYTE)'*', /*                164  0xA4 */
    (BYTE)'Y', /* Y=             165  0xA5 */
    (BYTE)'|', /* |              166  0xA6 */
    (BYTE)'S', /*                167  0xA7 */
    (BYTE)'"', /* dieresis       168  0xA8 */
    (BYTE)'c', /* (c)            169  0xA9 */
    (BYTE)'a', /* a_             170  0xAA */
    (BYTE)'<', /* <<             171  0xAB */
    (BYTE)'-', /* |-             172  0xAC */
    (BYTE)'-', /* -              173  0xAD */
    (BYTE)'R', /* (R)            174  0xAE */
    (BYTE)'-', /* upperline      175  0xAF */
    (BYTE)'.', /* bullet         176  0xB0 */
    (BYTE)'+', /* +-             177  0xB1 */
    (BYTE)'2', /* 2              178  0xB2 */
    (BYTE)'3', /* ^3             179  0xB3 */
    (BYTE)'\'',/* '              180  0xB4 */
    (BYTE)'u', /* m              181  0xB5 */
    (BYTE)'q', /* q esp.         182  0xB6 */
    (BYTE)'.', /* .              183  0xB7 */
    (BYTE)',', /* ,              184  0xB8 */
    (BYTE)'1', /* ^1             185  0xB9 */
    (BYTE)'o', /* x_             186  0xBA */
    (BYTE)'>', /* >>             187  0xBB */
    (BYTE)'/', /* 1/4            188  0xBC */
    (BYTE)'/', /* 1/2            189  0xBD */
    (BYTE)'/', /* 3/4            190  0xBE */
    (BYTE)'?', /* ?              191  0xBF */
    (BYTE)'A', /* a              192  0xC0 */
    (BYTE)'A', /* a'             193  0xC1 */
    (BYTE)'A', /* a^             194  0xC2 */
    (BYTE)'A', /* A~             195  0xC3 */
    (BYTE)'A', /* A              196  0xC4 */
    (BYTE)'A', /* Ao             197  0xC5 */
    (BYTE)'A', /* AE             198  0xC6 */
    (BYTE)'C', /* C cedilla     199  0xC7 */
    (BYTE)'E', /* e`             200  0xC8 */
    (BYTE)'E', /* e'             201  0xC9 */
    (BYTE)'E', /* e^             202  0xCA */
    (BYTE)'E', /* e              203  0xCB */
    (BYTE)'I', /* i`             204  0xCC */
    (BYTE)'I', /* i'             205  0xCD */
    (BYTE)'I', /* i^             206  0xCE */
    (BYTE)'I', /* i              207  0xCF */
    (BYTE)'D', /* UC Eth         208  0xD0 */
    (BYTE)'N', /* N~             209  0xD1 */
    (BYTE)'O', /* o`             210  0xD2 */
    (BYTE)'O', /* o'             211  0xD3 */
    (BYTE)'O', /* o^             212  0xD4 */
    (BYTE)'O', /* O~             213  0xD5 */
    (BYTE)'O', /* O              214  0xD6 */
    (BYTE)'x', /* multiply       215  0xD7 */
    (BYTE)'0', /* O/             216  0xD8 */
    (BYTE)'U', /* u`             217  0xD9 */
    (BYTE)'U', /* u'             218  0xDA */
    (BYTE)'U', /* u^             219  0xDB */
    (BYTE)'U', /* u              220  0xDC */
    (BYTE)'Y', /* Y'             221  0xDD */
    (BYTE)'T', /* u.c. thorn     222  0xDE */
    (BYTE)'B', /* ss             223  0xDF */
    (BYTE)'a', /* a              224  0xE0 */
    (BYTE)'a', /* a'             225  0xE1 */
    (BYTE)'a', /* a^             226  0xE2 */
    (BYTE)'a', /* a~             227  0xE3 */
    (BYTE)'a', /* a              228  0xE4 */
    (BYTE)'a', /* ao             229  0xE5 */
    (BYTE)'a', /* ae             230  0xE6 */
    (BYTE)'c', /* c              231  0xE7 */
    (BYTE)'e', /* e`             232  0xE8 */
    (BYTE)'e', /* e'             233  0xE9 */
    (BYTE)'e', /* e^             234  0xEA */
    (BYTE)'e', /* e              235  0xEB */
    (BYTE)'i', /* i`             236  0xEC */
    (BYTE)'i', /* i'             237  0xED */
    (BYTE)'i', /* i^             238  0xEE */
    (BYTE)'i', /* i              239  0xEF */
    (BYTE)'d', /* lc Eth         240  0xF0 */
    (BYTE)'n', /* n~             241  0xF1 */
    (BYTE)'o', /* o`             242  0xF2 */
    (BYTE)'o', /* o'             243  0xF3 */
    (BYTE)'o', /* o^             244  0xF4 */
    (BYTE)'o', /* o~             245  0xF5 */
    (BYTE)'o', /* o              246  0xF6 */
    (BYTE)'/', /* divide         247  0xF7 */
    (BYTE)'0', /* ph             248  0xF8 */
    (BYTE)'u', /* u`             249  0xF9 */
    (BYTE)'u', /* u'             250  0xFA */
    (BYTE)'u', /* u^             251  0xFB */
    (BYTE)'u', /* u              252  0xFC */
    (BYTE)'y', /* y'             253  0xFD */
    (BYTE)'t', /* l.c. thorn     253  0xFE */
    (BYTE)'y', /* y              255  0xFF */
};

BYTE CodePage437CharSet[] = {

    (BYTE)'_', /* undefined      128  0x80 */
    (BYTE)'_', /* undefined      129  0x81 */
    (BYTE)',', /* comma          130  0x82 */
    (BYTE)'ü', /* ASCII 159      131  0x83 */
    (BYTE)'_', /* undefined      132  0x84 */
    (BYTE)'_', /* undefined      133  0x85 */
    (BYTE)'_', /* undefined      134  0x86 */
    (BYTE)'_', /* undefined      135  0x87 */
    (BYTE)'^', /* ASCII 94       136  0x88 */
    (BYTE)'_', /* undefined      138  0x89 */
    (BYTE)'_', /* undefined      138  0x8A */
    (BYTE)'<', /* ASCII 60       139  0x8B */
    (BYTE)'_', /* andefined      140  0x8C */
    (BYTE)'_', /* undefined      141  0x8D */
    (BYTE)'_', /* undefined      142  0x8E */
    (BYTE)'_', /* undefined      143  0x8F */

    (BYTE)'_', /* undefined      144  0x90 */
    (BYTE) 96, /* open sing. qt. 145  0x91 use grave accent*/
    (BYTE) 39, /* close sing qt. 146  0x92 use apostrophe */
    (BYTE)'"', /* quotion mark   147  0x93 */
    (BYTE)'"', /* quotion mark   148  0x94 */
    (BYTE)'˙', /* ASCII 250      149  0x95 */
    (BYTE)'-', /* dash           150  0x96 */
    (BYTE)'ƒ', /* ASCII 196      151  0x97 */
    (BYTE)'~', /* ASCII 126      152  0x98 */
    (BYTE)'_', /* undefined      153  0x99 */
    (BYTE)'s', /* s              154  0x9A */
    (BYTE)'>', /* ASCII 62       155  0x9B */
    (BYTE)'_', /* undefined      156  0x9C */
    (BYTE)'_', /* undefined      157  0x9D */
    (BYTE)'_', /* undefined      158  0x9E */
    (BYTE)'Y', /* Y              159  0x9F */

    (BYTE)'ˇ', /* ASCII 255 NBS  160  0xA0 */
    (BYTE)'≠', /* ASCII 173      161  0xA1 */
    (BYTE)'õ', /* ASCII 155      162  0xA2 */
    (BYTE)'ú', /* ASCII 156      163  0xA3 */
    (BYTE)'*', /* *              164  0xA4 */
    (BYTE)'ù', /* ASCII 157      165  0xA5 */
    (BYTE)'|', /* |              166  0xA6 */
    (BYTE)'', /* ASCII 21       167  0xA7 */
    (BYTE)'"', /* dieresis       168  0xA8 */
    (BYTE)'c', /* (c)            169  0xA9 */
    (BYTE)'¶', /* ASCII 166      170  0xAA */
    (BYTE)'Æ', /* ASCII 174      171  0xAB */
    (BYTE)'™', /* ASCII 170      172  0xAC */
    (BYTE)'-', /* -              173  0xAD */
    (BYTE)'R', /* (R)            174  0xAE */
    (BYTE)'-', /* upperline      175  0xAF */
    (BYTE)'o', /* ASCII 248      176  0xB0 */
    (BYTE)'+', /* ASCII 241      177  0xB1 */
    (BYTE)'2', /* ASCII 253      178  0xB2 */
    (BYTE)'3', /* 3              179  0xB3 */
    (BYTE)'\'',/* '              180  0xB4 */
    (BYTE)'Ê', /* ASCII 230      181  0xB5 */
    (BYTE)'', /* ASCII 20       182  0xB6 */
    (BYTE)' ', /* ASCII 249      183  0xB7 */
    (BYTE)',', /* ,              184  0xB8 */
    (BYTE)'1', /* ^1             185  0xB9 */
    (BYTE)'o', /* ASCII 248      186  0xBA */
    (BYTE)'Ø', /* ASCII 175      187  0xBB */
    (BYTE)'¨', /* ASCII 172      188  0xBC */
    (BYTE)'´', /* ASCII 171      189  0xBD */
    (BYTE)'_', /* undefined      190  0xBE */
    (BYTE)'®', /* ASCII 168      191  0xBF */
    (BYTE)'A', /* a              192  0xC0 */
    (BYTE)'A', /* a'             193  0xC1 */
    (BYTE)'A', /* a^             194  0xC2 */
    (BYTE)'A', /* A~             195  0xC3 */
    (BYTE)'é', /* ASCII 142      196  0xC4 */
    (BYTE)'è', /* ASCII 143      197  0xC5 */
    (BYTE)'í', /* ASCII 146      198  0xC6 */
    (BYTE)'Ä', /* ASCII 128      199  0xC7 */
    (BYTE)'E', /* e`             200  0xC8 */
    (BYTE)'ê', /* ASCII 144      201  0xC9 */
    (BYTE)'E', /* e^             202  0xCA */
    (BYTE)'E', /* e              203  0xCB */
    (BYTE)'I', /* i`             204  0xCC */
    (BYTE)'I', /* i'             205  0xCD */
    (BYTE)'I', /* i^             206  0xCE */
    (BYTE)'I', /* i              207  0xCF */
    (BYTE)'D', /* UC Eth         208  0xD0 */
    (BYTE)'•', /* N~             209  0xD1 */
    (BYTE)'O', /* o`             210  0xD2 */
    (BYTE)'O', /* o'             211  0xD3 */
    (BYTE)'O', /* o^             212  0xD4 */
    (BYTE)'O', /* O~             213  0xD5 */
    (BYTE)'ô', /* ASCII 153      214  0xD6 */
    (BYTE)'x', /* multiply       215  0xD7 */
    (BYTE)'0', /* O/             216  0xD8 */
    (BYTE)'U', /* u`             217  0xD9 */
    (BYTE)'U', /* u'             218  0xDA */
    (BYTE)'U', /* u^             219  0xDB */
    (BYTE)'ö', /* ASCII 154      220  0xDC */
    (BYTE)'Y', /* Y'             221  0xDD */
    (BYTE)'T', /* u.c. thorn     222  0xDE */
    (BYTE)'·', /* ASCII 225      223  0xDF */
    (BYTE)'Ö', /* ASCII 133      224  0xE0 */
    (BYTE)'†', /* ASCII 160      225  0xE1 */
    (BYTE)'É', /* ASCII 131      226  0xE2 */
    (BYTE)'a', /* a~             227  0xE3 */
    (BYTE)'Ñ', /* ASCII 132      228  0xE4 */
    (BYTE)'Ü', /* ASCII 134      229  0xE5 */
    (BYTE)'ë', /* ASCII 145      230  0xE6 */
    (BYTE)'á', /* ASCII 135      231  0xE7 */
    (BYTE)'ä', /* ASCII 138      232  0xE8 */
    (BYTE)'Ç', /* ASCII 130      233  0xE9 */
    (BYTE)'à', /* ASCII 136      234  0xEA */
    (BYTE)'â', /* ASCII 137      235  0xEB */
    (BYTE)'ç', /* ASCII 141      236  0xEC */
    (BYTE)'°', /* ASCII 161      237  0xED */
    (BYTE)'å', /* ASCII 140      238  0xEE */
    (BYTE)'ã', /* ASCII 139      239  0xEF */
    (BYTE)'d', /* lc Eth         240  0xF0 */
    (BYTE)'§', /* ASCII 164      241  0xF1 */
    (BYTE)'ï', /* ASCII 149      242  0xF2 */
    (BYTE)'¢', /* ASCII 162      243  0xF3 */
    (BYTE)'ì', /* ASCII 147      244  0xF4 */
    (BYTE)'o', /* o~             245  0xF5 */
    (BYTE)'î', /* ASCII 148      246  0xF6 */
    (BYTE)'-', /* ASCII 246      247  0xF7 */
    (BYTE)'o', /* ASCII 237      248  0xF8 */
    (BYTE)'ó', /* ASCII 151      249  0xF9 */
    (BYTE)'£', /* ASCII 163      250  0xFA */
    (BYTE)'ñ', /* ASCII 150      251  0xFB */
    (BYTE)'Å', /* ASCII 129      252  0xFC */
    (BYTE)'y', /* y'             253  0xFD */
    (BYTE)'t', /* l.c. thorn     253  0xFE */
    (BYTE)'ò', /* ASCII 152      255  0xFF */
};

BYTE CodePage850CharSet[] = {

    (BYTE)'_', /* undefined      128  0x80 */
    (BYTE)'_', /* undefined      129  0x81 */
    (BYTE)',', /* comma          130  0x82 */
    (BYTE)'ü', /* ASCII 159      131  0x83 */
    (BYTE)'_', /* undefined      132  0x84 */
    (BYTE)'_', /* undefined      133  0x85 */
    (BYTE)'_', /* undefined      134  0x86 */
    (BYTE)'_', /* undefined      135  0x87 */
    (BYTE)'^', /* ASCII 94       136  0x88 */
    (BYTE)'_', /* undefined      137  0x89 */
    (BYTE)'_', /* undefined      138  0x8A */
    (BYTE)'<', /* ASCII 60       139  0x8B */
    (BYTE)'_', /* andefined      140  0x8C */
    (BYTE)'_', /* undefined      141  0x8D */
    (BYTE)'_', /* undefined      142  0x8E */
    (BYTE)'_', /* undefined      143  0x8F */

    (BYTE)'_', /* undefined      144  0x90 */
    (BYTE) 96, /* open sing. qt. 145  0x91 use grave accent*/
    (BYTE) 39, /* close sing qt. 146  0x92 use apostrophe */
    (BYTE)'"', /* quotion mark   147  0x93 */
    (BYTE)'"', /* quotion mark   148  0x94 */
    (BYTE)'˙', /* ASCII 250      149  0x95 */
    (BYTE)'-', /* dash           150  0x96 */
    (BYTE)'ƒ', /* ASCII 196      151  0x97 */
    (BYTE)'~', /* ASCII 126      152  0x98 */
    (BYTE)'_', /* undefined      153  0x99 */
    (BYTE)'s', /* s              154  0x9A */
    (BYTE)'>', /* ASCII 62       155  0x9B */
    (BYTE)'_', /* undefined      156  0x9C */
    (BYTE)'_', /* undefined      157  0x9D */
    (BYTE)'_', /* undefined      158  0x9E */
    (BYTE)'Y', /* Y              159  0x9F */

    (BYTE)'ˇ', /* ASCII 255 NBS  160  0xA0 */
    (BYTE)'≠', /* ASCII 173      161  0xA1 */
    (BYTE)'Ω', /* ASCII 189      162  0xA2 */
    (BYTE)'ú', /* ASCII 156      163  0xA3 */
    (BYTE)'œ', /* ASCII 207      164  0xA4 */
    (BYTE)'æ', /* ASCII 190      165  0xA5 */
    (BYTE)'|', /* |              166  0xA6 */
    (BYTE)'j', /* ASCII 245      167  0xA7 */
    (BYTE)'"', /* dieresis       168  0xA8 */
    (BYTE)'∏', /* ASCII 184      169  0xA9 */
    (BYTE)'¶', /* ASCII 166      170  0xAA */
    (BYTE)'Æ', /* ASCII 174      171  0xAB */
    (BYTE)'™', /* ASCII 170      172  0xAC */
    (BYTE)'-', /* -              173  0xAD */
    (BYTE)'©', /* ASCII 169      174  0xAE */
    (BYTE)'-', /* upperline      175  0xAF */
    (BYTE)'o', /* ASCII 248      176  0xB0 */
    (BYTE)'+', /* ASCII 241      177  0xB1 */
    (BYTE)'2', /* ASCII 253      178  0xB2 */
    (BYTE)'¸', /* ASCII 252      179  0xB3 */
    (BYTE)'\'',/* '              180  0xB4 */
    (BYTE)'Ê', /* ASCII 230      181  0xB5 */
    (BYTE)'˛', /* ASCII 244      182  0xB6 */
    (BYTE)'˛', /* ASCII 254      183  0xB7 */
    (BYTE)',', /* ,              184  0xB8 */
    (BYTE)'˚', /* ASCII 251      185  0xB9 */
    (BYTE)'o', /* ASCII 248      186  0xBA */
    (BYTE)'Ø', /* ASCII 175      187  0xBB */
    (BYTE)'¨', /* ASCII 172      188  0xBC */
    (BYTE)'´', /* ASCII 171      189  0xBD */
    (BYTE)'_', /* undefined      190  0xBE */
    (BYTE)'®', /* ASCII 168      191  0xBF */
    (BYTE)'∑', /* ASCII 183      192  0xC0 */
    (BYTE)'µ', /* ASCII 181      193  0xC1 */
    (BYTE)'∂', /* ASCII 182      194  0xC2 */
    (BYTE)'«', /* ASCII 199      195  0xC3 */
    (BYTE)'é', /* ASCII 142      196  0xC4 */
    (BYTE)'è', /* ASCII 143      197  0xC5 */
    (BYTE)'í', /* ASCII 146      198  0xC6 */
    (BYTE)'Ä', /* ASCII 128      199  0xC7 */
    (BYTE)'‘', /* ASCII 212      200  0xC8 */
    (BYTE)'ê', /* ASCII 144      201  0xC9 */
    (BYTE)'“', /* ASCII 210      202  0xCA */
    (BYTE)'”', /* ASCII 211      203  0xCB */
    (BYTE)'ﬁ', /* ASCII 222      204  0xCC */
    (BYTE)'÷', /* ASCII 214      205  0xCD */
    (BYTE)'◊', /* ASCII 215      206  0xCE */
    (BYTE)'ÿ', /* ASCII 216      207  0xCF */
    (BYTE)'—', /* ASCII 209      208  0xD0 */
    (BYTE)'•', /* ASCII 165      209  0xD1 */
    (BYTE)'„', /* ASCII 227      210  0xD2 */
    (BYTE)'‡', /* ASCII 224      211  0xD3 */
    (BYTE)'‚', /* ASCII 226      212  0xD4 */
    (BYTE)'‰', /* ASCII 228      213  0xD5 */
    (BYTE)'ô', /* ASCII 153      214  0xD6 */
    (BYTE)'û', /* ASCII 158      215  0xD7 */
    (BYTE)'ù', /* ASCII 157      216  0xD8 */
    (BYTE)'Î', /* ASCII 235      217  0xD9 */
    (BYTE)'È', /* ASCII 233      218  0xDA */
    (BYTE)'Í', /* ASCII 234      219  0xDB */
    (BYTE)'ö', /* ASCII 154      220  0xDC */
    (BYTE)'Ì', /* ASCII 237      221  0xDD */
    (BYTE)'Á', /* ASCII 231      222  0xDE */
    (BYTE)'·', /* ASCII 225      223  0xDF */
    (BYTE)'Ö', /* ASCII 133      224  0xE0 */
    (BYTE)'†', /* ASCII 160      225  0xE1 */
    (BYTE)'É', /* ASCII 131      226  0xE2 */
    (BYTE)'∆', /* ASCII 198      227  0xE3 */
    (BYTE)'Ñ', /* ASCII 132      228  0xE4 */
    (BYTE)'Ü', /* ASCII 134      229  0xE5 */
    (BYTE)'ë', /* ASCII 145      230  0xE6 */
    (BYTE)'á', /* ASCII 135      231  0xE7 */
    (BYTE)'ä', /* ASCII 138      232  0xE8 */
    (BYTE)'Ç', /* ASCII 130      233  0xE9 */
    (BYTE)'à', /* ASCII 136      234  0xEA */
    (BYTE)'â', /* ASCII 137      235  0xEB */
    (BYTE)'ç', /* ASCII 141      236  0xEC */
    (BYTE)'°', /* ASCII 161      237  0xED */
    (BYTE)'å', /* ASCII 140      238  0xEE */
    (BYTE)'ã', /* ASCII 139      239  0xEF */
    (BYTE)'–', /* ASCII 208      240  0xF0 */
    (BYTE)'§', /* ASCII 164      241  0xF1 */
    (BYTE)'ï', /* ASCII 149      242  0xF2 */
    (BYTE)'¢', /* ASCII 162      243  0xF3 */
    (BYTE)'ì', /* ASCII 147      244  0xF4 */
    (BYTE)'‰', /* ASCII 228      245  0xF5 */
    (BYTE)'î', /* ASCII 148      246  0xF6 */
    (BYTE)'-', /* ASCII 246      247  0xF7 */
    (BYTE)'c', /* ASCII 155      248  0xF8 */
    (BYTE)'ó', /* ASCII 151      249  0xF9 */
    (BYTE)'£', /* ASCII 163      250  0xFA */
    (BYTE)'ñ', /* ASCII 150      251  0xFB */
    (BYTE)'Å', /* ASCII 129      252  0xFC */
    (BYTE)'Ï', /* ASCII 236      253  0xFD */
    (BYTE)'Ë', /* ASCII 232      253  0xFE */
    (BYTE)'ò', /* ASCII 152      255  0xFF */
};


