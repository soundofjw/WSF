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
/// \file WSFLoaders.cpp Main Loader Functions

// FIND A LOADER FAST by searching for
// WSFL_xxx
// (replace xxx with normal extension)
// (eg WSFL_IT or WSFL_MOD)

char g_cWSFString[] = "WSF!";

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#include "WSFGlob.h"
#include "WSFMod.h"
#include "WSFComp.h"

#ifdef WINWSF

/* 

  WINWSF is a decleration used when compiling WinWSFTool.
  This allows us to give detailed status reports through
  the CStatusDlg class.

*/

#include "WinWSFTool\resource.h"		// main symbols
#include "WinWSFTool\StdAfx.h"
#include "WinWSFTool\StatusDlg.h"
extern CStatusDlg *g_mStatus;

#endif

// WSFL_IT

//***************************************************************************** */
// *FindSampleOffsets()
// 
// Purpose: Finds Sample Offsets
// 
// Author : [Josh Whelchel], Created : [2/12/2003 12:56:49 PM]
// Parameters:	
// 
// [*bData] :
// [nSize] :
// 
// Returns:	unsigned long
// 
//***************************************************************************** */

wsul *CITLoader::FindSampleOffsets( wsfb *bData, wsul nSize, int *nSOut )
{
#ifndef WSFNOSAVE
	short nOrds;
	short nInsts;
	short nSamps;
	unsigned long nOff;
	unsigned long *nSampOffs;
	int i;
	char cHead[4];

	// Read Header
	MOPEN(bData,nSize);
	MREAD(cHead,1,4);
	if (strncmp(cHead,"IMPM",4)!=0)
		return NULL;
	
	// Seek to important infos
	MSEEK(28);

	// Read number of orders
	MREAD(&nOrds,1,2);
	// Read number of insts
	MREAD(&nInsts,1,2);
	// Read number of samples
	MREAD(&nSamps,1,2);

	// Skip 4 to the cwmt thing
	MSEEK(4);
	MREAD(&m_nComp,1,2);

	// Head to samples ((2) Offset = 00C0h+OrdNum+InsNum*4)
	nOff = 192 + nOrds + (nInsts*4);
	MBEGIN(nOff);

	// Now we're at samples :O
	// Allocate room for sample offsets
	nSampOffs = (unsigned long*)malloc(4*nSamps);

	// Read offsets
	for (i=0;i<nSamps;i++)
		MREAD(&nSampOffs[i],1,4);

	nSOut[0] = nSamps;
	return nSampOffs;
#else
	return NULL;
#endif
}

//////////////////////////////////////////////////////
//  CITLoader::readblock()
//
/// \author Josh Whelchel
/// \date 2/23/2003 9:39:43 PM
/// \brief For Compression
/// 
// 
//////////////////////////////////////////////////////

int CITLoader::readblock(void)
{
	long size;

	MREAD(&size,1,4);
	if (size < 0)
		return size;

	sourcebuf = (wsfb*)malloc(size);
	if (!sourcebuf)
		return -1;

	MREAD(sourcebuf,size,1);

	sourcepos = sourcebuf;
	sourceend = sourcebuf + size;
	rembits = 8;
	return 0;
}



//////////////////////////////////////////////////////
//  CITLoader::freeblock()
// 
/// \author Josh Whelchel
/// \date 2/23/2003 9:39:50 PM 
/// \brief Frees Block
/// 
// 
//////////////////////////////////////////////////////

void CITLoader::freeblock(void)
{
	free(sourcebuf);
	sourcebuf = NULL;
}



//////////////////////////////////////////////////////
//  CITLoader::readbits()
// 
/// \author Josh Whelchel
/// \date 2/23/2003 9:39:57 PM
/// \brief Reads Bits
/// 
// 
//////////////////////////////////////////////////////

int CITLoader::readbits(int bitwidth)
{
	int val = 0;
	int b = 0;

	if (sourcepos >= sourceend) return val;

	while (bitwidth > rembits) {
		val |= *sourcepos++ << b;
		if (sourcepos >= sourceend) return val;
		b += rembits;
		bitwidth -= rembits;
		rembits = 8;
	}

	val |= (*sourcepos & ((1 << bitwidth) - 1)) << b;
	*sourcepos >>= bitwidth;
	rembits -= bitwidth;

	return val;
}



/** WARNING - do we even need to pass `right`? */
/** WARNING - why bother memsetting at all? The whole array is written... */
/** WARNING - check for endianness safety */
//////////////////////////////////////////////////////
//  decompress8()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 9:39:16 PM
/// \brief Decopmresses 8bit Sample
/// 
// 
//////////////////////////////////////////////////////

int CITLoader::decompress8(wsfb *left, wsfb *right, int len, int cmwt)
{
	int blocklen, blockpos;
	wsfb bitwidth;
	short val;
	char d1, d2;

	memset(left, 0, len * sizeof(*left));
	if (right) {
		memset(right, 0, len * sizeof(*right));
		len <<= 1;
	}

	while (len > 0) {
		//Read a block of compressed data:
		if (readblock())
			return -1;
		//Set up a few variables
		blocklen = (len < 0x8000) ? len : 0x8000; //Max block length is 0x8000 wsfbs
		blockpos = 0;
		bitwidth = 9;
		d1 = d2 = 0;
		//Start the decompression:
		while (blockpos < blocklen) {
			//Read a value:
			val = (short)readbits(bitwidth);
			//Check for bit width change:

			if (bitwidth < 7) { //Method 1:
				if (val == (1 << (bitwidth - 1))) {
					val = (short)readbits(3) + 1;
					bitwidth = (val < bitwidth) ? val : val + 1;
					continue;
				}
			}
			else if (bitwidth < 9) { //Method 2
				wsfb border = (0xFF >> (9 - bitwidth)) - 4;

				if (val > border && val <= (border + 8)) {
					val -= border;
					bitwidth = (val < bitwidth) ? val : val + 1;
					continue;
				}
			}
			else if (bitwidth == 9) { //Method 3
				if (val & 0x100) {
					bitwidth = (val + 1) & 0xFF;
					continue;
				}
			}
			else { //Illegal width, abort ?
				freeblock();
				return -1;
			}

			//Expand the value to signed wsfb:
			{
				char v; //The sample value:
				if (bitwidth < 8) {
					wsfb shift = 8 - bitwidth;
					v = (val << shift);
					v >>= shift;
				}
				else
					v = (char)val;

				//And integrate the sample value
				//(It always has to end with integration doesn't it ? ;-)
				d1 += v;
				d2 += d1;
			}

			//Store !
			/* Version 2.15 was an unofficial version with hacked compression
			 * code. Yay, better compression :D
			 */
			if (right && (len & 1))
				*right++ = (int)(signed char)(cmwt == 0x215 ? d2 : d1) << 8;
			else
				*left++ = (int)(signed char)(cmwt == 0x215 ? d2 : d1) << 8;
			len--;
			blockpos++;
		}
		freeblock();
	}
	return 0;
}

//////////////////////////////////////////////////////
//  CITLoader::decompress16()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 9:39:07 PM
/// \brief Decompresses Sample
/// 
// 
//////////////////////////////////////////////////////

int CITLoader::decompress16(wsfb *left, wsfb *right, int len, int cmwt)
{
	int blocklen, blockpos;
	wsfb bitwidth;
	long val;
	short d1, d2;

	memset(left, 0, len * sizeof(*left));
	if (right) {
		memset(right, 0, len * sizeof(*right));
		len <<= 1;
	}

	while (len > 0) {
		//Read a block of compressed data:
		if (readblock())
			return -1;
		//Set up a few variables
		blocklen = (len < 0x4000) ? len : 0x4000; // Max block length is 0x4000 wsfbs
		blockpos = 0;
		bitwidth = 17;
		d1 = d2 = 0;
		//Start the decompression:
		while (blockpos < blocklen) {
			val = readbits(bitwidth);
			//Check for bit width change:

			if (bitwidth < 7) { //Method 1:
				if (val == (1 << (bitwidth - 1))) {
					val = readbits(4) + 1;
					bitwidth = (val < bitwidth) ? val : val + 1;
					continue;
				}
			}
			else if (bitwidth < 17) { //Method 2
				short border = (0xFFFF >> (17 - bitwidth)) - 8;

				if (val > border && val <= (border + 16)) {
					val -= border;
					bitwidth = val < bitwidth ? val : val + 1;
					continue;
				}
			}
			else if (bitwidth == 17) { //Method 3
				if (val & 0x10000) {
					bitwidth = (val + 1) & 0xFF;
					continue;
				}
			}
			else { //Illegal width, abort ?
				freeblock();
				return -1;
			}

			//Expand the value to signed wsfb:
			{
				short v; //The sample value:
				if (bitwidth < 16) {
					wsfb shift = 16 - bitwidth;
					v = (short)(val << shift);
					v >>= shift;
				}
				else
					v = (short)val;

				//And integrate the sample value
				//(It always has to end with integration doesn't it ? ;-)
				d1 += v;
				d2 += d1;
			}

			//Store !
			/* Version 2.15 was an unofficial version with hacked compression
			 * code. Yay, better compression :D
			 */
			if (right && (len & 1))
				*right++ = (int)(signed short)(cmwt == 0x215 ? d2 : d1);
			else
				*left++ = (int)(signed short)(cmwt == 0x215 ? d2 : d1);
			len--;
			blockpos++;
		}
		freeblock();
	}
	return 0;
}

//////////////////////////////////////////////////////
//  *CITLoader::GetSamplePointers()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 5:23:22 PM
/// \param *bData MOD Data 
/// \param nSize Size of Data
/// \param gspData wsf_gspdata struct to fill
/// \return Array of samples (needs to be freed)
/// \brief Gets sample pointers from an IT module
/// 
// 
//////////////////////////////////////////////////////

int CITLoader::GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData )
{
#ifndef WSFNOSAVE
	int nSamps;
	int i;
	wsul *nOffs;
	
	nOffs = FindSampleOffsets(bData,nSize,&nSamps);
	MOPEN(bData,nSize);

	gspData->nSamples = 0;
	InitGSPData(gspData);

	sourcebuf = NULL;
	sourcepos = NULL;
	rembits = 0;

	for (i=0;i<nSamps;i++)
	{
		char cHead[4];
		unsigned long nOff;
		unsigned long nSOff;
		wsul nSampSize;
		wsfb flag;
		nOff = nOffs[i];
		char cName[27];
		wsul nInSamps;
		wsul nFreq;

		MBEGIN(nOff);
		// Read Header
		MREAD(cHead,1,4);

		// Check Header
		if (strncmp(cHead,"IMPS",4)!=0){
			FreeGSPData(gspData);
			free(nOffs);
			return 1;
		}

		// read name
		MREAD(cName,12,1);
		cName[12]=0;

		// go to Flag and read it
		MSEEK(2);
		MREAD(&flag,1,1);

		// go to number of samples
		MBEGIN(nOff+48);
		MREAD(&nSampSize,1,4);

		MSEEK(8);
		MREAD(&nFreq,1,4);

		nInSamps = nSampSize;

		// go to position
		MBEGIN(nOff+72);
		MREAD(&nSOff,1,4);

		if (flag&8){

			// We COULD do compression here, but we'd have to
			// change the way it saved. :P
			/// \todo Support Compression?
			continue;
#if 0
			int nBit,nCh;
			nSampSize = nInSamps;

//			int nBit,nCh;
			if (flag&2){
				nSampSize *= 2;	// 16 bit?
				nBit = 16;
			}
			else
				nBit = 8;

			if (flag&4){
				nSampSize *= 2;	// stereo?
				nCh = 2;
			}
			else
				nCh = 1;

			// We add 6 here because we want to mark it as a compressed sample.
			// We don't really need to repeat all of that code above.
			PushIntoGSP(gspData,nSampSize,nSOff,nInSamps,nCh+6,nBit,cName,nFreq);
#endif
		}
		else
		{
			int nBit,nCh;
			if (flag&2){
				nSampSize *= 2;	// 16 bit?
				nBit = 16;
			}
			else
				nBit = 8;

			if (flag&4){
				nSampSize *= 2;	// stereo?
				nCh = 2;
			}
			else
				nCh = 1;

			PushIntoGSP(gspData,nSampSize,nSOff,nInSamps,nCh,nBit,cName,nFreq);
		}

//		gspData->nSampOffs[i] = nSOff;
//		gspData->nSampSizes[i] = nSampSize;
	}

	// Free those offsets
	free(nOffs);

	return 0;
#else
	return 1;
#endif
}

//////////////////////////////////////////////////////
//  CITLoader::Save()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 5:44:14 PM
/// \param *wOut Mod Out Data
/// \param *wLD Data to Save
/// \brief Saves an IT
/// 
// 
//////////////////////////////////////////////////////

int CITLoader::Save( wsf_modout *wOut, wsf_loaddata *wLD )
{
	// Does Nothing

	return 0;
}


//////////////////////////////////////////////////////
//  CITLoader::Load()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 12:06:42 PM
/// \param *bData Data to Load
/// \param nSize Size of Data
/// \brief Loads IT modules.
/// Loads IT modules from bData.
///
// 
//////////////////////////////////////////////////////

int CITLoader::Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD )
{
#ifndef WSFNOSAVE
	int i;
	int nSamps;
	wsf_gspdata gD;

	if (GetSamplePointers(bData,nSize,&gD))
		return 1;

	MOPEN(bData,nSize);
	MBEGIN(4);
	MREAD(wLD->cTitle,1,26);

	nSamps = gD.nSamples;

	// Add and Pick latest sample
	wLD->bSamples = (wsf_sample*)malloc(sizeof(wsf_sample)*(nSamps));
	wLD->nSampleSizes = (unsigned long*)malloc(sizeof(unsigned long)*(nSamps));
	wLD->nSamples = nSamps;

	// Now we can read samples, just cuz we know where they are ^_^
	for (i=0;i<nSamps;i++)
	{
		wsul nSOff;
		unsigned long *nSize;
		wsf_sample *wS;

		wS = &wLD->bSamples[i];
		nSize = &wLD->nSampleSizes[i];

		memset(wS,0,sizeof(wsf_sample));
		nSize[0] = gD.nSampSizes[i];
				
		wS->nSize = nSize[0];
		wS->nBit = gD.nBit[i];
		wS->nCh = gD.nCh[i];
		wS->nRawSize = wS->nSize;
		wS->nSamps = gD.nInSamps[i];
		wS->nFreq = gD.nFreqs[i];
		nSOff = gD.nSampOffs[i];
		strncpy(wS->cName,(char*)gD.cNames+(i*27),26);
		wS->cName[26]=0;

		if (wS->nSize > 0)
		{
			MBEGIN(nSOff);
#if 0
			if (wS->nCh == 7) // 1+6
			{
				// We marked it earlier as compressed by adding 6. Now we uncompress.
				wS->nCh -= 6;
				gD.nCh[i] -= 6;

				char *bTemp;
				wsul nSize2;
				wsul nLx;
				bTemp = (char*)malloc(wS->nSamps*(wS->nBit/8));

				nLx = g_bS+g_nB-g_bP;

				if (wS->nBit == 8) 
					nSize2 = wsfITUnpack8Bit(bTemp,wS->nSamps,g_bP,nLx,(m_nComp == 0x215));
				else
					nSize2 = wsfITUnpack16Bit(bTemp,wS->nSamps,g_bP,nLx,(m_nComp == 0x215));

				if (nSize2==0)
				{
					// erk bad compression
					FreeGSPData(&gD);
					return 1;
				}

				wS->bSampData = (wsfb*)bTemp; 
				wS->nSize = nSize2;
				nSize[0] = nSize2;
			}
			else
#endif
			{
				wS->bSampData = (wsfb*)malloc(wS->nSize);
				MREAD(wS->bSampData,1,wS->nSize);
				nSize[0] = wS->nSize;
			}

			if (wS->nCh > 2){
				wS->nCh -= 6;	// mini-todo: A STEREO COMPRESSED SAMPLE?! :<
				fprintf(stderr,"\n\n -- This is a weird IT! Stereo Compressed Samples not supported. -- \n\n");
			}
		}
	}

	FreeGSPData(&gD);

	wLD->nModSize = nSize;
	wLD->bModData = (wsfb*)malloc(nSize);
	memcpy(wLD->bModData,bData,nSize);
	return 0;
#else
	return 1;
#endif
}


// WSFL_WSF

int CWSFLoader::Save( wsf_modout *wOut, wsf_loaddata *wLD )
{
	return Save(wOut,wLD,NULL);
}


//////////////////////////////////////////////////////
// CWSFLoader::PrepareSampArray()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 4:42:35 PM
/// \brief Prepares a WSF2SampArray and returns WSF2Samps
///
//
//////////////////////////////////////////////////////

WSF2Samps CWSFLoader::PrepareSampArray( void )
{
	WSF2Samps ws;
	ws = (WSF2Samps)malloc(sizeof(WSF2SampArray));

	ws->nSamps = 0;
	ws->bSamps = (WSF2Samp*)malloc(0);
	ws->nIDs = 0;
	ws->bIDs = (wsul*)malloc(0);

#ifdef WINWSF
	ws->nNoRender = 0;
	ws->nSamplesTotal = 0;
#endif
	ws->nSCounts = NULL;
	ws->nCurID = 0;			
	/// \todo Maybe we should have a different init for ws->nCurID. We know that if we run into 0 later, it could POSSIBLY be sample.

	return ws;
}


//////////////////////////////////////////////////////
// CWSFLoader::PushIntoSampArray()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 4:46:12 PM
/// \param *wS Incoming Sample
/// \param wSamps Sample Array
/// \brief Pushes sample into sample array
///
//
//////////////////////////////////////////////////////

wsul CWSFLoader::PushIntoSampArray( WSF2Samp *wS, WSF2Samps wSamps )
{
	wsul i;
	
	i = wSamps->nSamps;
	wSamps->nSamps++;

	wSamps->bSamps = (WSF2Samp*)realloc(wSamps->bSamps,sizeof(WSF2Samp)*wSamps->nSamps);
	
	memcpy(&wSamps->bSamps[i],wS,sizeof(WSF2Samp));

	return i;
}


//////////////////////////////////////////////////////
// CWSFLoader::PushIntoSampArray()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 4:59:24 PM
/// \param nID ID of sample in WSP (CWSFPack)
/// \param bFlag Sample reproduction flag
/// \param nPar1 Parameter 1
/// \param nPar2 Parameter 2
/// \param fAmp Amplification
/// \param wSamps Sample Array
/// \brief Pushes sample into sample array
///
//
//////////////////////////////////////////////////////

wsul CWSFLoader::PushIntoSampArray( wsul nID, 
		wsfb bFlag, wsul nPar1, wsul nPar2, float fAmp, WSF2Samps wSamps )
{
	WSF2Samp s;
	s.bFlag = bFlag;
	s.nPar1 = nPar1;
	s.nPar2 = nPar2;
	s.nID = nID;
	s.nSize = 0;
	s.fAmp = fAmp;
	return PushIntoSampArray(&s,wSamps);
}

//////////////////////////////////////////////////////
// CWSFLoader::PushRefIntoArray()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 4:57:35 PM
/// \param nID ID to push into
/// \param wSamps Sample Array Structure
/// \brief Pushes a Sample Reference into a WSF2Samps structure.
///
//
//////////////////////////////////////////////////////

wsul CWSFLoader::PushRefIntoArray( wsul nID, WSF2Samps wSamps )
{
	wsul i;
	
	i = wSamps->nIDs;
	wSamps->nIDs++;

	wSamps->bIDs = (wsul*)realloc(wSamps->bIDs,sizeof(wsul)*wSamps->nIDs);
	wSamps->bIDs[i] = nID;

	return i;
}

//////////////////////////////////////////////////////
// CWSFLoader::FreeSampArray()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 4:43:26 PM
/// \param wSamps Sample Array
/// \brief Frees a WSF2Samps array of WSF2SampArray
///
//
//////////////////////////////////////////////////////

void CWSFLoader::FreeSampArray( WSF2Samps wS )
{
	free(wS->bSamps);
	free(wS->bIDs);
	free(wS);
}

//////////////////////////////////////////////////////
// CWSFLoader::Save()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 3:47:02 PM
/// \param *wOut Data to write to (OUT)
/// \param *wLD Incoming Load Data (IN)
/// \param *nSCounts Sample Counts
/// \brief Saves v2.x WSFMOD
///
//
//////////////////////////////////////////////////////

int CWSFLoader::Save( wsf_modout *wOut, wsf_loaddata *wLD, long *nSCounts )
{
#ifndef WSFNOSAVE
	unsigned long i;
	wsfb bHi,bLo;
	wsfb *bDat;
	unsigned long nSize;
	unsigned short sH;
	unsigned long nCSize;
	CBaseLoader *xBL;
	wsfb *bCDat;
	long nSub;
	wsf_gspdata gD;
	char cOutExt[5];
	long nSCount[WSFS_COUNT];

	wsul nFillSize;
	nFillSize = strlen(g_cWSFString);

	WSF2Samps wSamps;
	wSamps = NULL;
	
	// Fill these to zero so we know we haven't used any yet.
	for (i=0;i<WSFS_COUNT;i++)
		nSCount[i] = 0;

	sH = 0;

	nSize = wLD->nModSize;
	bDat = (wsfb*)malloc(nSize);
	memcpy(bDat,wLD->bModData,nSize);

	// init nSub
	nSub = 0;

	// Begin to write :>
	MOPEN(wOut->bModData,0);

	bHi = WSFMVERHI;
	bLo = WSFMVERLO;
	
	// Write Header
	MAPP(WSFMHEAD,1,4);

	// Write Version
	MAPP(&bHi,1,1);
	MAPP(&bLo,1,1);

	// Write Pack Name
	MAPP(m_cCurPack,1,26);

	// Write Mod Type
	memset(cOutExt,0,5);
	strncpy(cOutExt,g_cModExt[wLD->nModType],5);
	MAPP(cOutExt,1,5);

	// Null Sample Places
	// Get Base Loader for that filetype
	xBL = GetBaseLoader(wLD->nModType);
	nSub = 0;

	if (xBL->GetSamplePointers(wLD->bModData,wLD->nModSize,&gD))
	{
		delete xBL;
		MRESET(&wOut->bModData,&wOut->nSize);
		free(wOut->bModData);
		wOut->bModData = NULL;
		free(bDat);
		return 1;
	}

	for (i=0;i<wLD->nSamples;i++)
	{
		wsfb *bNew;
		wsul nNew;
		wsul nSOff;

		nSOff = gD.nSampOffs[i]+nSub;

		if (wLD->nSampleSizes[i])
		{
			bNew = ReplaceData(bDat,nSize,&nNew,nSOff,wLD->nSampleSizes[i],(wsfb*)g_cWSFString,nFillSize);
			free(bDat);

			nSub += nFillSize-wLD->nSampleSizes[i];

			bDat = bNew;
			nSize = nNew;
		}
	}

	delete xBL;

	wSamps = PrepareSampArray();

#ifdef WINWSF
	g_mStatus->StepSubProgress(wLD->nSamples);
	wSamps->nSamplesTotal = wLD->nSamples; // Tell it how many total samples we have.
#endif

	// Write Mod FULL Size
	MAPP(&nSize,1,sizeof(unsigned long));

	// Compress MOD
	DoCompression(bDat,nSize,0,&bCDat,&nCSize,true);

	// Write Mod Compressed Size
	MAPP(&nCSize,1,sizeof(unsigned long));

	// Write (Compressed) Mod
	MAPP(bCDat,1,nCSize);

	// Free Compressed Mod
	free(bCDat);

	wSamps->nSCounts = nSCounts;

#ifdef WINWSF
	g_mStatus->StepSubProgress(wLD->nSamples);
#endif

	wsul nTimeSearch=0;

	// Read Samples
	for (i=0;i<wLD->nSamples;i++)
	{
		wsf_sample *wS;
		wsf_sampout wO;
		wsul nID;
		float fAmp;
		wsfb bFlag;
		wsul nPar1, nPar2;

		nPar1 = nPar2 = 0;
		bFlag = WSFS_NORMAL;
		fAmp = 100.0f;

		// Pick Sample
		wS = &wLD->bSamples[i];

		// Copy the sample over
		wO.nSize = gD.nSampSizes[i];
		wO.bSampData = (wsfb*)malloc(wO.nSize);
		wO.nBit = wS->nBit;
		wO.nCh = wS->nCh;
		wO.nFreq = wS->nFreq;
		memcpy(wO.bSampData,wS->bSampData,wO.nSize);

		if (wLD->nOggSize && wO.nSize >= wLD->nOggSize)
			wO.nOgg = 1; // Ogg
		else
			wO.nOgg = 0; // No Ogg

		nPar2 = 0;
		nID = m_wPack->SampleExist(&wO,&bFlag,&nPar1,&fAmp,&nPar2,wSamps);
		wsul nt;
		

		if (nID == WSFERR)
		{
			// Normal Sample Flag
			nID = m_wPack->AddSample(&wO);
			m_wPack->SetSampleName(nID,wS->cName);
			bFlag = WSFS_NORMAL;
			nPar1 = nPar2 = 0;
			fAmp = 100.0f;
			nt = PushIntoSampArray(nID,bFlag,nPar1,nPar2,fAmp,wSamps);
		}
		else{	// Increase the count of each type of sample.
			nSCount[bFlag] += 1;
			nt = PushIntoSampArray(nID,bFlag,nPar1,nPar2,fAmp,wSamps);
		}

		PushRefIntoArray(nt,wSamps);

		free(wO.bSampData);
	}

	// Write number of Samples
	MAPP(&wSamps->nSamps,1,sizeof(unsigned long));
	MAPP(&wSamps->nIDs,1,sizeof(wsul));

	for (i=0;i<wSamps->nSamps;i++)
	{
		WSF2Samp *wx;
		wx = &wSamps->bSamps[i];

		MAPP(&wx->nID,1,sizeof(wsul));
		MAPP(&wx->bFlag,1,sizeof(wsfb));
		MAPP(&wx->nPar1,1,sizeof(wsul));
		MAPP(&wx->nPar2,1,sizeof(wsul));
		MAPP(&wx->fAmp,1,sizeof(float));
	}

	for (i=0;i<wSamps->nIDs;i++){
		MAPP(&wSamps->bIDs[i],1,sizeof(wsul));
		MAPP(&gD.nSampSizes[i],1,sizeof(wsul));
		MAPP(&gD.nSampOffs[i],1,sizeof(wsul));
	}

	// Write Sub Diff
	MAPP(&nSub,1,sizeof(unsigned long));

	FreeSampArray(wSamps);

	FreeGSPData(&gD);
	free(bDat);

	// Save the size of output (Not in specification? Doesn't really matter, it's last.)
	MAPP(&sH,1,sizeof(unsigned short));

	MRESET(&wOut->bModData,&wOut->nSize);

	if (nSCounts)
		memcpy(nSCounts,&nSCount,sizeof(long)*(WSFS_COUNT));

	return 0;
#else
	return 1;
#endif
}

CWSFLoader::CWSFLoader()
{
	m_fPack = NULL;
}


//////////////////////////////////////////////////////
// CWSFLoader::LoadV1()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 6:30:08 PM
/// \param *wLD LoadData
///
// Finishes Loading :P
//////////////////////////////////////////////////////

int CWSFLoader::LoadV1( wsf_loaddata *wLD, char *cTPack )
{
	wsul *nIDs;
	wsfb *bFlags;
	wsul *nPars;
	wsfb *bAmps;
	wsul nExtras;
	wsul *bExtras;
	wsul i;
	wsfb bHi,bLo;
	char cPack[200];

	bHi = m_bHi;
	bLo = m_bLo;

	cPack[0] = 0;

	// After the mod is read, find samples
	MREAD(&wLD->nSamples,1,sizeof(unsigned long));
	wLD->bSamples = (wsf_sample*)malloc(sizeof(wsf_sample)*wLD->nSamples);
	wLD->nSampleSizes = (unsigned long*)malloc(sizeof(unsigned long)*wLD->nSamples);
	wLD->nSampleOffsets = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);		
	wLD->nModType = m_nModType;

	// Read Extras
	if (TESTVER(1,19))
	{
		wsul i;
		MREAD(&nExtras,1,sizeof(wsul));
		bExtras = (wsul*)malloc(sizeof(wsul)*nExtras);
		for (i=0;i<nExtras;i++)
		{
			MREAD(&bExtras[i],1,sizeof(wsul));
		}
	}
	else
	{
		nExtras=0;
		bExtras = (wsul*)malloc(0);
	}

	MREAD(&wLD->nSubDiff,1,sizeof(unsigned long));

	// Make array for number of IDs
	nIDs = (wsul*)malloc(sizeof(wsul)*(wLD->nSamples+nExtras));
	bFlags = (wsfb*)malloc(wLD->nSamples);
	nPars = (wsul*)malloc(wLD->nSamples*4);
	bAmps = (wsfb*)malloc(wLD->nSamples);

	// Read Sample IDs
	for (i=0;i<wLD->nSamples;i++)
	{
		wsul *nS;
		wsul *nID;

		nS = &wLD->nSampleSizes[i];
		nID = &nIDs[i];

		MREAD(nID,1,sizeof(wsul));
		MREAD(nS,1,sizeof(unsigned long));
		
		if (TESTVER(1,12))
		{
			MREAD(&wLD->nSampleOffsets[i],1,sizeof(wsul));
		}
		else
			wLD->nSampleOffsets[i] = 0;

		if (TESTVER(1,15))
		{
			MREAD(&bFlags[i],1,1);
			MREAD(&nPars[i],1,4);
			MREAD(&bAmps[i],1,1);
		}
		else
		{
			bFlags[i] = WSFS_NORMAL;
			bAmps[i] = 100;
			nPars[i] = 0;
		}
	}

	// Fill those extras
	for (i=0;i<nExtras;i++)
		nIDs[i+wLD->nSamples] = bExtras[i];

	free(bExtras);
	m_wPack->FreePack();

	if (!m_fPack)
	{
		// Adjust the pack name for position
		char *cPos;
		cPos = strrchr(m_cFile,'\\');

		if (!cPos)
			cPos = strrchr(m_cFile,'/');

		if (cPos)
		{
			char cB;
			cB = cPos[0];
			cPos[0] = 0;

			// Sets Pack Name
			sprintf(cPack,"%s%c%s",m_cFile,cB,cTPack);

			// Restore
			cPos[0] = cB;
		}
		else
			strcpy(cPack,cTPack);

		wsf_file *tPack;
		tPack = wsfopenfile(cPack);
		if (wLD->nPackOffset){
			tPack->nSize = wLD->nPackSize;
			wsfbegin(wLD->nPackOffset,tPack);
		}
		if (m_wPack->LoadPack(tPack,(wsul*)nIDs,(wsus)(wLD->nSamples+nExtras),0,(wLD->nPackOffset>0)))
		{
			free(nIDs);
			free(bFlags);
			free(nPars);
			free(bAmps);
			return 1;
		}
		wsfclose(tPack);

		strcpy(m_cCurPack,cPack);

	}else{

		wsul nFileSize;
		
		if (wLD->nPackOffset){
			nFileSize = m_fPack->nSize;
			m_fPack->nSize = wLD->nPackSize;
			wsfbegin(wLD->nPackOffset,m_fPack);
		}

		if (m_wPack->LoadPack(m_fPack,(wsul*)nIDs,(wsus)(wLD->nSamples+nExtras),0,(wLD->nPackOffset>0)))
		{
			free(nIDs);
			free(bFlags);
			free(nPars);
			free(bAmps);
			return 1;
		}

		if (wLD->nPackOffset){
			m_fPack->nSize = nFileSize;
			wsfbegin(0,m_fPack);
		}

		strcpy(m_cCurPack,"");
	}

	// Get Samples
	for (i=0;i<wLD->nSamples;i++)
	{
		wsf_sample *wS;
		wsf_sampout wO;
		
		// Pick Sample
		wS = &wLD->bSamples[i];

		if (m_wPack->GetSampV1(&wO,nIDs[i],bFlags[i],nPars[i],bAmps[i])){
			free(nIDs);
			free(bFlags);
			free(nPars);
			free(bAmps);
			if (wO.bSampData)
				free(wO.bSampData);
			return 1;
		}
		
		// Copy the sample over
		wS->nSize = wO.nSize;
		if (wO.bSampData)
		{
			wS->bSampData = (wsfb*)malloc(wS->nSize);
			memcpy(wS->bSampData,wO.bSampData,wS->nSize);
			free(wO.bSampData);
		}
		else
			wS->bSampData = NULL;

		wS->nBit = wO.nBit;
		wS->nCh = wO.nCh;

		wS->cSignature[0] = '\0';
	}

	free(nIDs);
	free(bFlags);
	free(nPars);
	free(bAmps);

	return 0;
}


//////////////////////////////////////////////////////
// CWSFLoader::LoadV2()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 6:59:57 PM
/// \param *wLD Loaddata
/// \param *cTPack Pack
///
//
//////////////////////////////////////////////////////

int CWSFLoader::LoadV2( wsf_loaddata *wLD, char *cTPack )
{
	wsul i;
	wsul *nIDs;
	wsfb bHi,bLo;
	wsul nSASize;
	WSF2Samps wSamps;
	char cPack[200];

	bHi = m_bHi;
	bLo = m_bLo;

	wSamps = PrepareSampArray();

	// Read in the size of our sample array
	MREAD(&nSASize,1,sizeof(wsul));

	// Read in the number of samples
	MREAD(&wLD->nSamples,1,sizeof(unsigned long));
	wLD->bSamples = (wsf_sample*)malloc(sizeof(wsf_sample)*wLD->nSamples);
	wLD->nSampleSizes = (unsigned long*)malloc(sizeof(unsigned long)*wLD->nSamples);
	wLD->nSampleOffsets = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);		
	wLD->nModType = m_nModType;

	nIDs = (wsul*)malloc(nSASize*sizeof(wsul));

	// Now we want to fill that sample array
	for (i=0;i<nSASize;i++)
	{
		float a;
		wsfb f;
		wsul p1,p2;
		wsul id;
		
		MREAD(&id,1,sizeof(wsul));
		MREAD(&f,1,sizeof(wsfb));
		MREAD(&p1,1,sizeof(wsul));
		MREAD(&p2,1,sizeof(wsul));
		MREAD(&a,1,sizeof(float));

		PushIntoSampArray(id,f,p1,p2,a,wSamps);
/*
		// Make IDs array for loading pack
		if (f != WSFS_SAMPAFTER)
			nIDs[i] = id;
		else
			if (i!=0)
				nIDs[i] = nIDs[i-1];
			else
				nIDs[i] = 0;
				*/
		nIDs[i] = id;
	}

	for (i=0;i<wLD->nSamples;i++)
	{
		wsul id;
		
		MREAD(&id,1,sizeof(wsul));
		MREAD(&wLD->nSampleSizes[i],1,sizeof(wsul));
		MREAD(&wLD->nSampleOffsets[i],1,sizeof(wsul));

		PushRefIntoArray(id,wSamps);
	}

	m_wPack->FreePack();

	if (!m_fPack)
	{
		// Adjust the pack name for position
		char *cPos;
		cPos = strrchr(m_cFile,'\\');

		if (!cPos)
			cPos = strrchr(m_cFile,'/');

		if (cPos)
		{
			char cB;
			cB = cPos[0];
			cPos[0] = 0;

			// Sets Pack Name
			sprintf(cPack,"%s%c%s",m_cFile,cB,cTPack);

			// Restore
			cPos[0] = cB;
		}
		else
			strcpy(cPack,cTPack);

		wsf_file *tPack;
		tPack = wsfopenfile(cPack);
		if (!tPack){
			return 1;
		}
		if (wLD->nPackOffset){
			tPack->nSize = wLD->nPackSize;
			wsfbegin(wLD->nPackOffset,tPack);
		}
		if (m_wPack->LoadPack(tPack,(wsul*)nIDs,(wsus)nSASize,0,(wLD->nPackOffset>0)))
		{
			free(nIDs);
			FreeSampArray(wSamps);
			return 1;
		}
		wsfclose(tPack);

		strcpy(m_cCurPack,cPack);

	}else{

		wsul nFileSize;
		
		if (wLD->nPackOffset){
			nFileSize = m_fPack->nSize;
			m_fPack->nSize = wLD->nPackSize;
			wsfbegin(wLD->nPackOffset,m_fPack);
		}

		if (m_wPack->LoadPack(m_fPack,(wsul*)nIDs,(wsus)nSASize,0,(wLD->nPackOffset>0)))
		{
			FreeSampArray(wSamps);
			free(nIDs);
			return 1;
		}

		if (wLD->nPackOffset){
			m_fPack->nSize = nFileSize;
			wsfbegin(0,m_fPack);
		}

		strcpy(m_cCurPack,"");
	}

	free(nIDs);

	// Get Samples
	for (i=0;i<wLD->nSamples;i++)
	{
		wsf_sample *wS;
		wsf_sampout wO;
		WSF2Samp *wV;
		
		// Pick Sample
		wV = &wSamps->bSamps[wSamps->bIDs[i]];
		wS = &wLD->bSamples[i];

		wO.bSampData=0;
//		( wsf_sampout *wOut, unsigned long nID, wsfb bFlag, wsul nPar, wsul nPar2, float bAmp, WSF2Samps wSamps )
 		if (m_wPack->GetSampV2(&wO,wV->nID,wV->bFlag,wV->nPar1,wV->nPar2,wV->fAmp,wSamps)){
			if (wO.bSampData)
				free(wO.bSampData);
			FreeSampArray(wSamps);
			return 1;
		}

		// Error Cheque
		if (wO.nSize != wLD->nSampleSizes[i]){
			if (wO.bSampData)
				free(wO.bSampData);
			FreeSampArray(wSamps);
			return 1;
		}
 
		// Copy the sample over
		wS->nSize = wO.nSize;
		if (wO.bSampData)
		{
			wS->bSampData = (wsfb*)malloc(wS->nSize);
			memcpy(wS->bSampData,wO.bSampData,wS->nSize);
		}
		else
			wS->bSampData = NULL;

		wS->nBit = wO.nBit;
		wS->nCh = wO.nCh;

		wS->cSignature[0] = '\0';

		if (wO.bSampData)
			free(wO.bSampData);
	}

	FreeSampArray(wSamps);
	

	return 0;
}

//***************************************************************************** */
// CWSFMod::LoadWSF()
// 
// Purpose: Loads WSF
// 
// Author : [Josh Whelchel], Created : [2/12/2003 12:35:21 PM]
// Parameters:	
// 
// [*MFile] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFLoader::Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD )
{
	wsfb bHi,bLo;
	char cHead[4];
	unsigned long nCSize;
	wsfb *bCDat;
	char cOutExt[5];
	char cTPack[27];

	// Open File
	MOPEN(bData,nSize);

	// Read Header
	MREAD(cHead,1,4);

	// Version
	MREAD(&bHi,1,1);
	MREAD(&bLo,1,1);
	m_bHi = bHi;
	m_bLo = bLo;

	cTPack[26] = 0;

	if (bHi > WSFMVERHI || (bHi == WSFMVERHI && bLo > WSFMVERLO))
		return 1; // INCORRECT VERSION, NOT SUPPORTED :<

	// Check it
	if (strncmp(cHead,WSFMHEAD,4)!=0)
		return 1;	// Wrong Header

	// Read Pack name
	MREAD(cTPack,1,26);

	if (TESTVER(1,11)) {
		MREAD(cOutExt,1,5);
		m_nModType = GetModType(cOutExt);
	}
	else
		m_nModType = WSFM_IT;	// Assume its an IT because before V1.11 they all were :P

	wLD->nModType = m_nModType;

	// Read size of mod file ahead and then read the mod
	MREAD(&wLD->nModSize,1,sizeof(unsigned long));		// Full Size
	MREAD(&nCSize,1,sizeof(unsigned long));		// Compressed Size

	// Allocate Room 
	bCDat = (wsfb*)malloc(nCSize);

	// Read Compressed
	MREAD(bCDat,1,nCSize);

	// DECOMPRESS :D
	DoCompression(bCDat,nCSize,wLD->nModSize,&wLD->bModData,&wLD->nModSize,false);
	free(bCDat);

	// Do loading based on version!
	if (bHi < 2)
		return LoadV1(wLD,cTPack); // Ver 1.x
	else
		return LoadV2(wLD,cTPack); // Ver 2.x+

	// This wouldn't make sense!
	return 1;
}

//////////////////////////////////////////////////////
//  CWSFLoader::SetPack()
// 
//  Description: 
/// Sets pack to wsf_file
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 8:13:29 AM
/// \param *wf 
//////////////////////////////////////////////////////

void CWSFLoader::SetPack( wsf_file *wf )
{
	m_fPack = wf;
}

//////////////////////////////////////////////////////
//  CS3MLoader::Load()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 12:28:33 AM
/// \param *bData S3M Data
/// \param nSize Size of S3M Data
/// \param *wLD LoadData
/// \return 
/// \note There is no way to tell if an S3M file is truly an S3M
/// in terms of a file header until after title :<
/// \brief Loads S3M
/// 
// 
//////////////////////////////////////////////////////

int CS3MLoader::Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD )
{
#ifndef WSFNOSAVE
	wsul i;
	wsf_gspdata gD;

	if (GetSamplePointers(bData,nSize,&gD))
		return 1;

	// Open the File
	MOPEN(bData,nSize);

	// Load The Title
	MREAD(wLD->cTitle,1,28);
	wLD->cTitle[28] = 0;

	// Copies learnt stuff over
	wLD->nSamples = gD.nSamples;
	wLD->nSampleSizes = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);
	wLD->bSamples = (wsf_sample*)malloc(sizeof(wsf_sample)*wLD->nSamples);

	// Loading samples now has become a matter of the below
	for (i=0;i<wLD->nSamples;i++)
	{
		wsul nSSize;
		wsf_sample *wS;
		wS = &wLD->bSamples[i];

		// Copy data over from gspData
		wS->nBit = gD.nBit[i];
		wS->nCh = gD.nCh[i];
		wS->nFreq = gD.nFreqs[i];
		wS->nSamps = gD.nInSamps[i];
		wS->cSignature[0] = 0;
		
		wS->bSampData = NULL;
		nSSize = gD.nSampSizes[i];
		wS->nSize = nSSize;

		strncpy(wS->cName,(char*)gD.cNames+(i*27),26);
		wS->cName[26]=0;
		
		if (!nSSize)
			continue;
		
		wS->bSampData = (wsfb*)malloc(nSSize);

		// Load from offset

		MBEGIN(gD.nSampOffs[i]);

		// Of course we are assuming these assholes are unsigned. (TODO: Don't Assume)
		{
			wsul xn;

			wS->nSize = nSSize;
			wS->nRawSize = wS->nSize;
			wLD->nSampleSizes[i] = wS->nSize;
			if (wS->nBit == 8)
			{
				unsigned char sb;
				for (xn=0;xn<nSSize;xn++)
				{
					MREAD(&sb,1,1);
					sb ^= 0x80;
					((signed char *)(wS->bSampData))[xn] = sb;
				}
			}
			else
			{
				unsigned short lb;
				nSSize /= 2;
				for (xn=0;xn<nSSize;xn++)
				{
					MREAD(&lb,1,2);
					lb ^= 0x8000;
					((signed short *)(wS->bSampData))[xn] = lb;
				}
			}
		}
	}

	FreeGSPData(&gD);

	// Load MOD data
	wLD->bModData = (wsfb*)malloc(nSize);
	wLD->nModSize = nSize;
	memcpy(wLD->bModData,bData,nSize);
	wLD->nSubDiff = 0;

	return 0;
#else
	return 1;
#endif
}

//////////////////////////////////////////////////////
//  CS3MLoader::Save()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 1:27:33 AM
/// \param *wOut Output Buffer
/// \param *wLD Data to Save
/// \brief Saves S3M Data
/// 
// 
//////////////////////////////////////////////////////

int CS3MLoader::Save( wsf_modout *wOut, wsf_loaddata *wLD )
{
	wsul i;

	// Convert samples to unsigned :?
	AllowSampleEdit(wLD);

	for (i=0;i<wLD->nSamples;i++)
	{
		wsul x;
		wsf_sample *s;
		wsfb *b;
		s = &wLD->bSamples[i];
		b = (wsfb*)malloc(s->nSize);

		s->nSamps = s->nSize;
		
		if (s->nBit == 16){
			s->nSamps /= 2;
			short me;
			unsigned short *p;
			p = (unsigned short *)b;
			for (x=0;x<s->nSamps;x++){
				me = ((short*)s->bSampData)[x];
				me ^= 0x8000;
				p[x] = me;
			}
		}
		else{
			char me;
			unsigned char *p;
			p = (unsigned char*)b;
			for (x=0;x<s->nSamps;x++){
				me = ((char*)s->bSampData)[x];
				me ^= 0x80;
				b[x] = me;
			}
		}

		free(s->bSampData);
		s->bSampData = b;
	}

	return 0;
}

//////////////////////////////////////////////////////
//  CS3MLoader::GetSamplePointers()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 12:48:46 AM
/// \param *bData S3M Data
/// \param nSize S3M Size
/// \param *gspData GSPData Struct
/// \brief Gets Sample Pointers from S3M
/// 
// 
//////////////////////////////////////////////////////

int CS3MLoader::GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData )
{
#ifndef WSFNOSAVE
	char cHead[4];
	wsfb bTest;
	wsus nOrds, nSamps;
	wsul *nSOffs;
	wsul i;
	wsfb nType;

	// Open the File
	MOPEN(bData,nSize);

	// Load The Title
	MSEEK(28);

	// Read the 1Ah
	MREAD(&bTest,1,1);

	if (bTest != 26)
		return 1;	// Not Equal Header

	MREAD(&nType,1,1);

	// Two Blanks
	MBEGIN(32);

	// We should now be at 32 (20h)
	MREAD(&nOrds,1,2);	// Orders
	MREAD(&nSamps,1,2); // Insturments

	// Go to 'SCRM' header
	MBEGIN(44);

	MREAD(cHead,1,4);

	if (strncmp("SCRM",cHead,4)!=0)
		return 1;	// unmatching header

	// Go to 60h (96) + orders
	MBEGIN(96+nOrds);

	// This brings us to insturment pointers
	nSOffs = (wsul*)malloc(sizeof(wsul)*nSamps);
	
	gspData->nSamples = 0;
	InitGSPData(gspData);

	for (i=0;i<nSamps;i++)
	{
		wsus nSO;
		MREAD(&nSO,1,2);
		nSOffs[i] = nSO * 16;
	}

	// Now read in each sample header
	for (i=0;i<nSamps;i++)
	{
		wsul nS;
		wsul nOff;
		wsus nX;
		wsul nLen;
		char cName[27];
		wsul nFreq;

		nS = nSOffs[i];
		MBEGIN(nS);
		MREAD(&bTest,1,1);	// Read Test Character
		
		if (bTest != 1){
//			PushIntoGSP(gspData,0,0,0,1,1,"INVALISAMPLE");
			continue;
		}

		MREAD(cName,12,1);
		cName[12]=0;

		MSEEK(1);

		// Read Sample Offset
		MREAD(&nX,1,2);
		nOff = nX << 4;

		// Read Length
		MREAD(&nLen,1,4);

		// Read Flag
		MBEGIN(nS+31);
		MREAD(&bTest,1,1); 

		if (nType != 16){
			MREAD(&nFreq,1,4);
		}
		else
		{
			wsus nF;
			MREAD(&nF,1,2);
			MSEEK(2);
			nFreq = nF;
		}

		if (nLen == 1)
			nLen = 0;

		wsul nInSamps;
		wsfb nCh,nBit;
		nInSamps = nLen;

		if (bTest & 2){	// Stereo
			nLen *= 2;
			nCh = 2;
		}
		else
			nCh = 1;

		if (bTest & 4){ // 16bit
			nLen *= 2;
			nBit = 16;
		}
		else
			nBit = 8;

//		gspData->nSampSizes[i] = nLen;
//		gspData->nSampOffs[i] = nOff * 16;

		MSEEK(15);
//		MREAD(cName,26,1);
//		cName[26]=0;

		PushIntoGSP(gspData,nLen,nOff,nInSamps,nCh,nBit,cName,nFreq);
	
	}

	free(nSOffs);

	return 0;
#else
	return 1;
#endif
}

// WSFL_XM

//////////////////////////////////////////////////////
//  CXMLoader::GetSamplePointers()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 7:08:13 PM
/// \param *bData XM Data
/// \param nSize Size of XM Data
/// \param *gspData gspData
/// \brief Gets Sample Pointers for XM Modules
/// 
// 
//////////////////////////////////////////////////////

int CXMLoader::GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData )
{
#ifndef WSFNOSAVE
	char cHead[18];
	wsfb bTest;
	wsus nTest;
	wsul nLong;
	wsus nInst;
	wsus nOrds;
	wsus nPats;
	wsul nSamps;
	wsul i;
	// Start Taking notes baby

#define XM_SAMPLE_16BIT            16
#define XM_SAMPLE_STEREO           32

	MOPEN(bData,nSize);

	// Read Header
	MREAD(cHead,1,17);
	cHead[17] = 0;

	// Test Header
	if (strcmp(cHead,"Extended Module: ")!=0)
		return 1;

	// Load Title :>
	MREAD(m_cTitle,1,20);

	m_cTitle[20] = 0;

	// Load $1a
	MREAD(&bTest,1,1);

	if (bTest != 0x1A)
		return 1;

	// Past Tracker Name
	MSEEK(20);

	// Read Version
	MREAD(&nTest,1,2);

	if (nTest != 0x0104)
		return 1;

	// ---- HEADER ----
	MREAD(&nLong,1,4);

	if (nLong != 0x0114)
		return 1;

	// Skip to Number of Insturments and Orders
	MREAD(&nOrds,1,2);
	
	MSEEK(4);

	MREAD(&nPats,1,2);
	MREAD(&nInst,1,2);
	
	MSEEK(6);

	// Now We're somewhere new
	// Pattern Order Table
	// (SKIP IT :O)
	MSEEK(256);

	// Now We're at pattern datas?
	for (i=0;i<nPats;i++)
	{
		MREAD(&nLong,1,4);

		if (nLong != 9)
			return 1;

		MSEEK(3);

		// Load Pattern Data Size and then skip it >:)
		MREAD(&nTest,1,2);
		MSEEK(nTest);
	}

	// INIT :D
	nSamps = 0;
	gspData->nSamples = 0;
	InitGSPData(gspData);

	// NOW WE ARE AT INSTURMENTS :D
	for (i=0;i<nInst;i++)
	{
		wsul nSub;
		wsul nO;
		wsus x;
		wsul nRead;
		wsus nS;

		nO = 0;
		nSub = 0;

		MREAD(&nLong,1,4);

		MSEEK(22);

		MSEEK(1);

		// Read Number of Samples
		MREAD(&nS,1,2);

		// Increase number of samples
		nSamps += nS;

		// Seek 25
		MSEEK(25);

		nRead = 22+4+2+25+1;
		MSEEK(nLong-nRead);

		// Skip right to sample data
		if (nS <= 0)
		{
			continue;			
		}
		
		// Seek to end where sample data is

		nO = MTELL();

		for (x=0;x<nS;x++)
		{
			// FOREACH SAMPLE :O
			wsul nLen;
			wsul nxSamps;
			wsfb nCh;
			wsfb nBit;
			wsul nFreq;
			wsul nOff;
			wsfb type;
			signed char relative_note_number;
			signed char finetune;

			MREAD(&nLen,1,4);

			MSEEK(9);
			MREAD(&finetune,1,1);
			MREAD(&type,1,1);
			MSEEK(1);
			MREAD(&relative_note_number,1,1);
			
			nCh = (type&XM_SAMPLE_STEREO)+1;
			if (type&XM_SAMPLE_16BIT)
				nBit = 16;
			else
				nBit = 8;
//			nBit = ((type&XM_SAMPLE_16BIT)*2)+8;

			nxSamps = nLen / nCh / (nBit/8);

			// Calculate Offset
			nOff = nO + (40*nS) + nSub;
			
			nSub += nLen;

			MSEEK(1);
//			MSEEK(22);

			char cName[27];
			memset(cName,0,27);
			
			MREAD(cName,22,1);

#define SEMITONE_BASE 1.059463094359295309843105314939748495817
#define PITCH_BASE 1.000225659305069791926712241547647863626

			nFreq = 
				(long)(16726.0*pow(SEMITONE_BASE, relative_note_number)*pow(PITCH_BASE, finetune*2));

			if (nLen)
				PushIntoGSP(gspData,nLen,nOff,nxSamps,nCh,nBit,cName,nFreq);
		}

		// Seek past sample data (again) XD
		MSEEK(nSub);
	}

	return 0;
#else
	return 1;
#endif
}

//////////////////////////////////////////////////////
//  CXMLoader::Save()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 8:02:32 PM
// 
/// 
// 
//////////////////////////////////////////////////////

int CXMLoader::Save( wsf_modout *wOut, wsf_loaddata *wLD )
{
	wsul i;

	// Convert samples to delta :?
	AllowSampleEdit(wLD);

	/*
		old=0;
		for i=1 to len
			new=sample[i]+old;
			sample[i]=new;
			old=new;

		
	*/

	for (i=0;i<wLD->nSamples;i++)
	{
		wsul x;
		wsf_sample *s;
		wsfb *b;
		s = &wLD->bSamples[i];
		b = (wsfb*)malloc(s->nSize);

		s->nSamps = s->nSize;
		
		//*((short *)(&buffer[bufcount])) = (short)(s_new - s_old);
		//s_old = s_new;
		if (s->nBit == 16){
			int old;
			int me;
			old = 0;
			s->nSamps /= 2;
			for (x=0;x<s->nSamps;x++){
				me = ((short*)s->bSampData)[x];
				((short*)b)[x] = me - old;
				old = me;
			}
		}
		else{
			int old;
			int me;
			old = 0;
			for (x=0;x<s->nSamps;x++){
				me = ((signed char*)s->bSampData)[x];
				((signed char*)b)[x] = me - old;
				old = me;
			}
		}

		free(s->bSampData);
		s->bSampData = b;
	}

	return 0;
}

//////////////////////////////////////////////////////
//  CXMLoader::Load()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 7:53:46 PM
/// \param *bData XM Data
/// \param nSize Size of XM Data
/// \param *wLD Loaddata Struct
// 
//////////////////////////////////////////////////////

int CXMLoader::Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD )
{
#ifndef WSFNOSAVE
	wsul i;
	wsf_gspdata gD;

	// Get Sample Pointers
	if (GetSamplePointers(bData,nSize,&gD))
		return 1;

	wLD->nSamples = gD.nSamples;
	strcpy(wLD->cTitle,m_cTitle);
	
	wLD->nSampleSizes = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);
	wLD->bSamples = (wsf_sample*)malloc(sizeof(wsf_sample)*wLD->nSamples);
	wLD->nSubDiff = 0;

	MOPEN(bData,nSize);

	for (i=0;i<gD.nSamples;i++)
	{
		wsf_sample *wS;
		wsul nSSize;
		
		// Make Pointer
		wS = &wLD->bSamples[i];

		memset(wS,0,sizeof(wsf_sample));

		// Set Sample Size
		nSSize = gD.nSampSizes[i];
		wLD->nSampleSizes[i] = nSSize;
		wS->nSize = nSSize;
		strncpy(wS->cName,(char*)gD.cNames+(i*27),26);
		wS->cName[26]=0;

		wS->nBit = gD.nBit[i];
		wS->nFreq = gD.nFreqs[i];
		wS->nCh = gD.nCh[i];
		wS->nSamps = gD.nInSamps[i];

		// Fill it Out
		if (nSSize)
		{
			// Delta Loading
			int old;
			wsul x;

			wS->bSampData = (wsfb*)malloc(nSSize);
			
			// Read in Sample from Offset Given (pray its right)
			MBEGIN(gD.nSampOffs[i]);
//			MREAD(wS->bSampData,1,nSSize);

			old = 0;
			if (wS->nBit == 8)
			{
				signed char read;
				for (x = 0; x < wS->nSamps; x++){
					// ((short *)sample->left)[i] = old += dumbfile_igetw(f);
					MREAD(&read,1,1);
					((char*)wS->bSampData)[x] = old += read;
				}
			}
			else
			{
				signed short read;
				for (x = 0; x < wS->nSamps; x++){
					// ((short *)sample->left)[i] = old += dumbfile_igetw(f);
					MREAD(&read,1,2);
					((short*)wS->bSampData)[x] = old += read;
				}
			}

		}
	}

	// Free Sample Pointers
	FreeGSPData(&gD);

	// Load MOD data
	wLD->bModData = (wsfb*)malloc(nSize);
	wLD->nModSize = nSize;
	memcpy(wLD->bModData,bData,nSize);

	return 0;
#else
	return 1;
#endif
}

// WSFL_MOD

//////////////////////////////////////////////////////
//  CMODLoader::LoadSampleHeader()
// 
//  Description: 
/// Loads a sample's header :D
/// 
/// 
/// \author Josh Whelchel
/// \date 8/3/2003 1:37:29 PM
/// \param *gspData gspData
/// \return nonzero if error
/// 
//////////////////////////////////////////////////////

#define BOGGLE(x) ( (( ( (wsfb*)&x)[1]) | ( ((wsfb*)&x)[0] << 8)) << 1 )

int CMODLoader::LoadSampleHeader( wsf_gspdata *gspData, char *cName )
{
	wsul nSize;
	wsus nSamps;
	wsfb nCh;
	wsfb nBit;

	// Read length of sample
	MREAD(&nSamps,1,2);

	nSamps = BOGGLE(nSamps);
	nSize = nSamps; 

	// Erk :o
	MSEEK(6);

	// Mod Documentation says so.
	nCh = 1;
	nBit = 8;

	if (nSize)
		PushIntoGSP(gspData,nSize,666,nSamps,nCh,nBit,cName,22050);
	else
		return 1;

	return 0;
}

//////////////////////////////////////////////////////
//  CMODLoader::GetSamplePointers()
// 
/// \author Josh Whelchel
/// \date 8/3/2003 1:31:37 PM
/// \param *bData MOD Data
/// \param nSize Size of MOD Data
/// \param *gspData GSP Data
/// \return 
/// 
//////////////////////////////////////////////////////

int CMODLoader::GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData )
{
#ifndef WSFNOSAVE
	wsul nSamples;
	wsul x;
	wsul i;
	wsul nPat;
	wsfb nOrd;
	wsul nCh;
	char tmpbuf[22];
	wsfb nSkip[31];
	wsfb order[128];

	// Assume 31 samples :3
	nSamples = 31;

	// Open
	MOPEN(bData,nSize);

	// Seek past title
	MSEEK(20);

	gspData->nSamples = 0;
	InitGSPData(gspData);

	// Read the first 15 samples
	for (i=0;i<15;i++)
	{
		char cName[28];
		MREAD(cName,22,1);
		cName[22]=0;
		nSkip[i] = LoadSampleHeader(gspData,cName);
	}
	
	// Read the temp buffer
	MREAD(tmpbuf,1,22);

	// Now we smile like they do at mcdonalds
	// :D

	// Now lets see if there are actually 15 or 31 samples ;_;
	// Stupid mod f00kers

	for (i = 0; i < 20; i++) {
		static unsigned char nottext[256] = {
			/*   0 */ 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
			/*  16 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/*  32 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/*  48 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/*  64 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/*  80 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/*  96 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			/* 112 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
			/* 128 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 144 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 160 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 176 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 192 */ 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
			/* 208 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 224 */ 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			/* 240 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
		};
		if (nottext[tmpbuf[i]]) {
			/* We have encountered a non-text character, so 15 samples. */
			nSamples = 15;
			goto samples_loaded_and_we_overshot_by_22_bytes;
		}
	}

	// Load that in, we skipped its title already
	nSkip[15] = LoadSampleHeader(gspData,"");

	// Load the other fooks in.
	for (i=16;i<31;i++)
	{
		char cName[27];
		MREAD(cName,22,1);
		cName[22] = 0;
		nSkip[i] = LoadSampleHeader(gspData,cName);
	}

	MREAD(tmpbuf,1,22);

samples_loaded_and_we_overshot_by_22_bytes:
	nOrd = tmpbuf[0];

	memcpy(order, &tmpbuf[2], 20);
	MREAD(order + 20, 108, 1);

	// This is horse shite
	if (nSamples == 15)
		nCh = 4;
	else {
		wsul fft;
		MREAD(&fft,1,sizeof(wsul));

		if ((fft & 0x0000FFFFL) == WSF_ID(0,0,'C','H')) {
			nCh = ((fft & 0x00FF0000L) >> 16) - '0';
			if ((unsigned int)nCh >= 10)
				/* Rightmost character wasn't a digit - assume four channels. */
				nCh = 4;
			else {
				nCh += (((fft & 0xFF000000L) >> 24) - '0') * 10;
				if ((unsigned int)(nCh - 1) >= 32 - 1)
					// Might be possible to allow up to DUMB_IT_N_CHANNELS later... (when the static buffer in it_mod_read_pattern changes to a malloc)
					/* No channels or too many? Can't be right... assume 4. */
					nCh = 4;
			}
		} else if ((fft & 0x00FFFFFFL) == WSF_ID(0,'C','H','N')) {
			nCh = (fft >> 24) - '0';
			if ((unsigned int)(nCh - 1) >= 9) {
				/* Character was '0' or it wasn't a digit */
				nCh = 4;
			}
		} else if ((fft & 0xFFFFFF00L) == WSF_ID('T','D','Z',0)) {
			nCh = (fft & 0x000000FFL) - '0';
			if ((unsigned int)(nCh - 1) >= 9)
				/* We've been very lenient, given that it should have been 1, 2
				 * or 3, but this MOD has been very naughty and must be punished.
				 */
				nCh = 4;
		} else switch (fft) {
			case WSF_ID('F','L','T','8'):
				nCh = 0;
				/* 0 indicates a special case; two four-channel patterns must
				 * be combined into one eight-channel pattern. Pattern
				 * indexes must be halved. Why oh why do they obfuscate so?
				 */
				for (i = 0; i < 128; i++)
					order[i] >>= 1;
				break;
			case WSF_ID('O','C','T','A'):
			case WSF_ID('C','D','8','1'):
				nCh = 8;
				break;
			/* case WSF_ID('M','.','K','.'): */
			/* case WSF_ID('M','!','K','!'): */
			/* case WSF_ID('F','L','T','4'): */
			default:
				nCh = 4;
		}
	}
	
	// Work out number of patterns
	nPat = 0;
	for (i = 0; i < 128; i++)
		if (nPat < order[i])
			nPat = order[i];
	nPat++;

	// Okay?

	for (i = 0; i < nPat; i++)
	{
		// Skip the patterns here :p
		if (nCh == 0) {
			int pos;
			/* Read the first four channels, leaving gaps for the rest. */
			for (pos = 0; pos < 64*8*4; pos += 8*4)
				MSEEK(4*4);
			/* Read the other channels into the gaps we left. */
			for (pos = 4*4; pos < 64*8*4; pos += 8*4)
				MSEEK(4*4);
//			n_channels = 8;
		} else
			MSEEK(64*nCh*4);
	}

	// Thank you ;_;

	for (i=0,x=0;i<nSamples;i++)
	{
		if (nSkip[i])
			continue;

		gspData->nSampOffs[x] = MTELL();
		MSEEK(gspData->nSampSizes[x]);

		x++;
	}

	// Yay ;_; I hope

	return 0;
#else
	return 1;
#endif
}

//////////////////////////////////////////////////////
//  CMODLoader::Load()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 12:28:33 AM
/// \param *bData MOD Data
/// \param nSize Size of MOD Data
/// \param *wLD LoadData
/// \return 
/// \note MOD files are by far the ugliest format ever created.
///		Somebody should be shot, and they should be shot now.
/// \brief Loads MOD
/// 
// 
//////////////////////////////////////////////////////

int CMODLoader::Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD )
{
#ifndef WSFNOSAVE
	wsul i;
	wsf_gspdata gD;

	if (GetSamplePointers(bData,nSize,&gD))
		return 1;

	// Open the File
	MOPEN(bData,nSize);

	// Load The Title
	MREAD(wLD->cTitle,1,20);

	// Copies learnt stuff over
	wLD->nSamples = gD.nSamples;
	wLD->nSampleSizes = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);
	wLD->bSamples = (wsf_sample*)malloc(sizeof(wsf_sample)*wLD->nSamples);

	// Loading samples now has become a matter of the below
	for (i=0;i<wLD->nSamples;i++)
	{
		wsul nSSize;
		wsf_sample *wS;
		wS = &wLD->bSamples[i];

		// Copy data over from gspData
		wS->nBit = gD.nBit[i];
		wS->nCh = gD.nCh[i];
		wS->nSamps = gD.nInSamps[i];
		wS->cSignature[0] = 0;
		wS->nFreq = gD.nFreqs[i];
		
		wS->bSampData = NULL;
		nSSize = gD.nSampSizes[i];
		wS->nSize = nSSize;
		wS->nRawSize = wS->nSize;
		strncpy(wS->cName,(char*)gD.cNames+(i*27),26);
		wS->cName[26]=0;

//		printf("%d: %dbit %dch %dsamps %dfreq %dsize\n",i,wS->nBit,wS->nCh,wS->nSamps,wS->nFreq,wS->nSize);
		
		if (!nSSize)
			continue;
		
		wS->bSampData = (wsfb*)malloc(nSSize);

		// Load from offset
		MBEGIN(gD.nSampOffs[i]);
		MREAD(wS->bSampData,1,nSSize);
		wS->nSize = nSSize;
		wLD->nSampleSizes[i] = nSSize;
	}

	FreeGSPData(&gD);

	// Load MOD data
	wLD->bModData = (wsfb*)malloc(nSize);
	wLD->nModSize = nSize;
	memcpy(wLD->bModData,bData,nSize);
	wLD->nSubDiff = 0;

	return 0;
#else
	return 1;
#endif
}

//////////////////////////////////////////////////////
//  CMODLoader::Save()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 1:27:33 AM
/// \param *wOut Output Buffer
/// \param *wLD Data to Save
/// \brief Saves MOD Data
/// 
// 
//////////////////////////////////////////////////////

int CMODLoader::Save( wsf_modout *wOut, wsf_loaddata *wLD )
{
	// Does Nothing

	return 0;
}

//***************************************************************************** */
// CWSFMod::SaveWSF() Save()
// 
// Purpose: Saves WSF
// 
// Author : [Josh Whelchel], Created : [2/15/2003 4:13:47 PM]
// Parameters:	
// 
// [*wOut] :
// 
// Returns:	int
// 
//***************************************************************************** */

#ifdef WANTSAVEVER1

/// \deprecated SaveVer1 applies to saving older formats. For this downgrading
/// it is reccommended to just use CWSFLoaderV1. (CWSFModV1, CWSFPackV1)

//////////////////////////////////////////////////////
// CWSFLoader::SaveVer1()
// 
/// \author Josh Whelchel
/// \param *wOut Data to be written to (OUT)
/// \param *wLD Load Data (IN)
/// \param *nSCounts Sample Counts (IN)
/// \brief Saves Version 1.x WSFMOD.
/// I moved this to the end of the source file because it is old.
//
//////////////////////////////////////////////////////

int CWSFLoader::SaveVer1( wsf_modout *wOut, wsf_loaddata *wLD, long *nSCounts )
{
	unsigned long i;
	wsfb bHi,bLo;
	wsfb *bDat;
	unsigned long nSize;
	wsfb bFill[4];
	unsigned short sH;
	unsigned long nCSize;
	CBaseLoader *xBL;
	wsfb *bCDat;
	long nSub;
	wsf_gspdata gD;
	char cOutExt[5];
	long nSCount[WSFS_COUNT];

	wsul *nPar;
	wsfb *bFlag;
	wsfb *bAmp;
	wsul *nIDx;
	
	wsul nExtras;
	wsul *bExtras;

	nExtras = 0;
	bExtras = (wsul*)malloc(0);
	
	for (i=0;i<WSFS_COUNT;i++)
		nSCount[i] = 0;

	sH = 0;

	bFill[0] = 'W';
	bFill[1] = 'S';
	bFill[2] = 'F';
	bFill[3] = '_';

	nSize = wLD->nModSize;
	bDat = (wsfb*)malloc(nSize);
	memcpy(bDat,wLD->bModData,nSize);

	// init nSub
	nSub = 0;

	// Begin to write :>
	MOPEN(wOut->bModData,0);

	bHi = /*WSFMVERHI*/1;
	bLo = /*WSFMVERLO*/20;
	
	// Write Header
	MAPP(WSFMHEAD,1,4);

	// Write Version
	MAPP(&bHi,1,1);
	MAPP(&bLo,1,1);

	// Write Pack Name
	MAPP(m_cCurPack,1,26);

	// Write Mod Type
	memset(cOutExt,0,5);
	strncpy(cOutExt,g_cModExt[wLD->nModType],5);
	MAPP(cOutExt,1,5);

	// Null Sample Places
	// Get Base Loader for that filetype
	xBL = GetBaseLoader(wLD->nModType);
	nSub = 0;

	if (xBL->GetSamplePointers(wLD->bModData,wLD->nModSize,&gD))
	{
		delete xBL;
		MRESET(&wOut->bModData,&wOut->nSize);

		free(wOut->bModData);
		wOut->bModData = NULL;
		
		free(bDat);

		free(bExtras);
		return 1;
	}

	for (i=0;i<wLD->nSamples;i++)
	{
		wsfb *bNew;
		wsul nNew;
		wsul nSOff;

		nSOff = gD.nSampOffs[i]+nSub;

		if (wLD->nSampleSizes[i])
		{
			bNew = ReplaceData(bDat,nSize,&nNew,nSOff,wLD->nSampleSizes[i],(wsfb*)bFill,4);
			free(bDat);

			nSub += 4-wLD->nSampleSizes[i];

			bDat = bNew;
			nSize = nNew;
		}
	}

	delete xBL;

#ifdef WINWSF
	g_mStatus->StepSubProgress(wLD->nSamples);
#endif

	// Write Mod FULL Size
	MAPP(&nSize,1,sizeof(unsigned long));

	// Compress MOD
	DoCompression(bDat,nSize,0,&bCDat,&nCSize,true);

	// Write Mod Compressed Size
	MAPP(&nCSize,1,sizeof(unsigned long));

	// Write (Compressed) Mod
	MAPP(bCDat,1,nCSize);

	// Free Compressed Mod
	free(bCDat);

	// Write number of Samples
	MAPP(&wLD->nSamples,1,sizeof(unsigned long));

	nIDx = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);
	nPar = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);
	bFlag = (wsfb*)malloc(sizeof(wsfb)*wLD->nSamples);
	bAmp = (wsfb*)malloc(sizeof(wsfb)*wLD->nSamples);

	wsul nTimeSearch=0;

#ifdef WINWSF
	g_mStatus->StepSubProgress(wLD->nSamples);
#endif

	// Read Samples
	for (i=0;i<wLD->nSamples;i++)
	{
		wsf_sample *wS;
		wsf_sampout wO;
		unsigned long *nS;
		unsigned long nID;
		char cSampSig[SIGNATURESIZE+1];

		// Pick Sample
		wS = &wLD->bSamples[i];
		nS = &wLD->nSampleSizes[i];

		// Copy the sample over
		wO.nSize = wS->nSize;
		wO.bSampData = (wsfb*)malloc(wO.nSize);
		wO.nBit = wS->nBit;
		wO.nCh = wS->nCh;
		wO.nFreq = wS->nFreq;
		memcpy(wO.bSampData,wS->bSampData,wO.nSize);

		if (wLD->nOggSize && wO.nSize >= wLD->nOggSize)
			wO.nOgg = 1; // Ogg
		else
			wO.nOgg = 0; // No Ogg

		// Create Signature
		m_wPack->CreateSignature(wS->bSampData,wS->nSize,wS->nCh,wS->nBit,wS->nSamps,cSampSig);
		
		/* // THIS IS HOW WE USED TO DO THINGS ^_^
		nID = m_wPack->SampleExist(cSampSig);

		if (nID == WSFERR)
			nID = m_wPack->AddSample(&wO,(wsfb*)cSampSig);
		*/

		wsul p;
		wsfb b,ba;
		float a;

		nID = m_wPack->SampleExist(&wO,&b,&p,&a);

		if (nID == WSFERR)
		{
			// Normal Sample Flag
			nID = m_wPack->AddSample(&wO,(wsfb*)cSampSig);
			m_wPack->SetSampleName(nID,wS->cName);
			b = WSFS_NORMAL;
			p = 0;
			ba = 100;
			a = 100.0f;
		}
		else if (b == WSFS_SAMPAFTER)
		{
			wsul nNewSize;
			wsfb *bTemp;
			nNewSize = m_wPack->GetSampSize(p);
			bTemp = (wsfb*)malloc(wO.nSize-nNewSize);
			wO.nSize -= nNewSize;
			wO.bSampData = (wsfb*)realloc(wO.bSampData,wO.nSize);
			memcpy(bTemp,wO.bSampData,wO.nSize);
			free(wO.bSampData);
			wO.bSampData = bTemp;

			wsfb b2;
			float a2;
			wsul p2;

			nTimeSearch++;

#ifdef WINWSF
			g_mStatus->SetSubProgressSteps((2+m_wPack->GetNumSamples())*(wLD->nSamples+nTimeSearch),0);
#endif

			nID = m_wPack->SampleExist(&wO,&b2,&p2,&a2);

			// TODO: a == 100 eh?
			if (nID == WSFERR || b2 != WSFS_NORMAL || a2 != 100.0f)
			{ 
				nID = m_wPack->AddSample(&wO,(wsfb*)cSampSig);
				m_wPack->SetSampleName(nID,wS->cName);
			}

			nExtras++;
			bExtras = (wsul*)realloc(bExtras,sizeof(wsul)*nExtras);
			bExtras[nExtras-1] = p;

#ifdef WINWSF
			g_mStatus->SetSubProgressSteps((2+m_wPack->GetNumSamples())*(wLD->nSamples+nTimeSearch),0);
#endif
			
			// Increase the count of each type of sample.
			nSCount[b] += 1;
		}
		else	// Increase the count of each type of sample.
			nSCount[b] += 1;

		ba = (wsfb)a;

		nIDx[i] = nID;
		bFlag[i] = b;
		bAmp[i] = ba;
		nPar[i] = p;

		free(wO.bSampData);
	}

	MAPP(&nExtras,1,sizeof(wsul));
	for (i=0;i<nExtras;i++)
		MAPP(&bExtras[i],1,sizeof(wsul));

	free(bExtras);

	// Write Sub Diff
	MAPP(&nSub,1,sizeof(unsigned long));

	for (i=0;i<wLD->nSamples;i++)
	{
		unsigned long *nS;

		// Pick Sample
		nS = &wLD->nSampleSizes[i];

		// Write Sample Signature
		MAPP(&nIDx[i],1,sizeof(unsigned long));
		// Write Sample Size
		MAPP(nS,1,sizeof(unsigned long));
		// Write Sample Offset!
		MAPP(&gD.nSampOffs[i],1,sizeof(wsul));

		// Write Flag, Parameter, and Ampage
		MAPP(&bFlag[i],1,1);
		MAPP(&nPar[i],1,4);
		MAPP(&bAmp[i],1,1);
	}

	free(bFlag);
	free(nPar);
	free(bAmp);
	free(nIDx);

	FreeGSPData(&gD);
	free(bDat);

	// Save the size of output
	MAPP(&sH,1,sizeof(unsigned short));

	MRESET(&wOut->bModData,&wOut->nSize);

	if (nSCounts)
		memcpy(nSCounts,&nSCount,sizeof(long)*(WSFS_COUNT));

	return 0;
}

#endif
