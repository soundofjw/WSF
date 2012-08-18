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
/// \file WSFMod.h Main MOD classes and defs.

#ifndef __WSFMOD1H__
#define __WSFMOD1H__

#include <string.h>
#include "WSFGlob.h"
#include "WSFPackV1.h"
#include "WSFMod.h"

#ifndef NULL
#define NULL 0
#endif

#define WSFM1HEAD	"WSF1"			///< File Header for WSF format
#define WSFM1VERHI	1				///< Version of WSF currently supported vX.00
#define WSFM1VERLO	19				///< Version of WSF currently supported v1.XX

class CWSFMod;

/// Searches through a data buffer for a string
wsfb *SearchForString( wsfb *lpBuf, wsul nSize, char *cStr, int nTagLen );

/// MOD loading formats.
/// These match the g_cModExt array of file extensions.
typedef enum modvers1_e
{
	WSFM1_IT,		///< IT File Format
	WSFM1_XM,		///< XM File Format
	WSFM1_S3M,		///< S3M File Format
	WSFM1_MOD,		///< MOD File Format
	
	WSFM1_WSF,		///< WSF File Format (internal use only)
	WSFM1_WTF,		///< WTF File Format (internal use only)
	
	// Insert Formats Here

	WSFM1_COUNT,
	WSFM1_NONE

} WSFModVers1;

/// Get Mod Type
int GetModType( char *cFile );

/// Mod Format Extensions.
/// These match WSFModVers enum
extern char *g_cModExt[];
/// Mod Format Descriptions.
/// These match WSFModVers enum and g_cModExt
extern char *g_cModDesc[];
/// Mod Format Enablers.
/// These are flags to show whether or not the format is supported or not
extern wsfb g_bModFlag[];

wsfb *ReplaceData( wsfb *bIn, unsigned long nIn, unsigned long *nOut, unsigned long nRepOff, unsigned long nRepLength, wsfb *bFill, unsigned long nFillLen );

/// Preserves ModFusion v1 WSF format!
/// Handles Loading + Saving WSF modules.
/// Handles all the WSF module functions
/// for loading and saving
class CWSFLoaderV1 : public CBaseLoader
{
public:

	CWSFLoaderV1();

	/// Base Function for Loading
	int Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD );

	/// Base Saving Function
	int Save( wsf_modout *wOut, wsf_loaddata *wLD );

	// Additional Saving Unit
	int Save( wsf_modout *wOut, wsf_loaddata *wLD, long *nSCounts );

	/// Function to get pointers to ACTUAL samples
//	int GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData );

	/// Sets Pack Name (for saving)
	int SetPack( char *cName, CWSFPackV1 *wPack )
	{
		if (cName)
			strcpy(m_cCurPack,cName);
		if (wPack)
			m_wPack = wPack;

		return 0;
	};

	/// Gets Pack
	CWSFPackV1 *GetPack( void )
	{
		return m_wPack;
	};

	void SetPack( wsf_file *wPack );

	/// Sets the need to frees Pack. If false then it'll load all samples at once
	void SetFreePack( int nFree )
	{
		m_nFreePack = nFree;
	};

private:
	char m_cCurPack[200];	///< Current Pack Name for WSF
	CWSFPackV1	*m_wPack;	///< Sample Pack
	wsfb m_nModType;		///< Internal MOD Type (yea) WSFModVers enum
	int m_nFreePack;

	wsf_file *m_fPack;		///< Pack File Handle if set (:

};

///	Handles Main MOD and WSF functions in the ModFusion 1 Compatible Format.
///	Handles MOD writing and saving, along with
///	WSF commands. Also handles tags in the
/// ascii section.
/// 
class CWSFModV1
{
public:

	/// Loads Mod into buffer
	int LoadMod( char *cFileName, int nFreePack = 1, wsul *nOSamps = NULL );
	/// Loads Mod into buffer
	int LoadMod( wsfb *bData, wsul nSize, int nFreePack, wsul *nOSamps, int nType );
	/// Loads Mod into buffer
	int LoadMod( wsf_file *wf, int nFreePack, wsul *nOSamps, int nType );

	/// Frees Mod
	int FreeMod( void );

	/// Write Mod
	int WriteMod( wsf_modout *wOut, int nWSF, long *nSCounts = NULL );

	/// Save Pack
	int SavePack( int nComp );

	/// Free Pack
	int FreePack( void );

	/// Set Pack Name (for Saving WSF)
	int SetPackName( char *cName );

	/// Load Previously Begun Pack
	int LoadPack( char *cName );

	int ReplaceSamplesForSave( wsf_modout *wOut, CBaseLoader *xBL, wsf_loaddata *wLD );

	/// Gets Mod Type
	wsfb GetModType2( void )
	{
		return m_nModType;
	};

	/// Returns the ascii handler class
	CWSFAscii *GetAscii( void )
	{
		return &m_mAsc;
	};

	CWSFPackV1 *GetPack( void )
	{
		return m_wPack;
	};

	/// Sets pack with this, NULL to stop using wsf_file.
	void SetPack( wsf_file *wPack )
	{
		m_fPack = wPack;
	};

	CWSFModV1();
	~CWSFModV1();

	/// Sets password sum before needing to convert to WSF or back
	void SetPassSum( int nPassSum );

	char m_cModTitle[30];		///< Mod Title
	char m_cCurPack[200];		///< Current Pack Name for WSF

	wsf_loaddata m_wLD;		///< Load Data Structure

private:

	int	m_nLoaded;			///< MOD Loaded?
	wsfb m_nModType;		///< Mod Type from WSFModVers enum

	CWSFPackV1 *m_wPack;		///< Pack
	
	CWSFAscii	m_mAsc;		///< Ascii Handler

	wsf_file *m_fPack;		///< If not NULL, will call SetPack with this.
};

CBaseLoader *GetBaseLoader( wsfb nModType );
void FreeGSPData( wsf_gspdata *gspData );
void InitGSPData( wsf_gspdata *gspData );

/// Pushes values into gspData (ONLY IF YOU START WITH 0 SAMPLES AND MUST LOAD THROUGH)
void PushIntoGSP( wsf_gspdata *gspData, wsul nSize, wsul nOffset, wsul nSamps, wsfb nCh, wsfb nBit, char *cName );

#endif
