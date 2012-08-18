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
/// \file WGMPack.cpp Whelchel Grouped Media handler

#include "WSFGlob.h"
#include "WGMPack.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#define xstrnicmp _strnicmp
#else
#define xstrnicmp strncasecmp
#endif

#ifndef strlwr
#include <ctype.h>
extern char *strlwr( char *str );
#endif

//////////////////////////////////////////////////////
//  CWGMPack::CWGMPack()
// 
//  Description: 
/// Constructor for CWGMPack class
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:09:37 PM
//////////////////////////////////////////////////////

CWGMPack::CWGMPack()
{
	m_nNumUnits = 0;
	m_wUnits = (wgmunit*)malloc(0);

	memset(m_cInfoString,0,100);
	strcpy(m_cInfoString,"Syntesis Entertainment: http://wsf.syntesis.ath.cx");
}

//////////////////////////////////////////////////////
//  CWGMPack::~CWGMPack()
// 
//  Description: 
/// Deconstructs CWGMPack class
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:10:07 PM
//////////////////////////////////////////////////////

CWGMPack::~CWGMPack()
{
	EmptyPack();
	m_nNumUnits = 0;
	free(m_wUnits);
}

//////////////////////////////////////////////////////
//  CWGMPack::AddUnit()
// 
//  Description: 
/// Adds unit to pack
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:10:26 PM
/// \param nSize Size of unit
/// \param *bData Unit data buffer
/// \param *cID String ID (up to 8 bytes) of data
/// \param nType Type of Unit
/// \param nOff Offset in file to unit
/// \return ID of unit
/// 
//////////////////////////////////////////////////////

wsul CWGMPack::AddUnit( wsul nSize, wsfb *bData, char *cID, wsfb nType, wsul nOff )
{
	wgmunit *wUnit;

	m_nNumUnits++;
	m_wUnits = (wgmunit*)realloc(m_wUnits,m_nNumUnits*sizeof(wgmunit));

	wUnit = &m_wUnits[m_nNumUnits-1];
	
	wUnit->nType = nType;
	wUnit->nSize = nSize;
	wUnit->nOffset = nOff;

	if (bData){
		wUnit->bData = (wsfb*)malloc(nSize);
		memcpy(wUnit->bData,bData,nSize);
	}else
		wUnit->bData = NULL;

	memset(wUnit->cID,0,8);
	strncpy(wUnit->cID,cID,8);

	return m_nNumUnits-1;
}

//////////////////////////////////////////////////////
//  CWGMPack::AddUnitFile()
// 
//  Description: 
/// Adds unit to pack from a file
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:48:54 PM
/// \param *cID ID for file
/// \param *cFile File to open and add
/// \return ID of unit (666 for error, or it may be an ID, you cannot tell :<)
/// 
//////////////////////////////////////////////////////

wsul CWGMPack::AddUnitFile( char *cID, char *cFile, wsfb nType )
{
	wsf_file *pFile;
	wsul nRet;
	wsul nSize;
	wsfb *bData;

	pFile = wsfopenfile(cFile);
	if (!pFile)
		return 666;

	nSize = pFile->nSize;

	bData = (wsfb*)malloc(nSize);
	wsfread(bData,nSize,pFile);

	wsfclose(pFile);

	nRet = AddUnit(nSize,bData,cID,nType);
	free(bData);
	
	return nRet;
}

//////////////////////////////////////////////////////
//  CWGMPack::GetUnit()
// 
//  Description: 
/// Retrieves unit for string ID
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:13:09 PM
/// \param *cID String ID (up to 8 bytes)
/// \param **wUnit optional pointer to wgmunit pointer to be filled with the unit
/// \return ID of unit (666 if none found, sorry if ID is 666 anyway :P)
/// 
//////////////////////////////////////////////////////

wsul CWGMPack::GetUnit( char *cID, wgmunit **wUnit )
{
	wsul i;

	for (i=0;i<m_nNumUnits;i++)
	{
		wgmunit *bwUnit;
		bwUnit = &m_wUnits[i];
		
		if (xstrnicmp(bwUnit->cID,cID,8)==0)
		{
			if (wUnit)
				wUnit[0] = bwUnit;
			return i;
		}
	}

	if (wUnit)
		wUnit[0] = NULL;

	return 666;
}

//////////////////////////////////////////////////////
//  CWGMPack::GetUnitForType()
// 
//  Description: 
/// Gets unit with nType type, gets 0based entry off of nPackNum
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 9:13:57 PM
/// \param nWhich Which unit to get, 0based
/// \param nType Type of unit to get
/// \return ID of (nPackNum)st (0base) unit w/ nType type (666 for none, or just high ID :<)
/// 
//////////////////////////////////////////////////////

wsul CWGMPack::GetUnitForType( wsul nWhich, wsfb nType )
{
	wsul i;
	wsul x=0;

	for (i=0;i<m_nNumUnits;i++)
	{
		wgmunit *bwUnit;
		bwUnit = &m_wUnits[i];
		
		if (bwUnit->nType == nType)
		{
			if (x==nWhich)
				return i;
			else
				x++;
		}
	}

	return 666;
}

//////////////////////////////////////////////////////
//  CWGMPack::GetUnitPackID()
// 
//  Description: 
/// Gets unit with WGMT_PACK type, gets 0based entry off of nPackNum
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 9:11:43 PM
/// \param nPackNum which pack to get, 0based
/// \return ID of (nPackNum)st (0base) unit w/ WGMT_PACK type (666 for none, or just high ID :<)
/// 
//////////////////////////////////////////////////////

wsul CWGMPack::GetUnitPackID( wsul nPackNum )
{
	return GetUnitForType(nPackNum,WGMT_PACK);
}

//////////////////////////////////////////////////////
//  CWGMPack::EmptyPack()
// 
//  Description: 
/// Empties Pack
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:15:10 PM
//////////////////////////////////////////////////////

void CWGMPack::EmptyPack( void )
{
	wsul i;
	for (i=0;i<m_nNumUnits;i++)
	{
		wgmunit *bwUnit;
		bwUnit = &m_wUnits[i];

		if (bwUnit->bData)
			free(bwUnit->bData);
		bwUnit->bData = NULL;
	}
	m_wUnits = (wgmunit*)realloc(m_wUnits,0);
	m_nNumUnits = 0;
	return;
}

//////////////////////////////////////////////////////
//  CWGMPack::RemUnit()
// 
//  Description: 
/// Removes unit from pack
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:19:40 PM
/// \param nID ID of unit to remove
//////////////////////////////////////////////////////

void CWGMPack::RemUnit( wsul nID )
{	
	wgmunit *wUnits;
	wsul i;
	wsul x;

	wUnits = (wgmunit*)malloc(sizeof(wgmunit)*(m_nNumUnits-1));

	x=0;
	i=0;
	for (i=0,x=0;i<m_nNumUnits;i++)
	{
		// i = input, x = output
		
		if (i != nID){
			memcpy(&wUnits[x],&m_wUnits[i],sizeof(wgmunit));
			x++;
		}
		else{
			if (m_wUnits[i].bData)
				free(m_wUnits[i].bData);
			m_wUnits[i].cID[0] = 0;
		}
	}

	m_nNumUnits = x;

	// Replace
	free(m_wUnits);
	m_wUnits = wUnits;

	return;
}

//////////////////////////////////////////////////////
//  CWGMPack::SavePack()
// 
//  Description: 
/// Saves Pack to a file
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:21:29 PM
/// \param *cFileName Filename
/// \return nonzero for error
//////////////////////////////////////////////////////

int CWGMPack::SavePack( char *cFileName )
{
	wsf_file *pFile;
	wsfb bHi,bLo;
	wsul i;

	pFile = wsfcreatefile(cFileName);
	if (!pFile)
		return 1;

	bHi = WGMVERHI;
	bLo = WGMVERLO;

	// Write Header
	wsfwrite((void*)WGMHEAD,4,pFile);

	// Write Version
	wsfwrite(&bHi,1,pFile);
	wsfwrite(&bLo,1,pFile);

	// Write Info String
	wsfwrite(m_cInfoString,100,pFile);

	// Write Number of Units
	wsfwrite(&m_nNumUnits,sizeof(wsul),pFile);

	for (i=0;i<m_nNumUnits;i++)
	{
		wgmunit *wUnit;
		wUnit = &m_wUnits[i];

		// Write Tpye
		wsfwrite(&wUnit->nType,1,pFile);

		// Write string id and size
		wsfwrite(wUnit->cID,8,pFile);
		wsfwrite(&wUnit->nSize,sizeof(wsul),pFile);

		// Write data
		if (wUnit->bData)
			wsfwrite(wUnit->bData,wUnit->nSize,pFile);
		else{
			wsfb *bTempBuf;
			bTempBuf = (wsfb*)malloc(wUnit->nSize);
			memset(bTempBuf,0,wUnit->nSize);
			wsfwrite(bTempBuf,wUnit->nSize,pFile);
			free(bTempBuf);			
		}
	}

	// Close
	wsfclose(pFile);

	// Saved (:
	return 0;
}

//////////////////////////////////////////////////////
//  CWGMPack::LoadPack()
// 
//  Description: 
/// Loads pack from file (it actually appends!)
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:32:49 PM
/// \param *cFileName Pack Filename
/// \param nIDCount Number of IDs of units you want to load data for, 0 to load all
/// \param nIDs Array of wsul of nIDCount length that specify the IDs of units you wish to load
/// \return nonzero for error
/// 
//////////////////////////////////////////////////////

int CWGMPack::LoadPack( char *cFileName, wsul nIDCount, wsul *nIDs )
{
	wsf_file *pFile;
	wsfb bHi,bLo;
	wsul i;
	wsul nNum;
	wsfb nF;
	wsul x;
	char cHead[4];

	pFile = wsfopenfile(cFileName);
	if (!pFile)
		return 1;

	// Read Header
	wsfread(cHead,4,pFile);

	// Bad Header?
	if (xstrnicmp(WGMHEAD,cHead,4)!=0){
		wsfclose(pFile);
		return 1;
	}

	// Read Version
	wsfread(&bHi,1,pFile);
	wsfread(&bLo,1,pFile);

	// Read Info String
	wsfread(m_cInfoString,100,pFile);

	// Read Number of Units
	wsfread(&nNum,sizeof(wsul),pFile);

	for (i=0;i<nNum;i++)
	{
		wgmunit wUnit;

		nF = 0;
		if (nIDs)
		{
			for (x=0;x<nIDCount;x++){
				if (nIDs[x] == i){
					nF = 1;
					break;
				}
			}
		}else
			nF = 1;

		// Read Type
		wsfread(&wUnit.nType,1,pFile);
		
		// Read string id and size
		wsfread(wUnit.cID,8,pFile);
		wsfread(&wUnit.nSize,sizeof(wsul),pFile);
		wUnit.bData = NULL;

		// Alloc + Read data

		wUnit.nOffset = pFile->nPos;
		if (nF == 1){
			wUnit.bData = (wsfb*)malloc(wUnit.nSize);
			wsfread(wUnit.bData,wUnit.nSize,pFile);
		}else
			wsfseek(wUnit.nSize,pFile);

		// Save
		AddUnit(wUnit.nSize,wUnit.bData,wUnit.cID,wUnit.nType,wUnit.nOffset);

		if (nF)
			free(wUnit.bData);
	}

	// Close
	wsfclose(pFile);

	// Loaded (:
	return 0;
}

//////////////////////////////////////////////////////
//  *CWGMPack::GetUnit()
// 
//  Description: 
/// Returns unit for ID
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:57:59 PM
/// \param nID Numeral ID
/// \return unit
/// 
//////////////////////////////////////////////////////

wgmunit *CWGMPack::GetUnit( wsul nID )
{
	if (nID >= 0 && nID < m_nNumUnits)
		return &m_wUnits[nID];
	return NULL;
}

//////////////////////////////////////////////////////
//  CWGMPack::SaveUnitToFile()
// 
//  Description: 
/// Saves unit to file, name will be autogenerated if no name generated.
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 9:51:41 PM
/// \param nID ID
/// \param *cFile Filename to use 
//////////////////////////////////////////////////////

void CWGMPack::SaveUnitToFile( wsul nID, char *cFile )
{
	wgmunit *wUnit;
	wsf_file *pFile;
	char cFName[400];
	char *cDot;
	char *cExt;

	wUnit = GetUnit(nID);

	if (!wUnit)
		return;
	if (!wUnit->bData)
		return;

	if (!cFile)
	{
		strncpy(cFName,wUnit->cID,8);
		cDot = strchr(cFName,'.');

		if (cDot)
			cDot[0] = 0;

		if (wUnit->nType == WGMT_TUNE)
			cExt = "wtf";
		else if (wUnit->nType == WGMT_PACK)
			cExt = "wsp";
		else
			cExt = "txt";

		sprintf(cFName,"%s.%s",cFName,cExt);
	}
	else
		strcpy(cFName,cFile);

	m_cLastFile[0] = 0;
	pFile = wsfcreatefile(cFName);

	if (!pFile)
		return;

	wsfwrite(wUnit->bData,wUnit->nSize,pFile);
	wsfclose(pFile);

	strcpy(m_cLastFile,cFName);

	return;
}
