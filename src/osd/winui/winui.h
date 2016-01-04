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

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <richedit.h>

// standard C headers
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <time.h>
#include <sys\stat.h>	// FIXME! use WIN32 API instead - directories.c

// MAME headers
#include "emu.h"
#include "options.h"
#include "pool.h"
#include "unzip.h"
#include "winutf8.h"
#include "strconv.h"
#include "drivenum.h"
#include "audit.h"
#include "winmain.h"
#include "png.h"
#include "window.h"
#include "sound\samples.h"

// MAMEUIFX headers
#include "resource.h"
#include "win_options.h"
#include "game_opts.h"
#include "mui_util.h"
#include "mui_audit.h"
#include "directories.h"
#include "mui_opts.h"
#include "properties.h"
#include "datafile.h"
#include "datamap.h"
#include "columnedit.h"
#include "picker.h"
#include "tabview.h"
#include "bitmask.h"
#include "treeview.h"
#include "splitters.h"
#include "history.h"
#include "dialogs.h"
#include "directinput.h"
#include "dijoystick.h"
#include "dxdecode.h"   
#include "screenshot.h"

#ifdef PTR64
#define MAMEUINAME		"MAMEUIFX64"
#else
#define MAMEUINAME		"MAMEUIFX32"
#endif
#define MAMENAME		"MAME"

#define SEARCH_PROMPT 	"<search here>"

/* for future use? though here is the best place to define them */
#define COLOR_WINXP		RGB(236, 233, 216)
#define COLOR_SILVER	RGB(224, 223, 227)
#define COLOR_ZUNE		RGB(226, 226, 226)
#define COLOR_ROYALE	RGB(235, 233, 237)
#define COLOR_WIN7		RGB(240, 240, 240)
#define COLOR_WHITE		RGB(255, 255, 255)

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
	FILETYPE_DEBUGSCRIPT_FILES = 9
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
} srcdriver_data_type;

typedef BOOL (WINAPI *common_file_dialog_proc)(LPOPENFILENAME lpofn);
BOOL CommonFileDialog(common_file_dialog_proc cfd, char *filename, int filetype);
HWND GetMainWindow(void);
HWND GetTreeView(void);
HWND GetProgressBar(void);
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
int GetParentIndex(const game_driver *driver);
int GetParentRomSetIndex(const game_driver *driver);
int GetGameNameIndex(const char *name);
int GetSrcDriverIndex(const char *name);
// sets text in part of the status bar on the main window
void SetStatusBarText(int part_index, const char *message);
void SetStatusBarTextF(int part_index, const char *fmt, ...) ATTR_PRINTF(2,3);
int MameUIMain(HINSTANCE hInstance, LPWSTR lpCmdLine);
BOOL MouseHasBeenMoved(void);
const char * GetSearchText(void);

#endif
