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

  winui.c

  Win32 GUI code.

  Created 8/12/97 by Christopher Kirmse (ckirmse@ricochet.net)
  Additional code November 1997 by Jeff Miller (miller@aa.net)
  More July 1998 by Mike Haaland (mhaaland@hypertech.com)
  Added Spitters/Property Sheets/Removed Tabs/Added Tree Control in
  Nov/Dec 1998 - Mike Haaland

***************************************************************************/

#include "winui.h"

#define MM_PLAY_GAME (WM_APP + 15000)

#define JOYGUI_MS 100

#define JOYGUI_TIMER 1
#define SCREENSHOT_TIMER 2

/* Max size of a sub-menu */
#define DBU_MIN_WIDTH  512
#define DBU_MIN_HEIGHT 250

#ifndef TVS_EX_DOUBLEBUFFER
#define TVS_EX_DOUBLEBUFFER 0x0004
#endif

#ifndef TVM_SETEXTENDEDSTYLE
#define TVM_SETEXTENDEDSTYLE 	(TV_FIRST + 44)
#endif

//I could not find a predefined value for this event and docs just say it has 1 for the parameter
#define TOOLBAR_EDIT_ACCELERATOR_PRESSED 1

#ifndef StatusBar_GetItemRect
#define StatusBar_GetItemRect(hWnd, iPart, lpRect) \
    SendMessage(hWnd, SB_GETRECT, iPart, (LPARAM)lpRect)
#endif

#ifndef ToolBar_CheckButton
#define ToolBar_CheckButton(hWnd, idButton, fCheck) \
    SendMessage(hWnd, TB_CHECKBUTTON, idButton, MAKELPARAM(fCheck, 0))
#endif

static int MIN_WIDTH  = DBU_MIN_WIDTH;
static int MIN_HEIGHT = DBU_MIN_HEIGHT;

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

static const ICONDATA g_iconData[] =
{
	{ IDI_WIN_NOROMS,			"noroms" },
	{ IDI_WIN_ROMS,				"roms" },
	{ IDI_WIN_UNKNOWN,			"unknown" },
	{ IDI_WIN_CLONE,			"clone" },
	{ IDI_WIN_REDX,				"warning" },
	{ IDI_WIN_IMPERFECT,		"imperfect" },
	{ 0 }
};

typedef struct _play_options play_options;
struct _play_options
{
	const char *record;			// OPTION_RECORD
	const char *playback;		// OPTION_PLAYBACK
	const char *state;			// OPTION_STATE
	const char *wavwrite;		// OPTION_WAVWRITE
	const char *mngwrite;		// OPTION_MNGWRITE
	const char *aviwrite;		// OPTION_AVIWRITE
};

/***************************************************************************
    function prototypes
 ***************************************************************************/

static BOOL Win32UI_init(void);
static void Win32UI_exit(void);
static void	SaveWindowArea(void);
static void	SaveWindowStatus(void);
static BOOL OnIdle(HWND hWnd);
static void OnSize(HWND hwnd, UINT state, int width, int height);
static void SetView(int menu_id);
static void ResetListView(void);
static void UpdateGameList(void);
static void DestroyIcons(void);
static void ReloadIcons(void);
static void PollGUIJoystick(void);
static BOOL MameCommand(HWND hwnd,int id, HWND hwndCtl, UINT codeNotify);
static void KeyboardKeyDown(int syskey, int vk_code, int special);
static void KeyboardKeyUp(int syskey, int vk_code, int special);
static void KeyboardStateClear(void);
static void UpdateStatusBar(void);
static BOOL TreeViewNotify(NMHDR *nm);
static int CLIB_DECL SrcDriverDataCompareFunc(const void *arg1, const void *arg2);
static int GamePicker_Compare(HWND hwndPicker, int index1, int index2, int sort_subitem);
static void DisableSelection(void);
static void EnableSelection(int nGame);
static HICON GetSelectedPickItemIcon(void);
static HICON GetSelectedPickItemIconSmall(void);
static void SetRandomPickItem(void);
static void	PickColor(COLORREF *cDefault);
static LPTREEFOLDER GetSelectedFolder(void);
static HICON GetSelectedFolderIcon(void);
static LRESULT CALLBACK PictureFrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK PictureWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static HBITMAP CreateBitmapTransparent(HBITMAP hSource);
static void MamePlayRecordGame(void);
static void MamePlayBackGame(void);
static void MamePlayRecordWave(void);
static void MamePlayRecordMNG(void);
static void MamePlayRecordAVI(void);
static void	MameLoadState(void);
static void MamePlayGameWithOptions(int nGame, const play_options *playopts);
static BOOL GameCheck(void);
static void FolderCheck(void);
static void ToggleScreenShot(void);
static void AdjustMetrics(void);
/* Icon routines */
static void CreateIcons(void);
static int GetIconForDriver(int nItem);
static void AddDriverIcon(int nItem, int default_icon_index);
// Context Menu handlers
static void UpdateMenu(HMENU hMenu);
static void InitMainMenu(HMENU hMainMenu);
static void InitTreeContextMenu(HMENU hTreeMenu);
static void InitBodyContextMenu(HMENU hBodyContextMenu);
static void ToggleShowFolder(int folder);
static BOOL HandleTreeContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL HandleScreenShotContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void	GamePicker_OnHeaderContextMenu(POINT pt, int nColumn);
static void	GamePicker_OnBodyContextMenu(POINT pt);
static void InitListView(void);
/* Re/initialize the ListView header columns */
static void ResetColumnDisplay(void);
static void CopyToolTipText (LPTOOLTIPTEXT lpttt);
static void ProgressBarShow(void);
static void ProgressBarHide(void);
static void ResizeProgressBar(void);
static void InitProgressBar(void);
static void InitToolbar(void);
static void InitStatusBar(void);
static void InitTabView(void);
static void InitListTree(void);
static void	InitMenuIcons(void);
static void ResetFonts(void);
static void SetMainTitle(void);
static void UpdateHistory(void);
static void RemoveCurrentGameCustomFolder(void);
static void RemoveGameCustomFolder(int driver_index);
static void BeginListViewDrag(NM_LISTVIEW *pnmv);
static void MouseMoveListViewDrag(POINTS pt);
static void ButtonUpListViewDrag(POINTS p);
static void CalculateBestScreenShotRect(HWND hWnd, RECT *pRect, BOOL restrict_height);
static void SwitchFullScreenMode(void);
static LRESULT CALLBACK MameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK StartupProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
static UINT_PTR CALLBACK HookProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
static UINT_PTR CALLBACK OFNHookProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static char* ConvertAmpersandString(const char *s);

enum
{
	FILETYPE_GAME_LIST = 1,
	FILETYPE_ROMS_LIST = 2,
};

static BOOL CommonListDialog(common_file_dialog_proc cfd, int filetype);
static void SaveGameListToFile(char *szFile, int filetype);

/***************************************************************************
    Internal structures
 ***************************************************************************/

/*
 * These next two structs represent how the icon information
 * is stored in an ICO file.
 */
typedef struct
{
	BYTE bWidth;               /* Width of the image */
	BYTE bHeight;              /* Height of the image (times 2) */
	BYTE bColorCount;          /* Number of colors in image (0 if >=8bpp) */
	BYTE bReserved;            /* Reserved */
	WORD wPlanes;              /* Color Planes */
	WORD wBitCount;            /* Bits per pixel */
	DWORD dwBytesInRes;        /* how many bytes in this resource? */
	DWORD dwImageOffset;       /* where in the file is this image */
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct
{
	UINT Width, Height, Colors; 	/* Width, Height and bpp */
	LPBYTE lpBits;                	/* ptr to DIB bits */
	DWORD dwNumBytes;            	/* how many bytes? */
	LPBITMAPINFO lpbi;              /* ptr to header */
	LPBYTE lpXOR;                 	/* ptr to XOR image bits */
	LPBYTE lpAND;                 	/* ptr to AND image bits */
} ICONIMAGE, *LPICONIMAGE;

/* Which edges of a control are anchored to the corresponding side of the parent window */
#define RA_LEFT     0x01
#define RA_RIGHT    0x02
#define RA_TOP      0x04
#define RA_BOTTOM   0x08
#define RA_ALL      0x0F

#define RA_END  	0
#define RA_ID   	1
#define RA_HWND 	2

typedef struct
{
	int type;       		/* Either RA_ID or RA_HWND, to indicate which member of u is used; or RA_END to signify last entry */
	union                   /* Can identify a child window by control id or by handle */
	{
		int id;       		/* Window control id */
		HWND hwnd;     		/* Window handle */
	} u;
	int action;     		/* What to do when control is resized */
	void *subwindow; 		/* Points to a Resize structure for this subwindow; NULL if none */
} ResizeItem;

typedef struct
{
	RECT rect;       			/* Client rect of window; must be initialized before first resize */
	const ResizeItem* items;   	/* Array of subitems to be resized */
} Resize;

static void ResizeWindow(HWND hParent, Resize *r);

#define SPLITTER_WIDTH	4
#define MIN_VIEW_WIDTH	10

/***************************************************************************
    Internal variables
 ***************************************************************************/
 
static TCHAR g_szPlayGameString[] = TEXT("&Play %s");
static char g_szGameCountString[] = "%d Games";
static HWND hMain = NULL;
static HWND	hSplash = NULL;
static HWND hWndList = NULL;
static HWND hTreeView = NULL;
static HWND hProgWnd = NULL;
static HWND hTabCtrl = NULL;
static HWND hSearchWnd = NULL;
static HWND	hProgress = NULL;
static HACCEL hAccel = NULL;
static HINSTANCE hInst = NULL;
static HANDLE hStartupThread = NULL;
static HFONT hFontGui = NULL;     		/* Font for tab view and search window */
static HFONT hFontList = NULL;     		/* Font for list view */
static HFONT hFontHist = NULL;     		/* Font for history view */
static HFONT hFontTree = NULL;     		/* Font for folders view */
/* menu icons bitmaps */
static HBITMAP hAboutMenu = NULL;
static HBITMAP hCustom = NULL;
static HBITMAP hDirectories = NULL;
static HBITMAP hExit = NULL;
static HBITMAP hFullscreen = NULL;
static HBITMAP hInterface = NULL;
static HBITMAP hHelp = NULL;
static HBITMAP hMameHome = NULL;
static HBITMAP hPlay = NULL;
static HBITMAP hPlayM1 = NULL;
static HBITMAP hOptions = NULL;
static HBITMAP hRefresh = NULL;
static HBITMAP hZip = NULL;
static HBITMAP hSaveList = NULL;
static HBITMAP hSaveRoms = NULL;
static HBITMAP hPlayback = NULL;
static HBITMAP hProperties = NULL;
static HBITMAP hAuditMenu = NULL;
static HBITMAP hVideo = NULL;
static HBITMAP hFonts = NULL;
static HBITMAP hFolders = NULL;
static HBITMAP hSort = NULL;
static HBITMAP hDriver = NULL;
static HBITMAP hFaq = NULL;
static HBITMAP hTabs = NULL;
static HBITMAP hTrouble = NULL;
static HBITMAP hCount = NULL;
static HBITMAP hRelease = NULL;
static HBITMAP hTime = NULL;
static HBITMAP hDescription	= NULL;
static HBITMAP hRom	= NULL;
static HBITMAP hSource = NULL;
static HBITMAP hManufacturer = NULL;
static HBITMAP hYear = NULL;
static HBITMAP hPlaywav	= NULL;
static HBITMAP hFont1 = NULL;
static HBITMAP hFont2 = NULL;
static HBITMAP hInfoback = NULL;
static HBITMAP hListback = NULL;
static HBITMAP hTreeback = NULL;
static HBITMAP hAscending = NULL;
static HBITMAP hFields = NULL;
static HBITMAP hRecavi = NULL;
static HBITMAP hRecinput = NULL;
static HBITMAP hRecwav = NULL;
static HBITMAP hPlaymng	= NULL;
static HBITMAP hRandom = NULL;
static HBITMAP hRecmng = NULL;
static HBITMAP hSavestate = NULL;
static HBITMAP hFilters	= NULL;
static HBITMAP hRemove = NULL;
static HBITMAP hRename = NULL;
static HBITMAP hReset = NULL;
static int optionfolder_count = 0;
/* global data--know where to send messages */
static BOOL in_emulation = FALSE;
static BOOL game_launched = FALSE;
/* idle work at startup */
static BOOL idle_work = FALSE;
/* object pool in use */
static object_pool *mameui_pool;
static int game_index = 0;
static int game_total = 0;
static int oldpercent = 0;
static BOOL bDoGameCheck = FALSE;
static BOOL bFolderCheck = FALSE;
static BOOL bChangedHook = FALSE;
/* Tree control variables */
static BOOL bEnableIndent = FALSE;
static BOOL bShowTree = FALSE;
static BOOL bShowToolBar = FALSE;
static BOOL bShowStatusBar = FALSE;
static BOOL bShowTabCtrl = FALSE;
static BOOL bProgressShown = FALSE;
static BOOL bListReady = FALSE;
/* use a joystick subsystem in the gui? */
static const struct OSDJoystick* g_pJoyGUI = NULL;
/* store current keyboard state (in bools) here */
static bool keyboard_state[4096]; 		/* __code_max #defines the number of internal key_codes */
/* search */
static char g_SearchText[256];
/* table copied from windows/inputs.c */
// table entry indices
#define MAME_KEY			0
#define DI_KEY				1
#define VIRTUAL_KEY			2
#define ASCII_KEY			3

// master keyboard translation table
static const int win_key_trans_table[][4] =
{
	// MAME key             dinput key          virtual key     ascii
	{ ITEM_ID_ESC,			DIK_ESCAPE,			VK_ESCAPE,		27 },
	{ ITEM_ID_1,			DIK_1,				'1',			'1' },
	{ ITEM_ID_2,			DIK_2,				'2',			'2' },
	{ ITEM_ID_3,			DIK_3,				'3',			'3' },
	{ ITEM_ID_4,			DIK_4,				'4',			'4' },
	{ ITEM_ID_5,			DIK_5,				'5',			'5' },
	{ ITEM_ID_6,			DIK_6,				'6',			'6' },
	{ ITEM_ID_7,			DIK_7,				'7',			'7' },
	{ ITEM_ID_8,			DIK_8,				'8',			'8' },
	{ ITEM_ID_9,			DIK_9,				'9',			'9' },
	{ ITEM_ID_0,			DIK_0,				'0',			'0' },
	{ ITEM_ID_BACKSPACE,	DIK_BACK,			VK_BACK,		8 },
	{ ITEM_ID_TAB,			DIK_TAB,			VK_TAB, 		9 },
	{ ITEM_ID_Q,			DIK_Q,				'Q',			'Q' },
	{ ITEM_ID_W,			DIK_W,				'W',			'W' },
	{ ITEM_ID_E,			DIK_E,				'E',			'E' },
	{ ITEM_ID_R,			DIK_R,				'R',			'R' },
	{ ITEM_ID_T,			DIK_T,				'T',			'T' },
	{ ITEM_ID_Y,			DIK_Y,				'Y',			'Y' },
	{ ITEM_ID_U,			DIK_U,				'U',			'U' },
	{ ITEM_ID_I,			DIK_I,				'I',			'I' },
	{ ITEM_ID_O,			DIK_O,				'O',			'O' },
	{ ITEM_ID_P,			DIK_P,				'P',			'P' },
	{ ITEM_ID_OPENBRACE,	DIK_LBRACKET,		VK_OEM_4,		'[' },
	{ ITEM_ID_CLOSEBRACE,	DIK_RBRACKET,		VK_OEM_6,		']' },
	{ ITEM_ID_ENTER,		DIK_RETURN, 		VK_RETURN,		13 },
	{ ITEM_ID_LCONTROL, 	DIK_LCONTROL,		VK_LCONTROL,	0 },
	{ ITEM_ID_A,			DIK_A,				'A',			'A' },
	{ ITEM_ID_S,			DIK_S,				'S',			'S' },
	{ ITEM_ID_D,			DIK_D,				'D',			'D' },
	{ ITEM_ID_F,			DIK_F,				'F',			'F' },
	{ ITEM_ID_G,			DIK_G,				'G',			'G' },
	{ ITEM_ID_H,			DIK_H,				'H',			'H' },
	{ ITEM_ID_J,			DIK_J,				'J',			'J' },
	{ ITEM_ID_K,			DIK_K,				'K',			'K' },
	{ ITEM_ID_L,			DIK_L,				'L',			'L' },
	{ ITEM_ID_COLON,		DIK_SEMICOLON,		VK_OEM_1,		';' },
	{ ITEM_ID_QUOTE,		DIK_APOSTROPHE,		VK_OEM_7,		'\'' },
	{ ITEM_ID_TILDE,		DIK_GRAVE,			VK_OEM_3,		'`' },
	{ ITEM_ID_LSHIFT,		DIK_LSHIFT, 		VK_LSHIFT,		0 },
	{ ITEM_ID_BACKSLASH,	DIK_BACKSLASH,		VK_OEM_5,		'\\' },
	{ ITEM_ID_Z,			DIK_Z,				'Z',			'Z' },
	{ ITEM_ID_X,			DIK_X,				'X',			'X' },
	{ ITEM_ID_C,			DIK_C,				'C',			'C' },
	{ ITEM_ID_V,			DIK_V,				'V',			'V' },
	{ ITEM_ID_B,			DIK_B,				'B',			'B' },
	{ ITEM_ID_N,			DIK_N,				'N',			'N' },
	{ ITEM_ID_M,			DIK_M,				'M',			'M' },
	{ ITEM_ID_SLASH,		DIK_SLASH,			VK_OEM_2,		'/' },
	{ ITEM_ID_RSHIFT,		DIK_RSHIFT, 		VK_RSHIFT,		0 },
	{ ITEM_ID_ASTERISK, 	DIK_MULTIPLY,		VK_MULTIPLY,	'*' },
	{ ITEM_ID_LALT, 		DIK_LMENU,			VK_LMENU,		0 },
	{ ITEM_ID_SPACE,		DIK_SPACE,			VK_SPACE,		' ' },
	{ ITEM_ID_CAPSLOCK, 	DIK_CAPITAL,		VK_CAPITAL, 	0 },
	{ ITEM_ID_F1,			DIK_F1,				VK_F1,			0 },
	{ ITEM_ID_F2,			DIK_F2,				VK_F2,			0 },
	{ ITEM_ID_F3,			DIK_F3,				VK_F3,			0 },
	{ ITEM_ID_F4,			DIK_F4,				VK_F4,			0 },
	{ ITEM_ID_F5,			DIK_F5,				VK_F5,			0 },
	{ ITEM_ID_F6,			DIK_F6,				VK_F6,			0 },
	{ ITEM_ID_F7,			DIK_F7,				VK_F7,			0 },
	{ ITEM_ID_F8,			DIK_F8,				VK_F8,			0 },
	{ ITEM_ID_F9,			DIK_F9,				VK_F9,			0 },
	{ ITEM_ID_F10,			DIK_F10,			VK_F10, 		0 },
	{ ITEM_ID_NUMLOCK,		DIK_NUMLOCK,		VK_NUMLOCK, 	0 },
	{ ITEM_ID_SCRLOCK,		DIK_SCROLL,			VK_SCROLL,		0 },
	{ ITEM_ID_7_PAD,		DIK_NUMPAD7,		VK_NUMPAD7, 	0 },
	{ ITEM_ID_8_PAD,		DIK_NUMPAD8,		VK_NUMPAD8, 	0 },
	{ ITEM_ID_9_PAD,		DIK_NUMPAD9,		VK_NUMPAD9, 	0 },
	{ ITEM_ID_MINUS_PAD,	DIK_SUBTRACT,		VK_SUBTRACT,	0 },
	{ ITEM_ID_4_PAD,		DIK_NUMPAD4,		VK_NUMPAD4, 	0 },
	{ ITEM_ID_5_PAD,		DIK_NUMPAD5,		VK_NUMPAD5, 	0 },
	{ ITEM_ID_6_PAD,		DIK_NUMPAD6,		VK_NUMPAD6, 	0 },
	{ ITEM_ID_PLUS_PAD, 	DIK_ADD,			VK_ADD, 		0 },
	{ ITEM_ID_1_PAD,		DIK_NUMPAD1,		VK_NUMPAD1, 	0 },
	{ ITEM_ID_2_PAD,		DIK_NUMPAD2,		VK_NUMPAD2, 	0 },
	{ ITEM_ID_3_PAD,		DIK_NUMPAD3,		VK_NUMPAD3, 	0 },
	{ ITEM_ID_0_PAD,		DIK_NUMPAD0,		VK_NUMPAD0, 	0 },
	{ ITEM_ID_DEL_PAD,		DIK_DECIMAL,		VK_DECIMAL, 	0 },
	{ ITEM_ID_F11,			DIK_F11,			VK_F11, 		0 },
	{ ITEM_ID_F12,			DIK_F12,			VK_F12, 		0 },
	{ ITEM_ID_F13,			DIK_F13,			VK_F13, 		0 },
	{ ITEM_ID_F14,			DIK_F14,			VK_F14, 		0 },
	{ ITEM_ID_F15,			DIK_F15,			VK_F15, 		0 },
	{ ITEM_ID_ENTER_PAD,	DIK_NUMPADENTER,	VK_RETURN,		0 },
	{ ITEM_ID_RCONTROL, 	DIK_RCONTROL,		VK_RCONTROL,	0 },
	{ ITEM_ID_SLASH_PAD,	DIK_DIVIDE,			VK_DIVIDE,		0 },
	{ ITEM_ID_PRTSCR,		DIK_SYSRQ,			0,				0 },
	{ ITEM_ID_RALT, 		DIK_RMENU,			VK_RMENU,		0 },
	{ ITEM_ID_HOME, 		DIK_HOME,			VK_HOME,		0 },
	{ ITEM_ID_UP,			DIK_UP,				VK_UP,			0 },
	{ ITEM_ID_PGUP, 		DIK_PRIOR,			VK_PRIOR,		0 },
	{ ITEM_ID_LEFT, 		DIK_LEFT,			VK_LEFT,		0 },
	{ ITEM_ID_RIGHT,		DIK_RIGHT,			VK_RIGHT,		0 },
	{ ITEM_ID_END,			DIK_END,			VK_END, 		0 },
	{ ITEM_ID_DOWN, 		DIK_DOWN,			VK_DOWN,		0 },
	{ ITEM_ID_PGDN, 		DIK_NEXT,			VK_NEXT,		0 },
	{ ITEM_ID_INSERT,		DIK_INSERT,			VK_INSERT,		0 },
	{ ITEM_ID_DEL,			DIK_DELETE,			VK_DELETE,		0 },
	{ ITEM_ID_LWIN, 		DIK_LWIN,			VK_LWIN,		0 },
	{ ITEM_ID_RWIN, 		DIK_RWIN,			VK_RWIN,		0 },
	{ ITEM_ID_MENU, 		DIK_APPS,			VK_APPS,		0 },
	{ ITEM_ID_PAUSE,		DIK_PAUSE,			VK_PAUSE,		0 },
	{ ITEM_ID_CANCEL,		0,					VK_CANCEL,		0 },
};

typedef struct
{
	char name[40];	    					// functionality name (optional)
	input_seq is;							// the input sequence (the keys pressed)
	UINT func_id;        					// the identifier
	input_seq* (*const getiniptr)(void);	// pointer to function to get the value from .ini file
} GUISequence;

static const GUISequence GUISequenceControl[]=
{
	{"gui_key_up",                input_seq(),    ID_UI_UP,           		Get_ui_key_up },
	{"gui_key_down",              input_seq(),    ID_UI_DOWN,         		Get_ui_key_down },
	{"gui_key_left",              input_seq(),    ID_UI_LEFT,         		Get_ui_key_left },
	{"gui_key_right",             input_seq(),    ID_UI_RIGHT,        		Get_ui_key_right },
	{"gui_key_start",             input_seq(),    ID_UI_START,       		Get_ui_key_start },
	{"gui_key_pgup",              input_seq(),    ID_UI_PGUP,        		Get_ui_key_pgup },
	{"gui_key_pgdwn",             input_seq(),    ID_UI_PGDOWN,       		Get_ui_key_pgdwn },
	{"gui_key_home",              input_seq(),    ID_UI_HOME,         		Get_ui_key_home },
	{"gui_key_end",               input_seq(),    ID_UI_END,          		Get_ui_key_end },
	{"gui_key_ss_change",         input_seq(),    IDC_SSFRAME,        		Get_ui_key_ss_change },
	{"gui_key_history_up",        input_seq(),    ID_UI_HISTORY_UP,   		Get_ui_key_history_up },
	{"gui_key_history_down",      input_seq(),    ID_UI_HISTORY_DOWN, 		Get_ui_key_history_down },
	{"gui_key_context_filters",   input_seq(),    ID_CONTEXT_FILTERS,       Get_ui_key_context_filters },
	{"gui_key_select_random",     input_seq(),    ID_CONTEXT_SELECT_RANDOM, Get_ui_key_select_random },
	{"gui_key_game_audit",        input_seq(),    ID_GAME_AUDIT,            Get_ui_key_game_audit },
	{"gui_key_game_properties",   input_seq(),    ID_GAME_PROPERTIES,       Get_ui_key_game_properties },
	{"gui_key_help_contents",     input_seq(),    ID_HELP_CONTENTS,         Get_ui_key_help_contents },
	{"gui_key_update_gamelist",   input_seq(),    ID_UPDATE_GAMELIST,       Get_ui_key_update_gamelist },
	{"gui_key_view_folders",      input_seq(),    ID_VIEW_FOLDERS,          Get_ui_key_view_folders },
	{"gui_key_view_fullscreen",   input_seq(),    ID_VIEW_FULLSCREEN,       Get_ui_key_view_fullscreen },
	{"gui_key_view_pagetab",      input_seq(),    ID_VIEW_PAGETAB,          Get_ui_key_view_pagetab },
	{"gui_key_view_picture_area", input_seq(),    ID_VIEW_PICTURE_AREA,     Get_ui_key_view_picture_area },
	{"gui_key_view_status",       input_seq(),    ID_VIEW_STATUS,           Get_ui_key_view_status },
	{"gui_key_view_toolbars",     input_seq(),    ID_VIEW_TOOLBARS,         Get_ui_key_view_toolbars },
	{"gui_key_view_tab_cabinet",  input_seq(),    ID_VIEW_TAB_CABINET,      Get_ui_key_view_tab_cabinet },
	{"gui_key_view_tab_cpanel",   input_seq(),    ID_VIEW_TAB_CONTROL_PANEL,Get_ui_key_view_tab_cpanel },
	{"gui_key_view_tab_flyer",    input_seq(),    ID_VIEW_TAB_FLYER,        Get_ui_key_view_tab_flyer },
	{"gui_key_view_tab_history",  input_seq(),    ID_VIEW_TAB_HISTORY,      Get_ui_key_view_tab_history },
	{"gui_key_view_tab_marquee",  input_seq(),    ID_VIEW_TAB_MARQUEE,      Get_ui_key_view_tab_marquee },
	{"gui_key_view_tab_screenshot", input_seq(),  ID_VIEW_TAB_SCREENSHOT,   Get_ui_key_view_tab_screenshot },
	{"gui_key_view_tab_title",    input_seq(),    ID_VIEW_TAB_TITLE,        Get_ui_key_view_tab_title },
	{"gui_key_view_tab_pcb",      input_seq(),    ID_VIEW_TAB_SCORES, 	    Get_ui_key_view_tab_scores },
	{"gui_key_view_tab_pcb",      input_seq(),    ID_VIEW_TAB_PCB, 	      	Get_ui_key_view_tab_pcb },
	{"gui_key_quit",              input_seq(),    ID_FILE_EXIT,             Get_ui_key_quit },
};

#define NUM_GUI_SEQUENCES (sizeof(GUISequenceControl) / sizeof(GUISequenceControl[0]))

static UINT lastColumnClick = 0;
static WNDPROC g_lpPictureFrameWndProc = NULL;
static WNDPROC g_lpPictureWndProc = NULL;
/* Tool and Status bar variables */
static HWND hStatusBar = NULL;
static HWND hToolBar = NULL;
/* Used to recalculate the main window layout */
static int bottomMargin = 0;
static int topMargin = 0;
static int have_history = FALSE;
static BOOL have_selection = FALSE;
static HBITMAP hMissing_bitmap = NULL;
static HBRUSH hBrush = NULL;
static HBRUSH hBrushDlg = NULL;
static HDC hDC = NULL;
/* Icon variables */
static HIMAGELIST hLarge = NULL;
static HIMAGELIST hSmall = NULL;
static HICON hIcon = NULL;
static int *icon_index = NULL; 	/* for custom per-game icons */

static const TBBUTTON tbb[] =
{
	{0, ID_VIEW_FOLDERS,    	TBSTATE_ENABLED, BTNS_CHECK,      {0, 0}, 0, 0},
	{1, ID_VIEW_PICTURE_AREA,	TBSTATE_ENABLED, BTNS_CHECK,      {0, 0}, 0, 1},
	{0, 0,                  	TBSTATE_ENABLED, BTNS_SEP,        {0, 0}, 0, 0},
	{2, ID_VIEW_ICONS_LARGE,  	TBSTATE_ENABLED, BTNS_CHECKGROUP, {0, 0}, 0, 2},
	{3, ID_VIEW_ICONS_SMALL, 	TBSTATE_ENABLED, BTNS_CHECKGROUP, {0, 0}, 0, 3},
	{0, 0,                  	TBSTATE_ENABLED, BTNS_SEP,        {0, 0}, 0, 0},
	{4, ID_ENABLE_INDENT,  		TBSTATE_ENABLED, BTNS_CHECK,      {0, 0}, 0, 12},
	{0, 0,                  	TBSTATE_ENABLED, BTNS_SEP,        {0, 0}, 0, 0},
	{6, ID_UPDATE_GAMELIST,  	TBSTATE_ENABLED, BTNS_BUTTON,     {0, 0}, 0, 4},
	{0, 0,                    	TBSTATE_ENABLED, BTNS_SEP,        {0, 0}, 0, 0},
	{7, ID_OPTIONS_INTERFACE,	TBSTATE_ENABLED, BTNS_BUTTON,     {0, 0}, 0, 5},
	{8, ID_OPTIONS_DEFAULTS, 	TBSTATE_ENABLED, BTNS_BUTTON,     {0, 0}, 0, 6},
	{0, 0,                   	TBSTATE_ENABLED, BTNS_SEP,        {0, 0}, 0, 0},
	{9, ID_VIDEO_SNAP,			TBSTATE_ENABLED, BTNS_BUTTON,     {0, 0}, 0, 7},
	{10,ID_PLAY_M1,   			TBSTATE_ENABLED, BTNS_BUTTON,     {0, 0}, 0, 8},
	{0, 0,                    	TBSTATE_ENABLED, BTNS_SEP,        {0, 0}, 0, 0},
	{11,ID_HELP_ABOUT,      	TBSTATE_ENABLED, BTNS_BUTTON,     {0, 0}, 0, 9},
	{12,ID_HELP_CONTENTS,   	TBSTATE_ENABLED, BTNS_BUTTON,     {0, 0}, 0, 10},
	{0, 0,                    	TBSTATE_ENABLED, BTNS_SEP,        {0, 0}, 0, 0},
	{13,ID_MAME_HOMEPAGE,     	TBSTATE_ENABLED, BTNS_BUTTON,     {0, 0}, 0, 11},
	{0, 0,                    	TBSTATE_ENABLED, BTNS_SEP,        {0, 0}, 0, 0}
};

#define NUM_TOOLBUTTONS ARRAY_LENGTH(tbb)
#define NUM_TOOLTIPS (13)

static const TCHAR szTbStrings[NUM_TOOLTIPS + 1][30] =
{
	TEXT("Toggle folders list"),
	TEXT("Toggle pictures area"),
	TEXT("Large icons"),
	TEXT("Small icons"),
	TEXT("Refresh"),
	TEXT("Interface setttings"),
	TEXT("Default games options"),
	TEXT("Play ProgettoSnaps movie"),
	TEXT("M1FX"),
	TEXT("About"),
	TEXT("Help"),
	TEXT("MAME homepage"),
	TEXT("Toggle grouped view"),
};

static const int CommandToString[] =
{
	ID_VIEW_FOLDERS,
	ID_VIEW_PICTURE_AREA,
	ID_VIEW_ICONS_LARGE,
	ID_VIEW_ICONS_SMALL,
	ID_UPDATE_GAMELIST,
	ID_OPTIONS_INTERFACE,
	ID_OPTIONS_DEFAULTS,
	ID_VIDEO_SNAP,
	ID_PLAY_M1,
	ID_HELP_ABOUT,
	ID_HELP_CONTENTS,
	ID_MAME_HOMEPAGE,
	ID_ENABLE_INDENT,
	-1
};

static const int s_nPickers[] =
{
	IDC_LIST
};

/* How to resize toolbar sub window */
static ResizeItem toolbar_resize_items[] =
{
	{ RA_ID,   { ID_TOOLBAR_EDIT }, RA_LEFT | RA_TOP,     NULL },
    { RA_END,  { 0 },               0,                    NULL }
};

static Resize toolbar_resize = { {0, 0, 0, 0}, toolbar_resize_items };

/* How to resize main window */
static ResizeItem main_resize_items[] =
{
	{ RA_HWND, { 0 },            RA_LEFT  | RA_RIGHT  | RA_TOP,     &toolbar_resize },
	{ RA_HWND, { 0 },            RA_LEFT  | RA_RIGHT  | RA_BOTTOM,  NULL },
	{ RA_ID,   { IDC_DIVIDER },  RA_LEFT  | RA_RIGHT  | RA_TOP,     NULL },
	{ RA_ID,   { IDC_TREE },     RA_LEFT  | RA_BOTTOM | RA_TOP,     NULL },
	{ RA_ID,   { IDC_SPLITTER }, RA_LEFT  | RA_BOTTOM | RA_TOP,     NULL },
	{ RA_ID,   { IDC_LIST },     RA_ALL,                            NULL },
	{ RA_ID,   { IDC_SPLITTER2 },RA_RIGHT | RA_BOTTOM | RA_TOP,     NULL },
	{ RA_ID,   { IDC_SSTAB },    RA_RIGHT | RA_TOP,                 NULL },
	{ RA_ID,   { IDC_SSPICTURE },RA_RIGHT | RA_BOTTOM | RA_TOP,     NULL },
	{ RA_ID,   { IDC_HISTORY },  RA_RIGHT | RA_BOTTOM | RA_TOP,     NULL },
	{ RA_ID,   { IDC_SSFRAME },  RA_RIGHT | RA_BOTTOM | RA_TOP,     NULL },
	{ RA_ID,   { IDC_SSBORDER }, RA_RIGHT | RA_BOTTOM | RA_TOP,     NULL },
	{ RA_END,  { 0 },            0,                                 NULL }
};

static Resize main_resize = { {0, 0, 0, 0}, main_resize_items };

/* last directory for common file dialogs */
static TCHAR last_directory[MAX_PATH] = TEXT(".");
/* Last directory for Save Game or ROMs List dialogs */
static TCHAR list_directory[MAX_PATH] = TEXT(".");
static BOOL g_listview_dragging = FALSE;
static HIMAGELIST himl_drag = NULL;
static int game_dragged = 0; 				/* which game started the drag */
static HTREEITEM prev_drag_drop_target; 	/* which tree view item we're currently highlighting */
static BOOL g_in_treeview_edit = FALSE;
static srcdriver_data_type *sorted_srcdrivers;

/***************************************************************************
    Global variables
 ***************************************************************************/

/* Icon displayed in system tray */
static NOTIFYICONDATA MameIcon;

/* List view Column text */
extern const TCHAR* const column_names[COLUMN_MAX] =
{
	TEXT("Description"),
	TEXT("ROM name"),
	TEXT("Source file"),
	TEXT("Manufacturer"),
	TEXT("Year"),
	TEXT("Clone of"),
	TEXT("Played"),
	TEXT("Play time")
};


/***************************************************************************
    External functions
 ***************************************************************************/
class mameui_output_error : public osd_output
{
public:
	virtual void output_callback(osd_output_channel channel, const char *msg, va_list args)
 	{
 		if (channel == OSD_OUTPUT_CHANNEL_ERROR)
 		{
 			char buffer[4096];

 			// if we are in fullscreen mode, go to windowed mode
 			if ((video_config.windowed == 0) && (win_window_list != NULL))
 				winwindow_toggle_full_screen();

 			vsnprintf(buffer, ARRAY_LENGTH(buffer), msg, args);
 			win_message_box_utf8(win_window_list ? win_window_list->m_hwnd : NULL, buffer, MAMEUINAME, MB_ICONERROR | MB_OK);
 		}
		else
			chain_output(channel, msg, args);
 	}
};

static DWORD RunMAME(int nGameIndex, const play_options *playopts)
{
	time_t start = 0;
	time_t end = 0;
	int i = 0;
	windows_options mame_opts;
	std::string error_string;

	// Tell mame were to get the INIs
	SetDirectories(mame_opts);

	// add image specific device options
	mame_opts.set_system_name(driver_list::driver(nGameIndex).name);

	// set any specified play options
	if (playopts != NULL)
	{
		if (playopts->record != NULL)
			mame_opts.set_value(OPTION_RECORD, playopts->record, OPTION_PRIORITY_CMDLINE, error_string);
		
		if (playopts->playback != NULL)
			mame_opts.set_value(OPTION_PLAYBACK, playopts->playback, OPTION_PRIORITY_CMDLINE, error_string);
		
		if (playopts->state != NULL)
			mame_opts.set_value(OPTION_STATE, playopts->state, OPTION_PRIORITY_CMDLINE, error_string);
		
		if (playopts->wavwrite != NULL)
			mame_opts.set_value(OPTION_WAVWRITE, playopts->wavwrite, OPTION_PRIORITY_CMDLINE, error_string);
		
		if (playopts->mngwrite != NULL)
			mame_opts.set_value(OPTION_MNGWRITE, playopts->mngwrite, OPTION_PRIORITY_CMDLINE, error_string);
		
		if (playopts->aviwrite != NULL)
			mame_opts.set_value(OPTION_AVIWRITE, playopts->aviwrite, OPTION_PRIORITY_CMDLINE, error_string);
	}

	// Mame will parse all the needed .ini files.

	// prepare MAMEUIFX to run the game
	ShowWindow(hMain, SW_HIDE);

	for (i = 0; i < ARRAY_LENGTH(s_nPickers); i++)
		Picker_ClearIdle(GetDlgItem(hMain, s_nPickers[i]));

	// Time the game run.
	time(&start);
	windows_osd_interface osd(mame_opts);
	mameui_output_error winerror;
	osd_output::push(&winerror);
	osd.register_options();
	machine_manager *manager = machine_manager::instance(mame_opts, osd);
	manager->execute();
	osd_output::pop(&winerror);
	global_free(manager);
	// Calc the duration
	time(&end);
	double elapsedtime = end - start;
	// Increment our playtime
	IncrementPlayTime(nGameIndex, elapsedtime);

	// the emulation is complete; continue
	for (i = 0; i < ARRAY_LENGTH(s_nPickers); i++)
		Picker_ResetIdle(GetDlgItem(hMain, s_nPickers[i]));
	
	return 0;
}

static int HandleKeyboardGUIMessage(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam)
{
	switch (uMsg)
	{
		case WM_CHAR: /* List-View controls use this message for searching the items "as user types" */
			return TRUE;

		case WM_KEYDOWN:
			KeyboardKeyDown(0, wParam, lParam);
			return TRUE;

		case WM_KEYUP:
			KeyboardKeyUp(0, wParam, lParam);
			return TRUE;

		case WM_SYSKEYDOWN:
			KeyboardKeyDown(1, wParam, lParam);
			return TRUE;

		case WM_SYSKEYUP:
			KeyboardKeyUp(1, wParam, lParam);
			return TRUE;
	}
	
	return FALSE;	/* message not processed */
}

int MameUIMain(HINSTANCE hInstance, LPWSTR lpCmdLine)
{
	if (__argc != 1)
	{
		extern int utf8_main(int, char*[]);
		char **utf8_argv = NULL;
		int i = 0;

		/* convert arguments to UTF-8 */
		utf8_argv = (char **) malloc(__argc * sizeof(*__targv));
		
		if (utf8_argv == NULL)
			return 999;
		
		for (i = 0; i < __argc; i++)
		{
			utf8_argv[i] = utf8_from_tstring(__targv[i]);
			
			if (utf8_argv[i] == NULL)
			{
				free(utf8_argv);
				return 999;
			}
		}

		/* run utf8_main */
		int rc = utf8_main(__argc, utf8_argv);

		/* free arguments */
		for (i = 0; i < __argc; i++)
			free(utf8_argv[i]);
		
		free(utf8_argv);

		exit(rc);
	}

	WNDCLASS wndclass;
	MSG msg;
	hInst = hInstance;

	// set up window class
    memset(&wndclass, 0, sizeof(WNDCLASS));
	wndclass.style = 0; //CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = MameWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = DLGWINDOWEXTRA;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAMEUI_ICON));
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_UI_MENU);
	wndclass.lpszClassName = TEXT("MainClass");

	RegisterClass(&wndclass);
	hMain = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), 0, NULL);
	InitToolbar();
	InitStatusBar();
	InitProgressBar();
	InitTabView();
	InitMenuIcons();
	SetMainTitle();
	
	memset (&MameIcon, 0, sizeof(NOTIFYICONDATA));
	MameIcon.cbSize = sizeof(NOTIFYICONDATA);
	MameIcon.hWnd = hMain;
	MameIcon.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SYSTRAY));
	MameIcon.uID = 0;
	MameIcon.uCallbackMessage = WM_USER;
	MameIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	MameIcon.dwInfoFlags = NIIF_USER;
	MameIcon.uVersion = NOTIFYICON_VERSION;
	wcscpy(MameIcon.szInfoTitle, TEXT("MAMEUIFX"));
	wcscpy(MameIcon.szInfo, TEXT("Still running...."));
	wcscpy(MameIcon.szTip, TEXT("MAMEUIFX"));

	hSplash = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_STARTUP), 0, (DLGPROC)StartupProc);
	SetActiveWindow(hSplash);
	SetForegroundWindow(hSplash);

 	while(GetMessage(&msg, NULL, 0, 0))
    {
		if (IsWindow(hMain))
		{
			BOOL absorbed_key = FALSE;

			if (GetKeyGUI())
				absorbed_key = HandleKeyboardGUIMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			else
				absorbed_key = TranslateAccelerator(hMain, hAccel, &msg);

			if (!absorbed_key)
			{
				if (!IsDialogMessage(hMain, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
    }

	return msg.wParam;
}

HWND GetMainWindow(void)
{
	return hMain;
}

HWND GetTreeView(void)
{
	return hTreeView;
}

HWND GetProgressBar(void)
{
	return hProgress;
}

object_pool *GetMameUIMemoryPool(void)
{
	return mameui_pool;
}

void GetRealColumnOrder(int order[])
{
	int tmpOrder[COLUMN_MAX];
	int nColumnMax = Picker_GetNumColumns(hWndList);

	/* Get the Column Order and save it */
	(void)ListView_GetColumnOrderArray(hWndList, nColumnMax, tmpOrder);

	for (int i = 0; i < nColumnMax; i++)
	{
		order[i] = Picker_GetRealColumnFromViewColumn(hWndList, tmpOrder[i]);
	}
}

/*
 * PURPOSE: Format raw data read from an ICO file to an HICON
 * PARAMS:  PBYTE ptrBuffer  - Raw data from an ICO file
 *          UINT nBufferSize - Size of buffer ptrBuffer
 * RETURNS: HICON - handle to the icon, NULL for failure
 * History: July '95 - Created
 *          March '00- Seriously butchered from MSDN for mine own
 *          purposes, sayeth H0ek.
 */
static HICON FormatICOInMemoryToHICON(PBYTE ptrBuffer, UINT nBufferSize)
{
	ICONIMAGE IconImage;
	UINT nBufferIndex = 0;
	HICON hIcon = NULL;

	/* Is there a WORD? */
	if (nBufferSize < sizeof(WORD))
		return NULL;

	/* Was it 'reserved' ?   (ie 0) */
	if ((WORD)(ptrBuffer[nBufferIndex]) != 0)
		return NULL;

	nBufferIndex += sizeof(WORD);

	/* Is there a WORD? */
	if (nBufferSize - nBufferIndex < sizeof(WORD))
		return NULL;

	/* Was it type 1? */
	if ((WORD)(ptrBuffer[nBufferIndex]) != 1)
		return NULL;

	nBufferIndex += sizeof(WORD);

	/* Is there a WORD? */
	if (nBufferSize - nBufferIndex < sizeof(WORD))
		return NULL;

	/* Then that's the number of images in the ICO file */
	int nNumImages = (WORD)(ptrBuffer[nBufferIndex]);

	/* Is there at least one icon in the file? */
	if (nNumImages < 1)
		return NULL;

	nBufferIndex += sizeof(WORD);

	/* Is there enough space for the icon directory entries? */
	if ((nBufferIndex + nNumImages * sizeof(ICONDIRENTRY)) > nBufferSize)
		return NULL;

	/* Assign icon directory entries from buffer */
	LPICONDIRENTRY lpIDE = (LPICONDIRENTRY)(&ptrBuffer[nBufferIndex]);
	nBufferIndex += nNumImages * sizeof (ICONDIRENTRY);

	IconImage.dwNumBytes = lpIDE->dwBytesInRes;

	/* Seek to beginning of this image */
	if (lpIDE->dwImageOffset > nBufferSize)
		return NULL;

	nBufferIndex = lpIDE->dwImageOffset;

	/* Read it in */
	if ((nBufferIndex + lpIDE->dwBytesInRes) > nBufferSize)
		return NULL;

	IconImage.lpBits = &ptrBuffer[nBufferIndex];
	
	/* It failed, odds are good we're on NT so try the non-Ex way */
	if (hIcon == NULL)
	{
		/* We would break on NT if we try with a 16bpp image */
		if (((LPBITMAPINFO)IconImage.lpBits)->bmiHeader.biBitCount != 16)
			hIcon = CreateIconFromResourceEx(IconImage.lpBits, IconImage.dwNumBytes, TRUE, 0x00030000, 0, 0, LR_DEFAULTSIZE);
	}
	
	return hIcon;
}

HICON LoadIconFromFile(const char *iconname)
{
	HICON hIcon = NULL;
	WIN32_FIND_DATA FindFileData;
	char tmpStr[MAX_PATH];
	PBYTE bufferPtr;
	zip_error ziperr;
	zip_file *zip;

	snprintf(tmpStr, ARRAY_LENGTH(tmpStr), "%s\\%s.ico", GetIconsDir(), iconname);
	HANDLE hFind = win_find_first_file_utf8(tmpStr, &FindFileData);
	
	if (hFind == INVALID_HANDLE_VALUE || (hIcon = win_extract_icon_utf8(hInst, tmpStr, 0)) == 0)
	{
		char tmpIcoName[MAX_PATH];
		
		snprintf(tmpStr, ARRAY_LENGTH(tmpStr), "%s\\icons.zip", GetIconsDir());
		snprintf(tmpIcoName, ARRAY_LENGTH(tmpIcoName), "%s.ico", iconname);
		ziperr = zip_file_open(tmpStr, &zip);

		if (ziperr == ZIPERR_NONE)
		{
			const zip_file_header *entry = zip_file_first_file(zip);

			while(!hIcon && entry)
			{
				if (!core_stricmp(entry->filename, tmpIcoName))
				{
					bufferPtr = (PBYTE)malloc(entry->uncompressed_length);

					if (bufferPtr)
					{
						ziperr = zip_file_decompress(zip, bufferPtr, entry->uncompressed_length);

						if (ziperr == ZIPERR_NONE)
							hIcon = FormatICOInMemoryToHICON(bufferPtr, entry->uncompressed_length);
						
						free(bufferPtr);
					}
				}
				
				entry = zip_file_next_file(zip);
			}
			
			zip_file_close(zip);
		}
	}
	
	return hIcon;
}

/* Return the number of folders with options */
void SetNumOptionFolders(int count)
{
	optionfolder_count = count;
}

/* search */
const char * GetSearchText(void)
{
	return g_SearchText;
}

/* Sets the treeview and listviews sizes in accordance with their visibility and the splitters */
static void ResizeTreeAndListViews(BOOL bResizeHidden)
{
	RECT rect;
	int nLastWidth = SPLITTER_WIDTH;
	int nLastWidth2 = 0;
	int nLeftWindowWidth = 0;

	/* Size the List Control in the Picker */
	GetClientRect(hMain, &rect);

	if (bShowStatusBar)
		rect.bottom -= bottomMargin;
	
	if (bShowToolBar)
		rect.top += topMargin;

	/* Tree control */
	ShowWindow(GetDlgItem(hMain, IDC_TREE), bShowTree ? SW_SHOW : SW_HIDE);

	for (int i = 0; g_splitterInfo[i].nSplitterWindow; i++)
	{
		BOOL bVisible = (GetWindowLong(GetDlgItem(hMain, g_splitterInfo[i].nLeftWindow), GWL_STYLE) & WS_VISIBLE) ? TRUE : FALSE;

		if (bResizeHidden || bVisible)
		{
			nLeftWindowWidth = nSplitterOffset[i] - SPLITTER_WIDTH / 2 - nLastWidth;

			/* special case for the rightmost pane when the screenshot is gone */
			if (!GetShowScreenShot() && !g_splitterInfo[i + 1].nSplitterWindow)
				nLeftWindowWidth = rect.right - nLastWidth - 4;

			/* woah?  are we overlapping ourselves? */
			if (nLeftWindowWidth < MIN_VIEW_WIDTH)
			{
				nLastWidth = nLastWidth2;
				nLeftWindowWidth = nSplitterOffset[i] - MIN_VIEW_WIDTH - (SPLITTER_WIDTH * 3 / 2) - nLastWidth;
				i--;
			}

			MoveWindow(GetDlgItem(hMain, g_splitterInfo[i].nLeftWindow), nLastWidth, rect.top + 3, nLeftWindowWidth, (rect.bottom - rect.top) - 8, TRUE);
			MoveWindow(GetDlgItem(hMain, g_splitterInfo[i].nSplitterWindow), nSplitterOffset[i], rect.top + 3, SPLITTER_WIDTH, (rect.bottom - rect.top) - 8, TRUE);
		}

		if (bVisible)
		{
			nLastWidth2 = nLastWidth;
			nLastWidth += nLeftWindowWidth + SPLITTER_WIDTH;
		}
	}
}

/* Adjust the list view and screenshot button based on GetShowScreenShot() */
void UpdateScreenShot(void)
{
	RECT rect;
	RECT fRect;
	POINT p = {0, 0};

	/* first time through can't do this stuff */
	if (hWndList == NULL)
		return;

	/* Size the List Control in the Picker */
	GetClientRect(hMain, &rect);

	if (bShowStatusBar)
		rect.bottom -= bottomMargin;
	
	if (bShowToolBar)
		rect.top += topMargin;

	if (GetShowScreenShot())
	{
		CheckMenuItem(GetMenu(hMain),ID_VIEW_PICTURE_AREA, MF_CHECKED);
		ToolBar_CheckButton(hToolBar, ID_VIEW_PICTURE_AREA, MF_CHECKED);
	}
	else
	{
		CheckMenuItem(GetMenu(hMain),ID_VIEW_PICTURE_AREA, MF_UNCHECKED);
		ToolBar_CheckButton(hToolBar, ID_VIEW_PICTURE_AREA, MF_UNCHECKED);
	}

	ResizeTreeAndListViews(FALSE);
	FreeScreenShot();

	if (have_selection)
		LoadScreenShot(Picker_GetSelectedItem(hWndList), TabView_GetCurrentTab(hTabCtrl));

	// figure out if we have a history or not, to place our other windows properly
	UpdateHistory();

	// setup the picture area
	if (GetShowScreenShot())
	{
		ClientToScreen(hMain, &p);
		GetWindowRect(GetDlgItem(hMain, IDC_SSFRAME), &fRect);
		OffsetRect(&fRect, -p.x, -p.y);

		// show history on this tab IFF
		// - we have history for the game
		// - we're on the first tab
		// - we DON'T have a separate history tab
		BOOL showing_history = (have_history && (TabView_GetCurrentTab(hTabCtrl) == GetHistoryTab() || GetHistoryTab() == TAB_ALL ) &&
			GetShowTab(TAB_HISTORY) == FALSE);
		CalculateBestScreenShotRect(GetDlgItem(hMain, IDC_SSFRAME), &rect, showing_history);
		DWORD dwStyle = GetWindowLong(GetDlgItem(hMain, IDC_SSPICTURE), GWL_STYLE);
		DWORD dwStyleEx = GetWindowLong(GetDlgItem(hMain, IDC_SSPICTURE), GWL_EXSTYLE);
		AdjustWindowRectEx(&rect, dwStyle, FALSE, dwStyleEx);
		MoveWindow(GetDlgItem(hMain, IDC_SSPICTURE), fRect.left + rect.left, fRect.top + rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
		ShowWindow(GetDlgItem(hMain, IDC_SSPICTURE), (TabView_GetCurrentTab(hTabCtrl) != TAB_HISTORY) ? SW_SHOW : SW_HIDE);
		ShowWindow(GetDlgItem(hMain, IDC_SSFRAME), SW_SHOW);
		ShowWindow(GetDlgItem(hMain, IDC_SSTAB), bShowTabCtrl ? SW_SHOW : SW_HIDE);
		ShowWindow(GetDlgItem(hMain, IDC_SSBORDER), bShowTabCtrl ? SW_HIDE : SW_SHOW);
		InvalidateRect(GetDlgItem(hMain, IDC_SSBORDER), NULL, FALSE);
		InvalidateRect(GetDlgItem(hMain, IDC_SSTAB), NULL, FALSE);
		InvalidateRect(GetDlgItem(hMain, IDC_SSFRAME), NULL, FALSE);
		InvalidateRect(GetDlgItem(hMain, IDC_SSPICTURE), NULL, FALSE);
	}
	else
	{
		ShowWindow(GetDlgItem(hMain, IDC_SSPICTURE), SW_HIDE);
		ShowWindow(GetDlgItem(hMain, IDC_SSFRAME), SW_HIDE);
		ShowWindow(GetDlgItem(hMain, IDC_SSBORDER), SW_HIDE);
		ShowWindow(GetDlgItem(hMain, IDC_SSTAB), SW_HIDE);
	}
}

void ResizePickerControls(HWND hWnd)
{
	RECT frameRect;
	RECT rect, sRect;
	static BOOL firstTime = TRUE;
	BOOL doSSControls = TRUE;
	int nSplitterCount = GetSplitterCount();

	/* Size the List Control in the Picker */
	GetClientRect(hWnd, &rect);

	/* Calc the display sizes based on g_splitterInfo */
	if (firstTime)
	{
		RECT rWindow;

		for (int i = 0; i < nSplitterCount; i++)
			nSplitterOffset[i] = rect.right * g_splitterInfo[i].dPosition;

		GetWindowRect(hStatusBar, &rWindow);
		bottomMargin = rWindow.bottom - rWindow.top;
		GetWindowRect(hToolBar, &rWindow);
		topMargin = rWindow.bottom - rWindow.top;
		firstTime = FALSE;
	}
	else
		doSSControls = GetShowScreenShot();

	if (bShowStatusBar)
		rect.bottom -= bottomMargin;

	if (bShowToolBar)
		rect.top += topMargin;

	MoveWindow(GetDlgItem(hWnd, IDC_DIVIDER), rect.left - 1, rect.top - 3, rect.right + 2, 1, TRUE);
	ResizeTreeAndListViews(TRUE);
	int nListWidth = nSplitterOffset[nSplitterCount - 1];
	int nScreenShotWidth = (rect.right - nListWidth);

	/* Screen shot Page tab control */
	if (bShowTabCtrl)
	{
		MoveWindow(GetDlgItem(hWnd, IDC_SSTAB), nListWidth + 2, rect.top + 3, nScreenShotWidth - 4, (rect.bottom - rect.top) - 7, doSSControls);
		rect.top += 21;
	}
	else
		MoveWindow(GetDlgItem(hWnd, IDC_SSBORDER), nListWidth + 2, rect.top + 3, nScreenShotWidth - 6, (rect.bottom - rect.top) - 8, doSSControls);

	/* resize the Screen shot frame */
	MoveWindow(GetDlgItem(hWnd, IDC_SSFRAME), nListWidth + 3, rect.top + 4, nScreenShotWidth - 8, (rect.bottom - rect.top) - 10, doSSControls);
	/* The screen shot controls */
	GetClientRect(GetDlgItem(hWnd, IDC_SSFRAME), &frameRect);
	/* Text control - game history */
	sRect.left = nListWidth + 12;
	sRect.right = sRect.left + (nScreenShotWidth - 26);

	if (GetShowTab(TAB_HISTORY))
	{
		// We're using the new mode, with the history filling the entire tab (almost)
		sRect.top = rect.top + 16;
		sRect.bottom = (rect.bottom - rect.top) - 30;
	}
	else
	{
		// We're using the original mode, with the history beneath the SS picture
		sRect.top = rect.top + 264;
		sRect.bottom = (rect.bottom - rect.top) - 278;
	}

	MoveWindow(GetDlgItem(hWnd, IDC_HISTORY), sRect.left, sRect.top, sRect.right - sRect.left, sRect.bottom, doSSControls);

	/* the other screen shot controls will be properly placed in UpdateScreenshot() */
}

int GetMinimumScreenShotWindowWidth(void)
{
	BITMAP bmp;
	
	GetObject(hMissing_bitmap, sizeof(BITMAP), &bmp);
	return bmp.bmWidth + 6; 	// 6 is for a little breathing room
}

int GetParentIndex(const game_driver *driver)
{
	return GetGameNameIndex(driver->parent);
}

int GetParentRomSetIndex(const game_driver *driver)
{
	int nParentIndex = GetGameNameIndex(driver->parent);

	if( nParentIndex >= 0)
	{
		if ((driver_list::driver(nParentIndex).flags & MACHINE_IS_BIOS_ROOT) == 0)
			return nParentIndex;
	}

	return -1;
}

int GetGameNameIndex(const char *name)
{
	return driver_list::find(name);
}

int GetSrcDriverIndex(const char *name)
{
	srcdriver_data_type *srcdriver_index_info;
	srcdriver_data_type key;
	key.name = name;

	srcdriver_index_info = (srcdriver_data_type *)bsearch(&key, sorted_srcdrivers, driver_list::total(), sizeof(srcdriver_data_type), SrcDriverDataCompareFunc);

	if (srcdriver_index_info == NULL)
		return -1;

	return srcdriver_index_info->index;
}

/***************************************************************************
    Internal functions
 ***************************************************************************/

static int CLIB_DECL SrcDriverDataCompareFunc(const void *arg1, const void *arg2)
{
	return strcmp(((srcdriver_data_type *)arg1)->name, ((srcdriver_data_type *)arg2)->name);
}

static void SetMainTitle(void)
{
	char buffer[256];

	snprintf(buffer, ARRAY_LENGTH(buffer), "%s -ARCADES ONLY- %s", MAMEUINAME, GetVersionString());
	win_set_window_text_utf8(hMain, buffer);
}

static void memory_error(const char *message)
{
	ErrorMessageBox(message);
	exit(-1);
}

static BOOL Win32UI_init()
{
	RECT rect;
	int i = 0; 
	extern const FOLDERDATA g_folderData[];
	extern const FILTER_ITEM g_filterList[];
	LONG_PTR l;

	/* Init DirectInput */
	DirectInputInitialize();
	OptionsInit();

	if (GetRequiredDriverCacheStatus())
		win_set_window_text_utf8(GetDlgItem(hSplash, IDC_PROGBAR), "Building folders structure...");
	else
		win_set_window_text_utf8(GetDlgItem(hSplash, IDC_PROGBAR), "Loading folders structure...");

	srand((unsigned)time(NULL));
	// create the memory pool
	mameui_pool = pool_alloc_lib(memory_error);
	// custom per-game icons
	icon_index = (int*)pool_malloc_lib(mameui_pool, sizeof(int) * driver_list::total());
	memset(icon_index, 0, sizeof(int) * driver_list::total());
	// sorted list of source drivers by name
	sorted_srcdrivers = (srcdriver_data_type *) pool_malloc_lib(mameui_pool, sizeof(srcdriver_data_type) * driver_list::total());
	memset(sorted_srcdrivers, 0, sizeof(srcdriver_data_type) * driver_list::total());

	for (i = 0; i < driver_list::total(); i++)
	{
		const char *driver_name = core_strdup(GetDriverFilename(i));
		sorted_srcdrivers[i].name = driver_name;
		sorted_srcdrivers[i].index = i;
		free(driver_name);
	}
	
	qsort(sorted_srcdrivers, driver_list::total(), sizeof(srcdriver_data_type), SrcDriverDataCompareFunc);

	{
		struct TabViewOptions opts;

		static const struct TabViewCallbacks s_tabviewCallbacks =
		{
			GetShowTabCtrl,				// pfnGetShowTabCtrl
			SetCurrentTab,				// pfnSetCurrentTab
			GetCurrentTab,				// pfnGetCurrentTab
			SetShowTab,					// pfnSetShowTab
			GetShowTab,					// pfnGetShowTab
			GetImageTabShortName,		// pfnGetTabShortName
			GetImageTabLongName,		// pfnGetTabLongName
			UpdateScreenShot			// pfnOnSelectionChanged
		};

		memset(&opts, 0, sizeof(opts));
		opts.pCallbacks = &s_tabviewCallbacks;
		opts.nTabCount = MAX_TAB_TYPES;
		SetupTabView(hTabCtrl, &opts);
	}

	/* subclass picture frame area */
	l = GetWindowLongPtr(GetDlgItem(hMain, IDC_SSFRAME), GWLP_WNDPROC);
	g_lpPictureFrameWndProc = (WNDPROC)l;
	SetWindowLongPtr(GetDlgItem(hMain, IDC_SSFRAME), GWLP_WNDPROC, (LONG_PTR)PictureFrameWndProc);
	/* subclass picture area */
	l = GetWindowLongPtr(GetDlgItem(hMain, IDC_SSPICTURE), GWLP_WNDPROC);
	g_lpPictureWndProc = (WNDPROC)l;
	SetWindowLongPtr(GetDlgItem(hMain, IDC_SSPICTURE), GWLP_WNDPROC, (LONG_PTR)PictureWndProc);
	/* Load the pic for the default screenshot. */
	hMissing_bitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SNAPSHOT));
	main_resize_items[0].u.hwnd = hToolBar;
	main_resize_items[1].u.hwnd = hStatusBar;
	GetClientRect(hMain, &rect);
	InitListTree();
	InitSplitters();
	int nSplitterCount = GetSplitterCount();
	
	for (i = 0; i < nSplitterCount; i++)
	{
		HWND hWnd = GetDlgItem(hMain, g_splitterInfo[i].nSplitterWindow);
		HWND hWndLeft = GetDlgItem(hMain, g_splitterInfo[i].nLeftWindow);
		HWND hWndRight = GetDlgItem(hMain, g_splitterInfo[i].nRightWindow);
		AddSplitter(hWnd, hWndLeft, hWndRight, g_splitterInfo[i].pfnAdjust);
	}

	/* Initial adjustment of controls on the Picker window */
	ResizePickerControls(hMain);
	TabView_UpdateSelection(hTabCtrl);
	bShowTree = GetShowFolderList();
	bShowToolBar = GetShowToolBar();
	bShowStatusBar = GetShowStatusBar();
	bShowTabCtrl = GetShowTabCtrl();
	bEnableIndent = GetEnableIndent();
	CheckMenuItem(GetMenu(hMain), ID_VIEW_FOLDERS, (bShowTree) ? MF_CHECKED : MF_UNCHECKED);
	ToolBar_CheckButton(hToolBar, ID_VIEW_FOLDERS, (bShowTree) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(GetMenu(hMain), ID_VIEW_TOOLBARS, (bShowToolBar) ? MF_CHECKED : MF_UNCHECKED);
	ShowWindow(hToolBar, (bShowToolBar) ? SW_SHOW : SW_HIDE);
	CheckMenuItem(GetMenu(hMain), ID_VIEW_STATUS, (bShowStatusBar) ? MF_CHECKED : MF_UNCHECKED);
	ShowWindow(hStatusBar, (bShowStatusBar) ? SW_SHOW : SW_HIDE);
	CheckMenuItem(GetMenu(hMain), ID_VIEW_PAGETAB, (bShowTabCtrl) ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(GetMenu(hMain), ID_ENABLE_INDENT, (bEnableIndent) ? MF_CHECKED : MF_UNCHECKED);
	ToolBar_CheckButton(hToolBar, ID_ENABLE_INDENT, (bEnableIndent) ? MF_CHECKED : MF_UNCHECKED);
	InitTree(g_folderData, g_filterList);
	win_set_window_text_utf8(GetDlgItem(hSplash, IDC_PROGBAR), "Parsing datafiles...");
	/* Initialize listview columns */
	InitListView();
	SendMessage(hProgress, PBM_SETPOS, 120, 0);
	win_set_window_text_utf8(GetDlgItem(hSplash, IDC_PROGBAR), "Initializing window...");
	ResetFonts();
	AdjustMetrics();
	UpdateScreenShot();
	InitMainMenu(GetMenu(hMain));
	hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDA_TAB_KEYS));
	/* clear keyboard state */
	KeyboardStateClear();

	if (GetJoyGUI() == TRUE)
	{
		g_pJoyGUI = &DIJoystick;
		
		if (g_pJoyGUI->init() != 0)
			g_pJoyGUI = NULL;
		else
			SetTimer(hMain, JOYGUI_TIMER, JOYGUI_MS, NULL);
	}
	else
		g_pJoyGUI = NULL;

	if (GetHideMouseOnStartup())
	{
		/*  For some reason the mouse is centered when a game is exited, which of
            course causes a WM_MOUSEMOVE event that shows the mouse. So we center
            it now, before the startup coords are initilized, and that way the mouse
            will still be hidden when exiting from a game (i hope) :) */
		SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
		// Then hide it
		ShowCursor(FALSE);
	}

	if (GetRunFullScreen())
	{
		LONG lMainStyle;

		// Remove menu
		SetMenu(hMain, NULL);
		// Frameless dialog (fake fullscreen)
		lMainStyle = GetWindowLong(hMain, GWL_STYLE);
		lMainStyle = lMainStyle & (WS_BORDER ^ 0xffffffff);
		SetWindowLong(hMain, GWL_STYLE, lMainStyle);
	}

	game_index = 0;
	game_total = driver_list::total();
	oldpercent = -1;
	bDoGameCheck = FALSE;
	bFolderCheck = FALSE;
	idle_work = TRUE;

	switch (GetViewMode())
	{
		case VIEW_ICONS_LARGE :
			SetView(ID_VIEW_ICONS_LARGE);
			break;

		case VIEW_ICONS_SMALL :
		default :
			SetView(ID_VIEW_ICONS_SMALL);
			break;
	}
	
	UpdateListView();
	ShowWindow(hSplash, SW_HIDE);
	CenterWindow(hMain);
	ShowWindow(hMain, GetWindowState());
	SetActiveWindow(hMain);
	SetForegroundWindow(hMain);
	SetFocus(hWndList);

	if (GetCycleScreenshot() > 0)
		SetTimer(hMain, SCREENSHOT_TIMER, GetCycleScreenshot() * 1000, NULL); 	//scale to Seconds
	
	return TRUE;
}

static void Win32UI_exit()
{
	SaveWindowStatus();
	ShowWindow(hMain, SW_HIDE);

	if (GetMinimizeTrayIcon())
		Shell_NotifyIcon(NIM_DELETE, &MameIcon);

	if (g_pJoyGUI != NULL)
		g_pJoyGUI->exit();

	DeleteObject(hBrush);
	DeleteObject(hBrushDlg);
	DeleteBitmap(hAboutMenu);
	DeleteBitmap(hCustom);
	DeleteBitmap(hDirectories);
	DeleteBitmap(hExit);
	DeleteBitmap(hFullscreen);
	DeleteBitmap(hInterface);
	DeleteBitmap(hHelp);
	DeleteBitmap(hMameHome);
	DeleteBitmap(hPlay);
	DeleteBitmap(hPlayM1);
	DeleteBitmap(hOptions);
	DeleteBitmap(hRefresh);
	DeleteBitmap(hZip);
	DeleteBitmap(hSaveList);
	DeleteBitmap(hSaveRoms);
	DeleteBitmap(hPlayback);
	DeleteBitmap(hProperties);
	DeleteBitmap(hAuditMenu);
	DeleteBitmap(hVideo);
	DeleteBitmap(hFonts);
	DeleteBitmap(hFolders);
	DeleteBitmap(hSort);
	DeleteBitmap(hDriver);
	DeleteBitmap(hFaq);
	DeleteBitmap(hTabs);
	DeleteBitmap(hTrouble);
	DeleteBitmap(hCount);
	DeleteBitmap(hRelease);
	DeleteBitmap(hTime);
	DeleteBitmap(hDescription);
	DeleteBitmap(hRom);
	DeleteBitmap(hSource);
	DeleteBitmap(hManufacturer);
	DeleteBitmap(hYear);
	DeleteBitmap(hPlaywav);
	DeleteBitmap(hFont1);
	DeleteBitmap(hFont2);
	DeleteBitmap(hInfoback);
	DeleteBitmap(hListback);
	DeleteBitmap(hTreeback);
	DeleteBitmap(hAscending);
	DeleteBitmap(hFields);
	DeleteBitmap(hRecavi);
	DeleteBitmap(hRecinput);
	DeleteBitmap(hRecwav);
	DeleteBitmap(hPlaymng);
	DeleteBitmap(hRandom);
	DeleteBitmap(hRecmng);
	DeleteBitmap(hSavestate);
	DeleteBitmap(hFilters);
	DeleteBitmap(hRemove);
	DeleteBitmap(hRename);
	DeleteBitmap(hReset);
	DeleteBitmap(hMissing_bitmap);
	DeleteFont(hFontGui);
	DeleteFont(hFontList);
	DeleteFont(hFontHist);
	DeleteFont(hFontTree);
	DestroyIcons();
	DestroyAcceleratorTable(hAccel);
	DirectInputClose();
	SetSavedFolderID(GetCurrentFolderID());
	SaveInterface();
	SaveGameList();
	SaveGameDefaults();
	FreeFolders();
	FreeScreenShot();
	pool_free_lib(mameui_pool);
	mameui_pool = NULL;
	DestroyWindow(hSplash);
	DestroyWindow(hMain);
}

static LRESULT CALLBACK MameWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CTLCOLORSTATIC:
		hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);

		if ((HWND)lParam == GetDlgItem(hMain, IDC_HISTORY))
			SetTextColor(hDC, GetHistoryFontColor());

		return (LRESULT) hBrushDlg;
		
	case WM_INITDIALOG:
		/* Initialize info for resizing subitems */
		GetClientRect(hWnd, &main_resize.rect);
		return TRUE;

	case WM_SETFOCUS:
		UpdateWindow(hMain);
		SetFocus(hWndList);
		break;

	case WM_SETTINGCHANGE:
		AdjustMetrics();
		return 0;

	case WM_SIZE:
		OnSize(hWnd, wParam, LOWORD(lParam), HIWORD(wParam));
		return TRUE;

	case MM_PLAY_GAME:
		MamePlayGame();
		return TRUE;

	case WM_INITMENUPOPUP:
		UpdateMenu(GetMenu(hWnd));
		break;

	case WM_CONTEXTMENU:
		if (HandleTreeContextMenu(hWnd, wParam, lParam) || HandleScreenShotContextMenu(hWnd, wParam, lParam))
			return FALSE;
		break;

	case WM_COMMAND:
		return MameCommand(hMain,(int)(LOWORD(wParam)),(HWND)(lParam),(UINT)HIWORD(wParam));

	case WM_GETMINMAXINFO:
	{
		MINMAXINFO *mminfo;		
		/* Don't let the window get too small; it can break resizing */
		mminfo = (MINMAXINFO *) lParam;
		mminfo->ptMinTrackSize.x = MIN_WIDTH;
		mminfo->ptMinTrackSize.y = MIN_HEIGHT;
		return 0;
	}
	
	case WM_TIMER:
		switch (wParam)
		{
		case JOYGUI_TIMER:
			PollGUIJoystick();
			break;
			
		case SCREENSHOT_TIMER:
			TabView_CalculateNextTab(hTabCtrl);
			UpdateScreenShot();
			TabView_UpdateSelection(hTabCtrl);
			break;
			
		default:
			break;
		}
		
		return TRUE;

	case WM_CLOSE:
		{
			if (GetExitDialog())
			{
				if (win_message_box_utf8(hMain, "Are you sure you want to quit?", MAMEUINAME, MB_ICONQUESTION | MB_YESNO) == IDNO)
				{	
					SetFocus(hWndList);
					return TRUE;
				}
			}
			
			Win32UI_exit();
			break;
		}

	case WM_DESTROY:
        PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, (UINT)wParam, MAKEPOINTS(lParam));
		break;

		/*
		Check to see if the mouse has been moved by the user since
        startup. I'd like this checking to be done only in the
        main WinProc (here), but somehow the WM_MOUSEDOWN messages
        are eaten up before they reach MameWindowProc. That's why
        there is one check for each of the subclassed windows too.

        POSSIBLE BUGS:
        I've included this check in the subclassed windows, but a
        mouse move in either the title bar, the menu, or the
        toolbar will not generate a WM_MOUSEOVER message. At least
        not one that I know how to pick up. A solution could maybe
        be to subclass those too, but that's too much work :)
		*/

	case WM_MOUSEMOVE:
		{
		if (MouseHasBeenMoved())
			ShowCursor(TRUE);

		if (g_listview_dragging)
			MouseMoveListViewDrag(MAKEPOINTS(lParam));
		else
			/* for splitters */
			OnMouseMove(hWnd, (UINT)wParam, MAKEPOINTS(lParam));

		break;
		}

	case WM_LBUTTONUP:
	    if (g_listview_dragging)
			ButtonUpListViewDrag(MAKEPOINTS(lParam));
		else
			/* for splitters */
			OnLButtonUp(hWnd, (UINT)wParam, MAKEPOINTS(lParam));

		break;

	case WM_NOTIFY:
		/* Where is this message intended to go */
		{
			LPNMHDR lpNmHdr = (LPNMHDR)lParam;
			TCHAR szClass[128];
			
			/* Fetch tooltip text */
			if (lpNmHdr->code == TTN_NEEDTEXT)
			{
				LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
				CopyToolTipText(lpttt);
			}

			if (lpNmHdr->hwndFrom == hTreeView)
				return TreeViewNotify(lpNmHdr);

			GetClassName(lpNmHdr->hwndFrom, szClass, ARRAY_LENGTH(szClass));
			
			if (!_tcscmp(szClass, WC_LISTVIEW))
				return Picker_HandleNotify(lpNmHdr);
			
			if (!_tcscmp(szClass, WC_TABCONTROL))
				return TabView_HandleNotify(lpNmHdr);
		}
		
		break;

	case WM_USER:
		if (lParam == WM_LBUTTONDBLCLK)
		{
			Shell_NotifyIcon(NIM_DELETE, &MameIcon);
			ShowWindow(hMain, SW_RESTORE);
			SetActiveWindow(hMain);
			SetForegroundWindow(hMain);
			SetFocus(hWndList);
		}
		
		break;

	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			if (!IsMaximized(hMain))
				SaveWindowArea();
			
			if (GetMinimizeTrayIcon())
			{
				ShowWindow(hMain, SW_MINIMIZE);
				ShowWindow(hMain, SW_HIDE);
				Shell_NotifyIcon(NIM_ADD, &MameIcon);
				Shell_NotifyIcon(NIM_SETVERSION, &MameIcon);
			}
		}
		else if (wParam == SC_MAXIMIZE)
			SaveWindowArea();
		
		break;

	default:
		break;
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static void SaveWindowArea(void)
{
	/* save main window size */
	RECT rect;
	AREA area;

	GetWindowRect(hMain, &rect);
	area.x		= rect.left;
	area.y		= rect.top;
	area.width	= rect.right  - rect.left;
	area.height = rect.bottom - rect.top;
	SetWindowArea(&area);
}

static void SaveWindowStatus(void)
{
	WINDOWPLACEMENT wndpl;
	int i = 0;

	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hMain, &wndpl);
	UINT state = wndpl.showCmd;

	if (state == SW_MINIMIZE || state == SW_SHOWMINIMIZED)
		state = SW_RESTORE;
	else if(state == SW_MAXIMIZE)
	{
		state = SW_MAXIMIZE;
		ShowWindow(hMain, SW_RESTORE);
	}
	else
	{
		state = SW_SHOWNORMAL;
		SaveWindowArea();
	}

	SetWindowState(state);

	for (i = 0; i < GetSplitterCount(); i++)
		SetSplitterPos(i, nSplitterOffset[i]);
			
	for (i = 0; i < sizeof(s_nPickers) / sizeof(s_nPickers[0]); i++)
		Picker_SaveColumnWidths(GetDlgItem(hMain, s_nPickers[i]));

	int nItem = Picker_GetSelectedItem(hWndList);
	SetDefaultGame(driver_list::driver(nItem).name);
}

static void FolderCheck(void)
{
	int counter = ListView_GetItemCount(hWndList);

	for (int i = 0; i < counter; i++)
	{
		LVITEM lvi;

		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_PARAM;
		(void)ListView_GetItem(hWndList, &lvi);
		SetRomAuditResults(lvi.lParam, UNKNOWN);
	}

	game_index = 0;
	game_total = counter;
	oldpercent = -1;
	bDoGameCheck = FALSE;
	bFolderCheck = TRUE;
	idle_work = TRUE;
	ReloadIcons();
	Picker_ResetIdle(hWndList);
}

static BOOL GameCheck(void)
{
	LVFINDINFO lvfi;
	int i = 0;
	int percentage = ((game_index + 1) * 100) / game_total;
	BOOL changed = FALSE;

	if (game_index == 0)
		ProgressBarShow();

	if (bFolderCheck == TRUE)
	{
		LVITEM lvi;

		lvi.iItem = game_index;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_PARAM;
		(void)ListView_GetItem(hWndList, &lvi);
		MameUIVerifyRomSet(lvi.lParam, 1);
		changed = TRUE;
		lvfi.flags	= LVFI_PARAM;
		lvfi.lParam = lvi.lParam;
	}
	else
	{
		MameUIVerifyRomSet(game_index, 1);
		changed = TRUE;
		lvfi.flags	= LVFI_PARAM;
		lvfi.lParam = game_index;
	}

	i = ListView_FindItem(hWndList, -1, &lvfi);
	
	if (changed && i != -1)
		(void)ListView_RedrawItems(hWndList, i, i);

	if (percentage != oldpercent)
	{
		SetStatusBarTextF(0, "Game search %02d%% completed", percentage);
		oldpercent = percentage;
	}
	
	SendMessage(hProgWnd, PBM_SETPOS, game_index, 0);
	game_index++;
	
	if (game_index >= game_total)
	{
		bDoGameCheck = FALSE;
		bFolderCheck = FALSE;
		ProgressBarHide();
		ResetWhichGamesInFolders();
		return FALSE;
	}

	return changed;
}

static BOOL OnIdle(HWND hWnd)
{
	static BOOL bFirstTime = TRUE;

	if (bFirstTime)
		bFirstTime = FALSE;
	
	if ((bDoGameCheck) || (bFolderCheck))
	{
		GameCheck();
		return idle_work;
	}

	// in case it's not found, get it back
	int driver_index = Picker_GetSelectedItem(hWndList);
	const char *pDescription = driver_list::driver(driver_index).description;
	SetStatusBarText(0, pDescription);
	const char *pName = driver_list::driver(driver_index).name;
	SetStatusBarText(1, pName);
	idle_work = FALSE;
	UpdateStatusBar();
	bFirstTime = TRUE;

	if (game_launched)
	{
		game_launched = FALSE;
		return idle_work;
	}
	
	if (!idle_work)
		UpdateListView();

	return idle_work;
}

static void OnSize(HWND hWnd, UINT nState, int nWidth, int nHeight)
{
	static BOOL firstTime = TRUE;

	if (nState != SIZE_MAXIMIZED && nState != SIZE_RESTORED)
		return;

	ResizeWindow(hWnd, &main_resize);
	ResizeProgressBar();
	
	if (firstTime == FALSE)
		OnSizeSplitter(hMain);
	
	/* Update the splitters structures as appropriate */
	RecalcSplitters();
	
	if (firstTime == FALSE)
		ResizePickerControls(hMain);
	
	firstTime = FALSE;
	UpdateScreenShot();
}

static void ResizeWindow(HWND hParent, Resize *r)
{
	int cmkindex = 0;
	HWND hControl = NULL;
	RECT parent_rect, rect;
	POINT p = {0, 0};

	if (hParent == NULL)
		return;

	/* Calculate change in width and height of parent window */
	GetClientRect(hParent, &parent_rect);
	int dy = parent_rect.bottom - r->rect.bottom;
	int dx = parent_rect.right - r->rect.right;
	ClientToScreen(hParent, &p);

	while (r->items[cmkindex].type != RA_END)
	{
		const ResizeItem *ri = &r->items[cmkindex];
		
		if (ri->type == RA_ID)
			hControl = GetDlgItem(hParent, ri->u.id);
		else
			hControl = ri->u.hwnd;

		if (hControl == NULL)
		{
			cmkindex++;
			continue;
		}

		/* Get control's rectangle relative to parent */
		GetWindowRect(hControl, &rect);
		OffsetRect(&rect, -p.x, -p.y);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		if (!(ri->action & RA_LEFT))
			rect.left += dx;

		if (!(ri->action & RA_TOP))
			rect.top += dy;

		if (ri->action & RA_RIGHT)
			rect.right += dx;

		if (ri->action & RA_BOTTOM)
			rect.bottom += dy;
		//Sanity Check the child rect
		if (parent_rect.top > rect.top)
			rect.top = parent_rect.top;
		
		if (parent_rect.left > rect.left)
			rect.left = parent_rect.left;
		
		if (parent_rect.bottom < rect.bottom) 
		{
			rect.bottom = parent_rect.bottom;
			//ensure we have at least a minimal height
			rect.top = rect.bottom - height;
			
			if (rect.top < parent_rect.top) 
				rect.top = parent_rect.top;
		}
		
		if (parent_rect.right < rect.right) 
		{
			rect.right = parent_rect.right;
			//ensure we have at least a minimal width
			rect.left = rect.right - width;
			
			if (rect.left < parent_rect.left) 
				rect.left = parent_rect.left;
		}
		
		MoveWindow(hControl, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top), TRUE);

		/* Take care of subcontrols, if appropriate */
		if (ri->subwindow != NULL)
			ResizeWindow(hControl, (Resize*)ri->subwindow);

		cmkindex++;
	}

	/* Record parent window's new location */
	memcpy(&r->rect, &parent_rect, sizeof(RECT));
}

static void ProgressBarShow()
{
	RECT rect;
	int widths[2] = {160, -1};

	SendMessage(hStatusBar, SB_SETPARTS, 2, (LPARAM)widths);
	SendMessage(hProgWnd, PBM_SETRANGE, 0, MAKELPARAM(0, game_total));
	SendMessage(hProgWnd, PBM_SETPOS, 0, 0);
	StatusBar_GetItemRect(hStatusBar, 1, &rect);
	MoveWindow(hProgWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	ShowWindow(hProgWnd, SW_SHOW);
	bProgressShown = TRUE;
}

static void ProgressBarHide()
{
	RECT rect;
	int widths[6];
	int numParts = 6;

	if (hProgWnd == NULL)
		return;

	ShowWindow(hProgWnd, SW_HIDE);
	widths[5] = 96;
	widths[4] = 80;
	widths[3] = 160;
	widths[2] = 120;
	widths[1] = 88;
	widths[0] = -1;
	SendMessage(hStatusBar, SB_SETPARTS, 1, (LPARAM)widths);
	StatusBar_GetItemRect(hStatusBar, 0, &rect);
	widths[0] = (rect.right - rect.left) - (widths[1] + widths[2] + widths[3] + widths[4] + widths[5]);
	widths[1] += widths[0];
	widths[2] += widths[1];
	widths[3] += widths[2];
	widths[4] += widths[3];
	widths[5] += widths[4];
	SendMessage(hStatusBar, SB_SETPARTS, numParts, (LPARAM)widths);
	UpdateStatusBar();
	bProgressShown = FALSE;
}

static void ResizeProgressBar()
{
	if (bProgressShown)
	{
		RECT rect;
		int  widths[2] = {160, -1};

		SendMessage(hStatusBar, SB_SETPARTS, 2, (LPARAM)widths);
		StatusBar_GetItemRect(hStatusBar, 1, &rect);
		MoveWindow(hProgWnd, rect.left, rect.top, rect.right  - rect.left, rect.bottom - rect.top, TRUE);
	}
	else
		ProgressBarHide();
}

static void InitProgressBar(void)
{
	RECT rect;
	
	StatusBar_GetItemRect(hStatusBar, 0, &rect);
	rect.left += 160;
	hProgWnd = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_CLIPSIBLINGS, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hStatusBar, NULL, hInst, NULL);
	SetWindowTheme(hProgWnd, L" ", L" ");
	SendMessage(hProgWnd, PBM_SETBARCOLOR, 0, RGB(85, 191, 132));
}

static void InitMenuIcons(void)
{
	HBITMAP hTemp = NULL;

	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ABOUTMENU));
	hAboutMenu = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CUSTOM));
	hCustom = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DIRECTORIES));
	hDirectories = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_EXIT));
	hExit = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FULLSCREEN));
	hFullscreen = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_INTERFACE));
	hInterface = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HELP));
	hHelp = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MAMEHOME));
	hMameHome = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAY));
	hPlay = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYM1));
	hPlayM1 = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OPTIONS));
	hOptions = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_REFRESH));
	hRefresh = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ZIP));
	hZip = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SAVELIST));
	hSaveList = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SAVEROMS));
	hSaveRoms = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYBACK));
	hPlayback = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PROPERTIES));
	hProperties = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_AUDIT));
	hAuditMenu = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_VIDEO));
	hVideo = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FONTS));
	hFonts = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FOLDERS));
	hFolders = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SORT));
	hSort = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DRIVER));
	hDriver = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FAQ));
	hFaq = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TABS));
	hTabs = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TROUBLE));
	hTrouble = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_COUNT));
	hCount = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RELEASE));
	hRelease = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TIME));
	hTime = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DESCRIPTION));
	hDescription = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ROM));
	hRom = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SOURCE));
	hSource = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MANUFACTURER));
	hManufacturer = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_YEAR));
	hYear = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYWAV));
	hPlaywav = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FONT1));
	hFont1 = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FONT2));
	hFont2 = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_INFOBACK));
	hInfoback = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LISTBACK));
	hListback = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TREEBACK));
	hTreeback = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ASCENDING));
	hAscending = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FIELDS));
	hFields = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RECAVI));
	hRecavi = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RECINPUT));
	hRecinput = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RECWAV));
	hRecwav = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYMNG));
	hPlaymng = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RANDOM));
	hRandom = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RECMNG));
	hRecmng = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SAVESTATE));
	hSavestate = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FILTERS));
	hFilters = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_REMOVE));
	hRemove = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RENAME));
	hRename = CreateBitmapTransparent(hTemp);
	hTemp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RESET));
	hReset = CreateBitmapTransparent(hTemp);
}

static void CopyToolTipText(LPTOOLTIPTEXT lpttt)
{
	int iButton = lpttt->hdr.idFrom;
	int game = Picker_GetSelectedItem(hWndList);
	static TCHAR String[1024];
	BOOL bConverted = FALSE;

	/* Map command ID to string index */
	for (int i = 0; CommandToString[i] != -1; i++)
	{
		if (CommandToString[i] == iButton)
		{
			iButton = i;
			bConverted = TRUE;
			break;
		}
	}
	
	if (bConverted)
	{
		/* Check for valid parameter */
		if (iButton > NUM_TOOLTIPS)
			_tcscpy(String, TEXT("Invalid button index"));
		else
			_tcscpy(String, szTbStrings[iButton]);
	}
	else
		_tcscpy(String, tstring_from_utf8(driver_list::driver(game).description));

	lpttt->lpszText = String;
}

static void InitToolbar(void)
{
    RECT rect;

	hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 0, 0, NUM_TOOLBUTTONS * 32, 32, hMain, NULL, hInst, NULL);
	HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TOOLBAR), IMAGE_BITMAP, 0, 0, LR_SHARED);
    HIMAGELIST hToolList = ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, NUM_TOOLBUTTONS, 0);	
	ImageList_AddMasked(hToolList, hBitmap, RGB(0, 0, 0));
	DeleteObject(hBitmap);
    SendMessage(hToolBar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DOUBLEBUFFER); 	
    SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessage(hToolBar, TB_SETIMAGELIST, 0, (LPARAM)hToolList);
    SendMessage(hToolBar, TB_ADDBUTTONS, NUM_TOOLBUTTONS, (LPARAM)&tbb);
    SendMessage(hToolBar, TB_AUTOSIZE, 0, 0); 	
	// get Edit Control position
	int idx = SendMessage(hToolBar, TB_BUTTONCOUNT, 0, 0) - 1;
	SendMessage(hToolBar, TB_GETITEMRECT, idx, (LPARAM)&rect);
	int iPosX = rect.right + 8;
	int iPosY = (rect.bottom - rect.top) / 4;
	int iHeight = rect.bottom - rect.top - 17;
	// create Search Edit Control
	hSearchWnd = CreateWindowEx(0, WC_EDIT, TEXT(SEARCH_PROMPT), ES_LEFT | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | WS_VISIBLE, iPosX, iPosY, 200, iHeight, hToolBar, (HMENU)ID_TOOLBAR_EDIT, hInst, NULL );
}

static void InitTabView(void)
{
	hTabCtrl = CreateWindowEx(WS_EX_COMPOSITED, WC_TABCONTROL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK, 0, 0, 0, 0, hMain, (HMENU)IDC_SSTAB, hInst, NULL);
	HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_TABVIEW), IMAGE_BITMAP, 0, 0, LR_SHARED);
    HIMAGELIST hTabViewList = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, MAX_TAB_TYPES, 0);	
	ImageList_AddMasked(hTabViewList, hBitmap, RGB(255, 255, 255));
	DeleteObject(hBitmap);
    SendMessage(hTabCtrl, TCM_SETIMAGELIST, 0, (LPARAM)hTabViewList);
}

static void InitStatusBar(void)
{
	hStatusBar = CreateWindowEx(WS_EX_COMPOSITED, STATUSCLASSNAME, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SBARS_SIZEGRIP | SBARS_TOOLTIPS, 0, 0, 0, 0, hMain, NULL, hInst, NULL);
}

static char *GameInfoStatusBar(int driver_index)
{
	static char status[64];
	
	memset(&status, 0, sizeof(status));

	if (DriverIsBroken(driver_index))
		return strcpy(status, "Not working");
	else if (DriverIsImperfect(driver_index))
		return strcpy(status, "Working with problems");
	else
		return strcpy(status, "Working");
}

static char *GameInfoScreen(int driver_index)
{
	machine_config config(driver_list::driver(driver_index), MameUIGlobal());
	static char scrtxt[256];
	
	memset(&scrtxt, 0, sizeof(scrtxt));

	if (DriverIsVector(driver_index))
	{
		if (DriverIsVertical(driver_index))
			strcpy(scrtxt, "Vector (V)");
		else
			strcpy(scrtxt, "Vector (H)");
	}
	else
	{
		const screen_device *screen = config.first_screen();

		if (screen == NULL)
			strcpy(scrtxt, "Screenless");
		else
		{
			const rectangle &visarea = screen->visible_area();
			char tmpbuf[256];
			
			if (DriverIsVertical(driver_index))
				snprintf(tmpbuf, ARRAY_LENGTH(tmpbuf), "%d x %d (V) %f Hz", visarea.height(), visarea.width(), ATTOSECONDS_TO_HZ(screen->refresh_attoseconds()));
			else
				snprintf(tmpbuf, ARRAY_LENGTH(tmpbuf), "%d x %d (H) %f Hz", visarea.width(), visarea.height(), ATTOSECONDS_TO_HZ(screen->refresh_attoseconds()));

			strcat(scrtxt, tmpbuf);
		}
	}

	return scrtxt;
}

static void UpdateStatusBar(void)
{
	LPTREEFOLDER lpFolder = GetCurrentFolder();
	int games_shown = 0;
	int i = -1;
	HICON hIconFX = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON), IMAGE_ICON, 16, 16, LR_SHARED);
	
	if (!lpFolder)
		return;

	while (1)
	{
		i = FindGame(lpFolder, i + 1);
		
		if (i == -1)
			break;

		if (!GameFiltered(i, lpFolder->m_dwFlags))
			games_shown++;
	}

	/* Show number of games in the current 'View' in the status bar */
	SetStatusBarTextF(4, g_szGameCountString, games_shown);
	i = Picker_GetSelectedItem(hWndList);

	if (games_shown == 0)
		DisableSelection();
	else
	{
		const char *pText = driver_list::driver(i).description;
		char *pStatus = GameInfoStatusBar(i);
		char *pScreen = GameInfoScreen(i);
		const char *pName = driver_list::driver(i).name;
		SetStatusBarText(0, pText);
		SetStatusBarText(1, pName);
		SendMessage(hStatusBar, SB_SETICON, 1, (LPARAM)GetSelectedPickItemIconSmall());
		SetStatusBarText(2, pStatus);
		SetStatusBarText(3, pScreen);
		SetStatusBarText(5, MAMEUINAME);
		SendMessage(hStatusBar, SB_SETICON, 5, (LPARAM)hIconFX);
	}
}

static void ResetFonts(void)
{
	LOGFONT font;
	LOGFONT font1;
	LOGFONT font2;
	LOGFONT font3;
	
	GetGuiFont(&font);
	
	if (hFontGui != NULL)
		DeleteFont(hFontGui);
	
	hFontGui = CreateFontIndirect(&font);
	
	if (hFontGui != NULL)
	{
		SetWindowFont(hSearchWnd, hFontGui, TRUE);
		SetWindowFont(hTabCtrl, hFontGui, TRUE);
		SetWindowFont(hStatusBar, hFontGui, TRUE);
	}
	
	GetListFont(&font1);
	
	if (hFontList != NULL)
		DeleteFont(hFontList);
	
	hFontList = CreateFontIndirect(&font1);
	
	if (hFontList != NULL)
		SetWindowFont(hWndList, hFontList, TRUE);

	GetHistoryFont(&font2);
	
	if (hFontHist != NULL)
		DeleteFont(hFontHist);
	
	hFontHist = CreateFontIndirect(&font2);
	
	if (hFontHist != NULL)
		SetWindowFont(GetDlgItem(hMain, IDC_HISTORY), hFontHist, TRUE);

	GetTreeFont(&font3);
	
	if (hFontTree != NULL)
		DeleteFont(hFontTree);
	
	hFontTree = CreateFontIndirect(&font3);
	
	if (hFontTree != NULL)
		SetWindowFont(hTreeView, hFontTree, TRUE);
}

static void InitListTree(void)
{
	hTreeView = GetDlgItem(hMain, IDC_TREE);
	hWndList = GetDlgItem(hMain, IDC_LIST);
	SetWindowTheme(hWndList, L"Explorer", NULL);
	SetWindowTheme(hTreeView, L"Explorer", NULL);

	if (IsWindowsSevenOrHigher())
	{
		(void)ListView_SetExtendedListViewStyle(hWndList, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER);
		SendMessage(hTreeView, TVM_SETEXTENDEDSTYLE, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);
	}
	else
		(void)ListView_SetExtendedListViewStyle(hWndList, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP | LVS_EX_UNDERLINEHOT | LVS_EX_ONECLICKACTIVATE | LVS_EX_DOUBLEBUFFER);
}
	
static void UpdateHistory(void)
{
	have_history = FALSE;
	
	hBrushDlg = CreateSolidBrush(GetHistoryBgColor());

	if (GetSelectedPick() >= 0)
	{
		char *histText = GetGameHistory(Picker_GetSelectedItem(hWndList));
		have_history = (histText && histText[0]) ? TRUE : FALSE;
		win_set_window_text_utf8(GetDlgItem(hMain, IDC_HISTORY), histText);
	}

	if (have_history && GetShowScreenShot() && ((TabView_GetCurrentTab(hTabCtrl) == TAB_HISTORY) ||
		(TabView_GetCurrentTab(hTabCtrl) == GetHistoryTab() && GetShowTab(TAB_HISTORY) == FALSE) ||
		(TAB_ALL == GetHistoryTab() && GetShowTab(TAB_HISTORY) == FALSE) ))
		ShowWindow(GetDlgItem(hMain, IDC_HISTORY), SW_SHOW);
	else
		ShowWindow(GetDlgItem(hMain, IDC_HISTORY), SW_HIDE);
}

static void DisableSelection(void)
{
	MENUITEMINFO mmi;
	HMENU hMenu = GetMenu(hMain);
	BOOL prev_have_selection = have_selection;

	mmi.cbSize = sizeof(mmi);
	mmi.fMask = MIIM_TYPE;
	mmi.fType = MFT_STRING;
	mmi.dwTypeData = (TCHAR *)TEXT("&Play");
	mmi.cch = _tcslen(mmi.dwTypeData);
	
	SetMenuItemInfo(hMenu, ID_FILE_PLAY, FALSE, &mmi);
	EnableMenuItem(hMenu, ID_FILE_PLAY, 		MF_GRAYED);
	EnableMenuItem(hMenu, ID_FILE_PLAY_RECORD,	MF_GRAYED);
	EnableMenuItem(hMenu, ID_GAME_PROPERTIES,	MF_GRAYED);
	SetStatusBarText(0, "No selection");
	SetStatusBarText(1, "");
	SendMessage(hStatusBar, SB_SETICON, 1, (LPARAM)NULL);
	SetStatusBarText(2, "");
	SetStatusBarText(3, "");
	SetStatusBarText(4, "");
	have_selection = FALSE;

	if (prev_have_selection != have_selection)
		UpdateScreenShot();
}

static void EnableSelection(int nGame)
{
	TCHAR buf[200];
	MENUITEMINFO mmi;
	HMENU hMenu = GetMenu(hMain);
	TCHAR *t_description = tstring_from_utf8(ConvertAmpersandString(driver_list::driver(nGame).description));
	
	if( !t_description )
		return;

	_sntprintf(buf, ARRAY_LENGTH(buf), g_szPlayGameString, t_description);
	
	mmi.cbSize = sizeof(mmi);
	mmi.fMask = MIIM_TYPE;
	mmi.fType = MFT_STRING;
	mmi.dwTypeData = buf;
	mmi.cch = _tcslen(mmi.dwTypeData);
	
	SetMenuItemInfo(hMenu, ID_FILE_PLAY, FALSE, &mmi);
	const char *pText = driver_list::driver(nGame).description;
	SetStatusBarText(0, pText);
	const char *pName = driver_list::driver(nGame).name;
	SetStatusBarText(1, pName);
	SendMessage(hStatusBar, SB_SETICON, 1, (LPARAM)GetSelectedPickItemIconSmall());
	char *pStatus = GameInfoStatusBar(nGame);
	SetStatusBarText(2, pStatus);
	char *pScreen = GameInfoScreen(nGame);
	SetStatusBarText(3, pScreen);
	EnableMenuItem(hMenu, ID_FILE_PLAY, 		MF_ENABLED);
	EnableMenuItem(hMenu, ID_FILE_PLAY_RECORD,	MF_ENABLED);
	EnableMenuItem(hMenu, ID_GAME_PROPERTIES, 	MF_ENABLED);

	if (bProgressShown && bListReady == TRUE)
		SetDefaultGame(driver_list::driver(nGame).name);

	have_selection = TRUE;
	UpdateScreenShot();
	free(t_description);
}

static const char* GetCloneParentName(int nItem)
{
	if (DriverIsClone(nItem))
	{
		int nParentIndex = GetParentIndex(&driver_list::driver(nItem));
		
		if( nParentIndex >= 0)
			return driver_list::driver(nParentIndex).description;
	}
	
	return "";
}

static BOOL TreeViewNotify(LPNMHDR nm)
{
	switch (nm->code)
	{
	case TVN_SELCHANGED :
	{
		HTREEITEM hti = TreeView_GetSelection(hTreeView);
		TVITEM tvi;

		tvi.mask = TVIF_PARAM | TVIF_HANDLE;
		tvi.hItem = hti;

		if (TreeView_GetItem(hTreeView, &tvi))
		{
			SetCurrentFolder((LPTREEFOLDER)tvi.lParam);
			
			if (bListReady)
			{
				UpdateListView();
				UpdateScreenShot();
				SetFocus(hTreeView);
			}
		}
		
		return TRUE;
	}
	
	case TVN_BEGINLABELEDIT :
	{
		TV_DISPINFO *ptvdi = (TV_DISPINFO *)nm;
		LPTREEFOLDER folder = (LPTREEFOLDER)ptvdi->item.lParam;

		if (folder->m_dwFlags & F_CUSTOM)
		{
			// user can edit custom folder names
			g_in_treeview_edit = TRUE;
			return FALSE;
		}
		
		// user can't edit built in folder names
		return TRUE;
	}
	
	case TVN_ENDLABELEDIT :
	{
		TV_DISPINFO *ptvdi = (TV_DISPINFO *)nm;
		LPTREEFOLDER folder = (LPTREEFOLDER)ptvdi->item.lParam;

		g_in_treeview_edit = FALSE;

		if (ptvdi->item.pszText == NULL || _tcslen(ptvdi->item.pszText) == 0)
			return FALSE;

		char *utf8_szText = utf8_from_tstring(ptvdi->item.pszText);

		if( !utf8_szText )
			return FALSE;

		BOOL result = TryRenameCustomFolder(folder, utf8_szText);
		free(utf8_szText);
		return result;
	}
	}
	
	return FALSE;
}

static void GamePicker_OnHeaderContextMenu(POINT pt, int nColumn)
{
	// Right button was clicked on header
	MENUINFO mi;
	HMENU hMenuLoad = LoadMenu(hInst, MAKEINTRESOURCE(IDR_CONTEXT_HEADER));
	HMENU hMenu = GetSubMenu(hMenuLoad, 0);

	memset(&mi, 0, sizeof(MENUINFO));
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE;
	mi.dwStyle = MNS_CHECKORBMP;
	mi.hbrBack = GetSysColorBrush(COLOR_WINDOW);
	
	SetMenuInfo(hMenu, &mi);
	SetMenuItemBitmaps(hMenu, ID_SORT_ASCENDING, MF_BYCOMMAND, hAscending, hAscending);
	SetMenuItemBitmaps(hMenu, ID_SORT_DESCENDING, MF_BYCOMMAND, hSort, hSort);
	SetMenuItemBitmaps(hMenu, ID_CUSTOMIZE_FIELDS, MF_BYCOMMAND, hFields, hFields);
	lastColumnClick = nColumn;
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hMain, NULL);
	DestroyMenu(hMenuLoad);
}

static char* ConvertAmpersandString(const char *s)
{
	/* takes a string and changes any ampersands to double ampersands,
       for setting text of window controls that don't allow us to disable
       the ampersand underlining.
     	*/
	/* returns a static buffer--use before calling again */

	static char buf[200];

	char *ptr = buf;
	
	while (*s)
	{
		if (*s == '&')
			*ptr++ = *s;
		
		*ptr++ = *s++;
	}
	
	*ptr = 0;

	return buf;
}

static int GUI_seq_pressed(const input_seq *seq)
{
	int res = 1;
	int invert = 0;
	int count = 0;

	for (int codenum = 0; codenum < ARRAY_LENGTH(seq); codenum++)
	{
		input_code code = (*seq)[codenum];

		if (code == input_seq::not_code)
			invert = !invert;
		else if (code == input_seq::or_code)
		{
			if (res && count)
				return 1;
			
			res = 1;
			count = 0;
		}
		else
		{
			if (res)
			{
				if ((keyboard_state[(int)(code.item_id())] != 0) == invert)
					res = 0;
			}
			
			invert = 0;
			++count;
		}
	}
	
	return res && count;
}

static void check_for_GUI_action(void)
{
	for (int i = 0; i < NUM_GUI_SEQUENCES; i++)
	{
		const input_seq *is = &(GUISequenceControl[i].is);

		if (GUI_seq_pressed(is))
		{
			switch (GUISequenceControl[i].func_id)
			{
			case ID_GAME_AUDIT:
			case ID_GAME_PROPERTIES:
			case ID_CONTEXT_FILTERS:
			case ID_UI_START:
				KeyboardStateClear(); /* beacuse whe won't receive KeyUp mesage when we loose focus */
				break;

			default:
				break;
			}
			
			SendMessage(hMain, WM_COMMAND, GUISequenceControl[i].func_id, 0);
		}
	}
}

static void KeyboardStateClear(void)
{
	memset(&keyboard_state, 0, sizeof(keyboard_state));
}

static void KeyboardKeyDown(int syskey, int vk_code, int special)
{
	int found = 0;
	int icode = 0;
	int special_code = (special >> 24) & 1;
	int scancode = (special>>16) & 0xff;

	if ((vk_code==VK_MENU) || (vk_code==VK_CONTROL) || (vk_code==VK_SHIFT))
	{
		found = 1;

		/* a hack for right shift - it's better to use Direct X for keyboard input it seems......*/
		if (vk_code==VK_SHIFT)
		{
			if (scancode>0x30) /* on my keyboard left shift scancode is 0x2a, right shift is 0x36 */
				special_code = 1;
		}
		
		if (special_code) /* right hand keys */
		{
			switch(vk_code)
			{
			case VK_MENU:
				icode = (int)(KEYCODE_RALT.item_id());
				break;
				
			case VK_CONTROL:
				icode = (int)(KEYCODE_RCONTROL.item_id());
				break;
				
			case VK_SHIFT:
				icode = (int)(KEYCODE_RSHIFT.item_id());
				break;
			}
		}
		else /* left hand keys */
		{
			switch(vk_code)
			{
			case VK_MENU:
				icode = (int)(KEYCODE_LALT.item_id());
				break;
				
			case VK_CONTROL:
				icode = (int)(KEYCODE_LCONTROL.item_id());
				break;
				
			case VK_SHIFT:
				icode = (int)(KEYCODE_LSHIFT.item_id());
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < ARRAY_LENGTH(win_key_trans_table); i++)
		{
			if (vk_code == win_key_trans_table[i][VIRTUAL_KEY])
			{
				icode = win_key_trans_table[i][MAME_KEY];
				found = 1;
				break;
			}
		}
	}
	
	if (!found)
		return;

	keyboard_state[icode] = true;
	check_for_GUI_action();
}

static void KeyboardKeyUp(int syskey, int vk_code, int special)
{
	int found = 0;
	int icode = 0;
	int special_code = (special >> 24) & 1;
	int scancode = (special>>16) & 0xff;

	if ((vk_code==VK_MENU) || (vk_code==VK_CONTROL) || (vk_code==VK_SHIFT))
	{
		found = 1;

		/* a hack for right shift - it's better to use Direct X for keyboard input it seems......*/
		if (vk_code==VK_SHIFT)
		{
			if (scancode>0x30) /* on my keyboard left shift scancode is 0x2a, right shift is 0x36 */
				special_code = 1;
		}
		
		if (special_code) /* right hand keys */
		{
			switch(vk_code)
			{
			case VK_MENU:
				icode = (int)(KEYCODE_RALT.item_id());
				break;
				
			case VK_CONTROL:
				icode = (int)(KEYCODE_RCONTROL.item_id());
				break;
				
			case VK_SHIFT:
				icode = (int)(KEYCODE_RSHIFT.item_id());
				break;
			}
		}
		else /* left hand keys */
		{
			switch(vk_code)
			{
			case VK_MENU:
				icode = (int)(KEYCODE_LALT.item_id());
				break;
				
			case VK_CONTROL:
				icode = (int)(KEYCODE_LCONTROL.item_id());
				break;
				
			case VK_SHIFT:
				icode = (int)(KEYCODE_LSHIFT.item_id());
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < ARRAY_LENGTH(win_key_trans_table); i++)
		{
			if (vk_code == win_key_trans_table[i][VIRTUAL_KEY])
			{
				icode = win_key_trans_table[i][MAME_KEY];
				found = 1;
				break;
			}
		}
	}
	
	if (!found)
		return;

	keyboard_state[icode] = false;
	check_for_GUI_action();
}

static void PollGUIJoystick()
{
	if (in_emulation)
		return;

	if (g_pJoyGUI == NULL)
		return;

	g_pJoyGUI->poll_joysticks();
	
	// User pressed UP
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyUp(0), GetUIJoyUp(1), GetUIJoyUp(2), GetUIJoyUp(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_UP, 0);

	// User pressed DOWN
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyDown(0), GetUIJoyDown(1), GetUIJoyDown(2), GetUIJoyDown(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_DOWN, 0);

	// User pressed LEFT
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyLeft(0), GetUIJoyLeft(1), GetUIJoyLeft(2), GetUIJoyLeft(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_LEFT, 0);

	// User pressed RIGHT
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyRight(0), GetUIJoyRight(1), GetUIJoyRight(2), GetUIJoyRight(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_RIGHT, 0);

	// User pressed START GAME
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyStart(0), GetUIJoyStart(1), GetUIJoyStart(2), GetUIJoyStart(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_START, 0);

	// User pressed PAGE UP
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyPageUp(0), GetUIJoyPageUp(1), GetUIJoyPageUp(2), GetUIJoyPageUp(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_PGUP, 0);

	// User pressed PAGE DOWN
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyPageDown(0), GetUIJoyPageDown(1), GetUIJoyPageDown(2), GetUIJoyPageDown(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_PGDOWN, 0);

	// User pressed HOME
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyHome(0), GetUIJoyHome(1), GetUIJoyHome(2), GetUIJoyHome(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_HOME, 0);

	// User pressed END
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyEnd(0), GetUIJoyEnd(1), GetUIJoyEnd(2), GetUIJoyEnd(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_END, 0);

	// User pressed CHANGE SCREENSHOT
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoySSChange(0), GetUIJoySSChange(1), GetUIJoySSChange(2), GetUIJoySSChange(3))))
		SendMessage(hMain, WM_COMMAND, IDC_SSFRAME, 0);

	// User pressed SCROLL HISTORY UP
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyHistoryUp(0), GetUIJoyHistoryUp(1), GetUIJoyHistoryUp(2), GetUIJoyHistoryUp(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_HISTORY_UP, 0);

	// User pressed SCROLL HISTORY DOWN
	if (g_pJoyGUI->is_joy_pressed(JOYCODE(GetUIJoyHistoryDown(0), GetUIJoyHistoryDown(1), GetUIJoyHistoryDown(2), GetUIJoyHistoryDown(3))))
		SendMessage(hMain, WM_COMMAND, ID_UI_HISTORY_DOWN, 0);
}

static void SetView(int menu_id)
{
	int i = 0;

	// first uncheck previous menu item, check new one
	CheckMenuRadioItem(GetMenu(hMain), ID_VIEW_ICONS_LARGE, ID_VIEW_ICONS_SMALL, menu_id, MF_CHECKED);
	ToolBar_CheckButton(hToolBar, menu_id, MF_CHECKED);

	// Associate the image lists with the list view control.
	if (menu_id == ID_VIEW_ICONS_LARGE)
		(void)ListView_SetImageList(hWndList, hLarge, LVSIL_SMALL);
	else
		(void)ListView_SetImageList(hWndList, hSmall, LVSIL_SMALL);

	for (i = 0; i < sizeof(s_nPickers) / sizeof(s_nPickers[0]); i++)
		Picker_SetViewID(GetDlgItem(hMain, s_nPickers[i]), menu_id - ID_VIEW_ICONS_LARGE);

	for (i = 0; i < sizeof(s_nPickers) / sizeof(s_nPickers[0]); i++)
		Picker_Sort(GetDlgItem(hMain, s_nPickers[i]));
}

static void ResetListView()
{
	int i = 0;
	LVITEM lvi;
	BOOL no_selection = FALSE;
	LPTREEFOLDER lpFolder = GetCurrentFolder();

	if (!lpFolder)
		return;
 
	/* If the last folder was empty, no_selection is TRUE */
	if (have_selection == FALSE)
 		no_selection = TRUE;
 
	int current_game = Picker_GetSelectedItem(hWndList);
	SetWindowRedraw(hWndList, FALSE);
	(void)ListView_DeleteAllItems(hWndList);
	// hint to have it allocate it all at once
	ListView_SetItemCount(hWndList, driver_list::total());

	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_INDENT;
	lvi.stateMask = 0;

	i = -1;

	do
	{
		/* Add the games that are in this folder */
		if ((i = FindGame(lpFolder, i + 1)) != -1)
		{
			if (GameFiltered(i, lpFolder->m_dwFlags))
				continue;

			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.lParam = i;
			lvi.pszText = LPSTR_TEXTCALLBACK;
			lvi.iImage = I_IMAGECALLBACK;
			lvi.iIndent = 0;
			
			if (GetEnableIndent())
			{
				if (GetParentFound(i) && DriverIsClone(i))
					lvi.iIndent = 1;
				else
					lvi.iIndent = 0;
			}

			(void)ListView_InsertItem(hWndList, &lvi);
		}
	} while (i != -1);

	Picker_Sort(hWndList);

	if (bListReady)
	{
	    /* If last folder was empty, select the first item in this folder */
	    if (no_selection)
		    Picker_SetSelectedPick(hWndList, 0);
		else
		    Picker_SetSelectedItem(hWndList, current_game);
	}

	SetWindowRedraw(hWndList, TRUE);
	UpdateStatusBar();
}

static void UpdateGameList(void)
{
	for (int i = 0; i < driver_list::total(); i++)
	{
		SetRomAuditResults(i, UNKNOWN);
	}
	
	game_index = 0;
	game_total = driver_list::total();
	oldpercent = -1;
	bDoGameCheck = TRUE;
	bFolderCheck = FALSE;
	idle_work = TRUE;
	ReloadIcons();
	Picker_ResetIdle(hWndList);
}

static UINT_PTR CALLBACK HookProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_INITDIALOG:
			CenterWindow(hDlg);
        	hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        	SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			hBrush = CreateSolidBrush(RGB(224, 223, 227));
			break;

		case WM_CTLCOLORDLG:
			return (LRESULT) hBrush;	
		
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
			hDC = (HDC)wParam;
			SetBkMode(hDC, TRANSPARENT);
			return (LRESULT) hBrush;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				bChangedHook = TRUE;
				DeleteObject(hBrush);
				DestroyIcon(hIcon);
				break;

			case IDCANCEL:
				bChangedHook = FALSE;
				DeleteObject(hBrush);
				DestroyIcon(hIcon);
				break;
			}
	}

	return FALSE;
}

static void PickFont(LOGFONT *font, COLORREF *color)
{
	CHOOSEFONT cf;
    bChangedHook = FALSE;
	
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hMain;
	cf.lpLogFont = font;
	cf.lpfnHook = &HookProc;
	cf.rgbColors = *color;
	cf.Flags = CF_BOTH | CF_INITTOLOGFONTSTRUCT | CF_EFFECTS | CF_ENABLEHOOK;
	
	if (!ChooseFont(&cf))
		return;

	*color = cf.rgbColors;
}

static void PickListFont(void)
{
	LOGFONT FontList;
	
	GetListFont(&FontList); 
	COLORREF ColorList = GetListFontColor();
 	PickFont(&FontList, &ColorList);
	
	if (bChangedHook)
	{
		SetListFont(&FontList);
		SetListFontColor(ColorList);
		
		if (hFontList != NULL)
			DeleteFont(hFontList);
		
		hFontList = CreateFontIndirect(&FontList);
		
		if (hFontList != NULL)
		{
			SetWindowFont(hWndList, hFontList, TRUE);
			(void)ListView_SetTextColor(hWndList, ColorList);
			UpdateListView();
		}
	}
}

static void PickHistoryFont(void)
{
	LOGFONT FontHist;

	GetHistoryFont(&FontHist); 
	COLORREF ColorHist = GetHistoryFontColor();
 	PickFont(&FontHist, &ColorHist);
	
	if (bChangedHook)
	{
		SetHistoryFont(&FontHist);
		SetHistoryFontColor(ColorHist);
		
		if (hFontHist != NULL)
			DeleteFont(hFontHist);
		
		hFontHist = CreateFontIndirect(&FontHist);
		
		if (hFontHist != NULL)
			SetWindowFont(GetDlgItem(hMain, IDC_HISTORY), hFontHist, TRUE);
	}
}

static void PickFoldersFont(void)
{
	LOGFONT FontTree;

	GetTreeFont(&FontTree); 
	COLORREF ColorTree = GetTreeFontColor();
 	PickFont(&FontTree, &ColorTree);
	
	if (bChangedHook)
	{
		SetTreeFont(&FontTree);
		SetTreeFontColor(ColorTree);
		
		if (hFontTree != NULL)
			DeleteFont(hFontTree);
		
		hFontTree = CreateFontIndirect(&FontTree);
		
		if (hFontTree != NULL)
		{
			SetWindowFont(hTreeView, hFontTree, TRUE);
			(void)TreeView_SetTextColor(hTreeView, ColorTree);
		}
	}
}

static void PickColor(COLORREF *cDefault)
{
	CHOOSECOLOR cc;
	COLORREF choice_colors[16];
	bChangedHook = FALSE;
	int i = 0;

	for (i = 0; i < 16; i++)
		choice_colors[i] = GetCustomColor(i);

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hMain;
	cc.lpfnHook = &HookProc;
	cc.rgbResult = *cDefault;
	cc.lpCustColors = choice_colors;
	cc.Flags = CC_ANYCOLOR | CC_RGBINIT | CC_FULLOPEN | CC_ENABLEHOOK;
	
	if (!ChooseColor(&cc))
		return;
	
	for (i = 0; i < 16; i++)
		SetCustomColor(i,choice_colors[i]);
	
	*cDefault = cc.rgbResult;
}

static void PickTreeBgColor(void)
{
 	COLORREF cTreeColor = GetFolderBgColor();
 	PickColor(&cTreeColor);
	
	if (bChangedHook)
	{
		SetFolderBgColor(cTreeColor);
		(void)TreeView_SetBkColor(hTreeView, GetFolderBgColor());
	}
}
 
static void PickHistoryBgColor(void)
{
	COLORREF cHistoryColor = GetHistoryBgColor();
 	PickColor(&cHistoryColor);
	
	if (bChangedHook)
	{
		SetHistoryBgColor(cHistoryColor);
		UpdateScreenShot();
	}
}

static void PickListBgColor(void)
{
	COLORREF cListColor = GetListBgColor();
 	PickColor(&cListColor);
	
	if (bChangedHook)
	{
		SetListBgColor(cListColor);
		(void)ListView_SetBkColor(hWndList, GetListBgColor());
		UpdateListView();
	}
}

static BOOL MameCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
	LPTREEFOLDER folder;

	switch (id)
	{
	case ID_FILE_PLAY:
		MamePlayGame();
		SetFocus(hWndList);
		return TRUE;

	case ID_FILE_PLAY_RECORD:
		MamePlayRecordGame();
		SetFocus(hWndList);
		return TRUE;

	case ID_FILE_PLAY_BACK:
		MamePlayBackGame();
		SetFocus(hWndList);
		return TRUE;

	case ID_FILE_PLAY_RECORD_WAVE:
		MamePlayRecordWave();
		SetFocus(hWndList);
		return TRUE;

	case ID_FILE_PLAY_RECORD_MNG:
		MamePlayRecordMNG();
		SetFocus(hWndList);
		return TRUE;

	case ID_FILE_PLAY_RECORD_AVI:
		MamePlayRecordAVI();
		SetFocus(hWndList);
		return TRUE;

	case ID_FILE_LOADSTATE :
		MameLoadState();
		SetFocus(hWndList);
		return TRUE;

	case ID_FILE_AUDIT:
		AuditDialog(hMain);
		UpdateListView();
		return TRUE;

	case ID_FILE_GAMELIST:
		if (CommonListDialog(GetOpenFileName, FILETYPE_GAME_LIST))
			return TRUE;
		else
			break;

	case ID_FILE_ROMSLIST:
		if (CommonListDialog(GetOpenFileName, FILETYPE_ROMS_LIST))
			return TRUE;
		else
			break;

	case ID_FILE_EXIT:
		PostMessage(hMain, WM_CLOSE, 0, 0);
		return TRUE;

	case ID_VIEW_ICONS_LARGE:
		SetView(ID_VIEW_ICONS_LARGE);
		UpdateListView();
		return TRUE;

	case ID_VIEW_ICONS_SMALL:
		SetView(ID_VIEW_ICONS_SMALL);
		UpdateListView();
		return TRUE;

	/* Arrange Icons submenu */
	case ID_VIEW_BYGAME:
		SetSortReverse(FALSE);
		SetSortColumn(COLUMN_GAMES);
		Picker_Sort(hWndList);
		break;

	case ID_VIEW_BYDIRECTORY:
		SetSortReverse(FALSE);
		SetSortColumn(COLUMN_ROMNAME);
		Picker_Sort(hWndList);
		break;

	case ID_VIEW_BYMANUFACTURER:
		SetSortReverse(FALSE);
		SetSortColumn(COLUMN_MANUFACTURER);
		Picker_Sort(hWndList);
		break;

	case ID_VIEW_BYYEAR:
		SetSortReverse(FALSE);
		SetSortColumn(COLUMN_YEAR);
		Picker_Sort(hWndList);
		break;

	case ID_VIEW_BYSOURCE:
		SetSortReverse(FALSE);
		SetSortColumn(COLUMN_SOURCEFILE);
		Picker_Sort(hWndList);
		break;

	case ID_VIEW_BYTIMESPLAYED:
		SetSortReverse(FALSE);
		SetSortColumn(COLUMN_PLAYED);
		Picker_Sort(hWndList);
		break;

	case ID_ENABLE_INDENT:
		bEnableIndent = !bEnableIndent;
		SetEnableIndent(bEnableIndent);
		CheckMenuItem(GetMenu(hMain), ID_ENABLE_INDENT, (bEnableIndent) ? MF_CHECKED : MF_UNCHECKED);
		ToolBar_CheckButton(hToolBar, ID_ENABLE_INDENT, (bEnableIndent) ? MF_CHECKED : MF_UNCHECKED);
		UpdateListView();
		break;

	case ID_VIEW_FOLDERS:
		bShowTree = !bShowTree;
		SetShowFolderList(bShowTree);
		CheckMenuItem(GetMenu(hMain), ID_VIEW_FOLDERS, (bShowTree) ? MF_CHECKED : MF_UNCHECKED);
		ToolBar_CheckButton(hToolBar, ID_VIEW_FOLDERS, (bShowTree) ? MF_CHECKED : MF_UNCHECKED);
		UpdateScreenShot();
		break;

	case ID_VIEW_TOOLBARS:
		bShowToolBar = !bShowToolBar;
		SetShowToolBar(bShowToolBar);
		CheckMenuItem(GetMenu(hMain), ID_VIEW_TOOLBARS, (bShowToolBar) ? MF_CHECKED : MF_UNCHECKED);
		ToolBar_CheckButton(hToolBar, ID_VIEW_TOOLBARS, (bShowToolBar) ? MF_CHECKED : MF_UNCHECKED);
		ShowWindow(hToolBar, (bShowToolBar) ? SW_SHOW : SW_HIDE);
		ResizePickerControls(hMain);
		UpdateScreenShot();
		break;

	case ID_VIEW_STATUS:
		bShowStatusBar = !bShowStatusBar;
		SetShowStatusBar(bShowStatusBar);
		CheckMenuItem(GetMenu(hMain), ID_VIEW_STATUS, (bShowStatusBar) ? MF_CHECKED : MF_UNCHECKED);
		ShowWindow(hStatusBar, (bShowStatusBar) ? SW_SHOW : SW_HIDE);
		ResizePickerControls(hMain);
		UpdateScreenShot();
		break;

	case ID_VIEW_PAGETAB:
		bShowTabCtrl = !bShowTabCtrl;
		SetShowTabCtrl(bShowTabCtrl);
		ShowWindow(hTabCtrl, (bShowTabCtrl) ? SW_SHOW : SW_HIDE);
		ResizePickerControls(hMain);
		UpdateScreenShot();
		InvalidateRect(hMain, NULL, TRUE);
		break;

		/*
        Switches to fullscreen mode. No check mark handeling
        for this item cause in fullscreen mode the menu won't
        be visible anyways.
		*/
	case ID_VIEW_FULLSCREEN:
		SwitchFullScreenMode();
		break;

	case ID_TOOLBAR_EDIT:
		{
			char buf[256];

			win_get_window_text_utf8(hWndCtl, buf, ARRAY_LENGTH(buf));
			
			switch (codeNotify)
			{
			case TOOLBAR_EDIT_ACCELERATOR_PRESSED:
			{
				HWND hToolbarEdit = GetDlgItem(hToolBar, ID_TOOLBAR_EDIT);
				SetFocus(hToolbarEdit);
				break;
			}
			
			case EN_CHANGE:
				//put search routine here first, add a 200ms timer later.
				if ((!_stricmp(buf, SEARCH_PROMPT) && !_stricmp(g_SearchText, "")) ||
				    (!_stricmp(g_SearchText, SEARCH_PROMPT) && !_stricmp(buf, "")))
					strcpy(g_SearchText, buf);
				else
				{
					strcpy(g_SearchText, buf);
					ResetListView();
				}
				
				break;
				
			case EN_SETFOCUS:
				if (!_stricmp(buf, SEARCH_PROMPT))
					win_set_window_text_utf8(hWndCtl, "");
				
				break;
				
			case EN_KILLFOCUS:
				if (*buf == 0)
					win_set_window_text_utf8(hWndCtl, SEARCH_PROMPT);
				
				break;
			}
		}
		
		break;

	case ID_GAME_AUDIT:
		InitGameAudit(0);
		InitPropertyPageToPage(hInst, hWnd, GetSelectedPickItemIcon(), OPTIONS_GAME, -1, Picker_GetSelectedItem(hWndList), AUDIT_PAGE);
		UpdateStatusBar();
		SetFocus(hWndList);
	   	break;

	/* ListView Context Menu */
	case ID_CONTEXT_ADD_CUSTOM:
	{
		(void)DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CUSTOM_FILE), hMain, AddCustomFileDialogProc, Picker_GetSelectedItem(hWndList));
		SetFocus(hWndList);
		break;
	}

	case ID_CONTEXT_REMOVE_CUSTOM:
	{
	    RemoveCurrentGameCustomFolder();
		SetFocus(hWndList);
		break;
	}

	/* Tree Context Menu */
	case ID_CONTEXT_FILTERS:
		if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FILTERS), hMain, FilterDialogProc) == TRUE)
		UpdateListView();
		return TRUE;

	// ScreenShot Context Menu
	// select current tab
	case ID_VIEW_TAB_SCREENSHOT:
	case ID_VIEW_TAB_FLYER:
	case ID_VIEW_TAB_CABINET:
	case ID_VIEW_TAB_MARQUEE:
	case ID_VIEW_TAB_TITLE:
	case ID_VIEW_TAB_CONTROL_PANEL:
	case ID_VIEW_TAB_PCB:
	case ID_VIEW_TAB_SCORES:
	case ID_VIEW_TAB_HISTORY:
		if (id == ID_VIEW_TAB_HISTORY && GetShowTab(TAB_HISTORY) == FALSE)
			break;

		TabView_SetCurrentTab(hTabCtrl, id - ID_VIEW_TAB_SCREENSHOT);
		UpdateScreenShot();
		TabView_UpdateSelection(hTabCtrl);
		break;

	// toggle tab's existence
	case ID_TOGGLE_TAB_SCREENSHOT:
	case ID_TOGGLE_TAB_FLYER:
	case ID_TOGGLE_TAB_CABINET:
	case ID_TOGGLE_TAB_MARQUEE:
	case ID_TOGGLE_TAB_TITLE:
	case ID_TOGGLE_TAB_CONTROL_PANEL:
	case ID_TOGGLE_TAB_PCB:
	case ID_TOGGLE_TAB_SCORES:
	case ID_TOGGLE_TAB_HISTORY:
	{
		int toggle_flag = id - ID_TOGGLE_TAB_SCREENSHOT;

		if (AllowedToSetShowTab(toggle_flag,!GetShowTab(toggle_flag)) == FALSE)
			// attempt to hide the last tab
			// should show error dialog? hide picture area? or ignore?
			break;

		SetShowTab(toggle_flag,!GetShowTab(toggle_flag));
		TabView_Reset(hTabCtrl);

		if (TabView_GetCurrentTab(hTabCtrl) == toggle_flag && GetShowTab(toggle_flag) == FALSE)
			// we're deleting the tab we're on, so go to the next one
			TabView_CalculateNextTab(hTabCtrl);

		// Resize the controls in case we toggled to another history
		// mode (and the history control needs resizing).
		ResizePickerControls(hMain);
		UpdateScreenShot();
		TabView_UpdateSelection(hTabCtrl);
		break;
	}

	/* Header Context Menu */
	case ID_SORT_ASCENDING:
		SetSortReverse(FALSE);
		SetSortColumn(Picker_GetRealColumnFromViewColumn(hWndList, lastColumnClick));
		Picker_Sort(hWndList);
		break;

	case ID_SORT_DESCENDING:
		SetSortReverse(TRUE);
		SetSortColumn(Picker_GetRealColumnFromViewColumn(hWndList, lastColumnClick));
		Picker_Sort(hWndList);
		break;

	case ID_CUSTOMIZE_FIELDS:
		if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_COLUMNS), hMain, ColumnDialogProc) == TRUE)
			ResetColumnDisplay();
		SetFocus(hWndList);
		return TRUE;

	case ID_GAME_PROPERTIES:
		folder = GetFolderByName(FOLDER_SOURCE, GetDriverFilename(Picker_GetSelectedItem(hWndList)) );
		InitPropertyPage(hInst, hWnd, GetSelectedPickItemIcon(), OPTIONS_GAME, folder->m_nFolderId, Picker_GetSelectedItem(hWndList));
		UpdateStatusBar();
		SetFocus(hWndList);
		break;

	case ID_FOLDER_PROPERTIES:
	{
		OPTIONS_TYPE curOptType = OPTIONS_SOURCE;
		folder = GetSelectedFolder();
		
		if(folder->m_nFolderId == FOLDER_VECTOR) 
			curOptType = OPTIONS_VECTOR;
		else if(folder->m_nFolderId == FOLDER_HORIZONTAL) 
			curOptType = OPTIONS_HORIZONTAL;
		else if(folder->m_nFolderId == FOLDER_VERTICAL) 
			curOptType = OPTIONS_VERTICAL;

		InitPropertyPage(hInst, hWnd, GetSelectedFolderIcon(), curOptType, folder->m_nFolderId, Picker_GetSelectedItem(hWndList));
		UpdateStatusBar();
		SetFocus(hWndList);
		break;
	}
	case ID_FOLDER_SOURCEPROPERTIES:
		folder = GetFolderByName(FOLDER_SOURCE, GetDriverFilename(Picker_GetSelectedItem(hWndList)) );
		InitPropertyPage(hInst, hWnd, GetSelectedFolderIcon(), (folder->m_nFolderId == FOLDER_VECTOR) ? OPTIONS_VECTOR : OPTIONS_SOURCE, 
			folder->m_nFolderId, Picker_GetSelectedItem(hWndList));
		UpdateStatusBar();
		SetFocus(hWndList);
		break;

	case ID_FOLDER_VECTORPROPERTIES:
		folder = GetFolderByID( FOLDER_VECTOR );
		InitPropertyPage(hInst, hWnd, GetSelectedFolderIcon(), OPTIONS_VECTOR, folder->m_nFolderId, Picker_GetSelectedItem(hWndList));
		UpdateStatusBar();
		SetFocus(hWndList);
		break;

	case ID_FOLDER_AUDIT:
		FolderCheck();
		UpdateListView();
		UpdateStatusBar();
		break;

	case ID_VIEW_PICTURE_AREA :
		ToggleScreenShot();
		break;

	case ID_UPDATE_GAMELIST:
		UpdateGameList();
		UpdateListView();
		UpdateStatusBar();
		break;

	case ID_OPTIONS_FONT:
		PickListFont();
		SetFocus(hWndList);
		return TRUE;

	case ID_OPTIONS_HISTORY_FONT:
		PickHistoryFont();
		SetFocus(hWndList);
		return TRUE;

	case ID_OPTIONS_TREE_FONT:
		PickFoldersFont();
		SetFocus(hWndList);
		return TRUE;

	case ID_OPTIONS_FOLDERS_COLOR:
		PickTreeBgColor();
		SetFocus(hWndList);
		return TRUE;

	case ID_OPTIONS_HISTORY_COLOR:
		PickHistoryBgColor();
		SetFocus(hWndList);
		return TRUE;

	case ID_OPTIONS_LIST_COLOR:
		PickListBgColor();
		SetFocus(hWndList);
		return TRUE;

	case ID_OPTIONS_DEFAULTS:
		InitDefaultPropertyPage(hInst, hWnd);
		SetFocus(hWndList);
		return TRUE;

	case ID_OPTIONS_DIR:
		{
			int nResult = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIRECTORIES), hMain, DirectoriesDialogProc);
			BOOL bUpdateRoms = ((nResult & DIRDLG_ROM) == DIRDLG_ROM) ? TRUE : FALSE;

			/* update game list */
			if (bUpdateRoms == TRUE)
				UpdateGameList();

			SetFocus(hWndList);
		}
		
		return TRUE;

	case ID_OPTIONS_RESET_DEFAULTS:
		if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RESET), hMain, ResetDialogProc) == TRUE)
			PostMessage(hMain, WM_CLOSE, 0, 0);
		else 
			UpdateListView();
		return TRUE;

	case ID_OPTIONS_INTERFACE:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INTERFACE_OPTIONS), hMain, InterfaceDialogProc);
		KillTimer(hMain, SCREENSHOT_TIMER);

		if( GetCycleScreenshot() > 0)
			SetTimer(hMain, SCREENSHOT_TIMER, GetCycleScreenshot()*1000, NULL ); 	// Scale to seconds

		SetFocus(hWndList);
		return TRUE;

	case ID_MAME_HOMEPAGE:
		{
			ShellExecuteCommon(hMain, "http://www.mamedev.org");
			SetFocus(hWndList);
			return TRUE;
		}
		break;

	case ID_MAME_FAQ:
		{
			ShellExecuteCommon(hMain, "http://mamedev.org/devwiki/index.php?title=Frequently_Asked_Questions"); 
			SetFocus(hWndList);
			return TRUE;
		}
		break;

	case ID_PLAY_VIDEO:
		{
			char videoplay[MAX_PATH];
			*videoplay = 0;
			if (CommonFileDialog(GetOpenFileName, videoplay, FILETYPE_AVI_FILES))
				ShellExecuteCommon(hMain, videoplay);
			SetFocus(hWndList);
			return TRUE;
		}
		break;

	case ID_PLAY_AUDIO:
		{
			char audioplay[MAX_PATH];
			*audioplay = 0;
			if (CommonFileDialog(GetOpenFileName, audioplay, FILETYPE_WAVE_FILES))
				ShellExecuteCommon(hMain, audioplay);
			SetFocus(hWndList);
			return TRUE;
		}
		break;

	case ID_PLAY_MNG:
		{
			char mngplay[MAX_PATH];
			*mngplay = 0;
			if (CommonFileDialog(GetOpenFileName, mngplay, FILETYPE_MNG_FILES))
				ShellExecuteCommon(hMain, mngplay);
			SetFocus(hWndList);
			return TRUE;
		}
		break;

	case ID_VIEW_ZIP:
		{
			char viewzip[MAX_PATH];
			int nGame = Picker_GetSelectedItem(hWndList);
			snprintf(viewzip, ARRAY_LENGTH(viewzip), "%s\\%s.zip", GetRomDirs(), driver_list::driver(nGame).name);
			ShellExecuteCommon(hMain, viewzip);
			SetFocus(hWndList);
			return TRUE;
		}
		break;
		
	case ID_VIDEO_SNAP:
		{
			char videosnap[MAX_PATH];
			int nGame = Picker_GetSelectedItem(hWndList);

			if (DriverIsClone(nGame))
			{
				int nParent = GetParentIndex(&driver_list::driver(nGame));
				snprintf(videosnap, ARRAY_LENGTH(videosnap), "%s\\%s.mp4", GetMoviesDir(), driver_list::driver(nParent).name);
			}
			else
				snprintf(videosnap, ARRAY_LENGTH(videosnap), "%s\\%s.mp4", GetMoviesDir(), driver_list::driver(nGame).name);

			ShellExecuteCommon(hMain, videosnap);
			SetFocus(hWndList);
			return TRUE;
		}
		break;

	case ID_PLAY_M1:
		{
			char command[256];
			const char *game = NULL;
			int audit_result = 0;
			int nGame = Picker_GetSelectedItem(hWndList);
			
			if (DriverIsClone(nGame))
			{
				int nParent = GetParentIndex(&driver_list::driver(nGame));
				audit_result = GetRomAuditResults(nParent);
				game = driver_list::driver(nParent).name;
				snprintf(command, ARRAY_LENGTH(command), "m1fx.exe %s", game);
			}
			else
			{
				audit_result = GetRomAuditResults(nGame);
				game = driver_list::driver(nGame).name;
				snprintf(command, ARRAY_LENGTH(command), "m1fx.exe %s", game);
			}
			
			if (IsAuditResultYes(audit_result))
			{
				TCHAR* t_command = tstring_from_utf8(command);
				STARTUPINFO siStartupInfo;
    			PROCESS_INFORMATION piProcessInfo;
    			memset(&siStartupInfo, 0, sizeof(STARTUPINFO));
    			memset(&piProcessInfo, 0, sizeof(PROCESS_INFORMATION));
    			siStartupInfo.cb = sizeof(STARTUPINFO);
    			CreateProcess(NULL, t_command, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &siStartupInfo, &piProcessInfo);				
				free(t_command);
				SetFocus(hWndList);
				return TRUE;
			}
			else
			{
				ErrorMessageBox("Game '%s' is missing ROMs!\r\nM1FX cannot be executed!", game);
				SetFocus(hWndList);
				return TRUE;
			}	
		}
		break;

	case ID_HELP_ABOUT:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hMain, AboutDialogProc);
		SetFocus(hWndList);
		return TRUE;

	case IDOK :
		/* cmk -- might need to check more codes here, not sure */
		if (codeNotify != EN_CHANGE && codeNotify != EN_UPDATE)
		{
			/* enter key */
			if (g_in_treeview_edit)
			{
				(void)TreeView_EndEditLabelNow(hTreeView, FALSE);
				return TRUE;
			}
			else if (have_selection)
				MamePlayGame();
		}
		
		break;

	case IDCANCEL : /* esc key */
		if (g_in_treeview_edit)
			(void)TreeView_EndEditLabelNow(hTreeView, TRUE);
		
		break;

	case IDC_PLAY_GAME :
		if (have_selection)
			MamePlayGame();
		
		break;

	case ID_UI_START:
		SetFocus(hWndList);
		MamePlayGame();
		break;

	case ID_UI_UP:
		Picker_SetSelectedPick(hWndList, GetSelectedPick() - 1);
		break;

	case ID_UI_DOWN:
		Picker_SetSelectedPick(hWndList, GetSelectedPick() + 1);
		break;

	case ID_UI_PGUP:
		Picker_SetSelectedPick(hWndList, GetSelectedPick() - ListView_GetCountPerPage(hWndList));
		break;

	case ID_UI_PGDOWN:
		if ((GetSelectedPick() + ListView_GetCountPerPage(hWndList)) < ListView_GetItemCount(hWndList))
			Picker_SetSelectedPick(hWndList, GetSelectedPick() + ListView_GetCountPerPage(hWndList));
		else
			Picker_SetSelectedPick(hWndList, ListView_GetItemCount(hWndList) - 1);
		break;

	case ID_UI_HOME:
		Picker_SetSelectedPick(hWndList, 0);
		break;

	case ID_UI_END:
		Picker_SetSelectedPick(hWndList, ListView_GetItemCount(hWndList) - 1);
		break;
	case ID_UI_LEFT:
		/* hmmmmm..... */
		SendMessage(hWndList, WM_HSCROLL, SB_LINELEFT, 0);
		break;

	case ID_UI_RIGHT:
		/* hmmmmm..... */
		SendMessage(hWndList, WM_HSCROLL, SB_LINERIGHT, 0);
		break;
	case ID_UI_HISTORY_UP:
		/* hmmmmm..... */
		{
			HWND hHistory = GetDlgItem(hMain, IDC_HISTORY);
			SendMessage(hHistory, EM_SCROLL, SB_PAGEUP, 0);
		}
		break;

	case ID_UI_HISTORY_DOWN:
		/* hmmmmm..... */
		{
			HWND hHistory = GetDlgItem(hMain, IDC_HISTORY);
			SendMessage(hHistory, EM_SCROLL, SB_PAGEDOWN, 0);
		}
		break;

	case IDC_SSFRAME:
		TabView_CalculateNextTab(hTabCtrl);
		UpdateScreenShot();
		TabView_UpdateSelection(hTabCtrl);
		break;

	case ID_CONTEXT_SELECT_RANDOM:
		SetRandomPickItem();
		break;

	case ID_CONTEXT_RESET_PLAYCOUNT:
		ResetPlayCount(Picker_GetSelectedItem(hWndList));
		(void)ListView_RedrawItems(hWndList, GetSelectedPick(), GetSelectedPick());
		break;

	case ID_CONTEXT_RESET_PLAYTIME:
		ResetPlayTime(Picker_GetSelectedItem(hWndList));
		(void)ListView_RedrawItems(hWndList, GetSelectedPick(), GetSelectedPick());
		break;

	case ID_CONTEXT_RENAME_CUSTOM:
		(void)TreeView_EditLabel(hTreeView, TreeView_GetSelection(hTreeView));		
		break;

	case ID_HELP_CONTENTS :
		ShellExecuteCommon(hMain, "help.chm");
		SetFocus(hWndList);
		return TRUE;
		
	case ID_HELP_TROUBLE:
		ShellExecuteCommon(hMain, "http://mame32fx.altervista.org/forum/");
		SetFocus(hWndList);
		return TRUE;
		
	case ID_HELP_WHATS_NEW :
		ShellExecuteCommon(hMain, ".\\docs\\whatsnew.txt");
		SetFocus(hWndList);
		return TRUE;

	default:
		if (id >= ID_SHOW_FOLDER_START1 && id <= ID_SHOW_FOLDER_START28)
		{
			ToggleShowFolder((id - ID_SHOW_FOLDER_START1) + 1);
			break;
		}
		else if (id >= ID_CONTEXT_SHOW_FOLDER_START && id < ID_CONTEXT_SHOW_FOLDER_END)
		{
			ToggleShowFolder(id - ID_CONTEXT_SHOW_FOLDER_START);
			break;
		}
	}

	return FALSE;
}

static void ResetColumnDisplay(void)
{
	int driver_index = GetGameNameIndex(GetDefaultGame());

	Picker_ResetColumnDisplay(hWndList);
	UpdateListView();
	Picker_SetSelectedItem(hWndList, driver_index);
}

static int GamePicker_GetItemImage(HWND hwndPicker, int nItem)
{
	return GetIconForDriver(nItem);
}

static const TCHAR *GamePicker_GetItemString(HWND hwndPicker, int nItem, int nColumn, TCHAR *pszBuffer, UINT nBufferLength)
{
	const TCHAR *s = NULL;
	const char* utf8_s = NULL;
	char playtime_buf[256];
	char playcount_buf[256];

	switch(nColumn)
	{
		case COLUMN_GAMES:
			/* Driver description */
			utf8_s = driver_list::driver(nItem).description;
			break;
		
		case COLUMN_ROMNAME:
			/* Driver name (directory) */
			utf8_s = driver_list::driver(nItem).name;
			break;

		case COLUMN_MANUFACTURER:
			/* Manufacturer */
			utf8_s = driver_list::driver(nItem).manufacturer;
			break;

		case COLUMN_PLAYED:
			/* played count */
			snprintf(playcount_buf, ARRAY_LENGTH(playcount_buf), "%d",  GetPlayCount(nItem));
			utf8_s = playcount_buf;
			break;

		case COLUMN_PLAYTIME:
			/* played time */
			GetTextPlayTime(nItem, playtime_buf);
			utf8_s = playtime_buf;
			break;

		case COLUMN_YEAR:
			/* Year */
			utf8_s = driver_list::driver(nItem).year;
			break;

		case COLUMN_SOURCEFILE:
			/* Source drivers */
			utf8_s = GetDriverFilename(nItem);
			break;

		case COLUMN_CLONE:
			utf8_s = GetCloneParentName(nItem);
			break;
	}

	if(utf8_s)
	{
		TCHAR* t_s = tstring_from_utf8(utf8_s);
		
		if(!t_s)
			return s;

		_sntprintf(pszBuffer, nBufferLength, TEXT("%s"), t_s);
		free(t_s);
		s = pszBuffer;
	}

	return s;
}

static void GamePicker_LeavingItem(HWND hwndPicker, int nItem)
{
	// leaving item...
}

static void GamePicker_EnteringItem(HWND hwndPicker, int nItem)
{
	EnableSelection(nItem);
}

static int GamePicker_FindItemParent(HWND hwndPicker, int nItem)
{
	return GetParentRomSetIndex(&driver_list::driver(nItem));
}

static int GamePicker_CheckNotWorkingItem(HWND hwndPicker, int nItem)
{
	return DriverIsBroken(nItem);
}

/* Initialize the Picker and List controls */
static void InitListView(void)
{
	static const struct PickerCallbacks s_gameListCallbacks =
	{
		SetSortColumn,					/* pfnSetSortColumn */
		GetSortColumn,					/* pfnGetSortColumn */
		SetSortReverse,					/* pfnSetSortReverse */
		GetSortReverse,					/* pfnGetSortReverse */
		SetViewMode,					/* pfnSetViewMode */
		GetViewMode,					/* pfnGetViewMode */
		SetColumnWidths,				/* pfnSetColumnWidths */
		GetColumnWidths,				/* pfnGetColumnWidths */
		SetColumnOrder,					/* pfnSetColumnOrder */
		GetColumnOrder,					/* pfnGetColumnOrder */
		SetColumnShown,					/* pfnSetColumnShown */
		GetColumnShown,					/* pfnGetColumnShown */
		GamePicker_Compare,				/* pfnCompare */
		MamePlayGame,					/* pfnDoubleClick */
		GamePicker_GetItemString,		/* pfnGetItemString */
		GamePicker_GetItemImage,		/* pfnGetItemImage */
		GamePicker_LeavingItem,			/* pfnLeavingItem */
		GamePicker_EnteringItem,		/* pfnEnteringItem */
		BeginListViewDrag,				/* pfnBeginListViewDrag */
		GamePicker_FindItemParent,		/* pfnFindItemParent */
		GamePicker_CheckNotWorkingItem,	/* pfnCheckNotWorkingItem */
		OnIdle,							/* pfnIdle */
		GamePicker_OnHeaderContextMenu,	/* pfnOnHeaderContextMenu */
		GamePicker_OnBodyContextMenu	/* pfnOnBodyContextMenu */
	};

	struct PickerOptions opts;

	// subclass the list view
	memset(&opts, 0, sizeof(opts));
	opts.pCallbacks = &s_gameListCallbacks;
	opts.nColumnCount = COLUMN_MAX;
	opts.ppszColumnNames = column_names;
	
	SetupPicker(hWndList, &opts);
	(void)ListView_SetTextBkColor(hWndList, CLR_NONE);
	(void)ListView_SetBkColor(hWndList, CLR_NONE);
	CreateIcons();
	ResetWhichGamesInFolders();
	ResetColumnDisplay();
	// Allow selection to change the default saved game
	bListReady = TRUE;
}

static void AddDriverIcon(int nItem,int default_icon_index)
{
	/* if already set to rom or clone icon, we've been here before */
	if (icon_index[nItem] == 1 || icon_index[nItem] == 3)
		return;

	HICON hIcon = LoadIconFromFile((char *)driver_list::driver(nItem).name);
	
	if (hIcon == NULL)
	{
		int nParentIndex = GetParentIndex(&driver_list::driver(nItem));
		
		if( nParentIndex >= 0)
		{
			hIcon = LoadIconFromFile((char *)driver_list::driver(nParentIndex).name);
			nParentIndex = GetParentIndex(&driver_list::driver(nParentIndex));
			
			if (hIcon == NULL && nParentIndex >= 0)
				hIcon = LoadIconFromFile((char *)driver_list::driver(nParentIndex).name);
		}
	}

	if (hIcon != NULL)
	{
		int nIconPos = ImageList_AddIcon(hSmall, hIcon);
		ImageList_AddIcon(hLarge, hIcon);
		
		if (nIconPos != -1)
			icon_index[nItem] = nIconPos;
		
		DestroyIcon(hIcon);
	}
	
	if (icon_index[nItem] == 0)
		icon_index[nItem] = default_icon_index;
}

static void DestroyIcons(void)
{
	if (hSmall != NULL)
	{
		ImageList_Destroy(hSmall);
		hSmall = NULL;
	}

	if (icon_index != NULL)
	{
		for (int i = 0; i < driver_list::total(); i++)
			icon_index[i] = 0; 	// these are indices into hSmall
	}

	if (hLarge != NULL)
	{
		ImageList_Destroy(hLarge);
		hLarge = NULL;
	}
}

static void ReloadIcons(void)
{
	int i = 0;

	// clear out all the images
	ImageList_RemoveAll(hSmall);
	ImageList_RemoveAll(hLarge);

	if (icon_index != NULL)
	{
		for (i = 0; i < driver_list::total(); i++)
			icon_index[i] = 0; 	// these are indices into hSmall
	}

	for (i = 0; g_iconData[i].icon_name; i++)
	{
		HICON hIcon = LoadIconFromFile((char *) g_iconData[i].icon_name);
		
		if (hIcon == NULL)
			hIcon = LoadIcon(hInst, MAKEINTRESOURCE(g_iconData[i].resource));

		ImageList_AddIcon(hSmall, hIcon);
		ImageList_AddIcon(hLarge, hIcon);
		DestroyIcon(hIcon);
	}
}

// create iconlist for Listview control
static void CreateIcons(void)
{
	int icon_count = 0;
	int grow = 1000;
	
	while(g_iconData[icon_count].icon_name)
		icon_count++;

	hSmall = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK, icon_count, icon_count + grow);

	if (hSmall == NULL) 
	{
		ErrorMessageBox("Cannot allocate small icon image list!");
		PostQuitMessage(0);
	}

	hLarge = ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, icon_count, icon_count + grow);

	if (hLarge == NULL) 
	{
		ErrorMessageBox("Cannot allocate large icon image list!");
		PostQuitMessage(0);
	}

	ReloadIcons();
}


static int GamePicker_Compare(HWND hwndPicker, int index1, int index2, int sort_subitem)
{
	int value = 0;  	/* Default to 0, for unknown case */

	switch (sort_subitem)
	{
	case COLUMN_GAMES:
		return core_stricmp(driver_list::driver(index1).description, driver_list::driver(index2).description);

	case COLUMN_ROMNAME:
		value = core_stricmp(driver_list::driver(index1).name, driver_list::driver(index2).name);
		break;

	case COLUMN_MANUFACTURER:
		value = core_stricmp(driver_list::driver(index1).manufacturer, driver_list::driver(index2).manufacturer);
		break;

	case COLUMN_PLAYED:
		value = GetPlayCount(index1) - GetPlayCount(index2);
		break;

	case COLUMN_PLAYTIME:
		value = GetPlayTime(index1) - GetPlayTime(index2);
		break;

	case COLUMN_YEAR:
		value = core_stricmp(driver_list::driver(index1).year, driver_list::driver(index2).year);
		break;

	case COLUMN_SOURCEFILE:
	{
		char file1[20];
		char file2[20];
		strcpy(file1, GetDriverFilename(index1));
		strcpy(file2, GetDriverFilename(index2));
		value = core_stricmp(file1, file2);
		break;
	}
	
	case COLUMN_CLONE:
	{
		const char *name1 = GetCloneParentName(index1);
		const char *name2 = GetCloneParentName(index2);

		if (*name1 == '\0')
			name1 = NULL;
		
		if (*name2 == '\0')
			name2 = NULL;

		if (NULL == name1 && NULL == name2)
			value = 0;
		else if (name2 == NULL)
			value = -1;
		else if (name1 == NULL)
			value = 1;
		else
			value = core_stricmp(name1, name2);
		
		break;
	}
	}
	// Handle same comparisons here
	if (value == 0 && COLUMN_GAMES != sort_subitem)
		value = GamePicker_Compare(hwndPicker, index1, index2, COLUMN_GAMES);

	return value;
}

int GetSelectedPick()
{
	/* returns index of listview selected item */
	/* This will return -1 if not found */
	return ListView_GetNextItem(hWndList, -1, LVIS_SELECTED | LVIS_FOCUSED);
}

static HICON GetSelectedPickItemIcon()
{
	LVITEM lvi;

	lvi.iItem = GetSelectedPick();
	lvi.iSubItem = 0;
	lvi.mask = LVIF_IMAGE;
	(void)ListView_GetItem(hWndList, &lvi);

	return ImageList_GetIcon(hLarge, lvi.iImage, ILD_TRANSPARENT);
}

static HICON GetSelectedPickItemIconSmall()
{
	LVITEM lvi;

	lvi.iItem = GetSelectedPick();
	lvi.iSubItem = 0;
	lvi.mask = LVIF_IMAGE;
	(void)ListView_GetItem(hWndList, &lvi);

	return ImageList_GetIcon(hSmall, lvi.iImage, ILD_TRANSPARENT);
}

static void SetRandomPickItem()
{
	int nListCount = ListView_GetItemCount(hWndList);

	if (nListCount > 0)
		Picker_SetSelectedPick(hWndList, rand() % nListCount);
}

static UINT_PTR CALLBACK OFNHookProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_INITDIALOG:
			CenterWindow(GetParent(hWnd));
        	hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        	SendMessage(GetParent(hWnd), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			break;
	}

	return FALSE;
}

BOOL CommonFileDialog(common_file_dialog_proc cfd, char *filename, int filetype)
{
	BOOL success = FALSE;
	OPENFILENAME of;
	const char *path = NULL;
	TCHAR t_filename_buffer[MAX_PATH];
	TCHAR fCurDir[MAX_PATH];

	// convert the filename to UTF-8 and copy into buffer
	TCHAR *t_filename = tstring_from_utf8(filename);
	
	if (t_filename != NULL)
	{
		_sntprintf(t_filename_buffer, ARRAY_LENGTH(t_filename_buffer), TEXT("%s"), t_filename);
		free(t_filename);
	}

	if (GetCurrentDirectory(MAX_PATH, fCurDir) > MAX_PATH)
		fCurDir[0] = 0;

	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = hMain;
	of.hInstance = NULL;
	of.lpstrCustomFilter = NULL;
	of.nMaxCustFilter = 0;
	of.nFilterIndex = 1;
	of.lpstrFile = t_filename_buffer;
	of.nMaxFile = ARRAY_LENGTH(t_filename_buffer);
	of.lpstrFileTitle = NULL;
	of.nMaxFileTitle = 0;
	of.lpstrTitle = NULL;
	of.Flags  = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLEHOOK;
	of.nFileOffset = 0;
	of.nFileExtension = 0;

	switch (filetype)
	{
	case FILETYPE_INPUT_FILES :
		path = GetInpDir();
		of.lpstrInitialDir = tstring_from_utf8(path);
		of.lpstrFilter = TEXT("inputs (*.inp,*.zip)\0*.inp;*.zip\0");
		of.lpstrDefExt = TEXT("inp");
		break;
		
	case FILETYPE_SAVESTATE_FILES :
		path = GetStateDir();
		of.lpstrInitialDir = tstring_from_utf8(path);
		of.lpstrFilter = TEXT("savestates (*.sta)\0*.sta;\0");
		of.lpstrDefExt = TEXT("sta");
		break;
		
	case FILETYPE_WAVE_FILES :
		path = GetAudioDir();
		of.lpstrInitialDir = tstring_from_utf8(path);
		of.lpstrFilter = TEXT("sounds (*.wav)\0*.wav;\0");
		of.lpstrDefExt = TEXT("wav");
		break;
		
	case FILETYPE_MNG_FILES :
		path = GetVideoDir();
		of.lpstrInitialDir = tstring_from_utf8(path);
		of.lpstrFilter = TEXT("videos (*.mng)\0*.mng;\0");
		of.lpstrDefExt = TEXT("mng");
		break;
		
	case FILETYPE_AVI_FILES :
		path = GetVideoDir();
		of.lpstrInitialDir = tstring_from_utf8(path);
		of.lpstrFilter = TEXT("videos (*.avi)\0*.avi;\0");
		of.lpstrDefExt = TEXT("avi");
		break;
		
	case FILETYPE_EFFECT_FILES :
		path = GetArtDir();
		of.lpstrInitialDir = tstring_from_utf8(path);
		of.lpstrFilter = TEXT("effects (*.png)\0*.png;\0");
		of.lpstrDefExt = TEXT("png");
		break;
		
	case FILETYPE_SHADER_FILES :
		path = GetGLSLDir();
		of.lpstrInitialDir = tstring_from_utf8(path);
		of.lpstrFilter = TEXT("shaders (*.vsh)\0*.vsh;\0");
		of.lpstrDefExt = TEXT("vsh");
		break;
		
	case FILETYPE_CHEAT_FILES :
		of.lpstrInitialDir = last_directory;
		of.lpstrFilter = TEXT("cheats (*.7z,*.zip)\0*.7z;*.zip;\0");
		of.lpstrDefExt = TEXT("7z");
		break;
		
	case FILETYPE_DEBUGSCRIPT_FILES :
		of.lpstrInitialDir = last_directory;
		of.lpstrFilter = TEXT("scripts (*.txt)\0*.txt;\0");
		of.lpstrDefExt = TEXT("txt");
		break;
	}
	
	of.lCustData = 0;
	of.lpfnHook = &OFNHookProc;
	of.lpTemplateName = NULL;

	success = cfd(&of);
	
	if (success)
	{
		GetCurrentDirectory(MAX_PATH, last_directory);

		if (fCurDir[0] != 0)
			SetCurrentDirectory(fCurDir);
	}

	char *utf8_filename = utf8_from_tstring(t_filename_buffer);
	
	if (utf8_filename != NULL)
	{
		snprintf(filename, MAX_PATH, "%s", utf8_filename);
		free(utf8_filename);
	}

	return success;
}

void SetStatusBarText(int part_index, const char *message)
{
	TCHAR *t_message = tstring_from_utf8(message);
	
	if(!t_message)
		return;
	
	SendMessage(hStatusBar, SB_SETTEXT, part_index, (LPARAM)t_message);
	free(t_message);
}

void SetStatusBarTextF(int part_index, const char *fmt, ...)
{
	char buf[256];
	va_list va;

	va_start(va, fmt);
	vsnprintf(buf, ARRAY_LENGTH(buf), fmt, va);
	va_end(va);
	SetStatusBarText(part_index, buf);
}

static void MamePlayBackGame(void)
{
	int nGame = Picker_GetSelectedItem(hWndList);
	char filename[MAX_PATH];
	play_options playopts;

	memset(&playopts, 0, sizeof(playopts));
	*filename = 0;

	if (CommonFileDialog(GetOpenFileName, filename, FILETYPE_INPUT_FILES))
	{
		file_error filerr;
		char name[MAX_PATH];
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		char *fname = utf8_from_tstring(tempname);
		strcpy(name, fname);
		free(t_filename);
		free(fname);
		
		emu_file check(GetInpDir(), OPEN_FLAG_READ);
		filerr = check.open(name);
		
		if (filerr != FILERR_NONE)
		{
			ErrorMessageBox("Could not open '%s' as a valid input file.", name);
			return;
		}

		// check for game name embedded in .inp header
		inp_header ihdr;

		// read the header and verify that it is a modern version; if not, print an error
		if (check.read(&ihdr, sizeof(inp_header)) != sizeof(inp_header))
		{
			ErrorMessageBox("Input file is corrupt or invalid (missing header).");
			return;
		}

		// find game and play it
		for (int i = 0; i < driver_list::total(); i++)
		{
			if (strcmp(driver_list::driver(i).name, ihdr.gamename) == 0)
			{
				nGame = i;
				break;
			}
		}

		playopts.playback = name;
		MamePlayGameWithOptions(nGame, &playopts);
	}
}

static void MameLoadState(void)
{
	int nGame = Picker_GetSelectedItem(hWndList);
	char filename[MAX_PATH];
	play_options playopts;

	memset(&playopts, 0, sizeof(playopts));
	*filename = 0;
	
	if (CommonFileDialog(GetOpenFileName, filename, FILETYPE_SAVESTATE_FILES))
	{
		char name[MAX_PATH];
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		PathRemoveExtension(tempname);
		char *fname = utf8_from_tstring(tempname);
		strcpy(name, fname);
		free(t_filename);
		free(fname);
		playopts.state = name;
		MamePlayGameWithOptions(nGame, &playopts);
	}
}

static void MamePlayRecordGame(void)
{
	int nGame = Picker_GetSelectedItem(hWndList);
	char filename[MAX_PATH];
	play_options playopts;
	
	memset(&playopts, 0, sizeof(playopts));
	*filename = 0;
	strcpy(filename, driver_list::driver(nGame).name);
	
	if (CommonFileDialog(GetSaveFileName, filename, FILETYPE_INPUT_FILES))
	{
		char name[MAX_PATH];
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		char *fname = utf8_from_tstring(tempname);
		strcpy(name, fname);
		free(t_filename);
		free(fname);
		playopts.record = name;
		MamePlayGameWithOptions(nGame, &playopts);
	}
}

void MamePlayGame(void)
{
	int nGame = Picker_GetSelectedItem(hWndList);
	play_options playopts;

	memset(&playopts, 0, sizeof(playopts));
	MamePlayGameWithOptions(nGame, &playopts);
}

static void MamePlayRecordWave(void)
{
	int nGame = Picker_GetSelectedItem(hWndList);
	char filename[MAX_PATH];
	play_options playopts;

	memset(&playopts, 0, sizeof(playopts));
	*filename = 0;
	strcpy(filename, driver_list::driver(nGame).name);

	if (CommonFileDialog(GetSaveFileName, filename, FILETYPE_WAVE_FILES))
	{
		memset(&playopts, 0, sizeof(playopts));
		playopts.wavwrite = filename;
		MamePlayGameWithOptions(nGame, &playopts);
	}
}

static void MamePlayRecordMNG(void)
{
	int nGame = Picker_GetSelectedItem(hWndList);
	char filename[MAX_PATH];
	play_options playopts;

	memset(&playopts, 0, sizeof(playopts));
	*filename = 0;
	strcpy(filename, driver_list::driver(nGame).name);
	
	if (CommonFileDialog(GetSaveFileName, filename, FILETYPE_MNG_FILES))
	{
		char name[MAX_PATH];
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		char *fname = utf8_from_tstring(tempname);
		strcpy(name, fname);
		free(t_filename);
		free(fname);
		playopts.mngwrite = name;
		MamePlayGameWithOptions(nGame, &playopts);
	}
}

static void MamePlayRecordAVI(void)
{
	int nGame = Picker_GetSelectedItem(hWndList);
	char filename[MAX_PATH];
	play_options playopts;

	memset(&playopts, 0, sizeof(playopts));
	*filename = 0;
	strcpy(filename, driver_list::driver(nGame).name);
	
	if (CommonFileDialog(GetSaveFileName, filename, FILETYPE_AVI_FILES))
	{
		char name[MAX_PATH];
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		char *fname = utf8_from_tstring(tempname);
		strcpy(name, fname);
		free(t_filename);
		free(fname);
		playopts.aviwrite = name;
		MamePlayGameWithOptions(nGame, &playopts);
	}
}

static void MamePlayGameWithOptions(int nGame, const play_options *playopts)
{
	if (g_pJoyGUI != NULL)
		KillTimer(hMain, JOYGUI_TIMER);
	
	if (GetCycleScreenshot() > 0)
		KillTimer(hMain, SCREENSHOT_TIMER);

	in_emulation = TRUE;
	DWORD dwExitCode = RunMAME(nGame, playopts);
	
	if (dwExitCode == 0)
	{
		IncrementPlayCount(nGame);
		(void)ListView_RedrawItems(hWndList, GetSelectedPick(), GetSelectedPick());
	}
	else
		ShowWindow(hMain, SW_SHOW);

	in_emulation = FALSE;
	game_launched = TRUE;
	
	// re-sort if sorting on # of times played
	if (GetSortColumn() == COLUMN_PLAYED)
		Picker_Sort(hWndList);

	UpdateStatusBar();
	UpdateWindow(hMain);
	ShowWindow(hMain, SW_SHOW);
	SetActiveWindow(hMain);
	SetForegroundWindow(hMain);
	SetFocus(hWndList);

	if (g_pJoyGUI != NULL)
		SetTimer(hMain, JOYGUI_TIMER, JOYGUI_MS, NULL);

	if (GetCycleScreenshot() > 0)
		SetTimer(hMain, SCREENSHOT_TIMER, GetCycleScreenshot()*1000, NULL); //scale to seconds
}

/* Toggle ScreenShot ON/OFF */
static void ToggleScreenShot(void)
{
	BOOL showScreenShot = GetShowScreenShot();

	SetShowScreenShot((showScreenShot) ? FALSE : TRUE);
	UpdateScreenShot();

	/* Redraw list view */
	if (showScreenShot)
		InvalidateRect(hWndList, NULL, FALSE);
}

static void AdjustMetrics(void)
{
	TEXTMETRIC tm;
	AREA area;

	/* WM_SETTINGCHANGE also */
	int xtraX = GetSystemMetrics(SM_CXFIXEDFRAME); 	/* Dialog frame width */
	int xtraY = GetSystemMetrics(SM_CYFIXEDFRAME); 	/* Dialog frame height */
	xtraY += GetSystemMetrics(SM_CYMENUSIZE);		/* Menu height */
	xtraY += GetSystemMetrics(SM_CYCAPTION);		/* Caption Height */
	int maxX = GetSystemMetrics(SM_CXSCREEN); 		/* Screen Width */
	int maxY = GetSystemMetrics(SM_CYSCREEN); 		/* Screen Height */
	HDC hDC = GetDC(hMain);
	GetTextMetrics (hDC, &tm);
	/* Convert MIN Width/Height from Dialog Box Units to pixels. */
	MIN_WIDTH  = (int)((tm.tmAveCharWidth / 4.0) * DBU_MIN_WIDTH)  + xtraX;
	MIN_HEIGHT = (int)((tm.tmHeight / 8.0) * DBU_MIN_HEIGHT) + xtraY;
	ReleaseDC(hMain, hDC);

	HWND hWnd = GetWindow(hMain, GW_CHILD);
	
	while(hWnd)
	{
		TCHAR szClass[128];

		if (GetClassName(hWnd, szClass, ARRAY_LENGTH(szClass)))
		{
			if (!_tcscmp(szClass, WC_LISTVIEW))
			{
				(void)ListView_SetBkColor(hWndList, GetListBgColor());
				(void)ListView_SetTextColor(hWndList, GetListFontColor());
			}
			else if (!_tcscmp(szClass, WC_TREEVIEW))
			{
				(void)TreeView_SetBkColor(hTreeView, GetFolderBgColor());
				(void)TreeView_SetTextColor(hTreeView, GetTreeFontColor());
			}
		}
		
		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
	}

	GetWindowArea(&area);
	int offX = area.x + area.width;
	int offY = area.y + area.height;

	if (offX > maxX)
	{
		offX = maxX;
		area.x = (offX - area.width > 0) ? (offX - area.width) : 0;
	}

	if (offY > maxY)
	{
		offY = maxY;
		area.y = (offY - area.height > 0) ? (offY - area.height) : 0;
	}

	SetWindowArea(&area);
	SetWindowPos(hMain, HWND_TOP, area.x, area.y, area.width, area.height, 0);
}

int FindIconIndex(int nIconResource)
{
	for (int i = 0; g_iconData[i].icon_name; i++)
	{
		if (g_iconData[i].resource == nIconResource)
			return i;
	}
	
	return -1;
}

int FindIconIndexByName(const char *icon_name)
{
	for (int i = 0; g_iconData[i].icon_name; i++)
	{
		if (!strcmp(g_iconData[i].icon_name, icon_name))
			return i;
	}

	return -1;
}

static BOOL UseBrokenIcon(int type)
{
	if (type == 4 && !GetUseBrokenIcon())
		return FALSE;

	return TRUE;
}

static int GetIconForDriver(int nItem)
{
	int iconRoms = 0;

	if (DriverUsesRoms(nItem))
	{
		int audit_result = GetRomAuditResults(nItem);

		if (audit_result == -1)
			iconRoms = 2;
		else if (IsAuditResultYes(audit_result))
			iconRoms = 1;
		else
			iconRoms = 0;
	}
	else
		iconRoms = 1;

	// iconRoms is now either 0 (no roms), 1 (roms), or 2 (unknown)

	/* these are indices into icon_names, which maps into our image list
    * also must match IDI_WIN_NOROMS + iconRoms
    */

	// Show Red-X if the ROMs are present and flagged as NOT WORKING
	if (iconRoms == 1 && DriverIsBroken(nItem))
		iconRoms = FindIconIndex(IDI_WIN_REDX);

	// Show imperfect if the ROMs are present and flagged as imperfect
	if (iconRoms == 1 && DriverIsImperfect(nItem))
		iconRoms = FindIconIndex(IDI_WIN_IMPERFECT);

	// show clone icon if we have roms and game is working
	if (iconRoms == 1 && DriverIsClone(nItem))
		iconRoms = FindIconIndex(IDI_WIN_CLONE);

	// if we have the roms, then look for a custom per-game icon to override
	if (iconRoms == 1 || iconRoms == 3 || iconRoms == 5 || !UseBrokenIcon(iconRoms))
	{
		if (icon_index[nItem] == 0)
			AddDriverIcon(nItem,iconRoms);

		iconRoms = icon_index[nItem];
	}

	return iconRoms;
}

static BOOL HandleTreeContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	TVHITTESTINFO hti;
	POINT pt;

	if ((HWND)wParam != GetDlgItem(hWnd, IDC_TREE))
		return FALSE;

	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	
	if (pt.x < 0 && pt.y < 0)
		GetCursorPos(&pt);

	/* select the item that was right clicked or shift-F10'ed */
	hti.pt = pt;
	ScreenToClient(hTreeView,&hti.pt);
	(void)TreeView_HitTest(hTreeView,&hti);
	
	if ((hti.flags & TVHT_ONITEM) != 0)
		(void)TreeView_SelectItem(hTreeView,hti.hItem);

	HMENU hTreeMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_CONTEXT_TREE));
	InitTreeContextMenu(hTreeMenu);
	HMENU hMenu = GetSubMenu(hTreeMenu, 0);
	UpdateMenu(hMenu);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(hTreeMenu);
	return TRUE;
}

static void GamePicker_OnBodyContextMenu(POINT pt)
{
	HMENU hMenuLoad = LoadMenu(hInst, MAKEINTRESOURCE(IDR_CONTEXT_MENU));
	HMENU hMenu = GetSubMenu(hMenuLoad, 0);
	
	InitBodyContextMenu(hMenu);
	UpdateMenu(hMenu);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hMain, NULL);
	DestroyMenu(hMenuLoad);
}

static BOOL HandleScreenShotContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	MENUINFO mi;

	if ((HWND)wParam != GetDlgItem(hWnd, IDC_SSPICTURE) && (HWND)wParam != GetDlgItem(hWnd, IDC_SSFRAME))
		return FALSE;

	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	
	if (pt.x < 0 && pt.y < 0)
		GetCursorPos(&pt);

	HMENU hMenuLoad = LoadMenu(hInst, MAKEINTRESOURCE(IDR_CONTEXT_SCREENSHOT));
	HMENU hMenu = GetSubMenu(hMenuLoad, 0);
	HMENU hSubMenu = GetSubMenu(hMenu, 2);

	memset(&mi, 0, sizeof(MENUINFO));
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE;
	mi.dwStyle = MNS_CHECKORBMP;
	mi.hbrBack = GetSysColorBrush(COLOR_WINDOW);
	
	SetMenuInfo(hMenu, &mi);
	SetMenuInfo(hSubMenu, &mi);
	SetMenuItemBitmaps(hMenu, 2, MF_BYPOSITION, hTabs, hTabs);
	UpdateMenu(hMenu);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(hMenuLoad);
	return TRUE;
}

static void UpdateMenu(HMENU hMenu)
{
	MENUITEMINFO mItem;
	LPTREEFOLDER lpFolder = GetCurrentFolder();
	int i = 0;

	if (have_selection)
	{
		TCHAR buf[200];
		int nGame = Picker_GetSelectedItem(hWndList);
		
		TCHAR *t_description = tstring_from_utf8(ConvertAmpersandString(driver_list::driver(nGame).description));

		if( !t_description )
			return;

		_sntprintf(buf, ARRAY_LENGTH(buf), g_szPlayGameString, t_description);
		memset(&mItem, 0, sizeof(MENUITEMINFO));
		mItem.cbSize = sizeof(MENUITEMINFO);
		mItem.fMask = MIIM_TYPE;
		mItem.fType = MFT_STRING;
		mItem.dwTypeData = buf;
		mItem.cch = _tcslen(mItem.dwTypeData);

		SetMenuItemInfo(hMenu, ID_FILE_PLAY, FALSE, &mItem);
		EnableMenuItem(hMenu, ID_CONTEXT_SELECT_RANDOM, MF_ENABLED);
		free(t_description);
	}
	else
	{
		EnableMenuItem(hMenu, ID_FILE_PLAY, MF_GRAYED);
		EnableMenuItem(hMenu, ID_FILE_PLAY_RECORD, MF_GRAYED);
		EnableMenuItem(hMenu, ID_GAME_PROPERTIES, MF_GRAYED);
		EnableMenuItem(hMenu, ID_CONTEXT_SELECT_RANDOM, MF_GRAYED);
	}

	if (lpFolder->m_dwFlags & F_CUSTOM)
	{
	    EnableMenuItem(hMenu, ID_CONTEXT_REMOVE_CUSTOM,	MF_ENABLED);
		EnableMenuItem(hMenu, ID_CONTEXT_RENAME_CUSTOM,	MF_ENABLED);
	}
	else
	{
	    EnableMenuItem(hMenu, ID_CONTEXT_REMOVE_CUSTOM,	MF_GRAYED);
		EnableMenuItem(hMenu, ID_CONTEXT_RENAME_CUSTOM,	MF_GRAYED);
	}

	if (lpFolder->m_dwFlags & F_INIEDIT)
		EnableMenuItem(hMenu,ID_FOLDER_PROPERTIES,	MF_ENABLED);
	else
		EnableMenuItem(hMenu,ID_FOLDER_PROPERTIES,	MF_GRAYED);

	CheckMenuRadioItem(hMenu, ID_VIEW_TAB_SCREENSHOT, ID_VIEW_TAB_HISTORY, ID_VIEW_TAB_SCREENSHOT + TabView_GetCurrentTab(hTabCtrl), MF_BYCOMMAND);

	// set whether we're showing the tab control or not
	if (bShowTabCtrl)
		CheckMenuItem(hMenu, ID_VIEW_PAGETAB, MF_BYCOMMAND | MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_VIEW_PAGETAB, MF_BYCOMMAND | MF_UNCHECKED);


	for (i = 0; i < MAX_TAB_TYPES; i++)
	{
		// disable menu items for tabs we're not currently showing
		if (GetShowTab(i))
			EnableMenuItem(hMenu, ID_VIEW_TAB_SCREENSHOT + i, MF_BYCOMMAND | MF_ENABLED);
		else
			EnableMenuItem(hMenu, ID_VIEW_TAB_SCREENSHOT + i, MF_BYCOMMAND | MF_GRAYED);

		// check toggle menu items
		if (GetShowTab(i))
			CheckMenuItem(hMenu, ID_TOGGLE_TAB_SCREENSHOT + i, MF_BYCOMMAND | MF_CHECKED);
		else
			CheckMenuItem(hMenu, ID_TOGGLE_TAB_SCREENSHOT + i, MF_BYCOMMAND | MF_UNCHECKED);
	}

	for (i = 0; i < MAX_FOLDERS; i++)
	{
		if (GetShowFolder(i))
		{
			CheckMenuItem(hMenu, ID_CONTEXT_SHOW_FOLDER_START + i, MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(hMenu, (ID_SHOW_FOLDER_START1 + i) - 1, MF_BYCOMMAND | MF_CHECKED);
		}
		else
		{
			CheckMenuItem(hMenu, ID_CONTEXT_SHOW_FOLDER_START + i, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(hMenu, (ID_SHOW_FOLDER_START1 + i) - 1, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}

}

void InitMainMenu(HMENU hMainMenu)
{
	MENUINFO mi;
	MENUITEMINFO mif;
	extern const FOLDERDATA g_folderData[];
	HMENU hFile = GetSubMenu(hMainMenu, 0);
	HMENU hView = GetSubMenu(hMainMenu, 1);
	HMENU hOption = GetSubMenu(hMainMenu, 2);
	HMENU hTools = GetSubMenu(hMainMenu, 3);
	HMENU hAbout = GetSubMenu(hMainMenu, 4);
	HMENU hSubSort = GetSubMenu(hView, 11);
	HMENU hSubFold = GetSubMenu(hView, 13);
	HMENU hSubView = GetSubMenu(hView, 15);
	HMENU hSubFonts = GetSubMenu(hOption, 4);
	
	memset(&mi, 0, sizeof(MENUINFO));
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE;
	mi.dwStyle = MNS_CHECKORBMP;
	mi.hbrBack = GetSysColorBrush(COLOR_WINDOW);
	
	SetMenuInfo(hFile, &mi);
	SetMenuInfo(hView, &mi);
	SetMenuInfo(hOption, &mi);
	SetMenuInfo(hTools, &mi);
	SetMenuInfo(hAbout, &mi);
	SetMenuInfo(hSubSort, &mi);
	SetMenuInfo(hSubFold, &mi);
	SetMenuInfo(hSubView, &mi);
	SetMenuInfo(hSubFonts, &mi);
	
	memset(&mif, 0, sizeof(MENUITEMINFO));
	mif.cbSize = sizeof(MENUITEMINFO);

	for (int i = 0; g_folderData[i].m_lpTitle != NULL; i++)
	{
		TCHAR* t_title = tstring_from_utf8(g_folderData[i].m_lpTitle);
		
		if(!t_title)
			return;

		mif.fMask = MIIM_TYPE | MIIM_ID;
		mif.fType = MFT_STRING;
		mif.dwTypeData = t_title;
		mif.cch = _tcslen(mif.dwTypeData);
		mif.wID = ID_SHOW_FOLDER_START1 + i;
		
		SetMenuItemInfo(hMainMenu, ID_SHOW_FOLDER_START1 + i, FALSE, &mif);
		free(t_title);
	}
	
	SetMenuItemBitmaps(hMainMenu, ID_HELP_ABOUT, MF_BYCOMMAND, hAboutMenu, hAboutMenu);
	SetMenuItemBitmaps(hMainMenu, ID_OPTIONS_DIR, MF_BYCOMMAND, hDirectories, hDirectories);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_EXIT, MF_BYCOMMAND, hExit, hExit);
	SetMenuItemBitmaps(hMainMenu, ID_VIEW_FULLSCREEN, MF_BYCOMMAND, hFullscreen, hFullscreen);
	SetMenuItemBitmaps(hMainMenu, ID_OPTIONS_INTERFACE, MF_BYCOMMAND, hInterface, hInterface);
	SetMenuItemBitmaps(hMainMenu, ID_HELP_CONTENTS, MF_BYCOMMAND, hHelp, hHelp);
	SetMenuItemBitmaps(hMainMenu, ID_MAME_HOMEPAGE, MF_BYCOMMAND, hMameHome, hMameHome);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_PLAY, MF_BYCOMMAND, hPlay, hPlay);
	SetMenuItemBitmaps(hMainMenu, ID_VIDEO_SNAP, MF_BYCOMMAND, hVideo, hVideo);
	SetMenuItemBitmaps(hMainMenu, ID_PLAY_M1, MF_BYCOMMAND, hPlayM1, hPlayM1);
	SetMenuItemBitmaps(hMainMenu, ID_OPTIONS_DEFAULTS, MF_BYCOMMAND, hOptions, hOptions);
	SetMenuItemBitmaps(hMainMenu, ID_UPDATE_GAMELIST, MF_BYCOMMAND, hRefresh, hRefresh);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_GAMELIST, MF_BYCOMMAND, hSaveList, hSaveList);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_ROMSLIST, MF_BYCOMMAND, hSaveRoms, hSaveRoms);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_PLAY_BACK, MF_BYCOMMAND, hPlayback, hPlayback);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_AUDIT, MF_BYCOMMAND, hAuditMenu, hAuditMenu);
	SetMenuItemBitmaps(hMainMenu, ID_PLAY_VIDEO, MF_BYCOMMAND, hVideo, hVideo);
	SetMenuItemBitmaps(hOption, 4, MF_BYPOSITION, hFonts, hFonts);
	SetMenuItemBitmaps(hView, 13, MF_BYPOSITION, hFolders, hFolders);
	SetMenuItemBitmaps(hView, 11, MF_BYPOSITION, hSort, hSort);
	SetMenuItemBitmaps(hMainMenu, ID_MAME_FAQ, MF_BYCOMMAND, hFaq, hFaq);
	SetMenuItemBitmaps(hView, 15, MF_BYPOSITION, hTabs, hTabs);
	SetMenuItemBitmaps(hMainMenu, ID_HELP_TROUBLE, MF_BYCOMMAND, hTrouble, hTrouble);
	SetMenuItemBitmaps(hMainMenu, ID_HELP_WHATS_NEW, MF_BYCOMMAND, hRelease, hRelease);
	SetMenuItemBitmaps(hSubSort, ID_VIEW_BYGAME, MF_BYCOMMAND, hDescription, hDescription);
	SetMenuItemBitmaps(hSubSort, ID_VIEW_BYDIRECTORY, MF_BYCOMMAND, hRom, hRom);
	SetMenuItemBitmaps(hSubSort, ID_VIEW_BYSOURCE, MF_BYCOMMAND, hSource, hSource);
	SetMenuItemBitmaps(hSubSort, ID_VIEW_BYMANUFACTURER, MF_BYCOMMAND, hManufacturer, hManufacturer);
	SetMenuItemBitmaps(hSubSort, ID_VIEW_BYYEAR, MF_BYCOMMAND, hYear, hYear);
	SetMenuItemBitmaps(hSubSort, ID_VIEW_BYTIMESPLAYED, MF_BYCOMMAND, hCount, hCount);
	SetMenuItemBitmaps(hMainMenu, ID_PLAY_AUDIO, MF_BYCOMMAND, hPlaywav, hPlaywav);
	SetMenuItemBitmaps(hSubFonts, ID_OPTIONS_TREE_FONT, MF_BYCOMMAND, hFont1, hFont1);
	SetMenuItemBitmaps(hSubFonts, ID_OPTIONS_HISTORY_FONT, MF_BYCOMMAND, hFont2, hFont2);
	SetMenuItemBitmaps(hSubFonts, ID_OPTIONS_FONT, MF_BYCOMMAND, hFont1, hFont1);
	SetMenuItemBitmaps(hSubFonts, ID_OPTIONS_HISTORY_COLOR, MF_BYCOMMAND, hInfoback, hInfoback);
	SetMenuItemBitmaps(hSubFonts, ID_OPTIONS_LIST_COLOR, MF_BYCOMMAND, hListback, hListback);
	SetMenuItemBitmaps(hSubFonts, ID_OPTIONS_FOLDERS_COLOR, MF_BYCOMMAND, hTreeback, hTreeback);
	SetMenuItemBitmaps(hMainMenu, ID_CUSTOMIZE_FIELDS, MF_BYCOMMAND, hFields, hFields);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_PLAY_RECORD_AVI, MF_BYCOMMAND, hRecavi, hRecavi);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_PLAY_RECORD, MF_BYCOMMAND, hRecinput, hRecinput);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_PLAY_RECORD_WAVE, MF_BYCOMMAND, hRecwav, hRecwav);
	SetMenuItemBitmaps(hMainMenu, ID_PLAY_MNG, MF_BYCOMMAND, hPlaymng, hPlaymng);
	SetMenuItemBitmaps(hMainMenu, ID_CONTEXT_SELECT_RANDOM, MF_BYCOMMAND, hRandom, hRandom);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_PLAY_RECORD_MNG, MF_BYCOMMAND, hRecmng, hRecmng);
	SetMenuItemBitmaps(hMainMenu, ID_FILE_LOADSTATE, MF_BYCOMMAND, hSavestate, hSavestate);
	SetMenuItemBitmaps(hMainMenu, ID_CONTEXT_FILTERS, MF_BYCOMMAND, hFilters, hFilters);
	SetMenuItemBitmaps(hMainMenu, ID_OPTIONS_RESET_DEFAULTS, MF_BYCOMMAND, hReset, hReset);
}

void InitTreeContextMenu(HMENU hTreeMenu)
{
	MENUINFO mi;
	MENUITEMINFO mii;
	extern const FOLDERDATA g_folderData[];
	HMENU hMenuTree = GetSubMenu(hTreeMenu, 0);
	
	memset(&mi, 0, sizeof(MENUINFO));
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE;
	mi.dwStyle = MNS_CHECKORBMP;
	mi.hbrBack = GetSysColorBrush(COLOR_WINDOW);
	
	SetMenuInfo(hMenuTree, &mi);
	
	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.wID = -1;
	mii.fMask = MIIM_SUBMENU | MIIM_ID;

	if (GetMenuItemInfo(hMenuTree, 3, TRUE, &mii) == FALSE)
		return;

	if (mii.hSubMenu == NULL)
		return;

	SetMenuItemBitmaps(hMenuTree, ID_FOLDER_PROPERTIES, MF_BYCOMMAND, hProperties, hProperties);
	SetMenuItemBitmaps(hMenuTree, ID_FOLDER_AUDIT, MF_BYCOMMAND, hAuditMenu, hAuditMenu);
	SetMenuItemBitmaps(hMenuTree, 3, MF_BYPOSITION, hFolders, hFolders);
	SetMenuItemBitmaps(hMenuTree, ID_CONTEXT_FILTERS, MF_BYCOMMAND, hFilters, hFilters);
	SetMenuItemBitmaps(hMenuTree, ID_CONTEXT_RENAME_CUSTOM, MF_BYCOMMAND, hRename, hRename);
	hMenuTree = mii.hSubMenu;
	SetMenuInfo(hMenuTree, &mi);

	for (int i = 0; g_folderData[i].m_lpTitle != NULL; i++)
	{
		TCHAR* t_title = tstring_from_utf8(g_folderData[i].m_lpTitle);
		
		if(!t_title)
			return;

		mii.fMask = MIIM_TYPE | MIIM_ID;
		mii.fType = MFT_STRING;
		mii.dwTypeData = t_title;
		mii.cch = _tcslen(mii.dwTypeData);
		mii.wID = ID_CONTEXT_SHOW_FOLDER_START + g_folderData[i].m_nFolderId;

		// menu in resources has one empty item (needed for the submenu to setup properly)
		// so overwrite this one, append after
		if (i == 0)
			SetMenuItemInfo(hMenuTree, ID_CONTEXT_SHOW_FOLDER_START, FALSE, &mii);
		else
			InsertMenuItem(hMenuTree, i, FALSE, &mii);

		free(t_title);
	}
}

void InitBodyContextMenu(HMENU hBodyContextMenu)
{
	TCHAR tmp[30];
	MENUINFO mi;
	MENUITEMINFO mii;
	
	memset(&mi, 0, sizeof(MENUINFO));
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE;
	mi.dwStyle = MNS_CHECKORBMP;
	mi.hbrBack = GetSysColorBrush(COLOR_WINDOW);
	
	SetMenuInfo(hBodyContextMenu, &mi);
	
	memset(&mii, 0, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);

	if (GetMenuItemInfo(hBodyContextMenu, ID_FOLDER_SOURCEPROPERTIES, FALSE, &mii) == FALSE)
		return;
	
	LPTREEFOLDER lpFolder = GetFolderByName(FOLDER_SOURCE, GetDriverFilename(Picker_GetSelectedItem(hWndList)));
	_sntprintf(tmp, ARRAY_LENGTH(tmp), TEXT("Properties for %s"), lpFolder->m_lptTitle);
	
	mii.fMask = MIIM_TYPE | MIIM_ID;
	mii.fType = MFT_STRING;
	mii.dwTypeData = tmp;
	mii.cch = _tcslen(mii.dwTypeData);
	mii.wID = ID_FOLDER_SOURCEPROPERTIES;

	// menu in resources has one default item
	// so overwrite this one
	SetMenuItemInfo(hBodyContextMenu, ID_FOLDER_SOURCEPROPERTIES, FALSE, &mii);
	SetMenuItemBitmaps(hBodyContextMenu, ID_CONTEXT_ADD_CUSTOM, MF_BYCOMMAND, hCustom, hCustom);
	SetMenuItemBitmaps(hBodyContextMenu, ID_FILE_PLAY, MF_BYCOMMAND, hPlay, hPlay);
	SetMenuItemBitmaps(hBodyContextMenu, ID_VIDEO_SNAP, MF_BYCOMMAND, hVideo, hVideo);
	SetMenuItemBitmaps(hBodyContextMenu, ID_PLAY_M1, MF_BYCOMMAND, hPlayM1, hPlayM1);
	SetMenuItemBitmaps(hBodyContextMenu, ID_VIEW_ZIP, MF_BYCOMMAND, hZip, hZip);
	SetMenuItemBitmaps(hBodyContextMenu, ID_GAME_PROPERTIES, MF_BYCOMMAND, hProperties, hProperties);
	SetMenuItemBitmaps(hBodyContextMenu, ID_GAME_AUDIT, MF_BYCOMMAND, hAuditMenu, hAuditMenu);
	SetMenuItemBitmaps(hBodyContextMenu, ID_FOLDER_SOURCEPROPERTIES, MF_BYCOMMAND, hDriver, hDriver);
	SetMenuItemBitmaps(hBodyContextMenu, ID_CONTEXT_RESET_PLAYCOUNT, MF_BYCOMMAND, hCount, hCount);
	SetMenuItemBitmaps(hBodyContextMenu, ID_CONTEXT_RESET_PLAYTIME, MF_BYCOMMAND, hTime, hTime);
	SetMenuItemBitmaps(hBodyContextMenu, ID_FILE_PLAY_RECORD, MF_BYCOMMAND, hRecinput, hRecinput);
	SetMenuItemBitmaps(hBodyContextMenu, ID_CONTEXT_REMOVE_CUSTOM, MF_BYCOMMAND, hRemove, hRemove);
	SetMenuItemBitmaps(hBodyContextMenu, ID_FILE_LOADSTATE, MF_BYCOMMAND, hSavestate, hSavestate);
}

void ToggleShowFolder(int folder)
{
	int current_id = GetCurrentFolderID();

	SetWindowRedraw(hWndList, FALSE);
	SetShowFolder(folder, !GetShowFolder(folder));
	ResetTreeViewFolders();
	SelectTreeViewFolder(current_id);
	SetWindowRedraw(hWndList, TRUE);
}

static LRESULT CALLBACK PictureFrameWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
    {
		if (MouseHasBeenMoved())
			ShowCursor(TRUE);
		break;
    }
    case WM_NCHITTEST :
	{
		POINT pt;
		RECT  rect;
		HWND hHistory = GetDlgItem(hMain, IDC_HISTORY);

		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		GetWindowRect(hHistory, &rect);
		// check if they clicked on the picture area (leave 6 pixel no man's land
		// by the history window to reduce mistaken clicks)
		// no more no man's land, the Cursor changes when Edit control is left, should be enough feedback
		if (have_history && ( ( (TabView_GetCurrentTab(hTabCtrl) == TAB_HISTORY) ||
			(TabView_GetCurrentTab(hTabCtrl) == GetHistoryTab() && GetShowTab(TAB_HISTORY) == FALSE) ||
			(TAB_ALL == GetHistoryTab() && GetShowTab(TAB_HISTORY) == FALSE) ) && PtInRect( &rect, pt ) ) )
			return HTTRANSPARENT;
		else
			return HTCLIENT;
		
		break;
	}
	
	case WM_CONTEXTMENU:
		if ( HandleScreenShotContextMenu(hWnd, wParam, lParam))
			return FALSE;
		
		break;
	}
	
	return CallWindowProc(g_lpPictureFrameWndProc, hWnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK PictureWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC	hdc, hdc_temp;
		RECT rect;
		HBITMAP old_bitmap;
		int width = 0;
		int height = 0;
		RECT rect2;
		int nBordersize = GetScreenshotBorderSize();
		HBRUSH hBrush = CreateSolidBrush(GetScreenshotBorderColor());

		hdc = BeginPaint(hWnd, &ps);
		hdc_temp = CreateCompatibleDC(hdc);
		
		if (ScreenShotLoaded())
		{
			width = GetScreenShotWidth();
			height = GetScreenShotHeight();
			old_bitmap = (HBITMAP)SelectObject(hdc_temp,GetScreenShotHandle());
		}
		else
		{
			BITMAP bmp;

			GetObject(hMissing_bitmap, sizeof(BITMAP), &bmp);
			width = bmp.bmWidth;
			height = bmp.bmHeight;
			old_bitmap = (HBITMAP)SelectObject(hdc_temp,hMissing_bitmap);
		}

		GetClientRect(hWnd,&rect);
		rect2 = rect;
		//Configurable Borders around images
		rect.bottom -= nBordersize;
		if (rect.bottom < 0)
			rect.bottom = rect2.bottom;
		
		rect.right -= nBordersize;
		
		if (rect.right < 0)
			rect.right = rect2.right;
		
		rect.top += nBordersize;
		
		if (rect.top > rect.bottom)
			rect.top = rect2.top;
		
		rect.left += nBordersize;
		
		if (rect.left > rect.right)
			rect.left = rect2.left;
		
		HRGN region1 = CreateRectRgnIndirect(&rect);
		HRGN region2 = CreateRectRgnIndirect(&rect2);
		CombineRgn(region2, region2, region1, RGN_DIFF);
		HBRUSH holdBrush = (HBRUSH)SelectObject(hdc, hBrush);
		FillRgn(hdc,region2, hBrush);
		SelectObject(hdc, holdBrush);
		DeleteBrush(hBrush);
		SetStretchBltMode(hdc, STRETCH_HALFTONE);
		StretchBlt(hdc,nBordersize,nBordersize,rect.right-rect.left,rect.bottom-rect.top, hdc_temp, 0, 0, width, height, SRCCOPY);
		SelectObject(hdc_temp,old_bitmap);
		DeleteDC(hdc_temp);
		DeleteObject(region1);
		DeleteObject(region2);
		EndPaint(hWnd,&ps);
		return TRUE;
	}
	}
	
	return CallWindowProc(g_lpPictureWndProc, hWnd, uMsg, wParam, lParam);
}

static void RemoveCurrentGameCustomFolder(void)
{
	RemoveGameCustomFolder(Picker_GetSelectedItem(hWndList));
}

static void RemoveGameCustomFolder(int driver_index)
{
	TREEFOLDER **folders;
	int num_folders = 0;

	GetFolders(&folders, &num_folders);

	for (int i = 0; i < num_folders; i++)
	{
	    if (folders[i]->m_dwFlags & F_CUSTOM && folders[i]->m_nFolderId == GetCurrentFolderID())
		{
		    RemoveFromCustomFolder(folders[i], driver_index);

			if (driver_index == Picker_GetSelectedItem(hWndList))
			{
				/* if we just removed the current game,
				move the current selection so that when we rebuild the listview it
                leaves the cursor on next or previous one */
			   int current_pick_index = GetSelectedPick();
			   Picker_SetSelectedPick(hWndList, GetSelectedPick() + 1);
			   
			   if (current_pick_index == GetSelectedPick()) /* we must have deleted the last item */
				  Picker_SetSelectedPick(hWndList, GetSelectedPick() - 1);
			}

			UpdateListView();
			return;
		}
	}
	
	ErrorMessageBox("Error searching for custom folder");

}

static void BeginListViewDrag(NM_LISTVIEW *pnmv)
{
    LVITEM lvi;
	POINT pt;

	lvi.iItem = pnmv->iItem;
	lvi.mask = LVIF_PARAM;
	
	(void)ListView_GetItem(hWndList, &lvi);
	game_dragged = lvi.lParam;
	pt.x = 0;
	pt.y = 0;
	/* Tell the list view control to create an image to use
    for dragging. */
    himl_drag = ListView_CreateDragImage(hWndList, pnmv->iItem, &pt);
    /* Start the drag operation. */
    ImageList_BeginDrag(himl_drag, 0, 0, 0);
	pt = pnmv->ptAction;
	ClientToScreen(hWndList,&pt);
	ImageList_DragEnter(GetDesktopWindow(), pt.x, pt.y);
    /* Hide the mouse cursor, and direct mouse input to the
    parent window. */
    SetCapture(hMain);
	prev_drag_drop_target = NULL;
    g_listview_dragging = TRUE;
}

static void MouseMoveListViewDrag(POINTS p)
{
   	TV_HITTESTINFO tvht;
  	POINT pt;
	
   	pt.x = p.x;
  	pt.y = p.y;
  	ClientToScreen(hMain,&pt);
   	ImageList_DragMove(pt.x,pt.y);
  	MapWindowPoints(GetDesktopWindow(), hTreeView, &pt, 1);
   	tvht.pt = pt;
   	HTREEITEM htiTarget = TreeView_HitTest(hTreeView,&tvht);

   	if (htiTarget != prev_drag_drop_target)
   	{
	   	ImageList_DragShowNolock(FALSE);
		
	   	if (htiTarget != NULL)
		   	(void)TreeView_SelectDropTarget(hTreeView,htiTarget);
	   	else
		   	(void)TreeView_SelectDropTarget(hTreeView,NULL);
		
	   	ImageList_DragShowNolock(TRUE);
	   	prev_drag_drop_target = htiTarget;
   	}
}

static void ButtonUpListViewDrag(POINTS p)
{
    POINT pt;
	TV_HITTESTINFO tvht;
	TVITEM tvi;

	ReleaseCapture();
    ImageList_DragLeave(hWndList);
    ImageList_EndDrag();
	ImageList_Destroy(himl_drag);
	(void)TreeView_SelectDropTarget(hTreeView,NULL);
	g_listview_dragging = FALSE;
	/* see where the game was dragged */
	pt.x = p.x;
	pt.y = p.y;
	MapWindowPoints(hMain, hTreeView, &pt, 1);
	tvht.pt = pt;
	HTREEITEM htiTarget = TreeView_HitTest(hTreeView,&tvht);
	
	if (htiTarget == NULL)
	{
	   	LVHITTESTINFO lvhtti;
	   	RECT rcList;

	   	/* the user dragged a game onto something other than the treeview */
	   	/* try to remove if we're in a custom folder */
	   	/* see if it was dragged within the list view; if so, ignore */
	   	MapWindowPoints(hTreeView, hWndList, &pt, 1);
	   	lvhtti.pt = pt;
	   	GetWindowRect(hWndList, &rcList);
	   	ClientToScreen(hWndList, &pt);
		
	   	if( PtInRect(&rcList, pt) != 0 )
		   	return;

	   	LPTREEFOLDER folder = GetCurrentFolder();
		
	   	if (folder->m_dwFlags & F_CUSTOM)
	   	{
		   	/* dragged out of a custom folder, so let's remove it */
		   	RemoveCurrentGameCustomFolder();
	   	}
		
	   	return;
	}

	tvi.lParam = 0;
	tvi.mask = TVIF_PARAM | TVIF_HANDLE;
	tvi.hItem = htiTarget;

	if (TreeView_GetItem(hTreeView, &tvi))
	{
		LPTREEFOLDER folder = (LPTREEFOLDER)tvi.lParam;
		AddToCustomFolder(folder, game_dragged);
	}

}

static LPTREEFOLDER GetSelectedFolder(void)
{
	HTREEITEM htree = TreeView_GetSelection(hTreeView);
	TVITEM tvi;

	if(htree != NULL)
	{
		tvi.hItem = htree;
		tvi.mask = TVIF_PARAM;
		(void)TreeView_GetItem(hTreeView,&tvi);
		return (LPTREEFOLDER)tvi.lParam;
	}
	
	return NULL;
}

static HICON GetSelectedFolderIcon(void)
{
	HTREEITEM htree = TreeView_GetSelection(hTreeView);
	TVITEM tvi;

	if (htree != NULL)
	{
		tvi.hItem = htree;
		tvi.mask = TVIF_PARAM;
		(void)TreeView_GetItem(hTreeView, &tvi);

		//hSmall_icon = TreeView_GetImageList(hTreeView,(int)tvi.iImage);
		HIMAGELIST hSmall_icon = NULL;
		return ImageList_GetIcon(hSmall_icon, tvi.iImage, ILD_TRANSPARENT);
	}
	
	return NULL;
}

/* Updates all currently displayed Items in the List with the latest Data*/
void UpdateListView(void)
{
	ResetWhichGamesInFolders();
	ResetListView();
	(void)ListView_RedrawItems(hWndList, ListView_GetTopIndex(hWndList),
		ListView_GetTopIndex(hWndList) + ListView_GetCountPerPage(hWndList));
	SetFocus(hWndList);
}

static void CalculateBestScreenShotRect(HWND hWnd, RECT *pRect, BOOL restrict_height)
{
	RECT rect;
	/* for scaling */
	int destW = 0; 
	int destH = 0;
	int x = 0;
	int	y = 0;
	double scale = 0;
	BOOL bReduce = FALSE;

	GetClientRect(hWnd, &rect);

	// Scale the bitmap to the frame specified by the passed in hwnd
	if (ScreenShotLoaded())
	{
		x = GetScreenShotWidth();
		y = GetScreenShotHeight();
	}
	else
	{
		BITMAP bmp;
		
		GetObject(hMissing_bitmap,sizeof(BITMAP),&bmp);
		x = bmp.bmWidth;
		y = bmp.bmHeight;
	}
	
	int rWidth  = (rect.right  - rect.left);
	int rHeight = (rect.bottom - rect.top);

	/* Limit the screen shot to max height of 264 */
	if (restrict_height == TRUE && rHeight > 264)
	{
		rect.bottom = rect.top + 264;
		rHeight = 264;
	}

	/* If the bitmap does NOT fit in the screenshot area */
	if (x > rWidth - 10 || y > rHeight - 10)
	{
		rect.right	-= 10;
		rect.bottom -= 10;
		rWidth	-= 10;
		rHeight -= 10;
		bReduce = TRUE;
		
		/* Try to scale it properly */
		/*  assumes square pixels, doesn't consider aspect ratio */
		if (x > y)
			scale = (double)rWidth / x;
		else
			scale = (double)rHeight / y;

		destW = (int)(x * scale);
		destH = (int)(y * scale);

		/* If it's still too big, scale again */
		if (destW > rWidth || destH > rHeight)
		{
			if (destW > rWidth)
				scale = (double)rWidth	/ destW;
			else
				scale = (double)rHeight / destH;

			destW = (int)(destW * scale);
			destH = (int)(destH * scale);
		}
	}
	else
	{
		if (GetStretchScreenShotLarger())
		{
			rect.right	-= 10;
			rect.bottom -= 10;
			rWidth	-= 10;
			rHeight -= 10;
			bReduce = TRUE;
			
			// Try to scale it properly
			// assumes square pixels, doesn't consider aspect ratio
			if (x < y)
				scale = (double)rWidth / x;
			else
				scale = (double)rHeight / y;

			destW = (int)(x * scale);
			destH = (int)(y * scale);

			// If it's too big, scale again
			if (destW > rWidth || destH > rHeight)
			{
				if (destW > rWidth)
					scale = (double)rWidth	/ destW;
				else
					scale = (double)rHeight / destH;

				destW = (int)(destW * scale);
				destH = (int)(destH * scale);
			}
		}
		else
		{
			// Use the bitmaps size if it fits
			destW = x;
			destH = y;
		}

	}

	int destX = ((rWidth  - destW) / 2);
	int destY = ((rHeight - destH) / 2);

	if (bReduce)
	{
		destX += 5;
		destY += 5;
	}
	
	int nBorder = GetScreenshotBorderSize();
	
	if( destX > nBorder+1)
		pRect->left   = destX - nBorder;
	else
		pRect->left   = 2;
	
	if( destY > nBorder+1)
		pRect->top	  = destY - nBorder;
	else
		pRect->top	  = 2;
	
	if( rWidth >= destX + destW + nBorder)
		pRect->right  = destX + destW + nBorder;
	else
		pRect->right  = rWidth - pRect->left;
	
	if( rHeight >= destY + destH + nBorder)
		pRect->bottom = destY + destH + nBorder;
	else
		pRect->bottom = rHeight - pRect->top;
}

/*
  Switches to either fullscreen or normal mode, based on the
  current mode.

  POSSIBLE BUGS:
  Removing the menu might cause problems later if some
  function tries to poll info stored in the menu. Don't
  know if you've done that, but this was the only way I
  knew to remove the menu dynamically.
*/

static void SwitchFullScreenMode(void)
{
	LONG lMainStyle;

	if (GetRunFullScreen())
	{
		// Hide the window
		ShowWindow(hMain, SW_HIDE);
		// Restore the menu
		SetMenu(hMain, LoadMenu(hInst, MAKEINTRESOURCE(IDR_UI_MENU)));
		InitMainMenu(GetMenu(hMain));
		// Refresh the checkmarks
		CheckMenuItem(GetMenu(hMain), ID_VIEW_FOLDERS, GetShowFolderList() ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(GetMenu(hMain), ID_VIEW_TOOLBARS, GetShowToolBar() ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(GetMenu(hMain), ID_VIEW_STATUS, GetShowStatusBar() ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(GetMenu(hMain), ID_VIEW_PAGETAB, GetShowTabCtrl() ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(GetMenu(hMain), ID_ENABLE_INDENT, GetEnableIndent() ? MF_CHECKED : MF_UNCHECKED);
		// Add frame to dialog again
		lMainStyle = GetWindowLong(hMain, GWL_STYLE);
		lMainStyle = lMainStyle | WS_BORDER;
		SetWindowLong(hMain, GWL_STYLE, lMainStyle);
		
		// Restore the window
		if (GetWindowState() == SW_MAXIMIZE)
			ShowWindow(hMain, SW_MAXIMIZE);
		else
			ShowWindow(hMain, SW_SHOWNORMAL);

		SetRunFullScreen(FALSE);
	}
	else
	{
		// Hide the window
		ShowWindow(hMain, SW_HIDE);
		// Remove menu
		SetMenu(hMain, NULL);
		// Frameless dialog (fake fullscreen)
		lMainStyle = GetWindowLong(hMain, GWL_STYLE);
		lMainStyle = lMainStyle & (WS_BORDER ^ 0xffffffff);
		SetWindowLong(hMain, GWL_STYLE, lMainStyle);
		
		// Keep track if we're already maximized before fullscreen
		if (IsMaximized(hMain))
			SetWindowState(SW_MAXIMIZE);
		else
			SetWindowState(SW_SHOWNORMAL);
		
		// Maximize the window
		ShowWindow(hMain, SW_MAXIMIZE);
		SetRunFullScreen(TRUE);
	}
}

/*
  Checks to see if the mouse has been moved since this func
  was first called (which is at startup). The reason for
  storing the startup coordinates of the mouse is that when
  a window is created it generates WM_MOUSEOVER events, even
  though the user didn't actually move the mouse. So we need
  to know when the WM_MOUSEOVER event is user-triggered.

  POSSIBLE BUGS:
  Gets polled at every WM_MOUSEMOVE so it might cause lag,
  but there's probably another way to code this that's
  way better?

*/

BOOL MouseHasBeenMoved(void)
{
    static int mouse_x = -1;
    static int mouse_y = -1;
	POINT p;

	GetCursorPos(&p);

    if (mouse_x == -1) // First time
    {
		mouse_x = p.x;
		mouse_y = p.y;
    }
	
	return (p.x != mouse_x || p.y != mouse_y);       
}

static DWORD WINAPI StartupThread(LPVOID hDlg)
{
	while(!Win32UI_init())
	{
		// put here something?
	}

	ExitThread(1);
	return 0;
}

static BOOL CALLBACK StartupProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
		HBITMAP hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SPLASH), IMAGE_BITMAP, 0, 0, LR_SHARED);
		SendMessage(GetDlgItem(hDlg, IDC_SPLASH), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
		hBrush = GetSysColorBrush(COLOR_3DFACE);
        hProgress = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 136, 526, 18, hDlg, NULL, hInst, NULL);
		SetWindowTheme(hProgress, L" ", L" ");
		SendMessage(hProgress, PBM_SETBKCOLOR, 0, GetSysColor(COLOR_3DFACE));
		SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 120));
		SendMessage(hProgress, PBM_SETPOS, 0, 0);
		hStartupThread = CreateThread(NULL, 0, StartupThread, hDlg, 0, 0);
		break;
		}

	case WM_CTLCOLORDLG:
		return (LRESULT) hBrush;

	case WM_CTLCOLORSTATIC:
		hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHT));
		return (LRESULT) hBrush;
	}
	
	return 0;
}

static BOOL CommonListDialog(common_file_dialog_proc cfd, int filetype)
{
	BOOL success = FALSE;
	OPENFILENAME of;
	TCHAR szFile[MAX_PATH];
	TCHAR szCurDir[MAX_PATH];

	szFile[0] = 0;

	// Save current directory (avoids mame file creation further failure)
	if (GetCurrentDirectory(MAX_PATH, szCurDir) > MAX_PATH)
	{
		// Path too large
		szCurDir[0] = 0;
	}

	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = hMain;
	of.hInstance = NULL;

	if (filetype == FILETYPE_GAME_LIST)
		of.lpstrTitle  = TEXT("Enter a file name to save the game list");
	else
		of.lpstrTitle  = TEXT("Enter a file name to save the ROMs list");
	
	of.lpstrFilter = TEXT("Standard text file (*.txt)\0*.txt\0");
	of.lpstrCustomFilter = NULL;
	of.nMaxCustFilter = 0;
	of.nFilterIndex = 1;
	of.lpstrFile = szFile;
	of.nMaxFile = sizeof(szFile);
	of.lpstrFileTitle = NULL;
	of.nMaxFileTitle = 0;
	of.lpstrInitialDir = list_directory;
	of.nFileOffset = 0;
	of.nFileExtension = 0;
	of.lpstrDefExt = TEXT("txt");
	of.lCustData = 0;
	of.lpfnHook = &OFNHookProc;
	of.lpTemplateName = NULL;
	of.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLEHOOK;

	while(!success)
	{
		if (GetSaveFileName(&of))
		{
			if (GetFileAttributes(szFile) != -1)
			{
				if (win_message_box_utf8(hMain, "File already exists, overwrite ?", MAMEUINAME, MB_ICONQUESTION | MB_YESNO) != IDYES )
					continue;
				else
					success = TRUE;

				SetFileAttributes(szFile, FILE_ATTRIBUTE_NORMAL);
			}

			SaveGameListToFile(utf8_from_tstring(szFile), filetype);
			// Save current directory (avoids mame file creation further failure)
			GetCurrentDirectory(MAX_PATH, list_directory);
			// Restore current file path
			if (szCurDir[0] != 0)
				SetCurrentDirectory(szCurDir);

			success = TRUE;
		}
		else
			break;
	}

	if (success)
		return TRUE;
	else
		return FALSE;
}

static void SaveGameListToFile(char *szFile, int filetype)
{
    int nListCount = ListView_GetItemCount(hWndList);
    const char *CrLf = "\n\n";
    LPTREEFOLDER lpFolder = GetCurrentFolder();
    LVITEM lvi;

  	FILE *f = fopen(szFile, "w");

    if (f == NULL)
    {
        ErrorMessageBox("Error : unable to access file");
        return;
    }

    // Title
    fprintf(f, "%s %s.%s", MAMEUINAME, GetVersionString(), CrLf);
	
	if (filetype == FILETYPE_GAME_LIST)
		fprintf(f, "This is the current list of games.%s", CrLf);
	else
		fprintf(f, "This is the current list of ROMs.%s", CrLf);

    // Current folder
    fprintf(f, "Current folder : <");
	
    if (lpFolder->m_nParent != -1)
    {
        // Shows only 2 levels (last and previous)
        LPTREEFOLDER lpF = GetFolder(lpFolder->m_nParent);

        if (lpF->m_nParent == -1)
            	fprintf(f, "\\");
 
        fprintf(f, "%s", lpF->m_lpTitle);
        fprintf(f, "\\");
    }
    else
       	fprintf(f, "\\");
 
    fprintf(f, "%s>%s.%s", lpFolder->m_lpTitle, (lpFolder->m_dwFlags & F_CUSTOM) ? " (custom folder)" : "", CrLf);

    // Sorting
    if (GetSortColumn() > 0)

       	fprintf(f, "Sorted by <%s> descending order", utf8_from_tstring(column_names[GetSortColumn()]));
    else
       	fprintf(f, "Sorted by <%s> ascending order", utf8_from_tstring(column_names[-GetSortColumn()]));

    fprintf(f, ", %d game(s) found.%s", nListCount, CrLf);

    // Games
    for (int nIndex = 0; nIndex < nListCount; nIndex++)
    {
		lvi.iItem = nIndex;
        lvi.iSubItem = 0;
        lvi.mask = LVIF_PARAM;

        if (ListView_GetItem(hWndList, &lvi))
        {
			int nGameIndex  = lvi.lParam;

			if (filetype == FILETYPE_GAME_LIST)
				fprintf(f, "%s", driver_list::driver(nGameIndex).description);
			else
				fprintf(f, "%s", driver_list::driver(nGameIndex).name);

            fprintf(f, "\n");
        }
    }

    fclose(f);
	win_message_box_utf8(hMain, "File saved successfully.", MAMEUINAME, MB_ICONINFORMATION | MB_OK);
}

static HBITMAP CreateBitmapTransparent(HBITMAP hSource)
{
	BITMAP bm;
	
	HDC hSrc = CreateCompatibleDC(NULL);
	HDC hDst = CreateCompatibleDC(NULL);
	GetObject(hSource, sizeof(bm), &bm);
	SelectObject(hSrc, hSource);
	HBITMAP hNew = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);
	SelectObject(hDst, hNew);
    BitBlt(hDst, 0, 0, bm.bmWidth, bm.bmHeight, hSrc, 0, 0, SRCCOPY);
    COLORREF clrTP = RGB(239, 239, 239);
    COLORREF clrBK = GetSysColor(COLOR_MENU);
	
	for (int nRow = 0; nRow < bm.bmHeight; nRow++)
        for (int nCol = 0; nCol < bm.bmWidth; nCol++)
            if (GetPixel(hDst, nCol, nRow) == clrTP)
                SetPixel(hDst, nCol, nRow, clrBK);

    DeleteDC(hDst);
	DeleteDC(hSrc);
    return hNew;
}

/* End of source file */
