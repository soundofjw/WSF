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
#include "WSFModV1.h"
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

 // Test

/*

wsfb	*gp_bS;	// Byte Start
wsfb	*gp_bP;	// Byte Pos
unsigned int gp_nB;	// Size of bytes
unsigned int gp_nW;

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
unsigned int gm_nB;	// Size of bytes
unsigned int gm_nW;

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

void WriteSample( wsfb **bPos, wsus nBit, int nWrite );

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

void WriteSampleR( wsfb **bPos, wsus nBit, int nWrite );

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

int ReadSample( wsfb **bPos, wsus nBit );

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

int ReadSampleR( wsfb **bPos, wsus nBit );

//***************************************************************************** */
// CWSFPackV1::CWSFPackV1()
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

CWSFPackV1::CWSFPackV1()
{
	// Set Not Loaded
	m_nLoaded = 0;
	// Allocate Sample Buffers to 0
	m_wSamples = (wsf_sample*)malloc(sizeof(wsf_sample));
	m_nSamples = 0;
	m_nPassSum = 0;
	m_nNeedPass = 0;
}

//***************************************************************************** */
// CWSFPackV1::~CWSFPackV1()
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

CWSFPackV1::~CWSFPackV1()
{
	// Loaded?
	if (m_nLoaded)
		FreePack();	// Frees the samples and other allocations

	// Free sample buffer
	free(m_wSamples);
}

//***************************************************************************** */
// CWSFPackV1::CompressSample()
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

int CWSFPackV1::CompressSample( wsfb *bFullSamp, unsigned int nFullSize, wsfb **bOutSamp, unsigned int *nCompSize )
{
	return DoCompression(bFullSamp,nFullSize,0,bOutSamp,nCompSize,true);
}

//***************************************************************************** */
// CWSFPackV1::DeCompressSample()
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

int CWSFPackV1::DeCompressSample( wsfb *bCompSamp, unsigned int nCompSize, wsfb **bOutSamp, unsigned int nFullSize )
{
	return DoCompression(bCompSamp,nCompSize,nFullSize,bOutSamp,NULL,false);
}

//***************************************************************************** */
// CWSFPackV1::LoadPack()
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

int CWSFPackV1::LoadPack( char *cFile, wsul *nIDs, wsus nNumIDs )
{
	wsf_file *PFile;
	int nR;
	// Open File
	PFile = wsfopenfile(cFile);
	// Error?
	if (!PFile)
		return 1;	// Return :P

	nR = LoadPack(PFile,nIDs,nNumIDs);
	wsfclose(PFile);
	return nR;
}

int CWSFPackV1::LoadPack( wsfb *bData, wsul nSize, wsul *nIDs, wsus nNumIDs )
{
	wsf_file *PFile;
	int nR;
	// Open File
	PFile = wsfopenmem(bData,nSize);
	// Error?
	if (!PFile)
		return 1;	// Return :P

	nR = LoadPack(PFile,nIDs,nNumIDs);
	wsfclose(PFile);
	return nR;
}

int CWSFPackV1::LoadPack( wsf_file *PFile, wsul *nIDs, wsus nNumIDs )
{
	char cHead[4];
	wsfb bHi, bLo;
	wsfb bComp;
	unsigned int i;

	wsfbegin(0,PFile);

	// Get Header
	wsfread(cHead,4,PFile);

	// Match Header
	if (strncmp(WSFP1HEAD,cHead,4)!=0){
		// Error No Match
		wsfclose(PFile);
		return 1;
	}

	// Get Version
	wsfread(&bHi,1,PFile);
	wsfread(&bLo,1,PFile);

	if (bHi > WSFP1VERHI || (bHi == WSFP1VERHI && bLo > WSFP1VERLO))
		return 1; // INCORRECT VERSION, NOT SUPPORTED :<

	if (TESTVER(1,23))
		wsfread(&m_nNeedPass,1,PFile);
	else
		m_nNeedPass = 0;

	// Get Compressed?
	wsfread(&bComp,1,PFile);	// 

	// Get Number of Samples
	wsfread(&m_nSamples,sizeof(unsigned int),PFile);

	// Reallocate for number of samples
	m_wSamples = (wsf_sample*)realloc(m_wSamples,sizeof(wsf_sample)*m_nSamples);

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
			wsus x;
			for (x=0;x<nNumIDs;x++)
			{
				if (nIDs[x]==i){
					nF = 1;		// Mark load flag to TRUE
					break;
				}
			}
		}
		else
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
			wSamp->nOgg = 0;

		if (bComp == 0)
		{
			// Uncompressed sampledata

			// Read Sample Signature
			wsfread(wSamp->cSignature,30,PFile);
			wSamp->cSignature[30] = 0;

			// Read Sample Size
			wsfread(&wSamp->nSize,sizeof(unsigned int),PFile);
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
			unsigned int nFullSize;	// Full Size
			unsigned int nCompSize;	// Size when Compressed
			wsfb *bCompData;			// Compressed DAta

			// Sample Signature
			wsfread(wSamp->cSignature,30,PFile);
			wSamp->cSignature[30] = 0;

			// Sample Size
			wsfread(&nCompSize,sizeof(unsigned int),PFile);
			wSamp->nStoredSize = nCompSize;

			if (nF)
			{
				// Make memory and compressed sample data
				bCompData = (wsfb*)malloc(nCompSize);
				wsfread(bCompData,nCompSize,PFile);

				// Sample UnComp Size
				wsfread(&nFullSize,sizeof(unsigned int),PFile);
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

//***************************************************************************** */
// CWSFPackV1::SavePack()
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

int CWSFPackV1::SavePack( char *cFile, wsfb bComp )
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

int CWSFPackV1::SavePack( wsfb **bData, wsul *nSize, wsfb bComp )
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

int CWSFPackV1::SavePack( wsf_file *PFile, wsfb bComp )
{
	wsfb bHi, bLo;
	unsigned int i;

	// Setup Stuff
	bHi = WSFP1VERHI;
	bLo = WSFP1VERLO;

	// Write Header
	wsfwrite((char*)WSFP1HEAD,4,PFile);

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
	wsfwrite(&m_nSamples,sizeof(unsigned int),PFile);

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
			OggEncode(wSamp->nSize,wSamp->bSampData,wSamp->nCh,wSamp->nBit,wSampf);

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
			wsfwrite(&nSSize,sizeof(unsigned int),PFile);

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
			unsigned int nFullSize;	// Full Size
			unsigned int nCompSize;	// Size when Compressed
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
			wsfwrite(&nCompSize,sizeof(unsigned int),PFile);
			
			// Make memory and compressed sample data
			wsfwrite(bCompData,nCompSize,PFile);

			// Sample UnComp Size
			wsfwrite(&nFullSize,sizeof(unsigned int),PFile);

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

//***************************************************************************** */
// CWSFPackV1::FreePack()
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

int CWSFPackV1::FreePack( void )
{
	unsigned int i;

	// Only call if loaded
	if (!m_nLoaded)
		return 1;

	// Free Sample data on each sample
	for (i=0;i<m_nSamples;i++)
		free(m_wSamples[i].bSampData);

	m_wSamples = (wsf_sample*)realloc(m_wSamples,sizeof(m_wSamples));
	m_nSamples = 0;
	m_nLoaded = 0;

	return 0;
}

//***************************************************************************** */
// CWSFPackV1::MakePercent()
// 
// Purpose: Makes Percentage
// 
// Author : [Josh Whelchel], Created : [2/17/2003 7:23:55 PM]
// Parameters:	
// 
// [nLow] :
// [nHi] :
// 
// Returns:	unsigned int
// 
//***************************************************************************** */

unsigned int CWSFPackV1::MakePercent( wsul nLow, wsul nHi )
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
// CWSFPackV1::MakeOldSignature()
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

int CWSFPackV1::MakeOldSignature( wsfb *wSamp, wsul nSize, char *cOut )
{
	unsigned int i=0;
	unsigned int nSegLen;
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
// CWSFPackV1::CreateSignature()
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

int CWSFPackV1::CreateSignature( wsfb *wSamp, wsul nSize, wsus nCh, wsus nBit, wsul nSamps, char *cOut )
{
	unsigned int nSegLen;		// Size of each segment to scan in order to equal SIGNATURESIZE for cOut
	unsigned int i;
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
// *CWSFPackV1::GetSamp()
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

int CWSFPackV1::GetSamp( wsf_sampout *wOut, char *cSampSig )
{
	unsigned int i;

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
// CWSFPackV1::GetSamp()
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

int CWSFPackV1::GetSamp( wsf_sampout *wOut, unsigned int nID, wsfb bFlag, wsul nPar, wsfb bAmp )
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
	wOut->bSampData = (wsfb*)malloc(wS->nSize);
	wOut->nOgg = wS->nOgg;
	wOut->nStoredSize = wS->nStoredSize;
	nSize = wS->nSize;

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
		wsul nW;

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
		wsul nW;

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
		wsul nW;

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
		wsul nW;

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
		wsul nW;
		wsf_sample *wSampB;
//		wsul nDif;

		wSampB = &m_wSamples[nPar];
		if (!wSampB->nSize)
			return 1; // Note: Should this be an error? yes.

		wOut->nSize += wSampB->nSize;
		wOut->bSampData = (wsfb*)realloc(wOut->bSampData,wOut->nSize);

		bWPos = wOut->bSampData;
		bPos = wSampB->bSampData;
		while (bPos < wSampB->bSampData+wSampB->nSize)
		{
			nW = ReadSample(&bPos,wS->nBit);
			WriteSample(&bWPos,wS->nBit,(wsus)((double)nW));
		}

		bPos = wS->bSampData;
		while (bPos < wS->bSampData)
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

//***************************************************************************** */
// CWSFPackV1::SampleExist()
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

unsigned int CWSFPackV1::SampleExist( char *cSampSig, int nCh, int nBit )
{
	unsigned int i;

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
//  CWSFPackV1::SamplesMatch()
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
/// \return 0 if Match, nonzero if no match
/// 
//////////////////////////////////////////////////////

wsul CWSFPackV1::SamplesMatch( wsf_sampout *wSampA, wsf_sample *wSampB, wsfb *bFlag, wsul *nPar, wsfb *nAmp, wsul nID, int nLimit )
{
#define SMEND(x,y,z) { nRet = x; bRFlag = y; nRPar = z; goto smend; }
	wsfb *bPosA, *bPosB;
//	wsfb *bWPosA, *bWPosB;
	wsfb *bSampA, *bSampB;
	wsul nSampA, nSampB;
	int nLoudA, nLoudB;
	wsus nBit,nCh;
	wsfb bRFlag;
	wsul nRPar;
	wsul nRet;
	double dNormA;
	double dNormB;
	int nMaxA;
	int nMaxB;
	int nMinA;
	int nMinB;

	wsus nBitA;
	wsus nBitB;

	// Generic 
	nBitA = wSampA->nBit;
	nBitB = wSampB->nBit;
	nBit = wSampA->nBit;
	nCh = wSampA->nCh;
	double dAmp;

#define COMPCHECK ((nA*dNormA != nB*dNormB) && (nA*dNormA != nMinA) && (nA*dNormA != nMaxA) && (nB*dNormB != nMinB) && (nB*dNormB != nMaxB))

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

	// Fill the buffers with their full volume
	
	// Determine the loudest volume (buffer A)
	bPosA = bSampA;
	nLoudA = 0;

	while (bPosA < bSampA+nSampA)
	{
		int nS;
		nS = ReadSample(&bPosA,nBit);

		if (nS > nLoudA)
			nLoudA = nS;
	}

	// Determine the loudest volume (buffer B)
	bPosB = bSampB;
	nLoudB = 0;

	while (bPosB < bSampB+nSampB)
	{
		int nS;
		nS = ReadSample(&bPosB,nBit);

		if (nS > nLoudB)
			nLoudB = nS;
	}

	if (nBitB == 8){
		dNormB = 255.0/nLoudA;
		nMaxB = 255;
		nMinB = -256;
	}
	else{
		dNormB = 32767.0/nLoudB;
		nMaxB = 32767;
		nMinB = -32768;
	}

	dAmp = nLoudA / nLoudB;

	// Test the two
	if (nBitA == 8){
		dNormA = 255.0/nLoudA;
		nMaxA = 255;
		nMinA = -256;
	}
	else{
		dNormA = 32767.0/nLoudA;
		nMaxA = 32767;
		nMinA = -32768;
	}


	// Compare the two sample buffers

	if (nSampA == nSampB || nSampA < nSampB)
	{
		// Test the normal sample

		bPosA = bSampA;
		bPosB = bSampB;

		nRet = 0;
		while (bPosA < bSampA+nSampA)
		{
			double nA,nB;
			nA = ReadSample(&bPosA,nBit);
			nB = ReadSample(&bPosB,nBit);

//			if (nA != nLoudA && nB != nLoudB && ((double)nA*dAmp) != ((double)nB)) 
//			if ( (nSampA < 200 || (nA != nLoudA && nB != nLoudB)) && ((double)nA*dAmp) != ((double)nB)) 
			if (COMPCHECK)		// < WORKS
			{
				nRet = 1;
				break;
			}
		}

		if (nRet == 0 && nSampA == nSampB)
			SMEND(0,WSFS_NORMAL,0);

		if (nRet == 0 && nSampA < nSampB)
			SMEND(0,WSFS_SAMPCUTOFF,nSampA);
	}

	if (nLimit)
		goto smend;

	if (nSampA == nSampB || nSampA < nSampB)
	{
		// Test the reversed sample

		bPosA = bSampA;
		bPosB = bSampB+nSampB-1-(nSampB-nSampA);

		nRet = 0;
		while (bPosA < bSampA+nSampA)
		{
			double nA,nB;
			nA = ReadSample(&bPosA,nBit);
			nB = ReadSampleR(&bPosB,nBit);

			if (COMPCHECK)		// < WORKS
			{
				nRet = 1;
				break;
			}
		}

		if (nRet == 0 && nSampA == nSampB)
			SMEND(0,WSFS_REVERSED,0);

		if (nRet == 0 && nSampA < nSampB)
			SMEND(0,WSFS_REVSAMPCUTOFF,nSampA);
	}

	/*
	if (nSampA > nSampB)
	{
		// Test the appended

		bPosA = bSampA + (nSampA-nSampB);

		bPosB = bSampB;
		nLoudA = 0;

		while (bPosA < bSampA+nSampA)
		{
			wsul nS;
			nS = ReadSample(&bPosA,nBit);

			if (nS > nLoudA)
				nLoudA = nS;
		}

		dAmp = nLoudA / nLoudB;

		bPosA = bSampA + (nSampA-nSampB);

		nRet = 0;
		while (bPosB < bSampB+nSampB)
		{
			wsul nA,nB;
			nA = ReadSample(&bPosA,nBit);
			nB = ReadSample(&bPosB,nBit);

//			if (nA != nLoudA && nB != nLoudB && ((double)nA*dAmp) != ((double)nB)) 
//			if ( (nSampA < 200 || (nA != nLoudA && nB != nLoudB)) && ((double)nA*dAmp) != ((double)nB)) 
			if ((double)nA != (double)nB*dAmp)		// < WORKS
			{
				nRet = 1;
				break;
			}
		}

		if (nRet == 0){
			SMEND(0,WSFS_SAMPAFTER,nID);
		}
	}*/

smend:

	if (nRet==0)
	{
		wsul nP;

		bFlag[0] = bRFlag;
		nPar[0] = nRPar;

		nP = (wsfb)MakePercent(nLoudA,nLoudB);
		if (nP >= 255)
			nAmp[0] = 255;
		else
			nAmp[0] = (wsfb)nP;

	}

	return nRet;
}

//////////////////////////////////////////////////////
//  CWSFPackV1::SampleExist()
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

wsul CWSFPackV1::SampleExist( wsf_sampout *wInSamp, wsfb *bFlag, wsul *nPar, wsfb *nAmp, int nLimit )
{
	wsul i;

	if (wInSamp->nSize <= 450)
		return WSFERR;

	// Search (this won't be pretty at all :o)
	for (i=0;i<m_nSamples;i++)
	{
		wsf_sample *wS;
		wS = &m_wSamples[i];	// Get Sample

//		if (wS->nBit != wInSamp->nBit)
//			continue;

		if (wS->nCh != wInSamp->nCh){
#ifdef WINWSF
			g_mStatus->StepSubProgress();
#endif
			continue;
		}

		// Okay now we have to test :0
		if (SamplesMatch(wInSamp,wS,bFlag,nPar,nAmp,i,nLimit)==0)
		{
#ifdef WINWSF
			g_mStatus->StepSubProgress(m_nSamples-i);
#endif
			return i;	// WOOT a match!
		}

#ifdef WINWSF
		g_mStatus->StepSubProgress();
#endif

	}

	return WSFERR;
}

//***************************************************************************** */
// CWSFPackV1::AddSample()
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

unsigned int CWSFPackV1::AddSample( wsf_sampout *wInSamp, wsfb *bSig )
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
	wSamp->nStoredSize = wInSamp->nStoredSize;
	wSamp->cName[0] = 0;

	// Make Signature
	memcpy(wSamp->cSignature,bSig,SIGNATURESIZE);
	wSamp->cSignature[30] = 0;

	// Its Loaded. XD
	m_nLoaded = 1;

	return m_nSamples-1;
}

void CWSFPackV1::SetSampleName( wsul nID, char *cName )
{
	wsf_sample *wSamp;
	// Pick Sample
	wSamp = &m_wSamples[nID];	

	strncpy(wSamp->cName,cName,26);
	wSamp->cName[26]=0;
}

void CWSFPackV1::GetSampleName( wsul nID, char *cOut )
{
	wsf_sample *wSamp;
	// Pick Sample
	wSamp = &m_wSamples[nID];	

	strcpy(cOut,wSamp->cName);
}

//////////////////////////////////////////////////////
//  CWSFPackV1::GetSampSize()
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

wsul CWSFPackV1::GetSampSize( wsul nID )
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

void CWSFPackV1::EncodeData( wsfb *cStr, int nEbit, wsul nLen )
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

void CWSFPackV1::DecodeData( wsfb *cStr, int nEbit, wsul nLen )
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

//////////////////////////////////////////////////////
//  CWSFPackV1::SetPassSum()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/5/2003 10:58:16 AM
/// \param nPassSum GenerateSum value, without the mod. (%)
// 
//////////////////////////////////////////////////////

void CWSFPackV1::SetPassSum( int nPassSum )
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

int GenerateSum(char *code, int size);

void CWSFPackV1::SetCompressionFlag( wsul nID, wsfb nOgg )
{
	m_wSamples[nID].nOgg = nOgg;	
}
