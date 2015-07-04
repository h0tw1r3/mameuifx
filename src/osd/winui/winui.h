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

#ifndef WINUI_H
#define WINUI_H

#define WIN32_LEAN_AND_MEAN
#include <commctrl.h>
#include <commdlg.h>
#include "emu.h"
#include "pool.h"
#include "screenshot.h"

#ifdef PTR64
#define MAMEUINAME		"MAMEUIFX64"
#else
#define MAMEUINAME		"MAMEUIFX32"
#endif
#define MAMENAME		"MAME"

#define SEARCH_PROMPT 	"<search here>"

enum
{
	TAB_PICKER = 0,
	TAB_DISPLAY,
	TAB_MISC,
	NUM_TABS
};

enum
{
	FILETYPE_INPUT_FILES = 1,
	FILETYPE_SAVESTATE_FILES = 2,
	FILETYPE_WAVE_FILES = 3,
	FILETYPE_AVI_FILES = 4,
	FILETYPE_MNG_FILES = 5,
	FILETYPE_EFFECT_FILES = 6,
	FILETYPE_SHADER_FILES = 7,
	FILETYPE_CHEAT_FILES = 8,
	FILETYPE_JOYMAP_FILES = 9,
	FILETYPE_DEBUGSCRIPT_FILES = 10
};

typedef struct
{
	INT resource;
	const char *icon_name;
} ICONDATA;

typedef struct
{
    const char *name;
    int index;
} driver_data_type;

typedef struct
{
	const char *name;
    int index;
} srcdriver_data_type;

extern TCHAR last_directory[MAX_PATH];
typedef BOOL (WINAPI *common_file_dialog_proc)(LPOPENFILENAME lpofn);
BOOL CommonFileDialog(common_file_dialog_proc cfd,char *filename, int filetype);
HWND GetMainWindow(void);
HWND GetTreeView(void);
HIMAGELIST GetLargeImageList(void);
HIMAGELIST GetSmallImageList(void);
int GetNumOptionFolders(void);
void SetNumOptionFolders(int count);
void GetRealColumnOrder(int order[]);
HICON LoadIconFromFile(const char *iconname);
void UpdateScreenShot(void);
void ResizePickerControls(HWND hWnd);
void MamePlayGame(void);
int FindIconIndex(int nIconResource);
int FindIconIndexByName(const char *icon_name);
int GetSelectedPick(void);
object_pool *GetMameUIMemoryPool(void);
void UpdateListView(void);
int GetMinimumScreenShotWindowWidth(void);
// we maintain an array of drivers sorted by name, useful all around
int GetDriverIndex(const game_driver *driver);
int GetParentIndex(const game_driver *driver);
int GetParentRomSetIndex(const game_driver *driver);
int GetGameNameIndex(const char *name);
int GetSrcDriverIndex(const char *name);
int GetIndexFromSortedIndex(int sorted_index);
// sets text in part of the status bar on the main window
void SetStatusBarText(int part_index, const char *message);
void SetStatusBarTextF(int part_index, const char *fmt, ...) ATTR_PRINTF(2,3);
int MameUIMain(HINSTANCE hInstance, LPWSTR lpCmdLine);
BOOL MouseHasBeenMoved(void);
const char * GetSearchText(void);

#endif
