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
/// \file WSFMod.cpp Main MOD functions

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "WSFGlob.h"
#include "WSFMod.h"
#include "WSFComp.h" 

#ifndef WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
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

char *g_cModExt[] = {
	".IT",				///< Extension for Impulse Tracker Module
	".XM",				///< Extension for XM Modules
	".S3M",				///< Extension for Scream Tracker Modules
	".MOD",				///< Extension for original MOD Modules

	".WSF",				///< Extension for WSF files (internal use only)
	".WTF",				///< Extension for WSF files (internal use only)

	// Insert Formats Here

	"XXX",
	"XXX"
};

char *g_cModDesc[] = {
	
	"Impulse Tracker Module",		// WSFM_IT
	"Fast Tracker Module",			// WSFM_XM
	"Scream Tracker Module",		// WSFM_S3M
	"MOD Module",					// WSFM_MOD

	"Whelchel Sound Format",		// WSFM_WSF
	"Whelchel Tune File",			// WSFM_WTF

	// Insert Formats Here

	"XXX",
	"XXX"
};

wsfb g_bModFlag[] = {

	1,		// IT
	1,		// XM
	1,		// S3M
	1,		// MOD

	1,		// WSF
	1,		// WTF

	// Insert Formats Here

	0,
	0,
};

#ifndef strlwr
#include <ctype.h>
char *strlwr( char *str )
{
	int len = strlen(str);
	for (int i=0;i<len;i++)
	{
		str[i] = tolower(str[i]);
	}
	return str;
}
#endif

//////////////////////////////////////////////////////
//  FreeGSPData()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 1:07:03 AM
/// \param *gspData Pointer to free
/// \brief Frees a wsf_gspdata structure
/// 
// 
//////////////////////////////////////////////////////

void FreeGSPData( wsf_gspdata *gspData )
{
	free(gspData->nSampOffs);
	free(gspData->nSampSizes);
	free(gspData->nFreqs);
	free(gspData->nBit);
	free(gspData->nCh);
	free(gspData->nInSamps);
	free(gspData->cNames);
}

//////////////////////////////////////////////////////
//  InitGSPData()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 1:17:37 AM
/// \param *gspData 
/// \brief Inits wsf_gspdata struct.
/// nSamples must be filled first
///
// 
//////////////////////////////////////////////////////

void InitGSPData( wsf_gspdata *gspData )
{
	wsul nSamps;
	nSamps = gspData->nSamples;
	gspData->nSampOffs = (wsul*)malloc(sizeof(wsul)*nSamps);
	gspData->nSampSizes = (wsul*)malloc(sizeof(wsul)*nSamps);
	gspData->nInSamps = (wsul*)malloc(sizeof(wsul)*nSamps);
	gspData->nBit = (wsfb*)malloc(sizeof(wsfb)*nSamps);
	gspData->nCh = (wsfb*)malloc(sizeof(wsfb)*nSamps);
	gspData->cNames = (char*)malloc(27*nSamps);
	gspData->nFreqs = (wsul*)malloc(sizeof(wsul)*nSamps);

	return;
}

//////////////////////////////////////////////////////
//  PushIntoGSP()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 7:32:36 PM
/// \param *gspData GSP Data
/// \param nSize Size of Sample
/// \param nOffset Offset of Sample
/// \param nSamps Samples
/// \param nCh Channels
/// \param nBit Bitrate
// 
//////////////////////////////////////////////////////

void PushIntoGSP( wsf_gspdata *gspData, wsul nSize, wsul nOffset, wsul nSamps, wsfb nCh, wsfb nBit, char *cName, wsul nFreq )
{
	wsul nID;

	if (nSize <= 0)
		return;

	nID = gspData->nSamples;
	gspData->nSamples++;
	
	gspData->nBit = (wsfb*)realloc(gspData->nBit,sizeof(wsfb)*gspData->nSamples);
	gspData->nCh = (wsfb*)realloc(gspData->nCh,sizeof(wsfb)*gspData->nSamples);

	gspData->nFreqs = (wsul*)realloc(gspData->nFreqs,sizeof(wsul)*gspData->nSamples);
	gspData->nInSamps = (wsul*)realloc(gspData->nInSamps,sizeof(wsul)*gspData->nSamples);
	gspData->nSampOffs = (wsul*)realloc(gspData->nSampOffs,sizeof(wsul)*gspData->nSamples);
	gspData->nSampSizes = (wsul*)realloc(gspData->nSampSizes,sizeof(wsul)*gspData->nSamples);

	gspData->cNames = (char*)realloc(gspData->cNames,27*gspData->nSamples);

	gspData->nBit[nID] = nBit;
	gspData->nCh[nID] = nCh;
	gspData->nFreqs[nID] = nFreq;
	gspData->nInSamps[nID] = nSamps;
	gspData->nSampOffs[nID] = nOffset;
	gspData->nSampSizes[nID] = nSize;
	strncpy((char*)gspData->cNames+(nID*27),cName,26);
	gspData->cNames[nID*27+26] = 0;
}

//***************************************************************************** */
// GetModType()
// 
// Purpose: Gets MOD Type
// 
// Author : [Josh Whelchel], Created : [2/12/2003 11:27:52 AM]
// Parameters:	
// 
// [*cFile] :
// 
// Returns:	int
// 
//***************************************************************************** */

int GetModType( char *cFile )
{
	char *cExt;
	int i;

	// Get File Extension
	cExt = strrchr(cFile,'.');
	if (!cExt)
		return WSFM_NONE;	// Couldn't find it ;_;

	// Go through the count
	for (i=0;i<WSFM_COUNT;i++){
		if (stricmp(cExt,g_cModExt[i])==0){
			if (i==WSFM_WTF)
				return WSFM_WSF;
			else
				return i;	// Found and label :D
		}
	}

	// Not found
	return WSFM_NONE;
}

//*****************************************************************************
// SearchForString()
// 
// Purpose: Searches buffer for a string
// 
// Author : [Josh Whelchel], Created : [2/9/2003 1:58:21 PM]
// Parameters:	
// 
// [*lpBuf] :
// [nSize] :
// [*cStr] :
// 
// Returns:	UL
// 
//*****************************************************************************

wsfb *SearchForString( wsfb *lpBuf, wsul nSize, char *cStr, int nTagLen )
{
	int nP;
	wsul i;
	wsfb *lpPos;
	wsfb *bStart;

	// Reset Tag Length
	nP = 0;

	// Search!
	for (i=0;i<nSize;i++)
	{
		lpPos = lpBuf+i;

		if (lpPos[0] == cStr[nP])
		{
			if (nP == 0)
				bStart = lpPos;

			nP++;
			if (nP == nTagLen)
				return bStart;
		}
		else
			nP = 0;
	}

	return NULL;
}

//////////////////////////////////////////////////////
//  *GetBaseLoader()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 6:07:14 PM
/// \param nModType Mod Type from WSFModVers enum
/// \return 
/// \brief Returns CBaseLoader of mod type
/// 
// 
//////////////////////////////////////////////////////

CBaseLoader *GetBaseLoader( wsfb nModType )
{
	CBaseLoader *xBL;
	xBL = NULL;

	switch (nModType)
	{
	case WSFM_IT:
		xBL = (CBaseLoader*) new CITLoader;
		break;
	case WSFM_S3M:
		xBL = (CBaseLoader*) new CS3MLoader;
		break;
	case WSFM_WTF:
	case WSFM_WSF:
		xBL = (CBaseLoader*) new CWSFLoader;
		break;
	case WSFM_XM:
		xBL = (CBaseLoader*) new CXMLoader;
		break;
	case WSFM_MOD:
		xBL = (CBaseLoader*) new CMODLoader;
		break;
	// Insert Formats Here (case WSFM_xxx: xBL = (CBaseLoader*) new CxxxLoader;
	}

	return (CBaseLoader*)xBL;
}

//***************************************************************************** */
// CWSFMod::CWSFMod()
// 
// Purpose: Construct
// 
// Author : [Josh Whelchel], Created : [2/12/2003 11:17:41 AM]
// Parameters:	
// 
// 
// Returns:	none
// 
//***************************************************************************** */

CWSFMod::CWSFMod()
{
	// Allocate new memories and stuff, and set loaded to 0
	m_nLoaded = 0;
	m_nModType = WSFM_NONE;
	m_fPack = NULL;

	m_wPack = new CWSFPack;

	memset(&m_wLD,0,sizeof(m_wLD));
	m_cModTitle[0] = 0;
}

//***************************************************************************** */
// CWSFMod::~CWSFMod()
// 
// Purpose: Killer
// 
// Author : [Josh Whelchel], Created : [2/12/2003 11:18:43 AM]
// Parameters:	
// 
// 
// Returns:	none
// 
//***************************************************************************** */

CWSFMod::~CWSFMod()
{
	if (m_nLoaded)
		FreeMod();	// Free mod if loaded

	delete m_wPack;
}

int nTest=0;

//***************************************************************************** */
// *ReplaceData()
// 
// Purpose: Replaces data in a buffer
// 
// Author : [Josh Whelchel], Created : [2/15/2003 4:31:52 PM]
// Parameters:	
// 
// [*bIn] : Input buffer
// [nIn] : Size of input buffer
// [*nOut] : Outputs size of new buffer
// [nRepOff] : Replaces data at this offset
// [nRepLength] : with this length
// [*bFill] : with data at this buffer
// [nFillLen] : that is this long
// and
// Returns:	the new buffer
// 
//***************************************************************************** */

/// Replaces a data segment with another.
/// In a large array of data, this function replaces a segment with another.
/// This is used when the samples are inserted into the mod from the sample library,
/// or vice versa when the samples are extracted from the sample library.
/// \note The bIn parameter is not automatically freed and it needs to be.
wsfb *CBaseLoader::ReplaceData( wsfb *bIn, unsigned long nIn, unsigned long *nOut, unsigned long nRepOff, unsigned long nRepLength, wsfb *bFill, unsigned long nFillLen )
{
	wsfb *bOut;
	wsfb *bPos;
	unsigned long nNewSize;
	
	// Gets new size
	nNewSize = nIn - nRepLength + nFillLen;
	bOut = (wsfb*)malloc(nNewSize);
	
	// If they want the output size, tell them
	if (nOut)
		nOut[0] = nNewSize;

	bPos = bOut;

	// Copy up to where the replacement area begins
	memcpy(bPos,bIn,nRepOff);
	bPos += nRepOff;		// Keep count

	// Copy the new area here
	memcpy(bPos,bFill,nFillLen);
	bPos += nFillLen;

	nTest++;
	if (nTest==17)
		nTest=0;

	// Fill the rest of the data in
	memcpy(bPos,bIn+nRepOff+nRepLength,nIn-nRepOff-nRepLength);
	bPos += nIn-nRepLength;

	// Done ! :D
	nTest++;

	return bOut;
}

//***************************************************************************** */
// CWSFMod::SetPackName()
// 
// Purpose: Sets Pack NAme
// 
// Author : [Josh Whelchel], Created : [2/15/2003 4:55:51 PM]
// Parameters:	
// 
// [*cName] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFMod::SetPackName( char *cName )
{
	strncpy(m_cCurPack,cName,26);
	m_cCurPack[26] = 0;

	return 0;
}

//***************************************************************************** */
// CWSFMod::LoadPack()
// 
// Purpose: Loads Pack
// 
// Author : [Josh Whelchel], Created : [2/15/2003 5:04:03 PM]
// Parameters:	
// 
// [*cName] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFMod::LoadPack( char *cName )
{
	strncpy(m_cCurPack,cName,26);
	m_cCurPack[26] = 0;

	m_wPack->LoadPack(cName,NULL,0);

	return 0;
}

#ifndef WSFNOSAVE

//***************************************************************************** */
// CWSFMod::SavePack()
// 
// Purpose: Saves Pack
// 
// Author : [Josh Whelchel], Created : [2/16/2003 10:53:06 AM]
// Parameters:	
// 
// [void] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFMod::SavePack( int nComp )
{
	return m_wPack->SavePack(m_cCurPack,(wsfb)nComp);
}

#endif

//////////////////////////////////////////////////////
//  CWSFMod::ReplaceSamplesForSave()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 5:37:08 PM
/// \param *wOut MOD Out Buffer
/// \param xBL Loader
/// \brief Prepares MOD Data buffer for save
/// 
// 
//////////////////////////////////////////////////////

int CWSFMod::ReplaceSamplesForSave( wsf_modout *wOut, CBaseLoader *xBL, wsf_loaddata *wLD )
{
	wsul nSize;
	wsfb *bDat;
	wsul i;
	wsul *nOffs;
	long nSub;

	// Copy over MOD Data
	nSize = wLD->nModSize;
	
	bDat = (wsfb*)malloc(nSize);
	memcpy(bDat,wLD->bModData,nSize);

	nSub = 0;

	if (wLD->nSampleOffsets[0] == 0)
	{
		wsf_gspdata gD;
		xBL->GetSamplePointers(bDat,nSize,&gD);
		nOffs = (wsul*)malloc(sizeof(wsul)*wLD->nSamples);
		memcpy(nOffs,gD.nSampOffs,sizeof(wsul)*wLD->nSamples);
		FreeGSPData(&gD);
	}
	else
		nOffs = wLD->nSampleOffsets;

	// Mess with MOD Data until we have the samples refilled
	for (i=0;i<wLD->nSamples;i++)
	{
		wsfb *bNew;
		wsul nNew;
		wsul nSOff;
		
		// Get Offset
		nSOff = nOffs[i];

		if (!wLD->bSamples[i].bSampData){
			free(nOffs);
			free(bDat);
			return 1;
		}

		// Replace Data
		if (wLD->bSamples[i].nSize >= 1)
		{
			bNew = xBL->ReplaceData(bDat,nSize,&nNew,nSOff,4,wLD->bSamples[i].bSampData,wLD->bSamples[i].nSize);
			free(bDat);
			bDat = bNew;
			nSize = nNew;

			nSub += 4-wLD->bSamples[i].nSize;
		}else{
			if (m_bHi < 2)
				continue;
			free(nOffs);
			free(bDat);
			return 1;
		}
	}

	if (wLD->nSampleOffsets != nOffs)
		free(nOffs);

	wOut->bModData = bDat;
	wOut->nSize = nSize;

	return 0;
}

//***************************************************************************** */
// CWSFMod::WriteMod()
// 
// Purpose: Saves mod
// 
// Author : [Josh Whelchel], Created : [2/12/2003 1:08:36 PM]
// Parameters:	
// 
// [*wOut] :
// [nWSF] : Save as WSF if true, otherwise use Mod Type
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFMod::WriteMod( wsf_modout *wOut, int nWSF, long *nSCounts )
{
	int nr;
	CBaseLoader *xBL;
	// Calculate size :x
	wOut->bModData = (wsfb*)malloc(0);
	wOut->nSize = 0;

#ifndef WSFNOSAVE
	if (nWSF){
		xBL = (CBaseLoader*)new CWSFLoader;
		((CWSFLoader*)xBL)->SetPack(m_cCurPack,m_wPack);
//		((CWSFLoader*)xBL)->SetPack(m_fPack);

#ifdef WINWSF
		g_mStatus->SetSubProgressSteps((2+m_wPack->GetNumSamples())*m_wLD.nSamples);
#endif

		nr = ((CWSFLoader*)xBL)->Save(wOut,&m_wLD,nSCounts);

		delete xBL;
	}
	else
#endif
	{
		xBL = GetBaseLoader(m_nModType);
		if (!xBL)
			return 1;

		// Return value is important.
		nr = xBL->Save(wOut,&m_wLD);

		// Do Replace Samples HERE
		if (!nr)
			nr = ReplaceSamplesForSave(wOut,xBL,&m_wLD);
		else
			nr -= 1;

		xBL->RestoreSampleEdit(&m_wLD);

		delete xBL;
	}

	return nr;
}

//***************************************************************************** */
// CWSFMod::LoadMod()
// 
// Purpose: Loads Mod
// 
// Author : [Josh Whelchel], Created : [2/12/2003 11:23:11 AM]
// Parameters:	
// 
// [*cFileName] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFMod::LoadMod( char *cFileName, int nFreePack, wsul *nOSamps, wsul nPackOff, wsul nPackSize )
{
	wsf_file *wf;
	int nR;
	wf = wsfopenfile(cFileName);

	if (!wf)
		return 1;

	nR = LoadMod(wf,nFreePack,nOSamps,GetModType(cFileName),nPackOff,nPackSize);

	wsfclose(wf);
	return nR;
}

int CWSFMod::LoadMod( wsfb *bData, wsul nSize, int nFreePack, wsul *nOSamps, int nType, wsul nPackOff, wsul nPackSize )
{
	wsf_file *wf;
	int nR;
	wf = wsfopenmem(bData,nSize);

	if (!wf)
		return 1;

	nR = LoadMod(wf,nFreePack,nOSamps,nType,nPackOff,nPackSize);

	wsfclose(wf);
	return nR;
}

int CWSFMod::LoadMod( wsf_file *wf, int nFreePack, wsul *nOSamps, int nType, wsul nPackOff, wsul nPackSize )
{
	int nR;
	CBaseLoader *xBL;
	wsfb *bMD;
	wsul nMD;

	FreeMod();
	m_wLD.nPackOffset = nPackOff;
	m_wLD.nPackSize = nPackSize;

	m_bHi = 0;
	m_bLo = 0;

	if (nOSamps)
		nOSamps[0] = 666;	/// 666 = Number could not be retrieved :/

	// Get Mod Type
	m_wLD.nModType = nType;

	if (m_wLD.nModType == WSFM_WTF)	// Interchangeable format
		m_wLD.nModType = WSFM_WSF; // Set to WSF

	// Mod not found
	if (m_wLD.nModType == WSFM_NONE)
		return 1;

	m_nModType = m_wLD.nModType;

	nMD = wf->nSize;

	bMD = (wsfb*)malloc(nMD);
	wsfread(bMD,nMD,wf);

	nR = 1;
	xBL = NULL;
	xBL = GetBaseLoader(m_wLD.nModType);

	if (xBL)
	{
		if (wf->cF)
			xBL->SetFileName(wf->cF);
		else
			xBL->SetFileName("");

		if (m_wLD.nModType == WSFM_WSF){
			((CWSFLoader*)xBL)->SetPack(m_cCurPack,m_wPack);
			((CWSFLoader*)xBL)->SetPack(m_fPack);
			((CWSFLoader*)xBL)->SetFreePack(nFreePack);
		}

		nR = xBL->Load(bMD,nMD,&m_wLD);			

		m_bHi = xBL->m_bHi;
		m_bLo = xBL->m_bLo;
		delete xBL;
	}

	free(bMD);

	strcpy(m_cModTitle,m_wLD.cTitle);

	m_nModType = m_wLD.nModType;

	if (nOSamps)
		nOSamps[0] = m_wLD.nSamples;

	if (nR == 0)
		m_nLoaded = 1;
	return nR;
}

//////////////////////////////////////////////////////
//  CWSFMod::SetPassSum()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/5/2003 11:07:38 AM
/// \param nPassSum Password Sum
// 
//////////////////////////////////////////////////////

void CWSFMod::SetPassSum( int nPassSum )
{
	m_wPack->SetPassSum(nPassSum);
}

//***************************************************************************** */
// CWSFMod::FreeMod()
// 
// Purpose: Frees Mod
// 
// Author : [Josh Whelchel], Created : [2/15/2003 4:53:43 PM]
// Parameters:	
// 
// [void] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFMod::FreeMod( void )
{
	unsigned long i;
	if (!m_nLoaded)
		return 1;

	if (m_wLD.bSamples)
	{
		for (i=0;i<m_wLD.nSamples;i++){
			if (m_wLD.bSamples[i].bSampData)
				free(m_wLD.bSamples[i].bSampData);	// Free only if we need to!
		}
	}

	if (m_wLD.bModData)
		free(m_wLD.bModData);

	if (m_wLD.bSamples)
		free(m_wLD.bSamples);

	if (m_wLD.nSampleSizes)
		free(m_wLD.nSampleSizes);

	memset(&m_wLD,0,sizeof(m_wLD));

	m_nLoaded = 0;
	m_nModType = 0;
	return 0;
}

//***************************************************************************** */
// CWSFMod::FreePack()
// 
// Purpose: Frees the pack
// 
// Author : [Josh Whelchel], Created : [2/16/2003 12:12:55 PM]
// Parameters:	
// 
// [void] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFMod::FreePack( void )
{
	m_cCurPack[0] = 0;
	return m_wPack->FreePack();
}

#ifndef WSFNOASCII

//***************************************************************************** */
// *DoReplace()
// 
// Purpose: Replaces actual point in string
// 
// Author : [Josh Whelchel], Created : [5/30/2002 5:13:15 PM]
// Parameters:	
// 
// [*cStr] : String
// [*cStart] : Start of old
// [*cNew] : New String
// [nNewSize] : New String Size
// [nSize] : Full Size
// 
// Returns:	char
// 
//***************************************************************************** */

/// Replaces the area of cStart with cNew in cStr
/// \param cStr String
/// \param cStart Start of old string
/// \param cNew New String
/// \param nNewSize New Size
/// \param nSize Full Size
/// \param cNS Gets location of new 'afterspot'
/// \param nStartSize Offset of beginning replacement area
/// \relates CWSFAscii
char *DoReplace( char *cStr, char *cStart, char *cNew, int nStartSize, int nNewSize, int nSize, char **cNS )
{
	int nBLen;
	int nOLen;
	char *cOut;
	char *cOld;

	// Find length of string before cStart
	nBLen = (int)(cStart-cStr);

	// Find Length of string after cStart
	nOLen = (int)nSize-1-(nBLen+nStartSize);
	cOld = (char*)malloc(nOLen+1);

	// Put in old text
	strcpy(cOld,(char*)(cStr+(nBLen+nStartSize)));

	cOut = (char*)realloc(cStr,nSize-nStartSize+nNewSize);

	cOut[nBLen] = '\0';

	// Append New Text
	strncat(cOut,cNew,nNewSize);

	// Append Old Text
	strcat(cOut,cOld);

	// Free Old Text
	free(cOld);

	// Fill location of new afterspot
	*cNS = (char*)(cOut+nBLen+nNewSize);

	// Return New Text
	return cOut;
}

//***************************************************************************** */
// *ReplaceStr()
// 
// Purpose: Replaces cFind with cNew in cStr
// 
// Author : [Josh Whelchel], Created : [5/30/2002 5:03:24 PM]
// 
// Returns:	char
// 
//***************************************************************************** */

/// Replaces cFind with cNew in cStr
/// \param cStr Input String
/// \param cFind String to Find
/// \param cNew String to replace
/// \relates CWSFAscii
char *ReplaceStr( char *cStr, char *cFind, char *cNew )
{
	char *cTemp;
	char *cCmd;
	int nNSize;
	int nFSize;
	int nSize;
	int nDiff;
	int nC;

	cCmd = cStr;
	nNSize = strlen(cNew);
	nFSize = strlen(cFind);
	nSize = strlen(cCmd);

	nDiff = abs(nFSize-nNSize);

	nC = 0;

	// Find diffrence in strings
	cTemp = strstr(cCmd,cFind);
	while (cTemp != NULL)
	{
		char *cStart;
		int xSize;
		xSize = nSize+(nDiff*nC)+1;

		// Replace
		cCmd = DoReplace(cCmd,cTemp,cNew,nFSize,nNSize,xSize,&cStart);

		cTemp = strstr(cStart,cFind);
		nC++;
	}

	return cCmd;
}

//////////////////////////////////////////////////////
//  MakeTitleFromTags()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/7/2003 10:57:58 AM
/// \param nInTags # of tags
/// \param **cInTags The tag array
/// \param **cInVals The value array
/// \param *cTitle Output Buffer
/// \param *cTitleFmt Format specifing %tagname% etc
/// \brief Creates a title based on given tags replacing %tagname% with the tagvalue given.
///
// 
//////////////////////////////////////////////////////

void CWSFAscii::MakeTitleFromTags( int nInTags, char **cInTags, char **cInVals, char *cTitle, char *cTitleFmt )
{
	char *cStart;
	char *cEnd;

	char **cTags;
	char **cVals;
	int nTags;

	char *cOut;

	int i;

	nTags=0;

	// First, find all the tags we'll be needing
	cTags = (char**)malloc(0);
	cVals = (char**)malloc(0);

	cOut = (char*)malloc(strlen(cTitleFmt)+1);
	cOut[0] = 0;
	strcpy(cOut,cTitleFmt);

	cStart = cTitleFmt;
	while (1)
	{
		int nLen;
		char **cT;
		// Find first %
		cStart = strchr(cStart,'%');

		if (!cStart)
			break;
		cStart++;

		// Find second %
		cEnd = strchr(cStart,'%');
		if (!cEnd)
			break;

		nTags++;
		cTags = (char**)realloc(cTags,sizeof(char*)*nTags);
		cVals = (char**)realloc(cVals,sizeof(char*)*nTags);

		nLen = cEnd-cStart;
		cT = &cTags[nTags-1];
		
		cT[0] = (char*)malloc(nLen+1);
		strncpy(cT[0],cStart,nLen);
		cT[0][nLen] = 0;

		cStart = cEnd+1;
	}

	// Now we get the values and fill
	for (i=0;i<nTags;i++)
	{
		int x;
		
		cVals[i] = (char*)malloc(100);
		// Go through each outer tag
		for (x=0;x<nInTags;x++)
		{
			// If the tags match...
			if (stricmp(cTags[i],cInTags[x])==0){
				strcpy(cVals[i],cInVals[x]);	// ... copy the value.
			}
		}
	}

	// Check and see if our values our nothingness
	for (i=0;i<nTags;i++)
	{
		if (cVals[i][0] == 0)
			sprintf(cVals[i],"Unknown %s",cTags[i]);
	}

	// Now we have the replacement madness ;_;
	for (i=0;i<nTags;i++)
	{
		char cMeRep[100];
		sprintf(cMeRep,"%%%s%%",cTags[i]);

		cOut = ReplaceStr(cOut,cMeRep,cVals[i]);
	}

	for (i=0;i<nTags;i++)
	{
		free(cTags[i]);
		free(cVals[i]);
	}

	strcpy(cTitle,cOut);

	free(cOut);
	free(cTags);
	free(cVals);
	return;
}

//***************************************************************************** */
// CWSFAscii::MakeTitle()
// 
// Purpose: Makes Title
// 
// Author : [Josh Whelchel], Created : [2/16/2003 6:42:13 PM]
// Parameters:	
// 
// [*filename] :
// [*title] :
// [*titlefmt] :
// 
// Returns:	none
// 
//***************************************************************************** */

/// Makes title from tags
/// This is a rather advanced function that can convert a string
/// containing desired title format into a string with the tags
/// replaced.\n
/// For Example: %%title% (%%game%) is replaced with\n
/// Tribulation (Commando)
void CWSFAscii::MakeTitle( char *filename, char *title, char *titlefmt )
{
	char *cStart;
	char *cEnd;

	char **cTags;
	char **cVals;
	int nTags;

	char *cOut;

	int i;

	nTags=0;

	// First, find all the tags we'll be needing
	cTags = (char**)malloc(0);
	cVals = (char**)malloc(0);

	cOut = (char*)malloc(strlen(titlefmt)+1);
	cOut[0] = 0;
	strcpy(cOut,titlefmt);

	cStart = titlefmt;
	while (1)
	{
		int nLen;
		char **cT;
		// Find first %
		cStart = strchr(cStart,'%');

		if (!cStart)
			break;
		cStart++;

		// Find second %
		cEnd = strchr(cStart,'%');
		if (!cEnd)
			break;

		nTags++;
		cTags = (char**)realloc(cTags,sizeof(char*)*nTags);
		cVals = (char**)realloc(cVals,sizeof(char*)*nTags);

		nLen = cEnd-cStart;
		cT = &cTags[nTags-1];
		
		cT[0] = (char*)malloc(nLen+1);
		strncpy(cT[0],cStart,nLen);
		cT[0][nLen] = 0;

		cStart = cEnd+1;
	}

	// Now that we know what tags we want, we do the vandango.
	for (i=0;i<nTags;i++)
	{
		// Allocate room for each value
		cVals[i] = (char*)malloc(100);
		memset(cVals[i],0,100);
	}

	// Now we get the values and fill
	GetWSFTags(filename,cTags,cVals,nTags);

	// Check and see if our values our nothingness
	for (i=0;i<nTags;i++)
	{
		if (cVals[i][0] == 0)
			sprintf(cVals[i],"Unknown %s",cTags[i]);
	}

	// Now we have the replacement madness ;_;
	for (i=0;i<nTags;i++)
	{
		char cMeRep[100];
		sprintf(cMeRep,"%%%s%%",cTags[i]);

		cOut = ReplaceStr(cOut,cMeRep,cVals[i]);
	}

	for (i=0;i<nTags;i++)
	{
		free(cTags[i]);
		free(cVals[i]);
	}

	strcpy(title,cOut);

	free(cOut);
	free(cTags);
	free(cVals);
	return;
}

//***************************************************************************** */
// *CWSFAscii::GetWSFAscii()
// 
// Purpose: Gets WSF Ascii section
// 
// Author : [Josh Whelchel], Created : [2/15/2003 9:32:36 PM]
// Parameters:	
// 
// [*fn] :
// [*nLen] :
// 
// Returns:	char
// 
//***************************************************************************** */

char *CWSFAscii::GetWSFAscii( char *fn, unsigned short *nLen )
{
	wsf_file *MFile;
	unsigned short nL;
	char *cAscii;

	MFile = wsfopenfile(fn);
	if (!MFile)
		return NULL;

	// Seek to last 2 bytes
	wsfend(-2,MFile);
	// Get length of ascii section
	wsfread(&nL,sizeof(unsigned short),MFile);
	wsfend(-2-nL,MFile);
	// Get ascii section and close
	cAscii = (char*)malloc(nL+1);
	wsfread(cAscii,nL,MFile);
	cAscii[nL] = 0;
	wsfclose(MFile);

	if (nLen)
		nLen[0] = nL;

	return cAscii;
}


//////////////////////////////////////////////////////
// *CWSFAscii::GetWSFAscii()
// 
/// \author Josh Whelchel
/// \date 2/8/2004 2:47:40 PM
/// \param *wFile 
/// \param *nLen 
/// \brief Gets Ascii from a wsf_file
///
//
//////////////////////////////////////////////////////

char *CWSFAscii::GetWSFAscii( wsf_file *wFile, unsigned short *nLen )
{
	wsf_file *MFile;
	unsigned short nL;
	char *cAscii;

	MFile = wFile;

	// Seek to last 2 bytes
	wsfend(-2,MFile);
	// Get length of ascii section
	wsfread(&nL,sizeof(unsigned short),MFile);
	wsfend(-2-nL,MFile);
	// Get ascii section and close
	cAscii = (char*)malloc(nL+1);
	wsfread(cAscii,nL,MFile);
	cAscii[nL] = 0;
	wsfbegin(0,wFile);

	if (nLen)
		nLen[0] = nL;

	return cAscii;
}


//***************************************************************************** */
// CWSFAscii::SetWSFAscii()
// 
// Purpose: Sets Ascii Section
// 
// Author : [Josh Whelchel], Created : [2/15/2003 9:34:16 PM]
// Parameters:	
// 
// [*fn] :
// [*cIn] :
// 
// Returns:	int
// 
//***************************************************************************** */

int CWSFAscii::SetWSFAscii( char *fn, char *cIn )
{
	wsf_file *MFile;
	unsigned short nS;
	wsfb *bDat;
	unsigned long nSize;
	
	// Open the file
	MFile = wsfopenfile(fn);
	wsfend(-2,MFile);	// Seek to length of ascii
	wsfread(&nS,2,MFile);	// Read it
	nSize = MFile->nSize;	// Tell the full size
	wsfbegin(0,MFile);	// Go back
	nSize -= 2+nS;	// Get size of all but ascii bit
	bDat = (wsfb*)malloc(nSize);	// Allocate
	wsfread(bDat,nSize,MFile);	// Read
	wsfclose(MFile);	// Close

	// Set strlength for incoming
	nS = strlen(cIn);

	MFile = wsfcreatefile(fn);	// Open
	wsfwrite(bDat,nSize,MFile); // Write
	wsfwrite(cIn,nS,MFile); // Write Ascii
	wsfwrite(&nS,2,MFile); // Write length trail
	wsfclose(MFile);

	free(bDat);

	return 0;
}

//////////////////////////////////////////////////////
//  CWSFAscii::AddWSFTags()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 4:09:08 PM
/// 
/// 
// 
//////////////////////////////////////////////////////

int CWSFAscii::AddWSFTags( char *fn, char **cTag, char **cValue, int nTags )
{
	int i,x;
	wsf_asciiout aO;

	// Get All the Tags
	GetAllTags(fn,&aO);

	// First, see if there are any tags we can just change
	for (i=0;i<nTags;i++)
	{
		char *cT;
		char *cV;

		// Setup Input
		cT = cTag[i];
		cV = cValue[i];

		// Test to see if the tag matches any of the currently existing tags
		for (x=0;x<aO.nTags;x++)
		{
			if (stricmp(cT,aO.cTags[x])==0)
			{
				// Change the value
				aO.cVals[x] = (char*)realloc(aO.cVals[x],strlen(cV)+1);
				strcpy(aO.cVals[x],cV);

				// Mar x so that it appears that we have gotten our tag
				x = -1;
				break;
			}
		}

		if (x != -1)
		{
			// We need to add the tag here
#ifdef WIN32
			AddTagToAsciiOut(&aO,_strlwr(cT),cV);
#else
			AddTagToAsciiOut(&aO,strlwr(cT),cV);
#endif
		}
	}

	SaveWSFTags(fn,aO.cTags,aO.cVals,aO.nTags);

	FreeAsciiOut(&aO);

	return 0;
}

//////////////////////////////////////////////////////
//  CWSFAscii::SaveWSFTags()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 4:14:49 PM
// 
//////////////////////////////////////////////////////

int CWSFAscii::SaveWSFTags( char *fn, char **cTag, char **cValue, int nTags )
{
	int i;
	char *cAscii;
	int nPos;
	int nLen;
	unsigned short nAsc;
/*
	// Get Ascii Section (IF WE WANT TO APPEND)
	cAscii = GetWSFAscii(fn,&nAsc);
	if (!cAscii)
		return -1;
*/
	cAscii = (char*)malloc(2);
	memset(cAscii,0,2);
	nAsc = 0;

	nPos = nAsc;
	nLen = nAsc;

	for (i=0;i<nTags;i++)
	{
		int nTL;
		int nVL;

		nTL = strlen(cTag[i]);
		nVL = strlen(cValue[i]);

#ifdef WIN32
		_strlwr(cTag[i]);
#else
		strlwr(cTag[i]);
#endif

		cAscii = (char*)realloc(cAscii,nLen+nTL+nVL+2);
		memcpy(cAscii+nPos,cTag[i],nTL);
		nPos += nTL;
		memcpy(cAscii+nPos,"=",1);
		nPos += 1;
		memcpy(cAscii+nPos,cValue[i],nVL);
		nPos += nVL;
		memcpy(cAscii+nPos,"\n",1);
		nPos += 1;

		nLen += nTL + 2 + nVL;
	}

	cAscii = (char*)realloc(cAscii,nLen+1);
	memcpy(cAscii+nPos,"\0",1);

	SetWSFAscii(fn,cAscii);

	free(cAscii);
	return 0;
}

//////////////////////////////////////////////////////
//  CWSFAscii::FreeAsciiOut()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 3:51:41 PM
// 
//////////////////////////////////////////////////////

int CWSFAscii::FreeAsciiOut( wsf_asciiout *aO )
{
	int i;

	for (i=0;i<aO->nTags;i++){
		free(aO->cTags[i]);
		free(aO->cVals[i]);
	}

	free(aO->cTags);
	free(aO->cVals);

	return 0;
}

//////////////////////////////////////////////////////
//  CWSFAscii::InitAsciiOut()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 3:54:00 PM
// 
//////////////////////////////////////////////////////

int CWSFAscii::InitAsciiOut( wsf_asciiout *aO )
{
	aO->nTags = 0;

	aO->cTags = (char**)malloc(0);
	aO->cVals = (char**)malloc(0);

	return 0;
}

//////////////////////////////////////////////////////
//  CWSFAscii::RemoveTagFromAsciiOut()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/5/2003 10:19:25 AM
// 
//////////////////////////////////////////////////////

int CWSFAscii::RemoveTagFromAsciiOut( wsf_asciiout *aO, char *cTag, int nIndex )
{
	char **cTags;
	char **cVals;
	int nTags;
	int i,x;

	nTags = aO->nTags-1;
	cTags = (char**)malloc(sizeof(char*)*nTags);
	cVals = (char**)malloc(sizeof(char*)*nTags);

	for (i=0,x=0;i<aO->nTags;i++)
	{
		if (cTag)
		{
			if (stricmp(cTag,aO->cTags[i])==0){
				free(aO->cTags[i]);
				free(aO->cVals[i]);
				continue;
			}
		}
		else
		{
			if (i == nIndex){
				free(aO->cTags[i]);
				free(aO->cVals[i]);
				continue;
			}
		}

		cTags[x] = aO->cTags[i];
		cVals[x] = aO->cVals[i];

		x++;			
	}

	free(aO->cTags);
	free(aO->cVals);

	aO->cTags = cTags;
	aO->cVals = cVals;
	aO->nTags = nTags;

	return 0;
}

//////////////////////////////////////////////////////
//  CWSFAscii::AddTagToAsciiOut()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 3:54:33 PM
// 
//////////////////////////////////////////////////////

int CWSFAscii::AddTagToAsciiOut( wsf_asciiout *aO, char *cTag, char *cVal )
{
	aO->nTags++;

	aO->cTags = (char**)realloc(aO->cTags,sizeof(char*)*aO->nTags);
	aO->cVals = (char**)realloc(aO->cVals,sizeof(char*)*aO->nTags);

#ifdef WIN32
	aO->cTags[aO->nTags-1] = _strdup(cTag);
	aO->cVals[aO->nTags-1] = _strdup(cVal);
#else
	aO->cTags[aO->nTags-1] = strdup(cTag);
	aO->cVals[aO->nTags-1] = strdup(cVal);
#endif

	return 0;
}

int CWSFAscii::GetAllTagsFromAscii( char *cAscii, unsigned short nLen, wsf_asciiout *aO )
{
	int i;
	char *cTPos;		// Tag Position
	char *cEPos;		// End Position
	char *cSPos;		// Space Position
	char *cQPos;		// = Position

	// Init it
	InitAsciiOut(aO);

	cTPos = cAscii;
	while (1)
	{
		// Get =
		cQPos = strchr(cTPos,'=');

		if (!cQPos)
			break;

		// Now we have xTag=x

		// Get new line or end of ascii
		cEPos = strchr(cQPos,'\n');

		if (!cEPos)
			cEPos = strchr(cQPos,'\0');	// try end of ascii

		// Couln't get end
		if (!cEPos)
			break;

		cEPos[0] = 0;
		
		// Get Tag

		// Null the =
		cQPos[0] = '\0';

		AddTagToAsciiOut(aO,cTPos,cQPos+1);
		
		cTPos = cEPos+1;
		cQPos[0] = '=';
	}

	// Replace Spaces
	for (i=0;i<aO->nTags;i++)
	{
		char *cT;
		char *cV;

		cT = aO->cTags[i];
		cV = aO->cVals[i];
		
		// Get rid of spaces
		cSPos = strchr(cT,'_');
		while (cSPos)
		{
			cSPos[0] = ' ';
			cSPos = strchr(cSPos,'_');
		}

		// Get rid of spaces
		cSPos = strchr(cV,'_');
		while (cSPos)
		{
			cSPos[0] = ' ';
			cSPos = strchr(cSPos,'_');
		}		
	}

	return 0;
}

//////////////////////////////////////////////////////
//  CWSFAscii::GetAllTags()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 3:57:03 PM
// 
//////////////////////////////////////////////////////

int CWSFAscii::GetAllTags( char *fn, wsf_asciiout *aO )
{
	wsf_file *wF;
	wF = wsfopenfile(fn);

	if (!wF)
		return 1;

	if (GetAllTags(wF,aO)){
		wsfclose(wF);
		return 1;
	}

	wsfclose(wF);
	return 0;
}

//////////////////////////////////////////////////////
//  CWSFAscii::GetAllTags()
// 
//  Author:
/// \author Josh Whelchel
/// \date 5/02/2004 11:51:04 PM
// 
//////////////////////////////////////////////////////

int CWSFAscii::GetAllTags( wsf_file *wF, wsf_asciiout *aO )
{
	int i;
	unsigned short nLen;
	char *cAscii;
	char *cTPos;		// Tag Position
	char *cEPos;		// End Position
	char *cSPos;		// Space Position
	char *cQPos;		// = Position

	// Init it
	InitAsciiOut(aO);

	// Get WSF Ascii Section
	cAscii = GetWSFAscii(wF,&nLen);

	if (!cAscii)
		return 1;

	cTPos = cAscii;
	while (1)
	{
		// Get =
		cQPos = strchr(cTPos,'=');

		if (!cQPos)
			break;

		// Now we have xTag=x

		// Get new line or end of ascii
		cEPos = strchr(cQPos,'\n');

		if (!cEPos)
			cEPos = strchr(cQPos,'\0');	// try end of ascii

		// Couln't get end
		if (!cEPos)
			break;

		cEPos[0] = 0;
		
		// Get Tag

		// Null the =
		cQPos[0] = '\0';

		AddTagToAsciiOut(aO,cTPos,cQPos+1);
		
		cTPos = cEPos+1;
		cQPos[0] = '=';
	}

	// Replace Spaces
	for (i=0;i<aO->nTags;i++)
	{
		char *cT;
		char *cV;

		cT = aO->cTags[i];
		cV = aO->cVals[i];
		
		// Get rid of spaces
		cSPos = strchr(cT,'_');
		while (cSPos)
		{
			cSPos[0] = ' ';
			cSPos = strchr(cSPos,'_');
		}

		// Get rid of spaces
		cSPos = strchr(cV,'_');
		while (cSPos)
		{
			cSPos[0] = ' ';
			cSPos = strchr(cSPos,'_');
		}		
	}

	// Free Ascii
	free(cAscii);

	return 0;
}

int CWSFAscii::GetWSFTags( wsf_file *wFile, char **cTag, char **cOut, int nTags )
{
	int i;
	unsigned short nLen;
	char *cAscii;
	char *cTPos;		// Tag Position
	char *cEPos;		// End Position
	char *cSPos;		// Space Position

	cAscii = GetWSFAscii(wFile,&nLen);

	if (!cAscii)
		return 1;

	for (i=0;i<nTags;i++)
	{
		char *cO;
		char *cT;

		cO = cOut[i];
		cT = cTag[i];

		cO[0] = 0;

		// setup
		cTPos = cAscii;
#ifdef WIN32
		_strlwr(cT);
#else
		strlwr(cT);
#endif

		// Get tag
tryagainascii:
		cTPos = strstr(cTPos,cT);
		if (!cTPos){
			continue;
		}

		cTPos += strlen(cT);
		if (cTPos[0] != '=')
			goto tryagainascii;

		cTPos++;

		// Find End position
		cEPos = strchr(cTPos,'\n');
		if (!cEPos){
			continue;
		}

		// Null it
		cEPos[0] = 0;

		// Send it through cOut :D
		strcpy(cO,cTPos);

		// Get rid of spaces
		cSPos = strchr(cO,'_');
		while (cSPos)
		{
			cSPos[0] = ' ';
			cSPos = strchr(cSPos,'_');
		}

		// UnNull it
		cEPos[0] = '\n';
	}

	free(cAscii);
	return 0;
}


//////////////////////////////////////////////////////
// CWSFAscii::GetTagValue()
// 
/// \author Josh Whelchel
/// \date 4/2/2004 11:45:28 PM
//
//////////////////////////////////////////////////////

int CWSFAscii::GetTagValue( wsf_asciiout *aO, char *cTag )
{
	int i;

	for (i=0;i<aO->nTags;i++)
	{
		if (stricmp(cTag,aO->cTags[i])==0)
			return i;
	}
	return -1;
}

//////////////////////////////////////////////////////
//  CWSFAscii::GetWSFTags()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 3:51:23 PM
/// 
// 
//////////////////////////////////////////////////////

int CWSFAscii::GetWSFTags( char *fn, char **cTag, char **cOut, int nTags )
{
	int i;
	unsigned short nLen;
	char *cAscii;
	char *cTPos;		// Tag Position
	char *cEPos;		// End Position
	char *cSPos;		// Space Position

	cAscii = GetWSFAscii(fn,&nLen);

	if (!cAscii)
		return 1;

	for (i=0;i<nTags;i++)
	{
		char *cO;
		char *cT;

		cO = cOut[i];
		cT = cTag[i];

		cO[0] = 0;

		// setup
		cTPos = cAscii;
#ifdef WIN32
		_strlwr(cT);
#else
		strlwr(cT);
#endif

		// Get tag
tryagainascii:
		cTPos = strstr(cTPos,cT);
		if (!cTPos){
			continue;
		}

		cTPos += strlen(cT);
		if (cTPos[0] != '=')
			goto tryagainascii;

		cTPos++;

		// Find End position
		cEPos = strchr(cTPos,'\n');
		if (!cEPos){
			continue;
		}

		// Null it
		cEPos[0] = 0;

		// Send it through cOut :D
		strcpy(cO,cTPos);

		// Get rid of spaces
		cSPos = strchr(cO,'_');
		while (cSPos)
		{
			cSPos[0] = ' ';
			cSPos = strchr(cSPos,'_');
		}

		// UnNull it
		cEPos[0] = '\n';
	}

	free(cAscii);
	return 0;
}

#endif

//////////////////////////////////////////////////////
//  CBaseLoader::GetSamplePointers()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 7:20:50 PM
/// 
// 
//////////////////////////////////////////////////////

int CBaseLoader::GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData )
{
	return 1;
}

//

//////////////////////////////////////////////////////
//  *wsfopenfile()
// 
//  Description: 
/// Opens a file as a wsf_file
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:06:21 AM
/// \param *cInput File Input
/// \return file handle
/// 
//////////////////////////////////////////////////////

wsf_file *wsfopenfile( char *cFile )
{
	wsf_file *wf;

	wf = (wsf_file*)malloc(sizeof(wsf_file));
	memset(wf,0,sizeof(wsf_file));

	wf->hFile = fopen(cFile,"rb");
	if (!wf->hFile){
		free(wf);
		return NULL;
	}

	fseek(wf->hFile,0,SEEK_END);
	wf->nSize = ftell(wf->hFile);
	fseek(wf->hFile,0,SEEK_SET);

	wf->bPos = wf->bData;

	wf->cF = (char*)malloc(strlen(cFile)+1);
	strcpy(wf->cF,cFile);

	return wf;
}

//////////////////////////////////////////////////////
//  *wsfopenmem()
// 
//  Description: 
/// Opens memory as a wsf_file
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:07:47 AM
/// \param *bData Data
/// \param nSize Size
/// \return file handle
/// 
//////////////////////////////////////////////////////

wsf_file *wsfopenmem( wsfb *bData, wsul nSize, wsfb nFree )
{
	wsf_file *wf;
	wf = (wsf_file*)malloc(sizeof(wsf_file));
	memset(wf,0,sizeof(wsf_file));

	if (!nFree)
	{
		wf->bData = bData;
		wf->nSize = nSize;
	}
	else
	{
		wf->nFree = 1;
		wf->bData = (wsfb*)malloc(nSize);
		memcpy(wf->bData,bData,nSize);
		wf->nSize = nSize;
	}

	wf->bPos = wf->bData;

	return wf;
}

//////////////////////////////////////////////////////
//  *wsfcreatefile()
// 
//  Description: 
/// Creates wsf_file as a file
/// 
/// \note File isn't written until wsfclosefile() is called
/// \author Josh Whelchel
/// \date 8/5/2003 12:08:36 AM
/// \param *cFile Filename
/// \return file handle
/// 
//////////////////////////////////////////////////////

wsf_file *wsfcreatefile( char *cFile )
{
	wsf_file *wf;

	wf = (wsf_file*)malloc(sizeof(wsf_file));
	memset(wf,0,sizeof(wsf_file));

	wf->hFile = fopen(cFile,"wb");
	if (!wf->hFile){
		free(wf);
		return NULL;
	}

	wf->cF = (char*)malloc(strlen(cFile)+1);
	strcpy(wf->cF,cFile);
	wf->bW = 1;

	return wf;
}

//////////////////////////////////////////////////////
//  *wsfcreatemem()
// 
//  Description: 
/// Creates wsf_file as memory
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:12:16 AM
/// \return 
/// 
//////////////////////////////////////////////////////

wsf_file *wsfcreatemem( void )
{
	wsf_file *wf;
	wf = (wsf_file*)malloc(sizeof(wsf_file));
	memset(wf,0,sizeof(wsf_file));
	
	wf->bData = (wsfb*)malloc(0);
	wf->bPos = wf->bData;
	wf->bW = 1;
	wf->nFree = 1;

	return wf;
}

//////////////////////////////////////////////////////
//  wsfclose()
// 
//  Description: 
/// Closes wsf_file handle
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:13:40 AM
/// \param *wf file handle
//////////////////////////////////////////////////////

void wsfclose( wsf_file *wf )
{
	if (wf->cF)
	{
		// Free memory
		free(wf->cF);
	}

	if (wf->hFile)
		fclose(wf->hFile);

	if (wf->nFree && wf->bData)
		free(wf->bData);

	free(wf);

	return;
}
	
//////////////////////////////////////////////////////
//  wsfread()
// 
//  Description: 
/// Reads from wsf_file
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:17:13 AM
/// \param *bData Data Buffer
/// \param nSize Bytes to Read
/// \param *wf File handle
/// \return Bytes Read
/// 
//////////////////////////////////////////////////////

wsul wsfread( void *bData, wsul nSize, wsf_file *wf )
{
	wsul nRead;

	nRead = nSize;
	if (nRead + wf->nPos >= wf->nSize)
		nRead = wf->nSize - wf->nPos;

	if (wf->hFile)		// FILE
		fread(bData,1,nSize,wf->hFile);
	else{				// MEMORY
		memcpy(bData,wf->bPos,nRead);
		wf->bPos += nRead;
	}

	wf->nPos += nRead;

	return nRead;
}

//////////////////////////////////////////////////////
//  wsfwrite()
// 
//  Description: 
/// Writes to wsf_file
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:18:12 AM
/// \param *bData Data Buffer
/// \param nSize Bytes to write
/// \param *wf File handle
/// \return Bytes Written
/// 
//////////////////////////////////////////////////////

wsul wsfwrite( void *bData, wsul nSize, wsf_file *wf )
{
	wf->nSize += nSize;

	if (wf->hFile)
		fwrite(bData,1,nSize,wf->hFile);
	else
	{
		wf->bData = (wsfb*)realloc(wf->bData,wf->nSize);
		wf->bPos = wf->bData + wf->nPos;

		memcpy(wf->bPos,bData,nSize);
		wf->bPos += nSize;
	}

	wf->nPos += nSize;

	return nSize;
}

//////////////////////////////////////////////////////
//  wsfseek()
// 
//  Description: 
/// Seeks from current position
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:21:17 AM
/// \param nPos Position to seek offset
/// \param *wf file handle
//////////////////////////////////////////////////////

void wsfseek( wsul nPos, wsf_file *wf )
{
	if (wf->hFile)
	{
		fseek(wf->hFile,nPos,SEEK_CUR);
	}
	else
		wf->bPos += nPos;
	
	wf->nPos += nPos;
}

//////////////////////////////////////////////////////
//  wsfbegin()
// 
//  Description: 
/// Seeks from beginning of file
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:21:03 AM
/// \param nOff File offset
/// \param *wf File Handle
//////////////////////////////////////////////////////

void wsfbegin( wsul nOff, wsf_file *wf )
{
	wf->nPos = nOff;

	if (wf->hFile)
		fseek(wf->hFile,nOff,SEEK_SET);
	else
		wf->bPos = wf->bData + nOff;
}

//////////////////////////////////////////////////////
//  wsfend()
// 
//  Description: 
/// Seeks from end of file
/// 
/// 
/// \author Josh Whelchel
/// \date 8/5/2003 12:25:20 AM
/// \param nOff offset
/// \param *wf file handle
//////////////////////////////////////////////////////

void wsfend( long nOff, wsf_file *wf )
{
	wf->nPos = wf->nSize + nOff;

	if (wf->hFile)
		fseek(wf->hFile,nOff,SEEK_END);
	else
		wf->bPos = wf->bData + wf->nSize + nOff;
}


//////////////////////////////////////////////////////
// CBaseLoader::AllowSampleEdit()
// 
// Description
// Freeze Samples
// 
/// \author Josh Whelchel
/// \date 7/5/2004 10:01:58 AM
/// \param *wLD Load Data to backup
///
//
//////////////////////////////////////////////////////

int CBaseLoader::AllowSampleEdit( wsf_loaddata *wLD )
{
	wsul i;

	m_wCopySamps = (wsf_sample*)malloc(sizeof(wsf_sample)*wLD->nSamples);

	for (i=0;i<wLD->nSamples;i++)
	{
		wsf_sample *wD;
		wsf_sample *wS;
		wD = &m_wCopySamps[i];
		wS = &wLD->bSamples[i];

		memcpy(wD,wS,sizeof(wsf_sample));

		wD->bSampData = (wsfb*)malloc(wS->nSize);
		memcpy(wD->bSampData,wS->bSampData,wS->nSize);		
	}

	return 0;
}


//////////////////////////////////////////////////////
// CBaseLoader::RestoreSampleEdit()
// 
// Description
// 
//
/// \author Josh Whelchel
/// \date 7/5/2004 10:08:51 AM
/// \param *wLD Load Data to 'refill'
///
//
//////////////////////////////////////////////////////

int CBaseLoader::RestoreSampleEdit( wsf_loaddata *wLD )
{
	wsul i;

	if (!m_wCopySamps)
		return 0;

	for (i=0;i<wLD->nSamples;i++)
	{
		wsf_sample *wD;
		wsf_sample *wS;
		wS = &m_wCopySamps[i];
		wD = &wLD->bSamples[i];

		memcpy(wD->bSampData,wS->bSampData,wD->nSize); 

		free(wS->bSampData);
	}

	free(m_wCopySamps);

	return 0;
}

CBaseLoader::CBaseLoader()
{
	m_wCopySamps = NULL;
}
