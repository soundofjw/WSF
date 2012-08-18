/*
** WSF Input Plugin
** Copyright (c) 2003, Josh Whelchel
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

/// \file WGMPickDlg.cpp
/// WGM Selection Dialog Source

#include "WGMPickDlg.h"

#define WGMPICKCLASS "WGMPickClass32"

LRESULT CALLBACK WGMPickProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	WGMPickData *wPD;
	wPD = (WGMPickData*)GetWindowLong(hwnd,GWL_USERDATA);

	switch (msg)
	{
	case WM_COMMAND:
		if ((HWND)lp == wPD->hButtons[0])
			wPD->wPick->m_nCmd[0] = WPD_PREV;
		if ((HWND)lp == wPD->hButtons[1])
			wPD->wPick->m_nCmd[0] = WPD_NEXT;
		break;

	case WM_CREATE:

		LPCREATESTRUCT lpC;
		lpC = (LPCREATESTRUCT)lp;

		wPD = (WGMPickData*)malloc(sizeof(WGMPickData));
		SetWindowLong(hwnd,GWL_USERDATA,(long)wPD);

		wPD->wPick = (CWGMPickDlg*)lpC->lpCreateParams;
//		wPD->wPick->m_hWnd = hwnd;
		wPD->wPick->m_nCmd[0] = WPD_NONE;

		wPD->hSlide = NULL; // Bah

		wPD->hButtons[0] = CreateWindow("BUTTON","Prev",WS_CHILD|WS_VISIBLE,5,5,190,140,hwnd,
			NULL,wPD->wPick->m_hInst,0);
		wPD->hButtons[1] = CreateWindow("BUTTON","Next",WS_CHILD|WS_VISIBLE,200,5,190,140,hwnd,
			NULL,wPD->wPick->m_hInst,0);
		
		return 0;

	case WM_DESTROY:
		
		return 0;
	}

	return DefWindowProc(hwnd,msg,wp,lp);
}

CWGMPickDlg::CWGMPickDlg()
{
	m_hInst = GetModuleHandle(NULL);

	WNDCLASS wC;
	memset(&wC,0,sizeof(WNDCLASS));
	wC.hInstance = m_hInst;
	wC.hIcon = NULL;
	wC.hCursor = LoadCursor(NULL,IDC_ARROW);
	wC.style = CS_VREDRAW|CS_HREDRAW;
	wC.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wC.lpszMenuName = NULL;
	wC.lpszClassName = WGMPICKCLASS;
	wC.lpfnWndProc = WGMPickProc;

	RegisterClass(&wC);

	m_hWnd = NULL;
	m_nCmd = NULL;
}

CWGMPickDlg::~CWGMPickDlg()
{
	if (m_hWnd)
		CloseWindow();
}

void CWGMPickDlg::OpenWindow( int nNumSongs, int *nCmd, HWND hPar )
{
	m_nSongs = nNumSongs;
	m_hWnd = CreateWindow(WGMPICKCLASS,"WGM Select",WS_VISIBLE,100,100,400,150,
		hPar,NULL,m_hInst,(LPVOID)this);

	return;
}

void CWGMPickDlg::CloseWindow( void )
{
	DestroyWindow(m_hWnd);
	m_hWnd = NULL;
	m_nSongs = 0;
	if (m_nCmd)
		m_nCmd[0] = WPD_NONE;
	m_nCmd = NULL;
	return;
}
