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

#ifndef __WSFMODH__
#define __WSFMODH__

#include <string.h>
#include "WSFGlob.h"
#include "WSFPack.h"

#ifndef NULL
#define NULL 0
#endif

#define WSFMHEAD	"WSF1"			///< File Header for WSF format
#define WSFMVERHI	2				///< Version of WSF currently supported vX.00
#define WSFMVERLO	02				///< Version of WSF currently supported v2.XX

class CWSFMod;

/// Searches through a data buffer for a string
wsfb *SearchForString( wsfb *lpBuf, wsul nSize, char *cStr, int nTagLen );

/// MOD loading formats.
/// These match the g_cModExt array of file extensions.
typedef enum modvers_e
{
	WSFM_IT,		///< IT File Format
	WSFM_XM,		///< XM File Format
	WSFM_S3M,		///< S3M File Format
	WSFM_MOD,		///< MOD File Format
	
	WSFM_WSF,		///< WSF File Format (internal use only)
	WSFM_WTF,		///< WTF File Format (internal use only)
	
	// Insert Formats Here

	WSFM_COUNT,
	WSFM_NONE

} WSFModVers;

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

/// WSFMODOUT for use with WriteMod
typedef struct wsf_modout_s
{
	/// Size of bModData
	unsigned long nSize;

	/// Mod Buffer
	wsfb *bModData;

} wsf_modout;

wsfb *ReplaceData( wsfb *bIn, unsigned long nIn, unsigned long *nOut, unsigned long nRepOff, unsigned long nRepLength, wsfb *bFill, unsigned long nFillLen );

#ifndef WSFNOASCII

/// Structure for use with CWSFAscii::GetAllTags.
/// This contains all the tags in a file
typedef struct wsf_asciiout_s
{
	char **cVals;		///< Values array
	char **cTags;		///< Tags array
	int nTags;			///< Number of tags and values
} wsf_asciiout;

/// Handles the ascii section of a WSF file.
/// \todo Tag removal is something that we need to support here
class CWSFAscii
{
	public:

	/// Gets WSF tags
	/// @param cTag is an array of tags. cTag = { "title", "artist" }; etc
	/// @param cOut is an array of pointers that the values are saved to.
	/// @param nTags is number of tags
	/// @param fn File Name of WSF File
		int GetWSFTags( char *fn, char **cTag, char **cOut, int nTags );

	/// Gets WSF tags
	/// @param cTag is an array of tags. cTag = { "title", "artist" }; etc
	/// @param cOut is an array of pointers that the values are saved to.
	/// @param nTags is number of tags
	/// @param wFile WSF File
		int GetWSFTags( wsf_file *wFile, char **cTag, char **cOut, int nTags );

	/// Saves WSF tags.
	/// This completely overwrites the old tags
	/// @param cTag is an array of tags. cTag = { "title", "artist" }; etc
	/// @param cValue is an array of pointers that the values are retrieved from.
	/// @param nTags is number of tags
	/// @param fn File Name of WSF File
		int SaveWSFTags( char *fn, char **cTag, char **cValue, int nTags );

	/// Adds WSF Tags
	/// @param cTag is an array of tags. cTag = { "title", "artist" }; etc
	/// @param cValue is an array of pointers that the values are retrieved from.
	/// @param nTags is number of tags
	/// @param fn File Name of WSF File
		int AddWSFTags( char *fn, char **cTag, char **cValue, int nTags );

	/// Sets the ascii section in a WSF
		int SetWSFAscii( char *fn, char *cIn );

		char *GetWSFAscii( wsf_file *wFile, unsigned short *nLen );

	/// Retrieves ascii section in a WSF
		char *GetWSFAscii( char *fn, unsigned short *nLen );

	/// Creates a Title
		void MakeTitle( char *filename, char *title, char *titlefmt );

	/// Makes a title based on tags, and not a given file
		void MakeTitleFromTags( int nInTags, char **cInTags, char **cInVals, char *cTitle, char *cTitleFmt );

	/// Gets all the WSF tags in a file.
	/// Retrieves all the WSF tags in a WSF file.
	/// @param fn Filename
	/// @param aO wsf_asciiout structure which is filled
		int GetAllTags( char *fn, wsf_asciiout *aO );

	/// Gets all the WSF tags in a file.
	/// Retrieves all the WSF tags in a WSF file.
	/// @param fn file
	/// @param aO wsf_asciiout structure which is filled
		int GetAllTags( wsf_file *wF, wsf_asciiout *aO );

	/// Gets all the WSF tags in a file.
	/// Retrieves all the WSF tags in a WSF file.
	/// @param cAscii Ascii Blob
	/// @param nLen Ascii Blob Length
	/// @param aO wsf_asciiout structure which is filled
		int GetAllTagsFromAscii( char *cAscii, unsigned short nLen, wsf_asciiout *aO );

	/// Frees a wsf_asciiout structure (usually filled by GetAllTags).
	/// @param aO wsf_asciiout structure to free
		int FreeAsciiOut( wsf_asciiout *aO );

	/// Inits a wsf_asciiout structure
	/// @param aO Structure to init
		int InitAsciiOut( wsf_asciiout *aO );

	/// Adds a tag and value to ascii out structure
	/// @param aO wsf_asciiout structure
	/// @param cTag Tag Name to add
	/// @param cVal Tag Value to add
		int AddTagToAsciiOut( wsf_asciiout *aO, char *cTag, char *cVal );

	/// Removes a tag from a ascii out structure
	/// @param aO wsf_asciiout structure
	/// @param cTag Name of tag to remove, NULL if to use nIndex
	/// @param nIndex If cTag is NULL, then this represents the value of the tag to be removed
		int RemoveTagFromAsciiOut( wsf_asciiout *aO, char *cTag, int nIndex );

	/// Gets a tag value from an AsciiOut structure.
	/// Returns -1 if it doesn't exist, and the ID in the wsf_asciiout structure of
	/// that tag value if it does.
		int GetTagValue( wsf_asciiout *aO, char *cTag );

};

#endif

/// Structure returned from the loaders and sent to the savers.
/// A loader must allocate bModData and wSamples and fill them.
/// The structure will already be filled when sent to save.
typedef struct wsf_loaddata_s
{
	/// Number of samples loaded
	wsul nSamples;

	/// Mod Data (uncompressed)
	wsfb *bModData;

	/// Mod Data Size
	wsul nModSize;

	/// Sample Buffer
	wsf_sample *bSamples;
	/// Sample Sizes
	wsul *nSampleSizes;

	/// Mod Title
	char cTitle[30];

	/// Subtraction Diffrences
	wsul nSubDiff;

	/// Mod Type from WSFModVers enum
	wsfb nModType;

	/// Sample Offsets
	wsul *nSampleOffsets;

	/// Min Sample Size to Compress (0 for no compression)
	wsul nOggSize;

	// Use the following for WGMs.
	/// Offset of Pack in file for WSF Loading
	wsul nPackOffset;

	/// Size of pack in file
	wsul nPackSize;

} wsf_loaddata;

/// Structure used for CBaseLoader::GetSamplePointers
typedef struct wsg_gspdata_s
{
	wsul nSamples;		///< Number of Samples
	wsul *nSampOffs;	///< Sample Offsets	(this needs to be freed)
	wsul *nSampSizes;	///< Sample Sizes (this needs to be freed)
//	wsul nSub;			///< Subtraction Diffrence

	wsfb *nCh;			///< Channels
	wsfb *nBit;			///< Bitrate
	wsul *nInSamps;		///< Samples Contained WITHIN Samples, not number of samples
	wsul *nFreqs;		///< C-5 Frequencies!
	char *cNames;		///< Sample Names
} wsf_gspdata;

/// Base class for loading and saving of modules.
class CBaseLoader
{
	public:

		CBaseLoader();

	/// Base Function for Loading.
	/// Implement this by loading data into the wLD structure.
		virtual int Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD )=0;

	/// Base Saving Function.
	/// Implement this by saving data FROM the wLD structure.
	/// If this function returns 0, then samples are replaced automatically.
	/// If 1 is returned, then the samples aren't replaced and this function must do ALL saving.
	/// If 2 or greater is returned, then an error is assumed.
		virtual int Save( wsf_modout *wOut, wsf_loaddata *wLD )=0;

	/// Function to get pointers to ACTUAL samples.
	/// Implement this for the WSFLoader.
	/// This should return the number of samples, sample offsets in the file, and each samples size in bytes.
		virtual int GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData );

	/// This function is to be called in Save() of the loader function to
	/// make a 'carbon copy' of the sample data if the function is going
	/// to modify the sample data in the LoadData structure. The carbon copy
	/// will then be used in restoring the data.
		int AllowSampleEdit( wsf_loaddata *wLD );

	/// This function is called from CWSFMod in the saving process to return
	/// samples to original form.
		int RestoreSampleEdit( wsf_loaddata *wLD );

	/// This function is called by CWSFMod to restore the original samples.

		wsfb *ReplaceData( wsfb *bIn, unsigned long nIn, unsigned long *nOut, unsigned long nRepOff, unsigned long nRepLength, wsfb *bFill, unsigned long nFillLen );

	wsfb	*g_bS;	///< Byte Start	(used for memorymacros MXXX)
	wsfb	*g_bP;	///< Byte Pos (used for memorymacros MXXX)
	unsigned long g_nB;	///< Size of bytes (used for memorymacros MXXX)
	unsigned long g_nW; ///< Byte Written (used for memorymacros MXXX)

	wsf_sample *m_wCopySamps;	///< 'Carbon Copy' of sample data for AllowSampleEdit

	char m_cTitle[30];	///< Holds Title just so it can be retrieved.
						///< \note This was added just because its a variable most mods have.

	char m_cFile[200];		///< Internal Filename

	/// Set File Name
	void SetFileName( char *cFile )
	{
		strcpy(m_cFile,cFile);
	};

	wsfb m_bHi;
	wsfb m_bLo;
};

/// Handles Loading + Saving Impulse Tracker Modules.
/// Handles all the IT module functions
/// for loading and saving
class CITLoader : public CBaseLoader
{
public:

	/// Base Function for Loading
		int Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD );

	/// Base Saving Function
		int Save( wsf_modout *wOut, wsf_loaddata *wLD );

	/// Base function for getting samples offsets (returns pointer to IMPS stuff)
		wsul *FindSampleOffsets( wsfb *bData, wsul nSize, int *nSOut );

	/// Function to get pointers to ACTUAL samples
		int GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData );

	// Functions for decompression

		int readblock(void);
		void freeblock(void);
		int readbits(int bitwidth);

		int decompress8(wsfb *left, wsfb *right, int len, int cmwt);
		int decompress16(wsfb *left, wsfb *right, int len, int cmwt);

		unsigned char *sourcebuf;
		unsigned char *sourcepos;
		unsigned char *sourceend;
		int rembits;

		short m_nComp;
};

/// Handles Loading + Saving Scream Tracker Modules.
/// Handles all the S3M module functions
/// for loading and saving
class CS3MLoader : public CBaseLoader
{
	public:

	/// Base Function for Loading
		int Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD );

	/// Base Saving Function
		int Save( wsf_modout *wOut, wsf_loaddata *wLD );

	/// Function to get pointers to ACTUAL samples
		int GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData );

};

/// Handles Loading + Saving MOD Modules.
/// Handles all the MOD module functions
/// for loading and saving
class CMODLoader : public CBaseLoader
{
	public:

	/// Loads the sample header
		int LoadSampleHeader( wsf_gspdata *gspData, char *cName );

	/// Base Function for Loading
		int Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD );

	/// Base Saving Function
		int Save( wsf_modout *wOut, wsf_loaddata *wLD );

	/// Function to get pointers to ACTUAL samples
		int GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData );

};

/// Handles Loading + Saving Fast Tracker Modules.
/// Handles all the XM module functions
/// for loading and saving
class CXMLoader : public CBaseLoader
{
	public:

	/// Base Function for Loading
		int Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD );

	/// Base Saving Function
		int Save( wsf_modout *wOut, wsf_loaddata *wLD );

	/// Function to get pointers to ACTUAL samples
		int GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData );

};

/// Handles Loading + Saving WSF modules.
/// Handles all the WSF module functions
/// for loading and saving
class CWSFLoader : public CBaseLoader
{
	public:

		CWSFLoader();

	/// Base Function for Loading
		int Load( wsfb *bData, wsul nSize, wsf_loaddata *wLD );

	/// For loading version v1.x WSFMods. Does not mean that it is ModFusion 1 compat.
	/// Use CWSFLoaderV1 class to load strict ModFusion compatible WSF.
		int LoadV1( wsf_loaddata *wLD, char *cTPack );

	/// For loading version v2.x WSFMods.
		int LoadV2( wsf_loaddata *wLD, char *cTPack );

	/// Base Saving Function
		int Save( wsf_modout *wOut, wsf_loaddata *wLD );

	/// Additional Saving Unit
		int Save( wsf_modout *wOut, wsf_loaddata *wLD, long *nSCounts );

	/// Saving V1.x Mods
		int SaveVer1( wsf_modout *wOut, wsf_loaddata *wLD, long *nSCounts );

	/// Function to get pointers to ACTUAL samples
//	int GetSamplePointers( wsfb *bData, wsul nSize, wsf_gspdata *gspData );

	/// Prepares a WSF2SampArray
		WSF2Samps PrepareSampArray( void );

	/// Frees WSF2SampArray
		void FreeSampArray( WSF2Samps wS );

	/// Pushes sample into Sample Array
		wsul PushIntoSampArray( WSF2Samp *wS, WSF2Samps wSamps );

	/// Pushes sample into Sample Array
		wsul PushIntoSampArray( wsul nID, wsfb bFlag, wsul nPar1, wsul nPar2, float fAmp, WSF2Samps wSamps );

	/// Pushes Reference into Sample Array
		wsul PushRefIntoArray( wsul nID, WSF2Samps wSamps );

	/// Sets Pack Name (for saving)
		int SetPack( char *cName, CWSFPack *wPack )
		{
			if (cName)
				strcpy(m_cCurPack,cName);
			if (wPack)
				m_wPack = wPack;

			return 0;
		};

	/// Gets Pack
		CWSFPack *GetPack( void )
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
		CWSFPack	*m_wPack;	///< Sample Pack
		wsfb m_nModType;		///< Internal MOD Type (yea) WSFModVers enum
		int m_nFreePack;

	wsf_file *m_fPack;		///< Pack File Handle if set (:

};

///	Handles Main MOD and WSF functions.
///	Handles MOD writing and saving, along with
///	WSF commands. Also handles tags in the
/// ascii section.
///
class CWSFMod
{
	public:

	/// Loads Mod into buffer
		int LoadMod( char *cFileName, int nFreePack = 1, wsul *nOSamps = NULL, wsul nPackOff = 0, wsul nPackSize = 0 );
	/// Loads Mod into buffer
		int LoadMod( wsfb *bData, wsul nSize, int nFreePack, wsul *nOSamps, int nType = WSFM_WTF, wsul nPackOff = 0, wsul nPackSize = 0 );
	/// Loads Mod into buffer
		int LoadMod( wsf_file *wf, int nFreePack = 1, wsul *nOSamps = NULL, int nType = WSFM_WTF, wsul nPackOff = 0, wsul nPackSize = 0 );

	/// Frees Mod
		int FreeMod( void );

	/// Write Mod.
	/// Write's Mod
	/// \param nWSF nonzero if saving to WSF file
		int WriteMod( wsf_modout *wOut, int nWSF, long *nSCounts = NULL );

#ifndef WSFNOSAVE

	/// Save Pack
	int SavePack( int nComp );

#endif

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

#ifndef WSFNOASCII
	/// Returns the ascii handler class
	CWSFAscii *GetAscii( void )
{
	return &m_mAsc;
};
#endif

	CWSFPack *GetPack( void )
{
	return m_wPack;
};

	/// Sets pack with this, NULL to stop using wsf_file.
	void SetPack( wsf_file *wPack )
{
	m_fPack = wPack;
};

	CWSFMod();
	~CWSFMod();

	/// Sets password sum before needing to convert to WSF or back
	void SetPassSum( int nPassSum );

	char m_cModTitle[30];		///< Mod Title
	char m_cCurPack[200];		///< Current Pack Name for WSF

	wsf_loaddata m_wLD;		///< Load Data Structure

	wsfb m_bHi;
	wsfb m_bLo;

	private:

		int	m_nLoaded;			///< MOD Loaded?
		wsfb m_nModType;		///< Mod Type from WSFModVers enum

		CWSFPack *m_wPack;		///< Pack
	
#ifndef WSFNOASCII
	CWSFAscii	m_mAsc;		///< Ascii Handler
#endif

	wsf_file *m_fPack;		///< If not NULL, will call SetPack with this.
};

/// Contains stuff for InfoDlg
typedef struct wsfinfo_s
{
	/// File Name
	char cFileName[200];

#ifndef WSFNOASCII
	/// Ascii Handler
	CWSFAscii *wAsc;

	/// Ascii Out header
	wsf_asciiout aO;
#endif

	/// Mod File
	CWSFMod *wMod;

	/// Samples
	wsul nSamps;

} WSFInfo;

CBaseLoader *GetBaseLoader( wsfb nModType );
void FreeGSPData( wsf_gspdata *gspData );
void InitGSPData( wsf_gspdata *gspData );

/// Pushes values into gspData (ONLY IF YOU START WITH 0 SAMPLES AND MUST LOAD THROUGH)
void PushIntoGSP( wsf_gspdata *gspData, wsul nSize, wsul nOffset, wsul nSamps, wsfb nCh, wsfb nBit, char *cName, wsul nFreq = 0 );

#define MOPEN(x,y)		{ g_bS = x; g_bP = g_bS; g_nB = y; g_nW = 0; }	///< Opens x as buffer for size of y
#define MREAD(x,y,z)	{ memcpy(x,g_bP,y*z); g_bP+=y*z; }				///< Same as fread without file pointer
#define MSAVE(x,y,z)	{ memcpy(g_bP,x,y*z); g_bP+=y*z; g_nW+=y*z; }	///< DO NOT USE :O
#define MSEEK(y)		{ g_bP+=y; }									///< Seeks y bytes from the current position
#define MBACK(y)		{ g_bP-=y; }									///< Seeks y bytes backwards from the cur position
#define MBEGIN(y)		{ g_bP=g_bS+y; }								///< Seeks y bytes from the beginning of the buffer
#define MEND(y)			{ g_bP=g_bS+g_nB-y; }							///< Seeks y bytes backwards from ending of buffer
#define MAPP(x,y,z)		{ g_nB += y*z; g_bS = (wsfb*)realloc(g_bS,g_nB); g_bP = g_bS + g_nW; memcpy(g_bP,x,y*z); g_nW += y*z; g_bP = g_bS + g_nW; } ///< Does the same as fwrite. If you use this, you will need to call MRESET when your done
#define MRESET(x,y)		{ *x = g_bS; *y = g_nB; }	///< Closes a file that was opened for writing. x should be a *wsfb to recieve data pointer, and y a wsul to recieve its length

#define MTELL() (wsul)(g_bP-g_bS)		///< Gets Current Offset

#endif
