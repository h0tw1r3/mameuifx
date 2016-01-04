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
 
#ifndef MUI_UTIL_H
#define MUI_UTIL_H

void ErrorMessageBox(const char *fmt, ...);
void dprintf(const char *fmt, ...);
void ShellExecuteCommon(HWND hWnd, const char *cName);
char * MyStrStrI(const char* pFirst, const char* pSrch);
char * ConvertToWindowsNewlines(const char *source);
const char * GetDriverFilename(int nIndex);
int DriverNumScreens(int driver_index);
BOOL DriverIsClone(int driver_index);
BOOL DriverIsBroken(int driver_index);
BOOL DriverIsHarddisk(int driver_index);
BOOL DriverHasOptionalBIOS(int driver_index);
BOOL DriverIsVector(int driver_index);
BOOL DriverIsBios(int driver_index);
BOOL DriverUsesRoms(int driver_index);
BOOL DriverUsesSamples(int driver_index);
BOOL DriverUsesLightGun(int driver_index);
BOOL DriverUsesTrackball(int driver_index);
BOOL DriverSupportsSaveState(int driver_index);
BOOL DriverIsVertical(int driver_index);
BOOL DriverIsMechanical(int driver_index);
BOOL DriverIsImperfect(int driver_index);
HICON win_extract_icon_utf8(HINSTANCE inst, const char* exefilename, UINT iconindex);
HANDLE win_find_first_file_utf8(const char* filename, LPWIN32_FIND_DATA findfiledata);
void CenterWindow(HWND hWnd);
BOOL IsWindowsSevenOrHigher(void);

#endif
