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
 
#ifndef MUI_OPTS_H
#define MUI_OPTS_H

#define GLOBAL_OPTIONS	-1
#define FOLDER_OPTIONS	-2

#define OPTIONS_TYPE_GLOBAL		-1
#define OPTIONS_TYPE_FOLDER		-2

#define UNKNOWN		-1

// Because we have added the Options after MAX_TAB_TYPES, we have to subtract 3 here
// (that's how many options we have after MAX_TAB_TYPES)
#define TAB_SUBTRACT 2

// Various levels of ini's we can edit.
typedef enum 
{
	OPTIONS_GLOBAL = 0,
	OPTIONS_HORIZONTAL,
	OPTIONS_VERTICAL,
	OPTIONS_RASTER,
	OPTIONS_VECTOR,
	OPTIONS_SOURCE,
	OPTIONS_PARENT,
	OPTIONS_GAME,
	OPTIONS_MAX
} OPTIONS_TYPE;

enum
{
	COLUMN_GAMES = 0,
	COLUMN_ROMNAME,
	COLUMN_SOURCEFILE,
	COLUMN_MANUFACTURER,
	COLUMN_YEAR,
	COLUMN_CLONE,
	COLUMN_PLAYED,
	COLUMN_PLAYTIME,
	COLUMN_MAX
};

typedef struct
{
	int x, y, width, height;
} AREA;

typedef struct
{
	char* screen;
	char* aspect;
	char* resolution;
	char* view;
} ScreenParams;

// List of artwork types to display in the screen shot area
enum
{
	// these must match array of strings image_tabs_long_name in options.c
	// if you add new Tabs, be sure to also add them to the ComboBox init in dialogs.c
	TAB_SCREENSHOT = 0,
	TAB_FLYER,
	TAB_CABINET,
	TAB_MARQUEE,
	TAB_TITLE,
	TAB_CONTROL_PANEL,
	TAB_PCB,
	TAB_SCORES,
	TAB_HISTORY,
	MAX_TAB_TYPES,
	TAB_ALL,
	TAB_NONE
};

windows_options & MameUIGlobal(void);

void OptionsInit(void);
void SetDirectories(windows_options &opts);
void LoadOptions(windows_options &opts, OPTIONS_TYPE opt_type, int game_num);
void SaveOptions(OPTIONS_TYPE opt_type, windows_options &opts, int game_num);
void LoadFolderFlags(void);
const char* GetFolderNameByID(UINT nID);
void SaveInterface(void);
void SaveGameDefaults(void);
void SaveGameList(void);
void ResetInterface(void);
void ResetGameDefaults(void);
void ResetAllGameOptions(void);
const char* GetImageTabLongName(int tab_index);
const char* GetImageTabShortName(int tab_index);
void SetViewMode(int val);
int  GetViewMode(void);
void SetVersionCheck(BOOL version_check);
BOOL GetVersionCheck(void);
void SetJoyGUI(BOOL use_joygui);
BOOL GetJoyGUI(void);
void SetKeyGUI(BOOL use_keygui);
BOOL GetKeyGUI(void);
void SetCycleScreenshot(int cycle_screenshot);
int GetCycleScreenshot(void);
void SetStretchScreenShotLarger(BOOL stretch);
BOOL GetStretchScreenShotLarger(void);
void SetScreenshotBorderSize(int size);
int GetScreenshotBorderSize(void);
void SetScreenshotBorderColor(COLORREF uColor);
COLORREF GetScreenshotBorderColor(void);
void SetFilterInherit(BOOL inherit);
BOOL GetFilterInherit(void);
void SetUseBrokenIcon(BOOL broken);
BOOL GetUseBrokenIcon(void);
void SetSavedFolderID(int val);
int GetSavedFolderID(void);
void SetShowScreenShot(BOOL val);
BOOL GetShowScreenShot(void);
void SetShowFolderList(BOOL val);
BOOL GetShowFolderList(void);
BOOL GetShowFolder(int folder);
void SetShowFolder(int folder,BOOL show);
void SetShowStatusBar(BOOL val);
BOOL GetShowStatusBar(void);
void SetShowToolBar(BOOL val);
BOOL GetShowToolBar(void);
void SetShowTabCtrl(BOOL val);
BOOL GetShowTabCtrl(void);
void SetCurrentTab(const char *shortname);
const char* GetCurrentTab(void);
void SetDefaultGame(const char *name);
const char* GetDefaultGame(void);
void SetWindowArea(const AREA *area);
void GetWindowArea(AREA *area);
void SetWindowState(int state);
int GetWindowState(void);
void SetColumnWidths(int widths[]);
void GetColumnWidths(int widths[]);
void SetColumnOrder(int order[]);
void GetColumnOrder(int order[]);
void SetColumnShown(int shown[]);
void GetColumnShown(int shown[]);
void SetSplitterPos(int splitterId, int pos);
int  GetSplitterPos(int splitterId);
void SetCustomColor(int iIndex, COLORREF uColor);
COLORREF GetCustomColor(int iIndex);
void GetGuiFont(LOGFONT *font);
void SetListFont(const LOGFONT *font);
void GetListFont(LOGFONT *font);
void SetHistoryFont(const LOGFONT *font);
void GetHistoryFont(LOGFONT *font);
void SetTreeFont(const LOGFONT *font);
void GetTreeFont(LOGFONT *font);
DWORD GetFolderFlags(int folder_index);
void SetListFontColor(COLORREF uColor);
COLORREF GetListFontColor(void);
void SetHistoryFontColor(COLORREF uColor);
COLORREF GetHistoryFontColor(void);
void SetTreeFontColor(COLORREF uColor);
COLORREF GetTreeFontColor(void);
void SetFolderBgColor(COLORREF uColor);
COLORREF GetFolderBgColor(void);
void SetHistoryBgColor(COLORREF uColor);
COLORREF GetHistoryBgColor(void);
void SetListBgColor(COLORREF uColor);
COLORREF GetListBgColor(void);
int GetHistoryTab(void);
void SetHistoryTab(int tab,BOOL show);
int GetShowTab(int tab);
void SetShowTab(int tab,BOOL show);
BOOL AllowedToSetShowTab(int tab,BOOL show);
void SetSortColumn(int column);
int  GetSortColumn(void);
void SetSortReverse(BOOL reverse);
BOOL GetSortReverse(void);
void SetDisplayNoRomsGames(BOOL value);
BOOL GetDisplayNoRomsGames(void);
void SetExitDialog(BOOL value);
BOOL GetExitDialog(void);
void SetEnableIndent(BOOL value);
BOOL GetEnableIndent(void);
void SetMinimizeTrayIcon(BOOL value);
BOOL GetMinimizeTrayIcon(void);
const char* GetRomDirs(void);
void SetRomDirs(const char* paths);
const char* GetSampleDirs(void);
void  SetSampleDirs(const char* paths);
const char* GetIniDir(void);
void SetIniDir(const char *path);
const char* GetCfgDir(void);
void SetCfgDir(const char* path);
const char* GetHiDir(void);
void SetHiDir(const char* path);
const char* GetGLSLDir(void);
void SetGLSLDir(const char* path);
const char* GetNvramDir(void);
void SetNvramDir(const char* path);
const char* GetInpDir(void);
void SetInpDir(const char* path);
const char* GetImgDir(void);
void SetImgDir(const char* path);
const char* GetStateDir(void);
void SetStateDir(const char* path);
const char* GetArtDir(void);
void SetArtDir(const char* path);
const char* GetFlyerDir(void);
void SetFlyerDir(const char* path);
const char* GetCabinetDir(void);
void SetCabinetDir(const char* path);
const char* GetMarqueeDir(void);
void SetMarqueeDir(const char* path);
const char* GetTitlesDir(void);
void SetTitlesDir(const char* path);
const char* GetControlPanelDir(void);
void SetControlPanelDir(const char *path);
const char* GetPcbDir(void);
void SetPcbDir(const char *path);
const char* GetMoviesDir(void);
void SetMoviesDir(const char *path);
const char* GetVideoDir(void);
void SetVideoDir(const char *path);
const char* GetAudioDir(void);
void SetAudioDir(const char *path);
const char* GetGuiDir(void);
void SetGuiDir(const char *path);
const char* GetDatsDir(void);
void SetDatsDir(const char *path);
const char* GetScoresDir(void);
void SetScoresDir(const char *path);
const char* GetHLSLDir(void);
void SetHLSLDir(const char* path);
const char* GetDiffDir(void);
void SetDiffDir(const char* path);
const char* GetIconsDir(void);
void SetIconsDir(const char* path);
const char* GetCtrlrDir(void);
void SetCtrlrDir(const char* path);
const char* GetCommentDir(void);
void SetCommentDir(const char* path);
const char* GetFolderDir(void);
void SetFolderDir(const char* path);
const char* GetFontDir(void);
void SetFontDir(const char* path);
const char* GetCrosshairDir(void);
void SetCrosshairDir(const char* path);
void ResetGameOptions(int driver_index);
int GetRomAuditResults(int driver_index);
void SetRomAuditResults(int driver_index, int audit_results);
void IncrementPlayCount(int driver_index);
int GetPlayCount(int driver_index);
void ResetPlayCount(int driver_index);
void IncrementPlayTime(int driver_index, int playtime);
int GetPlayTime(int driver_index);
void GetTextPlayTime(int driver_index, char *buf);
void ResetPlayTime(int driver_index);
const char* GetVersionString(void);
// Keyboard control of ui
input_seq* Get_ui_key_up(void);
input_seq* Get_ui_key_down(void);
input_seq* Get_ui_key_left(void);
input_seq* Get_ui_key_right(void);
input_seq* Get_ui_key_start(void);
input_seq* Get_ui_key_pgup(void);
input_seq* Get_ui_key_pgdwn(void);
input_seq* Get_ui_key_home(void);
input_seq* Get_ui_key_end(void);
input_seq* Get_ui_key_ss_change(void);
input_seq* Get_ui_key_history_up(void);
input_seq* Get_ui_key_history_down(void);
input_seq* Get_ui_key_context_filters(void);
input_seq* Get_ui_key_select_random(void);
input_seq* Get_ui_key_game_audit(void);
input_seq* Get_ui_key_game_properties(void);
input_seq* Get_ui_key_help_contents(void);
input_seq* Get_ui_key_update_gamelist(void);
input_seq* Get_ui_key_view_folders(void);
input_seq* Get_ui_key_view_fullscreen(void);
input_seq* Get_ui_key_view_pagetab(void);
input_seq* Get_ui_key_view_picture_area(void);
input_seq* Get_ui_key_view_status(void);
input_seq* Get_ui_key_view_toolbars(void);
input_seq* Get_ui_key_view_tab_cabinet(void);
input_seq* Get_ui_key_view_tab_cpanel(void);
input_seq* Get_ui_key_view_tab_scores(void);
input_seq* Get_ui_key_view_tab_flyer(void);
input_seq* Get_ui_key_view_tab_history(void);
input_seq* Get_ui_key_view_tab_marquee(void);
input_seq* Get_ui_key_view_tab_screenshot(void);
input_seq* Get_ui_key_view_tab_title(void);
input_seq* Get_ui_key_view_tab_pcb(void);
input_seq* Get_ui_key_quit(void);
/* joystick */
int GetUIJoyUp(int joycodeIndex);
void SetUIJoyUp(int joycodeIndex, int val);
int GetUIJoyDown(int joycodeIndex);
void SetUIJoyDown(int joycodeIndex, int val);
int GetUIJoyLeft(int joycodeIndex);
void SetUIJoyLeft(int joycodeIndex, int val);
int GetUIJoyRight(int joycodeIndex);
void SetUIJoyRight(int joycodeIndex, int val);
int GetUIJoyStart(int joycodeIndex);
void SetUIJoyStart(int joycodeIndex, int val);
int GetUIJoyPageUp(int joycodeIndex);
void SetUIJoyPageUp(int joycodeIndex, int val);
int GetUIJoyPageDown(int joycodeIndex);
void SetUIJoyPageDown(int joycodeIndex, int val);
int GetUIJoyHome(int joycodeIndex);
void SetUIJoyHome(int joycodeIndex, int val);
int GetUIJoyEnd(int joycodeIndex);
void SetUIJoyEnd(int joycodeIndex, int val);
int GetUIJoySSChange(int joycodeIndex);
void SetUIJoySSChange(int joycodeIndex, int val);
int GetUIJoyHistoryUp(int joycodeIndex);
void SetUIJoyHistoryUp(int joycodeIndex, int val);
int GetUIJoyHistoryDown(int joycodeIndex);
void SetUIJoyHistoryDown(int joycodeIndex, int val);
BOOL GetHideMouseOnStartup(void);
void SetHideMouseOnStartup(BOOL hide);
BOOL GetRunFullScreen(void);
void SetRunFullScreen(BOOL fullScreen);
void ColumnEncodeStringWithCount(const int *value, char *str, int count);
void ColumnDecodeStringWithCount(const char* str, int *value, int count);
int GetDriverCache(int driver_index);
void SetDriverCache(int driver_index, int val);
BOOL RequiredDriverCache(int check = 0);
void SetRequiredDriverCacheStatus(void);
BOOL GetRequiredDriverCacheStatus(void);

#endif
