/***************************************************************************

  M.A.M.E.UI  -  Multiple Arcade Machine Emulator with User Interface
  Win32 Portions Copyright (C) 1997-2003 Michael Soderstrom and Chris Kirmse,
  Copyright (C) 2003-2007 Chris Kirmse and the MAME32/MAMEUI team.

  This file is part of MAMEUI, and may only be used, modified and
  distributed under the terms of the MAME license, in "readme.txt".
  By continuing to use, modify or distribute this file you indicate
  that you have read the license and understand and accept it fully.

 ***************************************************************************/

#pragma once 
 
#ifndef BITMASK_H
#define BITMASK_H

/* Bit array type */
typedef struct
{
	UINT m_nSize;
	UCHAR* m_lpBits;
} BITS, *LPBITS;

/* Linked list type */
typedef struct
{
	LPVOID data;
	LPVOID next;
	LPVOID prev;
} NODE, *LPNODE;

/* Bit functions */
LPBITS NewBits(UINT nLength);	 /* in bits */
void DeleteBits(LPBITS lpBits);
bool TestBit(LPBITS lpBits, UINT nBit);
void SetBit(LPBITS lpBits, UINT nBit);
void ClearBit(LPBITS lpBits, UINT nBit);
void SetAllBits(LPBITS lpBits, bool bSet);
int FindBit(LPBITS lpBits, int nStartPos, bool bSet);

#endif
