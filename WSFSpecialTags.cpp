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

///
/// \file WSFSpecialTags.cpp
/// Contains functions to control Special Tags (like SEcho)

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <windows.h>

#include "WSFGlob.h"
#include "WSFSpecialTags.h"

char b[11][10] = 
{ 
 0,0,0,0,0,0,0,0,0,0,
 1,0,0,0,0,0,0,0,0,0,
 0,1,0,0,0,0,0,0,0,0,
 0,0,1,0,0,0,0,0,0,0,
 0,0,0,1,0,0,0,0,0,0,
 0,0,0,0,1,0,0,0,0,0,
 0,0,0,0,0,1,0,0,0,0,
 0,0,0,0,0,0,1,0,0,0,
 0,0,0,0,0,0,0,1,0,0,
 0,0,0,0,0,0,0,0,1,0,
 0,0,0,0,0,0,0,0,0,1
};

#pragma warning(disable: 4244)

void CWSFSTagHandler::RunSEcho( int *bSmpPtr, int nSamples, int nCh, int nBitRate )
{
	int x,s,e;
	int idx;
	int ctr;
	int delay;
	delay = 50;
	int *buf;
	int len;

	buf = bSmpPtr;

	if(echoctr==1)
	{
		len = nSamples*4*nCh;
/*		memcpy(echo[9],echo[8],len);
		memcpy(echo[8],echo[7],len);
		memcpy(echo[7],echo[6],len);
		memcpy(echo[6],echo[5],len);
		memcpy(echo[5],echo[4],len);*/
		memcpy(echo[4],echo[3],len);
		memcpy(echo[3],echo[2],len);
		memcpy(echo[2],echo[1],len);
		memcpy(echo[1],echo[0],len);
		echoctr=0;
	}

	len = nSamples;

	if(echoctr==0) 
		memcpy(echo[0],buf,len*4*nCh);

   idx=delay/10;
   if(delay>=98) idx=10; // scaling problem with short faders

   for(ctr=0; ctr<len*nCh; ctr++)
   {
    e=0;
	s=(int)(buf[ctr]);
    if(b[idx][0]==1)  e=(int)(echo[0][ctr]);
    if(b[idx][1]==1) e=e/2+(int)(echo[1][ctr]); 
    if(b[idx][2]==1) e=e/2+(int)(echo[2][ctr]);
    if(b[idx][3]==1) e=e/2+(int)(echo[3][ctr]);
    if(b[idx][4]==1) e=e/2+(int)(echo[4][ctr]);
/*    if(b[idx][5]==1) e=e/2+(int)(echo[5][ctr]);
    if(b[idx][6]==1) e=e/2+(int)(echo[6][ctr]);
    if(b[idx][7]==1) e=e/2+(int)(echo[7][ctr]);
    if(b[idx][8]==1) e=e/2+(int)(echo[8][ctr]);
    if(b[idx][9]==1) e=e/2+(int)(echo[9][ctr]);*/
    
	s=s*inpct;
    e=e*echopct;
	x=s+e;
    x=x*outpct;

	if (x > 0x7FFFFF)
		x=  0x7FFFFF;
	else if (x < -0x800000)
		x= -0x800000;

	buf[ctr] = (int)x;
   
   }
   echoctr++;

}


//////////////////////////////////////////////////////
// CWSFSTagHandler::CWSFSTagHandler()
// 
// Description
/// Constructor.
/// 
/// \author Josh Whelchel
/// \date 4/3/2004 11:52:33 AM
///
//
//////////////////////////////////////////////////////

CWSFSTagHandler::CWSFSTagHandler()
{
	m_interw = NULL;
	echo[0] = 0;
	Reset();
	ResetSettings();
}


//////////////////////////////////////////////////////
// CWSFSTagHandler::~CWSFSTagHandler()
// 
// Description
/// Deconstructor.
/// 
/// \author Josh Whelchel
/// \date 4/3/2004 12:35:15 PM
///
//
//////////////////////////////////////////////////////

CWSFSTagHandler::~CWSFSTagHandler()
{
	Reset();
	ResetSettings();
}


//////////////////////////////////////////////////////
// CWSFSTagHandler::ResetSettings()
// 
// Description
/// Resets only the tag settings
/// 
/// \author Josh Whelchel
/// \date 4/4/2004 12:55:35 AM
///
//
//////////////////////////////////////////////////////

void CWSFSTagHandler::ResetSettings( void )
{
	memset(&m_wSTags,0,sizeof(wsf_stags));
}

//////////////////////////////////////////////////////
// CWSFSTagHandler::Reset()
// 
// Description
/// Resets tag handler class, but not the settings!
/// 
/// \author Josh Whelchel
/// \date 4/3/2004 11:53:57 AM
///
//
//////////////////////////////////////////////////////

void CWSFSTagHandler::Reset( void )
{
	int i;

	if (m_interw)
		free(m_interw);

	if (echo[0]){
		for (i=0;i<5;i++){
			free(echo[i]);
			echo[i] = 0;
		}
	}

	m_interw = NULL;
}

//////////////////////////////////////////////////////
// CWSFSTagHandler::InitDSPEffects()
// 
// Description
/// Initilizes the DSP Effects.
/// Call GetSettings before you run this!
/// 
/// \author Josh Whelchel
/// \date 4/3/2004 11:50:54 AM
/// \param nBitRate Bitrate
/// \param nCh Channels
///
//
//////////////////////////////////////////////////////

void CWSFSTagHandler::InitDSPEffects( int nBitRate, int nCh, int nSRate )
{
	int i;

	if (nBitRate != 32)
		return;

	if (m_wSTags.dSEcho){
		if (m_interw)
			free(m_interw);
		m_interw = (int*)malloc(nSRate*2*nCh*(nBitRate/8));
	}
	
	if (m_wSTags.dSEcho){
		for (i=0;i<5;i++){
			echo[i] = (int*)malloc(20000);
			memset(echo[i],0,20000);
		}

		outpct=0.7;    
		echopct=m_wSTags.dSEcho;    
		inpct=1.0;
	}

	echoctr = 0;
	
	return;
}

//////////////////////////////////////////////////////
// CWSFSTagHandler::ProcessDSPEffects()
// 
// Description
/// Processes the DSP effects controlled by Special Tags.
/// This processer only handles 32-bit samples. (ahk!)
/// This isn't a problem when using DUMB: (dumb.sf.net)
/// 
/// \author Josh Whelchel
/// \date 4/3/2004 12:05:28 PM
/// \param *bSmpPtr Samples
/// \param nSamples Number of Samples (not buffer size)
/// \param nCh Channels
/// \param nBitRate Bitrate
/// \param nSRate SampleRate
///
//
//////////////////////////////////////////////////////

void CWSFSTagHandler::ProcessDSPEffects( int **bSmpPtr, int nSamples, int nCh, int nBitRate, int nSRate )
{
	if (nBitRate != 32 || bSmpPtr == NULL)
		return;

	if (m_interw)
	{
		int p,m,l;
		int x;
		int s;
		p = m = l = 0;

		for (p=0;p<nSamples;p++)
		{
			for (l=0;l<nCh;l++)
				m_interw[m++] = bSmpPtr[l][p];
		}

        s = nSamples*nCh*sizeof(int);

		// Now Process
		if (m_wSTags.dSEcho)
			RunSEcho(m_interw,nSamples,nCh,nBitRate);

		m=0;
		p=0;
        for (x = 0; x < nSamples; x++)
        {
			for (m=0;m<nCh;m++){
				int y;
				y = m_interw[p];
				if (y> 0x7FFFFF)
					y=0x7FFFFF;
				if (y< -0x800000)
					y=-0x800000;
				bSmpPtr[m][x] = y;
				p++;
			}
        }
	}
	else	// We're going to do nothing?
		return;

	return;
}


//////////////////////////////////////////////////////
// CWSFSTagHandler::LoadTags()
// 
// Description
/// Loads Special Tags from a wsf_file
/// 
/// \author Josh Whelchel
/// \date 4/3/2004 12:31:43 PM
/// \param *wf File Pointer
///
//
//////////////////////////////////////////////////////

int CWSFSTagHandler::LoadTags( wsf_file *wf )
{
	CWSFAscii cA;
	wsf_asciiout aO;
//	int i;

	cA.GetAllTags(wf,&aO);

//	i = cA.GetTagValue(&aO,"SEcho");
//	if (i != -1)
//		m_wSTags.dSEcho = atof(aO.cVals[i]);

//	if (m_wSTags.dSEcho >= 100 || m_wSTags.dSEcho < 0)
//		m_wSTags.dSEcho = 0.0; // What a weird value, thats not echo. It's DEATH!
//	else
//		m_wSTags.dSEcho /= 100.0;	// Make the percentage a double :P

	/// \todo Fix the Simple Echo.
	m_wSTags.dSEcho = 0.0;

	cA.FreeAsciiOut(&aO);
	return 0;
}

//////////////////////////////////////////////////////
// CWSFSTagHandler::LoadTags()
// 
// Description
/// Loads tags for a given filename.
/// 
/// \author Josh Whelchel
/// \date 4/3/2004 12:30:15 PM
/// \param *fn Filename
///
//
//////////////////////////////////////////////////////

int CWSFSTagHandler::LoadTags( char *fn )
{
	wsf_file *f;
	f = wsfopenfile(fn);

	if (!f)
		return 1;

	int n;
	n=LoadTags(f);
	wsfclose(f);
	return n;
}

//////////////////////////////////////////////////////
// *CWSFSTagHandler::GetSettings()
// 
// Description
/// Retrieves Special Tags settings in a wsf_stags structure.
/// 
/// \author Josh Whelchel
/// \date 4/3/2004 12:29:14 PM
///
//
//////////////////////////////////////////////////////

wsf_stags *CWSFSTagHandler::GetSettings( void )
{
	return &m_wSTags;
}

#if 0
	
void DoReverb(unsigned char *buf,int len)
{
short int x,s,e;
int ctr;

   for(ctr=0; ctr<len; ctr+=2)
   {
    e=0;
	s=(short int)(buf[ctr]+256*buf[ctr+1]);
    if(efxdata.delay>=5)  e=(short int)(echo1[ctr]+256*echo1[ctr+1])/2;
    if(efxdata.delay>=10) e=e/2+(short int)(echo[1][ctr]+256*echo[1][ctr+1])/2; 
    if(efxdata.delay>=20) e=e/2+(short int)(echo[2][ctr]+256*echo[2][ctr+1])/2;
    if(efxdata.delay>=30) e=e/2+(short int)(echo[3][ctr]+256*echo[3][ctr+1])/2;
    if(efxdata.delay>=40) e=e/2+(short int)(echo[4][ctr]+256*echo[4][ctr+1])/2;
    if(efxdata.delay>=50) e=e/2+(short int)(echo[5][ctr]+256*echo[5][ctr+1])/2;
    if(efxdata.delay>=60) e=e/2+(short int)(echo[6][ctr]+256*echo[6][ctr+1])/2;
    if(efxdata.delay>=70) e=e/2+(short int)(echo[7][ctr]+256*echo[7][ctr+1])/2;
    if(efxdata.delay>=80) e=e/2+(short int)(echo[8][ctr]+256*echo[8][ctr+1])/2;
    if(efxdata.delay>=90) e=e/2+(short int)(echo[9][ctr]+256*echo[9][ctr+1])/2;

    s=(int)((float)s*inpct);

    e=(int)((float)e*echopct);
    
	x=s+e;
    x=(int)((float)x*outpct);
	if(x<-32760) x=-32760;
	if(x>32760) x=32760;
	buf[ctr+1]=(unsigned char)((x & 0xff00)>>8);
	buf[ctr]=(unsigned char)(x & 0x00ff);
   }

}

#endif
