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

  dialogs.c

  Dialog box procedures go here

***************************************************************************/

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <uxtheme.h>

// standard C headers
#include <string.h>
#include <tchar.h>
#include <stdlib.h>

// MAMEUI headers
#include "bitmask.h"
#include "treeview.h"
#include "resource.h"
#include "mui_opts.h"
#include "mui_util.h"
#include "winui.h"
#include "properties.h"  // For GetHelpIDs

// MAME headers
#include "strconv.h"
#include "winutf8.h"

#ifndef TVS_EX_DOUBLEBUFFER
#define TVS_EX_DOUBLEBUFFER 0x0004
#endif

#ifndef TVM_SETEXTENDEDSTYLE
#define TVM_SETEXTENDEDSTYLE (TV_FIRST + 44)
#endif

#define FILTERTEXT_LEN 256
#define NUM_EXCLUSIONS  10

static struct ComboBoxHistoryTab
{
	const TCHAR*	m_pText;
	const int		m_pData;
} g_ComboBoxHistoryTab[] = 
{
	{ TEXT("Snapshot"),			TAB_SCREENSHOT },
	{ TEXT("Flyer"),			TAB_FLYER },
	{ TEXT("Cabinet"),			TAB_CABINET },
	{ TEXT("Marquee"),			TAB_MARQUEE },
	{ TEXT("Title"),			TAB_TITLE },
	{ TEXT("Control panel"),	TAB_CONTROL_PANEL },
	{ TEXT("PCB"),				TAB_PCB },
	{ TEXT("Scores"),			TAB_SCORES },
	{ TEXT("All"),				TAB_ALL },
	{ TEXT("None"),				TAB_NONE },
};
#define NUMHISTORYTAB ARRAY_LENGTH(g_ComboBoxHistoryTab)

static char g_FilterText[FILTERTEXT_LEN];

/* Pairs of filters that exclude each other */
static DWORD filterExclusion[NUM_EXCLUSIONS] =
{
	IDC_FILTER_CLONES,      IDC_FILTER_ORIGINALS,
	IDC_FILTER_NONWORKING,  IDC_FILTER_WORKING,
	IDC_FILTER_UNAVAILABLE, IDC_FILTER_AVAILABLE,
	IDC_FILTER_RASTER,      IDC_FILTER_VECTOR,
	IDC_FILTER_HORIZONTAL,  IDC_FILTER_VERTICAL
};

static void DisableFilterControls(HWND hWnd, LPCFOLDERDATA lpFilterRecord, LPCFILTER_ITEM lpFilterItem, DWORD dwFlags);
static void EnableFilterExclusions(HWND hWnd, DWORD dwCtrlID);
static DWORD ValidateFilters(LPCFOLDERDATA lpFilterRecord, DWORD dwFlags);
static void OnHScroll(HWND hWnd, HWND hWndCtl, UINT code, int pos);
static UINT_PTR CALLBACK CCHookProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
static void DisableVisualStylesInterface(HWND hDlg);
static void DisableVisualStylesReset(HWND hDlg);
static void DisableVisualStylesFilters(HWND hDlg);

static HICON hIcon;
static HBRUSH hBrush;
static HFONT hFont;
static HFONT hFontFX;
static HDC hDC;
static DWORD dwFilters;
static DWORD dwpFilters;
static LPCFOLDERDATA lpFilterRecord;
static LPTREEFOLDER default_selection = NULL;
static int driver_index;

/***************************************************************************/

const char * GetFilterText(void)
{
	return g_FilterText;
}

INT_PTR CALLBACK ResetDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	char temp[400];
	BOOL resetFilters = FALSE;
	BOOL resetGames = FALSE;
	BOOL resetUI = FALSE;
	BOOL resetDefaults = FALSE;

	switch (Msg)
	{
	case WM_INITDIALOG:
		CenterWindow(hDlg);
        hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		hBrush = GetSysColorBrush(COLOR_WINDOW);
		DisableVisualStylesReset(hDlg);
		return TRUE;

	case WM_CTLCOLORDLG:
		return (LRESULT) hBrush;	
		
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		hDC=(HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		return (LRESULT) hBrush;

	case WM_COMMAND :
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDOK :
			resetFilters = Button_GetCheck(GetDlgItem(hDlg, IDC_RESET_FILTERS));
			resetGames = Button_GetCheck(GetDlgItem(hDlg, IDC_RESET_GAMES));
			resetDefaults = Button_GetCheck(GetDlgItem(hDlg, IDC_RESET_DEFAULT));
			resetUI = Button_GetCheck(GetDlgItem(hDlg, IDC_RESET_UI));

			if (resetFilters || resetGames || resetUI || resetDefaults)
			{
				strcpy(temp, MAMEUINAME);
				strcat(temp, " will now reset the following\n");
				strcat(temp, "to the default settings:\n\n");

				if (resetDefaults)
					strcat(temp, "Global games options\n");

				if (resetGames)
					strcat(temp, "Individual game options\n");

				if (resetFilters)
					strcat(temp, "Custom folder filters\n");

				if (resetUI)
				{
					strcat(temp, "User interface settings\n\n");
					strcat(temp, "Resetting the user interface options\n");
					strcat(temp, "requires exiting ");
					strcat(temp, MAMEUINAME);
					strcat(temp, ".\n");
				}
			
				strcat(temp, "\nDo you wish to continue?");

				if (win_message_box_utf8(hDlg, temp, "Restore settings", MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					if (resetFilters)
						ResetFilters();

					if (resetGames)
						ResetAllGameOptions();

					if (resetDefaults)
						ResetGameDefaults();

					// This is the only case we need to exit and restart for.
					if (resetUI)
					{
						ResetInterface();
						DeleteObject(hBrush);
						DestroyIcon(hIcon);
						EndDialog(hDlg, 1);
						return TRUE;
					} 
					else 
					{
						DeleteObject(hBrush);
						DestroyIcon(hIcon);
						EndDialog(hDlg, 0);
						return TRUE;
					}
				} 
				else 
					// Give the user a chance to change what they want to reset.
					break;
			}
		// Nothing was selected but OK, just fall through
		case IDCANCEL :
			DeleteObject(hBrush);
			DestroyIcon(hIcon);
			EndDialog(hDlg, 0);
			return TRUE;
		}

		break;
	}
	
	return 0;
}

static UINT_PTR CALLBACK CCHookProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{

	switch (Msg)
	{
		case WM_INITDIALOG:
			CenterWindow(hDlg);
        	hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        	SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			break;
	}
	
	return FALSE;
}

INT_PTR CALLBACK InterfaceDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CHOOSECOLOR cc;
	COLORREF choice_colors[16];
	BOOL bRedrawList = FALSE;
	BOOL checked = FALSE;
	char buffer[200];
	int nCurSelection = 0;
	int nHistoryTab = 0;
	int value = 0;
	int i = 0;

	switch (Msg)
	{
	case WM_INITDIALOG:
		CenterWindow(hDlg);
        hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		hBrush = GetSysColorBrush(COLOR_WINDOW);
		DisableVisualStylesInterface(hDlg);
		Button_SetCheck(GetDlgItem(hDlg, IDC_JOY_GUI), GetJoyGUI());
		Button_SetCheck(GetDlgItem(hDlg, IDC_KEY_GUI), GetKeyGUI());
		Button_SetCheck(GetDlgItem(hDlg, IDC_BROADCAST), GetBroadcast());
		Button_SetCheck(GetDlgItem(hDlg, IDC_DISABLE_TRAY_ICON), GetMinimizeTrayIcon());
		Button_SetCheck(GetDlgItem(hDlg, IDC_DISPLAY_NO_ROMS), GetDisplayNoRomsGames());
		Button_SetCheck(GetDlgItem(hDlg, IDC_USE_BROKEN_ICON), GetUseBrokenIcon());
		Button_SetCheck(GetDlgItem(hDlg, IDC_HIDE_MOUSE), GetHideMouseOnStartup());
		Button_SetCheck(GetDlgItem(hDlg, IDC_STRETCH_SCREENSHOT_LARGER), GetStretchScreenShotLarger());
		Button_SetCheck(GetDlgItem(hDlg, IDC_FILTER_INHERIT), GetFilterInherit());
		// Get the current value of the control
		SendMessage(GetDlgItem(hDlg, IDC_CYCLETIMESEC), TBM_SETRANGE, TRUE, MAKELPARAM(0, 60)); /* [0, 60] */
		SendMessage(GetDlgItem(hDlg, IDC_CYCLETIMESEC), TBM_SETTICFREQ, 5, 0);
		value = GetCycleScreenshot();
		SendMessage(GetDlgItem(hDlg, IDC_CYCLETIMESEC), TBM_SETPOS, TRUE, value);
		sprintf(buffer, "%i", value);		
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_CYCLETIMESECTXT), buffer);

		for (i = 0; i < NUMHISTORYTAB; i++)
		{
			(void)ComboBox_InsertString(GetDlgItem(hDlg, IDC_HISTORY_TAB), i, g_ComboBoxHistoryTab[i].m_pText);
			(void)ComboBox_SetItemData(GetDlgItem(hDlg, IDC_HISTORY_TAB), i, g_ComboBoxHistoryTab[i].m_pData);
		}
		
		if (GetHistoryTab() < MAX_TAB_TYPES) 
			(void)ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_HISTORY_TAB), GetHistoryTab());
		else 
			(void)ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_HISTORY_TAB), GetHistoryTab() - TAB_SUBTRACT);

		SendMessage(GetDlgItem(hDlg, IDC_SCREENSHOT_BORDERSIZE), TBM_SETRANGE, TRUE, MAKELPARAM(0, 50)); /* [0, 50] */
		SendMessage(GetDlgItem(hDlg, IDC_SCREENSHOT_BORDERSIZE), TBM_SETTICFREQ, 5, 0);
		value = GetScreenshotBorderSize();
		SendMessage(GetDlgItem(hDlg, IDC_SCREENSHOT_BORDERSIZE), TBM_SETPOS, TRUE, value);
		sprintf(buffer, "%i", value);		
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_SCREENSHOT_BORDERSIZETXT), buffer);
		break;

	case WM_CTLCOLORDLG:
		return (LRESULT) hBrush;	
		
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		hDC=(HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		return (LRESULT) hBrush;

	case WM_HSCROLL:
		HANDLE_WM_HSCROLL(hDlg, wParam, lParam, OnHScroll);
		break;

	case WM_COMMAND :
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDC_SCREENSHOT_BORDERCOLOR:
		{
			for (i = 0; i < 16; i++)
				choice_colors[i] = GetCustomColor(i);

			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hDlg;
			cc.lpfnHook = &CCHookProc;
			cc.rgbResult = GetScreenshotBorderColor();
			cc.lpCustColors = choice_colors;
			cc.Flags = CC_ANYCOLOR | CC_RGBINIT | CC_FULLOPEN | CC_ENABLEHOOK;

			if (!ChooseColor(&cc))
				return TRUE;

			for (i = 0; i < 16; i++)
				SetCustomColor(i,choice_colors[i]);

			SetScreenshotBorderColor(cc.rgbResult);
			UpdateScreenShot();
			return TRUE;
		}

		case IDOK :
		{
			SetJoyGUI(Button_GetCheck(GetDlgItem(hDlg, IDC_JOY_GUI)));
			SetKeyGUI(Button_GetCheck(GetDlgItem(hDlg, IDC_KEY_GUI)));
			SetBroadcast(Button_GetCheck(GetDlgItem(hDlg, IDC_BROADCAST)));
			SetMinimizeTrayIcon(Button_GetCheck(GetDlgItem(hDlg, IDC_DISABLE_TRAY_ICON)));
			SetHideMouseOnStartup(Button_GetCheck(GetDlgItem(hDlg, IDC_HIDE_MOUSE)));

			if(Button_GetCheck(GetDlgItem(hDlg, IDC_RESET_PLAYCOUNT)))
			{
				for (i = 0; i< driver_list::total(); i++)
					ResetPlayCount(i);

				bRedrawList = TRUE;
			}

			if(Button_GetCheck(GetDlgItem(hDlg, IDC_RESET_PLAYTIME)))
			{
				for (i = 0; i< driver_list::total(); i++)
					ResetPlayTime(i);

				bRedrawList = TRUE;
			}

			value = SendDlgItemMessage(hDlg, IDC_CYCLETIMESEC, TBM_GETPOS, 0, 0);
			
			if( GetCycleScreenshot() != value )
				SetCycleScreenshot(value);

			value = SendDlgItemMessage(hDlg, IDC_SCREENSHOT_BORDERSIZE, TBM_GETPOS, 0, 0);
			
			if( GetScreenshotBorderSize() != value )
			{
				SetScreenshotBorderSize(value);
				UpdateScreenShot();
			}

			checked = Button_GetCheck(GetDlgItem(hDlg, IDC_STRETCH_SCREENSHOT_LARGER));
			
			if (checked != GetStretchScreenShotLarger())
			{
				SetStretchScreenShotLarger(checked);
				UpdateScreenShot();
			}

			checked = Button_GetCheck(GetDlgItem(hDlg, IDC_FILTER_INHERIT));
			
			if (checked != GetFilterInherit())
			{
				SetFilterInherit(checked);
				bRedrawList = TRUE;
			}

			checked = Button_GetCheck(GetDlgItem(hDlg, IDC_USE_BROKEN_ICON));
			
			if (checked != GetUseBrokenIcon())
			{
				SetUseBrokenIcon(checked);
				bRedrawList = TRUE;
			}

			checked = Button_GetCheck(GetDlgItem(hDlg, IDC_DISPLAY_NO_ROMS));
			
			if (checked != GetDisplayNoRomsGames())
			{
				SetDisplayNoRomsGames(checked);
				bRedrawList = TRUE;
 			}

			nCurSelection = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_HISTORY_TAB));
			
			if (nCurSelection != CB_ERR)
				nHistoryTab = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_HISTORY_TAB), nCurSelection);

			DeleteObject(hBrush);
			DestroyIcon(hIcon);
			EndDialog(hDlg, 0);

			if(GetHistoryTab() != nHistoryTab)
			{
				SetHistoryTab(nHistoryTab, TRUE);
				ResizePickerControls(GetMainWindow());
				UpdateScreenShot();
			}
			
			if(bRedrawList)
				UpdateListView();
				
			return TRUE;
		}

		case IDCANCEL :
			DeleteObject(hBrush);
			DestroyIcon(hIcon);
			EndDialog(hDlg, 0);
			return TRUE;
		}

		break;
	}

	return 0;
}

INT_PTR CALLBACK FilterDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	char strText[24];
	int i;
	char tmp[80];
	BOOL bShowExplanation = FALSE;
	
	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		CenterWindow(hDlg);
        hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		hBrush = GetSysColorBrush(COLOR_WINDOW);
		DisableVisualStylesFilters(hDlg);
		LPTREEFOLDER folder = GetCurrentFolder();
		LPTREEFOLDER lpParent = NULL;
		LPCFILTER_ITEM g_lpFilterList = GetFilterList();
		dwFilters = 0;

		if (folder != NULL)
		{
			win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_EDIT), g_FilterText);
			Edit_SetSel(GetDlgItem(hDlg, IDC_FILTER_EDIT), 0, -1);
			// Mask out non filter flags
			dwFilters = folder->m_dwFlags & F_MASK;
			// Display current folder name in dialog titlebar
			snprintf(tmp, ARRAY_LENGTH(tmp), "Filters for %s folder", folder->m_lpTitle);
			win_set_window_text_utf8(hDlg, tmp);

			if (GetFilterInherit())
			{
				lpParent = GetFolder(folder->m_nParent);

				if(lpParent)
				{
					/* Check the Parent Filters and inherit them on child,
                     * No need to promote all games to parent folder, works as is */
					dwpFilters = lpParent->m_dwFlags & F_MASK;
					/*Check all possible Filters if inherited solely from parent, e.g. not being set explicitly on our folder*/

					if((dwpFilters & F_CLONES) && !(dwFilters & F_CLONES))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_CLONES), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_CLONES), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_NONWORKING) && !(dwFilters & F_NONWORKING))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_NONWORKING), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_NONWORKING), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_UNAVAILABLE) && !(dwFilters & F_UNAVAILABLE))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_UNAVAILABLE), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_UNAVAILABLE), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_VECTOR) && !(dwFilters & F_VECTOR))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_VECTOR), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_VECTOR), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_RASTER) && !(dwFilters & F_RASTER))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_RASTER), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_RASTER), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_ORIGINALS) && !(dwFilters & F_ORIGINALS))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_ORIGINALS), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_ORIGINALS), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_WORKING) && !(dwFilters & F_WORKING))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_WORKING), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_WORKING), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_AVAILABLE) && !(dwFilters & F_AVAILABLE))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_AVAILABLE), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_AVAILABLE), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_HORIZONTAL) && !(dwFilters & F_HORIZONTAL))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_HORIZONTAL), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_HORIZONTAL), strText);
						bShowExplanation = TRUE;
					}

					if((dwpFilters & F_VERTICAL) && !(dwFilters & F_VERTICAL))
					{
						/*Add a Specifier to the Checkbox to show it was inherited from the parent*/
						win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_VERTICAL), strText, 20);
						strcat(strText, " (*)");
						win_set_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_VERTICAL), strText);
						bShowExplanation = TRUE;
					}
					/*Do not or in the Values of the parent, so that the values of the folder still can be set*/
					//dwFilters |= dwpFilters;
				}

				if(bShowExplanation)
				{
					ShowWindow(GetDlgItem(hDlg, IDC_INHERITED), TRUE);
				}
			}
			else
			{
				ShowWindow(GetDlgItem(hDlg, IDC_INHERITED), FALSE);
			}
			// Find the matching filter record if it exists
			lpFilterRecord = FindFilter(folder->m_nFolderId);

			// initialize and disable appropriate controls
			for (i = 0; g_lpFilterList[i].m_dwFilterType; i++)
			{
				DisableFilterControls(hDlg, lpFilterRecord, &g_lpFilterList[i], dwFilters);
			}
		}
		
		SetFocus(GetDlgItem(hDlg, IDC_FILTER_EDIT));
		return FALSE;
	}
	
	case WM_CTLCOLORDLG:
		return (LRESULT) hBrush;	
		
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLOREDIT:
		hDC=(HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		return (LRESULT) hBrush;

	case WM_COMMAND:
	{
		WORD wID = GET_WM_COMMAND_ID(wParam, lParam);
		WORD wNotifyCode = GET_WM_COMMAND_CMD(wParam, lParam);
		LPTREEFOLDER folder = GetCurrentFolder();
		LPCFILTER_ITEM g_lpFilterList = GetFilterList();

		switch (wID)
		{
		case IDOK:
			dwFilters = 0;
			win_get_window_text_utf8(GetDlgItem(hDlg, IDC_FILTER_EDIT), g_FilterText, FILTERTEXT_LEN);

			// see which buttons are checked
			for (i = 0; g_lpFilterList[i].m_dwFilterType; i++)
			{
				if (Button_GetCheck(GetDlgItem(hDlg, g_lpFilterList[i].m_dwCtrlID)))
					dwFilters |= g_lpFilterList[i].m_dwFilterType;
			}

			// Mask out invalid filters
			dwFilters = ValidateFilters(lpFilterRecord, dwFilters);
			// Keep non filter flags
			folder->m_dwFlags &= ~F_MASK;
			// put in the set filters
			folder->m_dwFlags |= dwFilters;
			DestroyIcon(hIcon);
			DeleteObject(hBrush);
			EndDialog(hDlg, 1);
			return TRUE;

		case IDCANCEL:
			DestroyIcon(hIcon);
			DeleteObject(hBrush);
			EndDialog(hDlg, 0);
			return TRUE;

		default:
			// Handle unchecking mutually exclusive filters
			if (wNotifyCode == BN_CLICKED)
				EnableFilterExclusions(hDlg, wID);
		}
	}
	
	break;
	}
	
	return 0;
}

INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	char tmp[80];
	
	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		CenterWindow(hDlg);
		hBrush = GetSysColorBrush(COLOR_3DFACE);
		HBITMAP hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SPLASH), IMAGE_BITMAP, 0, 0, LR_SHARED);
		SendMessage(GetDlgItem(hDlg, IDC_ABOUT), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
		hFont = CreateFont(-11, 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, TEXT("Verdana"));
		hFontFX = CreateFont(-12, 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, TEXT("Verdana"));
		SetWindowFont(GetDlgItem(hDlg, IDC_TEXT1), hFont, TRUE);
		SetWindowFont(GetDlgItem(hDlg, IDC_TEXT2), hFont, TRUE);
		SetWindowFont(GetDlgItem(hDlg, IDC_TEXT3), hFont, TRUE);
		SetWindowFont(GetDlgItem(hDlg, IDC_TEXT4), hFont, TRUE);
		SetWindowFont(GetDlgItem(hDlg, IDC_SICKFX), hFontFX, TRUE);
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_BUILD), "Build time: "__DATE__" - "__TIME__"");
		sprintf(tmp, "Version: %s", bare_build_version);
		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_BUILDVER), tmp);
		return TRUE;
	}
	
	case WM_CTLCOLORDLG:
		return (LRESULT) hBrush;

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		hDC=(HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));

		if ((HWND)lParam == GetDlgItem(hDlg, IDC_SICKFX))
			SetTextColor(hDC, RGB(63, 72, 204));
	
		return (LRESULT) hBrush;
		
	case WM_COMMAND:
		DeleteObject(hFont);
		DeleteObject(hFontFX);
		DeleteObject(hBrush);
		DestroyIcon(hIcon);
		EndDialog(hDlg, 0);
		return TRUE;
	}
	
	return FALSE;
}

INT_PTR CALLBACK AddCustomFileDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
	{
		CenterWindow(hDlg);
        hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		if (IsWindowsSevenOrHigher())
			SendMessage(GetDlgItem(hDlg, IDC_CUSTOM_TREE), TVM_SETEXTENDEDSTYLE, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);

		SetWindowTheme(GetDlgItem(hDlg, IDC_CUSTOM_TREE), L"Explorer", NULL);

	    TREEFOLDER **folders;
		int num_folders;
		int i;
		TVINSERTSTRUCT tvis;
		TVITEM tvi;
		BOOL first_entry = TRUE;
		HIMAGELIST treeview_icons = GetTreeViewIconList();

		// current game passed in using DialogBoxParam()
		driver_index = lParam;
		(void)TreeView_SetImageList(GetDlgItem(hDlg, IDC_CUSTOM_TREE), treeview_icons, LVSIL_NORMAL);
		GetFolders(&folders,&num_folders);

		// insert custom folders into our tree view
		for (i = 0; i < num_folders; i++)
		{
		    if (folders[i]->m_dwFlags & F_CUSTOM)
			{
			    HTREEITEM hti;
				int jj;

				if (folders[i]->m_nParent == -1)
				{
					memset(&tvi, '\0', sizeof(tvi));
				    tvis.hParent = TVI_ROOT;
					tvis.hInsertAfter = TVI_SORT;
					tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					tvi.pszText = folders[i]->m_lptTitle;
					tvi.lParam = (LPARAM)folders[i];
					tvi.iImage = GetTreeViewIconIndex(folders[i]->m_nIconId);
					tvi.iSelectedImage = 0;
					tvis.item = tvi;

					hti = TreeView_InsertItem(GetDlgItem(hDlg, IDC_CUSTOM_TREE), &tvis);

					/* look for children of this custom folder */
					for (jj = 0; jj < num_folders; jj++)
					{
					    if (folders[jj]->m_nParent == i)
						{
						    HTREEITEM hti_child;
						   	tvis.hParent = hti;
							tvis.hInsertAfter = TVI_SORT;
							tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
							tvi.pszText = folders[jj]->m_lptTitle;
							tvi.lParam = (LPARAM)folders[jj];
							tvi.iImage = GetTreeViewIconIndex(folders[jj]->m_nIconId);
							tvi.iSelectedImage = 0;
					        tvis.item = tvi;

							hti_child = TreeView_InsertItem(GetDlgItem(hDlg, IDC_CUSTOM_TREE), &tvis);
							
							if (folders[jj] == default_selection)
								(void)TreeView_SelectItem(GetDlgItem(hDlg, IDC_CUSTOM_TREE), hti_child);
						}
					}

					/*TreeView_Expand(GetDlgItem(hDlg,IDC_CUSTOM_TREE),hti,TVE_EXPAND);*/
					if (first_entry || folders[i] == default_selection)
					{
					    (void)TreeView_SelectItem(GetDlgItem(hDlg, IDC_CUSTOM_TREE),hti);
						first_entry = FALSE;
					}
				}
			}
		}

		win_set_window_text_utf8(GetDlgItem(hDlg, IDC_CUSTOMFILE_GAME), driver_list::driver(driver_index).description);
		return TRUE;
	}
	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{
		case IDOK:
		{
			TVITEM tvi;
		   	tvi.hItem = TreeView_GetSelection(GetDlgItem(hDlg, IDC_CUSTOM_TREE));
		   	tvi.mask = TVIF_PARAM;

		   	if (TreeView_GetItem(GetDlgItem(hDlg, IDC_CUSTOM_TREE),&tvi) == TRUE)
		   	{
				/* should look for New... */
			  	default_selection = (LPTREEFOLDER)tvi.lParam; 	/* start here next time */
			  	AddToCustomFolder((LPTREEFOLDER)tvi.lParam,driver_index);
		   	}

			DestroyIcon(hIcon);
		   	EndDialog(hDlg, 0);
		   	return TRUE;
		}
		
		case IDCANCEL:
			DestroyIcon(hIcon);
			EndDialog(hDlg, 0);
			return TRUE;
		}

		break;
	}
	
	return FALSE;
}

/***************************************************************************
    private functions
 ***************************************************************************/

static void DisableFilterControls(HWND hWnd, LPCFOLDERDATA lpFilterRecord, LPCFILTER_ITEM lpFilterItem, DWORD dwFlags)
{
	HWND  hWndCtrl = GetDlgItem(hWnd, lpFilterItem->m_dwCtrlID);
	DWORD dwFilterType = lpFilterItem->m_dwFilterType;

	/* Check the appropriate control */
	if (dwFilterType & dwFlags)
		Button_SetCheck(hWndCtrl, MF_CHECKED);

	/* No special rules for this folder? */
	if (!lpFilterRecord)
		return;

	/* If this is an excluded filter */
	if (lpFilterRecord->m_dwUnset & dwFilterType)
	{
		/* uncheck it and disable the control */
		Button_SetCheck(hWndCtrl, MF_UNCHECKED);
		EnableWindow(hWndCtrl, FALSE);
	}

	/* If this is an implied filter, check it and disable the control */
	if (lpFilterRecord->m_dwSet & dwFilterType)
	{
		Button_SetCheck(hWndCtrl, MF_CHECKED);
		EnableWindow(hWndCtrl, FALSE);
	}
}

// Handle disabling mutually exclusive controls
static void EnableFilterExclusions(HWND hWnd, DWORD dwCtrlID)
{
	int i;
	DWORD id;

	for (i = 0; i < NUM_EXCLUSIONS; i++)
	{
		// is this control in the list?
		if (filterExclusion[i] == dwCtrlID)
			// found the control id
			break;
	}

	// if the control was found
	if (i < NUM_EXCLUSIONS)
	{
		// find the opposing control id
		if (i % 2)
			id = filterExclusion[i - 1];
		else
			id = filterExclusion[i + 1];

		// Uncheck the other control
		Button_SetCheck(GetDlgItem(hWnd, id), MF_UNCHECKED);
	}
}

// Validate filter setting, mask out inappropriate filters for this folder
static DWORD ValidateFilters(LPCFOLDERDATA lpFilterRecord, DWORD dwFlags)
{
	DWORD dwFilters;

	if (lpFilterRecord != (LPFOLDERDATA)0)
	{
		// Mask out implied and excluded filters
		dwFilters = lpFilterRecord->m_dwSet | lpFilterRecord->m_dwUnset;
		return dwFlags & ~dwFilters;
	}

	// No special cases - all filters apply
	return dwFlags;
}

static void OnHScroll(HWND hWnd, HWND hWndCtl, UINT code, int pos)
{
	int value;
	char tmp[4];

	if (hWndCtl == GetDlgItem(hWnd, IDC_CYCLETIMESEC))
	{
		value = SendMessage(GetDlgItem(hWnd, IDC_CYCLETIMESEC), TBM_GETPOS, 0, 0);
		sprintf(tmp, "%i", value);
		win_set_window_text_utf8(GetDlgItem(hWnd, IDC_CYCLETIMESECTXT), tmp);
	}
	else if (hWndCtl == GetDlgItem(hWnd, IDC_SCREENSHOT_BORDERSIZE))
	{
		value = SendMessage(GetDlgItem(hWnd, IDC_SCREENSHOT_BORDERSIZE), TBM_GETPOS, 0, 0);
		sprintf(tmp, "%i", value);
		win_set_window_text_utf8(GetDlgItem(hWnd, IDC_SCREENSHOT_BORDERSIZETXT), tmp);
	}
}

static void DisableVisualStylesInterface(HWND hDlg)
{
	SetWindowTheme(GetDlgItem(hDlg, IDC_HIDE_MOUSE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_DISABLE_TRAY_ICON), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_JOY_GUI), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_KEY_GUI), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_BROADCAST), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_USE_BROKEN_ICON), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_INHERIT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_DISPLAY_NO_ROMS), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_HISTORY_TAB), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_RESET_PLAYCOUNT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_RESET_PLAYTIME), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_STRETCH_SCREENSHOT_LARGER), L" ", L" ");
}

static void DisableVisualStylesReset(HWND hDlg)
{
	SetWindowTheme(GetDlgItem(hDlg, IDC_RESET_UI), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_RESET_DEFAULT), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_RESET_GAMES), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_RESET_FILTERS), L" ", L" ");
}

static void DisableVisualStylesFilters(HWND hDlg)
{
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_VECTOR), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_CLONES), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_NONWORKING), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_HORIZONTAL), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_UNAVAILABLE), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_RASTER), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_ORIGINALS), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_WORKING), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_VERTICAL), L" ", L" ");
	SetWindowTheme(GetDlgItem(hDlg, IDC_FILTER_AVAILABLE), L" ", L" ");
}