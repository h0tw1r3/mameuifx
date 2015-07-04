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

  mui_util.c

 ***************************************************************************/

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <vector>

// standard C headers
#include <stdio.h>
#include <tchar.h>

// MAME/MAMEUI headers
#include "emu.h"
#include "unzip.h"
#include "sound/samples.h"
#include "winutf8.h"
#include "strconv.h"
#include "winui.h"
#include "resource.h"
#include "mui_util.h"
#include "mui_opts.h"

#include <shlwapi.h>


/***************************************************************************
    Internal structures
 ***************************************************************************/
struct DriversInfo
{
	int screenCount;
	bool isClone;
	bool isBroken;
	bool isHarddisk;
	bool hasOptionalBIOS;
	bool isVector;
	bool usesRoms;
	bool usesSamples;
	bool usesTrackball;
	bool usesLightGun;
	bool supportsSaveState;
	bool isVertical;
	bool isImperfect;
};

static std::vector<DriversInfo>	drivers_info;

enum
{
	DRIVER_CACHE_SCREEN		= 0x000F,
	DRIVER_CACHE_ROMS		= 0x0010,
	DRIVER_CACHE_CLONE		= 0x0020,
	DRIVER_CACHE_BIOS		= 0x0040,
	DRIVER_CACHE_HARDDISK	= 0x0080,
	DRIVER_CACHE_SAMPLES	= 0x0100,
	DRIVER_CACHE_VECTOR		= 0x0200,
	DRIVER_CACHE_LIGHTGUN	= 0x0400,
	DRIVER_CACHE_TRACKBALL	= 0x0800,
};

static FILE *pFile = NULL;
static char buf[2048 * 2048];
static char tmp[40];
static bool bFirst = true;

/***************************************************************************
    External functions
 ***************************************************************************/

void __cdecl ErrorMsg(const char *fmt, ...)
{
	DWORD dwWritten;
	char buf[5000];
	char buf2[5000];
	va_list va;

	va_start(va, fmt);
	vsprintf(buf, fmt, va);
	win_message_box_utf8(GetActiveWindow(), buf, MAMEUINAME, MB_ICONERROR | MB_OK);
	strcpy(buf2, MAMEUINAME ": ");
	strcat(buf2,buf);
	strcat(buf2, "\n");
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf2, strlen(buf2), &dwWritten, NULL);

	if (pFile == NULL)
		pFile = fopen("debug.txt", "wt");

	if (pFile != NULL)
	{
		fprintf(pFile, "%s", buf2);
		fflush(pFile);
	}

	va_end(va);
}

void __cdecl dprintf(const char *fmt, ...)
{
	char buf[5000];
	va_list va;

	va_start(va, fmt);
	_vsnprintf(buf,sizeof(buf),fmt,va);
	win_output_debug_string_utf8(buf);
	va_end(va);
}

void ShellExecuteCommon(HWND hWnd, const char *cName)
{
	HINSTANCE hErr;
	const char *msg = NULL;
	LPTSTR tName;

	tName = tstring_from_utf8(cName);

	if(!tName)
		return;

	hErr = ShellExecute(hWnd, NULL, tName, NULL, NULL, SW_SHOWNORMAL);

	if ((FPTR)hErr > 32)
	{
		osd_free(tName);
		return;
	}

	switch((FPTR)hErr)
	{
	case 0:
		msg = "The Operating System is out of memory or resources.";
		break;

	case ERROR_FILE_NOT_FOUND:
		msg = "The specified file was not found.";
		break;

	case SE_ERR_NOASSOC :
		msg = "There is no application associated with the given filename extension.";
		break;

	case SE_ERR_OOM :
		msg = "There was not enough memory to complete the operation.";
		break;

	case SE_ERR_PNF :
		msg = "The specified path was not found.";
		break;

	case SE_ERR_SHARE :
		msg = "A sharing violation occurred.";
		break;

	default:
		msg = "Unknown error.";
	}

	win_message_box_utf8(hWnd, msg, MAMEUINAME, MB_ICONERROR | MB_OK);
	osd_free(tName);
}

char* MyStrStrI(const char* pFirst, const char* pSrch)
{
	char *cp = (char*)pFirst;
	char *s1;
	char *s2;

	while (*cp)
	{
		s1 = cp;
		s2 = (char*)pSrch;

		while (*s1 && *s2 && !core_strnicmp(s1, s2, 1))
			s1++, s2++;

		if (!*s2)
			return cp;

		cp++;
	}

	return NULL;
}

char * ConvertToWindowsNewlines(const char *source)
{
	char *dest;

	dest = buf;

	while (*source != 0)
	{
		if (*source == '\n')
		{
			*dest++ = '\r';
			*dest++ = '\n';
		}
		else
			*dest++ = *source;

		source++;
	}

	*dest = 0;
	return buf;
}

/* Lop off path and extention from a source file name
 * This assumes their is a pathname passed to the function
 * like src\drivers\blah.c
 */
const char * GetDriverFilename(int nIndex)
{
	const char *ptmp;
	const char *s = driver_list::driver(nIndex).source_file;
	const char *ptmp2;

	tmp[0] = '\0';

	ptmp = strrchr(s, '\\');

	if (ptmp == NULL) 
	{
		ptmp = strrchr(s, '/');
	}
	else 
	{
		ptmp2 = strrchr(ptmp, '/');

		if (ptmp2 != NULL) 
		{
			ptmp = ptmp2;
		}
	}
	
	if (ptmp == NULL)
		return s;

	ptmp++;
	strcpy(tmp,ptmp);
	return tmp;
}

int numberOfScreens(const machine_config *config)
{
	const screen_device *screen  = config->first_screen();
	screen_device_iterator iter(config->root_device());
	UINT8 i = 0;
	
	for (screen = iter.first(); screen != NULL; screen = iter.next())
		i++;
	
	return i;
}

static BOOL isDriverVector(const machine_config *config)
{
	const screen_device *screen  = config->first_screen();

	if (screen != NULL) 
	{
		// parse "vector.ini" for vector games
		if (SCREEN_TYPE_VECTOR == screen->screen_type())
			return TRUE;
	}

	return FALSE;
}

static void SetDriversInfo(void)
{
	int ndriver;
	int cache = -1;
	int total = driver_list::total();
	struct DriversInfo *gameinfo = NULL;

	for (ndriver = 0; ndriver < total; ndriver++)
	{
		gameinfo = &drivers_info[ndriver];
		cache    = (gameinfo->screenCount & DRIVER_CACHE_SCREEN);
		
		if (gameinfo->isClone)			
			cache += DRIVER_CACHE_CLONE;
		
		if (gameinfo->isHarddisk)		
			cache += DRIVER_CACHE_HARDDISK;
		
		if (gameinfo->hasOptionalBIOS)	
			cache += DRIVER_CACHE_BIOS;
		
		if (gameinfo->isVector)			
			cache += DRIVER_CACHE_VECTOR;
		
		if (gameinfo->usesRoms)			
			cache += DRIVER_CACHE_ROMS;
		
		if (gameinfo->usesSamples)		
			cache += DRIVER_CACHE_SAMPLES;
		
		if (gameinfo->usesTrackball)	
			cache += DRIVER_CACHE_TRACKBALL;
		
		if (gameinfo->usesLightGun)		
			cache += DRIVER_CACHE_LIGHTGUN;

		SetDriverCache(ndriver, cache);
	}
}

static void InitDriversInfo(void)
{
	int ndriver;
	int total = driver_list::total();

	for (ndriver = 0; ndriver < total; ndriver++)
	{
		const game_driver *gamedrv = &driver_list::driver(ndriver);
		struct DriversInfo *gameinfo = &drivers_info[ndriver];
		const rom_entry *region, *rom;
		machine_config config(*gamedrv, MameUIGlobal());

		gameinfo->isClone = (GetParentRomSetIndex(gamedrv) != -1);
		gameinfo->isBroken = ((gamedrv->flags & (GAME_NOT_WORKING | GAME_UNEMULATED_PROTECTION | GAME_MECHANICAL)) != 0);
		gameinfo->isImperfect = ((gamedrv->flags & (GAME_WRONG_COLORS | GAME_IMPERFECT_COLORS | GAME_IMPERFECT_GRAPHICS	| GAME_NO_SOUND | GAME_IMPERFECT_SOUND)) != 0);
 		gameinfo->supportsSaveState = ((gamedrv->flags & GAME_SUPPORTS_SAVE) != 0);
		gameinfo->isHarddisk = FALSE;
		gameinfo->isVertical = (gamedrv->flags & ORIENTATION_SWAP_XY) ? TRUE : FALSE;
		device_iterator deviter(config.root_device());
		
		for (device_t *device = deviter.first(); device != NULL; device = deviter.next())
			for (region = rom_first_region(*device); region; region = rom_next_region(region))
				if (ROMREGION_ISDISKDATA(region))
					gameinfo->isHarddisk = TRUE;

		gameinfo->hasOptionalBIOS = FALSE;
		
		if (gamedrv->rom != NULL)
			for (rom = gamedrv->rom; !ROMENTRY_ISEND(rom); rom++)
				if (ROMENTRY_ISSYSTEM_BIOS(rom))
					gameinfo->hasOptionalBIOS = TRUE;
				
		gameinfo->screenCount = numberOfScreens(&config);
		gameinfo->isVector = isDriverVector(&config);
		gameinfo->usesRoms = FALSE;
		
		for (device_t *device = deviter.first(); device != NULL; device = deviter.next())
			for (region = rom_first_region(*device); region; region = rom_next_region(region))
				for (rom = rom_first_file(region); rom; rom = rom_next_file(rom))
					gameinfo->usesRoms = TRUE;

		gameinfo->usesSamples = FALSE;
		samples_device_iterator iter(config.root_device());
		
		if (iter.first() != NULL)
			gameinfo->usesSamples = TRUE;

		gameinfo->usesTrackball = FALSE;
		gameinfo->usesLightGun = FALSE;
		
		if (gamedrv->ipt != NULL)
		{
			ioport_port *port;
			ioport_list portlist;
			std::string errors;
			device_iterator iter(config.root_device());
			
			for (device_t *cfg = iter.first(); cfg; cfg = iter.next())
				if (cfg->input_ports())
					portlist.append(*cfg, errors);

			for (port = portlist.first(); port; port = port->next())
			{
				ioport_field *field;
				
				for (field = port->first_field(); field; field = field->next())
				{
					UINT32 type;
					type = field->type();
					
					if (type == IPT_END)
						break;
					
					if (type == IPT_DIAL || type == IPT_PADDLE || 
						type == IPT_TRACKBALL_X || type == IPT_TRACKBALL_Y ||
						type == IPT_AD_STICK_X || type == IPT_AD_STICK_Y)
						gameinfo->usesTrackball = TRUE;
						
					if (type == IPT_LIGHTGUN_X || type == IPT_LIGHTGUN_Y)
						gameinfo->usesLightGun = TRUE;
				}
			}
		}
	}

	SetDriversInfo();
}

static int InitDriversCache(void)
{
	int cache = -1;
	int total = driver_list::total();
	const game_driver *gamedrv = NULL;
	struct DriversInfo *gameinfo = NULL;
	int ndriver;

	if (RequiredDriverCache())
	{
		InitDriversInfo();
		return 0;
	}

	for (ndriver = 0; ndriver < total; ndriver++)
	{
		gamedrv = &driver_list::driver(ndriver);
		gameinfo = &drivers_info[ndriver];
		cache = GetDriverCache(ndriver);

		if (cache == -1)
		{
			InitDriversInfo();
			break;
		}

		gameinfo->isBroken  = ((gamedrv->flags & (GAME_NOT_WORKING | GAME_UNEMULATED_PROTECTION | GAME_MECHANICAL)) != 0);
		gameinfo->supportsSaveState = ((gamedrv->flags & GAME_SUPPORTS_SAVE) != 0);
		gameinfo->isVertical = ((gamedrv->flags & ORIENTATION_SWAP_XY) ? TRUE : FALSE);
		gameinfo->screenCount = (cache & DRIVER_CACHE_SCREEN);
		gameinfo->isClone = ((cache & DRIVER_CACHE_CLONE)     != 0);
		gameinfo->isHarddisk = ((cache & DRIVER_CACHE_HARDDISK)  != 0);
		gameinfo->hasOptionalBIOS = ((cache & DRIVER_CACHE_BIOS)      != 0);
		gameinfo->isVector = ((cache & DRIVER_CACHE_VECTOR)    != 0);
		gameinfo->usesRoms = ((cache & DRIVER_CACHE_ROMS)      != 0);
		gameinfo->usesSamples = ((cache & DRIVER_CACHE_SAMPLES)   != 0);
		gameinfo->usesTrackball = ((cache & DRIVER_CACHE_TRACKBALL) != 0);
		gameinfo->usesLightGun = ((cache & DRIVER_CACHE_LIGHTGUN)  != 0);
		gameinfo->isImperfect = ((gamedrv->flags & (GAME_WRONG_COLORS | GAME_IMPERFECT_COLORS | GAME_IMPERFECT_GRAPHICS	| GAME_NO_SOUND | GAME_IMPERFECT_SOUND)) != 0);
	}

	return 0;
}

static struct DriversInfo* GetDriversInfo(int driver_index)
{
	if (bFirst)
	{
		bFirst = false;
		drivers_info.clear();
		drivers_info.reserve(driver_list::total());
		InitDriversCache();
	}

	return &drivers_info[driver_index];
}

BOOL DriverIsClone(int driver_index)
{
	return GetDriversInfo(driver_index)->isClone;
}

BOOL DriverIsBroken(int driver_index)
{
	return GetDriversInfo(driver_index)->isBroken;
}

BOOL DriverIsHarddisk(int driver_index)
{
	return GetDriversInfo(driver_index)->isHarddisk;
}

BOOL DriverIsBios(int driver_index)
{
	BOOL bBios = FALSE;

	if(!((driver_list::driver(driver_index).flags & GAME_IS_BIOS_ROOT ) == 0))
		bBios = TRUE;

	return bBios;
}

BOOL DriverIsMechanical(int driver_index)
{
	BOOL bMechanical = FALSE;

	if(!((driver_list::driver(driver_index).flags & GAME_MECHANICAL ) == 0))
		bMechanical = TRUE;

	return bMechanical;
}

BOOL DriverHasOptionalBIOS(int driver_index)
{
	return GetDriversInfo(driver_index)->hasOptionalBIOS;
}

int DriverNumScreens(int driver_index)
{
	return GetDriversInfo(driver_index)->screenCount;
}

BOOL DriverIsVector(int driver_index)
{
	return GetDriversInfo(driver_index)->isVector;
}

BOOL DriverUsesRoms(int driver_index)
{
	return GetDriversInfo(driver_index)->usesRoms;
}

BOOL DriverUsesSamples(int driver_index)
{
	return GetDriversInfo(driver_index)->usesSamples;
}

BOOL DriverUsesTrackball(int driver_index)
{
	return GetDriversInfo(driver_index)->usesTrackball;
}

BOOL DriverUsesLightGun(int driver_index)
{
	return GetDriversInfo(driver_index)->usesLightGun;
}

BOOL DriverSupportsSaveState(int driver_index)
{
	return GetDriversInfo(driver_index)->supportsSaveState;
}

BOOL DriverIsVertical(int driver_index)
{
	return GetDriversInfo(driver_index)->isVertical;
}

BOOL DriverIsImperfect(int driver_index)
{
	return GetDriversInfo(driver_index)->isImperfect;
}

void FlushFileCaches(void)
{
	zip_file_cache_clear();
}

BOOL StringIsSuffixedBy(const char *s, const char *suffix)
{
	return (strlen(s) > strlen(suffix)) && (strcmp(s + strlen(s) - strlen(suffix), suffix) == 0);
}

/***************************************************************************
    Win32 wrappers
 ***************************************************************************/

void GetSystemErrorMessage(DWORD dwErrorId, TCHAR **tErrorMessage)
{
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrorId, 0, (LPTSTR)tErrorMessage, 0, NULL) == 0)
	{
		*tErrorMessage = (LPTSTR)LocalAlloc(LPTR, MAX_PATH * sizeof(TCHAR));
		_tcscpy(*tErrorMessage, TEXT("Unknown Error"));
	}
}

//============================================================
//  win_extract_icon_utf8
//============================================================

HICON win_extract_icon_utf8(HINSTANCE inst, const char* exefilename, UINT iconindex)
{
	HICON icon = 0;
	TCHAR *t_exefilename = tstring_from_utf8(exefilename);

	if(!t_exefilename)
		return icon;

	icon = ExtractIcon(inst, t_exefilename, iconindex);
	osd_free(t_exefilename);
	return icon;
}

//============================================================
//  win_tstring_strdup
//============================================================

TCHAR * win_tstring_strdup(LPCTSTR str)
{
	TCHAR *cpy = NULL;
	
	if (str != NULL)
	{
		cpy = (TCHAR*)osd_malloc((_tcslen(str) + 1) * sizeof(TCHAR));

		if (cpy != NULL)
			_tcscpy(cpy, str);
	}

	return cpy;
}

//============================================================
//  win_find_first_file_utf8
//============================================================

HANDLE win_find_first_file_utf8(const char* filename, LPWIN32_FIND_DATA findfiledata)
{
	HANDLE result = 0;
	TCHAR *t_filename = tstring_from_utf8(filename);
	
	if(!t_filename)
		return result;

	result = FindFirstFile(t_filename, findfiledata);
	osd_free(t_filename);
	return result;
}

void CenterWindow(HWND hWnd)
{
	HWND hWndParent;
	RECT rcCenter, rcWnd;
	int iWndWidth, iWndHeight, iScrWidth, iScrHeight, xLeft, yTop;

	hWndParent = GetParent(hWnd);
	GetWindowRect(hWnd, &rcWnd);
	iWndWidth  = rcWnd.right - rcWnd.left;
	iWndHeight = rcWnd.bottom - rcWnd.top;

	if (hWndParent != NULL)
	{
		GetWindowRect(hWndParent, &rcCenter);
	}
	else
	{
		rcCenter.left = 0;
		rcCenter.top = 0;
		rcCenter.right = GetSystemMetrics(SM_CXFULLSCREEN);
		rcCenter.bottom = GetSystemMetrics(SM_CYFULLSCREEN);
	}

	iScrWidth  = rcCenter.right - rcCenter.left;
	iScrHeight = rcCenter.bottom - rcCenter.top;
	xLeft = rcCenter.left;
	yTop = rcCenter.top;

	if (iScrWidth > iWndWidth)
		xLeft += ((iScrWidth - iWndWidth) / 2);
	
	if (iScrHeight > iWndHeight)
		yTop += ((iScrHeight - iWndHeight) / 2);

	// map screen coordinates to child coordinates
	SetWindowPos(hWnd, HWND_TOP, xLeft, yTop, -1, -1, SWP_NOSIZE);
}

BOOL IsWindowsSevenOrHigher(void) 
{
	OSVERSIONINFO osvi;
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   
	GetVersionEx(&osvi);
   
	if ((osvi.dwMajorVersion >= 6) && (osvi.dwMinorVersion >= 1))
		return TRUE;
		
	return FALSE;
}
