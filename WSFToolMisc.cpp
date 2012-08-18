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

/// \file WSFToolMisc.cpp 
/// Misc Functions for WSFTool

/*! \addtogroup WSFTool
   *  A suite of features to convert WSF's to MOD's and vice versa
   *  Built from WSFTool.cpp
   *  @{
   */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//#include <io.h>
#include "WSFGlob.h"
#include "WSFTool.h"

#ifndef stricmp
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

//*****************************************************************************
// EmptyID3()
// 
// Purpose: Empties an ID3 Tag
// 
// Author : [Josh Whelchel], Created : [8/10/2002 9:03:13 PM]
// Parameters:	
// 
// [*iTag] :
// 
// Returns:	none
// 
//*****************************************************************************

void EmptyID3( id3tag *iTag )
{
	iTag->bGenre = 0;
	
	memset(iTag->cAlbum,0,30);
	memset(iTag->cArtist,0,30);
	memset(iTag->cTitle,0,30);
	memset(iTag->cYear,0,4);
	
	iTag->nTrack = 0;

	return;
}

//*****************************************************************************
// *SaveID3()
// 
// Purpose: Saves ID3 Tag
// 
// Author : [Josh Whelchel], Created : [8/10/2002 11:23:00 PM]
// Parameters:	
// 
// [*iTag] : Tag
// [*bMP3] : Mp3
// [nLen] : Length
// 
// Returns:	char
// 
//*****************************************************************************

char *SaveID3( id3tag *iTag, char *bMP3, int *nLen )
{
	int nSize;
	char *cCur;
	char *cBuf;

	nSize = *nLen;
	cBuf = bMP3;

	// Go to where the ID3 SHOULD start.
	cCur = cBuf+(nSize-128);

	if (cCur >= cBuf && cCur[0] == 'T' && cCur[1] == 'A' && cCur[2] == 'G')
		cCur += 3;
	else
	{
		nSize += 128;
		// Make more space to add tag (:
		cBuf = (char*)realloc(cBuf,nSize);

		cCur = cBuf+(nSize-128);

		cCur[0] = 'T';
		cCur[1] = 'A';
		cCur[2] = 'G';

		cCur += 3;
	}

	// Title
	strncpy(cCur,iTag->cTitle,30);
	cCur += 30;

	// Artist
	strncpy(cCur,iTag->cArtist,30);
	cCur += 30;

	// Album
	strncpy(cCur,iTag->cAlbum,30);
	cCur += 30;

	// Year
	strncpy(cCur,iTag->cYear,4);
	cCur += 4;

	// Get Track from Commment? V1.1 spec.
	if (iTag->cComment[28] == 0)
		iTag->cComment[29] = iTag->nTrack;

	// Comment
	strncpy(cCur,iTag->cComment,30);
	cCur += 30;

	*nLen = nSize;

	return cBuf;
}

//*****************************************************************************
// LoadID3()
// 
// Purpose: Loads an ID3 tag
// 
// Author : [Josh Whelchel], Created : [8/10/2002 9:01:30 PM]
// Parameters:	
// 
// [*iTag] :
// [*bMP3] :
// [nLen] :
// 
// Returns:	none
// 
//*****************************************************************************

void LoadID3( id3tag *iTag, char *bMP3, int nLen )
{
	char *cCur;

	// Go to where the ID3 SHOULD start.
	cCur = bMP3+(nLen-128);

	iTag->bTag = FALSE;

	EmptyID3(iTag);

	if (cCur[0] == 'T' && cCur[1] == 'A' && cCur[2] == 'G')
	{
		iTag->bTag = TRUE;

		// Load the Tag
		cCur += 3;

		// Title! :)
		strncpy(iTag->cTitle,cCur,30);
		iTag->cTitle[30] = '\0';
		cCur+=30;

		// Artist
		strncpy(iTag->cArtist,cCur,30);
		iTag->cArtist[30] = '\0';
		cCur+=30;

		// Album
		strncpy(iTag->cAlbum,cCur,30);
		iTag->cAlbum[30] = '\0';
		cCur+=30;

		// Year
		strncpy(iTag->cYear,cCur,4);
		iTag->cYear[5] = '\0';
		cCur+=4;

		// Comment
		strncpy(iTag->cComment,cCur,30);
		iTag->cComment[30] = '\0';
		cCur+=30;

		// Get Track from Commment? V1.1 spec.
		if (iTag->cComment[28] == 0)
			iTag->nTrack = iTag->cComment[29];
		else
			iTag->nTrack = 0;

		// Genre
		iTag->bGenre = cCur[0];

		// Thats the end.
	}

	return;
}

//*****************************************************************************
// IsInCustom()
// 
// Purpose: Tests if character is within two custom characters
// 
// Author : [Josh Whelchel], Created : [7/5/2002 7:30:28 PM]
// Parameters:	
// 
// [*cChr] :
// [*cStr] :
// [cCustom] :
// [cCustom2] :
// 
// Returns:	int
// 
//*****************************************************************************

int IsInCustom( char *cChr, char *cStr, char cCustom, char cCustom2 )
{
	int nRet;
	char *cPos;
	int nLevel;

	cPos = cStr;
	nRet = FALSE;

	nLevel = 0;
	while (*cPos != '\0' && cPos != cChr )
	{
		// If they are the same treat like you would '"'
		if (cCustom==cCustom2)
		{
			if (*cPos == cCustom)
				nRet = (!nRet);
		}
		else
		{
			// If its in its in, otherwise it ain't. XDD
			if (cPos[0] == cCustom)
				nLevel++;

			if (cPos[0] == cCustom2)
				nLevel--;
		}


		if (cPos == cChr)
			break;

		if (*cPos == '\0')
			break;

		cPos++;
	}

	// Return based on type
	if (cCustom==cCustom2)	// Quote mode
		return nRet;
	else					// () mode
		return (nLevel>0);
}

//*****************************************************************************
// IsInQuotes()
// 
// Purpose: Tests if character is in quotes
// 
// Author : [Josh Whelchel], Created : [7/5/2002 7:30:20 PM]
// Parameters:	
// 
// [*cChr] :
// [*cStr] :
// 
// Returns:	int
// 
//*****************************************************************************

int IsInQuotes( char *cChr, char *cStr )
{
	return IsInCustom(cChr,cStr,'"','"');
}

//////////////////////////////////////////////////////
//  FillSettings()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 8:35:17 AM
/// \param cCmd Command Line
/// \param wS Settings to Fill
/// \brief Fills settings for a command line
/// 
// 
//////////////////////////////////////////////////////

void FillSettings( char *cCmd, WSFTSets *wS )
{
	int i;
	char *cStart;
	char *cSpace;
	int nQuotes=0;
	
	i=0;

	// Set Start
	cStart = cCmd;
	
	// Find first Space
	cSpace = strchr(cStart,' ');

	// As long as there is a space...
	while (1)
	{
		char bh;
		if (cSpace==NULL){
			// Look for end character
			cSpace = strchr(cCmd,'\0');

			if (cSpace!=NULL)
			{
				FillSetting(cStart,i,wS);
				i++;
				break;
			}

			break; // No Space, kill it boi.
		}

		// Test to see if our space is in quotes
		if (IsInQuotes(cSpace,cStart)==TRUE)
		{
			cSpace = strchr(cSpace+1,' ');
			nQuotes=1;
			continue;
		}

		// Null out the space
		bh = cSpace[0];
		cSpace[0] = '\0';
		FillSetting(cStart,i,wS);
		i++;
		cSpace[0] = bh;

		cStart = cSpace+1;
		cSpace = strchr(cStart,' ');

		nQuotes=0;
	}

	return;
}

//*****************************************************************************
// *CSBrain::StrClip()
// 
// Purpose: Clips a string
// 
// Author : [Josh Whelchel], Created : [7/5/2002 7:36:53 PM]
// Parameters:	
// 
// [*str] :
// 
// Returns:	char
// 
//*****************************************************************************

char *StrClip(char *str)
{
	char *ss;

	ss = &str[strlen(str)-1];
	while (*ss == ' ' || *ss == '\n' || *ss == '\r')
	{
		*ss = '\0';
		ss--;
	}

	return ss;
}

/*!
	}@
*/

