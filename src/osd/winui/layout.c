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

  layout.c

  MAME specific TreeView definitions (and maybe more in the future)

***************************************************************************/
// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commctrl.h>

// standard C headers
#include <stdio.h>  /* for sprintf */
#include <stdlib.h> /* For malloc and free */
#include <string.h>

// MAME/MAMEUI headers
#include "bitmask.h"
#include "treeview.h"
#include "emu.h"
#include "mui_util.h"
#include "resource.h"
#include "directories.h"
#include "mui_opts.h"
#include "splitters.h"
#include "mui_audit.h"
#include "winui.h"
#include "properties.h"

static BOOL FilterAvailable(int driver_index);

extern const FOLDERDATA g_folderData[] =
{
	{"All Games",       "allgames",          FOLDER_ALLGAMES,     IDI_FOLDER_ALLGAMES,	    0,             0,            NULL,                       NULL,                    TRUE },
	{"Available",       "available",         FOLDER_AVAILABLE,    IDI_FOLDER_AVAILABLE,     F_AVAILABLE,   0,            NULL,                       FilterAvailable,         TRUE },
	{"Unavailable",     "unavailable",       FOLDER_UNAVAILABLE,  IDI_FOLDER_UNAVAILABLE,   0,             F_AVAILABLE,  NULL,                       FilterAvailable,         FALSE },
	{"Manufacturer",    "manufacturer",      FOLDER_MANUFACTURER, IDI_FOLDER_MANUFACTURER,  0,             0,            CreateManufacturerFolders },
	{"Year",            "year",              FOLDER_YEAR,         IDI_FOLDER_YEAR,          0,             0,            CreateYearFolders },
	{"CPU",             "cpu",               FOLDER_CPU,          IDI_FOLDER_CPU,           0,             0,            CreateCPUFolders },
	{"SOUND",           "sound",             FOLDER_SOUND,        IDI_FOLDER_SOUND,         0,             0,            CreateSoundFolders },
	{"BIOS",	    	"bios",		     	 FOLDER_BIOS,	      IDI_FOLDER_BIOS,          0,             0,            CreateBIOSFolders, 	 	 DriverIsBios,  		  TRUE },
	{"CHD",             "harddisk",          FOLDER_HARDDISK,     IDI_HARDDISK,             0,             0,            NULL,                       DriverIsHarddisk,	  	  TRUE },
	{"Samples",    	  	"samples",           FOLDER_SAMPLES,      IDI_FOLDER_SAMPLES,       0,             0,            NULL,                       DriverUsesSamples,  	  TRUE },
	{"Mechanical",      "mechanical",        FOLDER_MECHANICAL,   IDI_MECHANICAL,           0,             0,            NULL,                       DriverIsMechanical,      TRUE },
	{"Lightgun",    	"lightgun",          FOLDER_LIGHTGUN,     IDI_FOLDER_LIGHTGUN,      0,             0,            NULL,                       DriverUsesLightGun,  	  TRUE },
	{"Trackball",    	"trackball",         FOLDER_TRACKBALL,    IDI_FOLDER_TRACKBALL,     0,             0,            NULL,                       DriverUsesTrackball,  	  TRUE },
	{"Working",         "working",           FOLDER_WORKING,      IDI_WORKING,              F_WORKING,     F_NONWORKING, NULL,                       DriverIsBroken,    	  FALSE },
	{"Imperfect",       "imperfect",         FOLDER_DEFICIENCY,   IDI_FOLDER_IMPERFECT,     0,             0,            CreateDeficiencyFolders },
	{"Not Working",     "nonworking",        FOLDER_NONWORKING,   IDI_NONWORKING,           F_NONWORKING,  F_WORKING,    NULL,                       DriverIsBroken,    	  TRUE },
	{"Originals",       "originals",         FOLDER_ORIGINAL,     IDI_FOLDER_ORIGINALS,     F_ORIGINALS,   F_CLONES,     NULL,                       DriverIsClone,     	  FALSE },
	{"Clones",          "clones",            FOLDER_CLONES,       IDI_FOLDER_CLONES,        F_CLONES,      F_ORIGINALS,  NULL,                       DriverIsClone,     	  TRUE },
	{"Horizontal",      "horizontal",        FOLDER_HORIZONTAL,   IDI_FOLDER_HORIZONTAL,    F_HORIZONTAL,  F_VERTICAL,   NULL,                       DriverIsVertical,        FALSE },
	{"Vertical",        "vertical",          FOLDER_VERTICAL,     IDI_FOLDER_VERTICAL,      F_VERTICAL,    F_HORIZONTAL, NULL,                       DriverIsVertical,        TRUE },
	{"Raster",          "raster",            FOLDER_RASTER,       IDI_FOLDER_RASTER,        F_RASTER,      F_VECTOR,     NULL,                       DriverIsVector,    	  FALSE },
	{"Vector",          "vector",            FOLDER_VECTOR,       IDI_FOLDER_VECTOR,        F_VECTOR,      F_RASTER,     NULL,                       DriverIsVector,    	  TRUE },
	{"Screens",         "screens",           FOLDER_SCREENS,      IDI_FOLDER_MONITOR,       0,             0,            CreateScreenFolders },
	{"Resolution",      "resolution",        FOLDER_RESOLUTION,   IDI_FOLDER_RESOL,         0,             0,            CreateResolutionFolders },
	{"Refresh",         "refresh",           FOLDER_FPS,          IDI_FOLDER_FPS,           0,             0,            CreateFPSFolders },
	{"Source",          "source",            FOLDER_SOURCE,       IDI_FOLDER_SOURCE,        0,             0,            CreateSourceFolders },
	{ NULL }
};

/* list of filter/control Id pairs */
extern const FILTER_ITEM g_filterList[] =
{
	{ F_CLONES,       IDC_FILTER_CLONES,      DriverIsClone, 		TRUE },
	{ F_NONWORKING,   IDC_FILTER_NONWORKING,  DriverIsBroken, 		TRUE },
	{ F_UNAVAILABLE,  IDC_FILTER_UNAVAILABLE, FilterAvailable, 		FALSE },
	{ F_RASTER,       IDC_FILTER_RASTER,      DriverIsVector, 		FALSE },
	{ F_VECTOR,       IDC_FILTER_VECTOR,      DriverIsVector, 		TRUE },
	{ F_ORIGINALS,    IDC_FILTER_ORIGINALS,   DriverIsClone, 		FALSE },
	{ F_WORKING,      IDC_FILTER_WORKING,     DriverIsBroken, 		FALSE },
	{ F_AVAILABLE,    IDC_FILTER_AVAILABLE,   FilterAvailable, 		TRUE },
	{ F_HORIZONTAL,   IDC_FILTER_HORIZONTAL,  DriverIsVertical, 	FALSE },
	{ F_VERTICAL,     IDC_FILTER_VERTICAL,    DriverIsVertical, 	TRUE },
	{ 0 }
};

extern const DIRECTORYINFO g_directoryInfo[] =
{
	{ "ROMs",                  GetRomDirs,      SetRomDirs,      TRUE,  DIRDLG_ROM },
	{ "Samples",               GetSampleDirs,   SetSampleDirs,   TRUE,  DIRDLG_SAM },
	{ "Ini files",             GetIniDir,       SetIniDir,       FALSE, DIRDLG_INI },
	{ "Config files",          GetCfgDir,       SetCfgDir,       FALSE, DIRDLG_CFG },
	{ "High scores",           GetHiDir,        SetHiDir,        FALSE, DIRDLG_HIS },
	{ "Snapshots",             GetImgDir,       SetImgDir,       FALSE, DIRDLG_IMG },
	{ "Input files",           GetInpDir,       SetInpDir,       FALSE, DIRDLG_INP },
	{ "State files",           GetStateDir,     SetStateDir,     FALSE, DIRDLG_STA },
	{ "Artwork files",         GetArtDir,       SetArtDir,       FALSE, DIRDLG_ART },
	{ "NVRAM files",           GetNvramDir,     SetNvramDir,     FALSE, DIRDLG_NVR },
	{ "Controller files",      GetCtrlrDir,     SetCtrlrDir,     FALSE, DIRDLG_CTL },
	{ "Crosshair files",       GetCrosshairDir, SetCrosshairDir, FALSE, DIRDLG_CRS },
	{ "HLSL files",            GetHLSLDir, 	  	SetHLSLDir, 	 FALSE, DIRDLG_HLS },
	{ "CHD Diff files",        GetDiffDir, 	  	SetDiffDir, 	 FALSE, DIRDLG_DIF },
	{ "GLSL shader files",     GetGLSLDir, 	  	SetGLSLDir, 	 FALSE, DIRDLG_GLS },
	{ "Font files",            GetFontDir,      SetFontDir,      FALSE, 0 },
	{ "Video files",           GetVideoDir,     SetVideoDir,     FALSE, 0 },
	{ "Audio files",           GetAudioDir,     SetAudioDir,     FALSE, 0 },
	{ "Flyers",                GetFlyerDir,     SetFlyerDir,     FALSE, 0 },
	{ "Cabinets",              GetCabinetDir,   SetCabinetDir,   FALSE, 0 },
	{ "Marquees",              GetMarqueeDir,   SetMarqueeDir,   FALSE, 0 },
	{ "Titles",                GetTitlesDir,    SetTitlesDir,    FALSE, 0 },
	{ "Control panels",        GetControlPanelDir,SetControlPanelDir, FALSE, 0 },
	{ "Scores snapshots",      GetScoresDir,    SetScoresDir,    FALSE, 0 },
	{ "PCBs",                  GetPcbDir,       SetPcbDir,       FALSE, 0 },
	{ "Folders",               GetFolderDir,    SetFolderDir,    FALSE, 0 },
	{ "Icons",                 GetIconsDir,     SetIconsDir,     FALSE, 0 },
	{ "Interface settings",    GetGuiDir,     	SetGuiDir,     	 FALSE, 0 },
	{ "Datafiles",             GetDatsDir,      SetDatsDir,      FALSE, 0 },
	{ NULL }
};

extern const SPLITTERINFO g_splitterInfo[] =
{
	{ 0.5,	IDC_SPLITTER,	IDC_TREE,	IDC_LIST,		AdjustSplitter1Rect },
	{ 0.5,	IDC_SPLITTER2,	IDC_LIST,	IDC_SSFRAME,	AdjustSplitter2Rect },
	{ -1 }
};

extern const PROPERTYSHEETINFO g_propSheets[] =
{
	{ FALSE,	NULL,					IDD_PROP_GAME,			GamePropertiesDialogProc },
	{ FALSE,	NULL,					IDD_PROP_AUDIT,			GameAuditDialogProc },
	{ TRUE,		NULL,					IDD_PROP_DISPLAY,		GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_ADVANCED,		GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_SCREEN,		GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_OPENGL,		GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_VECTOR,		GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_SOUND,			GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_INPUT,			GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_CONTROLLER,	GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_MISC,			GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_SNAP,			GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_DEBUG,			GameOptionsProc },
	{ FALSE }
};

extern const ICONDATA g_iconData[] =
{
	{ IDI_WIN_NOROMS,			"noroms" },
	{ IDI_WIN_ROMS,				"roms" },
	{ IDI_WIN_UNKNOWN,			"unknown" },
	{ IDI_WIN_CLONE,			"clone" },
	{ IDI_WIN_REDX,				"warning" },
	{ IDI_WIN_IMPERFECT,		"imperfect" },
	{ 0 }
};

static BOOL FilterAvailable(int driver_index)
{
	if (GetDisplayNoRomsGames())
	{
		if ((strcmp(driver_list::driver(driver_index).name, "pongf") == 0)
			|| (strcmp(driver_list::driver(driver_index).name, "pongd") == 0)
			|| (strcmp(driver_list::driver(driver_index).name, "breakout") == 0))
			return TRUE;
	}
	
	return IsAuditResultYes(GetRomAuditResults(driver_index));
}
