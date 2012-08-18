/*
	Whelchel Sound Format
		Based on MODs

  Josh Whelchel 2003
  www.syntesis.ath.cx

*/
 
/* LICENSE
-=-=-=-=-=-=-=-

	This source is released on a do-not-modify-and-call-it-your-own-because-your-selfish
  license. If you're going to change ANYTHING about the engine and release it either
  open source, or as a major format then it will require my permission first. I ask
  that if you use my technology, that I get due respect and credit. 

	Also, I like to hear when people use my format, give me an email and tell me about
  it. joshwhelchel@hotmail.com or siggy2336@hotmail.com will work.

  ~ Josh Whelchel 2003
  wsf.syntesis.ath.cx

*/


/// \file MiniWSF.cpp
/// A small windows program to self contain and extract WSF's

/*! \addtogroup MiniWSF
*  A small windows program to self contain and extract WSF
*  Good for One Hour Compos
*  Built from MiniWSF.cpp
*  @{
*/

#ifdef WIN32

#define VERHI 0
#define VERLO 1

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "resource.h"
#include "WSFGlob.h"
#include "WSFMod.h"
#include "WGMPack.h"

#define MIN(x,y) ((x < y)?x:y)

HINSTANCE hinst;
MiniWSFData g_WD;

//////////////////
/* File Dialogs */
//////////////////

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

// GetSaveName( Parent, Target, Filter, Number of Filters )
//		Returns the filename that user chooses
int GetSaveName(HWND hWnd, char *szF, char *Filter, int numfil, char *defext)
{
    OPENFILENAME ofn;

	szF[0] = '\0';

    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = Filter;
    ofn.nFilterIndex = numfil;
    ofn.lpstrFile = szF;
    ofn.nMaxFile = 128;
    ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName((LPOPENFILENAME)&ofn) == 0)
        return -1;

	if (ofn.nFileExtension >= strlen(szF))
		strcat(szF,defext);
	
	if (ofn.nFileExtension == 0) {
		strcat(szF,".");
		strcat(szF,defext);
	}

	
    return 0;
}

int BuildMake( HWND h, char *cTitle, char *cMessage )
{
	long nFiles;
	long x;
	char cOut[4500];
	char OutFile[MAX_PATH];
	OPENFILENAME opf;
	CWSFMod wMod;
	CWSFPack *wP;
	wsfb *bPack;
	wsul nPack;
	memset(&opf,0,sizeof(OPENFILENAME));

	opf.lStructSize = sizeof(OPENFILENAME);
	opf.hwndOwner = h;
	opf.hInstance = hinst;
	opf.Flags = OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_EXPLORER;
	opf.lpstrFilter = "Supported Files (*.MOD;*.XM;*.IT;*.S3M)\0*.mod;*.it;*.xm;*.s3m\0\0";
	opf.lpstrTitle = "Add Files";
	opf.nMaxFile = 4500;
	opf.lpstrFile = cOut;
	memset(cOut,0,4500);

	if (GetOpenFileName(&opf)==0)
		return 1;

	FILE *fTest;
	int nDir;

	nDir=0;
	fTest = fopen(cOut,"rb");
	if (!fTest)
		nDir=1;
	else
		fclose(fTest);

	nFiles = 0;

	if (nDir==0)
	{
		nFiles = 1;
		g_WD.cFiles = (char**)malloc(sizeof(char*));
		g_WD.cFiles[0] = (char*)malloc(strlen(cOut)+1);
		strcpy(g_WD.cFiles[0],cOut);
	}
	else
	{
		// Directory
		char *cCur;
		char cTemp[200];
		cCur = opf.lpstrFile + opf.nFileOffset;

		while (1)
		{
			char *c;
			strcpy(cTemp,cOut);
			strcat(cTemp,"\\");
			strcat(cTemp,cCur);

			nFiles++;
			g_WD.cFiles = (char**)realloc(g_WD.cFiles,sizeof(char*)*nFiles);
			g_WD.cFiles[nFiles-1] = (char*)malloc(strlen(cTemp)+1);
			strcpy(g_WD.cFiles[nFiles-1],cTemp);

			c = cCur + strlen(cCur);
			c++;

			if (c[1] == '\0')
				break;

			cCur = c;
		}
	}

	/// MAKE!
	g_WD.nMods = nFiles;
	g_WD.bMods = (wsfb**)malloc(sizeof(wsfb*)*nFiles);
	g_WD.nModSizes = (wsul*)malloc(sizeof(wsul)*nFiles);

	wMod.SetPackName("");

	for (x=0;x<nFiles;x++)
	{
		wsf_modout mo;
		if (wMod.LoadMod(g_WD.cFiles[x]))
			break;
		if (wMod.WriteMod(&mo,1))
			break;
		
		g_WD.bMods[x] = mo.bModData;
		g_WD.nModSizes[x] = mo.nSize;
	}

	if (x!=nFiles){
		MessageBox(h,"Error with one of the modules!","MiniWSF",MB_OK);
		return 0;
	}

	wP = wMod.GetPack();
	wP->SavePack(&bPack,&nPack,1);

	if (GetSaveName(h,OutFile,"MiniWSF Executable (*.exe)\0*.exe\0\0",1,"exe")==-1)
		return 1;

	// Save Filebum
	{
		char cFile[200];
		wsul num;
		wsul pnum;
		wsf_file *fbit;
		wsf_file *fout;
		wsfb *bd;

		GetModuleFileName(NULL,cFile,200);
		
		fbit = wsfopenfile(cFile);
		if (!fbit){
			free(bPack);
			return 0;
		}

		pnum = fbit->nSize;

		// Check and see the length bit.
		{
			wsul nPos;

			wsfend(-1*(signed)sizeof(wsul),fbit);
			wsfread(&nPos,sizeof(wsul),fbit);

			if (nPos < fbit->nSize){
				wsfbegin(nPos,fbit); 
				wsfread(&nPos,sizeof(wsul),fbit);

				if (nPos == 666)
					pnum = fbit->nPos-sizeof(wsul);				
			}
			wsfbegin(0,fbit);
		}

		bd = (wsfb*)malloc(pnum);
		wsfread(bd,pnum,fbit);
		wsfclose(fbit);

		fout = wsfcreatefile(OutFile);
		num = 666;
		wsfwrite(bd,pnum,fout);
		wsfwrite(&num,sizeof(wsul),fout);

		free(bd);

		wsfwrite(cTitle,30,fout);
		wsfwrite(cMessage,255,fout);
		wsfwrite(&nFiles,sizeof(wsul),fout);
		wsfwrite(&nPack,sizeof(wsul),fout);

		for (x=0;x<nFiles;x++)
		{
			wsul size;
			char *cN;

			// Make the Pack Name Small
			cN = strrchr(g_WD.cFiles[x],'\\');
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

		wsfwrite(bPack,nPack,fout);
		wsfwrite(&pnum,sizeof(wsul),fout);
		wsfclose(fout);
		free(bPack);
		free(g_WD.bMods);
		free(g_WD.nModSizes);
	}

	return 0;
}

void DoExtract( void )
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
		wMod.SetPack(fPack);
		wMod.LoadMod(g_WD.bMods[i],g_WD.nModSizes[i],0,NULL,WSFM_WSF);
		wMod.WriteMod(&mo,0);

		fout = wsfcreatefile(g_WD.cFiles[i]);
		wsfwrite(mo.bModData,mo.nSize,fout);
		wsfclose(fout);
	}

	wMod.SetPack(NULL);
	wsfclose(fPack);

	return;
}

BOOL CALLBACK CreateProc(HWND h,UINT m,WPARAM w,LPARAM l)
{
	switch (m) {

	case WM_INITDIALOG:
		{
			return TRUE;
		}
	
	case WM_COMMAND:
		{
			switch(w)
			{
			case IDQUIT:
				{
				EndDialog(h,0);
				}
				break;

			case IDMAKE:
				{
					char cTitle[30];
					char cMessage[255];

					GetDlgItemText(h,IDC_TITLE,cTitle,30);
					GetDlgItemText(h,IDC_MESSAGE,cMessage,255);
					if (BuildMake(h,cTitle,cMessage)==0)
						EndDialog(h,0);
				}
				break;
			}
			return TRUE;
		}


	break;
	}
		
	return FALSE;

}

BOOL CALLBACK UnpackProc(HWND h,UINT m,WPARAM w,LPARAM l)
{
	switch (m) {

	case WM_INITDIALOG:
		{
			char cInfo[400];
			wsul size;
			wsul i;

			size = g_WD.nPackSize;
			for (i=0;i<g_WD.nMods;i++)
				size += g_WD.nModSizes[i];

			sprintf(cInfo,
				"%d songs contained.\n"
				"Total Size: %.2fKB\n"
				"%s\n"
				"",
				g_WD.nMods,
				(float)(size/1024),
				g_WD.cMessage);

			SetWindowText(h,g_WD.cTitle);
			SetDlgItemText(h,IDC_TITLE,g_WD.cTitle);
			SetDlgItemText(h,IDC_INFO,cInfo);
			return TRUE;
		}
	
	case WM_COMMAND:
		{
			switch(w)
			{
			case IDQUIT:
				{
				EndDialog(h,0);
				}
				break;

			case IDEXTRACT:
				{
					DoExtract();
					EndDialog(h,0);
				}
				break;
			}
			return TRUE;
		}


	break;
	}
		
	return FALSE;

}

//////////////////////////////////////////////////////
// WinMain()
// 
// Description
/// Main entrance point of MiniWSF
/// 
/// 
/// \author Josh Whelchel
/// \date 4/1/2004 8:54:12 PM
/// \brief Main entrance for MiniWSF
///
//
//////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPre, LPSTR lpCmdLine, int nCmdShow )
{
	wsf_file *fd;
	char cFile[200];
	wsul nPos;
	wsul i;

	hinst = hInst;
	memset(&g_WD,0,sizeof(MiniWSFData));
	GetModuleFileName(NULL,cFile,200);
	fd = wsfopenfile(cFile);

	if (!fd){
		MessageBox(NULL,"Couldn't find internal resource!\n"
		"Invalid Program Data :P","MiniWSF",MB_OK|MB_ICONHAND);
		return 1;
	}

	wsfend(-1*(signed)sizeof(wsul),fd);
	wsfread(&nPos,sizeof(wsul),fd);

	if (nPos >= fd->nSize){
		wsfclose(fd);
		// Create MiniWSF Mode!
		return DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_CREATE),NULL,(DLGPROC)CreateProc,0);
	}

	wsfbegin(nPos,fd);
	wsfread(&nPos,sizeof(wsul),fd);

	if (nPos != 666){
		wsfclose(fd);
		// Create MiniWSF Mode!
		return DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_CREATE),NULL,(DLGPROC)CreateProc,0);
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

	// Dialog!
	DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_UNLOAD),NULL,(DLGPROC)UnpackProc,0);

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
 
/*!
	}@
*/ 

// NOT WINDOWS!
#else
 
#include <stdio.h>
int main( int argc, char **argv )
{
	printf("miniwsf is not intended for non-win32 systems!");
	return 1;
}

#endif

