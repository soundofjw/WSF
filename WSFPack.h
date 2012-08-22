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

#ifndef __WSFPACKH__
#define __WSFPACKH__

#include "WSFGlob.h"

#define WSFPHEAD	"WSP1"			///< WSP File Format Header
#define WSFPVERHI		2			///< Version of WSP currently supported vX.00
#define WSFPVERLO		03			///< Version of WSP currently supported v1.XX

#define SIGNATURESIZE	30			///< Size of Sample Signature

#define WSFKBDIFF		2			///< KB Difference range for comparing samples.

/// A UByte Value used to show that the sample being loaded is blank anyway.
/// Saves some space in WSP files.
#define WSP_MAGICSKIPNUM 62

/// A UByte Value used when an extreme is found during CWSFPack::CreateSignature
/// This allows for those huge jumps to be ignored
#define WSFMAGICNUMBER	36

/// This is the enumeration for the flags of samples in WSFs
typedef enum wsfmodflag_e
{
	WSFS_NORMAL=0,			///< Use original Sample
	WSFS_REVERSED,			///< Reverse the sample

	WSFS_SAMPCUTOFF,		///< Use the sample only until cutoff (p1=cutoff)
	WSFS_SAMPAFTER,			///< Add end of sample after this one (p1=sampid)

	WSFS_REVSAMPCUTOFF,		///< Use the sample only until cutoff (p1=cutoff) REVERSED

	WSFS_COUNT
} WSFFlag;	

/// Structure used in CWSFLoader for Version 2.0 WSFMOD.
typedef struct wsf2samp_s
{
	wsul nID;		///< ID in Sample Pack (CWSFPack (WSP))
	wsul nSize;		///< Size of sample in Bytes (Do we need this?)

	wsfb bFlag;		///< Flag from WSFFlag enumeration

	wsul nPar1;		///< Parameter One
	wsul nPar2;		///< Parameter Two

	float fAmp;		///< Amplification

} WSF2Samp;

/// WSF2 Sample Array structure.
typedef struct wsf2samparray_s
{
	wsul nSamps;	///< Number of samples in the SampleArray
	WSF2Samp *bSamps; ///< Sample Array of WSF2Samp

	wsul nIDs;		///< Number of Samples in the MOD
	wsul *bIDs;		///< Sample References to bSamps

#ifdef WINWSF
	int nNoRender;	///< Variable that specifies whether to render the WINWSF stuff or not.
	wsul nSamplesTotal; ///< Total number of samples in the mod (Only used in WINWSF)
#endif
	wsul nCurID;	///< ID of the currently tested sample. :0
	int *nSCounts; ///< Used to count the total number of each type. :D See CWSFLoader::Save()

} WSF2SampArray;

/// Typedef for WSF2SampArray pointer
typedef WSF2SampArray *WSF2Samps;

/// WSFPack Class, for handling .WSP files
/// Works with WSP Files
class CWSFPack
{
public:

	/// Loads Sample Pack
	/// \param cFile File to Open
	/// \param nIDs Array pointing to specific IDs to open (to save memory) Can be NULL
	/// \param nNumIDs Number of IDs in nIDs array
	int LoadPack( char *cFile, wsul *nIDs, wsul nNumIDs, int nNoBulk = 0 );

	/// Loads Sample Pack
	int LoadPack( wsfb *bData, wsul nSize, wsul *nIDs, wsul nNumIDs, int nNoBulk = 0 );

	/// Loads Sample Pack
	int LoadPack( wsf_file *PFile, wsul *nIDs, wsul nNumIDs, int nNoBulk = 0, int nNoBegin = 0 );

	/// Loads Sample Pack Data for Ver 1.99 and earlier
	int LoadPackV1( wsf_file *PFile, wsul *nIDs, wsul nNumIDs, int nNoBulk, wsfb bComp );

	/// Loads Sample Pack Data for Ver 2.00 and later
	int LoadPackV2( wsf_file *PFile, wsul *nIDs, wsul nNumIDs, int nNoBulk, wsfb bComp );

	/// Gets Sample by Sample Signature
	int GetSamp( wsf_sampout *wOut, char *cSampSig );	// Returns 1 if error :<

	/// Gets sample by ID. Use with Version 1!
	/// \return 0 if no error, 1 if error, -1 if loaded sample but NO SAMPLE DATA
	int GetSampV2( wsf_sampout *wOut, unsigned int nID, wsfb bFlag, wsul nPar1, wsul nPar2 = 0, float bAmp = 100.0f, WSF2Samps wSamps = NULL ); // Returns 1 if error :<

	/// Gets sample by ID. V1.x WSFMod compatible!
	/// \return 0 if no error, 1 if error, -1 if loaded sample but NO SAMPLE DATA
	int GetSampV1( wsf_sampout *wOut, unsigned int nID, wsfb bFlag, wsul nPar, wsfb bAmp ); // Returns 1 if error :<

	/// Gets sample by ID, used internally when not loading. Refers to V1.x WSFMod.
	/// (Usually used when retrieving no data)
	int GetSamp( wsf_sampout *wOut, unsigned int nID, wsfb bFlag, wsul nPar, wsfb bAmp );

	/// Gets a samples size, even if its unloaded.
	wsul GetSampSize( wsul nID );

	/// Gets a samples stored size, even if its unloaded.
	wsul GetSampCompSize( wsul nID );

	/// Frees Pack in memory
	int FreePack( void );

#ifndef WSFNOSAVE
	/// Tests if sample exists, and if it does returns its ID
	wsul SampleExist( char *cSampSig, int nCh = -1, int nBit = -1 );		// Determines if a sample exists

	/// Adds Sample
	/// Adds a sample to the pack. The wInSamp must already be specified.
	/// \param wInSamp Sample to add
	/// \param bSig PRE-CREATED signature. (See CWSFPack::CreateSignature) Can now be NULL
	wsul AddSample( wsf_sampout *wInSamp, wsfb *bSig = NULL );

	/// Tests to see if a Sample Exists based on another sample (woah).
	wsul SampleExist( wsf_sampout *wInSamp, wsfb *bFlag, wsul *nPar, float *nAmp, wsul *nParB = NULL, WSF2Samps wSm = NULL );

	/// Tests to see if two samples are the same
	wsul SamplesMatch( wsf_sampout *wSampA, wsf_sample *wSampB, wsfb *bFlag, wsul *nPar, float *nAmp, wsul nID, wsul *nParB = NULL, WSF2Samps wSm = NULL );

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

	/// Compresses Sample
	int CompressSample( wsfb *bFullSamp, unsigned int nFullSize, wsfb **bOutSamp, unsigned int *nCompSize );

#endif

	/// DeCompresses Sample
	int DeCompressSample( wsfb *bCompSamp, unsigned int nCompSize, wsfb **bOutSamp, unsigned int nFullSize );

	/// Makes percentage out of two integers
	/// \param nLow Low UL (50 out of 100 = 50)
	/// \param nHi Hi UL (50 out of 100 = 100)
	unsigned int MakePercent( wsul nLow, wsul nHi );

	void SetSampleFreq( wsul nID, wsul nFreq )
	{
		if (nID < m_nSamples && nID >= 0)
			m_wSamples[nID].nFreq = nFreq;
	};

	/// Gets Number of Samples
	unsigned int GetNumSamples( void )
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

	/// Encodes a buffer based on a bit (v2.03+)
	void EncodeData2( wsfb *cStr, int nEbit, wsul nLen );

	/// Decodes a buffer based on a bit (v2.03+)
	void DecodeData2( wsfb *cStr, int nEbit, wsul nLen );

	/// Sets the password sum for saving or loading (either works ^^;)
	void SetPassSum( int nPassSum );

	// Stuffs

	/// Init
	CWSFPack();
	/// Kill
	~CWSFPack();

	/// Needs password
	wsfb m_nNeedPass;

	wsfb m_bHi;
	wsfb m_bLo;

private:

	int m_nPassSum;				///< Password sum

	int m_nLoaded;				// Loaded?

	unsigned int m_nSamples;	// Number of Samples
	wsf_sample *m_wSamples;		// Samples	

};

// generates password sum
int GenerateSum(char *code, int size);

int OggEncode( wsul nSize, wsfb *bData, wsul nCh, int nBit, wsf_file *wOut, wsul nFreq = 0 );
int OggDecode( wsul nSize, wsfb *bData, int nBit, wsf_file *wOut );

#endif

