// license:BSD-3-Clause
// copyright-holders:Robbbert
/******************************************************************************

Driver completely rewritten by Robbbert in a process begun on 2014-01-22.

Based on the previous work done by PeT around February 2008, and possible others prior.
 added apfm1000 cartridge loading
 fixed apfm1000 pads
 added apf video mode


APF M1000/MP1000 and Imagination Machine
----------------------------------------
- The M1000 contains the video RAM, ROM, CPU, PIA0, handsets, Video and cart slot, and thus was a TV Game computer.
- The MPA-10 was a base unit containing the main keyboard, custom cassette recorder, 8k RAM and PIA1.
- When the two were joined, they formed the Imagination Machine.
- Although the BASIC cart could be plugged into the M1000, it could not be used as it needs the main keyboard.
- BB-01 Building Block - provides 4 cart slots. Includes a RS-232 cart for a printer or modem.
- R8-K 8K RAM Expansion cart.
- FI-100 Minifloppy Disk Interface Cartridge - drives 1 or 2 D100-0 floppy drives on AS-400 bus.
- D100-0 Floppy drive 5 1/4"
- A cassette program must be loaded on the same memory size it was saved from. Since the standard machine
  had 8K, almost all tapes require this exact amount of RAM to be present
- The cart-slot is physically the same as the Arcadia 2001.


RAM switch
----------
- The M1000 only had available the 1K video ram (0000-03FF)
- Space Destroyer needs more, so it includes another 1K in the cart
- The MPA-10 base includes 8K of RAM (A000-BFFF) (-ram 8K)
- A very few games need 16K which requires hacking the pcb (-ram 16K)
- Basic will work with 8K or 16K.


Status of cart-based games
--------------------------
backgammon - works, needs offset of 0x120, bottom line is coming from 0x3E0, should be 0x380
baseball - works, needs offset of 0x200, some bad colours
basic - works in apfimag only (as designed)
blackjack - works, some bad colours
bowling - works
boxing - works
brickdown - works
columns - runs but seems to be buggy
casino - appears to work (need instructions)
catena - works
hangman - works
pinball - works
movblock - works
rocket patrol - works, some bad colours
space destroyer - works
ufo - works


ToDo:
-----
- When pasting a large program, characters can be lost
- Some bad colours or graphics
- Tape loading is not very reliable
- Add back the disk support when we can get some info on it
  (6600, 6500-6503 wd179x disc controller? 6400, 6401)
- Need to add back the disk format in the new wdc code
  (40 tracks, 1 head, 8 sectors, 256 bytes sector length, first sector id 1)
- Need disk-based software


******************************************************************************************************************/

#include "emu.h"
#include "cpu/m6800/m6800.h"
#include "video/mc6847.h"
#include "sound/speaker.h"
#include "sound/wave.h"
#include "machine/6821pia.h"
#include "machine/wd_fdc.h"
#include "imagedev/cassette.h"
#include "formats/apf_apt.h"
#include "machine/ram.h"

#include "bus/apf/slot.h"
#include "bus/apf/rom.h"


class apf_state : public driver_device
{
public:
	apf_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_has_cart_ram(false)
		, m_maincpu(*this, "maincpu")
		, m_ram(*this, RAM_TAG)
		, m_crtc(*this, "mc6847")
		, m_speaker(*this, "speaker")
		, m_pia0(*this, "pia0")
		, m_pia1(*this, "pia1")
		, m_cass(*this, "cassette")
		, m_cart(*this, "cartslot")
		, m_fdc(*this, "fdc")
		, m_floppy0(*this, "fdc:0")
		, m_floppy1(*this, "fdc:1")
		, m_joy(*this, "joy")
		, m_key(*this, "key")
		, m_p_videoram(*this, "videoram")
	{ }

	DECLARE_READ8_MEMBER(videoram_r);
	DECLARE_READ8_MEMBER(pia0_porta_r);
	DECLARE_WRITE8_MEMBER(pia0_portb_w);
	DECLARE_WRITE_LINE_MEMBER(pia0_ca2_w);
	DECLARE_READ8_MEMBER(pia1_porta_r);
	DECLARE_READ8_MEMBER(pia1_portb_r);
	DECLARE_WRITE8_MEMBER(pia1_portb_w);
	DECLARE_WRITE8_MEMBER(apf_dischw_w);
	DECLARE_READ8_MEMBER(serial_r);
	DECLARE_WRITE8_MEMBER(serial_w);

private:
	UINT8 m_latch;
	UINT8 m_keyboard_data;
	UINT8 m_pad_data;
	UINT8 m_portb;
	bool m_ca2;
	bool m_has_cart_ram;
	virtual void machine_start();
	virtual void machine_reset();
	required_device<m6800_cpu_device> m_maincpu;
	optional_device<ram_device> m_ram;
	required_device<mc6847_base_device> m_crtc;
	required_device<speaker_sound_device> m_speaker;
	required_device<pia6821_device> m_pia0;
	optional_device<pia6821_device> m_pia1;
	optional_device<cassette_image_device> m_cass;
	required_device<apf_cart_slot_device> m_cart;
	optional_device<fd1771_t> m_fdc;
	optional_device<floppy_connector> m_floppy0;
	optional_device<floppy_connector> m_floppy1;
	required_ioport_array<4> m_joy;
	optional_ioport_array<8> m_key;
	required_shared_ptr<UINT8> m_p_videoram;
};


READ8_MEMBER( apf_state::videoram_r )
{
	if BIT(m_pad_data, 7) // AG line
	{
		// Need the cpu and crtc to be locked together for proper graphics
		// This is a hack to fix Rocket Patrol and Blackjack
		if (BIT(m_pad_data, 6) && !m_has_cart_ram)
			offset -= 0x400;

		// This is a hack to fix Space Destroyer
		if (BIT(m_pad_data, 6) && m_has_cart_ram)
			offset -= 0x120;

		UINT16 part1 = offset & 0x1f;
		UINT16 part2 = (offset & 0x1e0) >> 5;
		UINT16 part3 = (offset & 0x1e00) >> 4;
		if (m_ca2) m_latch = m_p_videoram[part3 | part1]; // get chr
		m_crtc->css_w(BIT(m_latch, 6));
		UINT16 latch = (m_latch & 0x1f) << 4;
		return m_p_videoram[latch | part2 | 0x200]; // get gfx
	}
	else
	{
		UINT8 data = m_p_videoram[(offset & 0x1ff) | 0x200];
		if (m_ca2) m_crtc->css_w(BIT(data, 6));
		m_crtc->inv_w(BIT(data, 6));
		m_crtc->as_w(BIT(data, 7));
		return data;
	}
}

READ8_MEMBER( apf_state::pia0_porta_r )
{
	UINT8 data = 0xff;

	for (int i = 3; i >= 0; i--)
		if (!BIT(m_pad_data, i))
			data &= m_joy[i]->read();

	return data;
}

WRITE8_MEMBER( apf_state::pia0_portb_w )
{
	/* bit 7..6 video control */
	m_crtc->ag_w(BIT(data, 7));
	m_crtc->gm0_w(BIT(data, 6));

	/* bit 3..0 keypad line select */
	m_pad_data = data;
}

WRITE_LINE_MEMBER( apf_state::pia0_ca2_w )
{
	m_ca2 = state;
}

READ8_MEMBER( apf_state::pia1_porta_r )
{
	return m_key[m_keyboard_data]->read();
}

READ8_MEMBER( apf_state::pia1_portb_r )
{
	UINT8 data = m_portb;

	if (m_cass->input() > 0.0038)
		data |= 0x80;

	return data;
}


WRITE8_MEMBER( apf_state::pia1_portb_w )
{
	/* bits 2..0 = keyboard line */
	/* bit 3 = cass audio enable */
	/* bit 4 = cassette motor */
	/* bit 5 = /cass write enable */
	/* bit 6 = cass out */
	/* bit 7 = cass in */

	m_portb = data & 0x7f;
	m_keyboard_data = data & 7;

	m_cass->change_state(BIT(data, 4) ? CASSETTE_MOTOR_ENABLED : CASSETTE_MOTOR_DISABLED, CASSETTE_MASK_MOTOR);

	if (!BIT(data, 5))
		m_cass->output(BIT(data, 6) ? -1.0 : 1.0);
}


void apf_state::machine_start()
{
	if (m_cart->exists())
	{
		switch (m_cart->get_type())
		{
			case APF_BASIC:
				m_maincpu->space(AS_PROGRAM).install_read_handler(0x6800, 0x7fff, read8_delegate(FUNC(apf_cart_slot_device::extra_rom),(apf_cart_slot_device*)m_cart));
				break;
			case APF_SPACEDST:
				m_maincpu->space(AS_PROGRAM).unmap_readwrite(0x9800, 0x9fff);
				m_maincpu->space(AS_PROGRAM).install_readwrite_handler(0x9800, 0x9bff, read8_delegate(FUNC(apf_cart_slot_device::read_ram),(apf_cart_slot_device*)m_cart), write8_delegate(FUNC(apf_cart_slot_device::write_ram),(apf_cart_slot_device*)m_cart));
				m_has_cart_ram = true;
				break;
		}

		m_cart->save_ram();
	}
}


void apf_state::machine_reset()
{
	address_space &space = m_maincpu->space(AS_PROGRAM);
	m_portb = 0;
	m_ca2 = 0;

	// apfimag only
	if (m_ram)
	{
		/* if we specified 8K of RAM, delete the extended RAM */
		if (m_ram->size() < 16*1024)
			space.unmap_readwrite(0xc000, 0xdfff);
		// this is a hack to get 'columns' to work. It misbehaves if a000-a003 are all zero
		else
			space.write_byte(0xa002, 0xe5);
	}
}

WRITE8_MEMBER( apf_state::apf_dischw_w)
{
	/* bit 3 is index of drive to select */
	UINT8 drive = BIT(data, 3);

	floppy_image_device *floppy = NULL;
	if (drive)
		floppy = m_floppy1->get_device();
	else
		floppy = m_floppy0->get_device();

	m_fdc->set_floppy(floppy);

	if (floppy)
	{
		floppy->mon_w(0);
	}

	logerror("disc w %04x %04x\n",offset,data);
}

READ8_MEMBER( apf_state::serial_r)
{
	logerror("serial r %04x\n",offset);
	return 0;
}

WRITE8_MEMBER( apf_state::serial_w)
{
	logerror("serial w %04x %04x\n",offset,data);
}

static ADDRESS_MAP_START( apfm1000_map, AS_PROGRAM, 8, apf_state )
	AM_RANGE(0x0000, 0x03ff) AM_MIRROR(0x1c00) AM_RAM AM_SHARE("videoram")
	AM_RANGE(0x2000, 0x3fff) AM_MIRROR(0x1ffc) AM_DEVREADWRITE("pia0", pia6821_device, read, write)
	AM_RANGE(0x4000, 0x4fff) AM_MIRROR(0x1000) AM_ROM AM_REGION("roms", 0)
	AM_RANGE(0x6800, 0x7fff) AM_NOP // BASIC accesses ROM here too, but this is installed at machine_start
	AM_RANGE(0x8000, 0x9fff) AM_DEVREAD("cartslot", apf_cart_slot_device, read_rom)
	AM_RANGE(0xe000, 0xefff) AM_MIRROR(0x1000) AM_ROM AM_REGION("roms", 0)
ADDRESS_MAP_END

static ADDRESS_MAP_START( apfimag_map, AS_PROGRAM, 8, apf_state )
	AM_IMPORT_FROM(apfm1000_map)
	AM_RANGE(0x6000, 0x63ff) AM_MIRROR(0x03fc) AM_DEVREADWRITE("pia1", pia6821_device, read, write)
	// These need to be confirmed, disk does not work
	AM_RANGE(0x6400, 0x64ff) AM_READWRITE(serial_r, serial_w)
	AM_RANGE(0x6500, 0x6503) AM_DEVREADWRITE("fdc", fd1771_t, read, write)
	AM_RANGE(0x6600, 0x6600) AM_WRITE(apf_dischw_w)
	AM_RANGE(0xa000, 0xbfff) AM_RAM // standard
	AM_RANGE(0xc000, 0xdfff) AM_RAM // expansion
ADDRESS_MAP_END


/* Each controller has these features:

   1 8-way joystick
   1 big red fire button on the upper side
   12-keys keypad with the following layout

   7 8 9 0
   4 5 6 Cl
   1 2 3 En

   On the control panel of the M-1000 there are two big buttons: a Reset key and the Power switch

   Reference: http://www.nausicaa.net/~lgreenf/apfpage2.htm
*/

static INPUT_PORTS_START( apfm1000 )

/*
       This simple Basic program can be used to read the joysticks and the keyboard:

       10 PRINT KEY$(n);
       20 GOTO 10

       where n = 0, 1 or 2 - 0 = keyboard, 1,2 = joysticks #1 and #2

       When reading the keyboard KEY$(0) returns the character associated to the key, with the
       following exceptions:

       Ctrl =    CHR$(1)
       Rept =    CHR$(2)
       Here Is = CHR$(4)
       Rubout =  CHR$(8)

       When reading the joysticks, KEY$() = "N", "S", "E", "W" for the directions
                                            "0" - "9" for the keypad digits
                                            "?" for "Cl"
                                            "!" for "En"


  ? player right is player 1
*/

	PORT_START("joy.0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 1") PORT_CODE(KEYCODE_1) PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 0") PORT_CODE(KEYCODE_0) PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 4") PORT_CODE(KEYCODE_4) PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 7") PORT_CODE(KEYCODE_7) PORT_PLAYER(2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 1") PORT_CODE(KEYCODE_1_PAD) PORT_PLAYER(1)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 0") PORT_CODE(KEYCODE_0_PAD) PORT_PLAYER(1)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 4") PORT_CODE(KEYCODE_4_PAD) PORT_PLAYER(1)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 7") PORT_CODE(KEYCODE_7_PAD) PORT_PLAYER(1)

	PORT_START("joy.1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN) PORT_NAME("PAD 1/RIGHT down") PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_NAME("PAD 1/RIGHT right") PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) PORT_NAME("PAD 1/RIGHT up") PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT) PORT_NAME("PAD 1/RIGHT left") PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN) PORT_NAME("PAD 2/LEFT down") PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_NAME("PAD 2/LEFT right") PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) PORT_NAME("PAD 2/LEFT up") PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT) PORT_NAME("PAD 2/LEFT left") PORT_PLAYER(1) PORT_8WAY

	PORT_START("joy.2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 3") PORT_CODE(KEYCODE_3) PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT clear") PORT_CODE(KEYCODE_DEL) PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 6") PORT_CODE(KEYCODE_6) PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 9") PORT_CODE(KEYCODE_9) PORT_PLAYER(2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 3") PORT_CODE(KEYCODE_3_PAD) PORT_PLAYER(1)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT clear") PORT_CODE(KEYCODE_DEL_PAD) PORT_PLAYER(1)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 6") PORT_CODE(KEYCODE_6_PAD) PORT_PLAYER(1)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 9") PORT_CODE(KEYCODE_9_PAD) PORT_PLAYER(1)

	PORT_START("joy.3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 2") PORT_CODE(KEYCODE_2) PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT enter/fire") PORT_CODE(KEYCODE_ENTER) PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 5") PORT_CODE(KEYCODE_5) PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 1/RIGHT 8") PORT_CODE(KEYCODE_8) PORT_PLAYER(2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 2") PORT_CODE(KEYCODE_2_PAD) PORT_PLAYER(1)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT enter/fire") PORT_CODE(KEYCODE_ENTER_PAD) PORT_PLAYER(1)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 5") PORT_CODE(KEYCODE_5_PAD) PORT_PLAYER(1)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("PAD 2/LEFT 8") PORT_CODE(KEYCODE_8_PAD) PORT_PLAYER(1)
INPUT_PORTS_END


static INPUT_PORTS_START( apfimag )

	PORT_INCLUDE( apfm1000 )

	/* Reference: http://www.nausicaa.net/~lgreenf/apfpage2.htm */

	PORT_START("key.0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X")               PORT_CODE(KEYCODE_X)          PORT_CHAR('X')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z")               PORT_CODE(KEYCODE_Z)          PORT_CHAR('Z')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q       IF")      PORT_CODE(KEYCODE_Q)          PORT_CHAR('Q')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2   \"    LET")   PORT_CODE(KEYCODE_2)          PORT_CHAR('2') PORT_CHAR('\"')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A")               PORT_CODE(KEYCODE_A)          PORT_CHAR('A')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1   !   GOSUB")   PORT_CODE(KEYCODE_1)          PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W       STEP")    PORT_CODE(KEYCODE_W)          PORT_CHAR('W')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S")               PORT_CODE(KEYCODE_S)          PORT_CHAR('S')

	PORT_START("key.1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C")               PORT_CODE(KEYCODE_C)          PORT_CHAR('C')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V")               PORT_CODE(KEYCODE_V)          PORT_CHAR('V')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R       READ")    PORT_CODE(KEYCODE_R)          PORT_CHAR('R')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3   #   DATA")    PORT_CODE(KEYCODE_3)          PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F")               PORT_CODE(KEYCODE_F)          PORT_CHAR('F')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4   $   INPUT")   PORT_CODE(KEYCODE_4)          PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E       STOP")    PORT_CODE(KEYCODE_E)          PORT_CHAR('E')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D")               PORT_CODE(KEYCODE_D)          PORT_CHAR('D')

	PORT_START("key.2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N   ^")           PORT_CODE(KEYCODE_N)          PORT_CHAR('N') PORT_CHAR('^')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B")               PORT_CODE(KEYCODE_B)          PORT_CHAR('B')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T       NEXT")    PORT_CODE(KEYCODE_T)          PORT_CHAR('T')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6   &   FOR")     PORT_CODE(KEYCODE_6)          PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G")               PORT_CODE(KEYCODE_G)          PORT_CHAR('G')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5   %   DIM")     PORT_CODE(KEYCODE_5)          PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y       PRINT")   PORT_CODE(KEYCODE_Y)          PORT_CHAR('Y')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H")               PORT_CODE(KEYCODE_H)          PORT_CHAR('H')

	PORT_START("key.3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M   ]")           PORT_CODE(KEYCODE_M)          PORT_CHAR('M') PORT_CHAR(']')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",   <")           PORT_CODE(KEYCODE_COMMA)      PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I       LIST")    PORT_CODE(KEYCODE_I)          PORT_CHAR('I')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7   '   RETURN")  PORT_CODE(KEYCODE_7)          PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K   [")           PORT_CODE(KEYCODE_K)          PORT_CHAR('K') PORT_CHAR('[')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8   (   THEN")    PORT_CODE(KEYCODE_8)          PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U       END")     PORT_CODE(KEYCODE_U)          PORT_CHAR('U')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J")               PORT_CODE(KEYCODE_J)          PORT_CHAR('J')

	PORT_START("key.4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("/   ?")           PORT_CODE(KEYCODE_SLASH)      PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".   >")           PORT_CODE(KEYCODE_STOP)       PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O   _   REM")     PORT_CODE(KEYCODE_O)          PORT_CHAR('O') PORT_CHAR('_')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0       GOTO")    PORT_CODE(KEYCODE_0)          PORT_CHAR('0')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L   \\")          PORT_CODE(KEYCODE_L)          PORT_CHAR('L') PORT_CHAR('\\')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9   )   ON")      PORT_CODE(KEYCODE_9)          PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P   @   USING")   PORT_CODE(KEYCODE_P)          PORT_CHAR('P') PORT_CHAR('@')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(";   +")           PORT_CODE(KEYCODE_COLON)      PORT_CHAR(';') PORT_CHAR('+')

	PORT_START("key.5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Space")           PORT_CODE(KEYCODE_SPACE)      PORT_CHAR(32)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(":   *")           PORT_CODE(KEYCODE_MINUS)      PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Return")          PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(13)
	PORT_BIT(0x08, 0x08, IPT_UNUSED)
	PORT_BIT(0x10, 0x10, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("-   =   RESTORE") PORT_CODE(KEYCODE_EQUALS)     PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Line Feed")       PORT_CODE(KEYCODE_OPENBRACE)  PORT_CHAR(10)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rubout")          PORT_CODE(KEYCODE_QUOTE)      PORT_CHAR(8)

	PORT_START("key.6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift")     PORT_CODE(KEYCODE_RSHIFT) PORT_CODE(KEYCODE_LSHIFT)     PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Esc")             PORT_CODE(KEYCODE_TAB)        PORT_CHAR(27)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl")            PORT_CODE(KEYCODE_LCONTROL)   PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rept")            PORT_CODE(KEYCODE_ENTER)      PORT_CHAR(UCHAR_MAMEKEY(TAB))
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Break")           PORT_CODE(KEYCODE_BACKSLASH)  PORT_CHAR(UCHAR_MAMEKEY(DEL))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Here Is")         PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT(0x40, 0x40, IPT_UNUSED) // another X
	PORT_BIT(0x80, 0x80, IPT_UNUSED) // another Z

	PORT_START("key.7")
	PORT_BIT(0xff, 0xff, IPT_UNUSED)
INPUT_PORTS_END


static SLOT_INTERFACE_START( apf_floppies )
	SLOT_INTERFACE( "525dd", FLOPPY_525_SSDD )
SLOT_INTERFACE_END


static SLOT_INTERFACE_START(apf_cart)
	SLOT_INTERFACE_INTERNAL("std",       APF_ROM_STD)
	SLOT_INTERFACE_INTERNAL("basic",     APF_ROM_BASIC)
	SLOT_INTERFACE_INTERNAL("spacedst",  APF_ROM_SPACEDST)
SLOT_INTERFACE_END


static MACHINE_CONFIG_START( apfm1000, apf_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6800, XTAL_3_579545MHz / 4 )  // divided by 4 in external clock circuit
	MCFG_CPU_PROGRAM_MAP(apfm1000_map)

	/* video hardware */
	MCFG_SCREEN_MC6847_NTSC_ADD("screen", "mc6847")

	MCFG_DEVICE_ADD("mc6847", MC6847_NTSC, XTAL_3_579545MHz)
	MCFG_MC6847_FSYNC_CALLBACK(DEVWRITELINE("pia0", pia6821_device, cb1_w))
	MCFG_MC6847_INPUT_CALLBACK(READ8(apf_state, videoram_r))
	MCFG_MC6847_FIXED_MODE(MC6847_MODE_GM2 | MC6847_MODE_GM1)
	// INTEXT = GND
	// other lines not connected

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	/* Devices */
	MCFG_DEVICE_ADD("pia0", PIA6821, 0)
	MCFG_PIA_READPA_HANDLER(READ8(apf_state, pia0_porta_r))
	MCFG_PIA_WRITEPB_HANDLER(WRITE8(apf_state, pia0_portb_w))
	MCFG_PIA_CA2_HANDLER(WRITELINE(apf_state, pia0_ca2_w))
	MCFG_PIA_CB2_HANDLER(DEVWRITELINE("speaker", speaker_sound_device, level_w))
	MCFG_PIA_IRQA_HANDLER(DEVWRITELINE("maincpu", m6800_cpu_device, irq_line))
	MCFG_PIA_IRQB_HANDLER(DEVWRITELINE("maincpu", m6800_cpu_device, irq_line))

	MCFG_APF_CARTRIDGE_ADD("cartslot", apf_cart, NULL)

	/* software lists */
	MCFG_SOFTWARE_LIST_ADD("cart_list", "apfm1000")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( apfimag, apfm1000 )
	MCFG_CPU_MODIFY( "maincpu" )
	MCFG_CPU_PROGRAM_MAP( apfimag_map)

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("8K")
	MCFG_RAM_EXTRA_OPTIONS("16K")

	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.15)

	MCFG_DEVICE_ADD("pia1", PIA6821, 0)
	MCFG_PIA_READPA_HANDLER(READ8(apf_state, pia1_porta_r))
	MCFG_PIA_READPB_HANDLER(READ8(apf_state, pia1_portb_r))
	MCFG_PIA_WRITEPB_HANDLER(WRITE8(apf_state, pia1_portb_w))

	MCFG_CASSETTE_ADD("cassette")
	MCFG_CASSETTE_FORMATS(apf_cassette_formats)
	MCFG_CASSETTE_DEFAULT_STATE(CASSETTE_STOPPED | CASSETTE_SPEAKER_ENABLED | CASSETTE_MOTOR_DISABLED)

	MCFG_FD1771_ADD("fdc", 1000000) // guess
	MCFG_FLOPPY_DRIVE_ADD("fdc:0", apf_floppies, "525dd", floppy_image_device::default_floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD("fdc:1", apf_floppies, "525dd", floppy_image_device::default_floppy_formats)

	MCFG_SOFTWARE_LIST_ADD("cass_list", "apfimag_cass")
MACHINE_CONFIG_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START(apfm1000)
	ROM_REGION(0x1000,"roms", 0)
	ROM_SYSTEM_BIOS( 0, "0", "Standard" )
	ROMX_LOAD("apf_4000.rom", 0x0000, 0x0800, CRC(cc6ac840) SHA1(1110a234bcad99bd0894ad44c591389d16376ca4), ROM_BIOS(1) )
	ROM_RELOAD(0x0800, 0x0800)

	ROM_SYSTEM_BIOS( 1, "trash", "Trash II" ) // In Rocket Patrol, the ships are replaced by garbage trucks
	ROMX_LOAD("trash-ii.bin", 0x0000, 0x1000, CRC(3bd8640a) SHA1(da4cd8163990adbc5acd3eab604b41e1066bb832), ROM_BIOS(2) )

	ROM_SYSTEM_BIOS( 2, "mod", "Mod Bios" ) // (c) 1982 W.Lunquist - In Basic, CALL 18450 to get a machine-language monitor
	ROMX_LOAD("mod_bios.bin", 0x0000, 0x1000, CRC(f320aba6) SHA1(9442349fca8b001a5765e2fe8b84db4ece7886c1), ROM_BIOS(3) )
ROM_END

#define rom_apfimag rom_apfm1000

// old rom, has a bad byte at 0087.
//ROMX_LOAD("apf_4000.rom", 0x0000, 0x0800, CRC(2a331a33) SHA1(387b90882cd0b66c192d9cbaa3bec250f897e4f1), ROM_BIOS(1) )

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*    YEAR  NAME     PARENT     COMPAT  MACHINE     INPUT      CLASS          INIT         COMPANY               FULLNAME */
COMP( 1979, apfimag,  apfm1000,  0,      apfimag,    apfimag,   driver_device,  0,   "APF Electronics Inc.", "APF Imagination Machine", 0 )
CONS( 1978, apfm1000, 0,         0,      apfm1000,   apfm1000,  driver_device,  0,   "APF Electronics Inc.", "APF M-1000", 0 )
