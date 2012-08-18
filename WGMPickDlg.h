/*
** WSF Input Plugin
** Copyright (c) 2003, Josh Whelchel
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

/// \file WGMPickDlg.h
/// WGM Selection Dialog Header

#include <windows.h>
#include "WSFGlob.h"

typedef enum
{
	WPD_NONE=0,		///< No Command

	WPD_NEXT,		///< Next Command
	WPD_PREV,		///< Previous Command

	WPD_SKIP		///< Sent WPD_SKIP + index of song
} WGMPickDlg_e;

class CWGMPickDlg;

typedef struct wgmpick_s
{
	CWGMPickDlg *wPick;	///< Pick Dialog Class

	HWND hButtons[2];	///< Prev/Next Buttons
	HWND hSlide;		///< Slider Handle

} WGMPickData;	

class CWGMPickDlg
{

public:

	CWGMPickDlg();
	~CWGMPickDlg();

	/// Opens Selection Window!
	void OpenWindow( int nNumSongs, int *nCmd, HWND hPar );

	/// Closes Window
	void CloseWindow( void );

	HWND m_hWnd;	///< Main Window Handle
	int *m_nCmd;	///< Command Output
	int m_nSongs;	///< Number of Songs

	HINSTANCE m_hInst;	///< Instance :O

};