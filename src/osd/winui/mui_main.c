/***************************************************************************

  M.A.M.E.UI  -  Multiple Arcade Machine Emulator with User Interface
  Win32 Portions Copyright (C) 1997-2003 Michael Soderstrom and Chris Kirmse,
  Copyright (C) 2003-2007 Chris Kirmse and the MAME32/MAMEUI team.

  This file is part of MAMEUI, and may only be used, modified and
  distributed under the terms of the MAME license, in "readme.txt".
  By continuing to use, modify or distribute this file you indicate
  that you have read the license and understand and accept it fully.

 ***************************************************************************/

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>

// MAME/MAMEUI headers
#include "strconv.h"
#include "winui.h"

#undef main
#undef wmain
#define main mame_main
#define wmain mame_main

extern int utf8_main(int argc, char *argv[]);

extern "C" int _tmain(int argc, TCHAR **argv)
{
	int i, rc;
	char **utf8_argv;

	/* convert arguments to UTF-8 */
	utf8_argv = (char **) malloc(argc * sizeof(*argv));
	
	if (utf8_argv == NULL)
		return 999;
	
	for (i = 0; i < argc; i++)
	{
		utf8_argv[i] = utf8_from_tstring(argv[i]);

		if (utf8_argv[i] == NULL)
		{
			osd_free(utf8_argv);
			return 999;
		}
	}

	/* run utf8_main */
	rc = utf8_main(argc, utf8_argv);

	/* free arguments */
	for (i = 0; i < argc; i++)
		osd_free(utf8_argv[i]);
	
	osd_free(utf8_argv);

	return rc;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	return MameUIMain(hInstance, lpCmdLine);
}
