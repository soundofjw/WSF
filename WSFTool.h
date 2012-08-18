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

/// \file WSFTool.h 
/// Main Header for WSFTool.cpp and WSFToolMisc.cpp

/// Settings Structure for WSFTool
typedef struct wsft_s
{
	/// Mode for Running in WSFTool of the WSFTModes enum
	int nMode;

	/// Input Files
	char cInput[200][200];
	int nInputs;

	// Arguments

	/// Argument 1.
	/// TOWSF = Pack Name
	char cArg1[100];

	/// Argument 2.
	/// unused
	char cArg2[100];

	/// Output Format.
	char cOutputFormat[100];

	/// Password (input from -M or for extracting w/ -W)
	wsul nPassSum;

	/// Min Ogg Size (0 for none)
	wsul nOggSize;

	/// Number Arg
	/// MP3:
	/// 0 - bitrate, 1 - quality
	long nArg[20];

	/// Number of Tags
	int nTags;

	/// Tags
	char **cTags;

	/// Values
	char **cVals;

} WSFTSets;

typedef struct id3tag_s
{
	char bTag;

	char cTitle[31];
	char cArtist[31];
	char cAlbum[31];
	char cYear[5];
	char cComment[31];
	
	char bGenre;

	char nTrack;
	char cFile[200];


} id3tag;

void FillSetting( char *cLine, int nNum, WSFTSets *wS );

/// Takes a command line and fills in wS from it.
/// \param wS Settings
/// \param cCmd Full Command Line
void FillSettings( char *cCmd, WSFTSets *wS );

/// Tests if character is in quotes in a string
int IsInQuotes( char *cChr, char *cStr );

/// Tests if character is within two characters in a string
int IsInCustom( char *cChr, char *cStr, char cCustom, char cCustom2 );

/// Clips a string
char *StrClip(char *str);

/// Saves an ID3 Tag
char *SaveID3( id3tag *iTag, char *bMP3, int *nLen );
/// Loads an ID3 Tag
void LoadID3( id3tag *iTag, char *bMP3, int nLen );
/// Empties an ID3 Tag
void EmptyID3( id3tag *iTag );

#define FALSE	0	///< FALSE FLAG
#define TRUE	1	///< TRUE FLAG
