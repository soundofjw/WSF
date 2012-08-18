/*
	Whelchel Sound Format
		Based on MODs

  Josh Whelchel 2003
  www.syntesis.ath.cx

*/

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
/// \file WSFGlob.h Global WSF stuff

#ifndef __WSFGLOBH__
#define __WSFGLOBH__

#include <stdio.h>

/// 'SNES' Echo value.
/// WSFTool and WinWSFTool both feature a option to tag WSF files as
/// 'SNES'-echo'd. This naming is simply a reference to how the results,
/// if the track suits it, will sound like it is being played on an SNES.
/// This macro simply defines how much echo we want to tag for.
/// This is the equiv. of saying "SEcho=DEFNESECHO"
#define DEFSNESECHO		"64"

/// Used to test version through bHi and bLo
#define TESTVER(x,y) (bHi > x || (bHi == x && bLo >= y))

/// WSF Byte
typedef unsigned char wsfb;

/// Unsigned Short Typedef
typedef unsigned short wsus;

/// Unsigned Long Typedef
typedef unsigned long wsul;

/// Sample Output for use with CWSFPack::GetSamp
typedef struct wsf_sampout_s
{
	unsigned long nSize;	///< Size of sample
	wsfb *bSampData;		///< Sample Data

	wsus nCh;			///< Number of Channels
	wsus nBit;			///< Bit Rate (8/16)

	wsfb nOgg;			///< Ogg Compression?
	wsul nStoredSize;		///< Stored Size

	wsul nFreq;			///< An Okay Freq

} wsf_sampout;

/// Sample Storage
typedef struct wsf_sample_s
{
	wsul nSize;				///< Sample Size (in Bytes)
	wsfb *bSampData;		///< Sample Data

	char cSignature[31];	///< Sample Signature (size = 30)

	wsus nCh;			///< Number of Channels
	wsul nSamps;		///< Number of Samples
	wsus nBit;			///< Bit Rate (8/16)

	wsfb nOgg;			///< Ogg Compression?

	wsul nRawSize;		///< Internal Raw Size to be used in CWSFPack
	wsul nStoredSize;	///< Stored Sample Size

	char cName[27];		///< Sample Name

	wsul nFreq;			///< Sample Frequency (An okay one)

} wsf_sample;

#define WSF_ID(a,b,c,d) (((unsigned int)(a) << 24) | \
                          ((unsigned int)(b) << 16) | \
                          ((unsigned int)(c) <<  8) | \
                          ((unsigned int)(d)      ))

#define WSFERR	16661  ///< Value used for returning errors with wsul return values

/// For use with loading mods in memory easily :D
typedef struct wsf_file_s
{
	wsfb *bData;
	wsul nSize;

	wsfb *bPos;
	wsul nPos;

	char *cF;
	wsfb bW;
	wsfb nFree;

	FILE *hFile;
} wsf_file;

wsf_file *wsfopenfile( char *cFile );
wsf_file *wsfopenmem( wsfb *bData, wsul nSize, wsfb nFree = 0 ); // nFree states that wsfclose should free the memory.
wsf_file *wsfcreatefile( char *cFile );
wsf_file *wsfcreatemem( void );

wsul wsfwrite( void *bData, wsul nSize, wsf_file *wf );
wsul wsfread( void *bData, wsul nSize, wsf_file *wf );
void wsfseek( wsul nPos, wsf_file *wf );
void wsfbegin( wsul nOff, wsf_file *wf );
void wsfend( long nOff, wsf_file *wf );

void wsfclose( wsf_file *wf );

/// Structure for MiniWSF
typedef struct miniwsfdata_s
{
	char cTitle[31];
	char cMessage[256];

	wsul nMods;
	wsul *nModSizes;
	wsfb **bMods;

	char **cFiles;

	wsul nPackSize;
	wsfb *bPack;

} MiniWSFData;

#endif

