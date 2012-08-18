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

/// \file WSFTool.cpp
/// A suite of features to convert WSF's to MOD's and vice versa

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
#include <math.h>

#ifndef WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

// HOW ANNOYING! DOS findfirst etc are different than windows. So all this
// crap needs to be set.

#ifdef WIN32
#include <io.h>
#define FINDFIRST(x,y) _findfirst(x,y)
#define FINDNEXT(x,y) _findnext(x,y)
#define FINDDATA _finddata_t 
#define FINDERR -1L
#define FINDSTR() if ( (hFile = FINDFIRST(wS->cInput,&c_file)) != FINDERR)
#define FINDPREP()
#endif

#include "WSFGlob.h"
#include "WSFMod.h"
#include "WGMPack.h"
#include "WSFTool.h"
#include "WSFSpecialTags.h"

#ifdef MP3SUPPORT

#include <lame.h>
#include "dumb.h"

#pragma comment( lib, "dumb.lib" )
#pragma comment( lib, "libmp3lame.lib" )

/// Default Bitrate for -MP3 option
#define DEFBITRATE	128
#define DEFQUAL		8

#endif

extern int g_nRate;

/// Default Size for OGG -V option
#define DEFOGG 15360

/// Main CWSFMod
CWSFMod *g_wMod;
/// Main CWSFAscii
CWSFAscii *g_mAsc;

/// WSFTool Version Hi x.00
#define WSFTVERHI	0
/// WSFTool Version Lo 1.xx
#define WSFTVERLO	80

/// WSFTool Running Modes
typedef enum wsft_e
{
	WSFT_TOWSF,		///< Converts input to wsf			-M
	WSFT_WSFTO,		///< Converts input FROM wsf		-W
	WSFT_SHOWEXT,	///< Shows Extensions Avalible		-showext
	WSFT_READPACK,	///< Reads a Pack and shows info	-P
	WSFT_READPACKWSF, ///< Reads a WSF and shows its PACKs info (like READPACK) -R
	WSFT_MODTAGS,	///< Modifies Tags					-T

#ifdef MP3SUPPORT
	WSFT_MAKEMP3,	///< Creates MP3s from WSFs :D		-MP3
	WSFT_LENGTH,	///< Determines the length of all the files :P	-LEN
#endif

	WSFT_MAKEWGM,	///< Creates a WGM from the selected files		-G
	WSFT_LOADWGM,	///< Unpacks a WGM								-U

	WSFT_NAME,		///< Renames WSF based on tags		-N
	WSFT_REMPASS,	///< Removes passwords from WSF/WTF files	-B
	WSFT_HELP,		///< Extra Help

	WSFT_PREVSAMP,	///< Previews Sample
	WSFT_PREVOGGSAMP,	///< Previews "Ogg'd" Sample
	WSFT_LISTSAMP,	///< Lists Samples in SampleID|SampleName|StoredSize|CFlag|CompSize format
	WSFT_COMPSAMP,	///< Compresses Sample

	WSFT_NONE,		///< Used when no mode is set

	WSFT_MODECOUNT
} WSFTModes;

//////////////////////////////////////////////////////
//  MakePercentEx()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 12:09:24 PM
/// \param nLow x/100
/// \param nHi 50/x
/// \return Percentage of nLow/nHi
/// \brief Creates a Percentage
/// 
// 
//////////////////////////////////////////////////////

wsul MakePercentEx( wsul nLow, wsul nHi )
{
	double n1;
	double n2;
	double nR;

	n1 = nLow;
	n2 = nHi;

	nR = n1/n2;
	nR *= 100.0;

	return (wsul)nR;
}

//////////////////////////////////////////////////////
//  RunTOWSF()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 8:19:15 PM
/// \param wS Settings
/// \brief Runs TOWSF Mode.
/// Used to convert MODS to WSFS
///
// 
//////////////////////////////////////////////////////

void RunTOWSF( WSFTSets *wS )
{
	char cFile[200];
	char *cOut;
	int nNum;
	char *cPack;
	wsul nSampsInput;
	wsul nSampsLoaded;
	int i;
	char **cVals;
	char **cTags;
	int nTags;
	long nSCount[WSFS_COUNT];
	wsf_modout wO;
	FILE *OFile;
	CWSFAscii mA;

	cPack = wS->cArg1;
	nSampsLoaded = 0;

	if (wS->cOutputFormat[0])
		cOut = wS->cOutputFormat;
	else
		cOut = NULL;

	nTags = wS->nTags+2;
	cTags = (char**)malloc(sizeof(char*)*nTags);
	cVals = (char**)malloc(sizeof(char*)*nTags);

	cTags[0] = strdup("pack");
	cTags[1] = strdup("title");
	cVals[0] = strdup(cPack);
	cVals[1] = (char*)malloc(40);

	for (i=0;i<wS->nTags;i++)
	{
		cTags[i+2] = strdup(wS->cTags[i]);
		cVals[i+2] = strdup(wS->cVals[i]);
	}

	g_wMod->m_wLD.nOggSize = wS->nArg[0];

	nNum = 0;
	// Search Files
	if (wS->nInputs > 0)
	{
		long nTemp[WSFS_COUNT];
		int xc;		

		for (xc=0;xc<WSFS_COUNT;xc++)
			nSCount[xc] = 0;
		
		// Load Pack ^_^
		g_wMod->SetPassSum(wS->nPassSum);
		g_wMod->FreePack();
		g_wMod->LoadPack(cPack);

		g_wMod->m_wLD.nOggSize = wS->nArg[0];

		g_wMod->m_cModTitle[0] = 0;

		// Load Mod
		printf("Loading Mod %s...\n",wS->cInput[0]);
		if (g_wMod->LoadMod(wS->cInput[0],1,&nSampsInput)) {
			printf("Could not load %s\n",wS->cInput[0]);
			return;
		}

		if (nSampsInput != 666)
			nSampsLoaded += nSampsInput;

		if (g_wMod->m_cModTitle[0])
			strcpy(cVals[1],g_wMod->m_cModTitle);
		else
		{
			char *cP;
			cP = strrchr(wS->cInput[0],'.');

			if (cP)
				cP[0] = 0;

			strcpy(cVals[1],wS->cInput[0]);

			if (cP)
				cP[0] = '.';
		}

		// Output Name
		if (cOut != NULL){
			// Make the title from given tags
			mA.MakeTitleFromTags(nTags,cTags,cVals,cFile,cOut);
		}
		else
		{			// Else remove extension and add .wsf
			char *cExt;
			cExt = strrchr(wS->cInput[0],'.');
			if (cExt)
				cExt[0]=0;
			sprintf(cFile,"%s.wtf",wS->cInput[0]);
			if (cExt)
				cExt[0]='.';
		}
		nNum++;

		// Get WSF
		printf("Writing WSF %s...\n",cFile);
		if (g_wMod->WriteMod(&wO,1,(long*)&nTemp)) {
			printf("Could not write %s\n",cFile);
			return;
		}

		for (xc=0;xc<WSFS_COUNT;xc++)
			nSCount[xc] += nTemp[xc];

		// Open a file and save it
		OFile = fopen(cFile,"wb");
		if (!OFile) {
			printf("Could not create %s!\n",cFile);
			return;
		}

		// Write and close
		fwrite(wO.bModData,1,wO.nSize,OFile);
		fclose(OFile);

		g_mAsc->AddWSFTags(cFile,(char**)cTags,(char**)cVals,nTags);

		// Kill Mod DATA
		free(wO.bModData);

		for (int ix=1;ix<wS->nInputs;ix++) 
		{
			char tmpstr[256];
			strcpy(tmpstr,wS->cInput[ix]);
			
			g_wMod->m_cModTitle[0] = 0;

			g_wMod->m_wLD.nOggSize = wS->nArg[0];

			// Load Mod
			printf("Loading Mod %s...\n",tmpstr);
			if (g_wMod->LoadMod(tmpstr,1,&nSampsInput)) {
				delete g_wMod;
				printf("Could not load %s!\n",tmpstr);
				return;
			}

			if (nSampsInput != 666)
				nSampsLoaded += nSampsInput;

			if (g_wMod->m_cModTitle[0])
				strcpy(cVals[1],g_wMod->m_cModTitle);
			else
			{
				char *cP;
				cP = strrchr(tmpstr,'.');

				if (cP)
					cP[0] = 0;

				strcpy(cVals[1],tmpstr);

				if (cP)
					cP[0] = '.';
			}

			// Output Name
			if (cOut != NULL){
				// Make the title from given tags
				mA.MakeTitleFromTags(nTags,cTags,cVals,cFile,cOut);
			}
			else
			{			// Else remove extension and add .wsf
				char *cExt;
				cExt = strrchr(tmpstr,'.');
				if (cExt)
					cExt[0]=0;
				sprintf(cFile,"%s.wtf",tmpstr);
				if (cExt)
					cExt[0]='.';
			}
			nNum++;

			g_wMod->m_wLD.nOggSize = wS->nArg[0];

			// Get WSF
			printf("Writing WSF %s...\n",cFile);
			if (g_wMod->WriteMod(&wO,1,(long*)&nTemp)) {
				printf("Could not write %s\n",cFile);
				return;
			}

			for (xc=0;xc<WSFS_COUNT;xc++)
				nSCount[xc] += nTemp[xc];

			// Open a file and save it
			OFile = fopen(cFile,"wb");
			if (!OFile) {
				delete g_wMod;
				printf("Could not create %s!\n",cFile);
				return;
			}

			// Write and close
			fwrite(wO.bModData,1,wO.nSize,OFile);
			fclose(OFile);

			g_mAsc->AddWSFTags(cFile,(char**)cTags,(char**)cVals,nTags);

			// Kill Mod DATA
			free(wO.bModData);
		}

		printf("Saving Pack...\n");

		g_wMod->SetPassSum(wS->nPassSum);
		g_wMod->SavePack(1);
	}
	else{
		printf("No Files were found!\n\n");
		for (i=0;i<nTags;i++){
			free(cVals[i]);
			free(cTags[i]);
		}

		free(cVals);
		free(cTags);
		return;
	}

	for (i=0;i<nTags;i++){
		free(cVals[i]);
		free(cTags[i]);
	}

	free(cVals);
	free(cTags);

	printf("\n");
	{
		wsul nOutP;
		CWSFPack mP;

		mP.LoadPack(cPack,&nOutP,0);
		nOutP = mP.GetNumSamples();

		printf("%u samples were loaded, %u samples were saved into pack %s\n",(unsigned int)nSampsLoaded,(unsigned int)nOutP,cPack);
		printf("%d samples were NORMAL\n%d samples were REVERSED\n"
			"%d samples were SPLICES (WSFS_SAMPCUTOFF) (%d reversed)\n%d samples were APENDEES (WSFS_SAMPAFTER)\n\n",
			nSCount[WSFS_NORMAL],nSCount[WSFS_REVERSED],
			nSCount[WSFS_SAMPCUTOFF]+nSCount[WSFS_REVSAMPCUTOFF],nSCount[WSFS_REVSAMPCUTOFF],
			nSCount[WSFS_SAMPAFTER]);
		printf("Storage Ratio of %d%%\n",(int)100-mP.MakePercent(nOutP,nSampsLoaded));
		printf("Saved %d WSF files\n",nNum);

		mP.FreePack();
	}
	printf("\n");

	return;
}

//////////////////////////////////////////////////////
//  RunWSFTO()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 8:19:39 PM
/// \param wS Settings
/// \brief Runs WSFTO Mode.
/// Used to convert WSFS back to their original format
///
// 
//////////////////////////////////////////////////////

void RunWSFTO( WSFTSets *wS )
{
	char *cOut;
	wsf_modout wO;
	cOut = NULL;
	CWSFAscii mA;

	if (wS->cOutputFormat[0])
		cOut = wS->cOutputFormat;

	// Search Files
	if (wS->nInputs > 0)
	{
		char *cPos;
		char tmpstr[200];
		char cFile[200];
		FILE *OFile;
		int nType;

		strcpy(tmpstr,wS->cInput[0]);
		strcpy(cFile,tmpstr);
		cPos = strrchr(cFile,'.');

		if (!cPos)
			return;

		g_wMod->SetPassSum(wS->nPassSum);

		printf("Loading %s...\n",tmpstr);
		if (g_wMod->LoadMod(tmpstr)){
			printf("Could not load %s\n",tmpstr);
			return;
		}

		if (g_wMod->GetPack()->m_nNeedPass && wS->nPassSum == 0)
			printf("Needs password! Use -x[pass] option\n");

		nType = g_wMod->GetModType2();

		// Output Name
		if (cOut != NULL){
			// Make the title from given tags
			mA.MakeTitle(tmpstr,cFile,cOut);
		}
		else
		{			// Else remove extension and add .wsf
			// Copies over extension
			strcpy(cPos,g_cModExt[nType]);
		}

		printf("Writing %s...\n",cFile);
		if (g_wMod->WriteMod(&wO,0))
		{
			printf("Could not save %s\n",cFile);
			return;
		}

		OFile = fopen(cFile,"wb");
		if (!OFile)
		{
			free(wO.bModData);
			printf("Could not open %s\n",cFile);
			return;
		}

		fwrite(wO.bModData,wO.nSize,1,OFile);
		fclose(OFile);

		free(wO.bModData);

		for (int ix=1;ix<wS->nInputs;ix++)
		{
			strcpy(tmpstr,wS->cInput[ix]);
			strcpy(cFile,tmpstr);
			cPos = strrchr(cFile,'.');

			if (!cPos)
				return;

			g_wMod->SetPassSum(wS->nPassSum);

			printf("Loading %s...\n",tmpstr);
			if (g_wMod->LoadMod(tmpstr)){
				printf("Could not load %s\n",tmpstr);
				return;
			}

			if (g_wMod->GetPack()->m_nNeedPass && wS->nPassSum == 0)
				printf("Needs password! Use -x[pass] option\n");

			nType = g_wMod->GetModType2();

			// Output Name
			if (cOut != NULL){
				// Make the title from given tags
				mA.MakeTitle(tmpstr,cFile,cOut);
			}
			else
			{			// Else remove extension and add .wsf
				// Copies over extension
				strcpy(cPos,g_cModExt[nType]);
			}

			printf("Writing %s...\n",cFile);
			if (g_wMod->WriteMod(&wO,0))
			{
				printf("Could not save %s\n",cFile);
				return;
			}

			OFile = fopen(cFile,"wb");
			if (!OFile)
			{
				free(wO.bModData);
				printf("Could not open %s\n",cFile);
				return;
			}

			fwrite(wO.bModData,wO.nSize,1,OFile);
			fclose(OFile);

			free(wO.bModData);
		}
	}

	return;
}

//////////////////////////////////////////////////////
//  ShowUsage()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 7:52:29 PM
/// \brief Shows WSFTool Usage
/// 
// 
//////////////////////////////////////////////////////

#ifndef DOXYSKIP
void ShowUsage( int nMore )
#else
void ShowUsage_WSFTool( int nMore )
#endif
{
	printf("Usage: WSFTool input -option -option\n");

#ifdef WIN32
	printf("\tinput - Input File(s) (can be wildcard if needed)\n");
#else
	printf("\tinput - Input File\n");
#endif
	
	printf("\nOptions:\n");
	
	printf("-M[pack] - Converts input file(s) to WTF/WSF file(s) using pack\n");
	printf("-W - Converts input file(s) FROM wsf file(s) to their original format\n");
	printf("-T - Tags WSF File(s) (use the -t option)\n");
#ifdef MP3SUPPORT
	printf("-MP3[brate] - Converts WSF input files to MP3 at brate\n");
#endif
	printf("-? - Shows extra options\n");
	printf("\n");

	if (!nMore){
		printf("Surround options in \" to create multiword options\n(eg \"-MCommando Pack.pak\")\n");
		printf("All options are case sensitive! -m != -M\n\n");
		return;
	}

	// Extended Options
	printf("-N[format] - Renames input WSF/WTF based on tags (use %%tagname%% fmt)\n");
	printf("-P - Reads Pack (input) and gives information about it\n");
	printf("-R - Reads information from WSF file(s)\n");
	printf("-x[password] - Protects/Unlocks a pack with a password\n");
	printf("-t[tag=value] - Adds tag to compiled WSF files (-M/-T required) use _ for space\n");
	printf("-G[wgmpack] - Packs input into a WGM file (appends or creates new) (use with -WG option)\n");
	printf("-U - Unpacks wgmpack (input) into current directory\n");
	printf("-WG[type] - Specifies type of entry into WGM. PACK,COMMENT,OTHER supported. Default = TUNE\n");
#ifdef MP3SUPPORT
	printf("-Q[quality] - Sets MP3 quality, 0=best(slow) 9=worst(fast)\n");
	printf("-LEN - determines the length of all the modules\n");
#endif
#if !defined(NOOGG) && !defined(NOOGGENC)
	printf("-V[size] - Compresses samples of size bytes or more to OGG\n");
	printf("-VQ[bitrate] - Ogg Compression Bitrate (96, 128, 256, etc)\n");
#endif
	printf("-O[output] - Sets Output Format (via tags). wsf%%title%%.wsf -> wsfMySong.wsf etc\n");
	printf("-showext - Shows supported Module formats\n\n");
/*
	printf("-ListSamp - Lists the samples of a WSP in ID|Name|Size|Bit|Ch|OGG fashion\n");
	printf("-PrevSamp[sample] - Streams the sample from a WSP in wave pcm format\n");
	printf("-PrevOgg[sample] - Streams the sample from a WSP as if it were OGG'd (in wave pcm)\n");
	printf("-Compress[sample] - Compresses a sample into OGG format\n");
*/
	printf("\nSurround options in \" to create multiword options\n(eg \"-MCommando Pack.pak\")\n");

	return;
}

//*****************************************************************************
// FillSetting()
// 
// Purpose: Fills singular setting
// 
// Author : [Josh Whelchel], Created : [2/1/2002 11:25:31 AM]
// Parameters:	
// 
// [*cLine] :
// [nNum] :
// 
// Returns:	none
// 
//*****************************************************************************
void FillSetting( char *cLine, int nNum, WSFTSets *wS )
{
	char *cmd;
	cmd = cLine;

	char *cT;
		
	// Remove that last "
	cT = StrClip(cmd);
	if (cT!=NULL){
		if (cT[0] == '"')
			cT[0] = '\0';
	}

	if (cmd[0] == '"')
	{
		cmd++;
	}

	// Parameter is Input
	if (cmd[0] != '-' && cmd[0] != '/') {
#ifdef WIN32
		FINDDATA c_file;
		long hFile=0;
		
		if ((hFile = FINDFIRST(cmd,&c_file)) != FINDERR)
		{
			strcpy(wS->cInput[wS->nInputs++],c_file.name);
			while (FINDNEXT(hFile,&c_file) == 0)
			{
				strcpy(wS->cInput[wS->nInputs++],c_file.name);
			}
		}
#else
		strcpy(wS->cInput[wS->nInputs++],cmd);
#endif		
		return;
	}

	if (cmd[0] == '-')
		cmd++;

	if (cmd[0] == '/')
		cmd++;

#define xS(y) strnicmp(cmd,y,strlen(y))==0

	// Hide?
	if (xS("showext"))
	{
		wS->nMode = WSFT_SHOWEXT;
		return;
	}
#ifdef MP3SUPPORT
	else
	if (xS("MP3"))
	{
		cmd+=3;
		wS->nMode = WSFT_MAKEMP3;

		if (cmd[0])
			sscanf(cmd,"%d",&wS->nArg[0]);
		else
			wS->nArg[0] = DEFBITRATE;

		wS->nArg[1] = DEFQUAL;

		return;
	}
	else
	if (xS("LEN"))
	{
		wS->nMode = WSFT_LENGTH;

		return;
	}
	else
	if (xS("PREVSAMP"))
	{
		cmd+=8;
		
		if (cmd[0])
			sscanf(cmd,"%d",&wS->nArg[0]);
		else
			wS->nArg[0] = 0;

		wS->nMode = WSFT_PREVSAMP;

		return;
	}
	else
	if (xS("PREVOGG"))
	{
		cmd+=8;
		
		if (cmd[0])
			sscanf(cmd,"%d",&wS->nArg[0]);
		else
			wS->nArg[0] = 0;

		wS->nMode = WSFT_PREVOGGSAMP;

		return;
	}
	else
	if (xS("COMPRESS"))
	{
		cmd+=8;
		
		if (cmd[0])
			sscanf(cmd,"%d",&wS->nArg[0]);
		else
			wS->nArg[0] = 0;

		wS->nMode = WSFT_COMPSAMP;

		return;
	}
	else
	if (xS("LISTSAMP"))
	{
		cmd+=8;
		
		if (cmd[0])
			sscanf(cmd,"%d",&wS->nArg[0]);
		else
			wS->nArg[0] = 0;

		wS->nMode = WSFT_LISTSAMP;

		return;
	}
	else
	if (xS("VQ"))
	{
		cmd+=2;
		
		if (cmd[0])
			sscanf(cmd,"%d",&wS->nArg[1]);
		else
			wS->nArg[1] = 96;

		g_nRate = wS->nArg[1];

		return;
	}
#endif
	else
	if (xS("WG"))
	{
		cmd+=2;

		if (!cmd[0])
			return;

		if (stricmp(cmd,"PACK")==0)
			wS->nArg[0] = WGMT_PACK;
		else if (stricmp(cmd,"COMMENT")==0)
			wS->nArg[0] = WGMT_COMMENT;
		else if (stricmp(cmd,"OTHER")==0)
			wS->nArg[0] = WGMT_OTHER;

		return;
	}

	if (cmd[0] == 'M')
	{
		if (!cmd[1])
			return;

		wS->nMode = WSFT_TOWSF;
		cmd++;

		strcpy(wS->cArg1,cmd);
		wS->nArg[0] = 0;

		return;
	}
	else
	if (cmd[0] == 'G')
	{
		if (!cmd[1])
			return;

		wS->nMode = WSFT_MAKEWGM;
		cmd++;

		strcpy(wS->cArg1,cmd);
		wS->nArg[0] = WGMT_TUNE;

		return;
	}
	else
	if (cmd[0] == 'U')
	{
		wS->nMode = WSFT_LOADWGM;
		return;
	}
#ifdef MP3SUPPORT
	else
	if (cmd[0] == 'Q')
	{
		cmd++;

		if (cmd)
			sscanf(cmd,"%d",&wS->nArg[1]);
		else
			wS->nArg[1] = DEFQUAL;
		return;
	}
#endif
	else
	if (cmd[0] == 'V')
	{
		cmd++;
		if (cmd[0])
			sscanf(cmd,"%d",&wS->nArg[0]);
		else
			wS->nArg[0] = DEFOGG;
	}
	else
	if (cmd[0] == 'W')
	{
		wS->nMode = WSFT_WSFTO;
		return;
	}
	else
	if (cmd[0] == 'B')
	{
		wS->nMode = WSFT_REMPASS;
		return;
	}
	else
	if (cmd[0] == '?')
	{
		// Help
		wS->nMode = WSFT_HELP;
		return;
	}
	else
	if (cmd[0] == 'N')
	{
		if (!cmd[1])
			return;

		cmd++;
		wS->nMode = WSFT_NAME;

		// Copy Format (-N[format])
		strcpy(wS->cOutputFormat,cmd);
	}
	else
	if (cmd[0] == 'O')
	{
		if (!cmd[1])
			return;

		// Copy Format (-O[format])

		cmd++;
		strcpy(wS->cOutputFormat,cmd);
		return;
	}
	else
	if (cmd[0] == 'x' || cmd[0] == 'X')
	{
		cmd++;
		// Password (-x[PASS])
		wS->nPassSum = GenerateSum(cmd,strlen(cmd));
	}
	else
	if (cmd[0] == 'P')
	{
		wS->nMode = WSFT_READPACK;
		return;
	}
	else
	if (cmd[0] == 'R')
	{
		wS->nMode = WSFT_READPACKWSF;		
		return;
	}
	else
	if (cmd[0] == 't')
	{
		char cVal[100];
		char cTag[100];
		char *cPos;

		// Add Tag
		if (!cmd[1])
			return;

		cmd++;
		cPos = strchr(cmd,'=');

		if (!cPos)
			return;

		cPos[0] = 0;
		cPos++;

		// Copy over the tag and value
		strcpy(cTag,cmd);
		strcpy(cVal,cPos);

		cPos--;
		cPos[0] = '=';

		wS->nTags++;
		wS->cVals = (char**)realloc(wS->cVals,sizeof(char*)*wS->nTags);
		wS->cTags = (char**)realloc(wS->cTags,sizeof(char*)*wS->nTags);

		wS->cVals[wS->nTags-1] = (char*)malloc(strlen(cVal)+1);
		strcpy(wS->cVals[wS->nTags-1],cVal);

		wS->cTags[wS->nTags-1] = (char*)malloc(strlen(cTag)+1);
		strcpy(wS->cTags[wS->nTags-1],cTag);

		return;
	}
	else
	if (cmd[0] == 'T')
	{
		wS->nMode = WSFT_MODTAGS;
		return;
	}

	return;
}

//////////////////////////////////////////////////////
//  ReadWSFInfos()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 12:21:43 PM
/// \param *cFile File to analyze
/// \param *wS Settings
/// \brief Reads out WSF infos
/// 
// 
//////////////////////////////////////////////////////

void ReadWSFInfos( char *cFile, WSFTSets *wS )
{
	FILE *MFile;
	wsul nFull;
	wsul nComp;
	char cExt[5];
	char cPack[27];
	char cHead[4];
	wsfb bHi,bLo;
	wsfb bType;
	wsul nSamps;
	wsul nModSamps;
	CWSFPack *mP;
	CWSFAscii *mA;
	char cTitle[100];

	mA = g_wMod->GetAscii();
	mA->MakeTitle(cFile,cTitle,"%title% (%artist%)");

	memset(cPack,0,27);
	memset(cExt,0,5);
	MFile = fopen(cFile,"rb");
	if (!MFile){
		printf("Couldn't Open %s\n",cFile);
		return;
	}

	fread(cHead,1,4,MFile);
	if (strncmp(cHead,WSFMHEAD,4)!=0){
		printf("Invalid WSF Module\n");
		fclose(MFile);
		return;
	}

	// Get Version Stuff
	fread(&bHi,1,1,MFile);
	fread(&bLo,1,1,MFile);

	// Seek to pack name and get it			
	fread(cPack,1,26,MFile);

	if (TESTVER(1,11))
	{
		fread(cExt,1,5,MFile);
		bType = GetModType(cExt);
	}
	else
	{
		bType = WSFM_IT;
		strcpy(cExt,".IT");
	}

	// Read in Sizes
	fread(&nFull,1,sizeof(wsul),MFile);
	fread(&nComp,1,sizeof(wsul),MFile);

	// Seek around the mod data
	fseek(MFile,nComp,SEEK_CUR);

	// Read in sample count
	fread(&nModSamps,1,sizeof(wsul),MFile);

	fclose(MFile);

	printf("%s\n----------------------\n",cFile);
	printf("%s\n\n",cTitle);
	printf("Pack: %s\n",cPack);
	printf("MOD Header: %s",cExt);

	// Show Description
	if (bType != WSFM_NONE)
		printf(" (%s)\n",g_cModDesc[bType]);
	else
		printf(" (unsupported format)\n");
			
	printf("\n");

	printf("MOD Data Size: %.2fKB\n",(double)(nFull/1024.0));
	printf("Compressed MOD Data Size: %.2fKB\n",(double)(nComp/1024.0));
	printf("Compression Ratio: %u%%\n",100-MakePercentEx(nComp,nFull));
			
	printf("\n");

	// Get Pack
	mP = g_wMod->GetPack();

	if (mP->LoadPack(cPack,&nSamps,0)){
		printf("Couldn't open pack %s\n",cPack);
		return;
	}
			
	// Get Mod Stuff
	nSamps = mP->GetNumSamples();

	mP->FreePack();

	// Finish Reading
	printf("%u of %u samples used (%u%%)\n",nModSamps,nSamps,MakePercentEx(nModSamps,nSamps));


	printf("----------------------\n");
}

#ifdef MP3SUPPORT

//////////////////////////////////////////////////////
//  GetModLength()
// 
//  Description: 
/// Returns length of MOD in seconds
/// 
/// 
/// \author Josh Whelchel
/// \date 6/4/2003 8:56:11 PM
/// \param *cIn 
/// \return 
/// 
//////////////////////////////////////////////////////

wsul GetModLength( char *cIn )
{
	wsul nSecs;

	DUMBFILE *df;
	wsfb bType;
	CWSFMod *wMod;
	DUH *g_mDuh;
	wsf_modout wO;

	wMod = new CWSFMod;

	if (wMod->LoadMod(cIn)){
		printf("Invalid Module (%s)\n",cIn);
		return -1;
	}

	bType = wMod->GetModType2();

	if (wMod->WriteMod(&wO,FALSE)){
		printf("Error wMod->WriteMod() (%s)\n",cIn);
		return -1;
	}

	df = dumbfile_open_memory((const char *)wO.bModData, wO.nSize);
	
	switch (bType)
	{
	case WSFM_IT:
		g_mDuh = dumb_read_it(df);
		break;
	case WSFM_S3M:
		g_mDuh = dumb_read_s3m(df);
		break;
	case WSFM_XM:
		g_mDuh = dumb_read_xm(df);
		break;
	case WSFM_MOD:
		g_mDuh = dumb_read_mod(df);
		break;
	default:
		printf("WSFPlay does not support the internal module within %s\n",cIn);
		delete wMod;
		return -1;
	}

	wMod->FreeMod();
	wMod->FreePack();
	free(wO.bModData);
	wO.nSize = 0;
	dumbfile_close(df);

	delete wMod;


	nSecs = duh_get_length(g_mDuh);

	nSecs /= 65536;
//	nSecs *= 1000;

	if (g_mDuh)
		unload_duh(g_mDuh);

	return nSecs;
}

void mp3sDSPProc(void *data, sample_t **samples, int n_channels, long length)
{
	CWSFSTagHandler *gST;
	gST = (CWSFSTagHandler*)data;

	gST->ProcessDSPEffects(samples,length,n_channels,32,44100);
	return;
}

//////////////////////////////////////////////////////
//  WriteMp3()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/4/2003 9:05:06 PM
/// \param *wS  Settings
/// \param *cIn Input Filename
/// \param *cFile Output Filename
/// \brief Writes MP3
/// 
// 
//////////////////////////////////////////////////////

void WriteMp3( WSFTSets *wS, char *cIn, char *cFile )
{
	unsigned char mp3buffer[LAME_MAXMP3BUFFER];
    wsfb     Buffer[576*2*2];
    int     iread, imp3;
    int     frames;
	FILE *outf;
	DUMBFILE *df;
//	char cTitle[200];
//	CWSFAscii *mAsc;
	wsfb bType;
	lame_global_flags *lS;
	CWSFMod *wMod;
	id3tag iTag;
	DUH *g_mDuh;
	wsf_modout wO;
	int done=0;
	int nCount=0;
	float delta;
	long towrite=576*2*2;
	DUH_SIGRENDERER *dSig;
	wsul nLen;
	DUMB_IT_SIGRENDERER *itsr;
	CWSFSTagHandler gST;

	printf("Converting %s to %s...\n",cIn,cFile);

	wMod = new CWSFMod;

	wMod->SetPassSum(wS->nPassSum);
	if (wMod->LoadMod(cIn)){
		printf("Invalid Module (%s)\n",cIn);
		return;
	}

	if (wMod->GetPack()->m_nNeedPass && wS->nPassSum == 0)
		printf("Needs password! Use -x[pass] option\n");

	bType = wMod->GetModType2();

	if (wMod->WriteMod(&wO,FALSE)){
		printf("Error wMod->WriteMod() (%s)\n",cIn);
		return;
	}

//	mAsc = wMod->GetAscii();
//	mAsc->MakeTitle(cIn,cTitle,"%title% (%artist%)");

	df = dumbfile_open_memory((const char *)wO.bModData, wO.nSize);
	
	switch (bType)
	{
	case WSFM_IT:
		g_mDuh = dumb_read_it(df);
		break;
	case WSFM_S3M:
		g_mDuh = dumb_read_s3m(df);
		break;
	case WSFM_XM:
		g_mDuh = dumb_read_xm(df);
		break;
	case WSFM_MOD:
		g_mDuh = dumb_read_mod(df);
		break;
	default:
		printf("WSFPlay does not support the internal module within %s\n",cIn);
		delete wMod;
		return;
	}

	wMod->FreeMod();
	wMod->FreePack();
	free(wO.bModData);
	wO.nSize = 0;
	dumbfile_close(df);

	delete wMod;

	nLen = duh_get_length(g_mDuh);
	dSig = duh_start_sigrenderer(g_mDuh,0,2,0);
	itsr = duh_get_it_sigrenderer(dSig);
	
	// ANTI LOOP!
	dumb_it_set_bypassloops(itsr,1);
    dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL);
    dumb_it_set_xm_speed_zero_callback
                           (itsr, &dumb_it_callback_terminate, NULL);

	lS = lame_init();
	lame_set_brate(lS,wS->nArg[0]);

	delta = 65536.0f / 44100.0f;
	lame_set_num_samples(lS,576);
	lame_set_quality(lS,wS->nArg[1]);
	lame_set_num_channels(lS,2);
	lame_set_in_samplerate(lS,44100);

	lame_init_params(lS);

	duh_sigrenderer_set_dsp_callback(dSig,(DUH_SIGRENDERER_DSP_CALLBACK)mp3sDSPProc,&gST);
	gST.LoadTags(cIn);
	outf = fopen(cFile,"wb");
	gST.InitDSPEffects(32,2,44100);
	/* encode until we hit eof */
    do {

        /* read in 'iread' samples */
		iread = duh_render(dSig,16,0,1.0f,delta,576,Buffer);
        frames = lame_get_frameNum(lS);

		/* encode */
		imp3 = lame_encode_buffer_interleaved(lS,(short*)Buffer,iread,mp3buffer,sizeof(mp3buffer));

        /* was our output buffer big enough? */
        if (imp3 < 0) {
            if (imp3 == -1)
                fprintf(stderr, "mp3 buffer is not big enough... \n");
            else
                fprintf(stderr, "mp3 internal error:  error code=%i\n", imp3);
			fclose(outf);
			lame_close(lS);
            return;
        }

        if (fwrite(mp3buffer, 1, imp3, outf) != (unsigned)imp3) {
            fprintf(stderr, "Error writing mp3 output \n");
			fclose(outf);
			lame_close(lS);
            return;
        }

		nCount++;

		if (nCount==1)
			printf("%5d/%5d\b\b\b\b\b\b\b\b\b\b\b",frames,(int)(lame_get_totalframes(lS)*(nLen/65536)*19));

		if (nCount >= 75)
			nCount=0;

    } while (iread);

	imp3 = lame_encode_flush(lS, mp3buffer, sizeof(mp3buffer)); /* may return one more mp3 frame */

    if (imp3 < 0) {
        if (imp3 == -1)
            fprintf(stderr, "mp3 buffer is not big enough... \n");
        else
            fprintf(stderr, "mp3 internal error:  error code=%i\n", imp3);
		fclose(outf);
		lame_close(lS);
        return;
    }

    fwrite(mp3buffer, 1, imp3, outf);

	// Write Tag
	{
		char *cTag;
		int nSize;
		CWSFAscii mAsc;
		wsf_asciiout aO;
		int i;

		cTag = (char*)malloc(0);

		mAsc.GetAllTags(cIn,&aO);

		EmptyID3(&iTag);

		for (i=0;i<aO.nTags;i++)
		{
			if (stricmp(aO.cTags[i],"artist")==0)
				strncpy(iTag.cArtist,aO.cVals[i],30);
			else if (stricmp(aO.cTags[i],"title")==0)
				strncpy(iTag.cTitle,aO.cVals[i],30);
			else if (stricmp(aO.cTags[i],"game")==0)
				strncpy(iTag.cAlbum,aO.cVals[i],30);
			else if (stricmp(aO.cTags[i],"album")==0)
				strncpy(iTag.cAlbum,aO.cVals[i],30);
		}

		nSize = 0;
		cTag = SaveID3(&iTag,cTag,&nSize);

		fwrite(cTag,1,nSize,outf);
		free(cTag);

		mAsc.FreeAsciiOut(&aO);
	}

	fclose(outf);

	duh_end_sigrenderer(dSig);

	if (g_mDuh)
		unload_duh(g_mDuh);
	
	g_mDuh = NULL;

	lame_close(lS);

	return;
}



//////////////////////////////////////////////////////
//  RunMakeMp3()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/4/2003 8:42:13 PM
/// \param *wS WSFTool Settings
/// \brief Creates Mp3s out of input files
/// 
// 
//////////////////////////////////////////////////////

void RunMakeMp3( WSFTSets *wS )
{
	int nNum;
	char *cOut;
	char cFile[200];
	CWSFAscii mA;

	printf("WSF2MP3 (%s) @ %dkbps\nUses LAME technology\n\n",wS->cInput,wS->nArg[0]);

	if (wS->cOutputFormat[0])
		cOut = wS->cOutputFormat;
	else
		cOut = NULL;

	nNum = 0;

	// Search Files
	if (wS->nInputs > 0)
	{
		// File One
		char tmpstr[200];
		strcpy(tmpstr,wS->cInput[0]);

		// Output Name
		if (cOut != NULL){
			// Using the ascii, generate a filename :D
			mA.MakeTitle(tmpstr,cFile,cOut);
		}
		else
		{			// Else remove extension and add .wsf
			char *cExt;
			cExt = strrchr(tmpstr,'.');
			if (cExt)
				cExt[0]=0;
			sprintf(cFile,"%s.mp3",tmpstr);
			if (cExt)
				cExt[0]='.';
		}

		WriteMp3(wS,tmpstr,cFile);
		nNum++;

		for (int ix=1;ix<wS->nInputs;ix++)
		{
			strcpy(tmpstr,wS->cInput[ix]);
			// File Two+

			// Output Name
			if (cOut != NULL){
				// Using the ascii, generate a filename :D
				mA.MakeTitle(tmpstr,cFile,cOut);
			}
			else
			{			// Else remove extension and add .wsf
				char *cExt;
				cExt = strrchr(tmpstr,'.');
				if (cExt)
					cExt[0]=0;
				sprintf(cFile,"%s.mp3",tmpstr);
				if (cExt)
					cExt[0]='.';
			}
			
			WriteMp3(wS,tmpstr,cFile);
			nNum++;

		}
	}

	printf("%d MP3s encoded!\n",nNum);

	return;
}

/*

  <Clubsoft> function getTime(secs) {
<Clubsoft> 	var tt;
<Clubsoft> 	hour=Math.floor(secs/3600);
<Clubsoft> 	min=Math.floor((secs-(hour*3600))/60);
<Clubsoft> 	sec=secs-(hour*3600)-(min*60);
<Clubsoft> 	tt = ""
<Clubsoft> 	tt += (hour < 10) ? "0" + hour + ":" : hour + ":";
<Clubsoft> 	tt += (min < 10) ? "0" + min + ":" : min + ":";
<Clubsoft> 	tt += (sec < 10) ? "0" + sec: sec;
<Clubsoft> 	return(tt);
<Clubsoft> }

  */

//////////////////////////////////////////////////////
//  GetTime()
// 
//  Description: 
/// Converts seconds to hh:mm:ss etc
/// 
/// 
/// \author Clubsoft (Port:Josh Whelchel)
/// \date 6/4/2003 9:16:25 PM
/// \param nSecs Seconds
/// \param *nHour Out: Hours
/// \param *nMin Out: Minutes
/// \param *nSec Out: Seconds
//////////////////////////////////////////////////////

void GetTime( wsul nSecs, wsul *nHour, wsul *nMin, wsul *nSec )
{
	wsul h,m,s;
	h = (wsul)floor((double)(nSecs/3600));
	m = (wsul)floor((double)(nSecs-(h*3600))/60);
	s = (wsul)floor((double)nSecs-(h*3600)-(m*60));

	nHour[0] = h;
	nMin[0] = m;
	nSec[0] = s;

}

//////////////////////////////////////////////////////
//  CountLength()
// 
//  Description: 
/// Counts the length of all the modules and displays
/// 
/// 
/// \author Josh Whelchel
/// \date 6/4/2003 8:55:20 PM
/// \param *wS Settings
//////////////////////////////////////////////////////

void CountLength( WSFTSets *wS )
{
	wsul nSecs;
	wsul nRet;
	wsul nHour,nMin,nSec;
	nSecs = 0;

	dumb_set_bypassloops(1);

	// Search Files
	if (wS->nInputs > 0)
	{
		char tmpstr[200];
		strcpy(tmpstr,wS->cInput[0]);
		// File One

		printf("%s...\n",tmpstr);
		nRet = GetModLength(tmpstr);

		if (nRet == -1)
		{
			printf("Blah at %s\n",tmpstr);
			return;
		}
		nSecs += nRet;

		for (int ix=1;ix<wS->nInputs;ix++)
		{
			strcpy(tmpstr,wS->cInput[ix]);
			// File Two+
			
			printf("%s...\n",tmpstr);
			nRet = GetModLength(tmpstr);

			if (nRet == -1)
			{
				printf("Blah at %s\n",tmpstr);
				return;
			}
			nSecs += nRet;
		}
	}

	//printf("Total seconds: %d\n",(int)nSecs);
	GetTime(nSecs,&nHour,&nMin,&nSec);

	printf("\n%.2d:%.2d:%.2d\n",(int)nHour,(int)nMin,(int)nSec);


	return;
}

#endif // MP3SUPPORT

//////////////////////////////////////////////////////
//  MakeWGM()
// 
//  Description: 
/// Packs input together to create a WGM pack
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:42:18 PM
/// \param *wS Sets
//////////////////////////////////////////////////////

void MakeWGM( WSFTSets *wS )
{
	wsul nStart;
	CWGMPack *wPack;
	wsul id;
	wsfb nType;
	char cID[9];

	wPack = new CWGMPack;

	wPack->LoadPack(wS->cArg1);
	nStart = wPack->GetNumUnits();

	// Search Files
	if (wS->nInputs > 0)
	{
		char tmpstr[200];
		strcpy(tmpstr,wS->cInput[0]);
		wsul nID;

		// File One
		nStart++;

		cID[8] = 0;
		strncpy(cID,tmpstr,3);
		sprintf(cID+3,"%d",nStart);

		nType = (wsfb)wS->nArg[0];

		if (nType == WGMT_TUNE)
		{
			wsf_file *wF;
			char cHead[5];
			wF = wsfopenfile(tmpstr);

			cHead[0]=0;
			if (wF)
			{
				wsfread(cHead,4,wF);
				wsfclose(wF);
			}

			if (strncmp(cHead,WGMHEAD,4)==0)
				nType = WGMT_PACK;
		}

		nID = wPack->AddUnitFile(cID,tmpstr,nType);

		printf("Added %s with cID '%s' & nID #%d\n",tmpstr,cID,nID);

		for (int ix=1;ix<wS->nInputs;ix++)
		{
			strcpy(tmpstr,wS->cInput[ix]);
			// File Two+
			nStart++;

			cID[8] = 0;
			strncpy(cID,tmpstr,3);
			sprintf(cID+3,"%d",nStart);

			nType = (wsfb)wS->nArg[0];

			if (nType == WGMT_TUNE)
			{
				wsf_file *wF;
				char cHead[5];
				wF = wsfopenfile(tmpstr);

				cHead[0]=0;
				if (wF)
				{
					wsfread(cHead,4,wF);
					wsfclose(wF);
				}

				if (strncmp(cHead,WGMHEAD,4)==0)
					nType = WGMT_PACK;
			}

			nID = wPack->AddUnitFile(cID,tmpstr,(wsfb)wS->nArg[0]);

			printf("Added %s with cID '%s' & nID #%d\n",tmpstr,cID,nID);
		}
	}

	id = wPack->GetUnitPackID();

	if (id != 666)
		printf("%d is the ID of the first pack found\n",id);
	else
		printf("No pack is included in this WGM\n");

	wPack->SavePack(wS->cArg1);

	delete wPack;

	return;
}

//////////////////////////////////////////////////////
//  UnpackWGM()
// 
//  Description: 
/// Unpacks WGM file into current directory
/// 
/// 
/// \author Josh Whelchel
/// \date 7/22/2003 8:55:23 PM
/// \param *wS hmm yes
//////////////////////////////////////////////////////

void UnpackWGM( WSFTSets *wS )
{
	wsul i;
	wsul id;
	wsul nUnits;
	CWGMPack *wPack;
	char *cExt;
	wPack = new CWGMPack;

	if (wPack->LoadPack(wS->cInput[0]))
	{
		delete wPack;
		printf("Invalid pack file specified!\n");
		return;
	}

	nUnits = wPack->GetNumUnits();
	for (i=0;i<nUnits;i++)
	{
		wgmunit *wUnit;
		FILE *pFile;
		char cFName[17];
		char *cDot;

		wUnit = wPack->GetUnit(i);

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

		pFile = fopen(cFName,"wb");

		if (!pFile){
			printf("Could not open for writing %s (ID:%d)\n",cFName,i);
			continue;
		}

		fwrite(wUnit->bData,1,wUnit->nSize,pFile);
		fclose(pFile);

		printf("Wrote unit #%d as '%s'\n",i,cFName);
	}

	id = wPack->GetUnitPackID();
	if (id == 666)
		printf("No pack was extracted because none was included!\n");
	else
	{
		wgmunit *wUnit;
		char cFName[17];
		char *cDot;

		wUnit = wPack->GetUnit(id);

		strncpy(cFName,wUnit->cID,8);
		cDot = strchr(cFName,'.');

		if (cDot)
			cDot[0] = 0;

		sprintf(cFName,"%s.wsp",cFName);

		printf("Pack is '%s' and will need to be renamed!!\n",cFName);
	}

	delete wPack;
	return;
}

//////////////////////////////////////////////////////
//  NameMods()
// 
//  Author:
/// \author Josh Whelchel
/// \date 4/7/2003 11:17:00 AM
/// \param *wS Settings
/// \brief Names WSF files for cOutputFormat
/// 
// 
//////////////////////////////////////////////////////

void NameMods( WSFTSets *wS )
{
	char *cOut;
	char cCmd[200];
	char cFile[200];
	CWSFAscii mA;
	cOut = wS->cOutputFormat;

	// Search Files
	if (wS->nInputs > 0)
	{
		char tmpstr[200];
		strcpy(tmpstr,wS->cInput[0]);
		// File One

		// Output Name

		// Using the ascii, generate a filename :D
		mA.MakeTitle(tmpstr,cFile,cOut);

		// Rename
		sprintf(cCmd,"mv \"%s\" \"%s\"",tmpstr,cFile);
		system(cCmd);

		for (int ix=1;ix<wS->nInputs;ix++)
		{
			// File Two+
			strcpy(tmpstr,wS->cInput[ix]);

			// Output Name
		
			// Using the ascii, generate a filename :D
			mA.MakeTitle(tmpstr,cFile,cOut);
			
			// Rename
			sprintf(cCmd,"mv \"%s\" \"%s\"",tmpstr,cFile);
			system(cCmd);

		}
	}	

}

//////////////////////////////////////////////////////
//  ModTags()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/23/2003 3:46:49 PM
/// \param *cFile File to Modify
/// \param *wS Settings
/// \brief Modifies Tags from WSFTSets
/// 
// 
//////////////////////////////////////////////////////

void ModTags( char *cFile, WSFTSets *wS )
{
	CWSFAscii *mA;
	mA = g_wMod->GetAscii();

	printf("Tagging %s...\n",cFile);
	mA->AddWSFTags(cFile,wS->cTags,wS->cVals,wS->nTags);
}

//////////////////////////////////////////////////////
//  main()
// 
//  Author:
/// \author Josh Whelchel
/// \date 2/22/2003 7:51:45 PM
/// \param argc Number of Parametes
/// \param argv Parameters
/// \return Main Return
/// \brief Main Entry Point for WSFTool
/// 
// 
//////////////////////////////////////////////////////

#ifndef DOXYSKIP
int main( int argc, char **argv )
#else
int main_WSFTool( int argc, char **argv )
#endif
{
	wsul x;
	int i;
	WSFTSets wSets;
	wsul nSamps;
	double nAvg;
	CWSFPack *mP;
	g_wMod = new CWSFMod;
	g_mAsc = g_wMod->GetAscii();

	memset(&wSets,0,sizeof(WSFTSets));
	wSets.nMode = WSFT_NONE;

#ifdef MP3SUPPORT
	dumb_set_bypassloops(1);
#endif

	printf("WSFTool v%d.%.2d by Josh Whelchel\nWSFPack v%d.%.2d + WSFMod v%d.%.2d\nSyntesis Entertainment + Apocalyptic Coders\nwww.acoders.com\n\n",WSFTVERHI,WSFTVERLO,WSFPVERHI,WSFPVERLO,WSFMVERHI,WSFMVERLO);

	if (argc < 2)
	{
		ShowUsage(0);
		printf("\n");
		delete g_wMod;
		return 1;
	}

	// Makes a full command line and sets settings
	{
		char cCmdLine[200];		
		cCmdLine[0] = 0;

		// Resets Tags
		wSets.nTags = 0;
		wSets.cTags = (char**)malloc(0);
		wSets.cVals = (char**)malloc(0);

		// Make full command line
		for (i=1;i<argc;i++)
		{
/*			strcat(cCmdLine,"\"");
			strcat(cCmdLine,argv[i]);
			strcat(cCmdLine,"\" ");
*/
			FillSetting(argv[i],i-1,&wSets);
		}

//		FillSettings(cCmdLine,&wSets);
	}

	if (wSets.nMode == WSFT_NONE || wSets.nMode == WSFT_HELP)
	{
		ShowUsage((wSets.nMode == WSFT_HELP));
		printf("\n");
		delete g_wMod;
		return 1;
	}

	mP = NULL;

	// Do Main Stuffs
	switch (wSets.nMode)
	{
	case WSFT_TOWSF:
		RunTOWSF(&wSets);
		break;
	case WSFT_WSFTO:
		RunWSFTO(&wSets);
		break;

#ifdef MP3SUPPORT
	case WSFT_MAKEMP3:
		RunMakeMp3(&wSets);
		break;

	case WSFT_LENGTH:
		CountLength(&wSets);
		break;
#endif

	case WSFT_NAME:
		// Rename Mods
		NameMods(&wSets);
		break;

	case WSFT_MAKEWGM:
		// Creates WGM
		MakeWGM(&wSets);
		break;

	case WSFT_LOADWGM:
		UnpackWGM(&wSets);
		break;

	case WSFT_COMPSAMP:
		{
			CWSFPack mP;

			// Set password sum!
			mP.SetPassSum(wSets.nPassSum);

			if (mP.LoadPack(wSets.cInput[0],NULL,0))
			{
				printf("Could not open %s as a wsf samplepack.\n",wSets.cInput[0]);
				break;
			}

			mP.SetCompressionFlag(wSets.nArg[0],1);

			// Set password sum!
			mP.SetPassSum(wSets.nPassSum);

			if (mP.SavePack(wSets.cInput[0],1))
			{
				printf("Couldn't save %s!\n",wSets.cInput[0]);
				break;
			}

			break;
		}

	case WSFT_LISTSAMP:
		{
			CWSFPack mP;
			wsul i;

			// Set password sum!
			mP.SetPassSum(wSets.nPassSum);

			if (mP.LoadPack(wSets.cInput[0],NULL,0))
			{
				printf("Could not open %s as a wsf samplepack.\n",wSets.cInput[0]);
				break;
			}

			for (i=0;i<mP.GetNumSamples();i++)
			{
				// Holy Crapsticks.
				wsf_sampout wO;
				char cName[28];
				mP.GetSamp(&wO,i,WSFS_NORMAL,0,100);
				mP.GetSampleName(i,cName);

				printf("%u|%s|%u|%c|%u%c%c",i,cName,wO.nSize,(wO.nOgg?'Y':'N'),
					wO.nStoredSize,10,13);

				free(wO.bSampData);
			}

			break;
		}

	case WSFT_REMPASS:
		{
			CWSFPack mP;

			if (!wSets.nPassSum)
			{
				printf("You did not supply a password to unlock the sample pack.\nUse -x[pass] option to supply a password!\n");
				break;
			}

			// Set password sum!
			mP.SetPassSum(wSets.nPassSum);

			if (mP.LoadPack(wSets.cInput[0],NULL,0))
			{
				printf("Could not open %s as a wsf samplepack.\n",wSets.cInput[0]);
				break;
			}

			// Set Password sum off!
			mP.SetPassSum(0);

			if (mP.SavePack(wSets.cInput[0],1))
			{
				printf("Couldn't save %s!\n",wSets.cInput[0]);
				break;
			}

			printf("\nPassword removed!\nAs a courtesy to the original artist please\ndo not distribute this sample pack.\n");
			printf("If you gave the wrong password, then the\nsample pack now has a lost\npassmod and cannot be recovered!!\n");

			break;
		}

	case WSFT_SHOWEXT:

		// Show Extensions

		printf("Supported Extensions:\n");
		for (i=0;i<WSFM_COUNT;i++)
		{
			if (g_bModFlag[i])
				printf("%s (*%s)\n",g_cModDesc[i],g_cModExt[i]);
		}

		break;

	case WSFT_MODTAGS:

		// Modifies Tags
		{
			// Search Files
			if (wSets.nInputs > 0)
			{
				ModTags(wSets.cInput[0],&wSets);

				for (int ix=1;ix<wSets.nInputs;ix++)
				{
					ModTags(wSets.cInput[ix],&wSets);
				}
			}

			break;
		}

	case WSFT_READPACKWSF:

		{
			printf("WSF Reading\n");

			// Search Files
			if (wSets.nInputs > 0)
			{
				ReadWSFInfos(wSets.cInput[0],&wSets);

				for (int ix=1;ix<wSets.nInputs;ix++)
				{
					printf("\n");
					ReadWSFInfos(wSets.cInput[ix],&wSets);
				}
			}

			break;
		}

	case WSFT_READPACK:

		// Read Pack

		// Get Pack
		mP = g_wMod->GetPack();

		if (mP->LoadPack(wSets.cInput[0],&nSamps,0)){
			printf("Couldn't open pack %s\n",wSets.cInput[0]);
			break;
		}

		// Get # of Samples
		nSamps = mP->GetNumSamples();

		// Get Average Size
		nAvg = 0.0;
		for (x=0;x<nSamps;x++)
		{
			nAvg += (double)mP->GetSampSize(x);
		}

		nAvg = nAvg/(double)nSamps;

		// Print out Data
		printf("Pack %s\n------------------\n",wSets.cInput[0]);
		printf("Number of Samples: %d\n",(int)nSamps);
		printf("Average Sample Size: %.0fKB\n",nAvg/1024.0);
		

		// Free Pack
		mP->FreePack();

		break;
	}

	for (i=0;i<wSets.nTags;i++){
		free(wSets.cVals[i]);
		free(wSets.cTags[i]);
	}

	free(wSets.cVals);
	free(wSets.cTags);

	printf("\n");
	delete g_wMod;
	return 0;
}

/*!
	}@
*/

