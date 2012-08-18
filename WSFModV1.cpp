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
#include "WSFComp.h"
#include "WSFPackV1.h"
#include "WSFModV1.h"

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

void FreeGSPData( wsf_gspdata *gspData );

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

void InitGSPData( wsf_gspdata *gspData );

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

void PushIntoGSP( wsf_gspdata *gspData, wsul nSize, wsul nOffset, wsul nSamps, wsfb nCh, wsfb nBit, char *cName );

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

int GetModType( char *cFile );

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

wsfb *SearchForString( wsfb *lpBuf, wsul nSize, char *cStr, int nTagLen );

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

CBaseLoader *GetBaseLoader( wsfb nModType );

//***************************************************************************** */
// CWSFModV1::CWSFModV1()
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

CWSFModV1::CWSFModV1()
{
	// Allocate new memories and stuff, and set loaded to 0
	m_nLoaded = 0;
	m_nModType = WSFM_NONE;
	m_fPack = NULL;

	m_wPack = new CWSFPackV1;

	memset(&m_wLD,0,sizeof(m_wLD));
	m_cModTitle[0] = 0;
}

//***************************************************************************** */
// CWSFModV1::~CWSFModV1()
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

CWSFModV1::~CWSFModV1()
{
	if (m_nLoaded)
		FreeMod();	// Free mod if loaded

	delete m_wPack;
}

//***************************************************************************** */
// CWSFModV1::SetPackName()
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

int CWSFModV1::SetPackName( char *cName )
{
	strncpy(m_cCurPack,cName,26);
	m_cCurPack[26] = 0;

	return 0;
}

//***************************************************************************** */
// CWSFModV1::LoadPack()
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

int CWSFModV1::LoadPack( char *cName )
{
	strncpy(m_cCurPack,cName,26);
	m_cCurPack[26] = 0;

	m_wPack->LoadPack(cName,NULL,0);

	return 0;
}

//***************************************************************************** */
// CWSFModV1::SavePack()
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

int CWSFModV1::SavePack( int nComp )
{
	return m_wPack->SavePack(m_cCurPack,(wsfb)nComp);
}

//////////////////////////////////////////////////////
//  CWSFModV1::ReplaceSamplesForSave()
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

int CWSFModV1::ReplaceSamplesForSave( wsf_modout *wOut, CBaseLoader *xBL, wsf_loaddata *wLD )
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

		// Replace Data
		if (wLD->bSamples[i].nSize > 1)
		{
			bNew = xBL->ReplaceData(bDat,nSize,&nNew,nSOff,4,wLD->bSamples[i].bSampData,wLD->bSamples[i].nSize);
			free(bDat);
			bDat = bNew;
			nSize = nNew;

			nSub += 4-wLD->bSamples[i].nSize;
		}
	}

	if (wLD->nSampleOffsets != nOffs)
		free(nOffs);

	wOut->bModData = bDat;
	wOut->nSize = nSize;

	return 0;
}

//***************************************************************************** */
// CWSFModV1::WriteMod()
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

int CWSFModV1::WriteMod( wsf_modout *wOut, int nWSF, long *nSCounts )
{
	int nr;
	CBaseLoader *xBL;
	// Calculate size :x
	wOut->bModData = (wsfb*)malloc(0);
	wOut->nSize = 0;

	if (nWSF){
		xBL = (CBaseLoader*)new CWSFLoaderV1;
		((CWSFLoaderV1*)xBL)->SetPack(m_cCurPack,m_wPack);
//		((CWSFLoaderV1*)xBL)->SetPack(m_fPack);

#ifdef WINWSF
		g_mStatus->SetSubProgressSteps((2+m_wPack->GetNumSamples())*m_wLD.nSamples);
#endif

		nr = ((CWSFLoaderV1*)xBL)->Save(wOut,&m_wLD,nSCounts);

		delete xBL;
	}
	else
	{
		xBL = GetBaseLoader(m_nModType);
		if (!xBL)
			return 1;

		// Do Replace Samples HERE
		ReplaceSamplesForSave(wOut,xBL,&m_wLD);		

		// Save
		nr = xBL->Save(wOut,&m_wLD);

		delete xBL;
	}

	return nr;
}

//***************************************************************************** */
// CWSFModV1::LoadMod()
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

int CWSFModV1::LoadMod( char *cFileName, int nFreePack, wsul *nOSamps )
{
	wsf_file *wf;
	int nR;
	wf = wsfopenfile(cFileName);

	if (!wf)
		return 1;

	nR = LoadMod(wf,nFreePack,nOSamps,GetModType(cFileName));

	wsfclose(wf);
	return nR;
}

int CWSFModV1::LoadMod( wsfb *bData, wsul nSize, int nFreePack, wsul *nOSamps, int nType )
{
	wsf_file *wf;
	int nR;
	wf = wsfopenmem(bData,nSize);

	if (!wf)
		return 1;

	nR = LoadMod(wf,nFreePack,nOSamps,nType);

	wsfclose(wf);
	return nR;
}

int CWSFModV1::LoadMod( wsf_file *wf, int nFreePack, wsul *nOSamps, int nType )
{
	int nR;
	CBaseLoader *xBL;
	wsfb *bMD;
	wsul nMD;

	FreeMod();

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
			((CWSFLoaderV1*)xBL)->SetPack(m_cCurPack,m_wPack);
			((CWSFLoaderV1*)xBL)->SetPack(m_fPack);
			((CWSFLoaderV1*)xBL)->SetFreePack(nFreePack);
		}

		nR = xBL->Load(bMD,nMD,&m_wLD);			
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
//  CWSFModV1::SetPassSum()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/5/2003 11:07:38 AM
/// \param nPassSum Password Sum
// 
//////////////////////////////////////////////////////

void CWSFModV1::SetPassSum( int nPassSum )
{
	m_wPack->SetPassSum(nPassSum);
}

//***************************************************************************** */
// CWSFModV1::FreeMod()
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

int CWSFModV1::FreeMod( void )
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
// CWSFModV1::FreePack()
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

int CWSFModV1::FreePack( void )
{
	m_cCurPack[0] = 0;
	return m_wPack->FreePack();
}
