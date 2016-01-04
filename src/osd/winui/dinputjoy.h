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
 
#ifndef DINPUTJOY_H
#define DINPUTJOY_H

/*
  limits:
  - 7 joysticks
  - 15 sticks on each joystick (15?)
  - 63 buttons on each joystick

  - 256 total inputs


   1 1 1 1 1 1
   5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
  +---+-----------+---------+-----+
  |Dir|Axis/Button|   Stick | Joy |
  +---+-----------+---------+-----+

    Stick:  0 for buttons 1 for axis
    Joy:    1 for Mouse/track buttons

*/
#define JOYCODE(joy, stick, axis_or_button, dir) \
        ((((dir)            & 0x03) << 14) |     \
         (((axis_or_button) & 0x3f) <<  8) |     \
         (((stick)          & 0x1f) <<  3) |     \
         (((joy)            & 0x07) <<  0))

#define GET_JOYCODE_JOY(code)    (((code) >> 0) & 0x07)
#define GET_JOYCODE_STICK(code)  (((code) >> 3) & 0x1f)
#define GET_JOYCODE_AXIS(code)   (((code) >> 8) & 0x3f)
#define GET_JOYCODE_BUTTON(code) GET_JOYCODE_AXIS(code)
#define GET_JOYCODE_DIR(code)    (((code) >>14) & 0x03)

#define JOYCODE_STICK_BTN    0
#define JOYCODE_STICK_AXIS   1
#define JOYCODE_STICK_POV    2

#define JOYCODE_DIR_BTN      0
#define JOYCODE_DIR_NEG      1
#define JOYCODE_DIR_POS      2

struct OSDJoystick
{
	int  (*init)(void);
	void (*exit)(void);
	int  (*is_joy_pressed)(int joycode);
	void (*poll_joysticks)(void);
	bool (*Available)(void);
};

extern const struct OSDJoystick DIJoystick;

bool DirectInputInitialize(void);
void DirectInputClose(void);

#endif
