/***************************************************************************
 *   Copyright (C) 2005 by Josh Whelchel   *
 *   joshwhel@insightbb.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///
/// \file WSFComp.h Compression Header

#ifdef __cplusplus
extern "C" {
#endif

/// Main Compression Routine
/// @param bInput Input of data
/// @param nFileSize Size of bInput
/// @param nOldSize can be ignored for Compressing
/// @param bOutput Replaced with pointer to data (must be freed)
/// @param nOutLen Replaced with output size
/// @param bCompress TRUE = Compress, FALSE = Decompress
int DoCompression( unsigned char *bInput, int nFileSize, int nOldSize, unsigned char **bOutput, unsigned int *nOutLen, char bCompress );

// Uses dumb here :/
unsigned int wsf_decompress16(unsigned char *f, signed char *left, signed char *right, int len, int cmwt);
unsigned int wsf_decompress8(unsigned char *f, signed char *left, signed char *right, int len, int cmwt);
// MODPLUG STYLE
unsigned int wsfITUnpack16Bit(char *pSample, unsigned int dwLen, unsigned char *lpMemFile, unsigned int dwMemLength, int b215);
unsigned int wsfITUnpack8Bit(char *pSample, unsigned int dwLen, unsigned char *lpMemFile, unsigned int dwMemLength, int b215);


#ifdef __cplusplus
}
#endif

/* minilzo.h -- mini version of the LZO real-time data compression library

   This file is part of the LZO real-time data compression library.

   Copyright (C) 1997 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996 Markus Franz Xaver Johannes Oberhumer

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   markus.oberhumer@jk.uni-linz.ac.at
 */

/*
 * NOTE:
 *   the full LZO package can be found at
 *   http://www.infosys.tuwien.ac.at/Staff/lux/marco/lzo.html
 */


#ifndef __MINILZO_H

#ifdef __LZOCONF_H
#  error you cannot use both LZO and miniLZO
#endif
#undef LZO_HAVE_CONFIG_H
#include "lzoconf.h"
#define __MINILZO_H

#define MINILZO_VERSION         0x1000


#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
//
************************************************************************/

#define LZO1X_MEM_COMPRESS      ((lzo_uint) (16384L * sizeof(lzo_byte *)))
#define LZO1X_MEM_DECOMPRESS    (0)


/* compression */
LZO_EXTERN(int)
lzo1x_1_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem );

/* decompression */
LZO_EXTERN(int)
lzo1x_decompress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem /* NOT USED */ );

/* safe decompression with overrun testing */
LZO_EXTERN(int)
lzo1x_decompress_safe   ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uint *dst_len,
                                lzo_voidp wrkmem /* NOT USED */ );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* already included */

