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
/// \file WSFSpecialTags.h
/// Contains functions to control Special Tags (like SEcho)


#ifndef WSFSTAGS
#define WSFSTAGS

/// Structure that contains the Special Tag settings for use with CWSFTagHandler
typedef struct wsf_stags_s
{
	double dSEcho;		///< Echo for SEcho



} wsf_stags;

#endif

#ifndef __WSFSPECIALTAGSH__
#define __WSFSPECIALTAGSH__

#include "WSFGlob.h"
#include "WSFMod.h"

/// This just helps us prevent clipping.
/// Right now we are just using the value of the anti-amp. This is bad.
#define LIMIECHO		dR

/// Class to handle Special Tags like 'SEcho'
class CWSFSTagHandler
{
public:

	CWSFSTagHandler();
	~CWSFSTagHandler();

	// Load Tags from file
	int LoadTags( wsf_file *wf );
	int LoadTags( char *fn );

	// Returns the settings buffer (you can modify)
	wsf_stags *GetSettings( void );
	
	// Resets the settings only
	void ResetSettings( void );

	void InitDSPEffects( int nBitRate, int nCh, int nSRate );

	// Process DSP Effects
	void ProcessDSPEffects( int **bSmpPtr, long nSamples,
		int nCh, int nBitRate, int nSRate );

	// Simple Echo Processer
	void RunSEcho( int *bSmpPtr, long nSamples,
		int nCh, int nBitRate );

	void Reset( void );

private:

	int			*m_interw;
	wsf_stags	m_wSTags;

	int echoctr;
	int *echo[10];
	double inpct,outpct,echopct;
};

#endif
