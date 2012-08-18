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
/// \file WGMPack.h Whelchel Grouped Media Header

#ifndef __WGMPACKH__
#define __WGMPACKH__

#include <string.h>
#include "WSFGlob.h"
#include "WSFPack.h"

#ifndef NULL
#define NULL 0
#endif

#define WGMHEAD	"WGM1"			///< File Header for WGM format
#define WGMVERHI	1			///< Version of WGM currently supported vX.00
#define WGMVERLO	0			///< Version of WGM currently supported v1.XX

/// Enumeration for unit types
typedef enum wgmtype_e
{
	WGMT_ZERO=0,				///< ZERO IDENTIFIER

	WGMT_TUNE,					///< Tune File
	WGMT_PACK,					///< Pack File (should only be one!)

	WGMT_COMMENT,				///< Comment File
	WGMT_OTHER,					///< Misc File

	WGMT_COUNT

} wgmtype;

/// Grouped Media Unit Structure
typedef struct wgmunit_s
{
	/// Data Buffer
	wsfb *bData;

	/// Size of data
	wsul nSize;

	/// 8 Byte String ID of music
	char cID[8];

	/// Type of unit, should only be one WGMT_PACK. Of the wgmtype_e enums
	wsfb nType;

	/// Offset of bData in the file
	wsul nOffset;

} wgmunit;

/// Handles the grouped media pack files
class CWGMPack
{

public:
	/// Inits the class
	CWGMPack();

	/// Deconstructs the class
	~CWGMPack();

	/// Adds a WTF/WSF to the group. Returns ID (0based)
	wsul AddUnit( wsul nSize, wsfb *bData, char *cID, wsfb nType = WGMT_TUNE, wsul nOff = 0 );

	/// Retrieves ID of unit for a string id, optionally returns pointer to the unit as well
	wsul GetUnit( char *cID, wgmunit **wUnit );

	/// Retrieves pointer to unit for ID
	wgmunit *GetUnit( wsul nID );

	/// Removes a WTF/WSF from the group.
	void RemUnit( wsul nID );

	/// Removes all units
	void EmptyPack( void );

	/// Saves unit to file, name will be auto generated if cFile is left NULL
	void SaveUnitToFile( wsul nID, char *cFile = NULL );

	/// Gets the filename used in last call to SaveUnitToFile
	void GetLastFile( char *cBuf )
	{
		strcpy(cBuf,m_cLastFile);
	};

	/// Adds a WTF/WSF to the group, specifed as filename, returns ID (0based)
	wsul AddUnitFile( char *cID, char *cFile, wsfb nType = WGMT_TUNE );

	/// Retrieves number of units
	wsul GetNumUnits( void )
	{
		return m_nNumUnits;
	};

	/// Sets Copyright/Company Info String
	void SetInfo( char *cInfo )
	{
		strncpy(m_cInfoString,cInfo,99);
	};

	/// Copies Copyright/Company Info String into cInfoBuf
	void GetInfo( char *cInfoBuf )
	{
		strcpy(cInfoBuf,m_cInfoString);
	};

	/// Loads Pack
	int LoadPack( char *cFileName, wsul nIDCount = 0, wsul *nIDs = NULL );

	/// Saves Pack
	int SavePack( char *cFileName );

	/// Gets the ID of the unit with WGMT_PACK type. If there is more than one, use nPackNum to specify which to get (0 based)
	wsul GetUnitPackID( wsul nPackNum = 0 );

	/// Gets the ID of the unit with nType. If there is more than one use nPackNum to specify which to get (0 based)
	wsul GetUnitForType( wsul nWhich, wsfb nType );
	
private:

	/// Number of units in pack
	wsul m_nNumUnits;

	/// Units
	wgmunit *m_wUnits;

	/// Copyright/Company Info String
	char m_cInfoString[100];

	/// Last used filename in call to SaveUnitToFile
	char m_cLastFile[200];

};

#endif
