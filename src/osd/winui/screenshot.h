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
 
#ifndef SCREENSHOT_H
#define SCREENSHOT_H

typedef struct _mybitmapinfo
{
	int bmWidth;
	int bmHeight;
	int bmColors;
} MYBITMAPINFO, *LPMYBITMAPINFO;

BOOL LoadScreenShot(int nGame, int nType);
HANDLE GetScreenShotHandle(void);
int GetScreenShotWidth(void);
int GetScreenShotHeight(void);
void FreeScreenShot(void);
BOOL ScreenShotLoaded(void);
BOOL LoadDIB(const char *filename, HGLOBAL *phDIB, HPALETTE *pPal, BOOL flyer);
HBITMAP DIBToDDB(HDC hDC, HANDLE hDIB, LPMYBITMAPINFO desc);

#endif
