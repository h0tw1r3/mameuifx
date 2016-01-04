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

  mui_audit.c

  Audit dialog

***************************************************************************/

#include "winui.h"

/***************************************************************************
    function prototypes
 ***************************************************************************/

static DWORD WINAPI AuditThreadProc(LPVOID hDlg);
static INT_PTR CALLBACK AuditWindowProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
static void ProcessNextRom(void);
static void ProcessNextSample(void);
static void DetailsPrintf(const char *fmt, ...);
static const char * StatusString(int iStatus);

/***************************************************************************
    Internal variables
 ***************************************************************************/

static HWND hAudit = NULL;
static int rom_index = 0;
static int roms_correct = 0;
static int roms_incorrect = 0;
static int roms_notfound = 0;
static int sample_index = 0;
static int samples_correct = 0;
static int samples_incorrect = 0;
static int samples_notfound = 0;
static int audit_color = 0;
static int audit_samples = 0;
static BOOL bCancel = FALSE;
static HICON audit_icon = NULL;
static HICON hIcon = NULL;
static HBRUSH hBrush = NULL;
static HDC hDC = NULL;
static HANDLE hThread = NULL;
static HFONT hFont = NULL;

/***************************************************************************
    External functions
 ***************************************************************************/

void AuditDialog(HWND hParent)
{
	rom_index         = 0;
	roms_correct      = 0;
	roms_incorrect    = 0;
	roms_notfound	  = 0;
	sample_index      = 0;
	samples_correct   = 0;
	samples_incorrect = 0;
	samples_notfound  = 0;
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_AUDIT), hParent, AuditWindowProc);
}

void InitGameAudit(int gameIndex)
{
	rom_index = gameIndex;
}

BOOL IsAuditResultYes(int audit_result)
{
	return audit_result == media_auditor::CORRECT || audit_result == media_auditor::BEST_AVAILABLE || audit_result == media_auditor::NONE_NEEDED;
}

BOOL IsAuditResultNo(int audit_result)
{
	return audit_result == media_auditor::NOTFOUND || audit_result == media_auditor::INCORRECT;
}

/***************************************************************************
    Internal functions
 ***************************************************************************/
// Verifies the ROM set while calling SetRomAuditResults
int MameUIVerifyRomSet(int game, bool choice)
{
	driver_enumerator enumerator(MameUIGlobal(), driver_list::driver(game));
	enumerator.next();
	media_auditor auditor(enumerator);
	media_auditor::summary summary = auditor.audit_media(AUDIT_VALIDATE_FAST);
	std::string summary_string;

	if (choice)
		auditor.winui_summarize(driver_list::driver(game).name, &summary_string); // audit all games
	else
		auditor.summarize(driver_list::driver(game).name, &summary_string); // audit one game

	// output the summary of the audit
	DetailsPrintf("%s", summary_string.c_str());
	SetRomAuditResults(game, summary);
	return summary;
}

// Verifies the Sample set while calling SetSampleAuditResults
int MameUIVerifySampleSet(int game)
{
	driver_enumerator enumerator(MameUIGlobal(), driver_list::driver(game));
	enumerator.next();
	media_auditor auditor(enumerator);
	media_auditor::summary summary = auditor.audit_samples();
	std::string summary_string;
	
	if (summary == media_auditor::NOTFOUND)
		strcatprintf(summary_string, "%-12s: SAMPLES NOT FOUND\n", driver_list::driver(game).name);
	else
		auditor.summarize(driver_list::driver(game).name, &summary_string);
	
	// output the summary of the audit
	DetailsPrintf("%s", summary_string.c_str());
	return summary;
}

static DWORD WINAPI AuditThreadProc(LPVOID hDlg)
{
	char buffer[200];

	while (!bCancel)
	{
		if (rom_index != -1)
		{
			snprintf(buffer, ARRAY_LENGTH(buffer), "Checking game... - %s", driver_list::driver(rom_index).name);
			win_set_window_text_utf8((HWND)hDlg, buffer);
			ProcessNextRom();
		}
		else
		{
			if (sample_index != -1)
			{
				snprintf(buffer, ARRAY_LENGTH(buffer), "Checking game... - %s", driver_list::driver(sample_index).name);
				win_set_window_text_utf8((HWND)hDlg, buffer);
				ProcessNextSample();
			}
			else
			{
				win_set_window_text_utf8((HWND)hDlg, "Audit process completed");
				ExitThread(1);
			}
		}
	}

	return 0;
}

static INT_PTR CALLBACK AuditWindowProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	DWORD dwExitCode = 0;
	bCancel = FALSE;
	
	switch (Msg)
	{
	case WM_INITDIALOG:
		hAudit = hDlg;
		CenterWindow(hAudit);
        hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MAMEUI_ICON));
        SendMessage(hAudit, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		hBrush = CreateSolidBrush(RGB(224, 223, 227));
		hFont = CreateFont(-11, 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, TEXT("Lucida Console"));
		SetWindowFont(GetDlgItem(hAudit, IDC_AUDIT_DETAILS), hFont, TRUE);
		SetWindowTheme(GetDlgItem(hAudit, IDC_AUDIT_DETAILS), L" ", L" ");
		SetWindowTheme(GetDlgItem(hAudit, IDC_ROMS_PROGRESS), L" ", L" ");
		SetWindowTheme(GetDlgItem(hAudit, IDC_SAMPLES_PROGRESS), L" ", L" ");
		SendMessage(GetDlgItem(hAudit, IDC_ROMS_PROGRESS), PBM_SETBARCOLOR, 0, RGB(85, 191, 132));
		SendMessage(GetDlgItem(hAudit, IDC_SAMPLES_PROGRESS), PBM_SETBARCOLOR, 0, RGB(85, 191, 132));
		SendMessage(GetDlgItem(hAudit, IDC_ROMS_PROGRESS), PBM_SETRANGE, 0, MAKELPARAM(0, driver_list::total()));
		SendMessage(GetDlgItem(hAudit, IDC_SAMPLES_PROGRESS), PBM_SETRANGE, 0, MAKELPARAM(0, driver_list::total()));
		rom_index = 0;
		hThread = CreateThread(NULL, 0, AuditThreadProc, hAudit, 0, 0);
		return 1;
	
	case WM_CTLCOLORDLG:
		return (LRESULT) hBrush;	
		
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_ROMS_CORRECT))
			SetTextColor(hDC, RGB(34, 177, 76));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_ROMS_INCORRECT))
			SetTextColor(hDC, RGB(198, 188, 0));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_ROMS_NOTFOUND))
			SetTextColor(hDC, RGB(237, 28, 36));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_ROMS_TOTAL))
			SetTextColor(hDC, RGB(63, 72, 204));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_SAMPLES_CORRECT))
			SetTextColor(hDC, RGB(34, 177, 76));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_SAMPLES_INCORRECT))
			SetTextColor(hDC, RGB(198, 188, 0));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_SAMPLES_NOTFOUND))
			SetTextColor(hDC, RGB(237, 28, 36));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_SAMPLES_TOTAL))
			SetTextColor(hDC, RGB(63, 72, 204));
			
		return (LRESULT) hBrush;
		
	case WM_CTLCOLOREDIT:
		hDC = (HDC)wParam;
		SetTextColor(hDC, RGB(136, 0, 21));
		return (LRESULT) GetStockObject(WHITE_BRUSH);
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			if (hThread)
			{
				bCancel = TRUE;
	
				if (GetExitCodeThread(hThread, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
				{
					PostMessage(hAudit, WM_COMMAND, wParam, lParam);
					return 1;
				}

				CloseHandle(hThread);
			}

			DeleteObject(hBrush);
			DeleteObject(hFont);
			DestroyIcon(hIcon);
			EndDialog(hAudit, 0);
			break;
		}
		
		return 1;
	}
	
	return 0;
}

/* Callback for the Audit property sheet */
INT_PTR CALLBACK GameAuditDialogProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		{
		const game_driver *game = &driver_list::driver(rom_index);
		machine_config config(*game, MameUIGlobal());
		char buffer[4096];
		char details[4096];
		UINT32 crctext = 0;

		zip_file_cache_clear();
		memset(&buffer, 0, sizeof(buffer));
		memset(&details, 0, sizeof(details));
		ModifyPropertySheetForTreeSheet(hDlg);
		hAudit = hDlg;
		hBrush = CreateSolidBrush(RGB(224, 223, 227));
		hFont = CreateFont(-11, 0, 0, 0, 400, 0, 0, 0, 0, 3, 2, 1, 34, TEXT("Lucida Console"));
		SetWindowFont(GetDlgItem(hAudit, IDC_ROM_DETAILS), hFont, TRUE);
		SetWindowFont(GetDlgItem(hAudit, IDC_AUDIT_DETAILS_PROP), hFont, TRUE);
		SetWindowTheme(GetDlgItem(hAudit, IDC_AUDIT_DETAILS_PROP), L" ", L" ");
		SetWindowTheme(GetDlgItem(hAudit, IDC_ROM_DETAILS), L" ", L" ");
		win_set_window_text_utf8(GetDlgItem(hAudit, IDC_PROP_TITLE), GameInfoTitle(OPTIONS_GAME, rom_index));
		int iStatus = MameUIVerifyRomSet(rom_index, 0);
		
		switch (iStatus)
		{
		case media_auditor::CORRECT:
		case media_auditor::BEST_AVAILABLE:
		case media_auditor::NONE_NEEDED:
			audit_icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_AUDIT_PASS));
			break;

		case media_auditor::NOTFOUND:
		case media_auditor::INCORRECT:
			audit_icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_AUDIT_FAIL));
			break;
		}
		
		SendMessage(GetDlgItem(hAudit, IDC_AUDIT_ICON), STM_SETICON, (WPARAM)audit_icon, 0);
		const char *lpStatus = StatusString(iStatus);
		win_set_window_text_utf8(GetDlgItem(hAudit, IDC_PROP_ROMS), lpStatus);

		if (DriverUsesSamples(rom_index))
		{
			iStatus = MameUIVerifySampleSet(rom_index);
			lpStatus = StatusString(iStatus);
		}
		else
		{
			lpStatus = "None required";
			audit_samples = 2;
		}
		
		win_set_window_text_utf8(GetDlgItem(hAudit, IDC_PROP_SAMPLES), lpStatus);
		snprintf(buffer, ARRAY_LENGTH(buffer), "NAME                SIZE       CRC\n"
			"---------------------------------------\n");
		strcat(details, buffer);
		device_iterator deviter(config.root_device());

		for (device_t *device = deviter.first(); device; device = deviter.next())
		{
			for (const rom_entry *region = rom_first_region(*device); region; region = rom_next_region(region))
			{
				for (const rom_entry *rom = rom_first_file(region); rom; rom = rom_next_file(rom))
				{
					UINT32 crc = 0;
					
					if (hash_collection(ROM_GETHASHDATA(rom)).crc(crc))
						crctext = crc;
					else
						crctext = 0;
					
					snprintf(buffer, ARRAY_LENGTH(buffer), "%-18s  %09d  %08x\n", ROM_GETNAME(rom), ROM_GETLENGTH(rom), crctext);
					strcat(details, buffer);
				}
			}
		}
		
		win_set_window_text_utf8(GetDlgItem(hAudit, IDC_ROM_DETAILS), ConvertToWindowsNewlines(details));
		ShowWindow(hAudit, SW_SHOW);
		return 1;
		}
		
	case WM_CTLCOLORDLG:
		return (LRESULT) hBrush;
		
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		{	
		hDC = (HDC)wParam;
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_PROP_ROMS))
		{
			if (audit_color == 0)
				SetTextColor(hDC, RGB(34, 177, 76));
			else if (audit_color == 1)
				SetTextColor(hDC, RGB(237, 28, 36));
			else
				SetTextColor(hDC, RGB(63, 72, 204));
		}
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_PROP_SAMPLES))
		{
			if (audit_samples == 0)
				SetTextColor(hDC, RGB(34, 177, 76));
			else if (audit_samples == 1)
				SetTextColor(hDC, RGB(237, 28, 36));
			else
				SetTextColor(hDC, RGB(63, 72, 204));
		}
		
		return (LRESULT) hBrush;
		}
	
	case WM_CTLCOLOREDIT:
		{
		hDC = (HDC)wParam;
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_ROM_DETAILS))
			SetTextColor(hDC, RGB(59, 59, 59));
		
		if ((HWND)lParam == GetDlgItem(hAudit, IDC_AUDIT_DETAILS_PROP))
			SetTextColor(hDC, RGB(136, 0, 21));

		return (LRESULT) GetStockObject(WHITE_BRUSH);
		}
	}

	return 0;
}

static void ProcessNextRom(void)
{
	char buffer[200];
	int retval = MameUIVerifyRomSet(rom_index, 1);
	
	switch (retval)
	{
	case media_auditor::BEST_AVAILABLE: 	/* correct, incorrect or separate count? */
	case media_auditor::CORRECT:
	case media_auditor::NONE_NEEDED:
		if(driver_list::driver(rom_index).name[0] != '_')	// skip __empty driver
		{
			roms_correct++;
			snprintf(buffer, ARRAY_LENGTH(buffer), "%i", roms_correct);
			win_set_window_text_utf8(GetDlgItem(hAudit, IDC_ROMS_CORRECT), buffer);
		}
		break;

	case media_auditor::NOTFOUND:
		roms_notfound++;
		snprintf(buffer, ARRAY_LENGTH(buffer), "%i", roms_notfound);
		win_set_window_text_utf8(GetDlgItem(hAudit, IDC_ROMS_NOTFOUND), buffer);
		break;

	case media_auditor::INCORRECT:
		roms_incorrect++;
		snprintf(buffer, ARRAY_LENGTH(buffer), "%i", roms_incorrect);
		win_set_window_text_utf8(GetDlgItem(hAudit, IDC_ROMS_INCORRECT), buffer);
		break;
	}

	snprintf(buffer, ARRAY_LENGTH(buffer), "%i", roms_correct + roms_incorrect + roms_notfound);
	win_set_window_text_utf8(GetDlgItem(hAudit, IDC_ROMS_TOTAL), buffer);
	rom_index++;
	SendMessage(GetDlgItem(hAudit, IDC_ROMS_PROGRESS), PBM_SETPOS, rom_index, 0);

	if (rom_index == driver_list::total())
	{
		sample_index = 0;
		rom_index = -1;
	}
}

static void ProcessNextSample(void)
{
	if (DriverUsesSamples(sample_index))
	{	
		int retval = MameUIVerifySampleSet(sample_index);
		char buffer[200];
		
		switch (retval)
		{
		case media_auditor::NOTFOUND:
			samples_notfound++;
			snprintf(buffer, ARRAY_LENGTH(buffer), "%i", samples_notfound);
			win_set_window_text_utf8(GetDlgItem(hAudit, IDC_SAMPLES_NOTFOUND), buffer);
			break;

		case media_auditor::INCORRECT:
			samples_incorrect++;
			snprintf(buffer, ARRAY_LENGTH(buffer), "%i", samples_incorrect);
			win_set_window_text_utf8(GetDlgItem(hAudit, IDC_SAMPLES_INCORRECT), buffer);
			break;

		default:
			samples_correct++;
			snprintf(buffer, ARRAY_LENGTH(buffer), "%i", samples_correct);
			win_set_window_text_utf8(GetDlgItem(hAudit, IDC_SAMPLES_CORRECT), buffer);
			break;
		}
		
		snprintf(buffer, ARRAY_LENGTH(buffer), "%i", samples_correct + samples_incorrect + samples_notfound);
		win_set_window_text_utf8(GetDlgItem(hAudit, IDC_SAMPLES_TOTAL), buffer);
	}

	sample_index++;
	SendMessage(GetDlgItem(hAudit, IDC_SAMPLES_PROGRESS), PBM_SETPOS, sample_index, 0);

	if (sample_index == driver_list::total())
	{
		DetailsPrintf("Audit complete.\n");
		win_set_window_text_utf8(GetDlgItem(hAudit, IDCANCEL), "Close");
		sample_index = -1;
	}
}

static void DetailsPrintf(const char *fmt, ...)
{
	va_list marker;
	char buffer[8000];
	BOOL scroll = TRUE;

	//RS 20030613 Different Ids for Property Page and Dialog
	// so see which one's currently instantiated
	HWND hEdit = GetDlgItem(hAudit, IDC_AUDIT_DETAILS);
	
	if (hEdit ==  NULL)
	{
		hEdit = GetDlgItem(hAudit, IDC_AUDIT_DETAILS_PROP);
		scroll = FALSE;
	}
	
	if (hEdit == NULL)
		return;

	va_start(marker, fmt);
	vsnprintf(buffer, ARRAY_LENGTH(buffer), fmt, marker);
	va_end(marker);
	TCHAR *t_s = tstring_from_utf8(ConvertToWindowsNewlines(buffer));
	
	if( !t_s || _tcscmp(TEXT(""), t_s) == 0)
		return;

	int textLength = Edit_GetTextLength(hEdit);
	Edit_SetSel(hEdit, textLength, textLength);
	Edit_ReplaceSel(hEdit, t_s);

	if (scroll)
		Edit_ScrollCaret(hEdit);

	free(t_s);
}

static const char * StatusString(int iStatus)
{
	static const char *ptr = "None required";
	audit_color = 2;

	switch (iStatus)
	{
	case media_auditor::CORRECT:
		ptr = "Passed";
		audit_color = 0;
		audit_samples = 0;
		break;

	case media_auditor::BEST_AVAILABLE:
		ptr = "Best available";
		audit_color = 0;
		audit_samples = 0;
		break;

	case media_auditor::NOTFOUND:
		ptr = "Not found";
		audit_color = 1;
		audit_samples = 1;
		break;

	case media_auditor::INCORRECT:
		ptr = "Failed";
		audit_color = 1;
		audit_samples = 1;
		break;
	}

	return ptr;
}
