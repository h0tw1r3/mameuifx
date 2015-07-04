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
 
#ifndef PROPERTIES_H
#define PROPERTIES_H

#define PROPERTIES_PAGE 0
#define AUDIT_PAGE      1

/* Get title string to display in the top of the property page,
 * Called also in mui_audit.c
 */
char * GameInfoTitle(OPTIONS_TYPE opt_type, UINT nIndex);
/* Called in winui.c to create the property page */
void InitPropertyPage(HINSTANCE hInst, HWND hWnd, HICON hIcon, OPTIONS_TYPE opt_type, int folder_id, int game_num);
void InitPropertyPageToPage(HINSTANCE hInst, HWND hWnd, HICON hIcon, OPTIONS_TYPE opt_type, int folder_id, int game_num, int start_page);
void InitDefaultPropertyPage(HINSTANCE hInst, HWND hWnd);
/* Get Game status text string */
const char *GameInfoStatus(int driver_index);
/* Get Game status bar text string */
const char *GameInfoStatusBar(int driver_index);

/* Property sheet info for layout.c */
typedef struct
{
	BOOL bOnDefaultPage;
	BOOL (*pfnFilterProc)(int nDriver);
	DWORD dwDlgID;
	DLGPROC pfnDlgProc;
} PROPERTYSHEETINFO;

extern const PROPERTYSHEETINFO g_propSheets[];
INT_PTR CALLBACK GamePropertiesDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK GameOptionsProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
void ModifyPropertySheetForTreeSheet(HWND);

#endif
