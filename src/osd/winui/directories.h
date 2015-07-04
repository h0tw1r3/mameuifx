/***************************************************************************

  M.A.M.E.UI  -  Multiple Arcade Machine Emulator with User Interface
  Win32 Portions Copyright (C) 1997-2003 Michael Soderstrom and Chris Kirmse,
  Copyright (C) 2003-2007 Chris Kirmse and the MAME32/MAMEUI team.

  This file is part of MAMEUI, and may only be used, modified and
  distributed under the terms of the MAME license, in "readme.txt".
  By continuing to use, modify or distribute this file you indicate
  that you have read the license and understand and accept it fully.

 ***************************************************************************/

#pragma once 
 
#ifndef DIRECTORIES_H
#define DIRECTORIES_H

/* Dialog return codes */
#define DIRDLG_ROM	0x0001
#define DIRDLG_SAM	0x0002
#define DIRDLG_INI	0x0004
#define DIRDLG_CFG	0x0008
#define DIRDLG_HIS	0x0010
#define DIRDLG_IMG	0x0020
#define DIRDLG_INP	0x0040
#define DIRDLG_STA	0x0080
#define DIRDLG_ART	0x0100
#define DIRDLG_NVR	0x0200
#define DIRDLG_CTL	0x0400
#define DIRDLG_CRS	0x0800
#define DIRDLG_HLS	0x1000
#define DIRDLG_DIF	0x2000
#define DIRDLG_GLS	0x4000

#define DIRLIST_NEWENTRYTEXT "<end of list>"

typedef struct
{
	LPCSTR   lpName;
	LPCSTR   (*pfnGetTheseDirs)(void);
	void     (*pfnSetTheseDirs)(LPCSTR lpDirs);
	BOOL     bMulti;
	int      nDirDlgFlags;
} DIRECTORYINFO;

/* in layout.c */
extern const DIRECTORYINFO g_directoryInfo[];
INT_PTR CALLBACK DirectoriesDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

#endif
