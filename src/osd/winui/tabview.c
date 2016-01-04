/***************************************************************************

  M.A.M.E.UI  -  Multiple Arcade Machine Emulator with User Interface
  Win32 Portions Copyright (C) 1997-2003 Michael Soderstrom and Chris Kirmse,
  Copyright (C) 2003-2007 Chris Kirmse and the MAME32/MAMEUI team.

  This file is part of MAMEUI, and may only be used, modified and
  distributed under the terms of the MAME license, in "readme.txt".
  By continuing to use, modify or distribute this file you indicate
  that you have read the license and understand and accept it fully.

 ***************************************************************************/

#include "winui.h"

struct TabViewInfo
{
	const struct TabViewCallbacks *pCallbacks;
	int nTabCount;
	WNDPROC pfnParentWndProc;
};

static struct TabViewInfo *GetTabViewInfo(HWND hWnd)
{
	LONG_PTR l;
	l = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	return (struct TabViewInfo *) l;
}

static LRESULT CallParentWndProc(WNDPROC pfnParentWndProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT rc;

	if (!pfnParentWndProc)
		pfnParentWndProc = GetTabViewInfo(hWnd)->pfnParentWndProc;

	if (IsWindowUnicode(hWnd))
		rc = CallWindowProcW(pfnParentWndProc, hWnd, message, wParam, lParam);
	else
		rc = CallWindowProcA(pfnParentWndProc, hWnd, message, wParam, lParam);

	return rc;
}

static LRESULT CALLBACK TabViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	struct TabViewInfo *pTabViewInfo;
	WNDPROC pfnParentWndProc;
	BOOL bHandled = FALSE;
	LRESULT rc = 0;

	pTabViewInfo = GetTabViewInfo(hWnd);
	pfnParentWndProc = pTabViewInfo->pfnParentWndProc;

	switch(message)
	{
		case WM_DESTROY:
			free(pTabViewInfo);
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR) pfnParentWndProc);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR) NULL);
			break;
	}

	if (!bHandled)
		rc = CallParentWndProc(pfnParentWndProc, hWnd, message, wParam, lParam);

	switch(message)
	{
		case WM_MOVE:
		case WM_SIZE:
			if (pTabViewInfo->pCallbacks->pfnOnMoveSize)
				pTabViewInfo->pCallbacks->pfnOnMoveSize();
			break;
	}

	return rc;
}

static int TabView_GetTabFromTabIndex(HWND hWndTabView, int tab_index)
{
	int shown_tabs = -1;
	int i;
	struct TabViewInfo *pTabViewInfo;

	pTabViewInfo = GetTabViewInfo(hWndTabView);

	for (i = 0; i < pTabViewInfo->nTabCount; i++)
	{
		if (!pTabViewInfo->pCallbacks->pfnGetShowTab || pTabViewInfo->pCallbacks->pfnGetShowTab(i))
		{
			shown_tabs++;

			if (shown_tabs == tab_index)
				return i;
		}
	}
	
	return 0;
}

int TabView_GetCurrentTab(HWND hWndTabView)
{
	struct TabViewInfo *pTabViewInfo;
	LPCSTR pszTab = NULL;
	LPCSTR pszThatTab;
	int i, nTab = -1;

	pTabViewInfo = GetTabViewInfo(hWndTabView);

	if (pTabViewInfo->pCallbacks->pfnGetCurrentTab)
		pszTab = pTabViewInfo->pCallbacks->pfnGetCurrentTab();

	if (pszTab)
	{
		if (pTabViewInfo->pCallbacks->pfnGetTabShortName)
		{
			for (i = 0; i < pTabViewInfo->nTabCount; i++)
			{
				pszThatTab = pTabViewInfo->pCallbacks->pfnGetTabShortName(i);
				
				if (pszThatTab && !core_stricmp(pszTab, pszThatTab))
				{
					nTab = i;
					break;
				}
			}
		}

		if (nTab < 0)
		{
			nTab = 0;
			sscanf(pszTab, "%d", &nTab);
		}
	}
	else
	{
		nTab = 0;
	}

	return nTab;
}

void TabView_SetCurrentTab(HWND hWndTabView, int nTab)
{
	struct TabViewInfo *pTabViewInfo;
	LPCSTR pszName;
	char szBuffer[16];

	pTabViewInfo = GetTabViewInfo(hWndTabView);

	if (pTabViewInfo->pCallbacks->pfnGetTabShortName)
	{
		pszName = pTabViewInfo->pCallbacks->pfnGetTabShortName(nTab);
	}
	else
	{
		snprintf(szBuffer, ARRAY_LENGTH(szBuffer), "%d", nTab);
		pszName = szBuffer;
	}

	if (pTabViewInfo->pCallbacks->pfnSetCurrentTab)
		pTabViewInfo->pCallbacks->pfnSetCurrentTab(pszName);
}

static int TabView_GetCurrentTabIndex(HWND hWndTabView)
{
	int shown_tabs = 0;
	int i;
	int nCurrentTab;
	struct TabViewInfo *pTabViewInfo;

	pTabViewInfo = GetTabViewInfo(hWndTabView);
	nCurrentTab = TabView_GetCurrentTab(hWndTabView);

	for (i = 0; i < pTabViewInfo->nTabCount; i++)
	{
		if (i == nCurrentTab)
			break;

		if (!pTabViewInfo->pCallbacks->pfnGetShowTab || pTabViewInfo->pCallbacks->pfnGetShowTab(i))
			shown_tabs++;
	}
	
	return shown_tabs;
}

void TabView_UpdateSelection(HWND hWndTabView)
{
	(void)TabCtrl_SetCurSel(hWndTabView, TabView_GetCurrentTabIndex(hWndTabView));
}

BOOL TabView_HandleNotify(LPNMHDR lpNmHdr)
{
	HWND hWndTabView;
	struct TabViewInfo *pTabViewInfo;
	BOOL bResult = FALSE;
	int nTabIndex, nTab;

	hWndTabView = lpNmHdr->hwndFrom;
	pTabViewInfo = GetTabViewInfo(hWndTabView);

	switch (lpNmHdr->code)
	{
		case TCN_SELCHANGE:
			nTabIndex = TabCtrl_GetCurSel(hWndTabView);
			nTab = TabView_GetTabFromTabIndex(hWndTabView, nTabIndex);
			TabView_SetCurrentTab(hWndTabView, nTab);
			
			if (pTabViewInfo->pCallbacks->pfnOnSelectionChanged)
				pTabViewInfo->pCallbacks->pfnOnSelectionChanged();
			
			bResult = TRUE;
			break;
	}
	
	return bResult;
}

void TabView_CalculateNextTab(HWND hWndTabView)
{
	struct TabViewInfo *pTabViewInfo;
	int i;
	int nCurrentTab;

	pTabViewInfo = GetTabViewInfo(hWndTabView);

	// at most loop once through all options
	for (i = 0; i < pTabViewInfo->nTabCount; i++)
	{
		nCurrentTab = TabView_GetCurrentTab(hWndTabView);
		TabView_SetCurrentTab(hWndTabView, (nCurrentTab + 1) % pTabViewInfo->nTabCount);
		nCurrentTab = TabView_GetCurrentTab(hWndTabView);

		if (!pTabViewInfo->pCallbacks->pfnGetShowTab || pTabViewInfo->pCallbacks->pfnGetShowTab(nCurrentTab))
			// this tab is being shown, so we're all set
			return;
	}
}

void TabView_Reset(HWND hWndTabView)
{
	struct TabViewInfo *pTabViewInfo;
	TCITEM tci;
	int i;
	TCHAR *t_text;

	pTabViewInfo = GetTabViewInfo(hWndTabView);
	(void)TabCtrl_DeleteAllItems(hWndTabView);

	memset(&tci, 0, sizeof(TCITEM));
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.cchTextMax = 20;

	for (i = 0; i < pTabViewInfo->nTabCount; i++)
	{
		if (!pTabViewInfo->pCallbacks->pfnGetShowTab || pTabViewInfo->pCallbacks->pfnGetShowTab(i))
		{
			t_text = tstring_from_utf8(pTabViewInfo->pCallbacks->pfnGetTabLongName(i));

			if( !t_text )
				return;

			tci.pszText = t_text;
			tci.iImage = i;
			(void)TabCtrl_InsertItem(hWndTabView, i, &tci);
			free(t_text);
		}
	}
	
	TabView_UpdateSelection(hWndTabView);
}

BOOL SetupTabView(HWND hWndTabView, const struct TabViewOptions *pOptions)
{
	struct TabViewInfo *pTabViewInfo;
	LONG_PTR l;
	BOOL bShowTabView;

	assert(hWndTabView);
	// Allocate the list view struct
	pTabViewInfo = (struct TabViewInfo *) malloc(sizeof(struct TabViewInfo));

	if (!pTabViewInfo)
		return FALSE;

	// And fill it out
	memset(pTabViewInfo, 0, sizeof(*pTabViewInfo));
	pTabViewInfo->pCallbacks = pOptions->pCallbacks;
	pTabViewInfo->nTabCount = pOptions->nTabCount;
	// Hook in our wndproc and userdata pointer
	l = GetWindowLongPtr(hWndTabView, GWLP_WNDPROC);
	pTabViewInfo->pfnParentWndProc = (WNDPROC) l;
	SetWindowLongPtr(hWndTabView, GWLP_USERDATA, (LONG_PTR) pTabViewInfo);
	SetWindowLongPtr(hWndTabView, GWLP_WNDPROC, (LONG_PTR) TabViewWndProc);
	bShowTabView = pTabViewInfo->pCallbacks->pfnGetShowTabCtrl ? pTabViewInfo->pCallbacks->pfnGetShowTabCtrl() : TRUE;
	ShowWindow(hWndTabView, bShowTabView ? SW_SHOW : SW_HIDE);
	TabView_Reset(hWndTabView);
	
	if (pTabViewInfo->pCallbacks->pfnOnSelectionChanged)
		pTabViewInfo->pCallbacks->pfnOnSelectionChanged();

	return TRUE;
}
