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

#include "emu.h" 		/* for input_seq definition */
#include "osdcomm.h"
#include "options.h"
#include "drivenum.h"
#include "video.h" 		/* for MAX_SCREENS Definition*/
#include "winmain.h"
#include "win_options.h"

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

// string iterator helper class
class string_iterator
{
public:
	// simple construction/destruction
	string_iterator() { copy(""); }
	string_iterator(const char *str) { copy(str); }

	// copy helpers
	void copy(const char *str)
	{
		/* reset the structure */
		m_str.clear();
		m_base = (str != NULL) ? str : "";
		m_cur = m_base;
	}

	// character searching helpers
	int next(int separator, bool duplicate = false)
	{
		const char *semi;

		/* if none left, return FALSE to indicate we are done */
		if (m_index != 0 and *m_cur == 0)
			return false;

		/* ignore duplicates of the separator */
		while (duplicate and m_index == 0 and *m_cur == separator)
			m_cur++;

		if (duplicate and *m_cur == 0)
			return false;

		/* copy up to the next separator */
		semi = strchr(m_cur, separator);

		if (semi == NULL)
			semi = m_cur + strlen(m_cur);

		m_str.assign(m_cur, semi - m_cur);
		m_cur = (*semi == 0) ? semi : semi + 1;

		/* ignore duplicates of the separator */
		while (duplicate and *m_cur and *m_cur == separator)
			m_cur++;

		/* bump the index and return true */
		m_index++;
		return true;
	}

	// C string conversion operators and helpers
	operator const char *() const { return m_str.c_str(); }
	const char *c_str() const { return m_str.c_str(); }

private:
	std::string		m_str;
	const char *	m_base;
	const char *	m_cur;
	int				m_index;
};

class winui_options : public core_options
{
public:
	// construction/destruction
	winui_options();

private:
	static const options_entry s_option_entries[];
};

class gamelist_options
{
public:
	// construction/destruction
	gamelist_options()
	{
		m_total = driver_list::total();
		m_list.reserve(m_total);
		driver_options option = { -1, -1, 0, 0};

		for (int i = 0; i < m_total; i++)
			m_list[i] = option;
	}

	int  rom(int index)                 { assert(0 <= index and index < driver_list::total()); return m_list[index].rom;        }
	void rom(int index, int val)        { assert(0 <= index and index < driver_list::total()); m_list[index].rom = val;         }

	int  cache(int index)               { assert(0 <= index and index < driver_list::total()); return m_list[index].cache;      }
	void cache(int index, int val)      { assert(0 <= index and index < driver_list::total()); m_list[index].cache = val;       }

	int  play_count(int index)          { assert(0 <= index and index < driver_list::total()); return m_list[index].play_count; }
	void play_count(int index, int val) { assert(0 <= index and index < driver_list::total()); m_list[index].play_count = val;  }

	int  play_time(int index)           { assert(0 <= index and index < driver_list::total()); return m_list[index].play_time;  }
	void play_time(int index, int val)  { assert(0 <= index and index < driver_list::total()); m_list[index].play_time = val;   }

	void add_entries()
	{
		options_entry entry[2] = { { 0 }, { 0 } };
		entry[0].defvalue    = "-1,-1";
		entry[0].flags       = OPTION_STRING;
		entry[0].description = NULL;

		for (int i = 0; i < m_total; i++)
		{
			entry[0].name = driver_list::driver(i).name;
			m_info.add_entries(entry);
		}
	}

	file_error load_file(const char *filename)
	{
		file_error filerr;
		core_file *file;

		filerr = core_fopen(filename, OPEN_FLAG_READ, &file);

		if (filerr == FILERR_NONE)
		{
			std::string error_string;
			m_info.parse_ini_file(*file, OPTION_PRIORITY_CMDLINE, OPTION_PRIORITY_CMDLINE, error_string);
			core_fclose(file);
		}

		load_settings();
		return filerr;
	}

	file_error save_file(const char *filename)
	{
		file_error filerr;
		core_file *file;
		std::string inistring;

		save_settings();
		output_ini(inistring, "GAMELIST CACHED VALUES");
		filerr = core_fopen(filename, OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS, &file);

		if (filerr == FILERR_NONE)
		{
			core_fputs(file, inistring.c_str());
			core_fclose(file);
		}

		return filerr;
	}

	void output_ini(std::string &buffer, const char *header = NULL)
	{
		std::string inibuffer;
		//inibuffer.expand(768 * 1024);

		m_info.output_ini(inibuffer);

		if (header != NULL && !inibuffer.empty())
		{
			strcatprintf(buffer, "\n#\n# %s\n#\n", header);
			buffer.append(inibuffer);
		}
	}

	void load_settings(void)
	{
		std::string value_str;

		for (int i = 0; i < m_total; i++)
		{
			value_str.assign(m_info.value(driver_list::driver(i).name));

			if (!value_str.empty())
				load_settings(value_str.c_str(), i);
		}
	}

	void load_settings(const char *str, int index)
	{
		string_iterator value_str(str);
		int value_int;

		for (int i = 0; i < 4; i++)
		{
			if (value_str.next(','))
			{
				if (value_str and (sscanf(value_str.c_str(), "%d", &value_int) == 1))
				{
					switch (i)
					{
						case 0:  m_list[index].rom        = value_int;  break;
						case 1:  m_list[index].cache      = value_int;  break;
						case 2:  m_list[index].play_count = value_int;  break;
						case 3:  m_list[index].play_time  = value_int;  break;
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	void save_settings(void)
	{
		std::string value_str;
		std::string error_string;

		for (int i = 0; i < m_total; i++)
		{
			strprintf(value_str, "%d,%d,%d,%d", m_list[i].rom, m_list[i].cache, m_list[i].play_count, m_list[i].play_time);
			m_info.set_value(driver_list::driver(i).name, value_str.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
		}
	}

private:
	win_options		m_info;
	int				m_total;

	struct driver_options
	{
		int	rom;
		int	cache;
		int	play_count;		
		int	play_time;
	};

	std::vector<driver_options>	m_list;
};

windows_options & MameUIGlobal(void);

void OptionsInit(void);
void SetDirectories(windows_options &opts);
void LoadOptions(windows_options &opts, OPTIONS_TYPE opt_type, int game_num);
void SaveOptions(OPTIONS_TYPE opt_type, windows_options &opts, int game_num);
void LoadFolderFlags(void);
const char* GetFolderNameByID(UINT nID);
void SaveInterface(void);
void SaveDirectories(void);
void SaveGameList(void);
void ResetInterface(void);
void ResetGameDefaults(void);
void ResetAllGameOptions(void);
const char * GetImageTabLongName(int tab_index);
const char * GetImageTabShortName(int tab_index);
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
void SetBroadcast(BOOL broadcast);
BOOL GetBroadcast(void);
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
const char *GetCurrentTab(void);
void SetDefaultGame(const char *name);
const char *GetDefaultGame(void);
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
void SetEnableIndent(BOOL value);
BOOL GetEnableIndent(void);
void SetMinimizeTrayIcon(BOOL value);
BOOL GetMinimizeTrayIcon(void);
const char* GetRomDirs(void);
void SetRomDirs(const char* paths);
const char* GetSampleDirs(void);
void  SetSampleDirs(const char* paths);
const char * GetIniDir(void);
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
const char * GetControlPanelDir(void);
void SetControlPanelDir(const char *path);
const char * GetPcbDir(void);
void SetPcbDir(const char *path);
const char * GetVideoDir(void);
void SetVideoDir(const char *path);
const char * GetAudioDir(void);
void SetAudioDir(const char *path);
const char * GetGuiDir(void);
void SetGuiDir(const char *path);
const char * GetDatsDir(void);
void SetDatsDir(const char *path);
const char * GetScoresDir(void);
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
const char * GetVersionString(void);
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
BOOL RequiredDriverCache(void);

#endif
