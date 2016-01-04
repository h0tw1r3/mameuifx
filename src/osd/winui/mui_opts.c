/***************************************************************************

  M.A.M.E.UI  -  Multiple Arcade Machine Emulator with User Interface
  Win32 Portions Copyright (C) 1997-2003 Michael Soderstrom and Chris Kirmse,
  Copyright (C) 2003-2007 Chris Kirmse and the MAME32/MAMEUI team.

  This file is part of MAMEUI, and may only be used, modified and
  distributed under the terms of the MAME license, in "readme.txt".
  By continuing to use, modify or distribute this file you indicate
  that you have read the license and understand and accept it fully.

 ***************************************************************************/

 /***************************************************************************

  mui_opts.c

  Stores core_opts options and per-game options;

***************************************************************************/

#include "winui.h"

/***************************************************************************
    Internal function prototypes
 ***************************************************************************/

static void LoadInterfaceFile(winui_options &opts, const char *filename);
static void SaveInterfaceFile(winui_options &opts, const char *filename);
static void LoadOptionsFile(windows_options &opts, const char *filename);
static void SaveOptionsFile(windows_options &opts, const char *filename);
static void LoadOptionsAndInterface(void);
static void CusColorEncodeString(const COLORREF *value, char* str);
static void CusColorDecodeString(const char* str, COLORREF *value);
static void SplitterEncodeString(const int *value, char* str);
static void SplitterDecodeString(const char *str, int *value);
static void FontEncodeString(const LOGFONT *f, char *str);
static void FontDecodeString(const char* str, LOGFONT *f);
static void TabFlagsEncodeString(int data, char *str);
static void TabFlagsDecodeString(const char *str, int *data);
static DWORD DecodeFolderFlags(const char *buf);
static const char * EncodeFolderFlags(DWORD value);
static void ParseIniFile(windows_options &opts, const char *name);

/***************************************************************************
    Internal defines
 ***************************************************************************/

#define INTERFACE_INI_FILENAME 					"interface"
#define GAMELIST_INI_FILENAME 					"gamelist"
#define DEFAULT_INI_FILENAME 					"mame"

#define MUIOPTION_VERSION						"version"
#define MUIOPTION_EXIT_DIALOG					"confirm_exit"
#define MUIOPTION_NOROMS_GAMES					"display_no_roms_games"
#define MUIOPTION_TRAY_ICON						"minimize_tray_icon"
#define MUIOPTION_LIST_MODE						"list_mode"
#define MUIOPTION_JOYSTICK_IN_INTERFACE			"joystick_in_gui_opts"
#define MUIOPTION_KEYBOARD_IN_INTERFACE			"keyboard_in_gui_opts"
#define MUIOPTION_CYCLE_SCREENSHOT				"cycle_screenshot"
#define MUIOPTION_STRETCH_SCREENSHOT_LARGER		"stretch_screenshot_larger"
#define MUIOPTION_SCREENSHOT_BORDER_SIZE		"screenshot_bordersize"
#define MUIOPTION_SCREENSHOT_BORDER_COLOR		"screenshot_bordercolor"
#define MUIOPTION_INHERIT_FILTER				"inherit_filter"
#define MUIOPTION_DEFAULT_FOLDER_ID				"default_folder_id"
#define MUIOPTION_SHOW_IMAGE_SECTION			"show_image_section"
#define MUIOPTION_SHOW_FOLDER_SECTION			"show_folder_section"
#define MUIOPTION_HIDE_FOLDERS					"hide_folders"
#define MUIOPTION_SHOW_STATUS_BAR				"show_status_bar"
#define MUIOPTION_SHOW_TABS						"show_tabs"
#define MUIOPTION_SHOW_TOOLBAR					"show_tool_bar"
#define MUIOPTION_CURRENT_TAB					"current_tab"
#define MUIOPTION_WINDOW_X						"window_x"
#define MUIOPTION_WINDOW_Y						"window_y"
#define MUIOPTION_WINDOW_WIDTH					"window_width"
#define MUIOPTION_WINDOW_HEIGHT					"window_height"
#define MUIOPTION_WINDOW_STATE					"window_state"
#define MUIOPTION_CUSTOM_COLOR					"custom_color"
#define MUIOPTION_USE_BROKEN_ICON				"use_broken_icon"
#define MUIOPTION_ENABLE_INDENT					"enable_clones_indentation"
#define MUIOPTION_GUI_FONT						"gui_font"
#define MUIOPTION_LIST_FONT						"list_font"
#define MUIOPTION_HISTORY_FONT					"history_font"
#define MUIOPTION_TREE_FONT						"tree_font"
#define MUIOPTION_LIST_COLOR					"list_color"
#define MUIOPTION_HISTORY_COLOR					"history_color"
#define MUIOPTION_TREE_COLOR					"tree_color"
#define MUIOPTION_TREEBG_COLOR					"tree_background_color"
#define MUIOPTION_LISTBG_COLOR					"list_background_color"
#define MUIOPTION_HISTORYBG_COLOR				"history_background_color"
#define MUIOPTION_HIDE_TABS						"hide_tabs"
#define MUIOPTION_HISTORY_TAB					"history_tab"
#define MUIOPTION_COLUMN_WIDTHS					"column_widths"
#define MUIOPTION_COLUMN_ORDER					"column_order"
#define MUIOPTION_COLUMN_SHOWN					"column_shown"
#define MUIOPTION_SPLITTERS						"splitters"
#define MUIOPTION_SORT_COLUMN					"sort_column"
#define MUIOPTION_SORT_REVERSED					"sort_reversed"
#define MUIOPTION_FLYER_DIRECTORY				"flyer_directory"
#define MUIOPTION_CABINET_DIRECTORY				"cabinet_directory"
#define MUIOPTION_MARQUEE_DIRECTORY				"marquee_directory"
#define MUIOPTION_TITLE_DIRECTORY				"title_directory"
#define MUIOPTION_CPANEL_DIRECTORY				"cpanel_directory"
#define MUIOPTION_PCB_DIRECTORY					"pcb_directory"
#define MUIOPTION_SCORES_DIRECTORY				"scores_directory"
#define MUIOPTION_ICONS_DIRECTORY				"icons_directory"
#define MUIOPTION_FOLDER_DIRECTORY				"folder_directory"
#define MUIOPTION_MOVIES_DIRECTORY				"movies_directory"
#define MUIOPTION_AUDIO_DIRECTORY				"audio_directory"
#define MUIOPTION_GUI_DIRECTORY					"gui_ini_directory"
#define MUIOPTION_DATS_DIRECTORY				"datafile_directory"
#define MUIOPTION_UI_KEY_UP						"ui_key_up"
#define MUIOPTION_UI_KEY_DOWN					"ui_key_down"
#define MUIOPTION_UI_KEY_LEFT					"ui_key_left"
#define MUIOPTION_UI_KEY_RIGHT					"ui_key_right"
#define MUIOPTION_UI_KEY_START					"ui_key_start"
#define MUIOPTION_UI_KEY_PGUP					"ui_key_pgup"
#define MUIOPTION_UI_KEY_PGDWN					"ui_key_pgdwn"
#define MUIOPTION_UI_KEY_HOME					"ui_key_home"
#define MUIOPTION_UI_KEY_END					"ui_key_end"
#define MUIOPTION_UI_KEY_SS_CHANGE				"ui_key_ss_change"
#define MUIOPTION_UI_KEY_HISTORY_UP				"ui_key_history_up"
#define MUIOPTION_UI_KEY_HISTORY_DOWN			"ui_key_history_down"
#define MUIOPTION_UI_KEY_CONTEXT_FILTERS		"ui_key_context_filters"
#define MUIOPTION_UI_KEY_SELECT_RANDOM			"ui_key_select_random"
#define MUIOPTION_UI_KEY_GAME_AUDIT				"ui_key_game_audit"
#define MUIOPTION_UI_KEY_GAME_PROPERTIES		"ui_key_game_properties"
#define MUIOPTION_UI_KEY_HELP_CONTENTS			"ui_key_help_contents"
#define MUIOPTION_UI_KEY_UPDATE_GAMELIST		"ui_key_update_game_opts"
#define MUIOPTION_UI_KEY_VIEW_FOLDERS			"ui_key_view_folders"
#define MUIOPTION_UI_KEY_VIEW_FULLSCREEN		"ui_key_view_fullscreen"
#define MUIOPTION_UI_KEY_VIEW_PAGETAB			"ui_key_view_pagetab"
#define MUIOPTION_UI_KEY_VIEW_PICTURE_AREA		"ui_key_view_picture_area"
#define MUIOPTION_UI_KEY_VIEW_STATUS			"ui_key_view_status"
#define MUIOPTION_UI_KEY_VIEW_TOOLBARS			"ui_key_view_toolbars"
#define MUIOPTION_UI_KEY_VIEW_TAB_CABINET		"ui_key_view_tab_cabinet"
#define MUIOPTION_UI_KEY_VIEW_TAB_CPANEL		"ui_key_view_tab_cpanel"
#define MUIOPTION_UI_KEY_VIEW_TAB_SCORES 		"ui_key_view_tab_scores"
#define MUIOPTION_UI_KEY_VIEW_TAB_FLYER			"ui_key_view_tab_flyer"
#define MUIOPTION_UI_KEY_VIEW_TAB_HISTORY		"ui_key_view_tab_history"
#define MUIOPTION_UI_KEY_VIEW_TAB_MARQUEE		"ui_key_view_tab_marquee"
#define MUIOPTION_UI_KEY_VIEW_TAB_SCREENSHOT	"ui_key_view_tab_screenshot"
#define MUIOPTION_UI_KEY_VIEW_TAB_TITLE			"ui_key_view_tab_title"
#define MUIOPTION_UI_KEY_VIEW_TAB_PCB   		"ui_key_view_tab_pcb"
#define MUIOPTION_UI_KEY_QUIT					"ui_key_quit"
#define MUIOPTION_UI_JOY_UP						"ui_joy_up"
#define MUIOPTION_UI_JOY_DOWN					"ui_joy_down"
#define MUIOPTION_UI_JOY_LEFT					"ui_joy_left"
#define MUIOPTION_UI_JOY_RIGHT					"ui_joy_right"
#define MUIOPTION_UI_JOY_START					"ui_joy_start"
#define MUIOPTION_UI_JOY_PGUP					"ui_joy_pgup"
#define MUIOPTION_UI_JOY_PGDWN					"ui_joy_pgdwn"
#define MUIOPTION_UI_JOY_HOME					"ui_joy_home"
#define MUIOPTION_UI_JOY_END					"ui_joy_end"
#define MUIOPTION_UI_JOY_SS_CHANGE				"ui_joy_ss_change"
#define MUIOPTION_UI_JOY_HISTORY_UP				"ui_joy_history_up"
#define MUIOPTION_UI_JOY_HISTORY_DOWN			"ui_joy_history_down"
#define MUIOPTION_HIDE_MOUSE					"hide_mouse"
#define MUIOPTION_FULL_SCREEN					"full_screen"
#define MUIOPTION_DEFAULT_GAME					"default_game"

/***************************************************************************
    Internal variables
 ***************************************************************************/

static char buf[80];
static BOOL RequiredDriverCacheStatus = FALSE;
static winui_options gui_opts;			// INTERFACE.INI options
static gamelist_options game_opts;		// GAMELIST.INI options
static windows_options core_opts;		// MAME.INI default options
static windows_options save_opts;		// MAME.INI current options

// UI options in INTERFACE.INI
const options_entry winui_options::s_option_entries[] =
{
	// UI options
	{ NULL,									NULL,       OPTION_HEADER,    	"APPLICATION VERSION" },
	{ MUIOPTION_VERSION,					"",         OPTION_STRING,		NULL },

	{ NULL,									NULL,       OPTION_HEADER,		"DISPLAY STATE OPTIONS" },
	{ MUIOPTION_DEFAULT_GAME,				"puckman",  OPTION_STRING,		NULL },
	{ MUIOPTION_DEFAULT_FOLDER_ID,			"0",        OPTION_INTEGER,		NULL },
	{ MUIOPTION_SHOW_IMAGE_SECTION,			"1",        OPTION_BOOLEAN,		NULL },
	{ MUIOPTION_FULL_SCREEN,				"0",        OPTION_BOOLEAN,		NULL },
	{ MUIOPTION_CURRENT_TAB,				"0",        OPTION_STRING,		NULL },
	{ MUIOPTION_SHOW_TOOLBAR,				"1",        OPTION_BOOLEAN,		NULL },
	{ MUIOPTION_SHOW_STATUS_BAR,			"1",        OPTION_BOOLEAN,		NULL },
	{ MUIOPTION_HIDE_FOLDERS,				"",         OPTION_STRING,		NULL },
	{ MUIOPTION_SHOW_FOLDER_SECTION,		"1",        OPTION_BOOLEAN,		NULL },
	{ MUIOPTION_SHOW_TABS,					"1",        OPTION_BOOLEAN,		NULL },
	{ MUIOPTION_HIDE_TABS,					"", 		OPTION_STRING, 		NULL },
	{ MUIOPTION_HISTORY_TAB,				"0",        OPTION_INTEGER,		NULL },
	{ MUIOPTION_SORT_COLUMN,				"0",        OPTION_INTEGER, 	NULL },
	{ MUIOPTION_SORT_REVERSED,				"0",        OPTION_BOOLEAN,  	NULL },
	{ MUIOPTION_WINDOW_X,					"0",        OPTION_INTEGER,  	NULL },
	{ MUIOPTION_WINDOW_Y,					"0",        OPTION_INTEGER,  	NULL },
	{ MUIOPTION_WINDOW_WIDTH,				"1150",     OPTION_INTEGER,  	NULL },
	{ MUIOPTION_WINDOW_HEIGHT,				"639",      OPTION_INTEGER,  	NULL },
	{ MUIOPTION_WINDOW_STATE,				"1",        OPTION_INTEGER,  	NULL },
	{ MUIOPTION_LIST_COLOR,					"0,0,0",    OPTION_INTEGER,  	NULL },
	{ MUIOPTION_HISTORY_COLOR,				"0,0,0",    OPTION_INTEGER,  	NULL },
	{ MUIOPTION_TREE_COLOR,					"0,0,0",    OPTION_INTEGER,  	NULL },
	{ MUIOPTION_TREEBG_COLOR,				"255,255,255", OPTION_INTEGER, 	NULL },
	{ MUIOPTION_LISTBG_COLOR,				"255,255,255", OPTION_INTEGER, 	NULL },
	{ MUIOPTION_HISTORYBG_COLOR,			"255,255,255", OPTION_INTEGER, 	NULL },
	{ MUIOPTION_CUSTOM_COLOR,				"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0", 	OPTION_STRING, NULL },
	{ MUIOPTION_LIST_MODE,					"0",       	OPTION_INTEGER,  	NULL },
	{ MUIOPTION_SPLITTERS,					"150,498", 	OPTION_STRING,   	NULL },
	{ MUIOPTION_GUI_FONT,					"-11,0,0,0,400,0,0,0,0,3,2,1,34,Tahoma",  OPTION_STRING, NULL },
	{ MUIOPTION_LIST_FONT,					"-11,0,0,0,400,0,0,0,0,3,2,1,34,Tahoma",  OPTION_STRING, NULL },
	{ MUIOPTION_HISTORY_FONT,				"-11,0,0,0,400,0,0,0,0,3,2,1,34,Verdana", OPTION_STRING, NULL },
	{ MUIOPTION_TREE_FONT,					"-11,0,0,0,400,0,0,0,0,3,2,1,34,Tahoma",  OPTION_STRING, NULL },
	{ MUIOPTION_COLUMN_WIDTHS,				"320,80,110,140,50,160,60,70", OPTION_STRING, NULL },
	{ MUIOPTION_COLUMN_ORDER,				"0,3,4,6,7,5,1,2", OPTION_STRING, NULL },
	{ MUIOPTION_COLUMN_SHOWN,				"1,1,1,1,1,1,1,1", OPTION_STRING, NULL },

	{ NULL,									NULL,       OPTION_HEADER,    	"INTERFACE OPTIONS" },
	{ MUIOPTION_NOROMS_GAMES,       		"1",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_TRAY_ICON,        			"0",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_EXIT_DIALOG,        		"1",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_JOYSTICK_IN_INTERFACE,		"1",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_KEYBOARD_IN_INTERFACE,		"0",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_HIDE_MOUSE,					"0",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_INHERIT_FILTER,				"0",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_USE_BROKEN_ICON,			"0",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_ENABLE_INDENT,				"0",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_STRETCH_SCREENSHOT_LARGER,	"0",        OPTION_BOOLEAN,   	NULL },
	{ MUIOPTION_CYCLE_SCREENSHOT,			"0",        OPTION_INTEGER,   	NULL },
	{ MUIOPTION_SCREENSHOT_BORDER_SIZE,		"0",       	OPTION_INTEGER,   	NULL },
	{ MUIOPTION_SCREENSHOT_BORDER_COLOR,	"-1",       OPTION_INTEGER,   	NULL },

	{ NULL,									NULL,       OPTION_HEADER,    	"SEARCH PATH OPTIONS" },
	{ MUIOPTION_FLYER_DIRECTORY,			"flyers",   OPTION_STRING,    	NULL },
	{ MUIOPTION_CABINET_DIRECTORY,			"cabinets", OPTION_STRING,    	NULL },
	{ MUIOPTION_MARQUEE_DIRECTORY,			"marquees", OPTION_STRING,    	NULL },
	{ MUIOPTION_TITLE_DIRECTORY,			"titles",   OPTION_STRING,    	NULL },
	{ MUIOPTION_CPANEL_DIRECTORY,			"cpanel",   OPTION_STRING,	  	NULL },
	{ MUIOPTION_PCB_DIRECTORY,			    "pcb",      OPTION_STRING,    	NULL },
	{ MUIOPTION_SCORES_DIRECTORY,			"scores",   OPTION_STRING,    	NULL },
	{ MUIOPTION_FOLDER_DIRECTORY,			"folders",  OPTION_STRING,    	NULL },
	{ MUIOPTION_ICONS_DIRECTORY,			"icons",    OPTION_STRING,    	NULL },
	{ MUIOPTION_MOVIES_DIRECTORY,			"movies",   OPTION_STRING,    	NULL },
	{ MUIOPTION_AUDIO_DIRECTORY,			"audio",    OPTION_STRING,    	NULL },
	{ MUIOPTION_GUI_DIRECTORY,				"gui",    	OPTION_STRING,    	NULL },
	{ MUIOPTION_DATS_DIRECTORY,				"dats",    	OPTION_STRING,    	NULL },

	{ NULL,									NULL,       OPTION_HEADER,    	"NAVIGATION KEY CODES" },
	{ MUIOPTION_UI_KEY_UP,					"KEYCODE_UP",	 OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_DOWN,				"KEYCODE_DOWN",  OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_LEFT,				"KEYCODE_LEFT",  OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_RIGHT,				"KEYCODE_RIGHT", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_START,				"KEYCODE_ENTER NOT KEYCODE_LALT", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_PGUP,				"KEYCODE_PGUP",  OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_PGDWN,				"KEYCODE_PGDN",  OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_HOME,				"KEYCODE_HOME",  OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_END,					"KEYCODE_END",	 OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_SS_CHANGE,			"KEYCODE_INSERT", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_HISTORY_UP,			"KEYCODE_DEL",	 OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_HISTORY_DOWN,		"KEYCODE_LALT KEYCODE_0", 	  OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_CONTEXT_FILTERS,		"KEYCODE_LCONTROL KEYCODE_F", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_SELECT_RANDOM,		"KEYCODE_LCONTROL KEYCODE_R", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_GAME_AUDIT,			"KEYCODE_LALT KEYCODE_A",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_GAME_PROPERTIES,		"KEYCODE_LALT KEYCODE_ENTER", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_HELP_CONTENTS,		"KEYCODE_F1",                 OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_UPDATE_GAMELIST,		"KEYCODE_F5",                 OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_FOLDERS,		"KEYCODE_LALT KEYCODE_D",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_FULLSCREEN,		"KEYCODE_F11",                OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_PAGETAB,		"KEYCODE_LALT KEYCODE_B",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_PICTURE_AREA,	"KEYCODE_LALT KEYCODE_P",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_STATUS,			"KEYCODE_LALT KEYCODE_S",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TOOLBARS,		"KEYCODE_LALT KEYCODE_T",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_CABINET,	"KEYCODE_LALT KEYCODE_3",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TAB_CPANEL,		"KEYCODE_LALT KEYCODE_6",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TAB_SCORES,		"KEYCODE_LALT KEYCODE_8",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TAB_FLYER,		"KEYCODE_LALT KEYCODE_2",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TAB_HISTORY,	"KEYCODE_LALT KEYCODE_9",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TAB_MARQUEE,	"KEYCODE_LALT KEYCODE_4",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TAB_SCREENSHOT,	"KEYCODE_LALT KEYCODE_1",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TAB_TITLE,		"KEYCODE_LALT KEYCODE_5",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_VIEW_TAB_PCB,		"KEYCODE_LALT KEYCODE_7",     OPTION_STRING, NULL },
    { MUIOPTION_UI_KEY_QUIT,				"KEYCODE_LALT KEYCODE_Q",     OPTION_STRING, NULL },

	{ NULL,									NULL,       OPTION_HEADER,    	"NAVIGATION JOYSTICK CODES" },
	{ MUIOPTION_UI_JOY_UP,					"1,1,1,1",  OPTION_STRING,    	NULL },
	{ MUIOPTION_UI_JOY_DOWN,				"1,1,1,2",  OPTION_STRING,    	NULL },
	{ MUIOPTION_UI_JOY_LEFT,				"1,1,2,1",  OPTION_STRING,   	NULL },
	{ MUIOPTION_UI_JOY_RIGHT,				"1,1,2,2",  OPTION_STRING,   	NULL },
	{ MUIOPTION_UI_JOY_START,				"1,0,1,0",  OPTION_STRING,  	NULL },
	{ MUIOPTION_UI_JOY_PGUP,				"2,1,2,1",  OPTION_STRING,  	NULL },
	{ MUIOPTION_UI_JOY_PGDWN,				"2,1,2,2",  OPTION_STRING,    	NULL },
	{ MUIOPTION_UI_JOY_HOME,				"0,0,0,0",  OPTION_STRING,    	NULL },
	{ MUIOPTION_UI_JOY_END,					"0,0,0,0",  OPTION_STRING,    	NULL },
	{ MUIOPTION_UI_JOY_SS_CHANGE,			"2,0,3,0",  OPTION_STRING,    	NULL },
	{ MUIOPTION_UI_JOY_HISTORY_UP,			"2,0,4,0",  OPTION_STRING,    	NULL },
	{ MUIOPTION_UI_JOY_HISTORY_DOWN,		"2,0,1,0",  OPTION_STRING,    	NULL },
	{ NULL }
};

static const options_entry filterOptions[] =
{
	{ "_filters",                			"0",        OPTION_INTEGER,   	NULL },
	{ NULL }
};

static const char *const image_tabs_long_name[MAX_TAB_TYPES] =
{
	"Snapshot",
	"Flyer",
	"Cabinet",
	"Marquee",
	"Title",
	"Control Panel",
	"PCB",
	"Scores",
	"Info"
};

static const char *const image_tabs_short_name[MAX_TAB_TYPES] =
{
	"snapshot",
	"flyer",
	"cabinet",
	"marquee",
	"title",
	"cpanel",
	"pcb",
	"scores",
	"history"
};

/***************************************************************************
    External functions
 ***************************************************************************/
winui_options::winui_options()
{
	add_entries(s_option_entries);
}

void OptionsInit(void)
{
	// gamelist creation
	game_opts.add_entries();
	// now load the options and interface settings
	LoadOptionsAndInterface();
}

windows_options & MameUIGlobal(void)
{
	return core_opts;
}

const char * GetImageTabLongName(int tab_index)
{
	assert(tab_index >= 0);
	assert(tab_index < ARRAY_LENGTH(image_tabs_long_name));
	return image_tabs_long_name[tab_index];
}

const char * GetImageTabShortName(int tab_index)
{
	assert(tab_index >= 0);
	assert(tab_index < ARRAY_LENGTH(image_tabs_short_name));
	return image_tabs_short_name[tab_index];
}

//============================================================
//  OPTIONS WRAPPERS
//============================================================

static COLORREF options_get_color(winui_options &opts, const char *name)
{
	const char *value_str;
	unsigned int r, g, b;
	COLORREF value;

	value_str = opts.value( name);

	if (sscanf(value_str, "%u,%u,%u", &r, &g, &b) == 3)
		value = RGB(r,g,b);
	else
		value = (COLORREF) - 1;

	return value;
}

static void options_set_color(winui_options &opts, const char *name, COLORREF value)
{
	char value_str[32];

	if (value == (COLORREF) - 1)
	{
		snprintf(value_str, ARRAY_LENGTH(value_str), "%d", (int)value);
	}
	else
	{
		snprintf(value_str, ARRAY_LENGTH(value_str), "%d,%d,%d", (((int)value) >>  0) & 0xFF,
			(((int)value) >>  8) & 0xFF, (((int)value) >> 16) & 0xFF);
	}
	
	std::string error_string;
	opts.set_value(name, value_str, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

static COLORREF options_get_color_default(winui_options &opts, const char *name, int default_color)
{
	COLORREF value = options_get_color(opts, name);
	
	if (value == (COLORREF) - 1)
		value = GetSysColor(default_color);
	
	return value;
}

static void options_set_color_default(winui_options &opts, const char *name, COLORREF value, int default_color)
{
	if (value == GetSysColor(default_color))
		options_set_color(gui_opts, name, (COLORREF) - 1);
	else
		options_set_color(gui_opts, name, value);
}

static input_seq *options_get_input_seq(winui_options &opts, const char *name)
{
	return NULL;
}

//============================================================
//  OPTIONS CALLS
//============================================================

void SetViewMode(int val)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_LIST_MODE, val, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

int GetViewMode(void)
{
	return gui_opts.int_value(MUIOPTION_LIST_MODE);
}

void SetEnableIndent (BOOL value)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_ENABLE_INDENT, value, OPTION_PRIORITY_CMDLINE, error_string);
    assert(error_string.empty());
}

BOOL GetEnableIndent (void)
{
	return gui_opts.bool_value(MUIOPTION_ENABLE_INDENT);
}

void SetDisplayNoRomsGames (BOOL value)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_NOROMS_GAMES, value, OPTION_PRIORITY_CMDLINE, error_string);
    assert(error_string.empty());
}

BOOL GetDisplayNoRomsGames (void)
{
	return gui_opts.bool_value(MUIOPTION_NOROMS_GAMES);
}

void SetExitDialog (BOOL value)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_EXIT_DIALOG, value, OPTION_PRIORITY_CMDLINE, error_string);
    assert(error_string.empty());
}

BOOL GetExitDialog (void)
{
	return gui_opts.bool_value(MUIOPTION_EXIT_DIALOG);
}

void SetMinimizeTrayIcon (BOOL value)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_TRAY_ICON, value, OPTION_PRIORITY_CMDLINE, error_string);
    assert(error_string.empty());
}

BOOL GetMinimizeTrayIcon (void)
{
	return gui_opts.bool_value(MUIOPTION_TRAY_ICON);
}

void SetJoyGUI(BOOL use_joygui)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_JOYSTICK_IN_INTERFACE, use_joygui, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetJoyGUI(void)
{
	return gui_opts.bool_value( MUIOPTION_JOYSTICK_IN_INTERFACE);
}

void SetKeyGUI(BOOL use_keygui)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_KEYBOARD_IN_INTERFACE, use_keygui, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetKeyGUI(void)
{
	return gui_opts.bool_value( MUIOPTION_KEYBOARD_IN_INTERFACE);
}

void SetCycleScreenshot(int cycle_screenshot)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_CYCLE_SCREENSHOT, cycle_screenshot, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

int GetCycleScreenshot(void)
{
	return gui_opts.int_value(MUIOPTION_CYCLE_SCREENSHOT);
}

void SetStretchScreenShotLarger(BOOL stretch)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_STRETCH_SCREENSHOT_LARGER, stretch, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetStretchScreenShotLarger(void)
{
	return gui_opts.bool_value( MUIOPTION_STRETCH_SCREENSHOT_LARGER);
}

void SetScreenshotBorderSize(int size)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SCREENSHOT_BORDER_SIZE, size, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

int GetScreenshotBorderSize(void)
{
	return gui_opts.int_value(MUIOPTION_SCREENSHOT_BORDER_SIZE);
}

void SetScreenshotBorderColor(COLORREF uColor)
{
	options_set_color_default(gui_opts, MUIOPTION_SCREENSHOT_BORDER_COLOR, uColor, COLOR_3DFACE);
}

COLORREF GetScreenshotBorderColor(void)
{
	return options_get_color_default(gui_opts, MUIOPTION_SCREENSHOT_BORDER_COLOR, COLOR_3DFACE);
}

void SetFilterInherit(BOOL inherit)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_INHERIT_FILTER, inherit, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetFilterInherit(void)
{
	return gui_opts.bool_value( MUIOPTION_INHERIT_FILTER);
}

void SetUseBrokenIcon(BOOL broken)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_USE_BROKEN_ICON, broken, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetUseBrokenIcon(void)
{
	return gui_opts.bool_value(MUIOPTION_USE_BROKEN_ICON);
}

void SetSavedFolderID(int val)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_DEFAULT_FOLDER_ID, val, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

int GetSavedFolderID(void)
{
	return gui_opts.int_value(MUIOPTION_DEFAULT_FOLDER_ID);
}

void SetShowScreenShot(BOOL val)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SHOW_IMAGE_SECTION, val, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetShowScreenShot(void)
{
	return gui_opts.bool_value(MUIOPTION_SHOW_IMAGE_SECTION);
}

void SetShowFolderList(BOOL val)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SHOW_FOLDER_SECTION, val, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetShowFolderList(void)
{
	return gui_opts.bool_value(MUIOPTION_SHOW_FOLDER_SECTION);
}

static void GetsShowFolderFlags(LPBITS bits)
{
	char s[1024];
	extern const FOLDERDATA g_folderData[];
	char *token;
	int j;

	snprintf(s, ARRAY_LENGTH(s), "%s", gui_opts.value(MUIOPTION_HIDE_FOLDERS));
	SetAllBits(bits, TRUE);
	token = strtok(s,", \t");
	
	while (token != NULL)
	{
		for (j = 0; g_folderData[j].m_lpTitle != NULL; j++)
		{
			if (strcmp(g_folderData[j].short_name,token) == 0)
			{
				ClearBit(bits, g_folderData[j].m_nFolderId);
				break;
			}
		}

		token = strtok(NULL, ", \t");
	}
}

BOOL GetShowFolder(int folder)
{
	BOOL result;
	LPBITS show_folder_flags = NewBits(MAX_FOLDERS);
	GetsShowFolderFlags(show_folder_flags);
	result = TestBit(show_folder_flags, folder);
	DeleteBits(show_folder_flags);
	return result;
}

void SetShowFolder(int folder,BOOL show)
{
	LPBITS show_folder_flags = NewBits(MAX_FOLDERS);
	int i;
	int num_saved = 0;
	char str[10000];
	extern const FOLDERDATA g_folderData[];
	int j;
	
	GetsShowFolderFlags(show_folder_flags);

	if (show)
		SetBit(show_folder_flags, folder);
	else
		ClearBit(show_folder_flags, folder);

	strcpy(str, "");

	// we save the ones that are NOT displayed, so we can add new ones
	// and upgraders will see them
	for (i = 0; i < MAX_FOLDERS; i++)
	{
		if (TestBit(show_folder_flags, i) == FALSE)
		{
			if (num_saved != 0)
				strcat(str,", ");

			for (j = 0; g_folderData[j].m_lpTitle != NULL; j++)
			{
				if (g_folderData[j].m_nFolderId == i)
				{
					strcat(str,g_folderData[j].short_name);
					num_saved++;
					break;
				}
			}
		}
	}

	std::string error_string;
	gui_opts.set_value(MUIOPTION_HIDE_FOLDERS, str, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
	DeleteBits(show_folder_flags);
}

void SetShowStatusBar(BOOL val)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SHOW_STATUS_BAR, val, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetShowStatusBar(void)
{
	return gui_opts.bool_value( MUIOPTION_SHOW_STATUS_BAR);
}

void SetShowTabCtrl (BOOL val)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SHOW_TABS, val, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetShowTabCtrl (void)
{
	return gui_opts.bool_value( MUIOPTION_SHOW_TABS);
}

void SetShowToolBar(BOOL val)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SHOW_TOOLBAR, val, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetShowToolBar(void)
{
	return gui_opts.bool_value( MUIOPTION_SHOW_TOOLBAR);
}

void SetCurrentTab(const char *shortname)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_CURRENT_TAB, shortname, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char *GetCurrentTab(void)
{
	return gui_opts.value( MUIOPTION_CURRENT_TAB);
}

void SetDefaultGame(const char *name)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_DEFAULT_GAME, name, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char *GetDefaultGame(void)
{
	return gui_opts.value( MUIOPTION_DEFAULT_GAME);
}

void SetWindowArea(const AREA *area)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_WINDOW_X, area->x, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
	gui_opts.set_value(MUIOPTION_WINDOW_Y, area->y, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
	gui_opts.set_value(MUIOPTION_WINDOW_WIDTH, area->width, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
	gui_opts.set_value(MUIOPTION_WINDOW_HEIGHT, area->height, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

void GetWindowArea(AREA *area)
{
	area->x = gui_opts.int_value(MUIOPTION_WINDOW_X);
	area->y = gui_opts.int_value(MUIOPTION_WINDOW_Y);
	area->width = gui_opts.int_value(MUIOPTION_WINDOW_WIDTH);
	area->height = gui_opts.int_value(MUIOPTION_WINDOW_HEIGHT);
}

void SetWindowState(int state)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_WINDOW_STATE, state, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

int GetWindowState(void)
{
	return gui_opts.int_value(MUIOPTION_WINDOW_STATE);
}

void SetCustomColor(int iIndex, COLORREF uColor)
{
	const char *custom_color_string;
	COLORREF custom_color[256];
	char buffer[256];

	custom_color_string = gui_opts.value( MUIOPTION_CUSTOM_COLOR);
	CusColorDecodeString(custom_color_string, custom_color);
	custom_color[iIndex] = uColor;
	CusColorEncodeString(custom_color, buffer);
	std::string error_string;
	gui_opts.set_value(MUIOPTION_CUSTOM_COLOR, buffer, OPTION_PRIORITY_CMDLINE,error_string);
	assert(error_string.empty());
}

COLORREF GetCustomColor(int iIndex)
{
	const char *custom_color_string;
	COLORREF custom_color[256];

	custom_color_string = gui_opts.value( MUIOPTION_CUSTOM_COLOR);
	CusColorDecodeString(custom_color_string, custom_color);

	if (custom_color[iIndex] == (COLORREF)-1)
		return (COLORREF)RGB(0,0,0);

	return custom_color[iIndex];
}

void SetListFont(const LOGFONT *font)
{
	char font_string[256];
	FontEncodeString(font, font_string);
	std::string error_string;
	gui_opts.set_value(MUIOPTION_LIST_FONT, font_string, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

void GetGuiFont(LOGFONT *font)
{
	const char *font_string = gui_opts.value( MUIOPTION_GUI_FONT);
	FontDecodeString(font_string, font);
}

void GetListFont(LOGFONT *font)
{
	const char *font_string = gui_opts.value( MUIOPTION_LIST_FONT);
	FontDecodeString(font_string, font);
}

void SetHistoryFont(const LOGFONT *font)
{
	char font_string[256];
	FontEncodeString(font, font_string);
	std::string error_string;
	gui_opts.set_value(MUIOPTION_HISTORY_FONT, font_string, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

void GetHistoryFont(LOGFONT *font)
{
	const char *font_string = gui_opts.value( MUIOPTION_HISTORY_FONT);
	FontDecodeString(font_string, font);
}

void SetTreeFont(const LOGFONT *font)
{
	char font_string[2560];
	FontEncodeString(font, font_string);
	std::string error_string;
	gui_opts.set_value(MUIOPTION_TREE_FONT, font_string, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

void GetTreeFont(LOGFONT *font)
{
	const char *font_string = gui_opts.value( MUIOPTION_TREE_FONT);
	FontDecodeString(font_string, font);
}

void SetListFontColor(COLORREF uColor)
{
	options_set_color_default(gui_opts, MUIOPTION_LIST_COLOR, uColor, COLOR_WINDOWTEXT);
}

COLORREF GetListFontColor(void)
{
	return options_get_color_default(gui_opts, MUIOPTION_LIST_COLOR, COLOR_WINDOWTEXT);
}

void SetHistoryFontColor(COLORREF uColor)
{
	options_set_color_default(gui_opts, MUIOPTION_HISTORY_COLOR, uColor, COLOR_WINDOWTEXT);
}

COLORREF GetHistoryFontColor(void)
{
	return options_get_color_default(gui_opts, MUIOPTION_HISTORY_COLOR, COLOR_WINDOWTEXT);
}

void SetTreeFontColor(COLORREF uColor)
{
	options_set_color_default(gui_opts, MUIOPTION_TREE_COLOR, uColor, COLOR_WINDOWTEXT);
}

COLORREF GetTreeFontColor(void)
{
	return options_get_color_default(gui_opts, MUIOPTION_TREE_COLOR, COLOR_WINDOWTEXT);
}

void SetFolderBgColor(COLORREF uColor)
{
	options_set_color_default(gui_opts, MUIOPTION_TREEBG_COLOR, uColor, COLOR_WINDOW);
}

COLORREF GetFolderBgColor(void)
{
	return options_get_color_default(gui_opts, MUIOPTION_TREEBG_COLOR, COLOR_WINDOW);
}

void SetHistoryBgColor(COLORREF uColor)
{
	options_set_color_default(gui_opts, MUIOPTION_HISTORYBG_COLOR, uColor, COLOR_WINDOW);
}

COLORREF GetHistoryBgColor(void)
{
	return options_get_color_default(gui_opts, MUIOPTION_HISTORYBG_COLOR, COLOR_WINDOW);
}

void SetListBgColor(COLORREF uColor)
{
	options_set_color_default(gui_opts, MUIOPTION_LISTBG_COLOR, uColor, COLOR_WINDOW);
}

COLORREF GetListBgColor(void)
{
	return options_get_color_default(gui_opts, MUIOPTION_LISTBG_COLOR, COLOR_WINDOW);
}

int GetShowTab(int tab)
{
	const char *show_tabs_string;
	int show_tab_flags;

	show_tabs_string = gui_opts.value( MUIOPTION_HIDE_TABS);
	TabFlagsDecodeString(show_tabs_string, &show_tab_flags);

	return (show_tab_flags & (1 << tab)) != 0;
}

void SetShowTab(int tab,BOOL show)
{
	const char *show_tabs_string;
	int show_tab_flags;
	char buffer[256];

	show_tabs_string = gui_opts.value( MUIOPTION_HIDE_TABS);
	TabFlagsDecodeString(show_tabs_string, &show_tab_flags);

	if (show)
		show_tab_flags |= 1 << tab;
	else
		show_tab_flags &= ~(1 << tab);

	TabFlagsEncodeString(show_tab_flags, buffer);
	std::string error_string;
	gui_opts.set_value(MUIOPTION_HIDE_TABS, buffer, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

// don't delete the last one
BOOL AllowedToSetShowTab(int tab,BOOL show)
{
	const char *show_tabs_string;
	int show_tab_flags;

	if (show == TRUE)
		return TRUE;

	show_tabs_string = gui_opts.value( MUIOPTION_HIDE_TABS);
	TabFlagsDecodeString(show_tabs_string, &show_tab_flags);
	show_tab_flags &= ~(1 << tab);

	return show_tab_flags != 0;
}

int GetHistoryTab(void)
{
	return gui_opts.int_value(MUIOPTION_HISTORY_TAB);
}

void SetHistoryTab(int tab, BOOL show)
{
	std::string error_string;
	
	if (show)
		gui_opts.set_value(MUIOPTION_HISTORY_TAB, tab, OPTION_PRIORITY_CMDLINE, error_string);
	else
		gui_opts.set_value(MUIOPTION_HISTORY_TAB, TAB_NONE, OPTION_PRIORITY_CMDLINE, error_string);

	assert(error_string.empty());
}

void SetColumnWidths(int width[])
{
	char column_width_string[256];
	ColumnEncodeStringWithCount(width, column_width_string, COLUMN_MAX);
	std::string error_string;
	gui_opts.set_value(MUIOPTION_COLUMN_WIDTHS, column_width_string, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

void GetColumnWidths(int width[])
{
	const char *column_width_string;
	column_width_string = gui_opts.value(MUIOPTION_COLUMN_WIDTHS);
	ColumnDecodeStringWithCount(column_width_string, width, COLUMN_MAX);
}

void SetSplitterPos(int splitterId, int pos)
{
	const char *splitter_string;
	int *splitter;
	char buffer[256];

	if (splitterId < GetSplitterCount())
	{
		splitter_string = gui_opts.value(MUIOPTION_SPLITTERS);
		splitter = (int *)malloc(GetSplitterCount() * sizeof(*splitter));
		SplitterDecodeString(splitter_string, splitter);
		splitter[splitterId] = pos;
		SplitterEncodeString(splitter, buffer);
		std::string error_string;
		gui_opts.set_value(MUIOPTION_SPLITTERS, buffer, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		free(splitter);
	}
}

int GetSplitterPos(int splitterId)
{
	const char *splitter_string;
	int *splitter;
	int value;

	splitter_string = gui_opts.value(MUIOPTION_SPLITTERS);
	splitter = (int *)malloc(GetSplitterCount() * sizeof(*splitter));
	SplitterDecodeString(splitter_string, splitter);

	if (splitterId < GetSplitterCount())
	{
		value = splitter[splitterId];
		free(splitter);
		return value;
	}
	
	free(splitter);
	return -1; /* Error */
}

void SetColumnOrder(int order[])
{
	char column_order_string[256];
	ColumnEncodeStringWithCount(order, column_order_string, COLUMN_MAX);
	std::string error_string;
	gui_opts.set_value(MUIOPTION_COLUMN_ORDER, column_order_string, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

void GetColumnOrder(int order[])
{
	const char *column_order_string;
	column_order_string = gui_opts.value(MUIOPTION_COLUMN_ORDER);
	ColumnDecodeStringWithCount(column_order_string, order, COLUMN_MAX);
}

void SetColumnShown(int shown[])
{
	char column_shown_string[256];
	ColumnEncodeStringWithCount(shown, column_shown_string, COLUMN_MAX);
	std::string error_string;
	gui_opts.set_value(MUIOPTION_COLUMN_SHOWN, column_shown_string, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

void GetColumnShown(int shown[])
{
	const char *column_shown_string;
	column_shown_string = gui_opts.value(MUIOPTION_COLUMN_SHOWN);
	ColumnDecodeStringWithCount(column_shown_string, shown, COLUMN_MAX);
}

void SetSortColumn(int column)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SORT_COLUMN, column, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

int GetSortColumn(void)
{
	return gui_opts.int_value(MUIOPTION_SORT_COLUMN);
}

void SetSortReverse(BOOL reverse)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SORT_REVERSED, reverse, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetSortReverse(void)
{
	return gui_opts.bool_value(MUIOPTION_SORT_REVERSED);
}

const char* GetRomDirs(void)
{
	return core_opts.media_path();
}

void SetRomDirs(const char* paths)
{
	std::string error_string;
	core_opts.set_value(OPTION_MEDIAPATH, paths, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetSampleDirs(void)
{
	return core_opts.value(OPTION_SAMPLEPATH);
}

void SetSampleDirs(const char* paths)
{
	std::string error_string;
	core_opts.set_value(OPTION_SAMPLEPATH, paths, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetIniDir(void)
{
	const char *ini_dir;
	const char *s;

	ini_dir = core_opts.value(OPTION_INIPATH);
	
	while((s = strchr(ini_dir, ';')) != NULL)
	{
		ini_dir = s + 1;
	}

	return ini_dir;

}

void SetIniDir(const char *path)
{
	std::string error_string;
	core_opts.set_value(OPTION_INIPATH, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetCtrlrDir(void)
{
	return core_opts.value(OPTION_CTRLRPATH);
}

void SetCtrlrDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_CTRLRPATH, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetCommentDir(void)
{
	return core_opts.value(OPTION_COMMENT_DIRECTORY);
}

void SetCommentDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_COMMENT_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetCfgDir(void)
{
	return core_opts.value(OPTION_CFG_DIRECTORY);
}

void SetCfgDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_CFG_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetHiDir(void)
{
	return core_opts.value(OPTION_HISCORE_DIRECTORY);
}

void SetHiDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_HISCORE_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetGLSLDir(void)
{
	return core_opts.value(OSDOPTION_GLSLPATH);
}

void SetGLSLDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OSDOPTION_GLSLPATH, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetNvramDir(void)
{
	return core_opts.value(OPTION_NVRAM_DIRECTORY);
}

void SetNvramDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_NVRAM_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetInpDir(void)
{
	return core_opts.value(OPTION_INPUT_DIRECTORY);
}

void SetInpDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_INPUT_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetImgDir(void)
{
	return core_opts.value(OPTION_SNAPSHOT_DIRECTORY);
}

void SetImgDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_SNAPSHOT_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetStateDir(void)
{
	return core_opts.value(OPTION_STATE_DIRECTORY);
}

void SetStateDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_STATE_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetArtDir(void)
{
	return core_opts.value(OPTION_ARTPATH);
}

void SetArtDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_ARTPATH, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetFontDir(void)
{
	return core_opts.value(OPTION_FONTPATH);
}

void SetFontDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_FONTPATH, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetCrosshairDir(void)
{
	return core_opts.value(OPTION_CROSSHAIRPATH);
}

void SetCrosshairDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_CROSSHAIRPATH, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetFlyerDir(void)
{
	return gui_opts.value(MUIOPTION_FLYER_DIRECTORY);
}

void SetFlyerDir(const char* path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_FLYER_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetCabinetDir(void)
{
	return gui_opts.value(MUIOPTION_CABINET_DIRECTORY);
}

void SetCabinetDir(const char* path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_CABINET_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetMarqueeDir(void)
{
	return gui_opts.value(MUIOPTION_MARQUEE_DIRECTORY);
}

void SetMarqueeDir(const char* path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_MARQUEE_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetTitlesDir(void)
{
	return gui_opts.value(MUIOPTION_TITLE_DIRECTORY);
}

void SetTitlesDir(const char* path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_TITLE_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetControlPanelDir(void)
{
	return gui_opts.value(MUIOPTION_CPANEL_DIRECTORY);
}

void SetControlPanelDir(const char *path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_CPANEL_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetPcbDir(void)
{
	return gui_opts.value(MUIOPTION_PCB_DIRECTORY);
}

void SetPcbDir(const char *path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_PCB_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetMoviesDir(void)
{
	return gui_opts.value(MUIOPTION_MOVIES_DIRECTORY);
}

void SetMoviesDir(const char *path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_MOVIES_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetVideoDir(void)
{
	return core_opts.value(OPTION_VIDEO_DIRECTORY);
}

void SetVideoDir(const char *path)
{
	std::string error_string;
	core_opts.set_value(OPTION_VIDEO_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetAudioDir(void)
{
	return gui_opts.value(MUIOPTION_AUDIO_DIRECTORY);
}

void SetAudioDir(const char *path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_AUDIO_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetGuiDir(void)
{
	return gui_opts.value(MUIOPTION_GUI_DIRECTORY);
}

void SetGuiDir(const char *path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_GUI_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetDatsDir(void)
{
	return gui_opts.value(MUIOPTION_DATS_DIRECTORY);
}

void SetDatsDir(const char *path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_DATS_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	core_opts.set_value(OPTION_HIGHPATH, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetScoresDir(void)
{
	return gui_opts.value(MUIOPTION_SCORES_DIRECTORY);
}

void SetScoresDir(const char *path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_SCORES_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetHLSLDir(void)
{
	return core_opts.value(WINOPTION_HLSLPATH);
}

void SetHLSLDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(WINOPTION_HLSLPATH, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetDiffDir(void)
{
	return core_opts.value(OPTION_DIFF_DIRECTORY);
}

void SetDiffDir(const char* path)
{
	std::string error_string;
	core_opts.set_value(OPTION_DIFF_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetIconsDir(void)
{
	return gui_opts.value(MUIOPTION_ICONS_DIRECTORY);
}

void SetIconsDir(const char* path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_ICONS_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char* GetFolderDir(void)
{
	return gui_opts.value(MUIOPTION_FOLDER_DIRECTORY);
}

void SetFolderDir(const char* path)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_FOLDER_DIRECTORY, path, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

int GetRomAuditResults(int driver_index)
{
	return game_opts.rom(driver_index);
}

void SetRomAuditResults(int driver_index, int audit_results)
{
	game_opts.rom(driver_index, audit_results);
}

void IncrementPlayCount(int driver_index)
{
	int count;
	
	count = game_opts.play_count(driver_index);
	game_opts.play_count(driver_index, count + 1);
}

int GetPlayCount(int driver_index)
{
	return game_opts.play_count(driver_index);
}

void ResetPlayCount(int driver_index)
{
	game_opts.play_count(driver_index, 0);
}

void IncrementPlayTime(int driver_index, int playtime)
{
	int time;

	time = game_opts.play_time(driver_index);
	game_opts.play_time(driver_index, time + playtime);
}

int GetPlayTime(int driver_index)
{
	return game_opts.play_time(driver_index);
}

void GetTextPlayTime(int driver_index, char *buf)
{
	int hour, minute, second;
	int temp = GetPlayTime(driver_index);
	char tmp[200];
	
	hour = temp / 3600;
	temp = temp - 3600 * hour;
	minute = temp / 60;
	second = temp - 60 * minute;
	snprintf(tmp, ARRAY_LENGTH(tmp), "%d:%02d:%02d", hour, minute, second);
	strcpy(buf, tmp);
}

void ResetPlayTime(int driver_index)
{
	game_opts.play_time(driver_index, 0);
}

input_seq* Get_ui_key_up(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_UP);
}

input_seq* Get_ui_key_down(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_DOWN);
}

input_seq* Get_ui_key_left(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_LEFT);
}

input_seq* Get_ui_key_right(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_RIGHT);
}

input_seq* Get_ui_key_start(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_START);
}

input_seq* Get_ui_key_pgup(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_PGUP);
}

input_seq* Get_ui_key_pgdwn(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_PGDWN);
}

input_seq* Get_ui_key_home(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_HOME);
}

input_seq* Get_ui_key_end(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_END);
}

input_seq* Get_ui_key_ss_change(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_SS_CHANGE);
}

input_seq* Get_ui_key_history_up(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_HISTORY_UP);
}

input_seq* Get_ui_key_history_down(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_HISTORY_DOWN);
}

input_seq* Get_ui_key_context_filters(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_CONTEXT_FILTERS);
}

input_seq* Get_ui_key_select_random(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_SELECT_RANDOM);
}

input_seq* Get_ui_key_game_audit(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_GAME_AUDIT);
}

input_seq* Get_ui_key_game_properties(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_GAME_PROPERTIES);
}

input_seq* Get_ui_key_help_contents(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_HELP_CONTENTS);
}

input_seq* Get_ui_key_update_gamelist(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_UPDATE_GAMELIST);
}

input_seq* Get_ui_key_view_folders(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_FOLDERS);
}

input_seq* Get_ui_key_view_fullscreen(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_FULLSCREEN);
}

input_seq* Get_ui_key_view_pagetab(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_PAGETAB);
}

input_seq* Get_ui_key_view_picture_area(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_PICTURE_AREA);
}

input_seq* Get_ui_key_view_status(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_STATUS);
}

input_seq* Get_ui_key_view_toolbars(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TOOLBARS);
}

input_seq* Get_ui_key_view_tab_cabinet(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_CABINET);
}

input_seq* Get_ui_key_view_tab_cpanel(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_CPANEL);
}

input_seq* Get_ui_key_view_tab_scores(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_SCORES);
}

input_seq* Get_ui_key_view_tab_flyer(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_FLYER);
}

input_seq* Get_ui_key_view_tab_history(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_HISTORY);
}

input_seq* Get_ui_key_view_tab_marquee(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_MARQUEE);
}

input_seq* Get_ui_key_view_tab_screenshot(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_SCREENSHOT);
}

input_seq* Get_ui_key_view_tab_title(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_TITLE);
}

input_seq* Get_ui_key_view_tab_pcb(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_VIEW_TAB_PCB);
}

input_seq* Get_ui_key_quit(void)
{
	return options_get_input_seq(gui_opts, MUIOPTION_UI_KEY_QUIT);
}

static int GetUIJoy(const char *option_name, int joycodeIndex)
{
	const char *joycodes_string;
	int joycodes[4];

	assert(0 <= joycodeIndex && joycodeIndex < 4);
	joycodes_string = gui_opts.value( option_name);
	ColumnDecodeStringWithCount(joycodes_string, joycodes, ARRAY_LENGTH(joycodes));

	return joycodes[joycodeIndex];
}

static void SetUIJoy(const char *option_name, int joycodeIndex, int val)
{
	const char *joycodes_string;
	int joycodes[4];
	char buffer[1024];

	assert(0 <= joycodeIndex && joycodeIndex < 4);
	joycodes_string = gui_opts.value( option_name);
	ColumnDecodeStringWithCount(joycodes_string, joycodes, ARRAY_LENGTH(joycodes));
	joycodes[joycodeIndex] = val;
	ColumnEncodeStringWithCount(joycodes, buffer, ARRAY_LENGTH(joycodes));
	std::string error_string;
	gui_opts.set_value(option_name, buffer, OPTION_PRIORITY_CMDLINE,error_string);
	assert(error_string.empty());
}

int GetUIJoyUp(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_UP, joycodeIndex);
}

void SetUIJoyUp(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_UP, joycodeIndex, val);
}

int GetUIJoyDown(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_DOWN, joycodeIndex);
}

void SetUIJoyDown(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_DOWN, joycodeIndex, val);
}

int GetUIJoyLeft(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_LEFT, joycodeIndex);
}

void SetUIJoyLeft(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_LEFT, joycodeIndex, val);
}

int GetUIJoyRight(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_RIGHT, joycodeIndex);
}

void SetUIJoyRight(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_RIGHT, joycodeIndex, val);
}

int GetUIJoyStart(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_START, joycodeIndex);
}

void SetUIJoyStart(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_START, joycodeIndex, val);
}

int GetUIJoyPageUp(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_PGUP, joycodeIndex);
}

void SetUIJoyPageUp(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_PGUP, joycodeIndex, val);
}

int GetUIJoyPageDown(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_PGDWN, joycodeIndex);
}

void SetUIJoyPageDown(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_PGDWN, joycodeIndex, val);
}

int GetUIJoyHome(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_HOME, joycodeIndex);
}

void SetUIJoyHome(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_HOME, joycodeIndex, val);
}

int GetUIJoyEnd(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_END, joycodeIndex);
}

void SetUIJoyEnd(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_END, joycodeIndex, val);
}

int GetUIJoySSChange(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_SS_CHANGE, joycodeIndex);
}

void SetUIJoySSChange(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_SS_CHANGE, joycodeIndex, val);
}

int GetUIJoyHistoryUp(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_HISTORY_UP, joycodeIndex);
}

void SetUIJoyHistoryUp(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_HISTORY_UP, joycodeIndex, val);
}

int GetUIJoyHistoryDown(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_HISTORY_DOWN, joycodeIndex);
}

void SetUIJoyHistoryDown(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_HISTORY_DOWN, joycodeIndex, val);
}

BOOL GetHideMouseOnStartup(void)
{
	return gui_opts.bool_value(MUIOPTION_HIDE_MOUSE);
}

void SetHideMouseOnStartup(BOOL hide)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_HIDE_MOUSE, hide, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

BOOL GetRunFullScreen(void)
{
	return gui_opts.bool_value(MUIOPTION_FULL_SCREEN);
}

void SetRunFullScreen(BOOL fullScreen)
{
	std::string error_string;
	gui_opts.set_value(MUIOPTION_FULL_SCREEN, fullScreen, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

/***************************************************************************
    Internal functions
 ***************************************************************************/

static void  CusColorEncodeString(const COLORREF *value, char* str)
{
	int i;
	char tmpStr[256];

	snprintf(tmpStr, ARRAY_LENGTH(tmpStr), "%u", (int) value[0]);
	strcpy(str, tmpStr);

	for (i = 1; i < 16; i++)
	{
		snprintf(tmpStr, ARRAY_LENGTH(tmpStr), ",%u", (unsigned) value[i]);
		strcat(str, tmpStr);
	}
}

static void CusColorDecodeString(const char* str, COLORREF *value)
{
	int i;
	char *s, *p;
	char tmpStr[256];

	strcpy(tmpStr, str);
	p = tmpStr;

	for (i = 0; p && i < 16; i++)
	{
		s = p;

		if ((p = strchr(s,',')) != NULL && *p == ',')
		{
			*p = '\0';
			p++;
		}

		value[i] = atoi(s);
	}
}

void ColumnEncodeStringWithCount(const int *value, char *str, int count)
{
	int i;
	char buffer[256];

	snprintf(buffer, ARRAY_LENGTH(buffer),"%d", value[0]);
	strcpy(str,buffer);

    for (i = 1; i < count; i++)
	{
		snprintf(buffer, ARRAY_LENGTH(buffer),",%d", value[i]);
		strcat(str, buffer);
	}
}

void ColumnDecodeStringWithCount(const char* str, int *value, int count)
{
	int i;
	char *s, *p;
	char tmpStr[256];

	if (str == NULL)
		return;

	strcpy(tmpStr, str);
	p = tmpStr;

    for (i = 0; p && i < count; i++)
	{
		s = p;

		if ((p = strchr(s,',')) != NULL && *p == ',')
		{
			*p = '\0';
			p++;
		}

		value[i] = atoi(s);
    }
}

static void SplitterEncodeString(const int *value, char* str)
{
	int i;
	char tmpStr[256];

	snprintf(tmpStr, ARRAY_LENGTH(tmpStr), "%d", value[0]);
	strcpy(str, tmpStr);

	for (i = 1; i < GetSplitterCount(); i++)
	{
		snprintf(tmpStr, ARRAY_LENGTH(tmpStr), ",%d", value[i]);
		strcat(str, tmpStr);
	}
}

static void SplitterDecodeString(const char *str, int *value)
{
	int i;
	char *s, *p;
	char tmpStr[256];

	strcpy(tmpStr, str);
	p = tmpStr;

	for (i = 0; p && i < GetSplitterCount(); i++)
	{
		s = p;

		if ((p = strchr(s,',')) != NULL && *p == ',')
		{
			*p = '\0';
			p++;
		}

		value[i] = atoi(s);
	}
}

/* Parse the given comma-delimited string into a LOGFONT structure */
static void FontDecodeString(const char *str, LOGFONT *f)
{
	const char *ptr;
	TCHAR *t_ptr;

	sscanf(str, "%li,%li,%li,%li,%li,%i,%i,%i,%i,%i,%i,%i,%i",
		   &f->lfHeight,
		   &f->lfWidth,
		   &f->lfEscapement,
		   &f->lfOrientation,
		   &f->lfWeight,
		   (int*)&f->lfItalic,
		   (int*)&f->lfUnderline,
		   (int*)&f->lfStrikeOut,
		   (int*)&f->lfCharSet,
		   (int*)&f->lfOutPrecision,
		   (int*)&f->lfClipPrecision,
		   (int*)&f->lfQuality,
		   (int*)&f->lfPitchAndFamily);
	ptr = strrchr(str, ',');
	
	if (ptr != NULL) 
	{
		t_ptr = tstring_from_utf8(ptr + 1);
		
		if(!t_ptr)
			return;
		
		_tcscpy(f->lfFaceName, t_ptr);
		free(t_ptr);
	}
}

/* Encode the given LOGFONT structure into a comma-delimited string */
static void FontEncodeString(const LOGFONT *f, char *str)
{
	char* utf8_FaceName = utf8_from_tstring(f->lfFaceName);
	char tmp[200];

	if(!utf8_FaceName)
		return;

	snprintf(tmp, ARRAY_LENGTH(tmp), "%li,%li,%li,%li,%li,%i,%i,%i,%i,%i,%i,%i,%i,%s",
			f->lfHeight,
			f->lfWidth,
			f->lfEscapement,
			f->lfOrientation,
			f->lfWeight,
			f->lfItalic,
			f->lfUnderline,
			f->lfStrikeOut,
			f->lfCharSet,
			f->lfOutPrecision,
			f->lfClipPrecision,
			f->lfQuality,
			f->lfPitchAndFamily,
			utf8_FaceName);
	strcpy(str, tmp);
	free(utf8_FaceName);
}

static void TabFlagsEncodeString(int data, char *str)
{
	int i;
	int num_saved = 0;

	strcpy(str,"");

	// we save the ones that are NOT displayed, so we can add new ones
	// and upgraders will see them
	for (i = 0; i < MAX_TAB_TYPES; i++)
	{
		if (((data & (1 << i)) == 0) && GetImageTabShortName(i))
		{
			if (num_saved != 0)
				strcat(str, ", ");

			strcat(str,GetImageTabShortName(i));
			num_saved++;
		}
	}
}

static void TabFlagsDecodeString(const char *str, int *data)
{
	char s[1000];
	char *token;
	int j;

	snprintf(s, ARRAY_LENGTH(s), "%s", str);
	// simple way to set all tab bits "on"
	*data = (1 << MAX_TAB_TYPES) - 1;
	token = strtok(s,", \t");

	while (token != NULL)
	{
		for (j = 0; j < MAX_TAB_TYPES; j++)
		{
			if (!GetImageTabShortName(j) || (strcmp(GetImageTabShortName(j), token) == 0))
			{
				// turn off this bit
				*data &= ~(1 << j);
				break;
			}
		}

		token = strtok(NULL,", \t");
	}

	if (*data == 0)
		// not allowed to hide all tabs, because then why even show the area?
		*data = (1 << TAB_SCREENSHOT);
}

static void LoadInterfaceFile(winui_options &opts, const char *filename)
{
	file_error filerr;
	core_file *file;

	filerr = core_fopen(filename, OPEN_FLAG_READ, &file);
	
	if (filerr == FILERR_NONE)
	{
		std::string error_string;
		opts.parse_ini_file(*file, OPTION_PRIORITY_CMDLINE, OPTION_PRIORITY_CMDLINE, error_string);
		core_fclose(file);
	}
}

static void LoadOptionsFile(windows_options &opts, const char *filename)
{
	file_error filerr;
	core_file *file;

	filerr = core_fopen(filename, OPEN_FLAG_READ, &file);
	
	if (filerr == FILERR_NONE)
	{
		std::string error_string;
		opts.parse_ini_file(*file, OPTION_PRIORITY_CMDLINE, OPTION_PRIORITY_CMDLINE, error_string);
		core_fclose(file);
	}
}

static void SaveInterfaceFile(winui_options &opts, const char *filename)
{
	file_error filerr;
	core_file *file;

	filerr = core_fopen(filename, OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS, &file);
	
	if (filerr == FILERR_NONE)
	{
		std::string inistring;
		opts.output_ini(inistring);
		core_fputs(file, inistring.c_str());
		core_fclose(file);
	}
}

static void SaveOptionsFile(windows_options &opts, const char *filename)
{
	file_error filerr;
	core_file *file;

	filerr = core_fopen(filename, OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS, &file);
	
	if (filerr == FILERR_NONE)
	{
		std::string inistring;
		opts.output_ini(inistring);
		core_fputs(file, inistring.c_str());
		core_fclose(file);
	}
}

/* Register access functions below */
static void LoadOptionsAndInterface(void)
{
	// parse GAMELIST.INI
	std::string gamename = std::string(GetGuiDir()).append(PATH_SEPARATOR).append(GAMELIST_INI_FILENAME).append(".ini");
	game_opts.load_file(gamename.c_str());
	// parse INTERFACE.INI
	std::string intername = std::string(GetGuiDir()).append(PATH_SEPARATOR).append(INTERFACE_INI_FILENAME).append(".ini");
	LoadInterfaceFile(gui_opts, intername.c_str());
	// parse MAME.INI
	std::string filename = std::string(GetIniDir()).append(PATH_SEPARATOR).append(DEFAULT_INI_FILENAME).append(".ini");
	LoadOptionsFile(core_opts, filename.c_str());
}

void SetDirectories(windows_options &opts)
{
	std::string error_string;
	opts.set_value(OPTION_MEDIAPATH, GetRomDirs(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_SAMPLEPATH, GetSampleDirs(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_INIPATH, GetIniDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_CFG_DIRECTORY, GetCfgDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_HISCORE_DIRECTORY, GetHiDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_SNAPSHOT_DIRECTORY, GetImgDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_INPUT_DIRECTORY, GetInpDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_STATE_DIRECTORY, GetStateDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_ARTPATH, GetArtDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_NVRAM_DIRECTORY, GetNvramDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_CTRLRPATH, GetCtrlrDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_CROSSHAIRPATH, GetCrosshairDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(WINOPTION_HLSLPATH, GetHLSLDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_DIFF_DIRECTORY, GetDiffDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_VIDEO_DIRECTORY, GetVideoDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_HIGHPATH, GetDatsDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OSDOPTION_GLSLPATH, GetGLSLDir(), OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
}

const char * GetFolderNameByID(UINT nID)
{
	UINT i;
	extern const FOLDERDATA g_folderData[];
	extern const LPEXFOLDERDATA ExtraFolderData[];

	for (i = 0; i < MAX_EXTRA_FOLDERS * MAX_EXTRA_SUBFOLDERS; i++)
	{
		if( ExtraFolderData[i] )
		{
			if (ExtraFolderData[i]->m_nFolderId == nID)
				return ExtraFolderData[i]->m_szTitle;
		}
	}
	
	for( i = 0; i < MAX_FOLDERS; i++)
	{
		if (g_folderData[i].m_nFolderId == nID)
			return g_folderData[i].m_lpTitle;
	}
	
	return NULL;
}

DWORD GetFolderFlags(int folder_index)
{
	LPTREEFOLDER lpFolder = GetFolder(folder_index);

	if (lpFolder)
		return lpFolder->m_dwFlags & F_MASK;

	return 0;
}

/* Decode the flags into a DWORD */
static DWORD DecodeFolderFlags(const char *buf)
{
	DWORD flags = 0;
	int shift = 0;
	const char *ptr = buf;

	while (*ptr && (1 << shift) & F_MASK)
	{
		if (*ptr++ == '1')
			flags |= (1 << shift);

		shift++;
	}
	
	return flags;
}

/* Encode the flags into a string */
static const char * EncodeFolderFlags(DWORD value)
{
	int shift = 0;

	memset(&buf, 0, sizeof(buf));

	while ((1 << shift) & F_MASK) 
	{
		buf[shift] = (value & (1 << shift)) ? '1' : '0';
		shift++;
	}

	return buf;
}

/* MSH 20080813
 * Read the folder filters from INTERFACE.ini.  This must only
 * be called AFTER the folders have all been created.
 */
void LoadFolderFlags(void)
{
	winui_options opts;
	int numFolders;
	LPTREEFOLDER lpFolder;
	int i;
	options_entry entries[2] = { { 0 }, { 0 } };
	char folder_name[80];
	char *ptr;
	const char *value;

	memcpy(entries, filterOptions, sizeof(filterOptions));
	numFolders = GetNumFolders();

	for (i = 0; i < numFolders; i++)
	{
		lpFolder = GetFolder(i);

		if (lpFolder)
		{
			// Convert spaces to underscores
			strcpy(folder_name, lpFolder->m_lpTitle);
			ptr = folder_name;
			
			while (*ptr && *ptr != '\0')
			{
				if (*ptr == ' ')
					*ptr = '_';

				ptr++;
			}

			std::string option_name = std::string(folder_name).append("_filters");
			// create entry
			entries[0].name = option_name.c_str();
			opts.add_entries(entries);
		}
	}

	// These are overlayed at the end of our UI ini
	// The normal read will skip them.
	std::string filename = std::string(GetGuiDir()).append(PATH_SEPARATOR).append(INTERFACE_INI_FILENAME).append(".ini");
    LoadInterfaceFile(opts, filename.c_str());

	// retrive the stored values
	for (i = 0; i < numFolders; i++)
	{
		lpFolder = GetFolder(i);

		if (lpFolder)
		{
			// Convert spaces to underscores
			strcpy(folder_name, lpFolder->m_lpTitle);
			ptr = folder_name;
			
			while (*ptr && *ptr != '\0')
			{
				if (*ptr == ' ')
					*ptr = '_';

				ptr++;
			}
			
			std::string option_name = std::string(folder_name).append("_filters");
			// get entry and decode it
			value = opts.value(option_name.c_str());

			if (value)
			{
				lpFolder->m_dwFlags |= DecodeFolderFlags(value) & F_MASK;
			}
		}
	}
}

// Adds our folder flags to a temporary winui_options, for saving.
static void AddFolderFlags(winui_options &opts)
{
	int numFolders;
	int i;
	LPTREEFOLDER lpFolder;
	int num_entries = 0;
	options_entry entries[2] = { { 0 }, { 0 } };
	char folder_name[256];
	char *ptr;

	entries[0].name = NULL;
	entries[0].defvalue = NULL;
	entries[0].flags = OPTION_HEADER;
	entries[0].description = "FOLDER FILTERS";
	opts.add_entries(entries);

	memcpy(entries, filterOptions, sizeof(filterOptions));
	numFolders = GetNumFolders();

	for (i = 0; i < numFolders; i++)
	{
		lpFolder = GetFolder(i);
		if (lpFolder && (lpFolder->m_dwFlags & F_MASK) != 0)
		{
			// Convert spaces to underscores
			strcpy(folder_name, lpFolder->m_lpTitle);
			ptr = folder_name;
			
			while (*ptr && *ptr != '\0')
			{
				if (*ptr == ' ')
					*ptr = '_';

				ptr++;
			}

			std::string option_name = std::string(folder_name).append("_filters");
			// create entry
			entries[0].name = option_name.c_str();
			opts.add_entries(entries);
			// store entry
			std::string error_string;
			opts.set_value(option_name.c_str(), EncodeFolderFlags(lpFolder->m_dwFlags), OPTION_PRIORITY_CMDLINE, error_string);
			assert(error_string.empty());
			// increment counter
			num_entries++;
		}
	}
}

void SaveInterface(void)
{
	AddFolderFlags(gui_opts);
	std::string filename = std::string(GetGuiDir()).append(PATH_SEPARATOR).append(INTERFACE_INI_FILENAME).append(".ini");
	SaveInterfaceFile(gui_opts, filename.c_str());
}

void SaveGameDefaults(void)
{
	LoadOptions(save_opts, OPTIONS_GLOBAL, GLOBAL_OPTIONS);
	SaveOptions(OPTIONS_GLOBAL, save_opts, GLOBAL_OPTIONS);
}

void SaveGameList(void)
{
	std::string filename = std::string(GetGuiDir()).append(PATH_SEPARATOR).append(GAMELIST_INI_FILENAME).append(".ini");
	game_opts.save_file(filename.c_str());
}

void ResetInterface(void)
{
	gui_opts.revert(OPTION_PRIORITY_CMDLINE);
	SaveInterface();
}

void ResetGameDefaults(void)
{
	core_opts.revert(OPTION_PRIORITY_CMDLINE);
	SaveOptions(OPTIONS_GLOBAL, core_opts, GLOBAL_OPTIONS);
}

void ResetAllGameOptions(void)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;
	char* utf8_filename;
	int i;
	
	for (i = 0; i < driver_list::total(); i++)
	{
		std::string filename = std::string(GetIniDir()).append(PATH_SEPARATOR).append(driver_list::driver(i).name).append(".ini");
		osd_rmfile(filename.c_str());
	}

    /* Easiest to just open the ini/source folder if it exists,
	then remove all the files in it that end in ini. */
	std::string pathname = std::string(GetIniDir()).append(PATH_SEPARATOR).append("source");
	std::string match = std::string(pathname.c_str()).append(PATH_SEPARATOR).append("*.ini");
	
	if ((hFindFile = win_find_first_file_utf8(match.c_str(), &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		utf8_filename = utf8_from_tstring(FindFileData.cFileName);
		std::string match = std::string(pathname.c_str()).append(PATH_SEPARATOR).append(utf8_filename);
		free(utf8_filename);
		osd_rmfile(match.c_str());

		while (FindNextFile(hFindFile, &FindFileData) != 0)
		{
			utf8_filename = utf8_from_tstring(FindFileData.cFileName);
			std::string match = std::string(pathname.c_str()).append(PATH_SEPARATOR).append(utf8_filename);
			free(utf8_filename);
			osd_rmfile(match.c_str());
		}

		FindClose(hFindFile);
	}
}

/* ParseIniFile - parse a single INI file */
static void ParseIniFile(windows_options &opts, const char *name)
{
	/* open the file; if we fail, that's ok */
	std::string fname = std::string(GetIniDir()).append(PATH_SEPARATOR).append(name).append(".ini");
	LoadOptionsFile(opts, fname.c_str());
	SetDirectories(opts);
}

/*  get options, based on passed in option level. */
void LoadOptions(windows_options &opts, OPTIONS_TYPE opt_type, int game_num)
{
	const game_driver *driver = NULL;
	std::string basename;

	// Copy over the defaults
	ParseIniFile(opts, DEFAULT_INI_FILENAME);

	if (opt_type == OPTIONS_GLOBAL)
		return;

	if (game_num >= 0)
		driver = &driver_list::driver(game_num);

	// if we have a valid game driver, parse game-specific INI files
	if (driver != NULL)
	{
		const game_driver *parent = NULL;
		int cl = driver_list::clone(*driver);
		
		if (cl!= -1) 
			parent = &driver_list::driver(cl);
		
		int gp = -1;
		
		if (parent!=NULL) 
			gp = driver_list::clone(*parent);
		
		const game_driver *gparent = NULL;
		
		if (parent != NULL) 
		{
			if (gp!= -1) 
				gparent= &driver_list::driver(gp);
		}

		// parse "vector.ini" for vector games
		if (DriverIsVector(game_num))
			ParseIniFile(opts, "vector");
		
		if (opt_type == OPTIONS_VECTOR)
			return;

		// parse "horizont.ini" for horizontal games
		if (!DriverIsVertical(game_num))
			ParseIniFile(opts, "horizont");
		
		if (opt_type == OPTIONS_HORIZONTAL)
			return;

		// parse "vertical.ini" for vertical games
		if (DriverIsVertical(game_num))
			ParseIniFile(opts, "vertical");

		if (opt_type == OPTIONS_VERTICAL)
			return;

		// then parse "<sourcefile>.ini"
		core_filename_extract_base(basename, driver->source_file, TRUE);
		std::string srcname = std::string("source").append(PATH_SEPARATOR).append(basename.c_str());
		ParseIniFile(opts, srcname.c_str());

		if (opt_type == OPTIONS_SOURCE)
			return;

		// then parent the grandparent, parent, and game-specific INIs
		if (gparent != NULL)
			ParseIniFile(opts, gparent->name);

		if (parent != NULL)
			ParseIniFile(opts, parent->name);

		if (opt_type == OPTIONS_PARENT)
			return;

		ParseIniFile(opts, driver->name);

		if (opt_type == OPTIONS_GAME)
			return;
	}
}

/*
 * Save ini file based on game_num and passed in opt_type.  If opts are
 * NULL, the ini will be removed.
 *
 * game_num must be valid or the driver cannot be expanded and anything
 * with a higher priority than OPTIONS_VECTOR will not be saved.
 */
void SaveOptions(OPTIONS_TYPE opt_type, windows_options &opts, int game_num)
{
	const game_driver *driver = NULL;
	std::string filename, basename;

	if (game_num >= 0)
		driver = &driver_list::driver(game_num);

	if (opt_type == OPTIONS_GLOBAL)
		filename.assign(DEFAULT_INI_FILENAME);
	else if (opt_type == OPTIONS_VECTOR)
		filename.assign("vector");
	else if (opt_type == OPTIONS_VERTICAL)
		filename.assign("vertical");
	else if (opt_type == OPTIONS_HORIZONTAL)
		filename.assign("horizont");
	else if (driver != NULL)
	{
		if (opt_type == OPTIONS_SOURCE)
		{
			// determine the <sourcefile>
			core_filename_extract_base(basename, driver->source_file, TRUE);
			std::string srcname = std::string("source").append(PATH_SEPARATOR).append(basename.c_str());
			filename.assign(srcname.c_str());
		}
		else if (opt_type == OPTIONS_GAME)
			filename.assign(driver->name);
	}
	
	if (!filename.empty())
	{
		std::string filepath = std::string(GetIniDir()).append(PATH_SEPARATOR).append(filename.c_str()).append(".ini");
		SetDirectories(opts);
		SaveOptionsFile(opts, filepath.c_str());
	}
}

const char * GetVersionString(void)
{
	return build_version;
}

int GetDriverCache(int driver_index)
{
	return game_opts.cache(driver_index);
}

void SetDriverCache(int driver_index, int val)
{
	game_opts.cache(driver_index, val);
}

void SetRequiredDriverCacheStatus(void)
{
	static bool bFirst = true;

	if (bFirst)
	{
		RequiredDriverCacheStatus = RequiredDriverCache(1);
		bFirst = false;
	}
}
BOOL GetRequiredDriverCacheStatus(void)
{
	SetRequiredDriverCacheStatus();

	return RequiredDriverCacheStatus;
}

BOOL RequiredDriverCache(int check)
{
	BOOL ret = false;

	if (strcmp(gui_opts.value(MUIOPTION_VERSION), GetVersionString()) != 0)
		ret = true;

	if (!check)
	{
		std::string error_string;
		gui_opts.set_value(MUIOPTION_VERSION, GetVersionString(), OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
	}
	
	return ret;
}
