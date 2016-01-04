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
bool DriverIsClone(int driver_index);
bool DriverIsBroken(int driver_index);
bool DriverIsHarddisk(int driver_index);
bool DriverHasOptionalBIOS(int driver_index);
bool DriverIsVector(int driver_index);
bool DriverIsBios(int driver_index);
bool DriverUsesRoms(int driver_index);
bool DriverUsesSamples(int driver_index);
bool DriverUsesLightGun(int driver_index);
bool DriverUsesTrackball(int driver_index);
bool DriverSupportsSaveState(int driver_index);
bool DriverIsVertical(int driver_index);
bool DriverIsMechanical(int driver_index);
bool DriverIsImperfect(int driver_index);
HICON win_extract_icon_utf8(HINSTANCE inst, const char* exefilename, UINT iconindex);
HANDLE win_find_first_file_utf8(const char* filename, WIN32_FIND_DATA *findfiledata);
void CenterWindow(HWND hWnd);
bool IsWindowsSevenOrHigher(void);

#endif
