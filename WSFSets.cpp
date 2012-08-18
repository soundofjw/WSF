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

/// \file WSFSets.cpp
/// Controls Settings of WSF playing utilities :D

#include <stdio.h>
#include <stdarg.h>
#include "WSFSets.h"

//***************************************************************************** */
// OpenKey()
// 
// Purpose: Opens/Creates a key
// 
// Author : [Josh Whelchel], Created : [6/30/2002 2:43:57 PM]
// Parameters:	
// 
// [hP] : Parent
// [*cAdd] : Address
// 
// Returns:	HKEY
// 
//***************************************************************************** */

/// Opens Registry Key
HKEY CWSFSets::OpenKey( HKEY hP, char *cAdd )
{
	HKEY hR;
	DWORD dT;
	RegCreateKeyEx(hP,cAdd,0,"",REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hR,&dT);
	return hR;
}

//***************************************************************************** */
// LoadSettings()
// 
// Purpose: Loads Settings
// 
// Author : [Josh Whelchel], Created : [2/16/2003 6:21:42 PM]
// Parameters:	
// 
// [void] :
// 
// Returns:	none
// 
//***************************************************************************** */

/// Loads settings from registry
void CWSFSets::LoadSettings( void )
{
	DWORD dVal;
	DWORD dSize;
	DWORD dType;
	HKEY hK;
	HKEY hKB;

	memset(&m_wSets,0,sizeof(WSFSettings));
	strcpy(m_wSets.cTitle,"%title% (%artist%)");
	
	// Go to where our settings are found
	hK = OpenKey(HKEY_LOCAL_MACHINE,"Software");
	hKB = OpenKey(hK,"Syntesis Entertainment");
	RegCloseKey(hK);
	hK = OpenKey(hKB,"WSFPlay");
	RegCloseKey(hKB);

	RegQueryValueEx(hK,NULL,0,NULL,(LPBYTE)&dVal,&dSize);
	if (dVal != 666)
		SaveSettings();

	m_wSets.cTitle[0] = NULL;
		
//	RegQueryValueEx(hK,"NoFreePack",0,NULL,(LPBYTE)&dVal,&dSize);
//	m_wSets.nNoFreePack = dVal;

	RegQueryValueEx(hK,"DisableSEcho",0,NULL,(LPBYTE)&dVal,&dSize);
	m_wSets.nDisableSEcho = dVal;

	RegQueryValueEx(hK,"LoBit",0,NULL,(LPBYTE)&dVal,&dSize);
	m_wSets.nLoBit = dVal;

	RegQueryValueEx(hK,"Mono",0,NULL,(LPBYTE)&dVal,&dSize);
	m_wSets.nMono = dVal;

	RegQueryValueEx(hK,"LoadLen",0,NULL,(LPBYTE)&dVal,&dSize);
	m_wSets.nLoadLen = dVal;

	dType = REG_SZ;
	RegQueryValueEx(hK,"TitleOutput",0,&dType,(LPBYTE)m_wSets.cTitle,&dSize);
	RegQueryValueEx(hK,"TitleOutput",0,&dType,(LPBYTE)m_wSets.cTitle,&dSize);

	m_wSets.nNoFreePack = 0;

	RegCloseKey(hK);

	return;
}

//***************************************************************************** */
// SaveSettings()
// 
// Purpose: Saves Settings
// 
// Author : [Josh Whelchel], Created : [2/16/2003 6:21:51 PM]
// Parameters:	
// 
// [void] :
// 
// Returns:	none
// 
//***************************************************************************** */

/// Saves settings to registry
void CWSFSets::SaveSettings( void )
{
	DWORD dVal;
	HKEY hK;
	HKEY hKB;

	// Go to where our settings are found
	hK = OpenKey(HKEY_LOCAL_MACHINE,"Software");
	hKB = OpenKey(hK,"Syntesis Entertainment");
	RegCloseKey(hK);
	hK = OpenKey(hKB,"WSFPlay");
	RegCloseKey(hKB);
		
//	dVal = m_wSets.nNoFreePack;
//	RegSetValueEx(hK,"NoFreePack",0,REG_DWORD,(LPBYTE)&dVal,sizeof(DWORD));

	dVal = m_wSets.nDisableSEcho;
	RegSetValueEx(hK,"DisableSEcho",0,REG_DWORD,(LPBYTE)&dVal,sizeof(DWORD));

	dVal = m_wSets.nLoBit;
	RegSetValueEx(hK,"LoBit",0,REG_DWORD,(LPBYTE)&dVal,sizeof(DWORD));

	dVal = m_wSets.nMono;
	RegSetValueEx(hK,"Mono",0,REG_DWORD,(LPBYTE)&dVal,sizeof(DWORD));

	dVal = m_wSets.nLoadLen;
	RegSetValueEx(hK,"LoadLen",0,REG_DWORD,(LPBYTE)&dVal,sizeof(DWORD));

	RegSetValueEx(hK,"TitleOutput",0,REG_SZ,(LPBYTE)m_wSets.cTitle,strlen(m_wSets.cTitle)+1);

	dVal = 666;
	RegSetValueEx(hK,NULL,0,REG_DWORD,(LPBYTE)&dVal,sizeof(DWORD));

	RegCloseKey(hK);

	return;
}

//***************************************************************************** */
// CWSFSets::CWSFSets()
// 
// Purpose: Sets
// 
// Author : [Josh Whelchel], Created : [2/17/2003 11:25:38 AM]
// Parameters:	
// 
// 
// Returns:	none
// 
//***************************************************************************** */

CWSFSets::CWSFSets()
{
	memset(&m_wSets,0,sizeof(WSFSettings));
// Already FALSE. :P
//	m_wSets.nDisableSEcho = FALSE;
//	m_wSets.nMono = FALSE;
//	m_wSets.nLoBit = FALSE;
	strcpy(m_wSets.cTitle,"%title% (%artist%)");
	return;
}