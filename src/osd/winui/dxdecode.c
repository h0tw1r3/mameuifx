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

/***************************************************************/

typedef struct tagERRORCODE
{
	HRESULT hr;
	const char *szError;
} ERRORCODE, * LPERRORCODE;

/***************************************************************/
static const ERRORCODE g_ErrorCode[] =
{
	{   DIERR_OLDDIRECTINPUTVERSION,        "DIERR_OLDDIRECTINPUTVERSION" },
	{   DIERR_BETADIRECTINPUTVERSION,       "DIERR_BETADIRECTINPUTVERSION" },
	{   DIERR_BADDRIVERVER,                 "DIERR_BADDRIVERVER" },
	{   DIERR_DEVICENOTREG,                 "DIERR_DEVICENOTREG" },
	{   DIERR_NOTFOUND,                     "DIERR_NOTFOUND" },
	{   DIERR_OBJECTNOTFOUND,               "DIERR_OBJECTNOTFOUND" },
	{   DIERR_INVALIDPARAM,                 "DIERR_INVALIDPARAM" },
	{   DIERR_NOINTERFACE,                  "DIERR_NOINTERFACE" },
	{   DIERR_GENERIC,                      "DIERR_GENERIC" },
	{   DIERR_OUTOFMEMORY,                  "DIERR_OUTOFMEMORY" },
	{   DIERR_UNSUPPORTED,                  "DIERR_UNSUPPORTED" },
	{   DIERR_NOTINITIALIZED,               "DIERR_NOTINITIALIZED" },
	{   DIERR_ALREADYINITIALIZED,           "DIERR_ALREADYINITIALIZED" },
	{   DIERR_NOAGGREGATION,                "DIERR_NOAGGREGATION" },
	{   DIERR_OTHERAPPHASPRIO,              "DIERR_OTHERAPPHASPRIO" },
	{   DIERR_INPUTLOST,                    "DIERR_INPUTLOST" },
	{   DIERR_ACQUIRED,                     "DIERR_ACQUIRED" },
	{   DIERR_NOTACQUIRED,                  "DIERR_NOTACQUIRED" },
	{   DIERR_READONLY,                     "DIERR_READONLY" },
	{   DIERR_HANDLEEXISTS,                 "DIERR_HANDLEEXISTS" },
	{   E_PENDING,                          "E_PENDING" },
	{   DIERR_INSUFFICIENTPRIVS,            "DIERR_INSUFFICIENTPRIVS" },
	{   DIERR_DEVICEFULL,                   "DIERR_DEVICEFULL" },
	{   DIERR_MOREDATA,                     "DIERR_MOREDATA" },
	{   DIERR_NOTDOWNLOADED,                "DIERR_NOTDOWNLOADED" },
	{   DIERR_HASEFFECTS,                   "DIERR_HASEFFECTS" },
	{   DIERR_NOTEXCLUSIVEACQUIRED,         "DIERR_NOTEXCLUSIVEACQUIRED" },
	{   DIERR_INCOMPLETEEFFECT,             "DIERR_INCOMPLETEEFFECT" },
	{   DIERR_NOTBUFFERED,                  "DIERR_NOTBUFFERED" },
	{   DIERR_EFFECTPLAYING,                "DIERR_EFFECTPLAYING" },
	{   E_NOINTERFACE,                      "E_NOINTERFACE" }

};

const char * DirectXDecodeError(HRESULT errorval)
{
	static char tmp[64];

	for (int i = 0; i < (sizeof(g_ErrorCode) / sizeof(g_ErrorCode[0])); i++)
	{
		if (g_ErrorCode[i].hr == errorval)
		{
			return g_ErrorCode[i].szError;
		}
	}
	
	snprintf(tmp, ARRAY_LENGTH(tmp), "UNKNOWN: 0x%x", (unsigned int)errorval);
	return tmp;
}
