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

  Properties.c

    Properties Popup and Misc UI support routines.

    Created 8/29/98 by Mike Haaland (mhaaland@hypertech.com)

***************************************************************************/

/***************************************************************************

MSH - 20070809
--
Notes on properties and ini files, reset and reset to default.
----------------------------------------------------------------------------
Each ini contains a complete option set.

Priority order for option sets (Lowest to Highest):

built-in defaults
program     ini (executable root filename ini)
debug       ini (if running a debug build)
vector      ini (really is vector.ini!)
vertical    ini (really is vertical.ini!)
horizont    ini (really is horizont.ini!)
driver      ini (source code root filename in which this driver is found)
grandparent ini (grandparent, not sure these exist, but it is possible)
parent      ini (where parent is the name of the parent driver)
game        ini (where game is the driver name for this game)

To determine which option set to use, start at the top level (lowest
priority), and overlay all higher priority ini's until the desired level
is reached.

The 'default' option set is the next priority higher up the list from
the desired level. For the default (program.ini) level, it is also the
default.

When MAME is run, the desired level is game ini.

Expected Code behavior:
----------------------------------------------------------------------------
This approach requires 3 option sets, 'current', 'original' and 'default'.

'current': used to populate the property pages, and to initialize the
'original' set.

'original': used to evaluate if the 'Reset' button is enabled.
If 'current' matches 'original', the 'Reset' button is disabled,
otherwise it is enabled.

'default': used to evaluate if the 'Restore to Defaults' button is enabled.
If 'current' matches 'default', the 'Restore to Defaults' button is disabled,
otherwise it is enabled.

When editing any option set, the desired level is set to the one being
edited, the default set for that level, is the next lower priority set found.

Upon entering the properties dialog:
a) 'current' is initialized
b) 'original' is initialized by 'current'
c) 'default' is initialized
d) Populate Property pages with 'current'
e) 'Reset' and 'Restore to Defaults' buttons are evaluated.

After any change:
a) 'current' is updated
b) 'Reset' and 'Restore to Defaults' buttons are re-evaluated.

Reset:
a) 'current' is reinitialized to 'original'
b) Re-populate Property pages with 'current'
c) 'Reset' and 'Restore to Defaults' buttons are re-evaluated.

Restore to Defaults:
a) 'current' is reinitialized to 'default'
b) Re-populate Property pages with 'current'
b) 'Reset' and 'Restore to Defaults' buttons are re-evaluated.

Apply:
a) 'original' is reinitialized to 'current'
b) 'Reset' and 'Restore to defaults' are re-evaluated.
c) If they 'current' matches 'default', remove the ini from disk.
   Otherwise, write the ini to disk.

Cancel:
a) Exit the dialog.

OK:
a) If they 'current' matches 'default', remove the ini from disk.
   Otherwise, write the ini to disk.
b) Exit the dialog.


***************************************************************************/

#include "winui.h"
#include <shlwapi.h>

#ifndef TVS_EX_DOUBLEBUFFER
#define TVS_EX_DOUBLEBUFFER 0x0004
#endif

#ifndef TVM_SETEXTENDEDSTYLE
#define TVM_SETEXTENDEDSTYLE (TV_FIRST + 44)
#endif

/**************************************************************
 * Local function prototypes
 **************************************************************/

static void InitializeOptions(HWND hDlg);
static void OptOnHScroll(HWND hWnd, HWND hWndCtl, UINT code, int pos);
static void NumScreensSelectionChange(HWND hWnd);
static void RefreshSelectionChange(HWND hWnd, HWND hWndCtrl);
static void InitializeSampleRateUI(HWND hWnd);
static void InitializeSoundModeUI(HWND hWnd);
static void InitializeSkippingUI(HWND hWnd);
static void InitializeRotateUI(HWND hWnd);
static void UpdateSelectScreenUI(HWND hWnd);
static void InitializeSelectScreenUI(HWND hWnd);
static void InitializeVideoUI(HWND hWnd);
static void InitializeSnapViewUI(HWND hWnd);
static void InitializeSnapNameUI(HWND hWnd);
static void InitializeBIOSUI(HWND hWnd);
static void InitializeControllerMappingUI(HWND hWnd);
static void InitializeJoystickIDUI(HWND hWnd);
static void UpdateOptions(HWND hDlg, datamap *map, windows_options &opts);
static void UpdateProperties(HWND hDlg, datamap *map, windows_options &opts);
static void PropToOptions(HWND hWnd, windows_options &opts);
static void OptionsToProp(HWND hWnd, windows_options &opts);
static void SetPropEnabledControls(HWND hWnd);
static BOOL SelectEffect(HWND hWnd);
static BOOL ResetEffect(HWND hWnd);
static BOOL SelectMameShader(HWND hWnd, int slot);
static BOOL ResetMameShader(HWND hWnd, int slot);
static void UpdateMameShader(HWND hWnd, int slot, windows_options &opts);
static BOOL SelectScreenShader(HWND hWnd, int slot);
static BOOL ResetScreenShader(HWND hWnd, int slot);
static void UpdateScreenShader(HWND hWnd, int slot, windows_options &opts);
static BOOL SelectCheatFile(HWND hWnd);
static BOOL ResetCheatFile(HWND hWnd);
static BOOL ChangeJoystickMap(HWND hWnd);
static BOOL ResetJoystickMap(HWND hWnd);
static BOOL SelectDebugscript(HWND hWnd);
static BOOL ResetDebugscript(HWND hWnd);
static void BuildDataMap(void);
static void ResetDataMap(HWND hWnd);
//mamefx: for coloring of changed elements
static BOOL IsControlOptionValue(HWND hDlg, HWND hWnd_ctrl, windows_options &opts, windows_options &ref);
static void DisableVisualStyles(HWND hDlg);
static void MovePropertySheetChildWindows(HWND hWnd, int nDx, int nDy);
static HTREEITEM GetSheetPageTreeItem(int nPage);
static int GetSheetPageTreeCurSelText(LPWSTR lpszText, int iBufSize);

/**************************************************************
 * Local private variables
 **************************************************************/

/* No longer used by the core, but we need it to predefine configurable screens for all games. */
#ifndef MAX_SCREENS
/* maximum number of screens for one game */
#define MAX_SCREENS					4
#endif
#define SHEET_TREE_WIDTH 180

static windows_options pDefaultOpts;
static windows_options pOrigOpts;
static windows_options pCurrentOpts;
static datamap *properties_datamap;
static BOOL orig_uses_defaults;
static int g_nGame = 0;
static int g_nFolder = 0;
static int g_nFolderGame = 0;
static int g_nPropertyMode = 0;
static BOOL g_bUseDefaults = FALSE;
static BOOL g_bReset = FALSE;
static BOOL g_bAutoAspect[MAX_SCREENS] = {FALSE, FALSE, FALSE, FALSE};
static BOOL g_bAutoSnapSize = FALSE;
static HICON game_icon = NULL;
static HBRUSH hBrush = NULL;
static HBRUSH hBrushDlg = NULL;
static HDC hDC = NULL;
static int status_color = 0;
static int g_nFirstInitPropertySheet = 0;
static RECT rcTabCtrl;
static RECT rcTabCaption;
static RECT rcChild;
static int nCaptionHeight;
static HWND hSheetTreeCtrl = NULL;
static HINSTANCE hSheetInstance = 0;
static WNDPROC pfnOldSheetProc = NULL;
static BOOL bPageTreeSelChangedActive = FALSE;
//mamefx: for coloring of changed elements
static windows_options pOptsGlobal;
static windows_options pOptsHorizontal;
static windows_options pOptsVertical;
static windows_options pOptsVector;
static windows_options pOptsSource;
static char buf[1024];
static char buffer[1024];

static const PROPERTYSHEETINFO g_propSheets[] =
{
	{ FALSE,	NULL,			IDD_PROP_GAME,			GamePropertiesDialogProc },
	{ FALSE,	NULL,			IDD_PROP_AUDIT,			GameAuditDialogProc },
	{ TRUE,		NULL,			IDD_PROP_DISPLAY,		GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_ADVANCED,		GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_SCREEN,		GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_OPENGL,		GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_SHADER,		GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_VECTOR,		GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_SOUND,			GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_INPUT,			GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_CONTROLLER,	GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_MISC,			GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_SNAP,			GameOptionsDialogProc },
	{ TRUE,		NULL,			IDD_PROP_DEBUG,			GameOptionsDialogProc },
	{ FALSE }
};

static struct ComboBoxVideo
{
	const TCHAR*	m_pText;
	const char*		m_pData;
} g_ComboBoxVideo[] =
{
	{ TEXT("Auto"),         "auto"   },
	{ TEXT("GDI"),			"gdi"    },
	{ TEXT("DirectDraw"),   "ddraw"  },
	{ TEXT("BGFX"),         "bgfx"   },
	{ TEXT("OpenGL"),       "opengl" },
	{ TEXT("Direct3D"),     "d3d"    },
//	{ TEXT("None"),         "none"   },		crashes the emulator?
};
#define NUMVIDEO ARRAY_LENGTH(g_ComboBoxVideo)

static struct ComboBoxSound
{
	const TCHAR*	m_pText;
	const char*		m_pData;
} g_ComboBoxSound[] =
{
	{ TEXT("Auto"),         "auto"   },
	{ TEXT("DirectSound"),  "dsound" },
	{ TEXT("None"),         "none"   },
};
#define NUMSOUND ARRAY_LENGTH(g_ComboBoxSound)

static struct ComboBoxSampleRate
{
	const TCHAR*	m_pText;
	const int		m_pData;
} g_ComboBoxSampleRate[] =
{
	{ TEXT("11025"),    	11025 },
	{ TEXT("22050"),    	22050 },
	{ TEXT("44100"),    	44100 },
	{ TEXT("48000"),    	48000 },
};
#define NUMSAMPLERATE ARRAY_LENGTH(g_ComboBoxSampleRate)

static struct ComboBoxSelectScreen
{
	const TCHAR*	m_pText;
	const int		m_pData;
} g_ComboBoxSelectScreen[] =
{
	{ TEXT("Screen 0"),    	0 },
	{ TEXT("Screen 1"),    	1 },
	{ TEXT("Screen 2"),    	2 },
	{ TEXT("Screen 3"),    	3 },
};
#define NUMSELECTSCREEN ARRAY_LENGTH(g_ComboBoxSelectScreen)

static struct ComboBoxView
{
	const TCHAR*	m_pText;
	const char*		m_pData;
} g_ComboBoxView[] =
{
	{ TEXT("Auto"),		   	"auto"     },
	{ TEXT("Standard"),   	"standard" },
	{ TEXT("Pixel aspect"),	"pixel"    },
	{ TEXT("Cocktail"),    	"cocktail" },
};
#define NUMVIEW ARRAY_LENGTH(g_ComboBoxView)

static struct ComboBoxFrameSkip
{
	const TCHAR*	m_pText;
	const int		m_pData;
} g_ComboBoxFrameSkip[] = 
{
	{ TEXT("Draw every frame"),	0  },
	{ TEXT("Skip 1 frame"),		1  },
	{ TEXT("Skip 2 frames"),	2  },
	{ TEXT("Skip 3 frames"), 	3  },
	{ TEXT("Skip 4 frames"), 	4  },
	{ TEXT("Skip 5 frames"), 	5  },
	{ TEXT("Skip 6 frames"), 	6  },
	{ TEXT("Skip 7 frames"), 	7  },
	{ TEXT("Skip 8 frames"), 	8  },
	{ TEXT("Skip 9 frames"), 	9  },
	{ TEXT("Skip 10 frames"), 	10 },
};
#define NUMFRAMESKIP ARRAY_LENGTH(g_ComboBoxFrameSkip)

static struct ComboBoxDevices
{
	const TCHAR*	m_pText;
	const char* 	m_pData;
} g_ComboBoxDevice[] =
{
	{ TEXT("None"),        	"none"     },
	{ TEXT("Keyboard"),    	"keyboard" },
	{ TEXT("Mouse"),		"mouse"    },
	{ TEXT("Joystick"),   	"joystick" },
	{ TEXT("Lightgun"),   	"lightgun" },
};
#define NUMDEVICES ARRAY_LENGTH(g_ComboBoxDevice)

static struct ComboBoxSnapName
{
	const TCHAR*	m_pText;
	const char*		m_pData;
} g_ComboBoxSnapName[] =
{
	{ TEXT("Gamename"),		   					"%g" 	  },
	{ TEXT("Gamename + increment"),	   			"%g%i" 	  },
	{ TEXT("Gamename/gamename"),    			"%g/%g"   },
	{ TEXT("Gamename/gamename + increment"),	"%g/%g%i" },
	{ TEXT("Gamename/increment"),    			"%g/%i"   },
};
#define NUMSNAPNAME ARRAY_LENGTH(g_ComboBoxSnapName)

static struct ComboBoxSnapView
{
	const TCHAR*	m_pText;
	const char*		m_pData;
} g_ComboBoxSnapView[] =
{
	{ TEXT("Auto"),		   	"auto"     },
	{ TEXT("Internal"),	   	"internal" },
	{ TEXT("Standard"),    	"standard" },
	{ TEXT("Pixel aspect"),	"pixel"    },
	{ TEXT("Cocktail"),    	"cocktail" },
};
#define NUMSNAPVIEW ARRAY_LENGTH(g_ComboBoxSnapView)

static struct ComboBoxJoystickID
{
	const TCHAR*	m_pText;
	const int		m_pData;
} g_ComboBoxJoystickID[] = 
{
	{ TEXT("0"),             0 },
	{ TEXT("1"),             1 },
	{ TEXT("2"),             2 },
	{ TEXT("3"),             3 },
	{ TEXT("4"),             4 },
	{ TEXT("5"),             5 },
	{ TEXT("6"),             6 },
	{ TEXT("7"),             7 },
};
#define NUMJOYSTICKID ARRAY_LENGTH(g_ComboBoxJoystickID)

/***************************************************************
 * Public functions
 ***************************************************************/

// This function (and the code that use it) is a gross hack - but at least the vile
// and disgusting global variables are gone, making it less gross than what came before
static int GetSelectedScreen(HWND hWnd)
{
	int nSelectedScreen = 0;
	HWND hCtrl = GetDlgItem(hWnd, IDC_SCREENSELECT);
	
	if (hCtrl)
		nSelectedScreen = ComboBox_GetCurSel(hCtrl);
	
	if ((nSelectedScreen < 0) || (nSelectedScreen >= NUMSELECTSCREEN))
		nSelectedScreen = 0;
	
	return nSelectedScreen;

}

static PROPSHEETPAGE *CreatePropSheetPages(HINSTANCE hInst, int nDriver, UINT *pnMaxPropSheets)
{
	PROPSHEETPAGE *pspages;
	int maxPropSheets;
	int possiblePropSheets;
	int i;
	BOOL isGame = (nDriver >= 0);

	i = (isGame) ? 0 : 2;

	for (; g_propSheets[i].pfnDlgProc; i++)
		;

	possiblePropSheets = (isGame) ? i + 1 : i - 1;
	pspages = (PROPSHEETPAGE *)malloc(sizeof(PROPSHEETPAGE) * possiblePropSheets);

	if (!pspages)
		return NULL;

	memset(pspages, 0, sizeof(PROPSHEETPAGE) * possiblePropSheets);
	maxPropSheets = 0;
	i = ( isGame ) ? 0 : 2;

	for (; g_propSheets[i].pfnDlgProc; i++)
	{
		if (nDriver == -1)
		{
			if (g_propSheets[i].bOnDefaultPage)
			{
				pspages[maxPropSheets].dwSize = sizeof(PROPSHEETPAGE);
				pspages[maxPropSheets].dwFlags = 0;
				pspages[maxPropSheets].hInstance = hInst;
				pspages[maxPropSheets].pszTemplate = MAKEINTRESOURCE(g_propSheets[i].dwDlgID);
				pspages[maxPropSheets].pfnCallback = NULL;
				pspages[maxPropSheets].lParam  = 0;
				pspages[maxPropSheets].pfnDlgProc = g_propSheets[i].pfnDlgProc;
				maxPropSheets++;
			}
		}
		else if ((nDriver >= 0) || g_propSheets[i].bOnDefaultPage)
		{
			if (!g_propSheets[i].pfnFilterProc || g_propSheets[i].pfnFilterProc(nDriver))
			{
				pspages[maxPropSheets].dwSize = sizeof(PROPSHEETPAGE);
				pspages[maxPropSheets].dwFlags = 0;
				pspages[maxPropSheets].hInstance = hInst;
				pspages[maxPropSheets].pszTemplate = MAKEINTRESOURCE(g_propSheets[i].dwDlgID);
				pspages[maxPropSheets].pfnCallback = NULL;
				pspages[maxPropSheets].lParam = 0;
				pspages[maxPropSheets].pfnDlgProc = g_propSheets[i].pfnDlgProc;
				maxPropSheets++;
			}
		}
	}

	if (pnMaxPropSheets)
		*pnMaxPropSheets = maxPropSheets;

	return pspages;
}

void InitDefaultPropertyPage(HINSTANCE hInst, HWND hWnd)
{
	PROPSHEETHEADER pshead;
	PROPSHEETPAGE   *pspage;
	
	g_nGame = GLOBAL_OPTIONS;

	/* Get default options to populate property sheets */
	LoadOptions(pCurrentOpts, OPTIONS_GLOBAL, g_nGame);
	LoadOptions(pDefaultOpts, OPTIONS_GLOBAL, g_nGame);
	g_bUseDefaults = FALSE;
	/* Stash the result for comparing later */
	pOrigOpts = pCurrentOpts;
	orig_uses_defaults = FALSE;
	g_bReset = FALSE;
	g_nPropertyMode = OPTIONS_GLOBAL;
	BuildDataMap();
	memset(&pshead, 0, sizeof(PROPSHEETHEADER));
	pspage = CreatePropSheetPages(hInst, -1, &pshead.nPages);

	if (!pspage)
		return;

	/* Fill in the property sheet header */
	pshead.hwndParent = hWnd;
	pshead.dwSize = sizeof(PROPSHEETHEADER);
	pshead.dwFlags = PSH_PROPSHEETPAGE | PSH_USEICONID | PSH_DEFAULT | PSH_NOCONTEXTHELP;
	pshead.hInstance = hInst;
	pshead.pszCaption = TEXT("Default games options");
	pshead.nStartPage = 0;
	pshead.pszIcon = MAKEINTRESOURCE(IDI_MAMEUI_ICON);
	pshead.ppsp = pspage;

	g_nFirstInitPropertySheet = 1;
	hSheetInstance = hInst;

	/* Create the Property sheet and display it */
	if (PropertySheet(&pshead) == -1)
	{
		DWORD dwError = GetLastError();
		ErrorMessageBox("PropertySheet creation error %d %X", (int)dwError, (int)dwError);
	}

	free(pspage);
}

/* Initilize the property pages for anything but the Default option set */
void InitPropertyPage(HINSTANCE hInst, HWND hWnd, HICON hIcon, OPTIONS_TYPE opt_type, int folder_id, int game_num)
{
	InitPropertyPageToPage(hInst, hWnd, hIcon, opt_type, folder_id, game_num, PROPERTIES_PAGE);
}

void InitPropertyPageToPage(HINSTANCE hInst, HWND hWnd, HICON hIcon, OPTIONS_TYPE opt_type, int folder_id, int game_num, int start_page )
{
	PROPSHEETHEADER pshead;
	PROPSHEETPAGE *pspage;
	TCHAR *t_description = 0;
	OPTIONS_TYPE default_type = opt_type;
	
	// Load the current options, this will pickup the highest priority option set.
	LoadOptions(pCurrentOpts, opt_type, game_num);

	// Load the default options, pickup the next lower options set than the current level.
	if (opt_type > OPTIONS_GLOBAL)
	{
		default_type = (OPTIONS_TYPE)(default_type - 1);
		
		if (OPTIONS_VERTICAL == opt_type) 
			//since VERTICAL and HORIZONTAL are equally ranked
			//we need to subtract 2 from vertical to also get to correct default
			default_type = (OPTIONS_TYPE)(default_type - 1);
	}
	
	LoadOptions(pDefaultOpts, default_type, game_num);
	//mamefx: for coloring of changed elements
	LoadOptions(pOptsGlobal, OPTIONS_GLOBAL, game_num);
	LoadOptions(pOptsHorizontal, OPTIONS_HORIZONTAL, game_num);
	LoadOptions(pOptsVertical, OPTIONS_VERTICAL, game_num);
	LoadOptions(pOptsVector, OPTIONS_VECTOR, game_num);
	LoadOptions(pOptsSource, OPTIONS_SOURCE, game_num);
	// Copy current_options to original options
	pOrigOpts = pCurrentOpts;
	// Copy icon to use for the property pages
	game_icon = CopyIcon(hIcon);
	// These MUST be valid, they are used as indicies
	g_nGame = game_num;
	g_nFolder = folder_id;
	// Keep track of OPTIONS_TYPE that was passed in.
	g_nPropertyMode = opt_type;
	// Evaluate if the current set uses the Default set
	g_bUseDefaults = (pCurrentOpts == pDefaultOpts);
	g_bReset = FALSE;
	BuildDataMap();
	memset(&pshead, 0, sizeof(PROPSHEETHEADER));
	// Set the game to audit to this game
	InitGameAudit(game_num);
	
	// Create the property sheets
	if( OPTIONS_GAME == opt_type )
		pspage = CreatePropSheetPages(hInst, game_num, &pshead.nPages);
	else
		pspage = CreatePropSheetPages(hInst, -1, &pshead.nPages);

	if (!pspage)
		return;

	// Get the description use as the dialog caption.
	switch(opt_type)
	{
		case OPTIONS_GAME:
			t_description = tstring_from_utf8(driver_list::driver(g_nGame).description);
			break;
		case OPTIONS_VECTOR:
		case OPTIONS_VERTICAL:
		case OPTIONS_HORIZONTAL:
			t_description = tstring_from_utf8(GetFolderNameByID(g_nFolder));
			break;
		case OPTIONS_SOURCE:
			t_description = tstring_from_utf8(GetDriverFilename(g_nGame));
			break;
		case OPTIONS_GLOBAL:
			t_description = tstring_from_utf8("Default games options");
			break;
		default:
			free(pspage);
			return;
	}
	
	// If we have no descrption, return.
	if(!t_description)
	{
		free(pspage);
		return;
	}
	
	/* Fill in the property sheet header */
	pshead.pszCaption = t_description;
	pshead.hwndParent = hWnd;
	pshead.dwSize = sizeof(PROPSHEETHEADER);
	pshead.dwFlags = PSH_PROPSHEETPAGE | PSH_PROPTITLE | PSH_USEICONID | PSH_DEFAULT | PSH_NOCONTEXTHELP;
	pshead.hInstance = hInst;
	pshead.nStartPage = start_page;
	pshead.pszIcon = MAKEINTRESOURCE(IDI_MAMEUI_ICON);
	pshead.ppsp = pspage;

	g_nFirstInitPropertySheet = 1;
	hSheetInstance = hInst;

	/* Create the Property sheet and display it */
	if (PropertySheet(&pshead) == -1)
	{
		DWORD dwError = GetLastError();
		ErrorMessageBox("PropertySheet creation error %d %X", (int)dwError, (int)dwError);
	}

	free(t_description);
	free(pspage);
}

/*********************************************************************
 * Local Functions
 *********************************************************************/

/* Build CPU info string */
static char *GameInfoCPU(int nIndex)
{
	machine_config config(driver_list::driver(nIndex), MameUIGlobal());
	memset(&buf, 0, sizeof(buf));
	execute_interface_iterator iter(config.root_device());
	device_execute_interface *cpu = iter.first();
	char temp[300];

	while (cpu)
	{
		if (cpu->device().clock() >= 1000000)
			snprintf(temp, ARRAY_LENGTH(temp), "%s %d.%06d MHz\r\n", cpu->device().name(), cpu->device().clock() / 1000000, cpu->device().clock() % 1000000);
		else
			snprintf(temp, ARRAY_LENGTH(temp), "%s %d.%03d kHz\r\n", cpu->device().name(), cpu->device().clock() / 1000, cpu->device().clock() % 1000);

		strcat(buf, temp);
		cpu = iter.next();
    }

	return buf;
}

/* Build Sound system info string */
static char *GameInfoSound(int nIndex)
{
	machine_config config(driver_list::driver(nIndex), MameUIGlobal());
	memset(&buf, 0, sizeof(buf));
	sound_interface_iterator iter(config.root_device());
	const device_sound_interface *sound = iter.first();

	while(sound)
	{
		int clock,count;
		device_type sound_type_;
		char tmpname[1024];
		char temp[300];
		snprintf(tmpname, ARRAY_LENGTH(tmpname), "%s", sound->device().name());
		sound_type_ = sound->device().type();
		clock = sound->device().clock();
		count = 1;
		sound = iter.next();
		
		/* Matching chips at the same clock are aggregated - this doesn't work */
		while (sound && sound->device().type() == sound_type_ && sound->device().clock() == clock)
		{
			count++;
			sound = iter.next();
		}

		if (count > 1)
		{
			snprintf(temp, ARRAY_LENGTH(temp), "%dx ", count);
			strcat(buf, temp);
		}
		
		strcat(buf, tmpname);

		if (clock)
		{
			if (clock >= 1000000)
				snprintf(temp, ARRAY_LENGTH(temp), " %d.%06d MHz", clock / 1000000, clock % 1000000);
			else
				snprintf(temp, ARRAY_LENGTH(temp), " %d.%03d kHz", clock / 1000, clock % 1000);
			
			strcat(buf, temp);
		}
		
		strcat(buf, "\r\n");
	}
	
	return buf;
}

/* Build Display info string */
static char *GameInfoScreen(int nIndex)
{
	machine_config config(driver_list::driver(nIndex), MameUIGlobal());
	memset(&buf, 0, sizeof(buf));
	char tmpbuf[256];

	if (DriverIsVector(nIndex))
	{
		if (DriverIsVertical(nIndex))
			strcpy(buf, "Vector (V)");
		else
			strcpy(buf, "Vector (H)");
	}
	else
	{
		screen_device_iterator iter(config.root_device());
		const screen_device *screen = iter.first();

		if (screen == NULL)
			strcpy(buf, "Screenless");
		else
		{
			for (; screen != NULL; screen = iter.next())
			{
				const rectangle &visarea = screen->visible_area();

				if (DriverIsVertical(nIndex))
					snprintf(tmpbuf, ARRAY_LENGTH(tmpbuf), "%d x %d (V) %f Hz\r\n", visarea.height(), visarea.width(), ATTOSECONDS_TO_HZ(screen->refresh_attoseconds()));
				else
					snprintf(tmpbuf, ARRAY_LENGTH(tmpbuf), "%d x %d (H) %f Hz\r\n", visarea.width(), visarea.height(), ATTOSECONDS_TO_HZ(screen->refresh_attoseconds()));

				strcat(buf, tmpbuf);
			}
		}
	}

	return buf;
}

/* Build game status string */
static char *GameInfoStatus(int driver_index)
{
	memset(&buffer, 0, sizeof(buffer));

	//Just show the emulation flags
	if (DriverIsBroken(driver_index))
	{
		strcpy(buffer, "Not working");
		status_color = 1;
		return buffer;
	}
	
	if (DriverIsImperfect(driver_index))	
	{
		strcpy(buffer, "Working with problems");
		status_color = 2;
		
		if (driver_list::driver(driver_index).flags & GAME_UNEMULATED_PROTECTION)
		{
			strcat(buffer, "\r\n");
			strcat(buffer, "Game protection isn't fully emulated");
		}
		
		if (driver_list::driver(driver_index).flags & GAME_WRONG_COLORS)
		{
			strcat(buffer, "\r\n");
			strcat(buffer, "Colors are completely wrong");
		}
		
		if (driver_list::driver(driver_index).flags & GAME_IMPERFECT_COLORS)
		{
			strcat(buffer, "\r\n");
			strcat(buffer, "Colors aren't 100% accurate");
		}
		
		if (driver_list::driver(driver_index).flags & GAME_IMPERFECT_GRAPHICS)
		{
			strcat(buffer, "\r\n");
			strcat(buffer, "Video emulation isn't 100% accurate");
		}
		
		if (driver_list::driver(driver_index).flags & GAME_NO_SOUND)
		{
			strcat(buffer, "\r\n");
			strcat(buffer, "Game lacks sound");
		}
		
		if (driver_list::driver(driver_index).flags & GAME_IMPERFECT_SOUND)
		{
			strcat(buffer, "\r\n");
			strcat(buffer, "Sound emulation isn't 100% accurate");
		}
		
		return buffer;
	}
	else
	{
		strcpy(buffer, "Working");
		status_color = 0;
		return buffer;
	}
}

/* Build game manufacturer string */
static char *GameInfoManufactured(int nIndex)
{
	memset(&buffer, 0, sizeof(buffer));
	snprintf(buffer, ARRAY_LENGTH(buffer), "%s %s", driver_list::driver(nIndex).manufacturer, driver_list::driver(nIndex).year);
	return buffer;
}

/* Build Game title string */
char *GameInfoTitle(OPTIONS_TYPE opt_type, int nIndex)
{
	memset(&buf, 0, sizeof(buf));

	if (OPTIONS_GLOBAL == opt_type)
		strcpy(buf, "Global games options\r\nDefault options used by all games");
	else if (OPTIONS_VECTOR == opt_type)
		strcpy(buf, "Global vector options\r\nDefault options used by all vector games");	
	else if (OPTIONS_VERTICAL == opt_type)
		strcpy(buf, "Global vertical options\r\nDefault options used by all vertical games");
	else if (OPTIONS_HORIZONTAL == opt_type)
		strcpy(buf, "Global horizontal options\r\nDefault options used by all horizontal games");
	else if (OPTIONS_SOURCE == opt_type)
		strcpy(buf, "Global driver options\r\nDefault options used by all games in the driver");
	else
		snprintf(buf, ARRAY_LENGTH(buf), "%s - \"%s\"", driver_list::driver(nIndex).description, driver_list::driver(nIndex).name);

	return buf;
}

/* Build game clone infromation string */
static char *GameInfoCloneOf(int nIndex)
{
	int nParentIndex= -1;
	memset(&buf, 0, sizeof(buf));

	if (DriverIsClone(nIndex))
	{
		nParentIndex = GetParentIndex(&driver_list::driver(nIndex));
		snprintf(buf, ARRAY_LENGTH(buf), "%s - \"%s\"", driver_list::driver(nParentIndex).description, driver_list::driver(nParentIndex).name);
	}

	return buf;
}

static char *GameInfoSaveState(int driver_index)
{
	memset(&buf, 0, sizeof(buf));

	if (DriverSupportsSaveState(driver_index))
		strcpy(buf, "Supported");
	else
		strcpy(buf, "Unsupported");

	return buf;
}

static const char *GameInfoSource(UINT nIndex)
{
	return GetDriverFilename(nIndex);
}

/* Display Tree Sheet */
static void UpdateSheetCaption(HWND hWnd)
{
	PAINTSTRUCT ps;
	HRGN hRgn;
	RECT rect, rc;
	WCHAR szText[256];
	int i;
	HFONT hFontCaption, hOldFont;

	memcpy(&rect, &rcTabCaption, sizeof(RECT));
	BeginPaint (hWnd, &ps);
	hDC = ps.hdc;
	hRgn = CreateRectRgn(rect.left, rect.top, rect.right - 2, rect.bottom);
	SelectClipRgn(hDC, hRgn);
	hBrush = CreateSolidBrush(RGB(127, 127, 127));
	FillRect(hDC, &rect, hBrush);
	DeleteObject(hBrush);
	i = GetSheetPageTreeCurSelText(szText, sizeof(szText));

	if (i > 0)
	{
		hFontCaption = CreateFont(-16, 0,		// height, width
							0, 					// angle of escapement
							0,					// base-line orientation angle
							700,				// font weight
							0, 0, 0, 			// italic, underline, strikeout
							0,					// character set identifier
							3,					// output precision
							2,					// clipping precision
							2,					// output quality
							34,					// pitch and family
							TEXT("Arial"));		// typeface name

		hOldFont = (HFONT)SelectObject(hDC, hFontCaption);
		SetTextColor(hDC, RGB(255, 255, 255));
		SetBkMode(hDC, TRANSPARENT);
		memcpy(&rc, &rect, sizeof(RECT));
		rc.left += 4;
		DrawText(hDC, (LPCTSTR)szText, lstrlen((LPTSTR)szText), &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		SelectObject(hDC, hOldFont);
		DeleteObject(hFontCaption);
	}

	SelectClipRgn(hDC, NULL);
	DeleteObject(hRgn);
	rect.left = SHEET_TREE_WIDTH + 15;
	rect.top = 8;
	rect.right = rcTabCaption.right - 1;
	rect.bottom = rcTabCtrl.bottom + 4;
	hRgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
	SelectClipRgn(hDC, hRgn);
	hBrush = CreateSolidBrush(RGB(127, 127, 127));
	FrameRect(hDC, &rect, hBrush);
	DeleteObject(hBrush);
	SelectClipRgn(hDC, NULL);
	DeleteObject(hRgn);
	EndPaint (hWnd, &ps);
	return;
}

static LRESULT CALLBACK NewSheetWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandled = FALSE;
	int nPage;
	TVITEM item;

	switch (Msg)
	{
	case WM_PAINT:
		UpdateSheetCaption(hWnd);
		bHandled = TRUE;
		break;

	case WM_NOTIFY:
		switch (((NMHDR *)lParam)->code)
		{
		case TVN_SELCHANGING:
			if ((bPageTreeSelChangedActive == FALSE) && (g_nFirstInitPropertySheet == 0))
			{
				NMTREEVIEW* pTvn = (NMTREEVIEW*)lParam;
				bPageTreeSelChangedActive = TRUE;
				
				item.hItem = pTvn->itemNew.hItem;
				item.mask = TVIF_PARAM;
				
				(void)TreeView_GetItem(hSheetTreeCtrl, &item);
				nPage = (int)item.lParam;
				
				if (nPage >= 0)
					PropSheet_SetCurSel(hWnd, 0, nPage);

				bPageTreeSelChangedActive = FALSE;
				bHandled = TRUE;
			}
			break;
			
		case TVN_SELCHANGED:
			InvalidateRect(hWnd, &rcTabCaption, FALSE);
			bHandled = TRUE;
			break;
		}
		
		break;

	case WM_DESTROY:
		if (hSheetTreeCtrl != NULL)
		{
			DestroyWindow(hSheetTreeCtrl);
			hSheetTreeCtrl = NULL;
		}

		if (pfnOldSheetProc)
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pfnOldSheetProc);

		DeleteObject(hBrushDlg);
		break;
	}

	if ((bHandled == FALSE) && pfnOldSheetProc)
		return CallWindowProc(pfnOldSheetProc, hWnd, Msg, wParam, lParam);

	return 0;
}

static void AdjustChildWindows(HWND hWnd)
{
	TCHAR szClass[128];
	DWORD dwStyle;

	GetClassName(hWnd, szClass, sizeof(szClass));
	
	if (!_tcscmp(szClass, WC_BUTTON))
	{
		dwStyle = GetWindowLong(hWnd, GWL_STYLE);
		
		if (((dwStyle & BS_GROUPBOX) == BS_GROUPBOX) && (dwStyle & WS_TABSTOP))
			SetWindowLong(hWnd, GWL_STYLE, (dwStyle & ~WS_TABSTOP));
	}
}

static void AdjustPropertySheetChildWindows(HWND hWnd)
{
	HWND hChild = GetWindow(hWnd, GW_CHILD);
	
	while (hChild)
	{
		hChild = GetNextWindow(hChild, GW_HWNDNEXT);
	}
}

static void MovePropertySheetChildWindows(HWND hWnd, int nDx, int nDy)
{
	HWND hChild = GetWindow(hWnd, GW_CHILD);
	char szText[256];

	while (hChild)
	{
		GetWindowText(hChild, (WCHAR*)szText, sizeof(szText));
		GetWindowRect(hChild, &rcChild);
		OffsetRect(&rcChild, nDx, nDy);
		ScreenToClient(hWnd, (LPPOINT)&rcChild);
		ScreenToClient(hWnd, ((LPPOINT)&rcChild)+1);
		AdjustChildWindows(hChild);
		MoveWindow(hChild, rcChild.left, rcChild.top, rcChild.right - rcChild.left, rcChild.bottom - rcChild.top, TRUE);
		hChild = GetNextWindow(hChild, GW_HWNDNEXT);
	}
}

static HTREEITEM GetSheetPageTreeItem(int nPage)
{
	HTREEITEM hItem;
	TVITEM item;
	int nTreePage;

	if (hSheetTreeCtrl == NULL)
		return NULL;

	hItem = TreeView_GetRoot(hSheetTreeCtrl);
	
	while (hItem)
	{
		item.hItem = hItem;
		item.mask = TVIF_PARAM;
		(void)TreeView_GetItem(hSheetTreeCtrl, &item);
		nTreePage = (int)item.lParam;

		if (nTreePage == nPage)
			return hItem;

		hItem = TreeView_GetNextSibling(hSheetTreeCtrl, hItem);
	}

	return NULL;
}

static int GetSheetPageTreeCurSelText(LPWSTR lpszText, int iBufSize)
{
	HTREEITEM hItem;
	TVITEM item;

	lpszText[0] = 0;

	if (hSheetTreeCtrl == NULL)
		return -1;

	hItem = TreeView_GetSelection(hSheetTreeCtrl);

	if (hItem == NULL)
		return -1;

	item.hItem      = hItem;
	item.mask  	    = TVIF_TEXT;
	item.pszText    = (LPTSTR)lpszText;
	item.cchTextMax = iBufSize;

	(void)TreeView_GetItem(hSheetTreeCtrl, &item);
	return wcslen(lpszText);
}

void ModifyPropertySheetForTreeSheet(HWND hPageDlg)
{
	HWND hWnd, hTabWnd;
	DWORD tabStyle;
	int nPage, nPageCount, i, start;
	RECT rectSheet, rectTree;
	HTREEITEM hItem;
	LONG_PTR prevProc;
	TCITEM item;
	HWND hTempTab;
	HFONT hTreeSheetFont = NULL;
	HIMAGELIST hTreeList = NULL;
	HICON hIconList = NULL;
	
	if (g_nFirstInitPropertySheet == 0)
	{
		AdjustPropertySheetChildWindows(hPageDlg);
		return;
	}

	hWnd = GetParent(hPageDlg);
	
	if (!hWnd)
		return;

	prevProc = GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	pfnOldSheetProc = (WNDPROC)prevProc;
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)NewSheetWndProc);
	hTabWnd = PropSheet_GetTabControl(hWnd);

	if (!hTabWnd)
		return;

	tabStyle = (GetWindowLong(hTabWnd, GWL_STYLE) & ~TCS_MULTILINE);
	SetWindowLong(hTabWnd, GWL_STYLE, tabStyle | TCS_SINGLELINE);
	ShowWindow(hTabWnd, SW_HIDE);
	EnableWindow(hTabWnd, FALSE);
	GetWindowRect(hTabWnd, &rcTabCtrl);
	ScreenToClient(hTabWnd, (LPPOINT)&rcTabCtrl);
	ScreenToClient(hTabWnd, ((LPPOINT)&rcTabCtrl) + 1);
	GetWindowRect(hWnd, &rectSheet);
	rectSheet.right += SHEET_TREE_WIDTH + 5;
	SetWindowPos(hWnd, HWND_TOP, 0, 0, rectSheet.right - rectSheet.left, rectSheet.bottom - rectSheet.top, SWP_NOZORDER | SWP_NOMOVE);
	CenterWindow(hWnd);
	MovePropertySheetChildWindows(hWnd, SHEET_TREE_WIDTH + 6, 0);

	if (hSheetTreeCtrl != NULL)
	{
		DestroyWindow(hSheetTreeCtrl);
		hSheetTreeCtrl = NULL;
	}

	memset(&rectTree, 0, sizeof(RECT));
	hTempTab = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_CLIPSIBLINGS,
		rectTree.left, rectTree.top, rectTree.right - rectTree.left, rectTree.bottom - rectTree.top,
		hWnd, (HMENU)0x1234, hSheetInstance, NULL);

	item.mask = TCIF_TEXT;
	item.iImage = 0;
	item.lParam = 0;
	item.pszText = (LPTSTR)"";
	
	(void)TabCtrl_InsertItem(hTempTab, 0, &item);
	(void)TabCtrl_GetItemRect(hTempTab, 0, &rcTabCaption);
	nCaptionHeight = (rcTabCaption.bottom - rcTabCaption.top);
	rcTabCaption.left = rcTabCtrl.left + SHEET_TREE_WIDTH + 16;
	rcTabCaption.top = 8;
	rcTabCaption.right = rcTabCaption.left + (rcTabCtrl.right - rcTabCtrl.left - 6);
	rcTabCaption.bottom = rcTabCaption.top + nCaptionHeight;
	DestroyWindow(hTempTab);
	rectTree.left = rcTabCtrl.left + 8;
	rectTree.top = rcTabCtrl.top  + 8;
	rectTree.right = rcTabCtrl.left + SHEET_TREE_WIDTH + 2;
	rectTree.bottom = rcTabCtrl.bottom + 4;
	hSheetTreeCtrl = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY, WC_TREEVIEW, NULL,
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT | TVS_TRACKSELECT, 
		rectTree.left, rectTree.top, rectTree.right - rectTree.left, rectTree.bottom - rectTree.top,
		hWnd, (HMENU)0x7EEE, hSheetInstance, NULL);

	if (IsWindowsSevenOrHigher())
		SendMessage(hSheetTreeCtrl, TVM_SETEXTENDEDSTYLE, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);

	SetWindowTheme(hSheetTreeCtrl, L"Explorer", NULL);
	(void)TreeView_SetItemHeight(hSheetTreeCtrl, 34);
	
	if (hSheetTreeCtrl == NULL)
	{
		DWORD dwError = GetLastError();
		ErrorMessageBox("PropertySheet TreeCtrl creation error %d %X", (int)dwError, (int)dwError);
	}

	hTreeSheetFont = CreateFont(-11, 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, TEXT("Verdana"));
	SetWindowFont(hSheetTreeCtrl, hTreeSheetFont, TRUE);
	(void)TreeView_DeleteAllItems(hSheetTreeCtrl);
	nPageCount = TabCtrl_GetItemCount(hTabWnd);
 	hTreeList = ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, nPageCount, 0);
	start = 14 - nPageCount;
	
	for (i = 0; i < nPageCount; i++)
	{
		hIconList = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_GENERALSHEET + i + start));
		ImageList_AddIcon(hTreeList, hIconList);
	}
	
	(void)TreeView_SetImageList(hSheetTreeCtrl, hTreeList, TVSIL_NORMAL);
	
	for (nPage = 0; nPage < nPageCount; nPage++)
	{
		WCHAR szText[256];
		TCITEM ti;
		TVINSERTSTRUCT tvis;
		LPTVITEM lpTvItem;

		// Get title and image of the page
		memset(&ti, 0, sizeof(TCITEM));
		ti.mask = TCIF_TEXT | TCIF_IMAGE;
		ti.cchTextMax = sizeof(szText);
		ti.pszText = (LPTSTR)szText;

		(void)TabCtrl_GetItem(hTabWnd, nPage, &ti);
		lpTvItem = &tvis.item;

		// Create an item in the tree for the page
		tvis.hParent = TVI_ROOT;
		tvis.hInsertAfter = TVI_LAST;
		lpTvItem->mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		lpTvItem->pszText = (LPTSTR)szText;
		lpTvItem->iImage = nPage;
		lpTvItem->iSelectedImage = nPage;
		lpTvItem->state = 0;
		lpTvItem->stateMask = 0;
		lpTvItem->lParam = (LPARAM)NULL;

		// insert Item
		hItem = TreeView_InsertItem(hSheetTreeCtrl, &tvis);

		if (hItem)
		{
			TVITEM item;

			item.hItem = hItem;
			item.mask = TVIF_PARAM;
			item.pszText = NULL;
			item.iImage = 0;
			item.iSelectedImage = 0;
			item.state = 0;
			item.stateMask = 0;
			item.lParam = nPage;

			(void)TreeView_SetItem(hSheetTreeCtrl, &item);
		}
	}

	nPage = TabCtrl_GetCurSel(hTabWnd);
	
	if (nPage != -1)
	{
		hItem = GetSheetPageTreeItem(nPage);
		
		if (hItem)
			(void)TreeView_SelectItem(hSheetTreeCtrl, hItem);
	}

	g_nFirstInitPropertySheet = 0;
}

/* Handle the information property page */
INT_PTR CALLBACK GamePropertiesDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		{
		ModifyPropertySheetForTreeSheet(hDlg);

		if (game_icon)
			SendDlgItemMessage(hDlg, IDC_GAME_ICON, STM_SETICON, (WPARAM) game_icon, 0);

		hBrushDlg = CreateSolidBrush(RGB(224, 223, 227));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_TITLE), GameInfoTitle((OPTIONS_TYPE)g_nPropertyMode, g_nGame));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_MANUFACTURED), GameInfoManufactured(g_nGame));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_STATUS), GameInfoStatus(g_nGame));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_CPU), GameInfoCPU(g_nGame));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_SOUND), GameInfoSound(g_nGame));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_SCREEN), GameInfoScreen(g_nGame));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_CLONEOF), GameInfoCloneOf(g_nGame));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_SOURCE), GameInfoSource(g_nGame));
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_SAVESTATE), GameInfoSaveState(g_nGame));
		
		if (DriverIsClone(g_nGame))
			ShowWindow(GetDlgItem(hDlg, IDC_PROP_CLONEOF_TEXT), SW_SHOW);
		else
			ShowWindow(GetDlgItem(hDlg, IDC_PROP_CLONEOF_TEXT), SW_HIDE);

		ShowWindow(hDlg, SW_SHOW);
		return 1;
		}

	case WM_CTLCOLORDLG:
		return (LRESULT) hBrushDlg;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		
		if ((HWND)lParam == GetDlgItem(hDlg, IDC_PROP_STATUS))
		{
			if (status_color == 0)
				SetTextColor(hDC, RGB(34, 177, 76));
			else if (status_color == 1)
				SetTextColor(hDC, RGB(237, 28, 36));
			else
				SetTextColor(hDC, RGB(198, 188, 0));
		}
		
		if (((HWND)lParam == GetDlgItem(hDlg, IDC_PROP_MANUFACTURED)) ||
			((HWND)lParam == GetDlgItem(hDlg, IDC_PROP_CPU)) ||
			((HWND)lParam == GetDlgItem(hDlg, IDC_PROP_SOUND)) ||
			((HWND)lParam == GetDlgItem(hDlg, IDC_PROP_SCREEN)) ||
			((HWND)lParam == GetDlgItem(hDlg, IDC_PROP_CLONEOF)) ||
			((HWND)lParam == GetDlgItem(hDlg, IDC_PROP_SOURCE)) ||
			((HWND)lParam == GetDlgItem(hDlg, IDC_PROP_SAVESTATE)))
				SetTextColor(hDC, RGB(63, 72, 204));

		return (LRESULT) hBrushDlg;
	}

    return 0;
}

/* Handle all options property pages */
INT_PTR CALLBACK GameOptionsDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		//mamefx: for coloring of changed elements
		DisableVisualStyles(hDlg);
		ModifyPropertySheetForTreeSheet(hDlg);
		hBrushDlg = CreateSolidBrush(RGB(224, 223, 227));
		/* Fill in the Game info at the top of the sheet */
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_PROP_TITLE), GameInfoTitle((OPTIONS_TYPE)g_nPropertyMode, g_nGame));
		InitializeOptions(hDlg);
		UpdateProperties(hDlg, properties_datamap, pCurrentOpts);
		g_bUseDefaults = (pCurrentOpts == pDefaultOpts);
		g_bReset = (pCurrentOpts == pOrigOpts) ? FALSE : TRUE;

		if (g_nGame == GLOBAL_OPTIONS)
			ShowWindow(GetDlgItem(hDlg, IDC_USE_DEFAULT), SW_HIDE);
		else
			EnableWindow(GetDlgItem(hDlg, IDC_USE_DEFAULT), (g_bUseDefaults) ? FALSE : TRUE);
		
		EnableWindow(GetDlgItem(hDlg, IDC_PROP_RESET), g_bReset);
		ShowWindow(hDlg, SW_SHOW);
		return 1;
		
	case WM_HSCROLL:
		/* slider changed */
		HANDLE_WM_HSCROLL(hDlg, wParam, lParam, OptOnHScroll);
		EnableWindow(GetDlgItem(hDlg, IDC_USE_DEFAULT), TRUE);
		PropSheet_Changed(GetParent(hDlg), hDlg);
		// make sure everything's copied over, to determine what's changed
		UpdateOptions(hDlg, properties_datamap, pCurrentOpts);
		// redraw it, it might be a new color now
		InvalidateRect((HWND)lParam,NULL,TRUE);
		break;

	case WM_COMMAND:
	{
		/* Below, 'changed' is used to signify the 'Apply'
         * button should be enabled.
         */
		WORD wID = GET_WM_COMMAND_ID(wParam, lParam);
		HWND hWndCtrl = GET_WM_COMMAND_HWND(wParam, lParam);
		WORD wNotifyCode = GET_WM_COMMAND_CMD(wParam, lParam);
		BOOL changed = FALSE;
		int nCurSelection = 0;
		TCHAR szClass[256];

		switch (wID)
		{
		case IDC_REFRESH:
			if (wNotifyCode == LBN_SELCHANGE)
			{
				RefreshSelectionChange(hDlg, hWndCtrl);
				changed = TRUE;
			}
			break;

		case IDC_ASPECT:
			nCurSelection = Button_GetCheck(GetDlgItem(hDlg, IDC_ASPECT));
			
			if( g_bAutoAspect[GetSelectedScreen(hDlg)] != nCurSelection )
			{
				changed = TRUE;
				g_bAutoAspect[GetSelectedScreen(hDlg)] = nCurSelection;
			}
			
			break;

		case IDC_SNAPSIZE:
			nCurSelection = Button_GetCheck(GetDlgItem(hDlg, IDC_SNAPSIZE));
			
			if( g_bAutoSnapSize != nCurSelection )
			{
				changed = TRUE;
				g_bAutoSnapSize = nCurSelection;
			}
			
			break;

		case IDC_SELECT_EFFECT:
			changed = SelectEffect(hDlg);
			break;

		case IDC_RESET_EFFECT:
			changed = ResetEffect(hDlg);
			break;

		case IDC_SELECT_SHADER0:
		case IDC_SELECT_SHADER1:
		case IDC_SELECT_SHADER2:
		case IDC_SELECT_SHADER3:
		case IDC_SELECT_SHADER4:
			changed = SelectMameShader(hDlg, (wID - IDC_SELECT_SHADER0));
			break;

		case IDC_RESET_SHADER0:
		case IDC_RESET_SHADER1:
		case IDC_RESET_SHADER2:
		case IDC_RESET_SHADER3:
		case IDC_RESET_SHADER4:
			changed = ResetMameShader(hDlg, (wID - IDC_RESET_SHADER0));
			break;

		case IDC_SELECT_SCR_SHADER0:
		case IDC_SELECT_SCR_SHADER1:
		case IDC_SELECT_SCR_SHADER2:
		case IDC_SELECT_SCR_SHADER3:
		case IDC_SELECT_SCR_SHADER4:
			changed = SelectScreenShader(hDlg, (wID - IDC_SELECT_SCR_SHADER0));
			break;

		case IDC_RESET_SCR_SHADER0:
		case IDC_RESET_SCR_SHADER1:
		case IDC_RESET_SCR_SHADER2:
		case IDC_RESET_SCR_SHADER3:
		case IDC_RESET_SCR_SHADER4:
			changed = ResetScreenShader(hDlg, (wID - IDC_RESET_SCR_SHADER0));
			break;

		case IDC_SELECT_CHEATFILE:
			changed = SelectCheatFile(hDlg);
			break;

		case IDC_RESET_CHEATFILE:
			changed = ResetCheatFile(hDlg);
			break;

		case IDC_JOYSTICKMAP:
			changed = ChangeJoystickMap(hDlg);
			break;

		case IDC_RESET_JOYSTICKMAP:
			changed = ResetJoystickMap(hDlg);
			break;

		case IDC_SELECT_DEBUGSCRIPT:
			changed = SelectDebugscript(hDlg);
			break;

		case IDC_RESET_DEBUGSCRIPT:
			changed = ResetDebugscript(hDlg);
			break;

		case IDC_PROP_RESET:
			if (wNotifyCode != BN_CLICKED)
				break;

			pCurrentOpts = pOrigOpts;
			UpdateProperties(hDlg, properties_datamap, pCurrentOpts);
			g_bUseDefaults = (pCurrentOpts == pDefaultOpts);
			g_bReset = FALSE;
			PropSheet_UnChanged(GetParent(hDlg), hDlg);
			EnableWindow(GetDlgItem(hDlg, IDC_USE_DEFAULT), (g_bUseDefaults) ? FALSE : TRUE);
			break;

		case IDC_USE_DEFAULT:
			pCurrentOpts = pDefaultOpts;
			// repopulate the controls with the new data
			UpdateProperties(hDlg, properties_datamap, pCurrentOpts);
			g_bUseDefaults = (pCurrentOpts == pDefaultOpts);
			// This evaluates properly
			g_bReset = (pCurrentOpts == pOrigOpts) ? FALSE : TRUE;
			// Enable/Dispable the Reset to Defaults button
			EnableWindow(GetDlgItem(hDlg, IDC_USE_DEFAULT), (g_bUseDefaults) ? FALSE : TRUE);

			// Tell the dialog to enable/disable the apply button.
			if (g_nGame != GLOBAL_OPTIONS)
			{
				if (g_bReset)
					PropSheet_Changed(GetParent(hDlg), hDlg);
				else
					PropSheet_UnChanged(GetParent(hDlg), hDlg);
			}
			
			break;

		// MSH 20070813 - Update all related controls
		case IDC_SCREENSELECT:
		case IDC_SCREEN:
			// NPW 3-Apr-2007:  Ugh I'm only perpetuating the vile hacks in this code
			if ((wNotifyCode == CBN_SELCHANGE) || (wNotifyCode == CBN_SELENDOK))
			{
				datamap_read_control(properties_datamap, hDlg, pCurrentOpts, wID);
				datamap_populate_control(properties_datamap, hDlg, pCurrentOpts, IDC_SIZES);
				//MSH 20070814 - Hate to do this, but its either this, or update each individual
				// control on the SCREEN tab.
				UpdateProperties(hDlg, properties_datamap, pCurrentOpts);
				changed = TRUE;
			}
			
			break;

		default:
			// use default behavior; try to get the result out of the datamap if
			// appropriate
			GetClassName(hWndCtrl, szClass, ARRAY_LENGTH(szClass));

			if (!_tcscmp(szClass, WC_COMBOBOX))
			{
				// combo box
				if ((wNotifyCode == CBN_SELCHANGE) || (wNotifyCode == CBN_SELENDOK))
					changed = datamap_read_control(properties_datamap, hDlg, pCurrentOpts, wID);
			}
			else if (!_tcscmp(szClass, WC_BUTTON) && (GetWindowLong(hWndCtrl, GWL_STYLE) & BS_CHECKBOX))
				// check box
				changed = datamap_read_control(properties_datamap, hDlg, pCurrentOpts, wID);
				
			break;
		}

		if (changed == TRUE)
		{
			// make sure everything's copied over, to determine what's changed
			UpdateOptions(hDlg, properties_datamap, pCurrentOpts);
			// enable the apply button
			PropSheet_Changed(GetParent(hDlg), hDlg);
			g_bUseDefaults = (pCurrentOpts == pDefaultOpts);
			g_bReset = (pCurrentOpts == pOrigOpts) ? FALSE : TRUE;
			EnableWindow(GetDlgItem(hDlg, IDC_USE_DEFAULT), (g_bUseDefaults) ? FALSE : TRUE);
		}
	}

	break;

	case WM_NOTIFY:
	{
		switch (((NMHDR*)lParam)->code)
		{
		//We'll need to use a CheckState Table
		//Because this one gets called for all kinds of other things too, and not only if a check is set
		case PSN_SETACTIVE:
			/* Initialize the controls. */
			UpdateProperties(hDlg, properties_datamap, pCurrentOpts);
			g_bUseDefaults = (pCurrentOpts == pDefaultOpts);
			g_bReset = (pCurrentOpts == pOrigOpts) ? FALSE : TRUE;
			// Sync RESET TO DEFAULTS buttons.
			EnableWindow(GetDlgItem(hDlg, IDC_USE_DEFAULT), (g_bUseDefaults) ? FALSE : TRUE);
			break;

		case PSN_APPLY:
			// Read the datamap
			UpdateOptions(hDlg, properties_datamap, pCurrentOpts);
			pOrigOpts = pCurrentOpts;
			// Repopulate the controls?  WTF?  We just read them, they should be fine.
			UpdateProperties(hDlg, properties_datamap, pCurrentOpts);
			// Determine button states.
			g_bUseDefaults = (pCurrentOpts == pDefaultOpts);
			g_bReset = FALSE;
			orig_uses_defaults = g_bUseDefaults;
			// Sync RESET and RESET TO DEFAULTS buttons.
			EnableWindow(GetDlgItem(hDlg, IDC_USE_DEFAULT), (g_bUseDefaults) ? FALSE : TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_PROP_RESET), g_bReset);
			// Save or remove the current options
			SaveOptions((OPTIONS_TYPE)g_nPropertyMode, pCurrentOpts, g_nGame);
			// Disable apply button
			PropSheet_UnChanged(GetParent(hDlg), hDlg);
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, PSNRET_NOERROR);
			return TRUE;

		case PSN_KILLACTIVE:
			/* Save Changes to the options here. */
			UpdateOptions(hDlg, properties_datamap, pCurrentOpts);
			// Determine button states.
			g_bUseDefaults = (pCurrentOpts == pDefaultOpts);
			ResetDataMap(hDlg);
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, FALSE);
			return TRUE;

		case PSN_RESET:
			// Reset to the original values. Disregard changes
			pCurrentOpts = pOrigOpts;
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, FALSE);
			break;
		}
	}
	
	break;

	//mamefx: for coloring of changed elements
	case WM_CTLCOLORDLG:
		return (LRESULT) hBrushDlg;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLOREDIT:
	{
		HDC hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		
		//Set the Coloring of the elements
		if (GetWindowLongPtr((HWND)lParam, GWL_ID) < 0)
			return (LRESULT) hBrushDlg;

		if (g_nPropertyMode == OPTIONS_GLOBAL)
			SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		else if (IsControlOptionValue(hDlg,(HWND)lParam, pCurrentOpts, pOptsGlobal))
			SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		else if (IsControlOptionValue(hDlg,(HWND)lParam, pCurrentOpts, pOptsHorizontal) && !DriverIsVertical(g_nGame))
			SetTextColor(hDC, RGB(163, 73, 164));	// purple
		else if (IsControlOptionValue(hDlg,(HWND)lParam, pCurrentOpts, pOptsVertical) && DriverIsVertical(g_nGame))
			SetTextColor(hDC, RGB(63, 72, 204));	// blue
		else if (IsControlOptionValue(hDlg,(HWND)lParam, pCurrentOpts, pOptsVector) && DriverIsVector(g_nGame))
			SetTextColor(hDC, RGB(255, 127, 39));	// orange
		else if (IsControlOptionValue(hDlg,(HWND)lParam, pCurrentOpts, pOptsSource))
			SetTextColor(hDC, RGB(237, 28, 36));	// red
		else if (IsControlOptionValue(hDlg,(HWND)lParam, pCurrentOpts, pDefaultOpts))
			SetTextColor(hDC, RGB(34, 177, 76));	// green
		else
		{
			switch (g_nPropertyMode)
			{
				case OPTIONS_GAME:
					SetTextColor(hDC, RGB(34, 177, 76));
					break;
				case OPTIONS_SOURCE:
					SetTextColor(hDC, RGB(237, 28, 36));
					break;
				case OPTIONS_VECTOR:
					SetTextColor(hDC, RGB(255, 127, 39));
					break;
				case OPTIONS_HORIZONTAL:
					SetTextColor(hDC, RGB(163, 73, 164));
					break;
				case OPTIONS_VERTICAL:
					SetTextColor(hDC, RGB(63, 72, 204));
					break;
				case OPTIONS_GLOBAL:
				default:
					SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
					break;
			}
		}

		if (Msg == WM_CTLCOLORLISTBOX || Msg == WM_CTLCOLOREDIT)
			return (LRESULT) GetStockObject(WHITE_BRUSH);
		else
			return (LRESULT) hBrushDlg;
	}
	}
	
	EnableWindow(GetDlgItem(hDlg, IDC_PROP_RESET), g_bReset);
	return 0;
}

/* Read controls that are not handled in the DataMap */
static void PropToOptions(HWND hWnd, windows_options &opts)
{
	HWND hCtrl;
	HWND hCtrl2;
	HWND hCtrl3;
	std::string error_string;
	char aspect_option[32];
	int n = 0;
	int d = 0;
	int width = 0;
	int height = 0;
	TCHAR buffer[200];
	char buffer2[200];
	
	/* aspect ratio */
	hCtrl  = GetDlgItem(hWnd, IDC_ASPECTRATION);
	hCtrl2 = GetDlgItem(hWnd, IDC_ASPECTRATIOD);
	hCtrl3 = GetDlgItem(hWnd, IDC_ASPECT);

	if (hCtrl && hCtrl2 && hCtrl3)
	{
		snprintf(aspect_option, ARRAY_LENGTH(aspect_option), "aspect%d", GetSelectedScreen(hWnd));

		if (Button_GetCheck(hCtrl3))
			opts.set_value(aspect_option, "auto", OPTION_PRIORITY_CMDLINE, error_string);
		else
		{
			Edit_GetText(hCtrl, buffer, ARRAY_LENGTH(buffer));
			_stscanf(buffer, TEXT("%d"), &n);
			Edit_GetText(hCtrl2, buffer, ARRAY_LENGTH(buffer));
			_stscanf(buffer, TEXT("%d"), &d);

			if (n == 0 || d == 0)
			{
				n = 4;
				d = 3;
			}

			snprintf(buffer2, ARRAY_LENGTH(buffer2), "%d:%d", n, d);
			opts.set_value(aspect_option, buffer2, OPTION_PRIORITY_CMDLINE, error_string);
		}
	}
	
	/* snapshot size */
	hCtrl  = GetDlgItem(hWnd, IDC_SNAPSIZEWIDTH);
	hCtrl2 = GetDlgItem(hWnd, IDC_SNAPSIZEHEIGHT);
	hCtrl3 = GetDlgItem(hWnd, IDC_SNAPSIZE);
	
	if (hCtrl && hCtrl2 && hCtrl3)
	{
		if (Button_GetCheck(hCtrl3))
			opts.set_value(OPTION_SNAPSIZE, "auto", OPTION_PRIORITY_CMDLINE, error_string);
		else
		{
			Edit_GetText(hCtrl, buffer, ARRAY_LENGTH(buffer));
			_stscanf(buffer, TEXT("%d"), &width);
			Edit_GetText(hCtrl2, buffer, ARRAY_LENGTH(buffer));
			_stscanf(buffer, TEXT("%d"), &height);

			if (width == 0 || height == 0)
			{
				width = 640;
				height = 480;
			}

			snprintf(buffer2, ARRAY_LENGTH(buffer2), "%dx%d", width, height);
			opts.set_value(OPTION_SNAPSIZE, buffer2, OPTION_PRIORITY_CMDLINE, error_string);
		}
	}
}

/* Update options from the dialog */
static void UpdateOptions(HWND hDlg, datamap *map, windows_options &opts)
{
	/* These are always called together, so make one convenience function. */
	datamap_read_all_controls(map, hDlg, opts);
	PropToOptions(hDlg, opts);
}

/* Update the dialog from the options */
static void UpdateProperties(HWND hDlg, datamap *map, windows_options &opts)
{
	/* set ticks frequency in variours sliders */
	SendMessage(GetDlgItem(hDlg, IDC_GAMMA), 		TBM_SETTICFREQ, 5, 0);
	SendMessage(GetDlgItem(hDlg, IDC_CONTRAST), 	TBM_SETTICFREQ, 4, 0);
	SendMessage(GetDlgItem(hDlg, IDC_BRIGHTCORRECT),TBM_SETTICFREQ, 4, 0);
	SendMessage(GetDlgItem(hDlg, IDC_PAUSEBRIGHT), 	TBM_SETTICFREQ, 2, 0);
	SendMessage(GetDlgItem(hDlg, IDC_FSGAMMA),		TBM_SETTICFREQ, 5, 0);
	SendMessage(GetDlgItem(hDlg, IDC_FSCONTRAST), 	TBM_SETTICFREQ, 4, 0);
	SendMessage(GetDlgItem(hDlg, IDC_FSBRIGHTNESS), TBM_SETTICFREQ, 4, 0);
	SendMessage(GetDlgItem(hDlg, IDC_SECONDSTORUN), TBM_SETTICFREQ, 5, 0);
	SendMessage(GetDlgItem(hDlg, IDC_JDZ), 			TBM_SETTICFREQ, 2, 0);
	SendMessage(GetDlgItem(hDlg, IDC_JSAT), 		TBM_SETTICFREQ, 2, 0);
	SendMessage(GetDlgItem(hDlg, IDC_VOLUME), 		TBM_SETTICFREQ, 4, 0);
	SendMessage(GetDlgItem(hDlg, IDC_HIGH_PRIORITY),TBM_SETTICFREQ, 2, 0);
	SendMessage(GetDlgItem(hDlg, IDC_SPEED), 		TBM_SETTICFREQ, 5, 0);
	SendMessage(GetDlgItem(hDlg, IDC_BEAM), 		TBM_SETTICFREQ, 5, 0);
	SendMessage(GetDlgItem(hDlg, IDC_FLICKER), 		TBM_SETTICFREQ, 5, 0);
	/* These are always called together, so make one convenience function. */
	datamap_populate_all_controls(map, hDlg, opts);
	OptionsToProp(hDlg, opts);
	SetPropEnabledControls(hDlg);
}

/* Populate controls that are not handled in the DataMap */
static void OptionsToProp(HWND hWnd, windows_options &opts)
{
	HWND hCtrl;
	HWND hCtrl2;
	TCHAR buf[100];
	int  n = 0;
	int  d = 0;
	int  width = 0;
	int  height = 0;
	const char* cBuffer;
	int i = 0;
	char aspect_option[32];
	char buffer[MAX_PATH];

	/* Setup refresh list based on depth. */
	datamap_update_control(properties_datamap, hWnd, pCurrentOpts, IDC_REFRESH);
	/* Setup Select screen*/
	UpdateSelectScreenUI(hWnd );

	hCtrl = GetDlgItem(hWnd, IDC_ASPECT);
	
	if (hCtrl)
		Button_SetCheck(hCtrl, g_bAutoAspect[GetSelectedScreen(hWnd)]);

	hCtrl = GetDlgItem(hWnd, IDC_SNAPSIZE);
	
	if (hCtrl)
		Button_SetCheck(hCtrl, g_bAutoSnapSize);

	/* Bios select list */
	hCtrl = GetDlgItem(hWnd, IDC_BIOS);
	
	if (hCtrl)
	{
		int iCount = ComboBox_GetCount(hCtrl);

		for (i = 0; i < iCount; i++)
		{
			cBuffer = (const char*)ComboBox_GetItemData(hCtrl, i);
			
			if (strcmp(cBuffer, pCurrentOpts.value(OPTION_BIOS)) == 0)
			{
				(void)ComboBox_SetCurSel(hCtrl, i);
				break;
			}

		}
	}

	hCtrl = GetDlgItem(hWnd, IDC_ASPECT);
	
	if (hCtrl)
	{
		snprintf(aspect_option, ARRAY_LENGTH(aspect_option), "aspect%d", GetSelectedScreen(hWnd));

		if( strcmp(opts.value(aspect_option), "auto") == 0)
		{
			Button_SetCheck(hCtrl, TRUE);
			g_bAutoAspect[GetSelectedScreen(hWnd)] = TRUE;
		}
		else
		{
			Button_SetCheck(hCtrl, FALSE);
			g_bAutoAspect[GetSelectedScreen(hWnd)] = FALSE;
		}
	}

	/* aspect ratio */
	hCtrl  = GetDlgItem(hWnd, IDC_ASPECTRATION);
	hCtrl2 = GetDlgItem(hWnd, IDC_ASPECTRATIOD);
	
	if (hCtrl && hCtrl2)
	{
		snprintf(aspect_option, ARRAY_LENGTH(aspect_option), "aspect%d", GetSelectedScreen(hWnd));
		
		if (sscanf(opts.value(aspect_option), "%d:%d", &n, &d) == 2 && n != 0 && d != 0)
		{
			_sntprintf(buf, ARRAY_LENGTH(buf), TEXT("%d"), n);
			Edit_SetText(hCtrl, buf);
			_sntprintf(buf, ARRAY_LENGTH(buf), TEXT("%d"), d);
			Edit_SetText(hCtrl2, buf);
		}
		else
		{
			Edit_SetText(hCtrl,  TEXT("4"));
			Edit_SetText(hCtrl2, TEXT("3"));
		}
	}

	hCtrl = GetDlgItem(hWnd, IDC_EFFECT);
	
	if (hCtrl) 
	{
		const char* effect = opts.value(OPTION_EFFECT);
		
		if (strcmp(effect, "none") == 0)
			win_set_window_text_utf8(hCtrl, "None");
		else
			win_set_window_text_utf8(hCtrl, effect);
	}

	for (i = 0; i < 5; i++)
	{
		UpdateMameShader(hWnd, i, opts);
		UpdateScreenShader(hWnd, i, opts);
	}

	hCtrl = GetDlgItem(hWnd, IDC_CHEATFILE);
	
	if (hCtrl) 
	{
		const char* cheatfile = opts.value(OPTION_CHEATPATH);
		
		if (strcmp(cheatfile, "cheat") == 0)
			win_set_window_text_utf8(hCtrl, "Default");
		else
		{
			char *cheatname = strrchr(cheatfile, '\\');
			strcpy(buffer, cheatname + 1);
			win_set_window_text_utf8(hCtrl, buffer);
		}
	}

	hCtrl = GetDlgItem(hWnd, IDC_JOYSTICKMAP);
	
	if (hCtrl) 
	{
		const char* joymap = opts.value(OPTION_JOYSTICK_MAP);
	
		win_set_window_text_utf8(hCtrl, joymap);
	}

	hCtrl = GetDlgItem(hWnd, IDC_DEBUGSCRIPT);
	
	if (hCtrl) 
	{
		const char* debug = opts.value(OPTION_DEBUGSCRIPT);
		
		if (strcmp(debug, "") == 0)
			win_set_window_text_utf8(hCtrl, "None");
		else
		{
			TCHAR *t_filename = tstring_from_utf8(debug);
			TCHAR *tempname = PathFindFileName(t_filename);
			PathRemoveExtension(tempname);
			char *optname = utf8_from_tstring(tempname);
			strcpy(buffer, optname);
			free(t_filename);
			free(optname);
			win_set_window_text_utf8(hCtrl, buffer);
		}
	}

	hCtrl = GetDlgItem(hWnd, IDC_SNAPSIZE);
	
	if (hCtrl)
	{
		if(strcmp(opts.value(OPTION_SNAPSIZE), "auto") == 0)
		{
			Button_SetCheck(hCtrl, TRUE);
			g_bAutoSnapSize = TRUE;
		}
		else
		{
			Button_SetCheck(hCtrl, FALSE);
			g_bAutoSnapSize = FALSE;
		}
	}

	/* snapshot size */
	hCtrl  = GetDlgItem(hWnd, IDC_SNAPSIZEWIDTH);
	hCtrl2 = GetDlgItem(hWnd, IDC_SNAPSIZEHEIGHT);
	
	if (hCtrl && hCtrl2)
	{
		if (sscanf(opts.value(OPTION_SNAPSIZE), "%dx%d", &width, &height) == 2 && width != 0 && height != 0)
		{
			_sntprintf(buf, ARRAY_LENGTH(buf), TEXT("%d"), width);
			Edit_SetText(hCtrl, buf);
			_sntprintf(buf, ARRAY_LENGTH(buf), TEXT("%d"), height);
			Edit_SetText(hCtrl2, buf);
		}
		else
		{
			Edit_SetText(hCtrl,  TEXT("640"));
			Edit_SetText(hCtrl2, TEXT("480"));
		}
	}
}

/* Adjust controls - tune them to the currently selected game */
static void SetPropEnabledControls(HWND hWnd)
{
	int  nIndex = g_nGame;
	BOOL d3d = (!core_stricmp(pCurrentOpts.value(OSDOPTION_VIDEO), "d3d") || !core_stricmp(pCurrentOpts.value(OSDOPTION_VIDEO), "auto"));
	BOOL ddraw = !core_stricmp(pCurrentOpts.value(OSDOPTION_VIDEO), "ddraw");
	BOOL opengl = !core_stricmp(pCurrentOpts.value(OSDOPTION_VIDEO), "opengl");
	BOOL in_window = pCurrentOpts.bool_value(OSDOPTION_WINDOW);
	BOOL joystick_attached = DIJoystick.Available();

	Button_SetCheck(GetDlgItem(hWnd, IDC_ASPECT), g_bAutoAspect[GetSelectedScreen(hWnd)]);
	/* Video options */
	EnableWindow(GetDlgItem(hWnd, IDC_D3D_FILTER),		(opengl || d3d));
	EnableWindow(GetDlgItem(hWnd, IDC_HWSTRETCH),       ddraw);
	EnableWindow(GetDlgItem(hWnd, IDC_REFRESH),         !in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_SIZES),         	!in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSGAMMA),      	!in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSGAMMATEXT),  	!in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSGAMMADISP),  	!in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSBRIGHTNESS),    !in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSBRIGHTNESSTEXT),!in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSBRIGHTNESSDISP),!in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSCONTRAST),      !in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSCONTRASTTEXT),  !in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_FSCONTRASTDISP),  !in_window);
	EnableWindow(GetDlgItem(hWnd, IDC_ASPECTRATIOTEXT), !g_bAutoAspect[GetSelectedScreen(hWnd)]);
	EnableWindow(GetDlgItem(hWnd, IDC_ASPECTRATION), 	!g_bAutoAspect[GetSelectedScreen(hWnd)]);
	EnableWindow(GetDlgItem(hWnd, IDC_ASPECTRATIOD), 	!g_bAutoAspect[GetSelectedScreen(hWnd)]);
	EnableWindow(GetDlgItem(hWnd, IDC_ASPECTRATIOP), 	!g_bAutoAspect[GetSelectedScreen(hWnd)]);
	EnableWindow(GetDlgItem(hWnd, IDC_HLSL_ON), 		d3d);
	EnableWindow(GetDlgItem(hWnd, IDC_GLSL), 			opengl);	
	EnableWindow(GetDlgItem(hWnd, IDC_GLSLFILTER), 		opengl);	
	EnableWindow(GetDlgItem(hWnd, IDC_GLSLPOW), 		opengl);	
	EnableWindow(GetDlgItem(hWnd, IDC_GLSLTEXTURE), 	opengl);	
	EnableWindow(GetDlgItem(hWnd, IDC_GLSLVBO), 		opengl);	
	EnableWindow(GetDlgItem(hWnd, IDC_GLSLPBO), 		opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_GLSLSYNC), 		opengl);	
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SHADER0), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SHADER1), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SHADER2), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SHADER3), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SHADER4), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SCR_SHADER0), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SCR_SHADER1), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SCR_SHADER2), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SCR_SHADER3), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_SELECT_SCR_SHADER4), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SHADER0), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SHADER1), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SHADER2), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SHADER3), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SHADER4), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SCR_SHADER0), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SCR_SHADER1), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SCR_SHADER2), 	opengl);
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SCR_SHADER3), 	opengl);	
	EnableWindow(GetDlgItem(hWnd, IDC_RESET_SCR_SHADER4), 	opengl);	
	/* Snapshot options */
	EnableWindow(GetDlgItem(hWnd, IDC_SNAPSIZETEXT), 	!g_bAutoSnapSize);
	EnableWindow(GetDlgItem(hWnd, IDC_SNAPSIZEHEIGHT), 	!g_bAutoSnapSize);
	EnableWindow(GetDlgItem(hWnd, IDC_SNAPSIZEWIDTH), 	!g_bAutoSnapSize);
	EnableWindow(GetDlgItem(hWnd, IDC_SNAPSIZEX), 		!g_bAutoSnapSize);
	/* Joystick options */
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK),		joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JDZTEXT),			joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JDZDISP),			joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JDZ),				joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JSATTEXT),		joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JSATDISP),		joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JSAT),			joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK_ID_1),	joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK_ID_2),	joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK_ID_3),	joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK_ID_4),	joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK_ID_5),	joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK_ID_6),	joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK_ID_7),	joystick_attached);
	EnableWindow(GetDlgItem(hWnd, IDC_JOYSTICK_ID_8),	joystick_attached);
	/* Misc options */
	if (Button_GetCheck(GetDlgItem(hWnd, IDC_AUTOFRAMESKIP)))
		EnableWindow(GetDlgItem(hWnd, IDC_FRAMESKIP), 	FALSE);
	else
		EnableWindow(GetDlgItem(hWnd, IDC_FRAMESKIP), 	TRUE);

	if (nIndex <= -1 || DriverHasOptionalBIOS(nIndex))
		EnableWindow(GetDlgItem(hWnd, IDC_BIOS),		TRUE);
	else
		EnableWindow(GetDlgItem(hWnd, IDC_BIOS),		FALSE);

	if (nIndex <= -1 || DriverSupportsSaveState(nIndex))
		EnableWindow(GetDlgItem(hWnd, IDC_ENABLE_AUTOSAVE), TRUE);
	else
		EnableWindow(GetDlgItem(hWnd, IDC_ENABLE_AUTOSAVE), FALSE);
}

//============================================================
//  CONTROL HELPER FUNCTIONS FOR DATA EXCHANGE
//============================================================

static BOOL RotateReadControl(datamap *map, HWND dialog, HWND control, windows_options &opts, const char *option_name)
{
	int selected_index = ComboBox_GetCurSel(control);
	int original_selection = 0;

	// Figure out what the original selection value is
	if (opts.bool_value(OPTION_ROR) && !opts.bool_value(OPTION_ROL))
		original_selection = 1;
	else if (!opts.bool_value(OPTION_ROR) && opts.bool_value(OPTION_ROL))
		original_selection = 2;
	else if (!opts.bool_value(OPTION_ROTATE))
		original_selection = 3;
	else if (opts.bool_value(OPTION_AUTOROR))
		original_selection = 4;
	else if (opts.bool_value(OPTION_AUTOROL))
		original_selection = 5;

	// Any work to do?  If so, make the changes and return TRUE.
	if (selected_index != original_selection)
	{
		// Set the options based on the new selection.
		std::string error_string;
		opts.set_value(OPTION_ROR,		selected_index == 1, OPTION_PRIORITY_CMDLINE, error_string);
		opts.set_value(OPTION_ROL,		selected_index == 2, OPTION_PRIORITY_CMDLINE, error_string);
		opts.set_value(OPTION_ROTATE,	selected_index != 3, OPTION_PRIORITY_CMDLINE, error_string);
		opts.set_value(OPTION_AUTOROR,	selected_index == 4, OPTION_PRIORITY_CMDLINE, error_string);
		opts.set_value(OPTION_AUTOROL,	selected_index == 5, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		return TRUE;
	}

	// No changes
	return FALSE;
}

static BOOL RotatePopulateControl(datamap *map, HWND dialog, HWND control, windows_options &opts, const char *option_name)
{
	int selected_index = 0;
	
	if (opts.bool_value(OPTION_ROR) && !opts.bool_value(OPTION_ROL))
		selected_index = 1;
	else if (!opts.bool_value(OPTION_ROR) && opts.bool_value(OPTION_ROL))
		selected_index = 2;
	else if (!opts.bool_value(OPTION_ROTATE))
		selected_index = 3;
	else if (opts.bool_value(OPTION_AUTOROR))
		selected_index = 4;
	else if (opts.bool_value(OPTION_AUTOROL))
		selected_index = 5;

	(void)ComboBox_SetCurSel(control, selected_index);
	return FALSE;
}

static BOOL ScreenReadControl(datamap *map, HWND dialog, HWND control, windows_options &opts, const char *option_name)
{
	char screen_option_name[32];
	const char *screen_option_value;
	int selected_screen;
	int screen_option_index;
	std::string error_string;

	selected_screen = GetSelectedScreen(dialog);
	screen_option_index = ComboBox_GetCurSel(control);
	screen_option_value = (const char*)ComboBox_GetItemData(control, screen_option_index);
	snprintf(screen_option_name, ARRAY_LENGTH(screen_option_name), "screen%d", selected_screen);
	opts.set_value(screen_option_name, screen_option_value, OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
	return FALSE;
}

static BOOL ScreenPopulateControl(datamap *map, HWND dialog, HWND control, windows_options &opts, const char *option_name)
{
	DISPLAY_DEVICE dd;
	int i = 0;
	int nSelection = 0;
	const char *option;
	char *device;
	TCHAR *t_device;
	char screen_option[32];

	/* Remove all items in the list. */
	(void)ComboBox_ResetContent(control);
	(void)ComboBox_InsertString(control, 0, TEXT("Auto"));
	(void)ComboBox_SetItemData(control, 0, "auto");
	
	memset(&dd, 0, sizeof(DISPLAY_DEVICE));
	dd.cb = sizeof(DISPLAY_DEVICE);
	
	for (i = 0; EnumDisplayDevices(NULL, i, &dd, 0); i++)
	{
		if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
		{
			//we have to add 1 to account for the "auto" entry
			device = utf8_from_tstring(dd.DeviceName);
			t_device = tstring_from_utf8(device);
			(void)ComboBox_InsertString(control, i + 1, t_device);
			(void)ComboBox_SetItemData(control, i + 1, device);
			snprintf(screen_option, ARRAY_LENGTH(screen_option), "screen%d", GetSelectedScreen(dialog));
			option = opts.value(screen_option);
			
			if (strcmp(option, device) == 0)
				nSelection = i + 1;
			
			free(t_device);
			free(device);
		}
	}
	
	(void)ComboBox_SetCurSel(control, nSelection);
	return FALSE;
}

static void ViewSetOptionName(datamap *map, HWND dialog, HWND control, char *buffer)
{
	snprintf(buffer, ARRAY_LENGTH(buffer), "view%d", GetSelectedScreen(dialog));
}

static BOOL ViewPopulateControl(datamap *map, HWND dialog, HWND control, windows_options &opts, const char *option_name)
{
	int i;
	int selected_index = 0;
	char view_option[32];
	const char *view;

	// determine the view option value
	snprintf(view_option, ARRAY_LENGTH(view_option), "view%d", GetSelectedScreen(dialog));
	view = opts.value(view_option);

	(void)ComboBox_ResetContent(control);
	
	for (i = 0; i < NUMVIEW; i++)
	{
		(void)ComboBox_InsertString(control, i, g_ComboBoxView[i].m_pText);
		(void)ComboBox_SetItemData(control, i, g_ComboBoxView[i].m_pData);

		if (!strcmp(view, g_ComboBoxView[i].m_pData))
			selected_index = i;
	}

	(void)ComboBox_SetCurSel(control, selected_index);
	return FALSE;
}

static BOOL DefaultInputReadControl(datamap *map, HWND dialog, HWND control, windows_options &opts, const char *option_name)
{
	const char *input_option_value;
	int input_option_index;
	std::string error_string;

	input_option_index = ComboBox_GetCurSel(control);
	input_option_value = (const char*)ComboBox_GetItemData(control, input_option_index);
	opts.set_value(OPTION_CTRLR, input_option_index ? input_option_value : "", OPTION_PRIORITY_CMDLINE, error_string);
	assert(error_string.empty());
	return FALSE;
}

static BOOL DefaultInputPopulateControl(datamap *map, HWND dialog, HWND control, windows_options &opts, const char *option_name)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char *ext;
	char *root;
	char path[MAX_PATH];
	int selected = 0;
	int index = 0;
	const char *ctrlr_option;
	TCHAR *t_root;

	// determine the ctrlr option
	ctrlr_option = opts.value(OPTION_CTRLR);

	// reset the controllers dropdown
	(void)ComboBox_ResetContent(control);
	(void)ComboBox_InsertString(control, index, TEXT("Default"));
	(void)ComboBox_SetItemData(control, index, "");
	index++;
	snprintf(path, ARRAY_LENGTH(path), "%s\\*.*", GetCtrlrDir());
	hFind = win_find_first_file_utf8(path, &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile (hFind, &FindFileData) != 0)
		{
			// copy the filename
			root = utf8_from_tstring(FindFileData.cFileName);
			// find the extension
			ext = strrchr(root, '.');
			
			if (ext)
			{
				// check if it's a cfg file
				if (strcmp (ext, ".cfg") == 0)
				{
					// and strip off the extension
					*ext = 0;

					// set the option?
					if (!strcmp(root, ctrlr_option))
						selected = index;

					// add it as an option
					t_root = tstring_from_utf8(root);
					(void)ComboBox_InsertString(control, index, t_root);
					(void)ComboBox_SetItemData(control, index, root);
					free(t_root);
					free(root);
					index++;
				}
			}
		}

		FindClose (hFind);
	}

	(void)ComboBox_SetCurSel(control, selected);

	return FALSE;
}

static void ResolutionSetOptionName(datamap *map, HWND dialog, HWND control, char *buffer)
{
	snprintf(buffer, ARRAY_LENGTH(buffer), "resolution%d", GetSelectedScreen(dialog));
}

static BOOL ResolutionReadControl(datamap *map, HWND dialog, HWND control, windows_options &opts, const char *option_name)
{
	HWND refresh_control = GetDlgItem(dialog, IDC_REFRESH);
	HWND sizes_control = GetDlgItem(dialog, IDC_SIZES);
	int refresh_index, refresh_value, width, height;
	char option_value[256];
	TCHAR buffer[256];
	std::string error_string;

	if (refresh_control && sizes_control)
	{
		(void)ComboBox_GetText(sizes_control, buffer, ARRAY_LENGTH(buffer) - 1);
		
		if (_stscanf(buffer, TEXT("%d x %d"), &width, &height) == 2)
		{
			refresh_index = ComboBox_GetCurSel(refresh_control);
			refresh_value = ComboBox_GetItemData(refresh_control, refresh_index);
			snprintf(option_value, ARRAY_LENGTH(option_value), "%dx%d@%d", width, height, refresh_value);
		}
		else
			strcpy(option_value, "auto");

		opts.set_value(option_name, option_value, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
	}

	return FALSE;
}

static BOOL ResolutionPopulateControl(datamap *map, HWND dialog, HWND control_, windows_options &opts, const char *option_name)
{
	HWND sizes_control = GetDlgItem(dialog, IDC_SIZES);
	HWND refresh_control = GetDlgItem(dialog, IDC_REFRESH);
	int width, height, refresh;
	const char *option_value;
	int sizes_index = 0;
	int refresh_index = 0;
	int sizes_selection = 0;
	int refresh_selection = 0;
	char screen_option[32];
	const char *screen;
	LPTSTR t_screen;
	TCHAR buf[16];
	int i;
	DEVMODE devmode;

	if (sizes_control && refresh_control)
	{
		// determine the resolution
		option_value = opts.value(option_name);
		
		if (sscanf(option_value, "%dx%d@%d", &width, &height, &refresh) != 3)
		{
			width = 0;
			height = 0;
			refresh = 0;
		}

		// reset sizes control
		(void)ComboBox_ResetContent(sizes_control);
		(void)ComboBox_InsertString(sizes_control, sizes_index, TEXT("Auto"));
		(void)ComboBox_SetItemData(sizes_control, sizes_index, 0);
		sizes_index++;
		// reset refresh control
		(void)ComboBox_ResetContent(refresh_control);
		(void)ComboBox_InsertString(refresh_control, refresh_index, TEXT("Auto"));
		(void)ComboBox_SetItemData(refresh_control, refresh_index, 0);
		refresh_index++;
		// determine which screen we're using
		snprintf(screen_option, ARRAY_LENGTH(screen_option), "screen%d", GetSelectedScreen(dialog));
		screen = opts.value(screen_option);
		t_screen = tstring_from_utf8(screen);
		// retrieve screen information
		devmode.dmSize = sizeof(devmode);
		
		for (i = 0; EnumDisplaySettings(t_screen, i, &devmode); i++)
		{
			if ((devmode.dmBitsPerPel == 32 ) // Only 32 bit depth supported by core
				&& (devmode.dmDisplayFrequency == refresh || refresh == 0))
			{
				_sntprintf(buf, ARRAY_LENGTH(buf), TEXT("%li x %li"), devmode.dmPelsWidth, devmode.dmPelsHeight);

				if (ComboBox_FindString(sizes_control, 0, buf) == CB_ERR)
				{
					(void)ComboBox_InsertString(sizes_control, sizes_index, buf);

					if ((width == devmode.dmPelsWidth) && (height == devmode.dmPelsHeight))
						sizes_selection = sizes_index;
					
					sizes_index++;
				}
			}

			if (devmode.dmDisplayFrequency >= 10 )
			{
				// I have some devmode "vga" which specifes 1 Hz, which is probably bogus, so we filter it out
				_sntprintf(buf, ARRAY_LENGTH(buf), TEXT("%li Hz"), devmode.dmDisplayFrequency);

				if (ComboBox_FindString(refresh_control, 0, buf) == CB_ERR)
				{
					(void)ComboBox_InsertString(refresh_control, refresh_index, buf);
					(void)ComboBox_SetItemData(refresh_control, refresh_index, devmode.dmDisplayFrequency);

					if (refresh == devmode.dmDisplayFrequency)
						refresh_selection = refresh_index;

					refresh_index++;
				}
			}
		}
		
		free(t_screen);
		(void)ComboBox_SetCurSel(sizes_control, sizes_selection);
		(void)ComboBox_SetCurSel(refresh_control, refresh_selection);
	}

	return FALSE;
}

//============================================================

/************************************************************
 * DataMap initializers
 ************************************************************/

/* Initialize local helper variables */
static void ResetDataMap(HWND hWnd)
{
	char screen_option[32];

	snprintf(screen_option, ARRAY_LENGTH(screen_option), "screen%d", GetSelectedScreen(hWnd));

	if (pCurrentOpts.value(screen_option) == NULL || (core_stricmp(pCurrentOpts.value(screen_option), "") == 0 )
		|| (core_stricmp(pCurrentOpts.value(screen_option), "auto") == 0 ) )
	{
		std::string error_string;
		pCurrentOpts.set_value(screen_option, "auto", OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
	}
}

/* Build the control mapping by adding all needed information to the DataMap */
static void BuildDataMap(void)
{
	properties_datamap = datamap_create();

	// core state options
	datamap_add(properties_datamap, IDC_ENABLE_AUTOSAVE,		DM_BOOL,	OPTION_AUTOSAVE);
	datamap_add(properties_datamap, IDC_SNAPVIEW,				DM_STRING,	OPTION_SNAPVIEW);
	datamap_add(properties_datamap, IDC_SNAPNAME,				DM_STRING,	OPTION_SNAPNAME);
	datamap_add(properties_datamap, IDC_SNAPBILINEAR,			DM_BOOL,	OPTION_SNAPBILINEAR);
	datamap_add(properties_datamap, IDC_SNAPBURNIN,				DM_BOOL,	OPTION_BURNIN);
	datamap_add(properties_datamap, IDC_SNAPSIZEWIDTH,			DM_STRING,	NULL);
	datamap_add(properties_datamap, IDC_SNAPSIZEHEIGHT,			DM_STRING,	NULL);
	// core performance options
	datamap_add(properties_datamap, IDC_AUTOFRAMESKIP,			DM_BOOL,	OPTION_AUTOFRAMESKIP);
	datamap_add(properties_datamap, IDC_FRAMESKIP,				DM_INT,		OPTION_FRAMESKIP);
	datamap_add(properties_datamap, IDC_SECONDSTORUN,			DM_INT,		OPTION_SECONDS_TO_RUN);
	datamap_add(properties_datamap, IDC_SECONDSTORUNDISP,		DM_INT,		OPTION_SECONDS_TO_RUN);
	datamap_add(properties_datamap, IDC_THROTTLE,				DM_BOOL,	OPTION_THROTTLE);
	datamap_add(properties_datamap, IDC_SLEEP,					DM_BOOL,	OPTION_SLEEP);
	datamap_add(properties_datamap, IDC_SPEED,					DM_FLOAT,	OPTION_SPEED);
	datamap_add(properties_datamap, IDC_SPEEDDISP,				DM_FLOAT,	OPTION_SPEED);
	datamap_add(properties_datamap, IDC_REFRESHSPEED,			DM_BOOL,	OPTION_REFRESHSPEED);
	// core rotation options
	datamap_add(properties_datamap, IDC_ROTATE,					DM_INT,		NULL);
	// ror, rol, autoror, autorol handled by callback
	datamap_add(properties_datamap, IDC_FLIPX,					DM_BOOL,	OPTION_FLIPX);
	datamap_add(properties_datamap, IDC_FLIPY,					DM_BOOL,	OPTION_FLIPY);
	// core artwork options
	datamap_add(properties_datamap, IDC_ARTWORK_CROP,			DM_BOOL,	OPTION_ARTWORK_CROP);
	datamap_add(properties_datamap, IDC_BACKDROPS,				DM_BOOL,	OPTION_USE_BACKDROPS);
	datamap_add(properties_datamap, IDC_OVERLAYS,				DM_BOOL,	OPTION_USE_OVERLAYS);
	datamap_add(properties_datamap, IDC_BEZELS,					DM_BOOL,	OPTION_USE_BEZELS);
	datamap_add(properties_datamap, IDC_CPANELS,				DM_BOOL,	OPTION_USE_CPANELS);
	datamap_add(properties_datamap, IDC_MARQUEES,				DM_BOOL,	OPTION_USE_MARQUEES);
	// core screen options
	datamap_add(properties_datamap, IDC_BRIGHTCORRECT,			DM_FLOAT,	OPTION_BRIGHTNESS);
	datamap_add(properties_datamap, IDC_BRIGHTCORRECTDISP,		DM_FLOAT,	OPTION_BRIGHTNESS);
	datamap_add(properties_datamap, IDC_CONTRAST,				DM_FLOAT,	OPTION_CONTRAST);
	datamap_add(properties_datamap, IDC_CONTRASTDISP,			DM_FLOAT,	OPTION_CONTRAST);
	datamap_add(properties_datamap, IDC_GAMMA,					DM_FLOAT,	OPTION_GAMMA);
	datamap_add(properties_datamap, IDC_GAMMADISP,				DM_FLOAT,	OPTION_GAMMA);
	datamap_add(properties_datamap, IDC_PAUSEBRIGHT,			DM_FLOAT,	OPTION_PAUSE_BRIGHTNESS);
	datamap_add(properties_datamap, IDC_PAUSEBRIGHTDISP,		DM_FLOAT,	OPTION_PAUSE_BRIGHTNESS);
	datamap_add(properties_datamap, IDC_EFFECT,					DM_STRING,	OPTION_EFFECT);
	datamap_add(properties_datamap, IDC_WIDESTRETCH,			DM_BOOL,	OPTION_WIDESTRETCH);
	// core opengl options
	datamap_add(properties_datamap, IDC_GLSLPOW,				DM_BOOL,	OSDOPTION_GL_FORCEPOW2TEXTURE);
	datamap_add(properties_datamap, IDC_GLSLTEXTURE,			DM_BOOL,	OSDOPTION_GL_NOTEXTURERECT);
	datamap_add(properties_datamap, IDC_GLSLVBO,				DM_BOOL,	OSDOPTION_GL_VBO);
	datamap_add(properties_datamap, IDC_GLSLPBO,				DM_BOOL,	OSDOPTION_GL_PBO);
	datamap_add(properties_datamap, IDC_GLSL,					DM_BOOL,	OSDOPTION_GL_GLSL);
	datamap_add(properties_datamap, IDC_GLSLFILTER,				DM_BOOL,	OSDOPTION_GLSL_FILTER);
	datamap_add(properties_datamap, IDC_GLSLSYNC,				DM_BOOL,	OSDOPTION_GLSL_SYNC);
	datamap_add(properties_datamap, IDC_MAME_SHADER0,			DM_STRING,	OSDOPTION_SHADER_MAME "0");
	datamap_add(properties_datamap, IDC_MAME_SHADER1,			DM_STRING,	OSDOPTION_SHADER_MAME "1");
	datamap_add(properties_datamap, IDC_MAME_SHADER2,			DM_STRING,	OSDOPTION_SHADER_MAME "2");
	datamap_add(properties_datamap, IDC_MAME_SHADER3,			DM_STRING,	OSDOPTION_SHADER_MAME "3");
	datamap_add(properties_datamap, IDC_MAME_SHADER4,			DM_STRING,	OSDOPTION_SHADER_MAME "4");
	datamap_add(properties_datamap, IDC_SCREEN_SHADER0,			DM_STRING,	OSDOPTION_SHADER_SCREEN "0");
	datamap_add(properties_datamap, IDC_SCREEN_SHADER1,			DM_STRING,	OSDOPTION_SHADER_SCREEN "1");
	datamap_add(properties_datamap, IDC_SCREEN_SHADER2,			DM_STRING,	OSDOPTION_SHADER_SCREEN "2");
	datamap_add(properties_datamap, IDC_SCREEN_SHADER3,			DM_STRING,	OSDOPTION_SHADER_SCREEN "3");
	datamap_add(properties_datamap, IDC_SCREEN_SHADER4,			DM_STRING,	OSDOPTION_SHADER_SCREEN "4");
	// core vector options
	datamap_add(properties_datamap, IDC_ANTIALIAS,				DM_BOOL,	OPTION_ANTIALIAS);
	datamap_add(properties_datamap, IDC_BEAM,					DM_FLOAT,	OPTION_BEAM);
	datamap_add(properties_datamap, IDC_BEAMDISP,				DM_FLOAT,	OPTION_BEAM);
	datamap_add(properties_datamap, IDC_FLICKER,				DM_FLOAT,	OPTION_FLICKER);
	datamap_add(properties_datamap, IDC_FLICKERDISP,			DM_FLOAT,	OPTION_FLICKER);
	// core sound options
	datamap_add(properties_datamap, IDC_SOUND_MODE,				DM_STRING,	OSDOPTION_SOUND);
	datamap_add(properties_datamap, IDC_SAMPLERATE,				DM_INT,		OPTION_SAMPLERATE);
	datamap_add(properties_datamap, IDC_SAMPLES,				DM_BOOL,	OPTION_SAMPLES);
	datamap_add(properties_datamap, IDC_VOLUME,					DM_INT,		OPTION_VOLUME);
	datamap_add(properties_datamap, IDC_VOLUMEDISP,				DM_INT,		OPTION_VOLUME);
	// core input options
	datamap_add(properties_datamap, IDC_COINLOCKOUT,			DM_BOOL,	OPTION_COIN_LOCKOUT);
	datamap_add(properties_datamap, IDC_DEFAULT_INPUT,			DM_STRING,	OPTION_CTRLR);
	datamap_add(properties_datamap, IDC_USE_MOUSE,				DM_BOOL,	OPTION_MOUSE);
	datamap_add(properties_datamap, IDC_JOYSTICK,				DM_BOOL,	OPTION_JOYSTICK);
	datamap_add(properties_datamap, IDC_LIGHTGUN,				DM_BOOL,	OPTION_LIGHTGUN);
	datamap_add(properties_datamap, IDC_STEADYKEY,				DM_BOOL,	OPTION_STEADYKEY);
	datamap_add(properties_datamap, IDC_MULTIKEYBOARD,			DM_BOOL,	OPTION_MULTIKEYBOARD);
	datamap_add(properties_datamap, IDC_MULTIMOUSE,				DM_BOOL,	OPTION_MULTIMOUSE);
	datamap_add(properties_datamap, IDC_RELOAD,					DM_BOOL,	OPTION_OFFSCREEN_RELOAD);
	datamap_add(properties_datamap, IDC_JDZ,					DM_FLOAT,	OPTION_JOYSTICK_DEADZONE);
	datamap_add(properties_datamap, IDC_JDZDISP,				DM_FLOAT,	OPTION_JOYSTICK_DEADZONE);
	datamap_add(properties_datamap, IDC_JSAT,					DM_FLOAT,	OPTION_JOYSTICK_SATURATION);
	datamap_add(properties_datamap, IDC_JSATDISP,				DM_FLOAT,	OPTION_JOYSTICK_SATURATION);
	datamap_add(properties_datamap, IDC_JOYSTICKMAP,			DM_STRING,	OPTION_JOYSTICK_MAP);
	// core input automatic enable options
	datamap_add(properties_datamap, IDC_PADDLE,					DM_STRING,	OPTION_PADDLE_DEVICE);
	datamap_add(properties_datamap, IDC_ADSTICK,				DM_STRING,	OPTION_ADSTICK_DEVICE);
	datamap_add(properties_datamap, IDC_PEDAL,					DM_STRING,	OPTION_PEDAL_DEVICE);
	datamap_add(properties_datamap, IDC_DIAL,					DM_STRING,	OPTION_DIAL_DEVICE);
	datamap_add(properties_datamap, IDC_TRACKBALL,				DM_STRING,	OPTION_TRACKBALL_DEVICE);
	datamap_add(properties_datamap, IDC_LIGHTGUNDEVICE,			DM_STRING,	OPTION_LIGHTGUN_DEVICE);
	datamap_add(properties_datamap, IDC_POSITIONAL,				DM_STRING,	OPTION_POSITIONAL_DEVICE);
	datamap_add(properties_datamap, IDC_MOUSE,					DM_STRING,	OPTION_MOUSE_DEVICE);
	// core debugging options
	datamap_add(properties_datamap, IDC_LOG,					DM_BOOL,	OPTION_LOG);
	datamap_add(properties_datamap, IDC_DEBUG,					DM_BOOL,	OPTION_DEBUG);
	datamap_add(properties_datamap, IDC_VERBOSE,				DM_BOOL,	OPTION_VERBOSE);
	datamap_add(properties_datamap, IDC_UPDATEINPAUSE,			DM_BOOL,	OPTION_UPDATEINPAUSE);
	datamap_add(properties_datamap, IDC_DEBUGSCRIPT,			DM_STRING,	OPTION_DEBUGSCRIPT);
	// core misc options
	datamap_add(properties_datamap, IDC_BIOS,					DM_STRING,	OPTION_BIOS);
	datamap_add(properties_datamap, IDC_CHEAT,					DM_BOOL,	OPTION_CHEAT);
	datamap_add(properties_datamap, IDC_HISCORE,				DM_BOOL,	OPTION_HISCORE);
	datamap_add(properties_datamap, IDC_SKIP_GAME_INFO,			DM_BOOL,	OPTION_SKIP_GAMEINFO);
	datamap_add(properties_datamap, IDC_CONFIRM_QUIT,			DM_BOOL,	OPTION_CONFIRM_QUIT);
	datamap_add(properties_datamap, IDC_CHEATFILE,				DM_STRING,	OPTION_CHEATPATH);
	// windows debugging options
	datamap_add(properties_datamap, IDC_OSLOG,					DM_BOOL,	OPTION_OSLOG);
	// windows performance options
	datamap_add(properties_datamap, IDC_HIGH_PRIORITY,			DM_INT,		WINOPTION_PRIORITY);
	datamap_add(properties_datamap, IDC_HIGH_PRIORITYTXT,		DM_INT,		WINOPTION_PRIORITY);
	datamap_add(properties_datamap, IDC_MULTITHREAD_RENDERING,	DM_BOOL,	OSDOPTION_MULTITHREADING);
	// windows video options
	datamap_add(properties_datamap, IDC_VIDEO_MODE,				DM_STRING,	OSDOPTION_VIDEO);
	datamap_add(properties_datamap, IDC_NUMSCREENS,				DM_INT,		OSDOPTION_NUMSCREENS);
	datamap_add(properties_datamap, IDC_NUMSCREENSDISP,			DM_INT,		OSDOPTION_NUMSCREENS);
	datamap_add(properties_datamap, IDC_WINDOWED,				DM_BOOL,	OSDOPTION_WINDOW);
	datamap_add(properties_datamap, IDC_MAXIMIZE,				DM_BOOL,	OSDOPTION_MAXIMIZE);
	datamap_add(properties_datamap, IDC_KEEPASPECT,				DM_BOOL,	OSDOPTION_KEEPASPECT);
	datamap_add(properties_datamap, IDC_PRESCALE,				DM_INT,		OSDOPTION_PRESCALE);
	datamap_add(properties_datamap, IDC_PRESCALEDISP,			DM_INT,		OSDOPTION_PRESCALE);
	datamap_add(properties_datamap, IDC_WAITVSYNC,				DM_BOOL,	OSDOPTION_WAITVSYNC);
	datamap_add(properties_datamap, IDC_SYNCREFRESH,			DM_BOOL,	OPTION_SYNCREFRESH);
	datamap_add(properties_datamap, IDC_D3D_FILTER,				DM_BOOL,	OSDOPTION_FILTER);
	// directdraw specific options
	datamap_add(properties_datamap, IDC_HWSTRETCH,				DM_BOOL,	WINOPTION_HWSTRETCH);
	// per window video options
	datamap_add(properties_datamap, IDC_SCREEN,					DM_STRING,	NULL);
	datamap_add(properties_datamap, IDC_SCREENSELECT,			DM_STRING,	NULL);
	datamap_add(properties_datamap, IDC_VIEW,					DM_STRING,	NULL);
	datamap_add(properties_datamap, IDC_ASPECTRATIOD,			DM_STRING,  NULL);
	datamap_add(properties_datamap, IDC_ASPECTRATION,			DM_STRING,  NULL);
	datamap_add(properties_datamap, IDC_REFRESH,				DM_STRING,  NULL);
	datamap_add(properties_datamap, IDC_SIZES,					DM_STRING,  NULL);
	// full screen options
	datamap_add(properties_datamap, IDC_TRIPLE_BUFFER,			DM_BOOL,	WINOPTION_TRIPLEBUFFER);
	datamap_add(properties_datamap, IDC_SWITCHRES,				DM_BOOL,	OSDOPTION_SWITCHRES);
	datamap_add(properties_datamap, IDC_FSBRIGHTNESS,			DM_FLOAT,	WINOPTION_FULLSCREENBRIGHTNESS);
	datamap_add(properties_datamap, IDC_FSBRIGHTNESSDISP,		DM_FLOAT,	WINOPTION_FULLSCREENBRIGHTNESS);
	datamap_add(properties_datamap, IDC_FSCONTRAST,				DM_FLOAT,	WINOPTION_FULLSCREENCONTRAST);
	datamap_add(properties_datamap, IDC_FSCONTRASTDISP,			DM_FLOAT,	WINOPTION_FULLSCREENCONTRAST);
	datamap_add(properties_datamap, IDC_FSGAMMA,				DM_FLOAT,	WINOPTION_FULLSCREENGAMMA);
	datamap_add(properties_datamap, IDC_FSGAMMADISP,			DM_FLOAT,	WINOPTION_FULLSCREENGAMMA);
	// windows sound options
	datamap_add(properties_datamap, IDC_AUDIO_LATENCY,			DM_INT,		OSDOPTION_AUDIO_LATENCY);
	datamap_add(properties_datamap, IDC_AUDIO_LATENCY_DISP,		DM_INT,		OSDOPTION_AUDIO_LATENCY);
	// input device options
	datamap_add(properties_datamap, IDC_DUAL_LIGHTGUN,			DM_BOOL,	WINOPTION_DUAL_LIGHTGUN);
	datamap_add(properties_datamap, IDC_JOYSTICK_ID_1,			DM_INT,		WINOPTION_JOYSTICK_ID_1);
	datamap_add(properties_datamap, IDC_JOYSTICK_ID_2,			DM_INT,		WINOPTION_JOYSTICK_ID_2);
	datamap_add(properties_datamap, IDC_JOYSTICK_ID_3,			DM_INT,		WINOPTION_JOYSTICK_ID_3);
	datamap_add(properties_datamap, IDC_JOYSTICK_ID_4,			DM_INT,		WINOPTION_JOYSTICK_ID_4);
	datamap_add(properties_datamap, IDC_JOYSTICK_ID_5,			DM_INT,		WINOPTION_JOYSTICK_ID_5);
	datamap_add(properties_datamap, IDC_JOYSTICK_ID_6,			DM_INT,		WINOPTION_JOYSTICK_ID_6);
	datamap_add(properties_datamap, IDC_JOYSTICK_ID_7,			DM_INT,		WINOPTION_JOYSTICK_ID_7);
	datamap_add(properties_datamap, IDC_JOYSTICK_ID_8,			DM_INT,		WINOPTION_JOYSTICK_ID_8);
	// hlsl
	datamap_add(properties_datamap, IDC_HLSL_ON,				DM_BOOL,	WINOPTION_HLSL_ENABLE);
	// set up callbacks
	datamap_set_callback(properties_datamap, IDC_ROTATE,		DCT_READ_CONTROL,		RotateReadControl);
	datamap_set_callback(properties_datamap, IDC_ROTATE,		DCT_POPULATE_CONTROL,	RotatePopulateControl);
	datamap_set_callback(properties_datamap, IDC_SCREEN,		DCT_READ_CONTROL,		ScreenReadControl);
	datamap_set_callback(properties_datamap, IDC_SCREEN,		DCT_POPULATE_CONTROL,	ScreenPopulateControl);
	datamap_set_callback(properties_datamap, IDC_VIEW,			DCT_POPULATE_CONTROL,	ViewPopulateControl);
	datamap_set_callback(properties_datamap, IDC_REFRESH,		DCT_READ_CONTROL,		ResolutionReadControl);
	datamap_set_callback(properties_datamap, IDC_REFRESH,		DCT_POPULATE_CONTROL,	ResolutionPopulateControl);
	datamap_set_callback(properties_datamap, IDC_SIZES,			DCT_READ_CONTROL,		ResolutionReadControl);
	datamap_set_callback(properties_datamap, IDC_SIZES,			DCT_POPULATE_CONTROL,	ResolutionPopulateControl);
	datamap_set_callback(properties_datamap, IDC_DEFAULT_INPUT,	DCT_READ_CONTROL,		DefaultInputReadControl);
	datamap_set_callback(properties_datamap, IDC_DEFAULT_INPUT,	DCT_POPULATE_CONTROL,	DefaultInputPopulateControl);
	datamap_set_option_name_callback(properties_datamap, IDC_VIEW,		ViewSetOptionName);
	//missing population of views with per game defined additional views
	datamap_set_option_name_callback(properties_datamap, IDC_REFRESH,	ResolutionSetOptionName);
	datamap_set_option_name_callback(properties_datamap, IDC_SIZES,		ResolutionSetOptionName);
	// formats
	datamap_set_int_format(properties_datamap, IDC_VOLUMEDISP,			"%ddB");
	datamap_set_int_format(properties_datamap, IDC_AUDIO_LATENCY_DISP,	"%d/5");
	datamap_set_float_format(properties_datamap, IDC_BEAMDISP,			"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_FLICKERDISP,		"%03.1f");
	datamap_set_float_format(properties_datamap, IDC_GAMMADISP,			"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_BRIGHTCORRECTDISP,	"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_CONTRASTDISP,		"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_PAUSEBRIGHTDISP,	"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_FSGAMMADISP,		"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_FSBRIGHTNESSDISP,	"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_FSCONTRASTDISP,	"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_JDZDISP,			"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_JSATDISP,			"%03.2f");
	datamap_set_float_format(properties_datamap, IDC_SPEEDDISP,			"%03.2f");
	// trackbar ranges - slider-name,start,end,step
	datamap_set_trackbar_range(properties_datamap, IDC_JDZ,         	0.0, 1.0, (float)0.05);
	datamap_set_trackbar_range(properties_datamap, IDC_JSAT,        	0.0, 1.0, (float)0.05);
	datamap_set_trackbar_range(properties_datamap, IDC_SPEED,       	0.0, 100.0, (float)1.00);
	datamap_set_trackbar_range(properties_datamap, IDC_BEAM,        	0.0, 10.0, (float)0.10);
	datamap_set_trackbar_range(properties_datamap, IDC_FLICKER,       	0.0, 100.0, (float)1.00);
	datamap_set_trackbar_range(properties_datamap, IDC_AUDIO_LATENCY, 	1, 5, 1);
	datamap_set_trackbar_range(properties_datamap, IDC_VOLUME,      	-32, 0, 1);
	datamap_set_trackbar_range(properties_datamap, IDC_HIGH_PRIORITY, 	-15, 1, 1);
	datamap_set_trackbar_range(properties_datamap, IDC_SECONDSTORUN, 	0, 60, 1);
	datamap_set_trackbar_range(properties_datamap, IDC_NUMSCREENS, 		1, 4, 1);
	datamap_set_trackbar_range(properties_datamap, IDC_PRESCALE, 		1, 3, 1);
	datamap_set_trackbar_range(properties_datamap, IDC_FSGAMMA, 		0.0, 3.0, (float)0.05);
	datamap_set_trackbar_range(properties_datamap, IDC_FSBRIGHTNESS, 	0.0, 2.0, (float)0.05);
	datamap_set_trackbar_range(properties_datamap, IDC_FSCONTRAST, 		0.0, 2.0, (float)0.05);
	datamap_set_trackbar_range(properties_datamap, IDC_GAMMA, 			0.0, 3.0, (float)0.05);
	datamap_set_trackbar_range(properties_datamap, IDC_BRIGHTCORRECT, 	0.0, 2.0, (float)0.05);
	datamap_set_trackbar_range(properties_datamap, IDC_CONTRAST, 		0.0, 2.0, (float)0.05);
	datamap_set_trackbar_range(properties_datamap, IDC_PAUSEBRIGHT, 	0.0, 1.0, (float)0.05);
}

//mamefx: for coloring of changed elements
static BOOL IsControlOptionValue(HWND hDlg, HWND hWnd_ctrl, windows_options &opts, windows_options &ref)
{
	const char *option_name;
	const char *opts_value;
	const char *ref_value;

	option_name = datamap_get_control_option_name(properties_datamap, hDlg, hWnd_ctrl);
	
	if (option_name == NULL)
		return TRUE;

	opts_value = opts.value(option_name);
	ref_value = ref.value(option_name);

	if (opts_value == ref_value)
		return TRUE;

	if (!opts_value || !ref_value)
		return FALSE;

	return strcmp(opts_value, ref_value) == 0;
}

/* Moved here cause it's called in a few places */
static void InitializeOptions(HWND hDlg)
{
	InitializeSampleRateUI(hDlg);
	InitializeSoundModeUI(hDlg);
	InitializeSkippingUI(hDlg);
	InitializeRotateUI(hDlg);
	InitializeSelectScreenUI(hDlg);
	InitializeBIOSUI(hDlg);
	InitializeControllerMappingUI(hDlg);
	InitializeVideoUI(hDlg);
	InitializeSnapViewUI(hDlg);
	InitializeSnapNameUI(hDlg);
	InitializeJoystickIDUI(hDlg);
}

static void OptOnHScroll(HWND hWnd, HWND hWndCtl, UINT code, int pos)
{
	if (hWndCtl == GetDlgItem(hWnd, IDC_NUMSCREENS))
		NumScreensSelectionChange(hWnd);
}

/* Handle changes to the Numscreens slider */
static void NumScreensSelectionChange(HWND hWnd)
{
	//Also Update the ScreenSelect Combo with the new number of screens
	UpdateSelectScreenUI(hWnd );
}

/* Handle changes to the Refresh drop down */
static void RefreshSelectionChange(HWND hWnd, HWND hWndCtrl)
{
	int nCurSelection;

	nCurSelection = ComboBox_GetCurSel(hWndCtrl);
	
	if (nCurSelection != CB_ERR)
	{
		datamap_read_control(properties_datamap, hWnd, pCurrentOpts, IDC_SIZES);
		datamap_populate_control(properties_datamap, hWnd, pCurrentOpts, IDC_SIZES);
	}
}

/* Populate the Sample Rate drop down */
static void InitializeSampleRateUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_SAMPLERATE);
	int i;

	if (hCtrl)
	{
		for (i = 0; i < NUMSAMPLERATE; i++)
		{
			(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxSampleRate[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxSampleRate[i].m_pData);
		}
	}
}

/* Populate the Sound Mode drop down */
static void InitializeSoundModeUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_SOUND_MODE);
	int i;

	if (hCtrl)
	{
		for (i = 0; i < NUMSOUND; i++)
		{
			(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxSound[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxSound[i].m_pData);
		}
	}
}

/* Populate the Frame Skipping drop down */
static void InitializeSkippingUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_FRAMESKIP);
	int i;

	if (hCtrl)
	{
		for (i = 0; i < NUMFRAMESKIP; i++)
		{
			(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxFrameSkip[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxFrameSkip[i].m_pData);
		}
	}
}

/* Populate the Rotate drop down */
static void InitializeRotateUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_ROTATE);

	if (hCtrl)
	{
		(void)ComboBox_AddString(hCtrl, TEXT("Default"));             // 0
		(void)ComboBox_AddString(hCtrl, TEXT("Clockwise"));           // 1
		(void)ComboBox_AddString(hCtrl, TEXT("Anti-clockwise"));      // 2
		(void)ComboBox_AddString(hCtrl, TEXT("None"));                // 3
		(void)ComboBox_AddString(hCtrl, TEXT("Auto clockwise"));      // 4
		(void)ComboBox_AddString(hCtrl, TEXT("Auto anti-clockwise")); // 5
	}
}

/* Populate the Video Mode drop down */
static void InitializeVideoUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_VIDEO_MODE);
	int i;

	if (hCtrl)
	{
		for (i = 0; i < NUMVIDEO; i++)
		{
			(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxVideo[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxVideo[i].m_pData);
		}
	}
}

static void InitializeSnapNameUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_SNAPNAME);
	int i;

	if (hCtrl)
	{
		for (i = 0; i < NUMSNAPNAME; i++)
		{
			(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxSnapName[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxSnapName[i].m_pData);
		}
	}
}

static void InitializeSnapViewUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_SNAPVIEW);
	int i;

	if (hCtrl)
	{
		for (i = 0; i < NUMSNAPVIEW; i++)
		{
			(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxSnapView[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxSnapView[i].m_pData);
		}
	}
}

static void UpdateSelectScreenUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_SCREENSELECT);
	int i, curSel;
	
	if (hCtrl)
	{
		curSel = ComboBox_GetCurSel(hCtrl);
		
		if ((curSel < 0) || (curSel >= NUMSELECTSCREEN))
			curSel = 0;
		
		(void)ComboBox_ResetContent(hCtrl);
		
		for (i = 0; i < NUMSELECTSCREEN && i < pCurrentOpts.int_value(OSDOPTION_NUMSCREENS) ; i++)
		{
			(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxSelectScreen[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxSelectScreen[i].m_pData);
		}
		
		// Smaller Amount of screens was selected, so use 0
		if( i < curSel )
			(void)ComboBox_SetCurSel(hCtrl, 0);
		else
			(void)ComboBox_SetCurSel(hCtrl, curSel);
	}
}

/* Populate the Select Screen drop down */
static void InitializeSelectScreenUI(HWND hWnd)
{
	UpdateSelectScreenUI(hWnd);
}

static void InitializeControllerMappingUI(HWND hWnd)
{
	int i;
	HWND hCtrl  = GetDlgItem(hWnd, IDC_PADDLE);
	HWND hCtrl1 = GetDlgItem(hWnd, IDC_ADSTICK);
	HWND hCtrl2 = GetDlgItem(hWnd, IDC_PEDAL);
	HWND hCtrl3 = GetDlgItem(hWnd, IDC_MOUSE);
	HWND hCtrl4 = GetDlgItem(hWnd, IDC_DIAL);
	HWND hCtrl5 = GetDlgItem(hWnd, IDC_TRACKBALL);
	HWND hCtrl6 = GetDlgItem(hWnd, IDC_LIGHTGUNDEVICE);
	HWND hCtrl7 = GetDlgItem(hWnd, IDC_POSITIONAL);

	if (hCtrl)
	{
		for (i = 0; i < NUMDEVICES; i++)
		{
			(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxDevice[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxDevice[i].m_pData);
		}
	}
	
	if (hCtrl1)
	{
		for (i = 0; i < NUMDEVICES; i++)
		{
			(void)ComboBox_InsertString(hCtrl1, i, g_ComboBoxDevice[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl1, i, g_ComboBoxDevice[i].m_pData);
		}
	}
	
	if (hCtrl2)
	{
		for (i = 0; i < NUMDEVICES; i++)
		{
			(void)ComboBox_InsertString(hCtrl2, i, g_ComboBoxDevice[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl2, i, g_ComboBoxDevice[i].m_pData);
		}
	}
	
	if (hCtrl3)
	{
		for (i = 0; i < NUMDEVICES; i++)
		{
			(void)ComboBox_InsertString(hCtrl3, i, g_ComboBoxDevice[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl3, i, g_ComboBoxDevice[i].m_pData);
		}
	}
	
	if (hCtrl4)
	{
		for (i = 0; i < NUMDEVICES; i++)
		{
			(void)ComboBox_InsertString(hCtrl4, i, g_ComboBoxDevice[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl4, i, g_ComboBoxDevice[i].m_pData);
		}
	}
	
	if (hCtrl5)
	{
		for (i = 0; i < NUMDEVICES; i++)
		{
			(void)ComboBox_InsertString(hCtrl5, i, g_ComboBoxDevice[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl5, i, g_ComboBoxDevice[i].m_pData);
		}
	}
	
	if (hCtrl6)
	{
		for (i = 0; i < NUMDEVICES; i++)
		{
			(void)ComboBox_InsertString(hCtrl6, i, g_ComboBoxDevice[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl6, i, g_ComboBoxDevice[i].m_pData);
		}
	}
	
	if (hCtrl7)
	{
		for (i = 0; i < NUMDEVICES; i++)
		{
			(void)ComboBox_InsertString(hCtrl7, i, g_ComboBoxDevice[i].m_pText);
			(void)ComboBox_SetItemData(hCtrl7, i, g_ComboBoxDevice[i].m_pData);
		}
	}
}

static void InitializeBIOSUI(HWND hWnd)
{
	HWND hCtrl = GetDlgItem(hWnd, IDC_BIOS);
	int i;
	TCHAR* t_s;
	
	if (hCtrl)
	{
		const game_driver *gamedrv = &driver_list::driver(g_nGame);
		const rom_entry *rom;

		if (g_nGame == GLOBAL_OPTIONS)
		{
			(void)ComboBox_InsertString(hCtrl, i, TEXT("None"));
			(void)ComboBox_SetItemData(hCtrl, i++, "");
			return;
		}
		
		if (g_nGame == FOLDER_OPTIONS) 		//Folder Options
		{
			gamedrv = &driver_list::driver(g_nFolderGame);
			
			if (DriverHasOptionalBIOS(g_nFolderGame) == FALSE)
			{
				(void)ComboBox_InsertString(hCtrl, i, TEXT("None"));
				(void)ComboBox_SetItemData(hCtrl, i++, "");
				return;
			}
			
			(void)ComboBox_InsertString(hCtrl, i, TEXT("Default"));
			(void)ComboBox_SetItemData(hCtrl, i++, "");

			if (gamedrv->rom != NULL)
			{
				for (rom = gamedrv->rom; !ROMENTRY_ISEND(rom); rom++)
				{
					if (ROMENTRY_ISSYSTEM_BIOS(rom))
					{
						const char *name = ROM_GETHASHDATA(rom);
						const char *biosname = ROM_GETNAME(rom);
						t_s = tstring_from_utf8(name);
						
						if(!t_s)
							return;
						
						(void)ComboBox_InsertString(hCtrl, i, t_s);
						(void)ComboBox_SetItemData(hCtrl, i++, biosname);
						free(t_s);
					}
				}
			}
			
			return;
		}

		if (DriverHasOptionalBIOS(g_nGame) == FALSE)
		{
			(void)ComboBox_InsertString(hCtrl, i, TEXT("None"));
			(void)ComboBox_SetItemData(hCtrl, i++, "");
			return;
		}
		
		(void)ComboBox_InsertString(hCtrl, i, TEXT("Default"));
		(void)ComboBox_SetItemData(hCtrl, i++, "");

		if (gamedrv->rom != NULL)
		{
			for (rom = gamedrv->rom; !ROMENTRY_ISEND(rom); rom++)
			{
				if (ROMENTRY_ISSYSTEM_BIOS(rom))
				{
					const char *name = ROM_GETHASHDATA(rom);
					const char *biosname = ROM_GETNAME(rom);
					t_s = tstring_from_utf8(name);
					
					if(!t_s)
						return;
					
					(void)ComboBox_InsertString(hCtrl, i, t_s);
					(void)ComboBox_SetItemData(hCtrl, i++, biosname);
					free(t_s);
				}
			}
		}
	}
}

static void InitializeJoystickIDUI(HWND hWnd)
{
	int i, j;
	
	for (j = 0; j < 8; j++)
	{
		HWND hCtrl = GetDlgItem(hWnd, IDC_JOYSTICK_ID_1 + j);

		if (hCtrl)
		{
			for (i = 0; i < NUMJOYSTICKID; i++)
			{
				(void)ComboBox_InsertString(hCtrl, i, g_ComboBoxJoystickID[i].m_pText);
				(void)ComboBox_SetItemData(hCtrl, i, g_ComboBoxJoystickID[i].m_pData);
			}
		}
	}
}

static BOOL SelectEffect(HWND hWnd)
{
	char filename[MAX_PATH];
	BOOL changed = FALSE;
	char option[MAX_PATH];
	
	*filename = 0;
	
	if (CommonFileDialog(GetOpenFileName, filename, FILETYPE_EFFECT_FILES))
	{
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		PathRemoveExtension(tempname);
		char *optname = utf8_from_tstring(tempname);
		strcpy(option, optname);
		free(t_filename);
		free(optname);

		if (strcmp(option, pCurrentOpts.value(OPTION_EFFECT)))
		{
			std::string error_string;
			pCurrentOpts.set_value(OPTION_EFFECT, option, OPTION_PRIORITY_CMDLINE, error_string);
			assert(error_string.empty());
			win_set_window_text_utf8(GetDlgItem(hWnd, IDC_EFFECT), option);
			changed = TRUE;
		}
	}
	
	return changed;
}

static BOOL ResetEffect(HWND hWnd)
{
	BOOL changed = FALSE;
	const char *new_value = "none";

	if (strcmp(new_value, pCurrentOpts.value(OPTION_EFFECT)))
	{
		std::string error_string;
		pCurrentOpts.set_value(OPTION_EFFECT, new_value, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		win_set_window_text_utf8(GetDlgItem(hWnd, IDC_EFFECT), "None");
		changed = TRUE;
	}
	
	return changed;
}

static BOOL SelectMameShader(HWND hWnd, int slot)
{
	char filename[MAX_PATH];
	BOOL changed = FALSE;
	char option[MAX_PATH];
	char shader[32];
	int dialog = 0;
	
	*filename = 0;
	snprintf(shader, ARRAY_LENGTH(shader), "glsl_shader_mame%d", slot);
	dialog = IDC_MAME_SHADER0 + slot;
	
	if (CommonFileDialog(GetOpenFileName, filename, FILETYPE_SHADER_FILES))
	{
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		PathRemoveExtension(tempname);
		char *optname = utf8_from_tstring(tempname);
		strcpy(option, optname);
		free(t_filename);
		free(optname);

		if (strcmp(option, pCurrentOpts.value(shader)))
		{
			std::string error_string;
			pCurrentOpts.set_value(shader, option, OPTION_PRIORITY_CMDLINE, error_string);
			assert(error_string.empty());
			win_set_window_text_utf8(GetDlgItem(hWnd, dialog), option);
			changed = TRUE;
		}
	}
	
	return changed;
}

static BOOL ResetMameShader(HWND hWnd, int slot)
{
	BOOL changed = FALSE;
	const char *new_value = "none";
	char option[32];
	int dialog = 0;
	
	snprintf(option, ARRAY_LENGTH(option), "glsl_shader_mame%d", slot);
	dialog = IDC_MAME_SHADER0 + slot;
	
	if (strcmp(new_value, pCurrentOpts.value(option)))
	{
		std::string error_string;
		pCurrentOpts.set_value(option, new_value, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		win_set_window_text_utf8(GetDlgItem(hWnd, dialog), "None");
		changed = TRUE;
	}
	
	return changed;
}

static BOOL SelectScreenShader(HWND hWnd, int slot)
{
	char filename[MAX_PATH];
	BOOL changed = FALSE;
	char option[MAX_PATH];
	char shader[32];
	int dialog = 0;
	
	*filename = 0;
	snprintf(shader, ARRAY_LENGTH(shader), "glsl_shader_screen%d", slot);
	dialog = IDC_SCREEN_SHADER0 + slot;
	
	if (CommonFileDialog(GetOpenFileName, filename, FILETYPE_SHADER_FILES))
	{
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		PathRemoveExtension(tempname);
		char *optname = utf8_from_tstring(tempname);
		strcpy(option, optname);
		free(t_filename);
		free(optname);

		if (strcmp(option, pCurrentOpts.value(shader)))
		{
			std::string error_string;
			pCurrentOpts.set_value(shader, option, OPTION_PRIORITY_CMDLINE, error_string);
			assert(error_string.empty());
			win_set_window_text_utf8(GetDlgItem(hWnd, dialog), option);
			changed = TRUE;
		}
	}
	
	return changed;
}

static BOOL ResetScreenShader(HWND hWnd, int slot)
{
	BOOL changed = FALSE;
	const char *new_value = "none";
	char option[32];
	int dialog = 0;
	
	snprintf(option, ARRAY_LENGTH(option), "glsl_shader_screen%d", slot);
	dialog = IDC_SCREEN_SHADER0 + slot;
	
	if (strcmp(new_value, pCurrentOpts.value(option)))
	{
		std::string error_string;
		pCurrentOpts.set_value(option, new_value, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		win_set_window_text_utf8(GetDlgItem(hWnd, dialog), "None");
		changed = TRUE;
	}
	
	return changed;
}

static void UpdateMameShader(HWND hWnd, int slot, windows_options &opts)
{
	char option[32];
	HWND hCtrl;
	
	hCtrl = GetDlgItem(hWnd, IDC_MAME_SHADER0 + slot);

	if (hCtrl)
	{
		snprintf(option, ARRAY_LENGTH(option), "glsl_shader_mame%d", slot);
		const char* value = opts.value(option);
		
		if (strcmp(value, "none") == 0)
			win_set_window_text_utf8(hCtrl, "None");
		else
			win_set_window_text_utf8(hCtrl, value);
	}
}

static void UpdateScreenShader(HWND hWnd, int slot, windows_options &opts)
{
	char option[32];
	HWND hCtrl;
	
	hCtrl = GetDlgItem(hWnd, IDC_SCREEN_SHADER0 + slot);

	if (hCtrl)
	{
		snprintf(option, ARRAY_LENGTH(option), "glsl_shader_screen%d", slot);
		const char* value = opts.value(option);
		
		if (strcmp(value, "none") == 0)
			win_set_window_text_utf8(hCtrl, "None");
		else
			win_set_window_text_utf8(hCtrl, value);
	}
}

static BOOL SelectCheatFile(HWND hWnd)
{
	char filename[MAX_PATH];
	BOOL changed = FALSE;
	char optvalue[MAX_PATH];
	char option[MAX_PATH];

	*filename = 0;
	
	if (CommonFileDialog(GetOpenFileName, filename, FILETYPE_CHEAT_FILES))
	{
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *t_cheatopt = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		PathRemoveExtension(tempname);
		PathRemoveExtension(t_cheatopt);
		char *optname = utf8_from_tstring(tempname);
		char *cheatopt = utf8_from_tstring(t_cheatopt);
		strcpy(option, optname);
		strcpy(optvalue, cheatopt);
		free(t_filename);
		free(t_cheatopt);
		free(optname);
		free(cheatopt);
		
		if (strcmp(optvalue, pCurrentOpts.value(OPTION_CHEATPATH)))
		{
			std::string error_string;
			pCurrentOpts.set_value(OPTION_CHEATPATH, optvalue, OPTION_PRIORITY_CMDLINE, error_string);
			assert(error_string.empty());
			win_set_window_text_utf8(GetDlgItem(hWnd, IDC_CHEATFILE), option);
			changed = TRUE;
		}
	}
	
	return changed;
}

static BOOL ResetCheatFile(HWND hWnd)
{
	BOOL changed = FALSE;
	const char *new_value = "cheat";

	if (strcmp(new_value, pCurrentOpts.value(OPTION_CHEATPATH)))
	{
		std::string error_string;
		pCurrentOpts.set_value(OPTION_CHEATPATH, new_value, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		win_set_window_text_utf8(GetDlgItem(hWnd, IDC_CHEATFILE), "Default");
		changed = TRUE;
	}
	
	return changed;
}

static BOOL ChangeJoystickMap(HWND hWnd)
{
	BOOL changed = FALSE;
	char joymap[90];
	
	win_get_window_text_utf8(GetDlgItem(hWnd, IDC_JOYSTICKMAP), joymap, sizeof(joymap));

	if (strcmp(joymap, pCurrentOpts.value(OPTION_JOYSTICK_MAP)))
	{
		std::string error_string;
		pCurrentOpts.set_value(OPTION_JOYSTICK_MAP, joymap, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		changed = TRUE;
	}

	return changed;
}

static BOOL ResetJoystickMap(HWND hWnd)
{
	BOOL changed = FALSE;
	const char *new_value = "auto";

	if (strcmp(new_value, pCurrentOpts.value(OPTION_JOYSTICK_MAP)))
	{
		std::string error_string;
		pCurrentOpts.set_value(OPTION_JOYSTICK_MAP, new_value, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		win_set_window_text_utf8(GetDlgItem(hWnd, IDC_JOYSTICKMAP), new_value);
		changed = TRUE;
	}
	
	return changed;
}

static BOOL SelectDebugscript(HWND hWnd)
{
	char filename[MAX_PATH];
	BOOL changed = FALSE;
	char option[MAX_PATH];

	*filename = 0;
	
	if (CommonFileDialog(GetOpenFileName, filename, FILETYPE_DEBUGSCRIPT_FILES))
	{
		TCHAR *t_filename = tstring_from_utf8(filename);
		TCHAR *tempname = PathFindFileName(t_filename);
		PathRemoveExtension(tempname);
		char *optname = utf8_from_tstring(tempname);
		strcpy(option, optname);
		free(t_filename);
		free(optname);

		if (strcmp(filename, pCurrentOpts.value(OPTION_DEBUGSCRIPT)))
		{
			std::string error_string;
			pCurrentOpts.set_value(OPTION_DEBUGSCRIPT, filename, OPTION_PRIORITY_CMDLINE, error_string);
			assert(error_string.empty());
			win_set_window_text_utf8(GetDlgItem(hWnd, IDC_DEBUGSCRIPT), option);
			changed = TRUE;
		}
	}
	
	return changed;
}

static BOOL ResetDebugscript(HWND hWnd)
{
	BOOL changed = FALSE;
	const char *new_value = "";

	if (strcmp(new_value, pCurrentOpts.value(OPTION_DEBUGSCRIPT)))
	{
		std::string error_string;
		pCurrentOpts.set_value(OPTION_DEBUGSCRIPT, new_value, OPTION_PRIORITY_CMDLINE, error_string);
		assert(error_string.empty());
		win_set_window_text_utf8(GetDlgItem(hWnd, IDC_DEBUGSCRIPT), "None");
		changed = TRUE;
	}
	
	return changed;
}

//mamefx: for coloring of changed elements
static void DisableVisualStyles(HWND hDlg)
{
	/* Display */
	SetWindowTheme(GetDlgItem(hDlg, IDC_WINDOWED), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MAXIMIZE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_KEEPASPECT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_THROTTLE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FLIPY), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FLIPX), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_VIDEO_MODE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_D3D_FILTER), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_HWSTRETCH), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_WIDESTRETCH), L" ", L" ");
	/* Advanced */
	SetWindowTheme(GetDlgItem(hDlg, IDC_TRIPLE_BUFFER), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SYNCREFRESH), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_WAITVSYNC), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_REFRESHSPEED), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_HLSL_ON), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_AUTOFRAMESKIP), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FRAMESKIP), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_EFFECT), L" ", L" ");
	/* Screen */
	SetWindowTheme(GetDlgItem(hDlg, IDC_VIEW), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SIZES), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_REFRESH), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SWITCHRES), L" ", L" ");		
	/* OpenGL */
	SetWindowTheme(GetDlgItem(hDlg, IDC_GLSLPOW), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_GLSLTEXTURE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_GLSLVBO), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_GLSLPBO), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_GLSL), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_GLSLFILTER), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_GLSLSYNC), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MAME_SHADER0), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MAME_SHADER1), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MAME_SHADER2), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MAME_SHADER3), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MAME_SHADER4), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SCREEN_SHADER0), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SCREEN_SHADER1), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SCREEN_SHADER2), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SCREEN_SHADER3), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SCREEN_SHADER4), L" ", L" ");
	/* Sound */
	SetWindowTheme(GetDlgItem(hDlg, IDC_SOUND_MODE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SAMPLES), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SAMPLERATE), L" ", L" ");
	/* Controllers */
	SetWindowTheme(GetDlgItem(hDlg, IDC_USE_MOUSE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_STEADYKEY), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_DEFAULT_INPUT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_LIGHTGUN), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_RELOAD), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_DUAL_LIGHTGUN), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MULTIKEYBOARD), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MULTIMOUSE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICKMAP), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK_ID_1), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK_ID_2), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK_ID_3), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK_ID_4), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK_ID_5), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK_ID_6), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK_ID_7), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOYSTICK_ID_8), L" ", L" ");
	/* Controller Mapping */
	SetWindowTheme(GetDlgItem(hDlg, IDC_PADDLE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_ADSTICK), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_PEDAL), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MOUSE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_DIAL), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_TRACKBALL), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_LIGHTGUNDEVICE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_POSITIONAL), L" ", L" ");
	/* Miscellaneous */
	SetWindowTheme(GetDlgItem(hDlg, IDC_CHEAT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_HISCORE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SLEEP), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SKIP_GAME_INFO), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_ENABLE_AUTOSAVE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MULTITHREAD_RENDERING), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_COINLOCKOUT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_CONFIRM_QUIT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_BIOS), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_BACKDROPS), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_BEZELS), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_OVERLAYS), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_CPANELS), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_MARQUEES), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_ARTWORK_CROP), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_CHEATFILE), L" ", L" ");
	/* Snapshot/Movie */
	SetWindowTheme(GetDlgItem(hDlg, IDC_SNAPVIEW), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SNAPNAME), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SNAPBILINEAR), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SNAPBURNIN), L" ", L" ");
	/* Debug */
	SetWindowTheme(GetDlgItem(hDlg, IDC_OSLOG), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_LOG), L" ", L" ");		
	SetWindowTheme(GetDlgItem(hDlg, IDC_VERBOSE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_UPDATEINPAUSE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_DEBUG), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_DEBUGSCRIPT), L" ", L" ");
	/* Vector */
	SetWindowTheme(GetDlgItem(hDlg, IDC_ANTIALIAS), L" ", L" ");
	/* Not Working for now, disabled for future FIX!! */
	SetWindowTheme(GetDlgItem(hDlg, IDC_ROTATE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SCREEN), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_ASPECT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_ASPECTRATION), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_ASPECTRATIOD), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SCREENSELECT), L" ", L" ");	
	SetWindowTheme(GetDlgItem(hDlg, IDC_SNAPSIZE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SNAPSIZEWIDTH), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_SNAPSIZEHEIGHT), L" ", L" ");
}

/* End of source file */
