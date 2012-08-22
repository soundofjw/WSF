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
/// \file MeltWSF.cpp 
/// Main MeltWSF source

/*! \addtogroup MeltWSF
*  A small console program to contain and extract WSF
*  Good for One Hour Compos
*  Built from MeltWSF.cpp
*  @{
*/


#define VERHI 0
#define VERLO 1

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "WSFGlob.h"
#include "WSFMod.h"

#ifndef stricmp
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

/// MeltWSF Version Hi x.00
#define MELTVERHI	0
/// MeltWSF Version Lo 1.xx
#define MELTVERLO	10

MiniWSFData g_WD;

#define MIN(x,y) ((x < y)?x:y)

#ifdef WIN32
#ifndef stricmp
//////////////////////////////////////////////////////
// stricmp()
// 
// Description
/// in-line stricmp function (because we want the smallest build size ever)
/// 
/// 
/// \author Josh Whelchel
/// \date 4/1/2004 9:09:02 PM
/// \param *s1 
/// \param *s2 
/// \brief 
///
//
//////////////////////////////////////////////////////

int stricmp( const char *s1, const char *s2 )
{
	int len,len1,len2;
	int c1, c2; 
	len1 = strlen(s1);
	len2 = strlen(s2);

	len = MIN(len1, len2);

	while (len--) {
		c1 = tolower((int)*(unsigned char *)s1++);
		c2 = tolower((int)*(unsigned char *)s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	} 
	return len1 - len2;

}
#endif
#endif

int MakeMelt( int argc, char **argv )
{
	int nFiles;
	int x;
	CWSFMod wMod;
	CWSFPack *wP;
	wsfb *bPack;
	wsul nPack;

	int i;
	
	int compile = 0;
	
	if (strnicmp(argv[1],"c",1)==0)
		compile=1;
	
	if (!compile)	
		nFiles = argc-5;
	else
		nFiles = argc-6;

	for (i=0;i<nFiles;i++)
	{
		// Directory
		char *cCur;
		
		cCur = argv[i+5+compile];
				
		g_WD.cFiles = (char**)realloc(g_WD.cFiles,sizeof(char*)*nFiles);
		g_WD.cFiles[i] = (char*)malloc(strlen(cCur)+1);

		strcpy(g_WD.cFiles[i],cCur);
	}

	/// MAKE!
	g_WD.nMods = nFiles;
	g_WD.bMods = (wsfb**)malloc(sizeof(wsfb*)*nFiles);
	g_WD.nModSizes = (wsul*)malloc(sizeof(wsul)*nFiles);

	wMod.SetPackName("");

	for (x=0;x<nFiles;x++)
	{
		wsf_modout mo;

		printf("Storing %s..",g_WD.cFiles[x]);
		if (wMod.LoadMod(g_WD.cFiles[x])){
			printf("Bad Module: %s\n",g_WD.cFiles[x]);
			break;
		}
		if (wMod.WriteMod(&mo,1)){
			printf("Bad Module: %s\n",g_WD.cFiles[x]);
			break;
		}
		printf(".\n");
			
		g_WD.bMods[x] = mo.bModData;
		g_WD.nModSizes[x] = mo.nSize;
	}

	if (x!=nFiles)
		return 1;

	printf("Storing Pack..");
	wP = wMod.GetPack();
	if (wP->SavePack(&bPack,&nPack,1)){
		printf("Pack Invalid?!\n");
		return 1;
	}

	printf(".\n");

	// Save Filebum
	{
		wsul num;
		wsul pnum;
		wsf_file *fout = 0;
		wsf_file *fin = 0;

		pnum = 0;

		printf("Saving... %s\n",argv[2]);

		fout = wsfcreatefile(argv[2]);
		if (!fout){
			printf("Couldn't create %s\n",argv[2]);
			return 1;
		}
		
		if (compile)
		{
			fin = wsfopenfile(argv[3]);
			if (!fin){
				printf("Couldn't open %s\n",argv[3]);
				wsfclose(fout);
				return 1;
			}
			
			wsfb *cData = (wsfb*)malloc(fin->nSize);
			wsfread(cData,fin->nSize,fin);
			wsfwrite(cData,fin->nSize,fout);
			free(cData);
			pnum = fin->nSize;
			wsfclose(fin);
		}

		num = 666;
		wsfwrite(&num,sizeof(wsul),fout);

		wsfwrite(argv[3+compile],30,fout);
		wsfwrite(argv[4+compile],255,fout);
		wsfwrite(&nFiles,sizeof(wsul),fout);
		wsfwrite(&nPack,sizeof(wsul),fout);

		for (x=0;x<nFiles;x++)
		{
			wsul size;
			char *cN;

			// Make the Pack Name Small
			cN = strrchr(g_WD.cFiles[x],'\\');
			if (!cN)
				cN = strrchr(g_WD.cFiles[x],'/');
			
			if (!cN)
				cN = g_WD.cFiles[x];
			else
				cN++;

			wsfwrite(&g_WD.nModSizes[x],sizeof(wsul),fout);
			wsfwrite(g_WD.bMods[x],g_WD.nModSizes[x],fout);
			size = strlen(cN);
			wsfwrite(&size,sizeof(wsul),fout);
			wsfwrite(cN,size,fout);
			free(g_WD.cFiles[x]);
			free(g_WD.bMods[x]);
		}

		free(g_WD.cFiles);
		free(g_WD.nModSizes);
		free(g_WD.bMods);

		wsfwrite(bPack,nPack,fout);
		wsfwrite(&pnum,sizeof(wsul),fout);
		wsfclose(fout);
		free(bPack);
	}

	printf("%s created!\n",argv[2]);

	return 0;
}

void DoExtractMelt( void )
{
	wsul i;
	wsf_file *fPack;
	CWSFMod wMod;
	
	fPack = wsfopenmem(g_WD.bPack,g_WD.nPackSize,1);
	wMod.SetPack(fPack);

	for (i=0;i<g_WD.nMods;i++)
	{
		wsf_file *fout;
		wsf_modout mo;

		printf("%s...\n",g_WD.cFiles[i]);

		wMod.SetPack(fPack);
		wMod.LoadMod(g_WD.bMods[i],g_WD.nModSizes[i],0,NULL,WSFM_WSF);
		wMod.WriteMod(&mo,0);

		fout = wsfcreatefile(g_WD.cFiles[i]);
		if (!fout){
			printf("Could not create %s!\n",g_WD.cFiles[i]);
			continue;
		}
		wsfwrite(mo.bModData,mo.nSize,fout);
		wsfclose(fout);
	}

	printf("Done!\n\n");

	wMod.SetPack(NULL);
	wsfclose(fPack);

	return;
}

//////////////////////////////////////////////////////
// main_MeltWSF()
// 
// Description
/// Handles main MeltWSF execution
/// 
/// \author Josh Whelchel
/// \date 7/6/2004 7:35:41 PM
/// \param argc # of params
/// \param **argv  params
///
//
//////////////////////////////////////////////////////

#ifndef DOXYSKIP
int main( int argc, char **argv )
#else
int main_MeltWSF( int argc, char **argv )
#endif
{
	wsf_file *fd;
	wsul nPos;
	wsul i;

	if (argc < 2)
	{
		printf("MeltWSF v%d.%.2d by Josh Whelchel\nWSFPack v%d.%.2d + WSFMod v%d.%.2d\nSyntesis Entertainment + Apocalyptic Coders\nwww.acoders.com\n\n",MELTVERHI,MELTVERLO,WSFPVERHI,WSFPVERLO,WSFMVERHI,WSFMVERLO);
		printf("Syntax:\n%s input.exe\n"
			"%s build output.mwp title \"message\" file1.it file2.xm ...\n"
			"%s compile output.mwp miniwsf.exe title \"message\" file1.it file2.xm ...\n"
			"\ninput can be a MiniWSF EXE or MWP\n\n",argv[0],argv[0],argv[0]);
		return 1;
	}

	memset(&g_WD,0,sizeof(MiniWSFData));

	if (argc >= 6 && (stricmp(argv[1],"build")==0 || stricmp(argv[1],"b")==0))
		return MakeMelt(argc,argv);
	
	if (argc >= 7 && (stricmp(argv[1],"compile")==0 || stricmp(argv[1],"c")==0))
		return MakeMelt(argc,argv);

	if (argc < 6 && (stricmp(argv[1],"build")==0 || stricmp(argv[1],"b")==0)){
		printf("Syntax:\n%s build output.wmp title \"message\" file1.it file2.xm\n",argv[0]);
		return 1;
	}
	
	if (argc < 6 && (stricmp(argv[1],"compile")==0 || stricmp(argv[1],"c")==0)){
		printf("Syntax:\n%s compile output.wmp miniwsf.exe title \"message\" file1.it file2.xm\n",argv[0]);
		return 1;
	}

	fd = wsfopenfile(argv[1]);

	if (!fd){
		printf("Couldn't open %s\n",argv[1]);
		return 1;
	}

	printf("Seeking to posbyte...");
	wsfend(-1*(signed)sizeof(wsul),fd);
	wsfread(&nPos,sizeof(wsul),fd);
	printf(" %d\n",nPos);

	if (nPos >= fd->nSize){
		wsfclose(fd);
		printf("File not valid!\n");
		return 1;
	}

	wsfbegin(nPos,fd);
	wsfread(&nPos,sizeof(wsul),fd);

	if (nPos != 666){
		wsfclose(fd);
		printf("File not valid!\n");
		return 1;
	}

	wsfread(g_WD.cTitle,30,fd);
	g_WD.cTitle[30] = 0;

	wsfread(g_WD.cMessage,255,fd);
	g_WD.cMessage[255] = 0;

	wsfread(&g_WD.nMods,sizeof(wsul),fd);
	wsfread(&g_WD.nPackSize,sizeof(wsul),fd);

	g_WD.bMods = (wsfb**)malloc(sizeof(wsfb*)*g_WD.nMods);
	g_WD.nModSizes = (wsul*)malloc(sizeof(wsul)*g_WD.nMods);
	g_WD.cFiles = (char**)malloc(sizeof(char*)*g_WD.nMods);

	for (i=0;i<g_WD.nMods;i++)
	{
		wsul size;
		wsfread(&g_WD.nModSizes[i],sizeof(wsul),fd);
		g_WD.bMods[i] = (wsfb*)malloc(g_WD.nModSizes[i]);
		wsfread(g_WD.bMods[i],g_WD.nModSizes[i],fd);
		wsfread(&size,sizeof(wsul),fd);
		g_WD.cFiles[i] = (char*)malloc(size+1);
		wsfread(g_WD.cFiles[i],size,fd);
		g_WD.cFiles[i][size] = 0;
	}
	
	g_WD.bPack = (wsfb*)malloc(g_WD.nPackSize);
	wsfread(g_WD.bPack,g_WD.nPackSize,fd);
	wsfclose(fd);

	{
		wsul size;
		size = g_WD.nPackSize;
		for (i=0;i<g_WD.nMods;i++)
			size += g_WD.nModSizes[i];
		printf("Title: %s\nMessage: %s\n"
				"%d songs contained.\n"
				"Total Size: %.2fKB\n\n"
				"",
				g_WD.cTitle,
				g_WD.cMessage,				
				g_WD.nMods,
				(float)(size/1024)
				);
	}
	
	{
		char c;
		
		printf("Do you want to extract these files here? (y/n)\n");
		getchar();
		c = getchar();
		
		if (c == 'y' || c == 'Y'){
			printf("\n");
			DoExtractMelt();
		}
	}

	// Free
	free(g_WD.bPack);
	for (i=0;i<g_WD.nMods;i++){
		free(g_WD.bMods[i]);
		free(g_WD.cFiles[i]);
	}
	free(g_WD.cFiles);
	free(g_WD.nModSizes);
	free(g_WD.bMods);

	return 0;
}

/// }@

