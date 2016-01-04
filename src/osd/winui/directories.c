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

  directories.c

***************************************************************************/

#include "winui.h"

#define MAX_DIRS 256
#define DirInfo_NumDir(pInfo, path)		((pInfo)[(path)].m_Path->m_NumDirectories)

/***************************************************************************
    Internal structures
 ***************************************************************************/

typedef struct
{
	TCHAR m_Directories[MAX_DIRS][MAX_PATH];
	int m_NumDirectories;
	BOOL m_bModified;
} tPath;

typedef struct
{
	tPath *m_Path;
	TCHAR *m_tDirectory;
} tDirInfo;

typedef struct
{
	const char *lpName;
	const char* (*pfnGetTheseDirs)(void);
	void (*pfnSetTheseDirs)(const char *lpDirs);
	BOOL bMulti;
	int nDirDlgFlags;
} DIRECTORYINFO;

static const DIRECTORYINFO g_directoryInfo[] =
{
	{ "ROMs",                  GetRomDirs,      SetRomDirs,      TRUE,  DIRDLG_ROM },
	{ "Samples",               GetSampleDirs,   SetSampleDirs,   TRUE,  0 },
	{ "Config files",          GetCfgDir,       SetCfgDir,       FALSE, 0 },
	{ "High scores",           GetHiDir,        SetHiDir,        FALSE, 0 },
	{ "Snapshots",             GetImgDir,       SetImgDir,       FALSE, 0 },
	{ "Input files",           GetInpDir,       SetInpDir,       FALSE, 0 },
	{ "State files",           GetStateDir,     SetStateDir,     FALSE, 0 },
	{ "Artwork files",         GetArtDir,       SetArtDir,       FALSE, 0 },
	{ "NVRAM files",           GetNvramDir,     SetNvramDir,     FALSE, 0 },
	{ "Controller files",      GetCtrlrDir,     SetCtrlrDir,     FALSE, 0 },
	{ "Crosshair files",       GetCrosshairDir, SetCrosshairDir, FALSE, 0 },
	{ "HLSL files",            GetHLSLDir, 	  	SetHLSLDir, 	 FALSE, 0 },
	{ "CHD Diff files",        GetDiffDir, 	  	SetDiffDir, 	 FALSE, 0 },
	{ "GLSL shader files",     GetGLSLDir, 	  	SetGLSLDir, 	 FALSE, 0 },
	{ "Font files",            GetFontDir,      SetFontDir,      FALSE, 0 },
	{ "Video files",           GetVideoDir,     SetVideoDir,     FALSE, 0 },
	{ "ProgettoSnaps movies",  GetMoviesDir,    SetMoviesDir,    FALSE, 0 },
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
	{ "Datafiles",             GetDatsDir,      SetDatsDir,      FALSE, 0 },
	{ NULL }
};

/***************************************************************************
    Function prototypes
 ***************************************************************************/

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
static BOOL BrowseForDirectory(HWND hwnd, const TCHAR* pStartDir, TCHAR* pResult);
static void DirInfo_SetDir(tDirInfo *pInfo, int nType, int nItem, const TCHAR* pText);
static TCHAR* DirInfo_Dir(tDirInfo *pInfo, int nType);
static TCHAR* DirInfo_Path(tDirInfo *pInfo, int nType, int nItem);
static void DirInfo_SetModified(tDirInfo *pInfo, int nType, BOOL bModified);
static BOOL DirInfo_Modified(tDirInfo *pInfo, int nType);
static TCHAR* FixSlash(TCHAR *s);
static void UpdateDirectoryList(HWND hDlg);
static void Directories_OnSelChange(HWND hDlg);
static BOOL Directories_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
static void Directories_OnDestroy(HWND hDlg);
static void Directories_OnClose(HWND hDlg);
static void Directories_OnOk(HWND hDlg);
static void Directories_OnCancel(HWND hDlg);
static void Directories_OnInsert(HWND hDlg);
static void Directories_OnBrowse(HWND hDlg);
static void Directories_OnDelete(HWND hDlg);
static void Directories_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

/***************************************************************************
    Internal variables
 ***************************************************************************/

static tDirInfo *g_pDirInfo;
static HBRUSH hBrush = NULL;
static HDC hDC = NULL;
static HICON hIcon = NULL;

/***************************************************************************
    External function definitions
 ***************************************************************************/

INT_PTR CALLBACK DirectoriesDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		CenterWindow(hDlg);
        hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		hBrush = CreateSolidBrush(RGB(224, 223, 227));

		if(IsWindowsSevenOrHigher())
			(void)ListView_SetExtendedListViewStyle(GetDlgItem(hDlg, IDC_DIR_LIST), LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
		else
			(void)ListView_SetExtendedListViewStyle(GetDlgItem(hDlg, IDC_DIR_LIST), LVS_EX_FULLROWSELECT | LVS_EX_UNDERLINEHOT | LVS_EX_ONECLICKACTIVATE | LVS_EX_DOUBLEBUFFER);
	
		SetWindowTheme(GetDlgItem(hDlg, IDC_DIR_LIST), L"Explorer", NULL);
		SetWindowTheme(GetDlgItem(hDlg, IDC_DIR_COMBO), L" ", L" ");
		return (BOOL)HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, Directories_OnInitDialog);

	case WM_CTLCOLORDLG:
		return (LRESULT) hBrush;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		return (LRESULT) hBrush;

	case WM_COMMAND:
		HANDLE_WM_COMMAND(hDlg, wParam, lParam, Directories_OnCommand);
		return TRUE;

	case WM_CLOSE:
		HANDLE_WM_CLOSE(hDlg, wParam, lParam, Directories_OnClose);
		break;

	case WM_DESTROY:
		DeleteObject(hBrush);
		DestroyIcon(hIcon);
		HANDLE_WM_DESTROY(hDlg, wParam, lParam, Directories_OnDestroy);
		break;

	default:
		return FALSE;
	}
	
	return FALSE;
}

/***************************************************************************
    Internal function definitions
 ***************************************************************************/

static BOOL IsMultiDir(int nType)
{
	return g_directoryInfo[nType].bMulti;
}

static void DirInfo_SetDir(tDirInfo *pInfo, int nType, int nItem, const TCHAR* pText)
{
	if (IsMultiDir(nType))
	{
		assert(nItem >= 0);
		_tcscpy(DirInfo_Path(pInfo, nType, nItem), pText);
		DirInfo_SetModified(pInfo, nType, TRUE);
	}
	else
	{
		char *str = utf8_from_tstring(pText);
		TCHAR *t_str = tstring_from_utf8(str);
		TCHAR *t_pOldText = pInfo[nType].m_tDirectory;

		if (t_pOldText)
			free(t_pOldText);

		pInfo[nType].m_tDirectory = t_str;
		free(t_str);
		free(str);
	}
}

static TCHAR* DirInfo_Dir(tDirInfo *pInfo, int nType)
{
	assert(!IsMultiDir(nType));
	return pInfo[nType].m_tDirectory;
}

static TCHAR* DirInfo_Path(tDirInfo *pInfo, int nType, int nItem)
{
	return pInfo[nType].m_Path->m_Directories[nItem];
}

static void DirInfo_SetModified(tDirInfo *pInfo, int nType, BOOL bModified)
{
	assert(IsMultiDir(nType));
	pInfo[nType].m_Path->m_bModified = bModified;
}

static BOOL DirInfo_Modified(tDirInfo *pInfo, int nType)
{
	assert(IsMultiDir(nType));
	return pInfo[nType].m_Path->m_bModified;
}

/* lop off trailing backslash if it exists */
static TCHAR * FixSlash(TCHAR *s)
{
	int len = 0;

	if (s)
		len = _tcslen(s);

	if (len > 3 && s[len - 1] == *PATH_SEPARATOR)
		s[len - 1] = '\0';

	return s;
}

static void UpdateDirectoryList(HWND hDlg)
{
	LVITEM Item;
	HWND hList  = GetDlgItem(hDlg, IDC_DIR_LIST);
	HWND hCombo = GetDlgItem(hDlg, IDC_DIR_COMBO);

	/* Remove previous */
	(void)ListView_DeleteAllItems(hList);

	/* Update list */
	memset(&Item, 0, sizeof(LVITEM));
	Item.mask = LVIF_TEXT | LVIF_IMAGE;
	Item.iImage = -1;

	int nType = ComboBox_GetCurSel(hCombo);

	if (IsMultiDir(nType))
	{
		Item.pszText = (TCHAR*) TEXT(DIRLIST_NEWENTRYTEXT);
		(void)ListView_InsertItem(hList, &Item);

		for (int i = DirInfo_NumDir(g_pDirInfo, nType) - 1; 0 <= i; i--)
		{
			Item.pszText = DirInfo_Path(g_pDirInfo, nType, i);
			(void)ListView_InsertItem(hList, &Item);
		}
	}
	else
	{
		Item.pszText = DirInfo_Dir(g_pDirInfo, nType);
		(void)ListView_InsertItem(hList, &Item);
	}

	/* select first one */
	ListView_SetItemState(hList, 0, LVIS_SELECTED, LVIS_SELECTED);
}

static void Directories_OnSelChange(HWND hDlg)
{
	UpdateDirectoryList(hDlg);
	int nType = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_DIR_COMBO));

	if (IsMultiDir(nType))
	{
		EnableWindow(GetDlgItem(hDlg, IDC_DIR_DELETE), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_DIR_INSERT), TRUE);
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg, IDC_DIR_DELETE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_DIR_INSERT), FALSE);
	}
}

static BOOL Directories_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	RECT rectClient;
	LVCOLUMN LVCol;
	int i = 0;
	TCHAR *token = NULL;
	TCHAR buf[MAX_PATH * MAX_DIRS];
	TCHAR *t_s = NULL;

	/* count how many dirinfos there are */
	int nDirInfoCount = 0;

	while(g_directoryInfo[nDirInfoCount].lpName)
		nDirInfoCount++;

	g_pDirInfo = (tDirInfo *) malloc(sizeof(tDirInfo) * nDirInfoCount);

	if (!g_pDirInfo) 		/* bummer */
		goto error;

	memset(g_pDirInfo, 0, sizeof(tDirInfo) * nDirInfoCount);

	for (i = nDirInfoCount - 1; i >= 0; i--)
	{
		t_s = tstring_from_utf8(g_directoryInfo[i].lpName);

		if(!t_s)
			return FALSE;

		(void)ComboBox_InsertString(GetDlgItem(hDlg, IDC_DIR_COMBO), 0, t_s);
		free(t_s);
		t_s = NULL;
	}

	(void)ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_DIR_COMBO), 0);
	GetClientRect(GetDlgItem(hDlg, IDC_DIR_LIST), &rectClient);

	memset(&LVCol, 0, sizeof(LVCOLUMN));
	LVCol.mask = LVCF_WIDTH;
	LVCol.cx = rectClient.right - rectClient.left;

	(void)ListView_InsertColumn(GetDlgItem(hDlg, IDC_DIR_LIST), 0, &LVCol);

	/* Keep a temporary copy of the directory strings in g_pDirInfo. */
	for (i = 0; i < nDirInfoCount; i++)
	{
		const char *s = g_directoryInfo[i].pfnGetTheseDirs();
		t_s = tstring_from_utf8(s);

		if(!t_s)
			return FALSE;

		if (g_directoryInfo[i].bMulti)
		{
			/* Copy the string to our own buffer so that we can mutilate it */
			_tcscpy(buf, t_s);

			g_pDirInfo[i].m_Path = (tPath*)malloc(sizeof(tPath));

			if (!g_pDirInfo[i].m_Path)
				goto error;

			g_pDirInfo[i].m_Path->m_NumDirectories = 0;
			token = _tcstok(buf, TEXT(";"));

			while ((DirInfo_NumDir(g_pDirInfo, i) < MAX_DIRS) && token)
			{
				_tcscpy(DirInfo_Path(g_pDirInfo, i, DirInfo_NumDir(g_pDirInfo, i)), token);
				DirInfo_NumDir(g_pDirInfo, i)++;
				token = _tcstok(NULL, TEXT(";"));
			}
			
			DirInfo_SetModified(g_pDirInfo, i, FALSE);
		}
		else
		{
			DirInfo_SetDir(g_pDirInfo, i, -1, t_s);
		}

		free(t_s);
		t_s = NULL;
	}

	UpdateDirectoryList(hDlg);
	return TRUE;

error:
	if(t_s)
		free(t_s);

	Directories_OnDestroy(hDlg);
	EndDialog(hDlg, -1);
	return FALSE;
}

static void Directories_OnDestroy(HWND hDlg)
{
	if (g_pDirInfo)
	{
		/* count how many dirinfos there are */
		int nDirInfoCount = 0;

		while(g_directoryInfo[nDirInfoCount].lpName)
			nDirInfoCount++;

		for (int i = 0; i < nDirInfoCount; i++)
		{
			if (g_pDirInfo[i].m_Path)
				free(g_pDirInfo[i].m_Path);
			
			if (g_pDirInfo[i].m_tDirectory)
				free(g_pDirInfo[i].m_tDirectory);
		}

		free(g_pDirInfo);
		g_pDirInfo = NULL;
	}
}

static void Directories_OnClose(HWND hDlg)
{
	EndDialog(hDlg, IDCANCEL);
}

static int RetrieveDirList(int nDir, int nFlagResult, void (*SetTheseDirs)(const char *s))
{
	int nResult = 0;

	if (DirInfo_Modified(g_pDirInfo, nDir))
	{
		TCHAR buf[MAX_PATH * MAX_DIRS];		
		memset(&buf, 0, sizeof(buf));
		int nPaths = DirInfo_NumDir(g_pDirInfo, nDir);

		for (int i = 0; i < nPaths; i++)
		{
			_tcscat(buf, FixSlash(DirInfo_Path(g_pDirInfo, nDir, i)));

			if (i < nPaths - 1)
				_tcscat(buf, TEXT(";"));
		}

		char *utf8_buf = utf8_from_tstring(buf);
		SetTheseDirs(utf8_buf);
		free(utf8_buf);
		nResult |= nFlagResult;
    }
	
	return nResult;
}

static void Directories_OnOk(HWND hDlg)
{
	int nResult = 0;

	for (int i = 0; g_directoryInfo[i].lpName; i++)
	{
		if (g_directoryInfo[i].bMulti)
			nResult |= RetrieveDirList(i, g_directoryInfo[i].nDirDlgFlags, g_directoryInfo[i].pfnSetTheseDirs);
		else
		{
			TCHAR *s = FixSlash(DirInfo_Dir(g_pDirInfo, i));
			char *utf8_s = utf8_from_tstring(s);
			g_directoryInfo[i].pfnSetTheseDirs(utf8_s);
			free(utf8_s);
		}
	}
	
	EndDialog(hDlg, nResult);
}

static void Directories_OnCancel(HWND hDlg)
{
	EndDialog(hDlg, IDCANCEL);
}

static void Directories_OnInsert(HWND hDlg)
{
	HWND hList = GetDlgItem(hDlg, IDC_DIR_LIST);
	int nItem = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
	TCHAR buf[MAX_PATH];

	if (BrowseForDirectory(hDlg, NULL, buf) == TRUE)
	{
		/* list was empty */
		if (nItem == -1)
			nItem = 0;

		int nType = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_DIR_COMBO));

		if (IsMultiDir(nType))
		{
			if (MAX_DIRS <= DirInfo_NumDir(g_pDirInfo, nType))
				return;

			for (int i = DirInfo_NumDir(g_pDirInfo, nType); nItem < i; i--)
				_tcscpy(DirInfo_Path(g_pDirInfo, nType, i), DirInfo_Path(g_pDirInfo, nType, i - 1));

			_tcscpy(DirInfo_Path(g_pDirInfo, nType, nItem), buf);
			DirInfo_NumDir(g_pDirInfo, nType)++;
			DirInfo_SetModified(g_pDirInfo, nType, TRUE);
		}

		UpdateDirectoryList(hDlg);
		ListView_SetItemState(hList, nItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}
}

static void Directories_OnBrowse(HWND hDlg)
{
	TCHAR inbuf[MAX_PATH];
	TCHAR outbuf[MAX_PATH];
	HWND hList = GetDlgItem(hDlg, IDC_DIR_LIST);
	int nItem = ListView_GetNextItem(hList, -1, LVNI_SELECTED);

	if (nItem == -1)
		return;

	int nType = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_DIR_COMBO));

	if (IsMultiDir(nType))
	{
		/* Last item is placeholder for append */
		if (nItem == ListView_GetItemCount(hList) - 1)
		{
			Directories_OnInsert(hDlg);
			return;
		}
	}

	ListView_GetItemText(hList, nItem, 0, inbuf, MAX_PATH);

	if (BrowseForDirectory(hDlg, inbuf, outbuf) == TRUE)
	{
		nType = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_DIR_COMBO));
		DirInfo_SetDir(g_pDirInfo, nType, nItem, outbuf);
		UpdateDirectoryList(hDlg);
	}
}

static void Directories_OnDelete(HWND hDlg)
{
	int nSelect = 0;
	HWND hList = GetDlgItem(hDlg, IDC_DIR_LIST);
	int nItem = ListView_GetNextItem(hList, -1, LVNI_SELECTED | LVNI_ALL);

	if (nItem == -1)
		return;

	/* Don't delete "Append" placeholder. */
	if (nItem == ListView_GetItemCount(hList) - 1)
		return;

	int nType = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_DIR_COMBO));

	if (IsMultiDir(nType))
	{
		for (int i = nItem; i < DirInfo_NumDir(g_pDirInfo, nType) - 1; i++)
			_tcscpy(DirInfo_Path(g_pDirInfo, nType, i), DirInfo_Path(g_pDirInfo, nType, i + 1));

		_tcscpy(DirInfo_Path(g_pDirInfo, nType, DirInfo_NumDir(g_pDirInfo, nType) - 1), TEXT(""));
		DirInfo_NumDir(g_pDirInfo, nType)--;
		DirInfo_SetModified(g_pDirInfo, nType, TRUE);
	}

	UpdateDirectoryList(hDlg);
	int nCount = ListView_GetItemCount(hList);

	if (nCount <= 1)
		return;

	/* If the last item was removed, select the item above. */
	if (nItem == nCount - 1)
		nSelect = nCount - 2;
	else
		nSelect = nItem;

	ListView_SetItemState(hList, nSelect, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

static void Directories_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
		if (codeNotify == BN_CLICKED)
			Directories_OnOk(hDlg);
		break;

	case IDCANCEL:
		if (codeNotify == BN_CLICKED)
			Directories_OnCancel(hDlg);
		break;

	case IDC_DIR_BROWSE:
		if (codeNotify == BN_CLICKED)
			Directories_OnBrowse(hDlg);
		break;

	case IDC_DIR_INSERT:
		if (codeNotify == BN_CLICKED)
			Directories_OnInsert(hDlg);
		break;

	case IDC_DIR_DELETE:
		if (codeNotify == BN_CLICKED)
			Directories_OnDelete(hDlg);
		break;

	case IDC_DIR_COMBO:
		switch (codeNotify)
		{
		case CBN_SELCHANGE:
			Directories_OnSelChange(hDlg);
			break;
		}
		break;
	}
}

/**************************************************************************

    Use the shell to select a Directory.

 **************************************************************************/

static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	/*
        Called just after the dialog is initialized
        Select the dir passed in BROWSEINFO.lParam
    */
	if (uMsg == BFFM_INITIALIZED)
	{
		if ((const char*)lpData != NULL)
			SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	
	return 0;
}

BOOL BrowseForDirectory(HWND hWnd, const TCHAR* pStartDir, TCHAR* pResult)
{
	BOOL bResult = FALSE;
	BROWSEINFO Info;
	LPITEMIDLIST pItemIDList = NULL;
	TCHAR buf[MAX_PATH];

	Info.hwndOwner = hWnd;
	Info.pidlRoot = NULL;
	Info.pszDisplayName = buf;
	Info.lpszTitle = TEXT("Select a directory:");
	Info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	Info.lpfn = BrowseCallbackProc;
	Info.lParam = (LPARAM)pStartDir;

	pItemIDList = SHBrowseForFolder(&Info);

	if (pItemIDList != NULL)
	{
		if (SHGetPathFromIDList(pItemIDList, buf) == TRUE)
		{
			_sntprintf(pResult, MAX_PATH, TEXT("%s"), buf);
			bResult = TRUE;
		}
	}
	else
		bResult = FALSE;

	return bResult;
}
