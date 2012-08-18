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
/// \file WSFPack.h Main Pack (WSP) classes and defs.

#ifndef __WSFPACK1H__
#define __WSFPACK1H__

#include "WSFGlob.h"
#include "WSFPack.h"

#define WSFP1HEAD	"WSP1"			///> WSP File Format Header
#define WSFP1VERHI		1			///> Version of WSP currently supported vX.00
#define WSFP1VERLO		29			///> Version of WSP currently supported v1.XX

#define SIGNATURESIZE	30			///> Size of Signature

/// A UByte Value used to show that the sample being loaded is blank anyway.
/// Saves some space in WSP files.
#define WSP_MAGICSKIPNUM 62

/// A UByte Value used when an extreme is found during CWSFPack::CreateSignature
/// This allows for those huge jumps to be ignored
#define WSFMAGICNUMBER	36

/// WSFPack Class, for handling .WSP files compatible with ModFusion 1.
/// Works with WSP Files.
class CWSFPackV1
{
public:

	/// Loads Sample Pack
	/// \param cFile File to Open
	/// \param nIDs Array pointing to specific IDs to open (to save memory) Can be NULL
	/// \param nNumIDs Number of IDs in nIDs array
	int LoadPack( char *cFile, wsul *nIDs, wsus nNumIDs );

	/// Loads Sample Pack
	int LoadPack( wsfb *bData, wsul nSize, wsul *nIDs, wsus nNumIDs );

	/// Loads Sample Pack
	int LoadPack( wsf_file *PFile, wsul *nIDs, wsus nNumIDs );

	/// Gets Sample by Sample Signature
	int GetSamp( wsf_sampout *wOut, char *cSampSig );	// Returns 1 if error :<

	/// Gets sample by ID
	int GetSamp( wsf_sampout *wOut, unsigned long nID, wsfb bFlag, wsul nPar, wsfb bAmp ); // Returns 1 if error :<

	/// Gets a samples size, even if its unloaded.
	wsul GetSampSize( wsul nID );

	/// Tests if sample exists, and if it does returns its ID
	wsul SampleExist( char *cSampSig, int nCh = -1, int nBit = -1 );		// Determines if a sample exists

	/// Frees Pack in memory
	int FreePack( void );

	/// Adds Sample
	/// Adds a sample to the pack. The wInSamp must already be specified.
	/// \param wInSamp Sample to add
	/// \param bSig PRE-CREATED signature. (See CWSFPack::CreateSignature)
	wsul AddSample( wsf_sampout *wInSamp, wsfb *bSig );

	/// Tests to see if a Sample Exists based on another sample (woah).
	wsul SampleExist( wsf_sampout *wInSamp, wsfb *bFlag, wsul *nPar, wsfb *nAmp, int nLimit = 0 );

	/// Tests to see if two samples are the same
	wsul SamplesMatch( wsf_sampout *wSampA, wsf_sample *wSampB, wsfb *bFlag, wsul *nPar, wsfb *nAmp, wsul nID, int nLimit = 0 );

	/// Creates Signature
	/// Creates a 30byte sample signature.
	/// This 'new' signature tests relationships between samples in a sample.
	/// The relationship therom would not be without Simon Pittock.
	/// \param wSamp Input Data
	/// \param nSize Size (IN BYTES) of sample
	/// \param nCh Number of channels
	/// \param nBit Bitrate, 8 or 16
	/// \param nSamps Number of Samples
	/// \param cOut Buffer that gets the signature
	int CreateSignature( wsfb *wSamp, wsul nSize, wsus nCh, wsus nBit, wsul nSamps, char *cOut );

	/// Creates 'OLD' Signature
	/// Oldstyle signatures are useful for samples that are less than 30 bytes
	/// \param wSamp Input Data
	/// \param nSize Size
	/// \param cOut Output Buffer that gets signature
	int MakeOldSignature( wsfb *wSamp, wsul nSize, char *cOut );

	/// Saves Pack
	/// (bComp = compression (1true,0false))
	int SavePack( char *cFile, wsfb bComp );		// bComp should be 0 for now
	/// Saves Pack
	int SavePack( wsfb **bData, wsul *nSize, wsfb bComp );		// bComp should be 0 for now
	/// Saves Pack
	int SavePack( wsf_file *PFile, wsfb bComp );

	/// DeCompresses Sample
	int DeCompressSample( wsfb *bCompSamp, unsigned long nCompSize, wsfb **bOutSamp, unsigned long nFullSize );
	/// Compresses Sample
	int CompressSample( wsfb *bFullSamp, unsigned long nFullSize, wsfb **bOutSamp, unsigned long *nCompSize );

	/// Makes percentage out of two integers
	/// \param nLow Low UL (50 out of 100 = 50)
	/// \param nHi Hi UL (50 out of 100 = 100)
	unsigned long MakePercent( wsul nLow, wsul nHi );

	/// Gets Number of Samples
	unsigned long GetNumSamples( void )
	{
		return m_nSamples;
	};

	void SetSampleName( wsul nID, char *cName );
	void GetSampleName( wsul nID, char *cOut );

	void SetCompressionFlag( wsul nID, wsfb nOgg );

	/// Encodes a buffer based on a bit
	void EncodeData( wsfb *cStr, int nEbit, wsul nLen );

	/// Decodes a buffer based on a bit
	void DecodeData( wsfb *cStr, int nEbit, wsul nLen );

	/// Sets the password sum for saving or loading (either works ^^;)
	void SetPassSum( int nPassSum );

	// Stuffs

	/// Init
	CWSFPackV1();
	/// Kill
	~CWSFPackV1();

	/// Needs password
	wsfb m_nNeedPass;

private:

	int m_nPassSum;				///< Password sum

	int m_nLoaded;				// Loaded?

	unsigned long m_nSamples;	// Number of Samples
	wsf_sample *m_wSamples;		// Samples	

};

// generates password sum
int GenerateSum(char *code, int size);

int OggEncode( wsul nSize, wsfb *bData, wsul nCh, int nBit, wsf_file *wOut, wsul nFreq );
int OggDecode( wsul nSize, wsfb *bData, int nBit, wsf_file *wOut );

#endif
