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
/// \file WSFPack.cpp Main PACK functions

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "WSFGlob.h"
#include "WSFMod.h"
#include "WSFComp.h"

#ifndef strlwr
#include <ctype.h>
extern char *strlwr( char *str );
#endif

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

/*
0 - ???
1 - Sample (NO OGG Compression)
2 - Sample (OGG Compression)
3 - No Sample, skip to next sample and read next command (That's Odd, it's like using WSF_MAGICSKIPNUM)
236 - EOF
*/

#define WSFB_UNK		0	
#define WSFB_SAMP		1
#define WSFB_SAMPOGG	2
#define WSFB_SKIP		3
#define WSFB_EOF		236

 // Test

/*

wsfb	*gp_bS;	// Byte Start
wsfb	*gp_bP;	// Byte Pos
unsigned long gp_nB;	// Size of bytes
unsigned long gp_nW;

#define POPEN(x,y)		{ gp_bS = x; gp_bP = gp_bS; gp_nB = y; gp_nW = 0; }
#define PREAD(x,y,z)	{ memcpy(x,gp_bP,y*z); gp_bP+=y*z; }
#define PSAVE(x,y,z)	{ memcpy(gp_bP,x,y*z); gp_bP+=y*z; gp_nW+=y*z; }
#define PSEEK(y)		{ gp_bP+=y; }
#define PBACK(y)		{ gp_bP-=y; }
#define PBEGIN(y)		{ gp_bP=gp_bS+y; }
#define PEND(y)			{ gp_bP=gp_bS+gp_nB-y; }
#define PAPP(x,y,z)		{ gp_nB += y*z; gp_bS = (wsfb*)realloc(gp_bS,gp_nB); gp_bP = gp_bS + gp_nW; memcpy(gp_bP,x,y*z); gp_nW += y*z; gp_bP = gp_bS + gp_nW; }
#define PRESET(x,y)		{ *x = gp_bS; *y = gp_nB; }

wsfb	*gm_bS;	// Byte Start
wsfb	*gm_bP;	// Byte Pos
unsigned long gm_nB;	// Size of bytes
unsigned long gm_nW;

#define MOPEN(x,y)		{ gm_bS = x; gm_bP = gm_bS; gm_nB = y; gm_nW = 0; }
#define MREAD(x,y,z)	{ memcpy(x,gm_bP,y*z); gm_bP+=y*z; }
#define MSAVE(x,y,z)	{ memcpy(gm_bP,x,y*z); gm_bP+=y*z; gm_nW+=y*z; }
#define MSEEK(y)		{ gm_bP+=y; }
#define MBACK(y)		{ gm_bP-=y; }
#define MBEGIN(y)		{ gm_bP=gm_bS+y; }
#define MEND(y)			{ gm_bP=gm_bS+gm_nB-y; }
#define MAPP(x,y,z)		{ gm_nB += y*z; gm_bS = (wsfb*)realloc(gm_bS,gm_nB); gm_bP = gm_bS + gm_nW; memcpy(gm_bP,x,y*z); gm_nW += y*z; gm_bP = gm_bS + gm_nW; }
#define MRESET(x,y)		{ *x = gm_bS; *y = gm_nB; }

*/

//////////////////////////////////////////////////////
//  WriteSample()
// 
//  Description: 
/// Writes sample into bPos and increases it
/// 
/// 
/// \author Josh Whelchel
/// \date 8/3/2003 3:58:09 PM
/// \param **bPos Where to write the sample (pointer to a pointer)
/// \param nBit Bits
/// \param nWrite What to Write
//////////////////////////////////////////////////////

void WriteSample( wsfb **bPos, wsus nBit, int nWrite )
{
	if (nBit == 8){
		((char*)bPos[0])[0] = (char)nWrite;
		bPos[0] += 1;
	}
	else
	{
		((short*)bPos[0])[0] = (short)nWrite;
		bPos[0] += 2;
	}
	
	return;
}

//////////////////////////////////////////////////////
//  WriteSampleR()
// 
//  Description: 
/// Writes sample into bPos and decreases it
/// 
/// 
/// \author Josh Whelchel
/// \date 8/3/2003 3:58:09 PM
/// \param **bPos Where to write the sample (pointer to a pointer)
/// \param nBit Bits
/// \param nWrite What to Write
//////////////////////////////////////////////////////

void WriteSampleR( wsfb **bPos, wsus nBit, int nWrite )
{
	if (nBit == 8){
		((char*)bPos[0])[0] = (char)nWrite;
		bPos[0] -= 1;
	}
	else
	{
		((short*)bPos[0])[0] = (short)nWrite;
		bPos[0] -= 2;
	}
	
	return;
}

//////////////////////////////////////////////////////
//  ReadSample()
// 
//  Description: 
/// Reads a sample from bPos and increases it
/// 
/// 
/// \author Josh Whelchel
/// \date 8/3/2003 3:58:06 PM
/// \param **bPos Where to read the sample (pointer to a pointer)
/// \param nBit Bits
/// \return sample
/// 
//////////////////////////////////////////////////////

int ReadSample( wsfb **bPos, wsus nBit )
{
	int nRet;

	if (nBit == 8){
		nRet = ((char*)bPos[0])[0];
		bPos[0] += 1;
	}
	else
	{
		nRet = ((short*)bPos[0])[0];
		bPos[0] += 2;
	}
	
	return nRet;
}

//////////////////////////////////////////////////////
//  ReadSampleR()
// 
//  Description: 
/// Reads Sample Reversed
/// 
/// 
/// \author Josh Whelchel
/// \date 8/3/2003 4:13:24 PM
/// \param **bPos Where to read the sample (pointer to a pointer)
/// \param nBit Bits
/// \return sample
/// 
//////////////////////////////////////////////////////

int ReadSampleR( wsfb **bPos, wsus nBit )
{
	int nRet;

	if (nBit == 8){
		nRet = ((char*)bPos[0])[0];
		bPos[0] -= 1;
	}
	else
	{
		nRet = ((short*)bPos[0])[0];
		bPos[0] -= 2;
	}
	
	return nRet;
}

//////////////////////////////////////////////////////
// PushIntoSampArray()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 4:46:12 PM
/// \param *wS Incoming Sample
/// \param wSamps Sample Array
/// \brief Pushes sample into sample array
///
//
//////////////////////////////////////////////////////

wsul PushIntoSampArray( WSF2Samp *wS, WSF2Samps wSamps )
{
	wsul i;
	
	i = wSamps->nSamps;
	wSamps->nSamps++;

	wSamps->bSamps = (WSF2Samp*)realloc(wSamps->bSamps,sizeof(WSF2Samp)*wSamps->nSamps);
	
	memcpy(&wSamps->bSamps[i],wS,sizeof(WSF2Samp));

	return i;
}

//////////////////////////////////////////////////////
// PushIntoSArray()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 4:59:24 PM
/// \param nID ID of sample in WSP (CWSFPack)
/// \param nSize Size of sample in bytes
/// \param bFlag Sample reproduction flag
/// \param nPar1 Parameter 1
/// \param nPar2 Parameter 2
/// \param fAmp Amplification
/// \param wSamps Sample Array
/// \brief Pushes sample into sample array
///
//
//////////////////////////////////////////////////////

wsul PushIntoSArray( wsul nID,
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

//***************************************************************************** */
// CWSFPack::CWSFPack()
// 
// Purpose: Constructor
// 
// Author : [Josh Whelchel], Created : [2/12/2003 10:39:33 AM]
// Parameters:	
// 
// 
// Returns:	int
// 
//***************************************************************************** */

CWSFPack::CWSFPack()
{
	// Set Not Loaded
	m_nLoaded = 0;
	// Allocate Sample Buffers to 0
	m_wSamples = (wsf_sample*)malloc(0);
	m_nSamples = 0;
	m_nPassSum = 0;
	m_nNeedPass = 0;
}

//***************************************************************************** */
// CWSFPack::~CWSFPack()
// 
// Purpose: Deconstrer
// 
// Author : [Josh Whelchel], Created : [2/12/2003 10:40:45 AM]
// Parameters:	
// 
// 
// Returns:	none
// 
//***************************************************************************** */

CWSFPack::~CWSFPack()
{
	// Loaded?
	if (m_nLoaded)
		FreePack();	// Frees the samples and other allocations

	// Free sample buffer
	free(m_wSamples);
}

//***************************************************************************** */
// CWSFPack::DeCompressSample()
// 
// Purpose: Decompresses Sample
// 
// Author : [Josh Whelchel], Created : [2/16/2003 4:03:02 PM]
// Parameters:	
// 
// [*bCompSamp] :
// [nCompSize] :
// [*bOutSamp] :
// [nFullSize] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFPack::DeCompressSample( wsfb *bCompSamp, unsigned long nCompSize, wsfb **bOutSamp, unsigned long nFullSize )
{
	return DoCompression(bCompSamp,nCompSize,nFullSize,bOutSamp,NULL,false);
}

//***************************************************************************** */
// CWSFPack::LoadPack()
// 
// Purpose: Loads the pack
// 
// Author : [Josh Whelchel], Created : [2/12/2003 10:42:10 AM]
// Parameters:	
// 
// [*cFile] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFPack::LoadPack( char *cFile, wsul *nIDs, wsul nNumIDs, int nNoBulk )
{
	wsf_file *PFile;
	int nR;
	// Open File
	PFile = wsfopenfile(cFile);
	// Error?
	if (!PFile)
		return 1;	// Return :P

//	wsfbegin(0,PFile);
	nR = LoadPack(PFile,nIDs,nNumIDs,nNoBulk);
	wsfclose(PFile);
	return nR;
}

int CWSFPack::LoadPack( wsfb *bData, wsul nSize, wsul *nIDs, wsul nNumIDs, int nNoBulk )
{
	wsf_file *PFile;
	int nR;
	// Open File
	PFile = wsfopenmem(bData,nSize);
	// Error?
	if (!PFile)
		return 1;	// Return :P

//	wsfbegin(0,PFile);
	nR = LoadPack(PFile,nIDs,nNumIDs,nNoBulk);
	wsfclose(PFile);
	return nR;
}

int CWSFPack::LoadPackV2( wsf_file *PFile, wsul *nIDs, wsul nNumIDs, int nNoBulk, wsfb bComp )
{
	wsfb bHi, bLo;
	wsfb bC;
	wsul nCur;
	wsul nUTSize;
	wsf_sample *wSamp;

	bHi = m_bHi;
	bLo = m_bLo;

	nCur = 0;

	while (1)
	{
		wsul nF;
		nF = 0;
		nUTSize = 0;

		wSamp = &m_wSamples[nCur];	// Pick current sample

		// Read Command Byte
		wsfread(&bC,1,PFile);

		if (bC == WSFB_EOF || nCur >= m_nSamples)
			break;

		memset(wSamp,0,sizeof(wsf_sample));

		if (bC == WSFB_SKIP){
			nCur++;
			continue;
		}

		// Test if we need to load it
		if (nIDs != NULL)
		{
			wsul x;
			for (x=0;x<nNumIDs;x++)
			{
				if (nIDs[x]==nCur){
					nF = 1;		// Mark load flag to TRUE
					break;
				}
			}
		}
		else if (!nNoBulk)
			nF = 1;		// Since there is no array we're loading them all!

		if (bC == WSFB_SAMP || bC == WSFB_SAMPOGG)
		{
			// Load Sample
			// Read other stuff
		
			// Convert to byte 
			wsfb bTemp;

			// Sample Signature
			wSamp->cSignature[0] = 0;

			// Load Various Infos
			wsfread(&bTemp,1,PFile);
			wSamp->nBit = bTemp;

			wsfread(&bTemp,1,PFile);
			wSamp->nCh = bTemp;	
			
			wsfread(&wSamp->nFreq,sizeof(wsul),PFile);
			
			wsfread(wSamp->cName,26,PFile);
			wSamp->cName[26]=0;

			// Ogg Compression Test?
			if (bC == WSFB_SAMPOGG){
				wsfread(&nUTSize,sizeof(wsul),PFile);
				wSamp->nOgg = 1;
			} else
				wSamp->nOgg = 0;
			
			if (bComp == 0)
			{
				// Uncompressed sampledata

				// Read Sample Size
				wsfread(&wSamp->nSize,sizeof(unsigned long),PFile);
				wSamp->nRawSize = wSamp->nSize;
				wSamp->nStoredSize = wSamp->nSize;

				if (nF)
				{
					// Allocate memory for sample
					wSamp->bSampData = (wsfb*)malloc(wSamp->nSize);

					// Read Sample
					wsfread(wSamp->bSampData,wSamp->nSize,PFile);

					if (m_nNeedPass && m_nPassSum)
						if (TESTVER(2,03))
							DecodeData2(wSamp->bSampData,m_nPassSum,wSamp->nSize);
						else
							DecodeData(wSamp->bSampData,m_nPassSum,wSamp->nSize);
				}
				else{
					wSamp->nSize = 0;
//					fseek(PFile,wSamp->nRawSize,SEEK_CUR);
					wsfseek(wSamp->nRawSize,PFile);
				}
			}
			else
			{
				// Compressed Sampledata ;_;
				unsigned long nFullSize;	// Full Size
				unsigned long nCompSize;	// Size when Compressed
				wsfb *bCompData;			// Compressed DAta

				// Sample Size
				wsfread(&nCompSize,sizeof(unsigned long),PFile);
				wSamp->nStoredSize = nCompSize;

				if (nF)
				{
					// Make memory and compressed sample data
					bCompData = (wsfb*)malloc(nCompSize);
					wsfread(bCompData,nCompSize,PFile);

					// Sample UnComp Size
					wsfread(&nFullSize,sizeof(unsigned long),PFile);
					wSamp->nSize = nFullSize;
					wSamp->nRawSize = nFullSize;

					DeCompressSample(bCompData,nCompSize,&wSamp->bSampData,nFullSize);

					if (m_nNeedPass && m_nPassSum)
						if (TESTVER(2,03))
							DecodeData2(wSamp->bSampData,m_nPassSum,wSamp->nSize);
						else
							DecodeData(wSamp->bSampData,m_nPassSum,wSamp->nSize);

					free(bCompData);
				}
				else{
//					fseek(PFile,nCompSize,SEEK_CUR);
					wsfseek(nCompSize,PFile);
					wsfread(&wSamp->nRawSize,sizeof(wsul),PFile);
					wSamp->nSize = wSamp->nRawSize;
				}
			}

			if (wSamp->nOgg && nF)
			{
				wsf_file *wOut;
				wOut = wsfcreatemem();
				OggDecode(wSamp->nSize,wSamp->bSampData,wSamp->nBit,wOut);

				free(wSamp->bSampData);
				wSamp->bSampData = wOut->bData;
				wSamp->nSize = nUTSize;

				/// \todo Error checking, like if new sample is bigger than last time :3

				// Fill remaining information with NULL, why not?
				if (wOut->nSize < nUTSize)
				{
					wSamp->bSampData = (wsfb*)realloc(wSamp->bSampData,nUTSize);
					memset(wSamp->bSampData+wOut->nSize,0,nUTSize-wOut->nSize);
				}
			}

			nCur++;
		}
	}

	// Its Loaded
	m_nLoaded = 1;

	return 0;
}

int CWSFPack::LoadPackV1( wsf_file *PFile, wsul *nIDs, wsul nNumIDs, int nNoBulk, wsfb bComp )
{
	wsul i;
	wsfb bHi, bLo;

	bHi = m_bHi;
	bLo = m_bLo;

	// Load Samples!
	for (i=0;i<m_nSamples;i++)
	{
		int nF;
		wsul nUTSize;
		wsf_sample *wSamp;
		wSamp = &m_wSamples[i];	// Pick current sample

		nF = 0;	// Reset Load FLAG
		memset(wSamp,0,sizeof(wsf_sample));

		// Test if we need to load it
		if (nIDs != NULL)
		{
			wsul x;
			for (x=0;x<nNumIDs;x++)
			{
				if (nIDs[x]==i){
					nF = 1;		// Mark load flag to TRUE
					break;
				}
			}
		}
		else if (!nNoBulk)
			nF = 1;		// Since there is no array we're loading them all!

		if (TESTVER(1,28)){
			wsfread(&wSamp->nOgg,1,PFile);

			if (TESTVER(1,29))
			{
				if (wSamp->nOgg == WSP_MAGICSKIPNUM)
				{
					wSamp->nOgg = 0;
					continue;
				}
			}

			wsfread(&nUTSize,sizeof(wsul),PFile);
		}
		else
		{
			wSamp->nOgg = 0;
			nUTSize = 0;
		}

		if (bComp == 0)
		{
			// Uncompressed sampledata

			// Read Sample Signature
			wsfread(wSamp->cSignature,30,PFile);
			wSamp->cSignature[30] = 0;

			// Read Sample Size
			wsfread(&wSamp->nSize,sizeof(unsigned long),PFile);
			wSamp->nRawSize = wSamp->nSize;
			wSamp->nStoredSize = wSamp->nSize;

			if (nF)
			{
				// Allocate memory for sample
				wSamp->bSampData = (wsfb*)malloc(wSamp->nSize);

				// Read Sample
				wsfread(wSamp->bSampData,wSamp->nSize,PFile);

				if (m_nNeedPass && m_nPassSum)
					DecodeData(wSamp->bSampData,m_nPassSum,wSamp->nSize);
			}
			else{
				wSamp->nSize = 0;
//				fseek(PFile,wSamp->nRawSize,SEEK_CUR);
				wsfseek(wSamp->nRawSize,PFile);
			}
		}
		else
		{
			// Compressed Sampledata ;_;
			unsigned long nFullSize;	// Full Size
			unsigned long nCompSize;	// Size when Compressed
			wsfb *bCompData;			// Compressed DAta

			// Sample Signature
			wsfread(wSamp->cSignature,30,PFile);
			wSamp->cSignature[30] = 0;

			// Sample Size
			wsfread(&nCompSize,sizeof(unsigned long),PFile);
			wSamp->nStoredSize = nCompSize;

			if (nF)
			{
				// Make memory and compressed sample data
				bCompData = (wsfb*)malloc(nCompSize);
				wsfread(bCompData,nCompSize,PFile);

				// Sample UnComp Size
				wsfread(&nFullSize,sizeof(unsigned long),PFile);
				wSamp->nSize = nFullSize;
				wSamp->nRawSize = nFullSize;

				DeCompressSample(bCompData,nCompSize,&wSamp->bSampData,nFullSize);

				if (m_nNeedPass && m_nPassSum)
					DecodeData(wSamp->bSampData,m_nPassSum,wSamp->nSize);

				free(bCompData);
			}
			else{
//				fseek(PFile,nCompSize,SEEK_CUR);
				wsfseek(nCompSize,PFile);
				wsfread(&wSamp->nRawSize,sizeof(wsul),PFile);
				wSamp->nSize = wSamp->nRawSize;
			}
		}

		// Read other stuff
		if (TESTVER(1,26))
		{
			// Convert to byte
			wsfb bTemp;

			wsfread(&bTemp,1,PFile);
			wSamp->nBit = bTemp;

			wsfread(&bTemp,1,PFile);
			wSamp->nCh = bTemp;	
			
			if (TESTVER(1,36))
			{
				wsfread(&wSamp->nFreq,sizeof(wsul),PFile);
			}
		}

		if (TESTVER(1,28))
			wsfread(wSamp->cName,26,PFile);
		else
			memset(wSamp->cName,0,27);
		wSamp->cName[26]=0;

		if (wSamp->nOgg && nF)
		{
			wsf_file *wOut;
			wOut = wsfcreatemem();
			OggDecode(wSamp->nSize,wSamp->bSampData,wSamp->nBit,wOut);

			free(wSamp->bSampData);
			wSamp->bSampData = wOut->bData;
			wSamp->nSize = nUTSize;

			/// \todo Error checking, like if new sample is bigger than last time :3

			// Fill remaining information with NULL, why not?
			if (wOut->nSize < nUTSize)
			{
				wSamp->bSampData = (wsfb*)realloc(wSamp->bSampData,nUTSize);
				memset(wSamp->bSampData+wOut->nSize,0,nUTSize-wOut->nSize);
			}
		}

		// Onwards!
	}

	// Its Loaded
	m_nLoaded = 1;

	return 0;
}

int CWSFPack::LoadPack( wsf_file *PFile, wsul *nIDs, wsul nNumIDs, int nNoBulk, int nNoBegin )
{
	char cHead[4];
	wsfb bHi, bLo;
	wsfb bComp;

	if (!PFile)
		return 1;

	if (!nNoBegin)
		wsfbegin(0,PFile);

	m_bHi = 0;
	m_bLo = 0;

	// Get Header
	wsfread(cHead,4,PFile);

	// Match Header
	if (strncmp(WSFPHEAD,cHead,4)!=0){
		// Error No Match
		wsfclose(PFile);
		return 1;
	}

	// Get Version
	wsfread(&bHi,1,PFile);
	wsfread(&bLo,1,PFile);
	m_bHi = bHi;
	m_bLo = bLo;

	if (bHi > WSFPVERHI || (bHi == WSFPVERHI && bLo > WSFPVERLO))
		return 1; // INCORRECT VERSION, NOT SUPPORTED :<

	if (TESTVER(1,23))
		wsfread(&m_nNeedPass,1,PFile);
	else
		m_nNeedPass = 0;

	// Get Compressed?
	wsfread(&bComp,1,PFile);	// 

	// Get Number of Samples
	wsfread(&m_nSamples,sizeof(unsigned long),PFile);

	// Reallocate for number of samples
	m_wSamples = (wsf_sample*)realloc(m_wSamples,sizeof(wsf_sample)*m_nSamples);

	// Load Version 1
	if (bHi < 2)
		return LoadPackV1(PFile,nIDs,nNumIDs,nNoBulk,bComp);
	else
		return LoadPackV2(PFile,nIDs,nNumIDs,nNoBulk,bComp);
	
	return 1;
}

#ifndef WSFNOSAVE

//***************************************************************************** */
// CWSFPack::MakeOldSignature()
// 
// Purpose: Creates Sample Signature based on 'OLD'
// 
// Author : [Josh Whelchel], Created : [2/17/2003 8:28:38 PM]
// Parameters:	
// 
// [*wSamp] :
// [nSize] :
// [*cOut] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFPack::MakeOldSignature( wsfb *wSamp, wsul nSize, char *cOut )
{
	unsigned long i=0;
	unsigned long nSegLen;
	wsfb *bPos;
	char *cPos;

	// Fill with Magic Number
	memset(cOut,0,SIGNATURESIZE);

	// Get Segment Length
	nSegLen = 1;

	cPos = cOut + SIGNATURESIZE - 30;
	bPos = wSamp;

	cPos[0] = (char)255;
	while (bPos < (wSamp+nSize))
	{
		i++;
		cPos[0] ^= bPos[0];
		bPos++;

		if (i >= nSegLen){
			// Reset
			i=0;
			
			cPos++;
			cPos[0] = (char)255;
		}

		if (cPos == &cOut[30])
		{
			// Done
			break;
		}
		
	}

	cPos[0] = 0;
	
	return 0;
}

//***************************************************************************** */
// CWSFPack::CreateSignature()
// 
// Purpose: Creates Signature
// 
// Author : [Josh Whelchel], Created : [2/12/2003 11:01:09 AM]
// Parameters:	
// 
// [*wSamp] :
// [nSize] :
// [*cOut] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFPack::CreateSignature( wsfb *wSamp, wsul nSize, wsus nCh, wsus nBit, wsul nSamps, char *cOut )
{
	unsigned long nSegLen;		// Size of each segment to scan in order to equal SIGNATURESIZE for cOut
	unsigned long i;
	wsfb *bPos;
	char *cPos;
	wsul nSub;
	wsfb bRel;
	wsus nHi;
	wsus nLo;
	wsul nSampsToTest;
	int nSeg;

	if (nSamps <= 30 || nBit == 0)
		return MakeOldSignature( wSamp, nSize, cOut );

	// Get number of samples to test
	nSampsToTest = (wsul)(((double)nSamps)*0.50);

	if (nSampsToTest < 32)
		nSampsToTest = nSamps;

	// Get Segment Length
	nSegLen = (nSampsToTest / SIGNATURESIZE) - 1;
	// Setup Start Position
	bPos = wSamp;
	cPos = cOut;
	memset(cOut,0,30);
	i = 0;
	nSeg=0;

	if (nBit==16)
		nSub = ((wsus*)wSamp)[0];
	else
		nSub = wSamp[0];

	// Set Position
	if (nCh == 2){
		// STEREO
		if (nBit == 16)
			bPos += 4*(nSamps-nSampsToTest);
		else
			bPos += 2*(nSamps-nSampsToTest);
	}
	else
	{
		// MONO
		if (nBit == 16)
			bPos += 2*(nSamps-nSampsToTest);
		else
			bPos += 1*(nSamps-nSampsToTest);
	}


	nHi = 0;
	// Get Highest Point
	for (i=nSamps-nSampsToTest;i<nSamps;i+=nCh)
	{
		if (nBit == 16)	// 16 bit
		{
			wsus nVal;
			nVal = ((wsus*)wSamp)[i];

			if (nVal >= nHi)
				nHi = nVal;

			if (nVal <= nLo)
				nLo = nVal;
		}		
		else // 8 bit
		{
			wsfb bVal;
			bVal = wSamp[i];

			if (bVal >= nHi)
				nHi = bVal;
			if (bVal <= nLo)
				nLo = bVal;
		}
	}

	cPos[0] = (char)255;
	bRel = 0;
	while (bPos < (wSamp+nSize))
	{
		// Deal with the sample
		if (nBit == 16)
		{
			// 16 bit
			wsus sSample;

			sSample = ((wsus*)bPos)[0];

			if (sSample == nHi || sSample == nLo)
				bRel = WSFMAGICNUMBER;	// Its at an extreme, don't test :/
			else
			{
				bRel = (wsfb)MakePercent(abs(sSample-nSub),nHi);
				nSub = sSample;
			}

			if (nCh==2)
				bPos += 4;
			else
				bPos += 2;
		}
		else
		{
			// 8 bit
			wsfb bSample;

			bSample = bPos[0];

			if (bSample == nHi || bSample == nLo)
				bRel = WSFMAGICNUMBER;  // Its at an extreme, DONT TEST :O
			else
			{
				bRel = (wsfb)MakePercent(abs(bSample-nSub),nHi);
				nSub = bSample;
			}

			if (nCh==2)
				bPos += 2;
			else
				bPos += 1;
		}

		i++;
		cPos[0] = bRel^cPos[0];

		if (i >= nSegLen){
			// Reset
			i=0;
			
			cPos++;
			nSeg++;
			cPos[0] = (char)255;
		}

		if (cPos == &cOut[30])
		{
			// Done
			break;
		}
		
	}

	// End Character
	cPos[0] = 0;
	
	return 0;
}

//***************************************************************************** */
// CWSFPack::CompressSample()
// 
// Purpose: Compresses Sample
// 
// Author : [Josh Whelchel], Created : [2/16/2003 4:02:32 PM]
// Parameters:	
// 
// [*bFullSamp] :
// [nFullSize] :
// [**bOutSamp] :
// [*nCompSize] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFPack::CompressSample( wsfb *bFullSamp, unsigned long nFullSize, wsfb **bOutSamp, unsigned long *nCompSize )
{
	return DoCompression(bFullSamp,nFullSize,0,bOutSamp,nCompSize,true);
}

//***************************************************************************** */
// CWSFPack::SampleExist()
// 
// Purpose: Determines if a sample exists
// 
// Author : [Josh Whelchel], Created : [2/15/2003 5:49:12 PM]
// Parameters:	
// 
// [*cSampSig] :
// 
// Returns:	int
// 
//***************************************************************************** */

unsigned long CWSFPack::SampleExist( char *cSampSig, int nCh, int nBit )
{
	unsigned long i;

	// Search!
	for (i=0;i<m_nSamples;i++)
	{
		wsf_sample *wS;
		wS = &m_wSamples[i];		// Get Sample
		// Match?
		if (memcmp(cSampSig,wS->cSignature,SIGNATURESIZE)==0){
			// Keep :D

			if (nCh > -1 && wS->nCh != (unsigned)nCh)
				continue;

			if (nBit > -1 && wS->nBit != (unsigned)nBit)
				continue;

			return i;
		}
		// Keep Going :<
	}

	return WSFERR;
}

//////////////////////////////////////////////////////
//  CWSFPack::SamplesMatch()
// 
//  Description: 
/// Tests two samples to see if they match in some way, and returns the WSFFlag flag if they do
/// 
/// 
/// \author Josh Whelchel
/// \date 8/3/2003 3:39:54 PM
/// \param *wSampA Sample One
/// \param *wSampB Sample Two 
/// \param *bFlag Flag Output
/// \param *nPar Parameter Output
/// \param *nAmp Amplification Output (0-255, 100 being normal)
/// \param nID ID of Second Sample
/// \param nParB Parameter B Output; If you fill this buffer with 1, it will skip WSFS_SAMPAFTER testing
/// \param wSm Sample Array to push new stuff into!
/// \return 0 if Match, nonzero if no match
/// 
//
//////////////////////////////////////////////////////

wsul CWSFPack::SamplesMatch( wsf_sampout *wSampA, wsf_sample *wSampB, wsfb *bFlag, wsul *nPar, float *nAmp, wsul nID, wsul *nParB, WSF2Samps wSm )
{
#define SMEND(x,y,z,e) { nRet = x; bRFlag = y; nRPar = z; nRParB = e; goto smend; }
	wsfb *bPosA, *bPosB;
//	wsfb *bWPosA, *bWPosB;
	wsfb *bSampA, *bSampB;
	wsul nSampA, nSampB;
	int nLoudA, nLoudB;
	wsus nCh;
	wsfb bRFlag;
	wsul nRPar;
	wsul nRParB;
	wsul nRet;
	double dAmp;
	double mA,mB;

	wsus nBitA;
	wsus nBitB;

	// Generic 
	nBitA = wSampA->nBit;
	nBitB = wSampB->nBit;
	nCh = wSampA->nCh;

	if (!wSampA->nSize || !wSampB->nSize)
		return 1;

	// If we have stereo tracks just do a straight test
	if (nCh == 2)
	{
		if (wSampA->nSize != wSampB->nSize)
			return 1;

		if (memcmp(wSampB->bSampData,wSampA->bSampData,wSampA->nSize)==0)
		{
			bFlag[0] = WSFS_NORMAL;
			nPar[0] = 0;
			nAmp[0] = 100;
			if (nParB)
				nParB[0] = 0;
			return 0;
		}
		else
			return 1;
	}

	nRet = 1;

	// Sample A
	bSampA = wSampA->bSampData;
	nSampA = wSampA->nSize;
	
	// Sample B
	bSampB = wSampB->bSampData;
	nSampB = wSampB->nRawSize;

	// Test the two

	// Fill the buffers with their full volume
	
	// Determine the loudest volume (buffer A)
	bPosA = bSampA;
	nLoudA = 0;

	while (bPosA < bSampA+nSampA)
	{
		int nS;
		nS = ReadSample(&bPosA,nBitA);

		if (abs(nS) > abs(nLoudA))
			nLoudA = abs(nS);
	}

	// Determine the loudest volume (buffer B)
	bPosB = bSampB;
	nLoudB = 0;

	while (bPosB < bSampB+nSampB)
	{
		int nS;
		nS = ReadSample(&bPosB,nBitB);

		if (abs(nS) > abs(nLoudB))
			nLoudB = abs(nS);
	}

#define COMPCHECK ((mA != mB))

	if (nLoudB)
		dAmp = nLoudA / nLoudB;
	else
		dAmp = 1.0f;

	// Compare the two sample buffers

	if (nSampA <= nSampB)
	{
		// Test the normal sample

		bPosA = bSampA;
		bPosB = bSampB;

		nRet = 0;
		while (bPosA < bSampA+nSampA)
		{
			double nA,nB;
			nA = ReadSample(&bPosA,nBitA);
			nB = ReadSample(&bPosB,nBitB);

			mA = (int)(nA/nLoudA);
			mB = (int)(nB/nLoudB);

//			if (nA != nLoudA && nB != nLoudB && ((double)nA*dAmp) != ((double)nB)) 
//			if ( (nSampA < 200 || (nA != nLoudA && nB != nLoudB)) && ((double)nA*dAmp) != ((double)nB)) 
//			if ((double)nA != (double)nB*dAmp && (nB != nLoudB) && (nA != nLoudA) )		// < WORKS
			if (COMPCHECK)
			{
				nRet = 1;
				break;
			}
		}

		if (nRet == 0 && nSampA == nSampB)
			SMEND(0,WSFS_NORMAL,0,0);

		if (nRet == 0 && nSampA < nSampB)
			SMEND(0,WSFS_SAMPCUTOFF,nSampA,0);
	}

	if (nSampA <= nSampB)
	{
		// Test the reversed sample

		bPosA = bSampA;
		bPosB = bSampB+nSampB-1-(nSampB-nSampA);

		nRet = 0;
		while (bPosA < bSampA+nSampA)
		{
			double nA,nB;
			nA = ReadSample(&bPosA,nBitA);
			nB = ReadSampleR(&bPosB,nBitB);

			mA = (int)(nA/nLoudA);
			mB = (int)(nB/nLoudB);

			if (COMPCHECK)		// < WORKS
			{
				nRet = 1;
				break;
			}
		}

		if (nRet == 0 && nSampA == nSampB)
			SMEND(0,WSFS_REVERSED,0,0);

		if (nRet == 0 && nSampA < nSampB)
			SMEND(0,WSFS_REVSAMPCUTOFF,nSampA,0);
	}

	// We can say that when nParB is NULL that we aren't looking for a full sample.
	// However, this doesn't REALLY matter anymore, because of the sample array's
	// implemented in V2.x of CWSFMod. Funny how we control that here in CWSFPack.
	if (!nParB)
		goto smend;

	// We can safely say this part sucks.
#if 0
	if (nSampA > nSampB && wSm->nCurID != nID && nParB[0] != 1)
	{
		// Test the appended

		bPosA = bSampA + (nSampA-nSampB);

		bPosB = bSampB;
		nLoudA = 0;

		// We need the normaliziation for only the tested part of data.
		while (bPosA < bSampA+nSampA)
		{
			int nS;
			nS = ReadSample(&bPosA,nBitA);

			if (abs(nS) > abs(nLoudA))
				nLoudA = abs(nS);
		}

		if (nLoudB)
			dAmp = nLoudA / nLoudB;
		else
			dAmp = 1.0f;

		bPosA = bSampA + (nSampA-nSampB);

		nRet = 0;
		while (bPosB < bSampB+nSampB)
		{
			double nA,nB;
			nA = ReadSample(&bPosA,nBitA);
			nB = ReadSample(&bPosB,nBitB);

			mA = (int)(nA/nLoudA);
			mB = (int)(nB/nLoudB);

//			if (nA != nLoudA && nB != nLoudB && ((double)nA*dAmp) != ((double)nB)) 
//			if ( (nSampA < 200 || (nA != nLoudA && nB != nLoudB)) && ((double)nA*dAmp) != ((double)nB)) 
			if (COMPCHECK)		// < WORKS
			{
				nRet = 1;
				break;
			}
		}

		// Here comes the crazy part
		// Make this Sample smaller and add its reference. :P
		// Then we will push it into wSm
		if (nRet == 0)
		{
			wsul nNewSize;
			wsfb *bTemp;
			nNewSize = nSampB;
			bTemp = (wsfb*)malloc(wSampA->nSize-nNewSize);
			wSampA->nSize -= nNewSize;
			wSampA->bSampData = (wsfb*)realloc(wSampA->bSampData,wSampA->nSize);
			memcpy(bTemp,wSampA->bSampData,wSampA->nSize);
			free(wSampA->bSampData);
			wSampA->bSampData = bTemp;

			wsfb b;
			wsul p1,p2;
			float a;
			wsul nOld;

#ifdef WINWSF
			if (!wSm->nNoRender)
				g_mStatus->SetSubProgressSteps((2+m_nSamples)*(wSm->nSamplesTotal),0);

			wsul nOR;
			nOR = wSm->nNoRender;
			wSm->nNoRender = 1;
#endif

			nOld = wSm->nCurID;
			p2 = 1;		// Set this to one so we don't run into it again.
			wSm->nCurID = nID;
			nRParB = SampleExist(wSampA,&b,&p1,&a,&p2,wSm);
			wSm->nCurID = nOld;

#ifdef WINWSF
			wSm->nNoRender = nOR;
#endif

			if (nRParB == WSFERR)
			{
				// Normal Sample Flag
				nRParB = AddSample(wSampA);
				/// \todo Maybe we should get the sample name of sample A? Maybe not.
				SetSampleName(nRParB,wSampB->cName);
				b = WSFS_NORMAL;
				p1 = p2 = 0;
				a = 100.0f;
			}
			else
			{	// Increase the count of each type of sample.
				if (wSm->nSCounts)
					wSm->nSCounts[b] += 1; // Increase Count :3
			}

#ifdef WINWSF
			if (!wSm->nNoRender)
				g_mStatus->SetSubProgressSteps((2+m_nSamples)*(wSm->nSamplesTotal),0);
#endif

			SMEND(0,WSFS_SAMPAFTER,PushIntoSArray(nID,WSFS_NORMAL,0,0,100.0f,wSm),
				PushIntoSArray(nRParB,b,p1,p2,a,wSm));
/*
			if (nIDMe == WSFERR)
			{ 
				nIDMe = AddSample(wSampA);
				SetSampleName(nIDMe,wSampB->cName); 
				
				// See note above
				SMEND(0,WSFS_SAMPAFTER,PushIntoSArray(nID,WSFS_NORMAL,0,0,100.0f,wSm),PushIntoSArray(nIDMe,b,p1,p2,a,wSm));	
			}
			else if (b == WSFS_SAMPAFTER)
			{
				// We use nPar2 in CWSFPack::SampleMatch() to tell us what our new ID
				// needs to be. Pretty simple.

				wsul nX;
				nX = p2;
				p2 = 0;

				if (wSm->nSCounts)
					wSm->nSCounts[b] += 1; // Increase Count :3

				SMEND(0,WSFS_SAMPAFTER,PushIntoSArray(nID,WSFS_NORMAL,0,0,100.0f,wSm),nX);	
			}
			else
			{
				// Crapola.
				// Now we need to find out where to put these other parameters.
				// Another Reference

				// See note above.
//				nIDMe = PushIntoSArray(nID,b,p1,p2,a,wSm);
				if (wSm->nSCounts)
					wSm->nSCounts[b] += 1; // Increase Count :3

				SMEND(0,WSFS_SAMPAFTER,PushIntoSArray(nID,WSFS_NORMAL,0,0,100.0f,wSm),PushIntoSArray(nIDMe,b,p1,p2,a,wSm));
			}
*/
		}
	}
#endif

smend:

	if (nRet==0)
	{

		bFlag[0] = bRFlag;
		nPar[0] = nRPar;

		if (nParB)
			nParB[0] = nRParB;

		nAmp[0] = (float)nLoudA/(float)nLoudB;
	}

	return nRet;
}

//////////////////////////////////////////////////////
//  CWSFPack::SampleExist()
// 
//  Description: 
/// Searches all the samples to see if another matches
/// 
/// 
/// \author Josh Whelchel
/// \date 8/3/2003 3:34:06 PM
/// \param *wInSamp Sample to look for
/// \param *bFlag Outputs the modification flag (WSFFlag enum)
/// \param *nPar Outputs the parameter
/// \param *nAmp Outputs the amplification
/// \return 
/// 
//////////////////////////////////////////////////////

wsul CWSFPack::SampleExist( wsf_sampout *wInSamp, wsfb *bFlag, wsul *nPar, float *nAmp, wsul *nParB, WSF2Samps wSm )
{
	wsul i;

	if (wInSamp->nSize <= 450)
		return WSFERR;

	// Search (this won't be pretty at all :o)

	// Search first for samples within a range (WSFKBDIFF)
	for (i=0;i<m_nSamples;i++)
	{
		wsf_sample *wS;
		wS = &m_wSamples[i];	// Get Sample

		// Pass Range Check?
		if ((wS->nSize > wInSamp->nSize+(1024*WSFKBDIFF)) || (wS->nSize < wInSamp->nSize-(1024*WSFKBDIFF))) 
			continue;

		if (wS->nBit != wInSamp->nBit){
#ifdef WINWSF
			if (!wSm->nNoRender)
				g_mStatus->StepSubProgress();
#endif
			continue;
		}

		/// \todo We want differing bitrates to be O.K.

		if (wS->nCh != wInSamp->nCh){
#ifdef WINWSF
			if (!wSm->nNoRender)
				g_mStatus->StepSubProgress();
#endif
			continue;
		}

		// Okay now we have to test :0
		if (SamplesMatch(wInSamp,wS,bFlag,nPar,nAmp,i,nParB,wSm)==0)
		{
#ifdef WINWSF
			if (!wSm->nNoRender)
				g_mStatus->StepSubProgress(m_nSamples-i);
#endif
			return i;	// WOOT a match!
		}

#ifdef WINWSF
		if (!wSm->nNoRender)
			g_mStatus->StepSubProgress();
#endif

	}

	// Search last for samples not in the range (WSFKBDIFF)
	for (i=0;i<m_nSamples;i++)
	{
		wsf_sample *wS;
		wS = &m_wSamples[i];	// Get Sample

		// Pass Range Check?
		if ((wS->nSize <= wInSamp->nSize+(1024*WSFKBDIFF)) && (wS->nSize >= wInSamp->nSize-(1024*WSFKBDIFF))) 
			continue;

		if (wS->nBit != wInSamp->nBit){
#ifdef WINWSF
			if (!wSm->nNoRender)
				g_mStatus->StepSubProgress();
#endif
			continue;
		}

		/// \todo We want differing bitrates to be O.K.

		if (wS->nCh != wInSamp->nCh){
#ifdef WINWSF
			if (!wSm->nNoRender)
				g_mStatus->StepSubProgress();
#endif
			continue;
		}

		// Okay now we have to test :0
		if (SamplesMatch(wInSamp,wS,bFlag,nPar,nAmp,i,nParB,wSm)==0)
		{
#ifdef WINWSF
			if (!wSm->nNoRender)
				g_mStatus->StepSubProgress(m_nSamples-i);
#endif
			return i;	// WOOT a match!
		}

#ifdef WINWSF
		if (!wSm->nNoRender)
			g_mStatus->StepSubProgress();
#endif

	}

	return WSFERR;
}

//***************************************************************************** */
// CWSFPack::AddSample()
// 
// Purpose: Adds Sample to pack
// 
// Author : [Josh Whelchel], Created : [2/12/2003 11:12:43 AM]
// Parameters:	
// 
// [*wInSamp] :
// 
// Returns:	int
// 
//***************************************************************************** */

unsigned long CWSFPack::AddSample( wsf_sampout *wInSamp, wsfb *bSig )
{
	wsf_sample *wSamp;

	// Increase number of samples
	m_nSamples++;
	// Reallocate
	m_wSamples = (wsf_sample*)realloc(m_wSamples,sizeof(wsf_sample)*m_nSamples);

	// Pick Sample
	wSamp = &m_wSamples[m_nSamples-1];

	// Copy over
	wSamp->nRawSize = wInSamp->nSize;
	wSamp->nSize = wInSamp->nSize;
	wSamp->bSampData = (wsfb*)malloc(wSamp->nSize);
	memcpy(wSamp->bSampData,wInSamp->bSampData,wSamp->nSize);
	wSamp->nBit = wInSamp->nBit;
	wSamp->nCh = wInSamp->nCh;
	wSamp->nOgg = wInSamp->nOgg;
	wSamp->nFreq = wInSamp->nFreq;
	wSamp->nStoredSize = wInSamp->nStoredSize;
	wSamp->cName[0] = 0;

	// Make Signature
	if (bSig)
	{
		memcpy(wSamp->cSignature,bSig,SIGNATURESIZE);
		wSamp->cSignature[30] = 0;
	}else
		memset(wSamp->cSignature,30,0);

	// Its Loaded. XD
	m_nLoaded = 1;

	return m_nSamples-1;
}

//***************************************************************************** */
// CWSFPack::SavePack()
// 
// Purpose: Saves Pack
// 
// Author : [Josh Whelchel], Created : [2/12/2003 10:56:57 AM]
// Parameters:	
// 
// [*cFile] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFPack::SavePack( char *cFile, wsfb bComp )
{
	int nR;
	wsf_file *PFile;

	// Open File
	PFile = wsfcreatefile(cFile);

	// Error?
	if (!PFile)
		return 1;	// Return :P

	nR = SavePack(PFile,bComp);

	wsfclose(PFile);

	return nR;
}

int CWSFPack::SavePack( wsfb **bData, wsul *nSize, wsfb bComp )
{
	int nR;
	wsf_file *PFile;

	// Open File
	PFile = wsfcreatemem();

	// Error?
	if (!PFile)
		return 1;	// Return :P

	nR = SavePack(PFile,bComp);

	bData[0] = 0;
	nSize[0] = 0;
	if (nR == 0)
	{
		bData[0] = (wsfb*)malloc(PFile->nSize);
		memcpy(bData[0],PFile->bData,PFile->nSize);
		nSize[0] = PFile->nSize;
	}

	wsfclose(PFile);

	return nR;
}

/*
int CWSFPack::SavePack( wsf_file *PFile, wsfb bComp )
{
	wsfb bHi, bLo;
	unsigned long i;

	// Setup Stuff
	bHi = WSFPVERHI;
	bLo = WSFPVERLO;

	// Write Header
	wsfwrite((void*)WSFPHEAD,4,PFile);

	// Write Version
	wsfwrite(&bHi,1,PFile);
	wsfwrite(&bLo,1,PFile);

	// Set Password Flag
	if (m_nPassSum)
		m_nNeedPass = 1;
	else
		m_nNeedPass = 0;

	// Need Password
	wsfwrite(&m_nNeedPass,1,PFile);

	// Compressed?
	wsfwrite(&bComp,1,PFile);	// (at 1.0 should be 0)

	// # of samples
	wsfwrite(&m_nSamples,sizeof(unsigned long),PFile);

#if WINWSF
	g_mStatus->SetSubProgressSteps(m_nSamples);
#endif

	// Save Samples!
	for (i=0;i<m_nSamples;i++)
	{
		wsul nSSize;
		wsfb *bSData;

		wsf_sample *wSamp;
		wSamp = &m_wSamples[i];	// Pick current sample
		
#ifdef PRINTSAVE
		printf("Sample %d/%d..",i+1,m_nSamples);
#endif

		if (wSamp->nSize == 0){
			wsfb b;
			b = WSP_MAGICSKIPNUM;
			wsfwrite(&b,1,PFile);
#ifdef PRINTSAVE
		printf(".\n");
#endif
#ifdef WINWSF
		g_mStatus->StepSubProgress();
#endif
			continue;
		}

		wsfwrite(&wSamp->nOgg,1,PFile);
		wsfwrite(&wSamp->nSize,sizeof(wsul),PFile);

		if (wSamp->nOgg)
		{
			// Compress :0
			wsf_file *wSampf;
			wSampf = wsfcreatemem();

#ifdef PRINTSAVE
			printf(". (encoding)...");
#endif
			OggEncode(wSamp->nSize,wSamp->bSampData,wSamp->nCh,wSamp->nBit,wSampf,wSamp->nFreq);

			nSSize = wSampf->nSize;
			bSData = wSampf->bData;
		}
		else
		{
			nSSize = wSamp->nSize;
			bSData = wSamp->bSampData;
		}

#ifdef PRINTSAVE
		printf(". (crypting)...");
#endif

		if (bComp == 0)
		{
			// Uncompressed sampledata

			// Write Sample Signature
			wsfwrite(wSamp->cSignature,30,PFile);

			// Write Sample Size
			wsfwrite(&nSSize,sizeof(unsigned long),PFile);

			// Encode Sample
			if (m_nPassSum)
				EncodeData(bSData,m_nPassSum,nSSize);

			// Write Sample
			wsfwrite(bSData,nSSize,PFile);

			// Return Sample
			if (m_nPassSum)
				DecodeData(bSData,m_nPassSum,nSSize);
		}
		else
		{
			// Compressed Sampledata ;_;
			unsigned long nFullSize;	// Full Size
			unsigned long nCompSize;	// Size when Compressed
			wsfb *bCompData;			// Compressed Data

			// Sample Signature
			wsfwrite(wSamp->cSignature,30,PFile);

			// Encode Sample
			if (m_nPassSum)
				EncodeData(bSData,m_nPassSum,nSSize);

			nFullSize = nSSize;
			CompressSample(bSData,nFullSize,&bCompData,&nCompSize);

			// Return Sample
			if (m_nPassSum)
				DecodeData(bSData,m_nPassSum,nSSize);

			// Sample Size
			wsfwrite(&nCompSize,sizeof(unsigned long),PFile);
			
			// Make memory and compressed sample data
			wsfwrite(bCompData,nCompSize,PFile);

			// Sample UnComp Size
			wsfwrite(&nFullSize,sizeof(unsigned long),PFile);

			free(bCompData);
		}

		if (wSamp->nOgg)
			free(bSData);

		// Write other stuff
		{
			// Convert to byte
			wsfb bTemp;

			bTemp = (wsfb)wSamp->nBit;
			wsfwrite(&bTemp,1,PFile);

			bTemp = (wsfb)wSamp->nCh;
			wsfwrite(&bTemp,1,PFile);

			wsfwrite(&wSamp->nFreq,sizeof(wsul),PFile);
		}

		wsfwrite(wSamp->cName,26,PFile);

#ifdef PRINTSAVE
		printf("\n");
#endif

#ifdef WINWSF
		g_mStatus->StepSubProgress();
#endif

		// Onwards!
	}

	return 0;
}
*/

int CWSFPack::SavePack( wsf_file *PFile, wsfb bComp )
{
	wsfb bHi, bLo;
	wsfb bC;
	unsigned long i;

	// Setup Stuff
	bHi = WSFPVERHI;
	bLo = WSFPVERLO;

	// Write Header
	wsfwrite((void*)WSFPHEAD,4,PFile);

	// Write Version
	wsfwrite(&bHi,1,PFile);
	wsfwrite(&bLo,1,PFile);

	// Set Password Flag
	if (m_nPassSum)
		m_nNeedPass = 1;
	else
		m_nNeedPass = 0;

	// Need Password
	wsfwrite(&m_nNeedPass,1,PFile);

	// Compressed?
	wsfwrite(&bComp,1,PFile);	// (at 1.0 should be 0)

	// # of samples
	wsfwrite(&m_nSamples,sizeof(unsigned long),PFile);

#if WINWSF
	g_mStatus->SetSubProgressSteps(m_nSamples);
#endif

	// Save Samples!
	for (i=0;i<m_nSamples;i++)
	{
		wsul nSSize;
		wsfb *bSData;
		
		wsf_sample *wSamp;
		wSamp = &m_wSamples[i];	// Pick current sample

		bC = WSFB_UNK;
		
#ifdef PRINTSAVE
		printf("Sample %d/%d..",i+1,m_nSamples);
#endif

		if (wSamp->nSize == 0){
			bC = WSFB_SKIP;
			wsfwrite(&bC,1,PFile);
#ifdef PRINTSAVE
		printf(".\n");
#endif
#ifdef WINWSF
		g_mStatus->StepSubProgress();
#endif
			continue;
		}

		if (wSamp->nOgg)
			bC = WSFB_SAMPOGG;
		else
			bC = WSFB_SAMP;

		// Write Command
		wsfwrite(&bC,1,PFile);

		// Write other stuff
		{
			// Convert to byte
			wsfb bTemp;

			bTemp = (wsfb)wSamp->nBit;
			wsfwrite(&bTemp,1,PFile);

			bTemp = (wsfb)wSamp->nCh;
			wsfwrite(&bTemp,1,PFile);

			wsfwrite(&wSamp->nFreq,sizeof(wsul),PFile);
		}

		wsfwrite(wSamp->cName,26,PFile);

		// Write Untouched Size?			
		if (wSamp->nOgg)
		{
			// Compress :0
			wsf_file *wSampf;

			// untouched size
			wsfwrite(&wSamp->nSize,sizeof(wsul),PFile);

			wSampf = wsfcreatemem();

#ifdef PRINTSAVE
			printf(". (encoding)...");
#endif
			OggEncode(wSamp->nSize,wSamp->bSampData,wSamp->nCh,wSamp->nBit,wSampf,wSamp->nFreq);

			nSSize = wSampf->nSize;
			bSData = wSampf->bData;
		}
		else
		{
			nSSize = wSamp->nSize;
			bSData = wSamp->bSampData;
		}

#ifdef PRINTSAVE
		printf(". (crypting)...");
#endif

		if (bComp == 0)
		{
			// Uncompressed sampledata

			// Write Sample Size
			wsfwrite(&nSSize,sizeof(unsigned long),PFile);

			// Encode Sample
			if (m_nPassSum)
				EncodeData2(bSData,m_nPassSum,nSSize);

			// Write Sample
			wsfwrite(bSData,nSSize,PFile);

			// Return Sample
			if (m_nPassSum)
				DecodeData2(bSData,m_nPassSum,nSSize);
		}
		else
		{
			// Compressed Sampledata ;_;
			unsigned long nFullSize;	// Full Size
			unsigned long nCompSize;	// Size when Compressed
			wsfb *bCompData;			// Compressed Data

			// Encode Sample
			if (m_nPassSum)
				EncodeData2(bSData,m_nPassSum,nSSize);

			nFullSize = nSSize;
			CompressSample(bSData,nFullSize,&bCompData,&nCompSize);

			// Return Sample
			if (m_nPassSum)
				DecodeData2(bSData,m_nPassSum,nSSize);

			// Sample Size
			wsfwrite(&nCompSize,sizeof(unsigned long),PFile);
			
			// Make memory and compressed sample data
			wsfwrite(bCompData,nCompSize,PFile);

			// Sample UnComp Size
			wsfwrite(&nFullSize,sizeof(unsigned long),PFile);

			free(bCompData);
		}

		if (wSamp->nOgg)
			free(bSData);

#ifdef PRINTSAVE
		printf("\n");
#endif

#ifdef WINWSF
		g_mStatus->StepSubProgress();
#endif

		// Onwards!
	}

	// Write that last EOF
	bC = WSFB_EOF;
	wsfwrite(&bC,1,PFile);

	return 0;
}

#endif

//***************************************************************************** */
// CWSFPack::FreePack()
// 
// Purpose: Frees Samples
// 
// Author : [Josh Whelchel], Created : [2/12/2003 10:51:39 AM]
// Parameters:	
// 
// [void] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFPack::FreePack( void )
{
	unsigned long i;

	// Only call if loaded
	if (!m_nLoaded)
		return 1;

	// Free Sample data on each sample
	for (i=0;i<m_nSamples;i++)
		free(m_wSamples[i].bSampData);

	m_wSamples = (wsf_sample*)realloc(m_wSamples,0);
	m_nSamples = 0;
	m_nLoaded = 0;

	return 0;
}

//***************************************************************************** */
// CWSFPack::MakePercent()
// 
// Purpose: Makes Percentage
// 
// Author : [Josh Whelchel], Created : [2/17/2003 7:23:55 PM]
// Parameters:	
// 
// [nLow] :
// [nHi] :
// 
// Returns:	unsigned long
// 
//***************************************************************************** */

unsigned long CWSFPack::MakePercent( wsul nLow, wsul nHi )
{
	double n1;
	double n2;
	double nR;

	n1 = nLow;
	n2 = nHi;

	nR = n1/n2;
	nR *= 100.0;

	return (wsul)nR;
}

//***************************************************************************** */
// *CWSFPack::GetSamp()
// 
// Purpose: Gets Sample for its signature
// 
// Author : [Josh Whelchel], Created : [2/12/2003 11:08:52 AM]
// Parameters:	
// 
// [*cSampSig] :
// 
// Returns:	wsf_sampout
// 
//***************************************************************************** */

int CWSFPack::GetSamp( wsf_sampout *wOut, char *cSampSig )
{
	unsigned long i;

	wOut->bSampData = 0;
	wOut->nSize = 0;
	// Search!
	for (i=0;i<m_nSamples;i++)
	{
		wsf_sample *wS;
		wS = &m_wSamples[i];		// Get Sample
		// Match?
		if (memcmp(cSampSig,wS->cSignature,30)==0){
			// Make sampout

			// Copy it all :D
			wOut->nSize = wS->nSize;
			wOut->bSampData = (wsfb*)malloc(wS->nSize);
			memcpy(wOut->bSampData,wS->bSampData,wOut->nSize);

			return 0;
		}
		// Keep Going :<
	}

	// Not Found!
	return 1;
}

//***************************************************************************** */
// CWSFPack::GetSamp()
// 
// Purpose: Gets Sample for ID
// 
// Author : [Josh Whelchel], Created : [2/16/2003 8:26:23 AM]
// Parameters:	
// 
// [*wOut] :
// [nID] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFPack::GetSampV1( wsf_sampout *wOut, unsigned long nID, wsfb bFlag, wsul nPar, wsfb bAmp )
{
	wsul nSize;
	wsfb *bWPos;
	wsfb *bPos;
	wsf_sample *wS;
	double dAmp;
	if (nID >= m_nSamples)
		return 1;

	wS = &m_wSamples[nID];

	// Allocate
	wOut->nSize = wS->nSize;
	wOut->nOgg = wS->nOgg;
	wOut->nStoredSize = wS->nStoredSize;
	wOut->nCh = wS->nCh;
	wOut->nBit = wS->nBit;
	wOut->bSampData = NULL;
	wOut->nFreq = wS->nFreq;
	nSize = wS->nSize;

	if (!wS->bSampData && wS->nSize != 0)
		return -1;

	wOut->bSampData = (wsfb*)malloc(wS->nSize);

	// Copy it all :D
	if (bFlag == WSFS_NORMAL && bAmp == 100)
	{
		memcpy(wOut->bSampData,wS->bSampData,wOut->nSize);
		return 0;
	}

	dAmp = ((double)bAmp)/100.0;

	// Normal!
	if (bFlag == WSFS_NORMAL)
	{
		int nW;

		bWPos = wOut->bSampData;
		bPos = wS->bSampData;
		while (bPos < wS->bSampData+nSize)
		{
			nW = ReadSample(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	}
	// Reversed!
	else if (bFlag == WSFS_REVERSED)
	{
		int nW;

		bWPos = wOut->bSampData;
		bPos = wS->bSampData+nSize-1;
		while (bPos >= wS->bSampData)
		{
			nW = ReadSampleR(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	}
	// Only until cutoff
	else if (bFlag == WSFS_SAMPCUTOFF)
	{
		int nW;

		wOut->nSize = nPar;
		wOut->bSampData = (wsfb*)realloc(wOut->bSampData,wOut->nSize);

		bWPos = wOut->bSampData;
		bPos = wS->bSampData;

		while (bPos < wS->bSampData+nPar)
		{
			nW = ReadSample(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	}
	// Reverse Only until cutoff
	else if (bFlag == WSFS_REVSAMPCUTOFF)
	{
		int nW;

		wOut->nSize = nPar;
		wOut->bSampData = (wsfb*)realloc(wOut->bSampData,wOut->nSize);

		bWPos = wOut->bSampData;
		bPos = wS->bSampData+wS->nSize-1;

		while (bPos >= wS->bSampData+(wS->nSize-nPar))
		{
			nW = ReadSampleR(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	}
	// Append onto End!
	else if (bFlag == WSFS_SAMPAFTER)
	{
		int nW;
		wsf_sample *wSampB;
		wsul nOldSize;
//		wsul nDif;

		wSampB = &m_wSamples[nPar];
		if (!wSampB->nSize){
			free(wOut->bSampData);
			wOut->bSampData = NULL;
			return 1; // Note: Should this be an error? yes.
		}

		nOldSize = wOut->nSize;
		wOut->nSize += wSampB->nSize;
		wOut->bSampData = (wsfb*)realloc(wOut->bSampData,wOut->nSize);

		bWPos = wOut->bSampData;

		bPos = wS->bSampData;
		while (bPos < wS->bSampData+nOldSize)
		{
			nW = ReadSample(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW));
		} 

		bPos = wSampB->bSampData;
		while (bPos < wSampB->bSampData+wSampB->nSize)
		{
			nW = ReadSample(&bPos,wSampB->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	}
	// WHAT THE FOOK?
	else
	{
		free(wOut->bSampData);
		wOut->bSampData = NULL;
		wOut->nSize = 0;
		return 1;
	}

	return 0;
}

int CWSFPack::GetSamp( wsf_sampout *wOut, unsigned long nID, wsfb bFlag, wsul nPar, wsfb bAmp )
{
	return GetSampV1(wOut,nID,bFlag,nPar,bAmp);
}

//////////////////////////////////////////////////////
// CWSFPack::GetSampV2()
// 
/// \author Josh Whelchel
/// \date 3/31/2004 6:42:16 PM
/// \param *wOut Sample Output (OUT)
/// \param nID ID of sample retrieving
/// \param bFlag Flag of production
/// \param nPar Parameter 1
/// \param nPar2 Parameter 2
/// \param bAmp Amplification
/// \param wSamps Samples Array (Version 2 WSFMod)
///
//
//////////////////////////////////////////////////////

int CWSFPack::GetSampV2( wsf_sampout *wOut, unsigned long nID, wsfb bFlag, wsul nPar, wsul nPar2, float bAmp, WSF2Samps wSamps )
{
	wsul nSize;
	wsfb *bWPos;
	wsfb *bPos;
	wsf_sample *wS;
	double dAmp;
	if (nID >= m_nSamples)
		return 1;

	wS = &m_wSamples[nID];

	// Allocate
	wOut->nSize = wS->nSize;
	wOut->nOgg = wS->nOgg;
	wOut->nStoredSize = wS->nStoredSize;
	wOut->nCh = wS->nCh;
	wOut->nBit = wS->nBit;
	wOut->bSampData = NULL;
	wOut->nFreq = wS->nFreq;
	nSize = wS->nSize;

	if (!wS->bSampData && wS->nSize != 0 && bFlag != WSFS_SAMPAFTER)
		return 1;

	wOut->bSampData = (wsfb*)malloc(wS->nSize);

	// Copy it all :D
	if (bFlag == WSFS_NORMAL && bAmp == 100)
	{
		memcpy(wOut->bSampData,wS->bSampData,wOut->nSize);
		return 0;
	}

	dAmp = (double)bAmp;

	// Normal!
	if (bFlag == WSFS_NORMAL)
	{
		int nW;

		bWPos = wOut->bSampData;
		bPos = wS->bSampData;
		while (bPos < wS->bSampData+nSize)
		{
			nW = ReadSample(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	}
	// Reversed!
	else if (bFlag == WSFS_REVERSED)
	{
		int nW;

		bWPos = wOut->bSampData;
		bPos = wS->bSampData+nSize-1;
		while (bPos >= wS->bSampData)
		{
			nW = ReadSampleR(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	}
	// Only until cutoff
	else if (bFlag == WSFS_SAMPCUTOFF)
	{
		int nW;

		wOut->nSize = nPar;
		wOut->bSampData = (wsfb*)realloc(wOut->bSampData,wOut->nSize);

		bWPos = wOut->bSampData;
		bPos = wS->bSampData;

		while (bPos < wS->bSampData+nPar)
		{
			nW = ReadSample(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	}
	// Reverse Only until cutoff
	else if (bFlag == WSFS_REVSAMPCUTOFF)
	{
		int nW;

		wOut->nSize = nPar;
		wOut->bSampData = (wsfb*)realloc(wOut->bSampData,wOut->nSize);

		bWPos = wOut->bSampData+wOut->nSize-1;
		bPos = wS->bSampData;

		while (bPos < wS->bSampData+nPar)
		{
			nW = ReadSample(&bPos,wS->nBit);
			WriteSampleR(&bWPos,wS->nBit,(wsus)((double)nW*dAmp));
		}
	} 
	// Append onto End!
	else if (bFlag == WSFS_SAMPAFTER)
	{
		int nW;
		wsf_sampout wB;
		wsf_sampout wA;
		wsul nOldSize;
//		wsul nDif;
		WSF2Samp *wR;

		// The nID pointed to a reference (;

		// Set our reference for sample A
		wR = &wSamps->bSamps[nPar2];

		if (wOut->bSampData)
			free(wOut->bSampData);

		// Get our A Sample
		if (GetSampV2(&wA,wR->nID,wR->bFlag,wR->nPar1,wR->nPar2,wR->fAmp,wSamps))
			return 1;

		// Allocate
		wOut->nSize = wA.nSize;
		wOut->nOgg = wA.nOgg;
		wOut->nStoredSize = wA.nStoredSize;
		wOut->nCh = wA.nCh;
		wOut->nBit = wA.nBit;
		wOut->bSampData = NULL;
		wOut->nFreq = wA.nFreq;
		nSize = wA.nSize;

		if (!wA.bSampData || !wA.nSize)
			return 1;

		// Set our reference for sample B
		wR = &wSamps->bSamps[nPar];

		// Get our B Sample
		if (GetSampV2(&wB,wR->nID,wR->bFlag,wR->nPar1,wR->nPar2,wR->fAmp,wSamps))
			return 1;

		/*
		wSampB = &m_wSamples[nPar];
		if (!wSampB->nSize)
			return 1; // Note: Should this be an error? yes.
		*/
		if (!wB.bSampData || !wB.nSize){
			free(wA.bSampData);
			return 1;
		}

		nOldSize = wOut->nSize;
		wOut->nSize += wB.nSize;
		wOut->bSampData = (wsfb*)malloc(wOut->nSize);

		bWPos = wOut->bSampData;

		// Write Sample A
		bPos = wA.bSampData;
		while (bPos < wA.bSampData+wA.nSize)
		{
			nW = ReadSample(&bPos,wA.nBit);
			WriteSample(&bWPos,wA.nBit,(wsus)((double)nW));
		} 

		// Write Sample B
		bPos = wB.bSampData;
		while (bPos < wB.bSampData+wB.nSize)
		{
			nW = ReadSample(&bPos,wB.nBit);
//			WriteSample(&bWPos,wA.nBit,(wsus)((double)nW*dAmp));
			WriteSample(&bWPos,wA.nBit,(wsus)((double)0));
		}

		// Free Sample B
		free(wB.bSampData);
		free(wA.bSampData);
	}
	// WHAT THE FOOK?
	else
	{
		free(wOut->bSampData);
		wOut->bSampData = NULL;
		wOut->nSize = 0;
		return 1;
	}

	return 0;
}

void CWSFPack::SetSampleName( wsul nID, char *cName )
{
	wsf_sample *wSamp;
	// Pick Sample
	wSamp = &m_wSamples[nID];	

	strncpy(wSamp->cName,cName,26);
	wSamp->cName[26]=0;
}

void CWSFPack::GetSampleName( wsul nID, char *cOut )
{
	wsf_sample *wSamp;
	// Pick Sample
	wSamp = &m_wSamples[nID];	

	strcpy(cOut,wSamp->cName);
}

//////////////////////////////////////////////////////
//  CWSFPack::GetSampCompSize()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 11:37:50 AM
/// \param nID ID of Sample to retrieve
/// \return Sample Size
/// \brief Gets the STORED size of a sample (even if unloaded).
/// This was made so that a sample size could be retrieved
/// even if it wasn't to be loaded.
// 
//////////////////////////////////////////////////////

wsul CWSFPack::GetSampCompSize( wsul nID )
{
	if (nID >= m_nSamples)
		return 0;

	return m_wSamples[nID].nStoredSize;
}

//////////////////////////////////////////////////////
//  CWSFPack::GetSampSize()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 11:37:50 AM
/// \param nID ID of Sample to retrieve
/// \return Sample Size
/// \brief Gets the RAW size of a sample (even if unloaded).
/// This was made so that a sample size could be retrieved
/// even if it wasn't to be loaded.
// 
//////////////////////////////////////////////////////

wsul CWSFPack::GetSampSize( wsul nID )
{
	if (nID >= m_nSamples)
		return 0;

	return m_wSamples[nID].nRawSize;
}

//*****************************************************************************
// EncodeString()
// 
// Purpose: Encodes a string based on Encode Bit
// 
// Author : [Josh Whelchel], Created : [12/31/2001 6:20:50 PM]
// Parameters:	
// 
// [*cStr] : String to Encode
// [nEbit] : Bit
// 
// Returns:	none
// 
//*****************************************************************************

void CWSFPack::EncodeData( wsfb *cStr, int nEbit, wsul nLen )
{
	wsul i;
	wsfb *cBit;

	for (i=0;i<nLen;i++)
	{
		cBit = &cStr[i];
		cBit[0] = cBit[0] + nEbit;
	}

	return;
}

//*****************************************************************************
// DecodeString()
// 
// Purpose: Decodes string based on Decode Bit
// 
// Author : [Josh Whelchel], Created : [12/31/2001 6:22:05 PM]
// Parameters:	
// 
// [*cStr] : String to Decode
// [nEbit] : Bit
// 
// Returns:	none
// 
//*****************************************************************************

void CWSFPack::DecodeData( wsfb *cStr, int nEbit, wsul nLen )
{
	wsul i;
	wsfb *cBit;

	for (i=0;i<nLen;i++)
	{
		cBit = &cStr[i];
		cBit[0] = cBit[0] - nEbit;
	}

	return;
}

//*****************************************************************************
// EncodeData2()
// 
// Purpose: Encodes a string based on Encode Bit
// 
// Author : [Josh Whelchel], Created : [12/31/2001 6:20:50 PM]
// Parameters:	
// 
// [*cStr] : String to Encode
// [nEbit] : Bit
// 
// Returns:	none
// 
//*****************************************************************************

void CWSFPack::EncodeData2( wsfb *cStr, int nEbit, wsul nLen )
{
	wsul i;
	wsfb *cBit;

	for (i=0;i<nLen;i++)
	{
		cBit = &cStr[i];
		cBit[0] = cBit[0] ^ nEbit;
	}

	return;
}

//*****************************************************************************
// DecodeData2()
// 
// Purpose: Decodes string based on Decode Bit (for v2.03+)
// 
// Author : [Josh Whelchel], Created : [12/31/2001 6:22:05 PM]
// Parameters:	
// 
// [*cStr] : String to Decode
// [nEbit] : Bit
// 
// Returns:	none
// 
//*****************************************************************************

void CWSFPack::DecodeData2( wsfb *cStr, int nEbit, wsul nLen )
{
	wsul i;
	wsfb *cBit;

	for (i=0;i<nLen;i++)
	{
		cBit = &cStr[i];
		cBit[0] = cBit[0] ^ nEbit;
	}

	return;
}

//////////////////////////////////////////////////////
//  CWSFPack::SetPassSum()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/5/2003 10:58:16 AM
/// \param nPassSum GenerateSum value, without the mod. (%)
// 
//////////////////////////////////////////////////////

void CWSFPack::SetPassSum( int nPassSum )
{
	m_nPassSum = nPassSum%50;
	return;
}	

//////////////////////////////////////////////////////
//  GenerateSum()
// 
//  Author:
/// \author Josh Whelchel
/// \date 10/15/2001 10:54:24 AM
/// \param *code Code to generate sum for
/// \param size Size of code
/// \brief Converts a string into a number by adding up its bytevals
/// This is used for password protection scheming
///
// 
//////////////////////////////////////////////////////

int GenerateSum(char *code, int size)
{
	int i;
	int sum;
	
#ifdef WIN32
	_strlwr(code);	
#else
	strlwr(code);	
#endif
	sum = 0;
	for (i = 0; i < size; i++)
	{
		sum +=(int)code[i];
	}
	
	return sum;
}

void CWSFPack::SetCompressionFlag( wsul nID, wsfb nOgg )
{
	m_wSamples[nID].nOgg = nOgg;	
}

