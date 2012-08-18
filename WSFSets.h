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

/// \file WSFSets.h
/// Contains classes: CWSFSets

#include <windows.h>

/// Settings Structure
/// Includes all important settings for WSF playback
typedef struct WSFSettings_s
{
	int nDisableSEcho;			///< Use built-in reverb
	int nNoFreePack;			///< Dont free pack after loading it (NO PURPOSE)
	int nMono;					///< Play in Mono?
	int nLoBit;					///< Play in 8Bit?
	int nLoadLen;				///< Load Lengths?
	char cTitle[100];			///< Title Display
} WSFSettings;

/// Controls Registry Settings
class CWSFSets
{
public:

	/// Saves Settings
	void SaveSettings( void );

	/// Loads Settings
	void LoadSettings( void );

	/// Gets Settings
	WSFSettings *GetSettings( void )
	{
		return &m_wSets;
	}

	/// Opens Key
	HKEY OpenKey( HKEY hP, char *cAdd );

	/// Constructor
	CWSFSets();

private:

	/// Settings
	WSFSettings m_wSets;

};