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

  treeview.c

  TreeView support routines - MSH 11/19/1998

***************************************************************************/

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commctrl.h>

// standard C headers
#include <stdio.h>  // for sprintf
#include <stdlib.h> // For malloc and free
#include <ctype.h> // For tolower
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tchar.h>
#include <io.h>

// MAME/MAMEUI headers
#include "emu.h"
#include "hash.h"
#include "mui_util.h"
#include "bitmask.h"
#include "winui.h"
#include "treeview.h"
#include "resource.h"
#include "mui_opts.h"
#include "dialogs.h"
#include "winutf8.h"
#include "strconv.h"

#define ICON_MAX ARRAY_LENGTH(treeIconNames)

/***************************************************************************
    public structures
 ***************************************************************************/

/* Name used for user-defined custom icons */
/* external *.ico file to look for. */

typedef struct
{
	int		nResourceID;
	LPCSTR	lpName;
} TREEICON;

static TREEICON treeIconNames[] =
{
	{ IDI_FOLDER_OPEN,         	"foldopen" },
	{ IDI_FOLDER,              	"folder" },
	{ IDI_FOLDER_AVAILABLE,    	"foldavail" },
	{ IDI_FOLDER_MANUFACTURER, 	"foldmanu" },
	{ IDI_FOLDER_UNAVAILABLE,  	"foldunav" },
	{ IDI_FOLDER_YEAR,         	"foldyear" },
	{ IDI_FOLDER_SOURCE,       	"foldsrc" },
	{ IDI_FOLDER_HORIZONTAL,   	"horz" },
	{ IDI_FOLDER_VERTICAL,     	"vert" },
	{ IDI_MANUFACTURER,        	"manufact" },
	{ IDI_WORKING,             	"working" },
	{ IDI_NONWORKING,          	"nonwork" },
	{ IDI_YEAR,                	"year" },
	{ IDI_SOUND,               	"sound" },
	{ IDI_CHIP,                	"chip" },
	{ IDI_HARDDISK,            	"harddisk" },
	{ IDI_MECHANICAL,           "mechanical" },
    { IDI_SOURCE,              	"source" },
    { IDI_MONITOR,             	"monitor" },
    { IDI_FOLDER_IMPERFECT,    	"foldimp" },
    { IDI_FOLDER_CPU,          	"foldcsb" },
    { IDI_FOLDER_SOUND,        	"foldcsb" },
    { IDI_FOLDER_BIOS,         	"foldcsb" },
    { IDI_FOLDER_ALLGAMES,     	"allgames" },
    { IDI_FOLDER_CLONES,       	"foldclone" },
    { IDI_FOLDER_ORIGINALS,    	"foldorig" },
    { IDI_FOLDER_RASTER,       	"foldrast" },
    { IDI_FOLDER_VECTOR,       	"foldvect" },
    { IDI_FOLDER_MONITOR,      	"multimon" },
    { IDI_FOLDER_FPS,      	   	"foldfps" },
    { IDI_FOLDER_RESOL,        	"foldres" },
    { IDI_FOLDER_LIGHTGUN,      "foldgun" },
    { IDI_FOLDER_TRACKBALL,     "foldball" },
    { IDI_FOLDER_SAMPLES,      	"foldsamp" }
};

/***************************************************************************
    private variables
 ***************************************************************************/

/* this has an entry for every folder eventually in the UI, including subfolders */
static TREEFOLDER **treeFolders = 0;
static UINT numFolders  = 0;        		/* Number of folder in the folder array */
static UINT next_folder_id = MAX_FOLDERS;
static UINT folderArrayLength = 0;  		/* Size of the folder array */
static LPTREEFOLDER lpCurrentFolder = 0;    /* Currently selected folder */
static UINT nCurrentFolder = 0;     		/* Current folder ID */
static WNDPROC g_lpTreeWndProc = 0;   		/* for subclassing the TreeView */
static HIMAGELIST hTreeSmall = 0;         	/* TreeView Image list of icons */
/* this only has an entry for each TOP LEVEL extra folder + SubFolders*/
LPEXFOLDERDATA	ExtraFolderData[MAX_EXTRA_FOLDERS * MAX_EXTRA_SUBFOLDERS];
static int numExtraFolders = 0;
static int numExtraIcons = 0;
static char *ExtraFolderIcons[MAX_EXTRA_FOLDERS];
// built in folders and filters
static LPCFOLDERDATA  g_lpFolderData;
static LPCFILTER_ITEM g_lpFilterList;

/***************************************************************************
    private function prototypes
 ***************************************************************************/

extern BOOL	InitFolders(void);
static BOOL CreateTreeIcons(void);
static void	CreateAllChildFolders(void);
static BOOL AddFolder(LPTREEFOLDER lpFolder);
static LPTREEFOLDER NewFolder(const char *lpTitle, UINT nFolderId, int nParent, UINT nIconId, DWORD dwFlags);
static void DeleteFolder(LPTREEFOLDER lpFolder);
static LRESULT CALLBACK TreeWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static int InitExtraFolders(void);
static void FreeExtraFolders(void);
static void SetExtraIcons(char *name, int *id);
static BOOL TryAddExtraFolderAndChildren(int parent_index);
static BOOL TrySaveExtraFolder(LPTREEFOLDER lpFolder);
static void LoadExternalFolders(int parent_index, const char *fname, int id);

/***************************************************************************
    public functions
 ***************************************************************************/

/* De-allocate all folder memory */
void FreeFolders(void)
{
	int i = 0;

	if (treeFolders != NULL)
	{
		if (numExtraFolders)
		{
			FreeExtraFolders();
			numFolders -= numExtraFolders;
		}

		for (i = numFolders - 1; i >= 0; i--)
		{
			DeleteFolder(treeFolders[i]);
			treeFolders[i] = NULL;
			numFolders--;
		}
		
		osd_free(treeFolders);
		treeFolders = NULL;
	}
	
	numFolders = 0;
}

/* Reset folder filters */
void ResetFilters(void)
{
	int i = 0;

	if (treeFolders != 0)
	{
		for (i = 0; i < (int)numFolders; i++)
		{
			treeFolders[i]->m_dwFlags &= ~F_MASK;
		}
	}
}

void InitTree(LPCFOLDERDATA lpFolderData, LPCFILTER_ITEM lpFilterList)
{
	LONG_PTR l;

	g_lpFolderData = lpFolderData;
	g_lpFilterList = lpFilterList;

	InitFolders();
	/* this will subclass the treeview (where WM_DRAWITEM gets sent for
    the header control) */
	l = GetWindowLongPtr(GetTreeView(), GWLP_WNDPROC);
	g_lpTreeWndProc = (WNDPROC)l;
	SetWindowLongPtr(GetTreeView(), GWLP_WNDPROC, (LONG_PTR)TreeWndProc);
}

void SetCurrentFolder(LPTREEFOLDER lpFolder)
{
	lpCurrentFolder = (lpFolder == 0) ? treeFolders[0] : lpFolder;
	nCurrentFolder = (lpCurrentFolder) ? lpCurrentFolder->m_nFolderId : 0;
}

LPTREEFOLDER GetCurrentFolder(void)
{
	return lpCurrentFolder;
}

UINT GetCurrentFolderID(void)
{
	return nCurrentFolder;
}

int GetNumFolders(void)
{
	return numFolders;
}

LPTREEFOLDER GetFolder(UINT nFolder)
{
	return (nFolder < numFolders) ? treeFolders[nFolder] : NULL;
}

LPTREEFOLDER GetFolderByID(UINT nID)
{
	UINT i;

	for (i = 0; i < numFolders; i++)
	{
		if (treeFolders[i]->m_nFolderId == nID)
		{
			return treeFolders[i];
		}
	}

	return (LPTREEFOLDER)0;
}

void AddGame(LPTREEFOLDER lpFolder, UINT nGame)
{
	if (lpFolder)
		SetBit(lpFolder->m_lpGameBits, nGame);
}

void RemoveGame(LPTREEFOLDER lpFolder, UINT nGame)
{
	ClearBit(lpFolder->m_lpGameBits, nGame);
}

int FindGame(LPTREEFOLDER lpFolder, int nGame)
{
	return FindBit(lpFolder->m_lpGameBits, nGame, TRUE);
}

// Called to re-associate games with folders
void ResetWhichGamesInFolders(void)
{
	UINT i, jj, k;
	BOOL b;
	int nGames = driver_list::total();

	for (i = 0; i < numFolders; i++)
	{
		LPTREEFOLDER lpFolder = treeFolders[i];
		// setup the games in our built-in folders
		for (k = 0; g_lpFolderData[k].m_lpTitle; k++)
		{
			if (lpFolder->m_nFolderId == g_lpFolderData[k].m_nFolderId)
			{
				if (g_lpFolderData[k].m_pfnQuery || g_lpFolderData[k].m_bExpectedResult)
				{
					SetAllBits(lpFolder->m_lpGameBits, FALSE);
					
					for (jj = 0; jj < nGames; jj++)
					{
						// invoke the query function
						b = g_lpFolderData[k].m_pfnQuery ? g_lpFolderData[k].m_pfnQuery(jj) : TRUE;
						// if we expect FALSE, flip the result
						if (!g_lpFolderData[k].m_bExpectedResult)
							b = !b;
						// if we like what we hear, add the game
						if (b)
							AddGame(lpFolder, jj);
					}
				}

				break;
			}
		}
	}
}

/* Used to build the GameList */
BOOL GameFiltered(int nGame, DWORD dwMask)
{
	int i;
	LPTREEFOLDER lpFolder = GetCurrentFolder();
	LPTREEFOLDER lpParent = NULL;

	//Filter out the Bioses on all Folders, except for the Bios Folder
	if(lpFolder->m_nFolderId != FOLDER_BIOS)
	{
		if(DriverIsBios(nGame))
      		return TRUE;
	}

	if(driver_list::driver(nGame).name[0] == '_')
		return TRUE;

	// Filter games--return TRUE if the game should be HIDDEN in this view
	if(GetFilterInherit())
	{
		if(lpFolder)
		{
			lpParent = GetFolder( lpFolder->m_nParent );
			if(lpParent)
			{
				/* Check the Parent Filters and inherit them on child,
                * The inherited filters don't display on the custom Filter Dialog for the Child folder
                * No need to promote all games to parent folder, works as is */
				dwMask |= lpParent->m_dwFlags;
			}
		}
	}

	if (strlen(GetSearchText()) && _stricmp(GetSearchText(), SEARCH_PROMPT))
	{
		if (MyStrStrI(driver_list::driver(nGame).description,GetSearchText()) == NULL &&
			MyStrStrI(driver_list::driver(nGame).name,GetSearchText()) == NULL)
			return TRUE;
	}
	
	/*Filter Text is already global*/
	if (MyStrStrI(driver_list::driver(nGame).description,GetFilterText()) == NULL &&
		MyStrStrI(driver_list::driver(nGame).name,GetFilterText()) == NULL &&
		MyStrStrI(driver_list::driver(nGame).source_file,GetFilterText()) == NULL &&
		MyStrStrI(driver_list::driver(nGame).manufacturer,GetFilterText()) == NULL)
		return TRUE;

	// Are there filters set on this folder?
	if ((dwMask & F_MASK) == 0)
		return FALSE;

	// Filter out clones?
	if (dwMask & F_CLONES && DriverIsClone(nGame))
		return TRUE;

	for (i = 0; g_lpFilterList[i].m_dwFilterType; i++)
	{
		if (dwMask & g_lpFilterList[i].m_dwFilterType)
		{
			if (g_lpFilterList[i].m_pfnQuery(nGame) == g_lpFilterList[i].m_bExpectedResult)
				return TRUE;
		}
	}

	return FALSE;
}

/* Get the parent of game in this view */
BOOL GetParentFound(int nGame) 	// not used
{
	int nParentIndex = -1;
	LPTREEFOLDER lpFolder = GetCurrentFolder();

	if( lpFolder )
	{
		nParentIndex = GetParentIndex(&driver_list::driver(nGame));

		/* return FALSE if no parent is there in this view */
		if( nParentIndex == -1)
			return FALSE;

		/* return FALSE if the folder should be HIDDEN in this view */
		if (TestBit(lpFolder->m_lpGameBits, nParentIndex) == 0)
			return FALSE;

		/* return FALSE if the game should be HIDDEN in this view */
		if (GameFiltered(nParentIndex, lpFolder->m_dwFlags))
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

LPCFILTER_ITEM GetFilterList(void)
{
	return g_lpFilterList;
}

/***************************************************************************
    private functions
 ***************************************************************************/

void CreateSourceFolders(int parent_index)
{
	int i, jj, k = 0;
	int nGames = driver_list::total();
	int start_folder = numFolders;
	LPTREEFOLDER lpFolder = treeFolders[parent_index];
	LPTREEFOLDER lpTemp;

	// no games in top level folder
	SetAllBits(lpFolder->m_lpGameBits, FALSE);
	
	for (jj = 0; jj < nGames; jj++)
	{
		const char *s = GetDriverFilename(jj);

		if (s == NULL || s[0] == '\0')
			continue;

		// look for an existant source treefolder for this game
		// (likely to be the previous one, so start at the end)
		for (i = numFolders - 1; i >= start_folder; i--)
		{
			if (strcmp(treeFolders[i]->m_lpTitle, s) == 0)
			{
				AddGame(treeFolders[i], jj);
				break;
			}
		}

		if (i == start_folder - 1)
		{
			// nope, it's a source file we haven't seen before, make it.
			lpTemp = NewFolder(s, next_folder_id++, parent_index, IDI_SOURCE, GetFolderFlags(numFolders));
			AddFolder(lpTemp);
			AddGame(lpTemp, jj);
		}
	}

	SetNumOptionFolders(k - 1);
}

void CreateManufacturerFolders(int parent_index)
{
	int i, jj;
	int nGames = driver_list::total();
	int start_folder = numFolders;
	LPTREEFOLDER lpFolder = treeFolders[parent_index];
	LPTREEFOLDER lpTemp;

	// no games in top level folder
	SetAllBits(lpFolder->m_lpGameBits, FALSE);

	for (jj = 0; jj < nGames; jj++)
	{
		const char *s = driver_list::driver(jj).manufacturer;

		if (s == NULL || s[0] == '\0')
			continue;

		// look for an existant manufacturer treefolder for this game
		// (likely to be the previous one, so start at the end)
		for (i = numFolders - 1; i >= start_folder; i--)
		{
			if (strcmp(treeFolders[i]->m_lpTitle, s) == 0)
			{
				AddGame(treeFolders[i], jj);
				break;
			}
		}
		if (i == start_folder - 1)
		{
			// nope, it's a manufacturer we haven't seen before, make it.
			lpTemp = NewFolder(s, next_folder_id++, parent_index, IDI_MANUFACTURER, GetFolderFlags(numFolders));
			AddFolder(lpTemp);
			AddGame(lpTemp, jj);
		}
	}
}

void CreateDeficiencyFolders(int parent_index)
{
	int jj;
	int nGames = driver_list::total();
	LPTREEFOLDER lpFolder = treeFolders[parent_index];
	LPTREEFOLDER lpProt, lpWrongCol, lpImpCol, lpImpGraph, lpMissSnd, lpImpSnd;

	// create our subfolders
	lpProt = NewFolder("Unemulated Protection", next_folder_id++, parent_index, IDI_FOLDER, GetFolderFlags(numFolders));
	lpWrongCol = NewFolder("Wrong Colors", next_folder_id++, parent_index, IDI_FOLDER, GetFolderFlags(numFolders));
	lpImpCol = NewFolder("Imperfect Colors", next_folder_id++, parent_index, IDI_FOLDER, GetFolderFlags(numFolders));
	lpImpGraph = NewFolder("Imperfect Graphics", next_folder_id++, parent_index, IDI_FOLDER, GetFolderFlags(numFolders));
	lpMissSnd = NewFolder("Missing Sound", next_folder_id++, parent_index, IDI_FOLDER, GetFolderFlags(numFolders));
	lpImpSnd = NewFolder("Imperfect Sound", next_folder_id++, parent_index, IDI_FOLDER, GetFolderFlags(numFolders));

	AddFolder(lpProt);
	AddFolder(lpWrongCol);
	AddFolder(lpImpCol);
	AddFolder(lpImpGraph);
	AddFolder(lpMissSnd);
	AddFolder(lpImpSnd);

	// no games in top level folder
	SetAllBits(lpFolder->m_lpGameBits, FALSE);

	for (jj = 0; jj < nGames; jj++)
	{
		if (driver_list::driver(jj).flags & GAME_WRONG_COLORS)
		{
			AddGame(lpWrongCol, jj);
		}
		
		if (driver_list::driver(jj).flags & GAME_UNEMULATED_PROTECTION)
		{
			AddGame(lpProt, jj);
		}
		
		if (driver_list::driver(jj).flags & GAME_IMPERFECT_COLORS)
		{
			AddGame(lpImpCol, jj);
		}
		
		if (driver_list::driver(jj).flags & GAME_IMPERFECT_GRAPHICS)
		{
			AddGame(lpImpGraph, jj);
		}
		
		if (driver_list::driver(jj).flags & GAME_NO_SOUND)
		{
			AddGame(lpMissSnd, jj);
		}
		
		if (driver_list::driver(jj).flags & GAME_IMPERFECT_SOUND)
		{
			AddGame(lpImpSnd, jj);
		}
	}
}

void CreateYearFolders(int parent_index)
{
	int i, jj;
	int nGames = driver_list::total();
	int start_folder = numFolders;
	LPTREEFOLDER lpFolder = treeFolders[parent_index];
	LPTREEFOLDER lpTemp;

	// no games in top level folder
	SetAllBits(lpFolder->m_lpGameBits, FALSE);

	for (jj = 0; jj < nGames; jj++)
	{
		const char *s = driver_list::driver(jj).year;

		if (s == NULL || s[0] == '\0')
			continue;

		// look for an extant year treefolder for this game
		// (likely to be the previous one, so start at the end)
		for (i = numFolders - 1; i >= start_folder; i--)
		{
			if (strcmp(treeFolders[i]->m_lpTitle, s) == 0)
			{
				AddGame(treeFolders[i], jj);
				break;
			}
		}
		if (i == start_folder - 1)
		{
			// nope, it's a year we haven't seen before, make it.
			lpTemp = NewFolder(s, next_folder_id++, parent_index, IDI_YEAR, GetFolderFlags(numFolders));
			AddFolder(lpTemp);
			AddGame(lpTemp, jj);
		}
	}
}

void CreateBIOSFolders(int parent_index)
{
	int i, jj;
	int nGames = driver_list::total();
	int start_folder = numFolders;
	const game_driver *drv;
	int nParentIndex = -1;
	LPTREEFOLDER lpFolder = treeFolders[parent_index];
	LPTREEFOLDER lpTemp;

	// no games in top level folder
	SetAllBits(lpFolder->m_lpGameBits, FALSE);

	for (jj = 0; jj < nGames; jj++)
	{

		if (DriverIsClone(jj))
		{
			nParentIndex = GetParentIndex(&driver_list::driver(jj));
			
			if (nParentIndex < 0) 
				return;
			
			drv = &driver_list::driver(nParentIndex);
		}
		else
			drv = &driver_list::driver(jj);

		nParentIndex = GetParentIndex(drv);

		if (nParentIndex < 0 || !driver_list::driver(nParentIndex).description)
			continue;

		for (i = numFolders - 1; i >= start_folder; i--)
		{
			if (strcmp(treeFolders[i]->m_lpTitle, driver_list::driver(nParentIndex).description) == 0)
			{
				AddGame(treeFolders[i], jj);
				break;
			}
		}

		if (i == start_folder - 1)
		{
			lpTemp = NewFolder(driver_list::driver(nParentIndex).description, next_folder_id++, parent_index, IDI_CHIP, GetFolderFlags(numFolders));
			AddFolder(lpTemp);
			AddGame(lpTemp, jj);
		}
	}
}

void CreateCPUFolders(int parent_index)
{
    std::string fname = std::string(GetGuiDir()).append(PATH_SEPARATOR).append("cpu.ini");
	LoadExternalFolders(parent_index, fname.c_str(), IDI_CHIP);
}

void CreateSoundFolders(int parent_index)
{
    std::string fname = std::string(GetGuiDir()).append(PATH_SEPARATOR).append("sound.ini");
	LoadExternalFolders(parent_index, fname.c_str(), IDI_CHIP);
}

void CreateScreenFolders(int parent_index)
{
	int i, jj;
	int nGames = driver_list::total();
	int start_folder = numFolders;
	LPTREEFOLDER lpFolder = treeFolders[parent_index];
	LPTREEFOLDER lpTemp;

	// no games in top level folder
	SetAllBits(lpFolder->m_lpGameBits, FALSE);
	
	for (jj = 0; jj < nGames; jj++)
	{
		int screens = DriverNumScreens(jj);
		char s[2];
		itoa(screens, s, 10);

		// look for an existant screens treefolder for this game
		// (likely to be the previous one, so start at the end)
		for (i = numFolders - 1; i >= start_folder; i--)
		{
			if (strcmp(treeFolders[i]->m_lpTitle, s) == 0)
			{
				AddGame(treeFolders[i], jj);
				break;
			}
		}

		if (i == start_folder - 1)
		{
			// nope, it's a screen file we haven't seen before, make it.
			lpTemp = NewFolder(s, next_folder_id++, parent_index, IDI_MONITOR, GetFolderFlags(numFolders));
			AddFolder(lpTemp);
			AddGame(lpTemp, jj);
		}
	}
}

void CreateResolutionFolders(int parent_index)
{
    std::string fname = std::string(GetGuiDir()).append(PATH_SEPARATOR).append("resolution.ini");
	LoadExternalFolders(parent_index, fname.c_str(), IDI_FOLDER);
}

void CreateFPSFolders(int parent_index)
{
    std::string fname = std::string(GetGuiDir()).append(PATH_SEPARATOR).append("refresh.ini");
	LoadExternalFolders(parent_index, fname.c_str(), IDI_FOLDER);
}

static void LoadExternalFolders(int parent_index, const char *fname, int id)
{
    FILE *fp = NULL;
    char readbuf[256];
    char *p;
    char *name;
    int current_id;
    LPTREEFOLDER lpTemp = NULL;
	LPTREEFOLDER lpFolder = treeFolders[parent_index];

 	// no games in top level folder
	SetAllBits(lpFolder->m_lpGameBits, FALSE);
	current_id = lpFolder->m_nFolderId;
    fp = fopen(fname, "r");
 
    while (fgets(readbuf, 256, fp))
    {
		/* do we have [...] ? */
		if (readbuf[0] == '[')
		{
			p = strchr(readbuf, ']');
			
			if (p == NULL)
				continue;

			*p = '\0';
			name = &readbuf[1];

			/* is it [FOLDER_SETTINGS]? */
			if (strcmp(name, "FOLDER_SETTINGS") == 0)
            {
                current_id = -1;
                continue;
            }
            else
            {
                /* is it [ROOT_FOLDER]? */
                if (!strcmp(name, "ROOT_FOLDER"))
                {
                    current_id = lpFolder->m_nFolderId;
                    lpTemp = lpFolder;
                }
                else
                {
					current_id = next_folder_id++;
					lpTemp = NewFolder(name, current_id, parent_index, id, GetFolderFlags(numFolders));
					AddFolder(lpTemp);
				}
        	}
		}
    	else if (current_id != -1)
		{
			/* string on a line by itself -- game name */
            name = strtok(readbuf, " \t\r\n");
			
            if (name == NULL)
            {
                current_id = -1;
                continue;
            }
			
			AddGame(lpTemp, GetGameNameIndex(name));
		}
	}

    if ( fp )
         fclose( fp );
 }

// creates child folders of all the top level folders, including custom ones
void CreateAllChildFolders(void)
{
	int num_top_level_folders = numFolders;
	int i, j;

	for (i = 0; i < num_top_level_folders; i++)
	{
		LPTREEFOLDER lpFolder = treeFolders[i];
		LPCFOLDERDATA lpFolderData = NULL;

		for (j = 0; g_lpFolderData[j].m_lpTitle; j++)
		{
			if (g_lpFolderData[j].m_nFolderId == lpFolder->m_nFolderId)
			{
				lpFolderData = &g_lpFolderData[j];
				break;
			}
		}

		if (lpFolderData != NULL)
		{
			if (lpFolderData->m_pfnCreateFolders != NULL)
				lpFolderData->m_pfnCreateFolders(i);
		}
		else
		{
			if ((lpFolder->m_dwFlags & F_CUSTOM) == 0)
				continue;

			// load the extra folder files, which also adds children
			if (TryAddExtraFolderAndChildren(i) == FALSE)
				lpFolder->m_nFolderId = FOLDER_NONE;
		}
	}
}

// adds these folders to the treeview
void ResetTreeViewFolders(void)
{
	HWND hTreeView = GetTreeView();
	int i;
	TVITEM tvi;
	TVINSERTSTRUCT	tvs;

	// currently "cached" parent
	HTREEITEM hti_parent = NULL;
	int index_parent = -1;

	(void)TreeView_DeleteAllItems(hTreeView);
	tvs.hInsertAfter = TVI_LAST; // main items inserted according to g_folderData[] array

	for (i = 0; i < numFolders; i++)
	{
		LPTREEFOLDER lpFolder = treeFolders[i];
		
		if (lpFolder->m_nParent == -1)
		{
			if (lpFolder->m_nFolderId < MAX_FOLDERS)
			{
				// it's a built in folder, let's see if we should show it
				if (GetShowFolder(lpFolder->m_nFolderId) == FALSE)
					continue;
			}

			tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvs.hParent = TVI_ROOT;
			tvi.pszText = lpFolder->m_lptTitle;
			tvi.lParam = (LPARAM)lpFolder;
			tvi.iImage = GetTreeViewIconIndex(lpFolder->m_nIconId);
			tvi.iSelectedImage = 0;
			tvs.item = tvi;

			// Add root branch
			hti_parent = TreeView_InsertItem(hTreeView, &tvs);
			continue;
		}

		// not a top level branch, so look for parent
		if (treeFolders[i]->m_nParent != index_parent)
		{
			hti_parent = TreeView_GetRoot(hTreeView);

			while (1)
			{
				if (hti_parent == NULL)
					// couldn't find parent folder, so it's a built-in but
					// not shown folder
					break;

				tvi.hItem = hti_parent;
				tvi.mask = TVIF_PARAM;
				
				(void)TreeView_GetItem(hTreeView, &tvi);

				if (((LPTREEFOLDER)tvi.lParam) == treeFolders[treeFolders[i]->m_nParent])
					break;

				hti_parent = TreeView_GetNextSibling(hTreeView, hti_parent);
			}

			// if parent is not shown, then don't show the child either obviously!
			if (hti_parent == NULL)
				continue;

			index_parent = treeFolders[i]->m_nParent;
		}

		tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvs.hParent = hti_parent;
		tvi.iImage = GetTreeViewIconIndex(treeFolders[i]->m_nIconId);
		tvi.iSelectedImage = 0;
		tvi.pszText = treeFolders[i]->m_lptTitle;
		tvi.lParam = (LPARAM)treeFolders[i];
		tvs.item = tvi;

		// Add it to this tree branch
		tvs.hInsertAfter = TVI_SORT; 	// sub items always sorted
		(void)TreeView_InsertItem(hTreeView, &tvs);
		tvs.hInsertAfter = TVI_LAST; 	// restore for next main item
	}
}

void SelectTreeViewFolder(int folder_id)
{
	HWND hTreeView = GetTreeView();
	HTREEITEM hti;
	TVITEM tvi;

	memset(&tvi,0,sizeof(tvi));
	hti = TreeView_GetRoot(hTreeView);

	while (hti != NULL)
	{
		HTREEITEM hti_next;

		tvi.hItem = hti;
		tvi.mask = TVIF_PARAM;
		
		(void)TreeView_GetItem(hTreeView, &tvi);

		if (((LPTREEFOLDER)tvi.lParam)->m_nFolderId == folder_id)
		{
			(void)TreeView_SelectItem(hTreeView,tvi.hItem);
			SetCurrentFolder((LPTREEFOLDER)tvi.lParam);
			return;
		}

		hti_next = TreeView_GetChild(hTreeView, hti);
		
		if (hti_next == NULL)
		{
			hti_next = TreeView_GetNextSibling(hTreeView, hti);
			
			if (hti_next == NULL)
			{
				hti_next = TreeView_GetParent(hTreeView, hti);
				
				if (hti_next != NULL)
					hti_next = TreeView_GetNextSibling(hTreeView, hti_next);
			}
		}

		hti = hti_next;
	}

	// could not find folder to select
	// make sure we select something
	tvi.hItem = TreeView_GetRoot(hTreeView);
	tvi.mask = TVIF_PARAM;
	
	(void)TreeView_GetItem(hTreeView, &tvi);
	(void)TreeView_SelectItem(hTreeView, tvi.hItem);
	SetCurrentFolder((LPTREEFOLDER)tvi.lParam);

}

/*
 * Does this folder have an INI associated with it?
 * Currently only TRUE for FOLDER_VECTOR and children
 * of FOLDER_SOURCE.
 */
static BOOL FolderHasIni(LPTREEFOLDER lpFolder) 
{
	if (FOLDER_VECTOR == lpFolder->m_nFolderId || FOLDER_VERTICAL == lpFolder->m_nFolderId || FOLDER_HORIZONTAL == lpFolder->m_nFolderId) 
		return TRUE;
	
	if (lpFolder->m_nParent != -1 && FOLDER_SOURCE == treeFolders[lpFolder->m_nParent]->m_nFolderId) 
		return TRUE;
	
	return FALSE;
}

/* Add a folder to the list.  Does not allocate */
static BOOL AddFolder(LPTREEFOLDER lpFolder)
{
	TREEFOLDER **tmpTree = NULL;
	UINT oldFolderArrayLength = folderArrayLength;
	
	if (numFolders + 1 >= folderArrayLength)
	{
		folderArrayLength += 500;
		tmpTree = (TREEFOLDER **)malloc(sizeof(TREEFOLDER **) * folderArrayLength);
		memcpy(tmpTree, treeFolders, sizeof(TREEFOLDER **) * oldFolderArrayLength);
		
		if (treeFolders) 
			osd_free(treeFolders);
		
		treeFolders = tmpTree;
	}

	/* Is there an folder.ini that can be edited? */
	if (FolderHasIni(lpFolder)) 
		lpFolder->m_dwFlags |= F_INIEDIT;

	treeFolders[numFolders] = lpFolder;
	numFolders++;
	return TRUE;
}

/* Allocate and initialize a NEW TREEFOLDER */
static LPTREEFOLDER NewFolder(const char *lpTitle, UINT nFolderId, int nParent, UINT nIconId, DWORD dwFlags)
{
	LPTREEFOLDER lpFolder = (LPTREEFOLDER)malloc(sizeof(TREEFOLDER));
	memset(lpFolder, '\0', sizeof (TREEFOLDER));
	lpFolder->m_lpTitle = (LPSTR)malloc(strlen(lpTitle) + 1);
	strcpy((char *)lpFolder->m_lpTitle, lpTitle);
	lpFolder->m_lptTitle = tstring_from_utf8(lpFolder->m_lpTitle);
	lpFolder->m_lpGameBits = NewBits(driver_list::total());
	lpFolder->m_nFolderId = nFolderId;
	lpFolder->m_nParent = nParent;
	lpFolder->m_nIconId = nIconId;
	lpFolder->m_dwFlags = dwFlags;
	return lpFolder;
}

/* Deallocate the passed in LPTREEFOLDER */
static void DeleteFolder(LPTREEFOLDER lpFolder)
{
	if (lpFolder)
	{
		if (lpFolder->m_lpGameBits)
		{
			DeleteBits(lpFolder->m_lpGameBits);
			lpFolder->m_lpGameBits = 0;
		}
		
		osd_free(lpFolder->m_lptTitle);
		lpFolder->m_lptTitle = 0;
		osd_free(lpFolder->m_lpTitle);
		lpFolder->m_lpTitle = 0;
		osd_free(lpFolder);
		lpFolder = 0;
	}
}

/* Can be called to re-initialize the array of treeFolders */
BOOL InitFolders(void)
{
	int i = 0;
	DWORD dwFolderFlags;
	LPCFOLDERDATA fData = 0;
	
	if (treeFolders != NULL)
	{
		for (i = numFolders - 1; i >= 0; i--)
		{
			DeleteFolder(treeFolders[i]);
			treeFolders[i] = 0;
			numFolders--;
		}
	}
	
	numFolders = 0;
	
	if (folderArrayLength == 0)
	{
		folderArrayLength = 200;
		treeFolders = (TREEFOLDER **)malloc(sizeof(TREEFOLDER **) * folderArrayLength);
		
		if (!treeFolders)
		{
			folderArrayLength = 0;
			return 0;
		}
		else
			memset(treeFolders,'\0', sizeof(TREEFOLDER **) * folderArrayLength);
	}
	
	// built-in top level folders
	for (i = 0; g_lpFolderData[i].m_lpTitle; i++)
	{
		fData = &g_lpFolderData[i];
		/* get the saved folder flags */
		dwFolderFlags = GetFolderFlags(numFolders);
		/* create the folder */
		AddFolder(NewFolder(fData->m_lpTitle, fData->m_nFolderId, -1, fData->m_nIconId, dwFolderFlags));
	}

	numExtraFolders = InitExtraFolders();

	for (i = 0; i < numExtraFolders; i++)
	{
		LPEXFOLDERDATA  fExData = ExtraFolderData[i];
		// OR in the saved folder flags
		dwFolderFlags = fExData->m_dwFlags | GetFolderFlags(numFolders);
		// create the folder
		AddFolder(NewFolder(fExData->m_szTitle, fExData->m_nFolderId, fExData->m_nParent, fExData->m_nIconId, dwFolderFlags));
	}

	CreateAllChildFolders();
	CreateTreeIcons();
	ResetWhichGamesInFolders();
	ResetTreeViewFolders();
	SelectTreeViewFolder(GetSavedFolderID());
	LoadFolderFlags();
	return TRUE;
}

// create iconlist and Treeview control
static BOOL CreateTreeIcons()
{
	HICON hIcon;
	INT i;
	HINSTANCE hInst = GetModuleHandle(NULL);

	int numIcons = ICON_MAX + numExtraIcons;
	hTreeSmall = ImageList_Create (16, 16, ILC_COLORDDB | ILC_MASK, numIcons, numIcons);

	for (i = 0; i < ICON_MAX; i++)
	{
		hIcon = LoadIconFromFile(treeIconNames[i].lpName);
		
		if (!hIcon)
			hIcon = LoadIcon(hInst, MAKEINTRESOURCE(treeIconNames[i].nResourceID));

		if (ImageList_AddIcon(hTreeSmall, hIcon) == -1)
		{
			ErrorMsg("Error creating icon on regular folder, %i %i", i, hIcon != NULL);
			return FALSE;
		}
	}

	for (i = 0; i < numExtraIcons; i++)
	{
		if ((hIcon = LoadIconFromFile(ExtraFolderIcons[i])) == 0)
			hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FOLDER));

		if (ImageList_AddIcon(hTreeSmall, hIcon) == -1)
		{
			ErrorMsg("Error creating icon on extra folder, %i %i", i, hIcon != NULL);
			return FALSE;
		}
	}

	// Be sure that all the small icons were added.
	if (ImageList_GetImageCount(hTreeSmall) < numIcons)
	{
		ErrorMsg("Error with icon list--too few images.  %i %i", ImageList_GetImageCount(hTreeSmall),numIcons);
		return FALSE;
	}

	// Be sure that all the small icons were added.
	if (ImageList_GetImageCount (hTreeSmall) < ICON_MAX)
	{
		ErrorMsg("Error with icon list--too few images.  %i < %i", ImageList_GetImageCount(hTreeSmall),(INT)ICON_MAX);
		return FALSE;
	}

	// Associate the image lists with the list view control.
	(void)TreeView_SetImageList(GetTreeView(), hTreeSmall, TVSIL_NORMAL);
	return TRUE;
}

/* Header code - Directional Arrows */
static LRESULT CALLBACK TreeWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (MouseHasBeenMoved())
			ShowCursor(TRUE);
		break;
	}
	
	case WM_KEYDOWN :
		if (wParam == VK_F2)
		{
			if (lpCurrentFolder->m_dwFlags & F_CUSTOM)
			{
				(void)TreeView_EditLabel(hWnd, TreeView_GetSelection(hWnd));
				return TRUE;
			}
		}
		
		break;
	}
	
	/* message not handled */
	return CallWindowProc(g_lpTreeWndProc, hWnd, uMsg, wParam, lParam);
}

/*
 * Filter code - should be moved to filter.c/filter.h
 * Added 01/09/99 - MSH <mhaaland@hypertech.com>
 */

/* find a FOLDERDATA by folderID */
LPCFOLDERDATA FindFilter(DWORD folderID)
{
	int i;

	for (i = 0; g_lpFolderData[i].m_lpTitle; i++)
	{
		if (g_lpFolderData[i].m_nFolderId == folderID)
			return &g_lpFolderData[i];
	}

	return (LPFOLDERDATA) 0;
}

LPTREEFOLDER GetFolderByName(int nParentId, const char *pszFolderName)
{
	int i = 0, nParent;

	//First Get the Parent TreeviewItem
	//Enumerate Children
	for(i = 0; i < numFolders; i++)
	{
		if (!strcmp(treeFolders[i]->m_lpTitle, pszFolderName))
		{
			nParent = treeFolders[i]->m_nParent;
			
			if ((nParent >= 0) && treeFolders[nParent]->m_nFolderId == nParentId)
				return treeFolders[i];
		}
	}

	return NULL;
}

static int InitExtraFolders(void)
{
	struct stat stat_buffer;
	struct _finddata_t files;
	int i, count = 0;
	long hLong;
	char *ext;
	char buf[256];
	char curdir[MAX_PATH];
	const char *dir = GetFolderDir();

	memset(ExtraFolderData, 0, (MAX_EXTRA_FOLDERS * MAX_EXTRA_SUBFOLDERS)* sizeof(LPEXFOLDERDATA));

	/* NPW 9-Feb-2003 - MSVC stat() doesn't like stat() called with an empty
     	* string
     	*/
	if (dir[0] == '\0')
		dir = ".";

	// Why create the directory if it doesn't exist, just return 0 folders.
	if (stat(dir, &stat_buffer) != 0)
		return 0;

	_getcwd(curdir, MAX_PATH);
	chdir(dir);
	hLong = _findfirst("*", &files);

	for (i = 0; i < MAX_EXTRA_FOLDERS; i++)
    {
		ExtraFolderIcons[i] = NULL;
    }

	numExtraIcons = 0;

	while (!_findnext(hLong, &files))
	{
		if ((files.attrib & _A_SUBDIR) == 0)
		{
			FILE *fp;

			fp = fopen(files.name, "r");
			
			if (fp != NULL)
			{
				int icon[2] = { 0, 0 };
				char *p, *name;

				while (fgets(buf, 256, fp))
				{
					if (buf[0] == '[')
					{
						p = strchr(buf, ']');
						
						if (p == NULL)
							continue;

						*p = '\0';
						name = &buf[1];
						
						if (!strcmp(name, "FOLDER_SETTINGS"))
						{
							while (fgets(buf, 256, fp))
							{
								name = strtok(buf, " =\r\n");
								
								if (name == NULL)
									break;

								if (!strcmp(name, "RootFolderIcon"))
								{
									name = strtok(NULL, " =\r\n");
									
									if (name != NULL)
										SetExtraIcons(name, &icon[0]);
								}
								if (!strcmp(name, "SubFolderIcon"))
								{
									name = strtok(NULL, " =\r\n");
									
									if (name != NULL)
										SetExtraIcons(name, &icon[1]);
								}
							}
							
							break;
						}
					}
				}
				
				fclose(fp);
				strcpy(buf, files.name);
				ext = strrchr(buf, '.');

				if (ext && *(ext + 1) && !core_stricmp(ext + 1, "ini"))
				{
					ExtraFolderData[count] =(EXFOLDERDATA*) malloc(sizeof(EXFOLDERDATA));
					
					if (ExtraFolderData[count])
					{
						*ext = '\0';
						memset(ExtraFolderData[count], 0, sizeof(EXFOLDERDATA));
						strncpy(ExtraFolderData[count]->m_szTitle, buf, 63);
						ExtraFolderData[count]->m_nFolderId = next_folder_id++;
						ExtraFolderData[count]->m_nParent = -1;
						ExtraFolderData[count]->m_dwFlags = F_CUSTOM;
						ExtraFolderData[count]->m_nIconId = icon[0] ? -icon[0] : IDI_FOLDER;
						ExtraFolderData[count]->m_nSubIconId = icon[1] ? -icon[1] : IDI_FOLDER;
						count++;
					}
				}
			}
		}
	}

	chdir(curdir);
	return count;
}

void FreeExtraFolders(void)
{
	int i;

	for (i = 0; i < numExtraFolders; i++)
	{
		if (ExtraFolderData[i])
		{
			osd_free(ExtraFolderData[i]);
			ExtraFolderData[i] = NULL;
		}
	}

	for (i = 0; i < numExtraIcons; i++)
    {
		osd_free(ExtraFolderIcons[i]);
    }

	numExtraIcons = 0;
}


static void SetExtraIcons(char *name, int *id)
{
	char *p = strchr(name, '.');

	if (p != NULL)
		*p = '\0';

	ExtraFolderIcons[numExtraIcons] = (char*)malloc(strlen(name) + 1);
	
	if (ExtraFolderIcons[numExtraIcons])
	{
		*id = ICON_MAX + numExtraIcons;
		strcpy(ExtraFolderIcons[numExtraIcons], name);
		numExtraIcons++;
	}
}


// Called to add child folders of the top level extra folders already created
BOOL TryAddExtraFolderAndChildren(int parent_index)
{
    FILE *fp = NULL;
    char fname[MAX_PATH];
    char readbuf[256];
    char *p;
    char *name;
    int id, current_id, i;
    LPTREEFOLDER lpTemp = NULL;
	LPTREEFOLDER lpFolder = treeFolders[parent_index];

    current_id = lpFolder->m_nFolderId;
    id = lpFolder->m_nFolderId - MAX_FOLDERS;
    sprintf(fname, "%s\\%s.ini", GetFolderDir(), ExtraFolderData[id]->m_szTitle);

    fp = fopen(fname, "r");
	
    if (fp == NULL)
        return FALSE;

    while (fgets(readbuf, 256, fp))
    {
		/* do we have [...] ? */
		if (readbuf[0] == '[')
		{
			p = strchr(readbuf, ']');
			
			if (p == NULL)
				continue;

			*p = '\0';
			name = &readbuf[1];

			/* is it [FOLDER_SETTINGS]? */
			if (strcmp(name, "FOLDER_SETTINGS") == 0)
            {
                current_id = -1;
                continue;
            }
            else
            {
                /* is it [ROOT_FOLDER]? */
                if (!strcmp(name, "ROOT_FOLDER"))
                {
                    current_id = lpFolder->m_nFolderId;
                    lpTemp = lpFolder;
                }
                else
                {
                    /* must be [folder name] */
                    current_id = next_folder_id++;
					/* create a new folder with this name,
					and the flags for this folder as read from the registry */
					lpTemp = NewFolder(name, current_id, parent_index, ExtraFolderData[id]->m_nSubIconId, GetFolderFlags(numFolders) | F_CUSTOM);
					ExtraFolderData[current_id] = (EXFOLDERDATA*)malloc(sizeof(EXFOLDERDATA));
					memset(ExtraFolderData[current_id], 0, sizeof(EXFOLDERDATA));
					ExtraFolderData[current_id]->m_nFolderId = current_id - MAX_EXTRA_FOLDERS;
					ExtraFolderData[current_id]->m_nIconId = ExtraFolderData[id]->m_nSubIconId;
					ExtraFolderData[current_id]->m_nParent = ExtraFolderData[id]->m_nFolderId;
					ExtraFolderData[current_id]->m_nSubIconId = -1;
					strcpy( ExtraFolderData[current_id]->m_szTitle, name );
					ExtraFolderData[current_id]->m_dwFlags = ExtraFolderData[id]->m_dwFlags;
                    AddFolder(lpTemp);
                }
        	}
		}
    	else if (current_id != -1)
		{
			/* string on a line by itself -- game name */
            name = strtok(readbuf, " \t\r\n");
			
            if (name == NULL)
            {
                current_id = -1;
                continue;
            }

            /* IMPORTANT: This assumes that all driver names are lowercase! */
			for (i = 0; name[i]; i++)
				name[i] = tolower(name[i]);

			if (lpTemp == NULL)
			{
				ErrorMsg("Error parsing %s: missing [folder name] or [ROOT_FOLDER]", fname);
				current_id = lpFolder->m_nFolderId;
				lpTemp = lpFolder;
			}
			
			AddGame(lpTemp, GetGameNameIndex(name));
		}
	}

    if ( fp )
         fclose( fp );

    return TRUE;
}


void GetFolders(TREEFOLDER ***folders,int *num_folders)
{
	*folders = treeFolders;
	*num_folders = numFolders;
}

static BOOL TryRenameCustomFolderIni(LPTREEFOLDER lpFolder,const char *old_name,const char *new_name)
{
	char filename[MAX_PATH];
	char new_filename[MAX_PATH];
	LPTREEFOLDER lpParent = NULL;
	
	if (lpFolder->m_nParent >= 0)
	{
		//it is a custom SubFolder
		lpParent = GetFolder( lpFolder->m_nParent );
		
		if( lpParent )
		{
			snprintf(filename, ARRAY_LENGTH(filename), "%s\\%s\\%s.ini", GetIniDir(), lpParent->m_lpTitle, old_name );
			snprintf(new_filename, ARRAY_LENGTH(new_filename), "%s\\%s\\%s.ini", GetIniDir(), lpParent->m_lpTitle, new_name );
			win_move_file_utf8(filename, new_filename);
		}
	}
	else
	{
		//Rename the File, if it exists
		snprintf(filename, ARRAY_LENGTH(filename), "%s\\%s.ini", GetIniDir(), old_name );
		snprintf(new_filename, ARRAY_LENGTH(new_filename), "%s\\%s.ini", GetIniDir(), new_name );
		win_move_file_utf8(filename, new_filename);
		//Rename the Directory, if it exists
		snprintf(filename, ARRAY_LENGTH(filename), "%s\\%s", GetIniDir(), old_name );
		snprintf(new_filename, ARRAY_LENGTH(new_filename), "%s\\%s", GetIniDir(), new_name );
		win_move_file_utf8(filename, new_filename);
	}

	return TRUE;
}

BOOL TryRenameCustomFolder(LPTREEFOLDER lpFolder, const char *new_name)
{
	BOOL retval;
	char filename[MAX_PATH];
	char new_filename[MAX_PATH];

	if (lpFolder->m_nParent >= 0)
	{
		// a child extra folder was renamed, so do the rename and save the parent
		// save old title
		char *old_title = lpFolder->m_lpTitle;
		// set new title
		lpFolder->m_lpTitle = (char *)malloc(strlen(new_name) + 1);
		strcpy(lpFolder->m_lpTitle, new_name);

		// try to save
		if (TrySaveExtraFolder(lpFolder) == FALSE)
		{
			// failed, so free newly allocated title and restore old
			osd_free(lpFolder->m_lpTitle);
			lpFolder->m_lpTitle = old_title;
			return FALSE;
		}
		
		TryRenameCustomFolderIni(lpFolder, old_title, new_name);
		// successful, so free old title
		osd_free(old_title);
		return TRUE;
	}

	// a parent extra folder was renamed, so rename the file
	snprintf(new_filename, ARRAY_LENGTH(new_filename), "%s\\%s.ini", GetFolderDir(), new_name);
	snprintf(filename, ARRAY_LENGTH(filename), "%s\\%s.ini", GetFolderDir(), lpFolder->m_lpTitle);
	retval = win_move_file_utf8(filename,new_filename);

	if (retval)
	{
		TryRenameCustomFolderIni(lpFolder, lpFolder->m_lpTitle, new_name);
		osd_free(lpFolder->m_lpTitle);
		lpFolder->m_lpTitle = (char *)malloc(strlen(new_name) + 1);
		strcpy(lpFolder->m_lpTitle, new_name);
	}
	else
	{
		char buf[500];
		snprintf(buf, ARRAY_LENGTH(buf), "Error while renaming custom file %s to %s", filename, new_filename);
		win_message_box_utf8(GetMainWindow(), buf, MAMEUINAME, MB_ICONERROR | MB_OK);
	}
	
	return retval;
}

void AddToCustomFolder(LPTREEFOLDER lpFolder,int driver_index)
{
    if ((lpFolder->m_dwFlags & F_CUSTOM) == 0)
	{
	    win_message_box_utf8(GetMainWindow(), "Unable to add game to non-custom folder", MAMEUINAME, MB_ICONERROR | MB_OK);
		return;
	}

	if (TestBit(lpFolder->m_lpGameBits,driver_index) == 0)
	{
		AddGame(lpFolder,driver_index);
		if (TrySaveExtraFolder(lpFolder) == FALSE)
			RemoveGame(lpFolder,driver_index); 	// undo on error
	}
}

void RemoveFromCustomFolder(LPTREEFOLDER lpFolder,int driver_index)
{
    if ((lpFolder->m_dwFlags & F_CUSTOM) == 0)
	{
	    win_message_box_utf8(GetMainWindow(), "Unable to remove game from non-custom folder", MAMEUINAME, MB_ICONERROR | MB_OK);
		return;
	}

	if (TestBit(lpFolder->m_lpGameBits,driver_index) != 0)
	{
		RemoveGame(lpFolder,driver_index);
		if (TrySaveExtraFolder(lpFolder) == FALSE)
			AddGame(lpFolder,driver_index); // undo on error
	}
}

BOOL TrySaveExtraFolder(LPTREEFOLDER lpFolder)
{
    char fname[MAX_PATH];
	FILE *fp;
	BOOL error = FALSE;
    int i,j;

	LPTREEFOLDER root_folder = NULL;
	LPEXFOLDERDATA extra_folder = NULL;

	for (i = 0; i < numExtraFolders; i++)
	{
	    if (ExtraFolderData[i]->m_nFolderId == lpFolder->m_nFolderId)
		{
		    root_folder = lpFolder;
		    extra_folder = ExtraFolderData[i];
			break;
		}

		if (lpFolder->m_nParent >= 0 && ExtraFolderData[i]->m_nFolderId == treeFolders[lpFolder->m_nParent]->m_nFolderId)
		{
			root_folder = treeFolders[lpFolder->m_nParent];
			extra_folder = ExtraFolderData[i];
			break;
		}
	}

	if (extra_folder == NULL || root_folder == NULL)
	{
		win_message_box_utf8(GetMainWindow(), "Error finding custom file name to save", MAMEUINAME, MB_ICONERROR | MB_OK);
	   	return FALSE;
	}
	
    /* "folder\title.ini" */
    snprintf(fname, sizeof(fname), "%s\\%s.ini", GetFolderDir(), extra_folder->m_szTitle);

    fp = fopen(fname, "wt");
	
    if (fp == NULL)
		error = TRUE;
	else
	{
	   	TREEFOLDER *folder_data;

	   	fprintf(fp,"[FOLDER_SETTINGS]\n");
		
	   	// negative values for icons means it's custom, so save 'em
	   	if (extra_folder->m_nIconId < 0)
	   	{
		   	fprintf(fp, "RootFolderIcon %s\n",
			ExtraFolderIcons[(-extra_folder->m_nIconId) - ICON_MAX]);
	   	}
	   	if (extra_folder->m_nSubIconId < 0)
	   	{
			fprintf(fp,"SubFolderIcon %s\n",
			ExtraFolderIcons[(-extra_folder->m_nSubIconId) - ICON_MAX]);
	   	}

	   	/* need to loop over all our TREEFOLDERs--first the root one, then each child.
           start with the root */
	   	folder_data = root_folder;
	   	fprintf(fp,"\n[ROOT_FOLDER]\n");

	   	for (i = 0; i < driver_list::total(); i++)
	   	{
		   	int driver_index = GetIndexFromSortedIndex(i);
			
		   	if (TestBit(folder_data->m_lpGameBits,driver_index))
			   	fprintf(fp, "%s\n", driver_list::driver(driver_index).name);
	   	}

	   	/* look through the custom folders for ones with our root as parent */
	   	for (j = 0; j < numFolders; j++)
	   	{
		   	folder_data = treeFolders[j];

		   	if (folder_data->m_nParent >= 0 && treeFolders[folder_data->m_nParent] == root_folder)
		   	{
			   	fprintf(fp, "\n[%s]\n", folder_data->m_lpTitle);

			   	for (i = 0; i < driver_list::total(); i++)
			   	{
				   	int driver_index = GetIndexFromSortedIndex(i);
					
				   	if (TestBit(folder_data->m_lpGameBits,driver_index))
					   	fprintf(fp, "%s\n", driver_list::driver(driver_index).name);
			   	}
		   	}
	   	}
		
	   	if (fclose(fp) != 0)
		   	error = TRUE;
	}

	if (error)
	{
		char buf[500];
		snprintf(buf, ARRAY_LENGTH(buf),"Error while saving custom file %s", fname);
		win_message_box_utf8(GetMainWindow(), buf, MAMEUINAME, MB_ICONERROR | MB_OK);
	}
	
	return !error;
}

HIMAGELIST GetTreeViewIconList(void)
{
    return hTreeSmall;
}

int GetTreeViewIconIndex(int icon_id)
{
	int i;

	if (icon_id < 0)
		return -icon_id;

	for (i = 0; i < ARRAY_LENGTH(treeIconNames); i++)
	{
		if (icon_id == treeIconNames[i].nResourceID)
			return i;
	}

	return - 1;
}

/* End of source file */
