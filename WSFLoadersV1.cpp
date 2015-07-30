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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

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

// WSFL_WSF

int CWSFLoaderV1::Save( wsf_modout *wOut, wsf_loaddata *wLD )
{
	return Save(wOut,wLD,NULL);
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

int CWSFLoaderV1::Save( wsf_modout *wOut, wsf_loaddata *wLD, int *nSCounts )
{
	unsigned int i;
	wsfb bHi,bLo;
	wsfb *bDat;
	unsigned int nSize;
	wsfb bFill[4];
	unsigned short sH;
	unsigned int nCSize;
	CBaseLoader *xBL;
	wsfb *bCDat;
	int nSub;
	wsf_gspdata gD;
	char cOutExt[5];
	int nSCount[WSFS_COUNT];

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

	bHi = WSFM1VERHI;
	bLo = WSFM1VERLO;
	
	// Write Header
	MAPP(WSFM1HEAD,1,4);

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

	nSub=0;
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
	MAPP(&nSize,1,4);

	// Compress MOD
	DoCompression(bDat,nSize,0,&bCDat,&nCSize,true);

	// Write Mod Compressed Size
	MAPP(&nCSize,1,4);

	// Write (Compressed) Mod
	MAPP(bCDat,1,nCSize);

	// Free Compressed Mod
	free(bCDat);

	// Write number of Samples
	MAPP(&wLD->nSamples,1,4);

	nIDx = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);
	nPar = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);
	bFlag = (wsfb*)malloc(sizeof(wsfb)*wLD->nSamples);
	bAmp = (wsfb*)malloc(sizeof(wsfb)*wLD->nSamples);

#ifdef WINWSF
	g_mStatus->StepSubProgress(wLD->nSamples);
#endif

	// Read Samples
	for (i=0;i<wLD->nSamples;i++)
	{
		wsf_sample *wS;
		wsf_sampout wO;
		unsigned int *nS;
		unsigned int nID;
		char cSampSig[SIGNATURESIZE+1];

		// Pick Sample
		wS = &wLD->bSamples[i];
		nS = &wLD->nSampleSizes[i];

		// Copy the sample over
		wO.nSize = wS->nSize;
		wO.bSampData = (wsfb*)malloc(wO.nSize);
		wO.nBit = wS->nBit;
		wO.nCh = wS->nCh;
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
		wsfb b,a;

		nID = m_wPack->SampleExist(&wO,&b,&p,&a);

		if (nID == WSFERR)
		{
			// Normal Sample Flag
			nID = m_wPack->AddSample(&wO,(wsfb*)cSampSig);
			m_wPack->SetSampleName(nID,wS->cName);
			b = WSFS_NORMAL;
			p = 0;
			a = 100;
		}
/*		else if (bFlag[i] == WSFS_SAMPAFTER)
		{
			wsul nNewSize;
			wsfb *bTemp;
			nNewSize = m_wPack->GetSampSize(p);
			bTemp = (wsfb*)malloc(wO.nSize-nNewSize);
			wO.nSize -= nNewSize;
			memcpy(bTemp,wO.bSampData+nNewSize,wO.nSize);
			free(wO.bSampData);
			wO.bSampData = bTemp;

			nID = m_wPack->SampleExist(&wO,&b,&p,&a);

			// TODO: b == 100 eh?
			if (nID == WSFERR || p != WSFS_NORMAL || b != 100)
			{
				nID = m_wPack->AddSample(&wO,(wsfb*)cSampSig);
				m_wPack->SetSampleName(nID,wS->cName);
			}
			else
			{
				nExtras++;
				bExtras = (wsul*)realloc(bExtras,nExtras);
				bExtras[nExtras-1] = nID;
			}

			nExtras++;
			bExtras = (wsul*)realloc(bExtras,nExtras);
			bExtras[nExtras-1] = nPar[i];

			// Increase the count of each type of sample.
			nSCount[b] += 1;
		}*/
		else	// Increase the count of each type of sample.
			nSCount[b] += 1;

		nIDx[i] = nID;
		bFlag[i] = b;
		bAmp[i] = a;
		nPar[i] = p;

		free(wO.bSampData);
	}

	MAPP(&nExtras,1,sizeof(wsul));
	for (i=0;i<nExtras;i++)
		MAPP(&bExtras[i],1,sizeof(wsul));

	free(bExtras);

	// Write Sub Diff
	MAPP(&nSub,1,4);

	for (i=0;i<wLD->nSamples;i++)
	{
		unsigned int *nS;

		// Pick Sample
		nS = &wLD->nSampleSizes[i];

		// Write Sample Signature
		MAPP(&nIDx[i],1,4);
		// Write Sample Size
		MAPP(nS,1,4);
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
		memcpy(nSCounts,&nSCount,4*(WSFS_COUNT));

	return 0;
}

CWSFLoaderV1::CWSFLoaderV1()
{
	m_fPack = NULL;
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

int CWSFLoaderV1::Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD )
{
	wsfb bHi,bLo;
	char cHead[4];
	char cTPack[27];
	char cPack[200];
	unsigned int i;
	unsigned int nCSize;
	wsfb *bCDat;
	wsul *nIDs;
	wsfb *bFlags;
	wsul *nPars;
	wsfb *bAmps;
	wsul nExtras;
	wsul *bExtras;
	char cOutExt[5];

	cTPack[26] = 0;
	cPack[0] = 0;

	// Open File
	MOPEN(bData,nSize);

	// Read Header
	MREAD(cHead,1,4);

	// Version
	MREAD(&bHi,1,1);
	MREAD(&bLo,1,1);

	if (bHi > WSFM1VERHI || (bHi == WSFM1VERHI && bLo > WSFM1VERLO))
		return 1; // INCORRECT VERSION, NOT SUPPORTED :<

	// Check it
	if (strncmp(cHead,WSFM1HEAD,4)!=0)
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
	MREAD(&wLD->nModSize,1,4);		// Full Size
	MREAD(&nCSize,1,4);		// Compressed Size

	// Allocate Room 
	bCDat = (wsfb*)malloc(nCSize);

	// Read Compressed
	MREAD(bCDat,1,nCSize);

	// DECOMPRESS :D
	DoCompression(bCDat,nCSize,wLD->nModSize,&wLD->bModData,&wLD->nModSize,false);
	free(bCDat);

	// After the mod is read, find samples
	MREAD(&wLD->nSamples,1,4);
	wLD->bSamples = (wsf_sample*)malloc(sizeof(wsf_sample)*wLD->nSamples);
	wLD->nSampleSizes = (unsigned int*)malloc(4*wLD->nSamples);
	wLD->nSampleOffsets = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);

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


	MREAD(&wLD->nSubDiff,1,4);

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
		MREAD(nS,1,4);
		
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

		if (m_wPack->LoadPack(cPack,(wsul*)nIDs,(wsus)(wLD->nSamples+nExtras)))
		{
			free(nIDs);
			return 1;
		}

		strcpy(m_cCurPack,cPack);

	}else{
		if (m_wPack->LoadPack(m_fPack,(wsul*)nIDs,(wsus)(wLD->nSamples+nExtras)))
		{
			free(nIDs);
			return 1;
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

		if (m_wPack->GetSamp(&wO,nIDs[i],bFlags[i],nPars[i],bAmps[i]))
			return 1;
		
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

	free(nIDs);
	free(bFlags);
	free(nPars);
	free(bAmps);

	wLD->nModType = m_nModType;

	return 0;
}

//////////////////////////////////////////////////////
//  CWSFLoaderV1::SetPack()
// 
//  Description: 
/// Sets pack to wsf_file
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 8:13:29 AM
/// \param *wf 
//////////////////////////////////////////////////////

void CWSFLoaderV1::SetPack( wsf_file *wf )
{
	m_fPack = wf;
}

