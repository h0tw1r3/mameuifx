// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Driver for Midway Zeus games

**************************************************************************/

#include "emu.h"
#include "includes/midzeus.h"
#include "video/poly.h"
#include "video/rgbutil.h"



/*************************************
 *
 *  Constants
 *
 *************************************/

#define DUMP_WAVE_RAM       0

#define WAVERAM0_WIDTH      512
#define WAVERAM0_HEIGHT     2048

#define WAVERAM1_WIDTH      512
#define WAVERAM1_HEIGHT     512

#define BLEND_OPAQUE1       0x00000000
#define BLEND_OPAQUE2       0x4b23cb00
#define BLEND_OPAQUE3       0x4b23dd00
#define BLEND_OPAQUE4       0x00004800
#define BLEND_OPAQUE5       0xdd23dd00
#define BLEND_ADD1          0x40b68800
#define BLEND_ADD2          0xc9b78800
#define BLEND_MUL1          0x4093c800


/*************************************
 *
 *  Type definitions
 *
 *************************************/

struct mz_poly_extra_data
{
	const void *    palbase;
	const void *    texbase;
	UINT16          solidcolor;
	UINT16          voffset;
	INT16           zoffset;
	UINT16          transcolor;
	UINT16          texwidth;
	UINT16          color;
	UINT32          alpha;
	UINT32          ctrl_word;
	bool            blend_enable;
	bool            depth_test_enable;
	bool            depth_write_enable;
	UINT32          blend;
	UINT8           (*get_texel)(const void *, int, int, int);
};


class midzeus_renderer : public poly_manager<float, mz_poly_extra_data, 4, 10000>
{
public:
	midzeus_renderer(midzeus_state &state);

	void render_poly(INT32 scanline, const extent_t& extent, const mz_poly_extra_data& object, int threadid);
	void render_poly_solid_fixedz(INT32 scanline, const extent_t& extent, const mz_poly_extra_data& object, int threadid);

	void zeus_draw_quad(int long_fmt, const UINT32 *databuffer, UINT32 texdata, int logit);
	void zeus_draw_debug_quad(const rectangle& rect, const vertex_t* vert);

private:
	midzeus_state& m_state;
};

typedef midzeus_renderer::vertex_t poly_vertex;


/*************************************
 *
 *  Global variables
 *
 *************************************/

static midzeus_renderer *poly;
static UINT8 log_fifo;

static UINT32 zeus_fifo[20];
static UINT8 zeus_fifo_words;
static INT16 zeus_matrix[3][3];
static INT32 zeus_point[3];
static INT16 zeus_light[3];
static void *zeus_renderbase;
static UINT32 zeus_palbase;
static UINT32 zeus_unkbase;
static int zeus_enable_logging;
static UINT32 zeus_objdata;
static rectangle zeus_cliprect;

static UINT32 *waveram[2];
static int yoffs;
static int texel_width;
static int is_mk4b;


/*************************************
 *
 *  Function prototypes
 *
 *************************************/

static inline UINT8 get_texel_4bit(const void *base, int y, int x, int width);
static inline UINT8 get_texel_alt_4bit(const void *base, int y, int x, int width);
static inline UINT8 get_texel_8bit(const void *base, int y, int x, int width);
static inline UINT8 get_texel_alt_8bit(const void *base, int y, int x, int width);


/*************************************
 *
 *  Macros
 *
 *************************************/

#define WAVERAM_BLOCK0(blocknum)                ((void *)((UINT8 *)waveram[0] + 8 * (blocknum)))
#define WAVERAM_BLOCK1(blocknum)                ((void *)((UINT8 *)waveram[1] + 8 * (blocknum)))

#define WAVERAM_PTR8(base, bytenum)             ((UINT8 *)(base) + BYTE4_XOR_LE(bytenum))
#define WAVERAM_READ8(base, bytenum)            (*WAVERAM_PTR8(base, bytenum))
#define WAVERAM_WRITE8(base, bytenum, data)     do { *WAVERAM_PTR8(base, bytenum) = (data); } while (0)

#define WAVERAM_PTR16(base, wordnum)            ((UINT16 *)(base) + BYTE_XOR_LE(wordnum))
#define WAVERAM_READ16(base, wordnum)           (*WAVERAM_PTR16(base, wordnum))
#define WAVERAM_WRITE16(base, wordnum, data)    do { *WAVERAM_PTR16(base, wordnum) = (data); } while (0)

#define WAVERAM_PTR32(base, dwordnum)           ((UINT32 *)(base) + (dwordnum))
#define WAVERAM_READ32(base, dwordnum)          (*WAVERAM_PTR32(base, dwordnum))
#define WAVERAM_WRITE32(base, dwordnum, data)   do { *WAVERAM_PTR32(base, dwordnum) = (data); } while (0)

#define PIXYX_TO_WORDNUM(y, x)                  (((y) << 10) | (((x) & 0x1fe) << 1) | ((x) & 1))
#define DEPTHYX_TO_WORDNUM(y, x)                (PIXYX_TO_WORDNUM(y, x) | 2)

#define WAVERAM_PTRPIX(base, y, x)              WAVERAM_PTR16(base, PIXYX_TO_WORDNUM(y, x))
#define WAVERAM_READPIX(base, y, x)             (*WAVERAM_PTRPIX(base, y, x))
#define WAVERAM_WRITEPIX(base, y, x, color)     do { *WAVERAM_PTRPIX(base, y, x) = (color);  } while (0)

#define WAVERAM_PTRDEPTH(base, y, x)            WAVERAM_PTR16(base, DEPTHYX_TO_WORDNUM(y, x))
#define WAVERAM_READDEPTH(base, y, x)           (*WAVERAM_PTRDEPTH(base, y, x))
#define WAVERAM_WRITEDEPTH(base, y, x, color)   do { *WAVERAM_PTRDEPTH(base, y, x) = (color);  } while (0)



/*************************************
 *
 *  Inlines for block addressing
 *
 *************************************/

static inline void *waveram0_ptr_from_block_addr(UINT32 addr)
{
	UINT32 blocknum = (addr % WAVERAM0_WIDTH) + ((addr >> 12) % WAVERAM0_HEIGHT) * WAVERAM0_WIDTH;
	return WAVERAM_BLOCK0(blocknum);
}

static inline void *waveram0_ptr_from_expanded_addr(UINT32 addr)
{
	UINT32 blocknum = (addr % WAVERAM0_WIDTH) + ((addr >> 16) % WAVERAM0_HEIGHT) * WAVERAM0_WIDTH;
	return WAVERAM_BLOCK0(blocknum);
}

static inline void *waveram1_ptr_from_expanded_addr(UINT32 addr)
{
	UINT32 blocknum = (addr % WAVERAM1_WIDTH) + ((addr >> 16) % WAVERAM1_HEIGHT) * WAVERAM1_WIDTH;
	return WAVERAM_BLOCK1(blocknum);
}

static inline void *waveram0_ptr_from_texture_addr(UINT32 addr, int width)
{
	UINT32 blocknum = (((addr & ~1) * width) / 8) % (WAVERAM0_WIDTH * WAVERAM0_HEIGHT);
	return WAVERAM_BLOCK0(blocknum);
}



/*************************************
 *
 *  Inlines for rendering
 *
 *************************************/

static inline void waveram_plot_depth(int y, int x, UINT16 color, UINT16 depth)
{
	if (zeus_cliprect.contains(x, y))
	{
		WAVERAM_WRITEPIX(zeus_renderbase, y, x, color);
		WAVERAM_WRITEDEPTH(zeus_renderbase, y, x, depth);
	}
}

#ifdef UNUSED_FUNCTION
static inline void waveram_plot(int y, int x, UINT16 color)
{
	if (zeus_cliprect.contains(x, y))
		WAVERAM_WRITEPIX(zeus_renderbase, y, x, color);
}

static inline void waveram_plot_check_depth(int y, int x, UINT16 color, UINT16 depth)
{
	if (zeus_cliprect.contains(x, y))
	{
		UINT16 *depthptr = WAVERAM_PTRDEPTH(zeus_renderbase, y, x);
		if (depth <= *depthptr)
		{
			WAVERAM_WRITEPIX(zeus_renderbase, y, x, color);
			*depthptr = depth;
		}
	}
}

static inline void waveram_plot_check_depth_nowrite(int y, int x, UINT16 color, UINT16 depth)
{
	if (zeus_cliprect.contains(x, y))
	{
		UINT16 *depthptr = WAVERAM_PTRDEPTH(zeus_renderbase, y, x);
		if (depth <= *depthptr)
			WAVERAM_WRITEPIX(zeus_renderbase, y, x, color);
	}
}
#endif


/*************************************
 *
 *  Inlines for texel accesses
 *
 *************************************/

// 4x2 block size
static inline UINT8 get_texel_4bit(const void *base, int y, int x, int width)
{
	UINT32 byteoffs = (y / 2) * (width * 2) + ((x / 8) << 3) + ((y & 1) << 2) + ((x / 2) & 3);
	return (WAVERAM_READ8(base, byteoffs) >> (4 * (x & 1))) & 0x0f;
}

static inline UINT8 get_texel_8bit(const void *base, int y, int x, int width)
{
	UINT32 byteoffs = (y / 2) * (width * 2) + ((x / 4) << 3) + ((y & 1) << 2) + (x & 3);
	return WAVERAM_READ8(base, byteoffs);
}

// 2x2 block size
static inline UINT8 get_texel_alt_4bit(const void *base, int y, int x, int width)
{
	UINT32 byteoffs = (y / 4) * (width * 4) + ((x / 4) << 3) + ((y & 3) << 1) + ((x / 2) & 1);
	return (WAVERAM_READ8(base, byteoffs) >> (4 * (x & 1))) & 0x0f;
}

static inline UINT8 get_texel_alt_8bit(const void *base, int y, int x, int width)
{
	UINT32 byteoffs =  (y / 4) * (width * 4) + ((x / 2) << 3) + ((y & 3) << 1) + (x & 1);
	return WAVERAM_READ8(base, byteoffs);
}

/*************************************
 *
 *  Video startup
 *
 *************************************/

midzeus_renderer::midzeus_renderer(midzeus_state &state)
	: poly_manager<float, mz_poly_extra_data, 4, 10000>(state.machine()),
		m_state(state)
{}

VIDEO_START_MEMBER(midzeus_state,midzeus)
{
	int i;

	/* allocate memory for "wave" RAM */
	waveram[0] = auto_alloc_array(machine(), UINT32, WAVERAM0_WIDTH * WAVERAM0_HEIGHT * 8/4);
	waveram[1] = auto_alloc_array(machine(), UINT32, WAVERAM1_WIDTH * WAVERAM1_HEIGHT * 8/4);

	/* initialize a 5-5-5 palette */
	for (i = 0; i < 32768; i++)
		m_palette->set_pen_color(i, pal5bit(i >> 10), pal5bit(i >> 5), pal5bit(i >> 0));

	/* initialize polygon engine */
	poly = auto_alloc(machine(), midzeus_renderer(*this));

	/* we need to cleanup on exit */
	machine().add_notifier(MACHINE_NOTIFY_EXIT, machine_notify_delegate(FUNC(midzeus_state::exit_handler), this));

	yoffs = 0;
	texel_width = 256;
	zeus_renderbase = waveram[1];

	/* state saving */
	save_item(NAME(zeus_fifo));
	save_item(NAME(zeus_fifo_words));
	save_item(NAME(zeus_matrix));
	save_item(NAME(zeus_point));
	save_item(NAME(zeus_light));
	save_item(NAME(zeus_palbase));
	save_item(NAME(zeus_objdata));
	save_item(NAME(zeus_cliprect.min_x));
	save_item(NAME(zeus_cliprect.max_x));
	save_item(NAME(zeus_cliprect.min_y));
	save_item(NAME(zeus_cliprect.max_y));
	save_pointer(NAME(waveram[0]), WAVERAM0_WIDTH * WAVERAM0_HEIGHT * 8 / sizeof(waveram[0][0]));
	save_pointer(NAME(waveram[1]), WAVERAM1_WIDTH * WAVERAM1_HEIGHT * 8 / sizeof(waveram[1][0]));

	/* hack */
	is_mk4b = strcmp(machine().system().name, "mk4b") == 0;
}


void midzeus_state::exit_handler()
{
#if DUMP_WAVE_RAM
	FILE *f = fopen("waveram.dmp", "w");
	int i;

	for (i = 0; i < WAVERAM0_WIDTH * WAVERAM0_HEIGHT; i++)
	{
		if (i % 4 == 0) fprintf(f, "%03X%03X: ", i / WAVERAM0_WIDTH, i % WAVERAM0_WIDTH);
		fprintf(f, " %08X %08X ",
			WAVERAM_READ32(waveram[0], i*2+0),
			WAVERAM_READ32(waveram[0], i*2+1));
		if (i % 4 == 3) fprintf(f, "\n");
	}
	fclose(f);
#endif
}



/*************************************
 *
 *  Video update
 *
 *************************************/

UINT32 midzeus_state::screen_update_midzeus(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int x, y;

	poly->wait("VIDEO_UPDATE");

	/* normal update case */
	if (!machine().input().code_pressed(KEYCODE_W))
	{
		const void *base = waveram1_ptr_from_expanded_addr(m_zeusbase[0xcc]);
		int xoffs = screen.visible_area().min_x;
		for (y = cliprect.min_y; y <= cliprect.max_y; y++)
		{
			UINT16 *dest = &bitmap.pix16(y);
			for (x = cliprect.min_x; x <= cliprect.max_x; x++)
				dest[x] = WAVERAM_READPIX(base, y, x - xoffs) & 0x7fff;
		}
	}

	/* waveram drawing case */
	else
	{
		const void *base;

		if (machine().input().code_pressed(KEYCODE_DOWN)) yoffs += machine().input().code_pressed(KEYCODE_LSHIFT) ? 0x40 : 1;
		if (machine().input().code_pressed(KEYCODE_UP)) yoffs -= machine().input().code_pressed(KEYCODE_LSHIFT) ? 0x40 : 1;
		if (machine().input().code_pressed(KEYCODE_LEFT) && texel_width > 4) { texel_width >>= 1; while (machine().input().code_pressed(KEYCODE_LEFT)) ; }
		if (machine().input().code_pressed(KEYCODE_RIGHT) && texel_width < 512) { texel_width <<= 1; while (machine().input().code_pressed(KEYCODE_RIGHT)) ; }

		if (yoffs < 0) yoffs = 0;
		base = waveram0_ptr_from_block_addr(yoffs << 12);

		for (y = cliprect.min_y; y <= cliprect.max_y; y++)
		{
			UINT16 *dest = &bitmap.pix16(y);
			for (x = cliprect.min_x; x <= cliprect.max_x; x++)
			{
				UINT8 tex = get_texel_8bit(base, y, x, texel_width);
				dest[x] = (tex << 8) | tex;
			}
		}
		popmessage("offs = %06X", yoffs << 12);
	}

	return 0;
}



/*************************************
 *
 *  Core read handler
 *
 *************************************/

READ32_MEMBER(midzeus_state::zeus_r)
{
	int logit = (offset < 0xb0 || offset > 0xb7);
	UINT32 result = m_zeusbase[offset & ~1];

	switch (offset & ~1)
	{
		case 0xf0:
			result = m_screen->hpos();
			logit = 0;
			break;

		case 0xf2:
			result = m_screen->vpos();
			logit = 0;
			break;

		case 0xf4:
			result = 6;
			if (m_screen->vblank())
				result |= 0x800;
			logit = 0;
			break;

		case 0xf6:      // status -- they wait for this & 9 == 0
			// value & $9600 must == $9600 to pass Zeus system test
			result = 0x9600;
			if (m_zeusbase[0xb6] == 0x80040000)
				result |= 1;
			logit = 0;
			break;
	}

	/* 32-bit mode */
	if (m_zeusbase[0x80] & 0x00020000)
	{
		if (offset & 1)
			result >>= 16;
		if (logit)
		{
			if (offset & 1)
				logerror("%06X:zeus32_r(%02X) = %08X -- unexpected in 32-bit mode\n", space.device().safe_pc(), offset, result);
			else if (offset != 0xe0)
				logerror("%06X:zeus32_r(%02X) = %08X\n", space.device().safe_pc(), offset, result);
			else
				logerror("%06X:zeus32_r(%02X) = %08X\n", space.device().safe_pc(), offset, result);
		}
	}

	/* 16-bit mode */
	else
	{
		if (offset & 1)
			result >>= 16;
		else
			result &= 0xffff;
		if (logit)
			logerror("%06X:zeus16_r(%02X) = %04X\n", space.device().safe_pc(), offset, result);
	}
	return result;
}



/*************************************
 *
 *  Core write handler
 *
 *************************************/

WRITE32_MEMBER(midzeus_state::zeus_w)
{
	int logit = zeus_enable_logging || ((offset < 0xb0 || offset > 0xb7) && (offset < 0xe0 || offset > 0xe1));

	if (logit)
		logerror("%06X:zeus_w", space.device().safe_pc());

	/* 32-bit mode */
	if (m_zeusbase[0x80] & 0x00020000)
		zeus_register32_w(offset, data, logit);

	/* 16-bit mode */
	else
		zeus_register16_w(offset, data, logit);
}



/*************************************
 *
 *  Handle writes to an internal
 *  pointer register
 *
 *************************************/

void midzeus_state::zeus_pointer_w(UINT32 which, UINT32 data, int logit)
{
	switch (which & 0xffffff)
	{
		case 0x008000:
		case 0x018000:
			if (logit)
				logerror(" -- setptr(objdata)\n");
			zeus_objdata = data;
			break;

		// case 0x00c040: -- set in model data in invasn
		case 0x00c040:
			if (logit)
				logerror(" -- setptr(palbase)\n");
			zeus_palbase = data;
			break;

		case 0x02c0f0:
			if (logit)
				logerror(" -- setptr(unkbase)\n");
			zeus_unkbase = data;
			break;


		// case 0x004040: -- set via FIFO command in mk4 (len=02)

		// case 0x02c0f0: -- set in model data in mk4 (len=0f)
		// case 0x03c0f0: -- set via FIFO command in mk4 (len=00)

		// case 0x02c0e7: -- set via FIFO command in mk4 (len=08)

		// case 0x04c09c: -- set via FIFO command in mk4 (len=08)

		// case 0x05c0a5: -- set via FIFO command in mk4 (len=21)
		// case 0x80c0a5: -- set via FIFO command in mk4 (len=3f)
		// case 0x81c0a5: -- set via FIFO command in mk4 (len=35)
		// case 0x82c0a5: -- set via FIFO command in mk4 (len=41)


		// case 0x00c0f0: -- set via FIFO command in invasn (len=0f)

		// case 0x00c0b0: -- set via FIFO command in invasn (len=3f) -- seems to be the same as c0a5
		// case 0x05c0b0: -- set via FIFO command in invasn (len=21)

		// case 0x00c09c: -- set via FIFO command in invasn (len=06)

		// case 0x00c0a3: -- set via FIFO command in invasn (len=0a)


		default:
			if (logit)
				logerror(" -- setptr(%06X)\n", which & 0xffffff);
			break;
	}

	if (logit)
		log_waveram(data);
}



/*************************************
 *
 *  Handle register writes
 *
 *************************************/

void midzeus_state::zeus_register16_w(offs_t offset, UINT16 data, int logit)
{
	/* writes to register $CC need to force a partial update */
	if ((offset & ~1) == 0xcc)
		m_screen->update_partial(m_screen->vpos());

	/* write to high part on odd addresses */
	if (offset & 1)
		m_zeusbase[offset & ~1] = (m_zeusbase[offset & ~1] & 0x0000ffff) | (data << 16);

	/* write to low part on event addresses */
	else
		m_zeusbase[offset & ~1] = (m_zeusbase[offset & ~1] & 0xffff0000) | (data & 0xffff);

	/* log appropriately */
	if (logit)
		logerror("(%02X) = %04X [%08X]\n", offset, data & 0xffff, m_zeusbase[offset & ~1]);

	/* handle the update */
	if ((offset & 1) == 0)
		zeus_register_update(offset);
}


void midzeus_state::zeus_register32_w(offs_t offset, UINT32 data, int logit)
{
	/* writes to register $CC need to force a partial update */
	if ((offset & ~1) == 0xcc)
		m_screen->update_partial(m_screen->vpos());

	/* always write to low word? */
	m_zeusbase[offset & ~1] = data;

	/* log appropriately */
	if (logit)
	{
		if (offset & 1)
			logerror("(%02X) = %08X -- unexpected in 32-bit mode\n", offset, data);
		else if (offset != 0xe0)
			logerror("(%02X) = %08X\n", offset, data);
		else
			logerror("(%02X) = %08X\n", offset, data);
	}

	/* handle the update */
	if ((offset & 1) == 0)
		zeus_register_update(offset);
}



/*************************************
 *
 *  Update state after a register write
 *
 *************************************/

void midzeus_state::zeus_register_update(offs_t offset)
{
	/* handle the writes; only trigger on low accesses */
	switch (offset)
	{
		case 0x52:
			m_zeusbase[0xb2] = m_zeusbase[0x52];
			break;

		case 0x60:
			/* invasn writes here to execute a command (?) */
			if (m_zeusbase[0x60] & 1)
			{
				if ((m_zeusbase[0x80] & 0xffffff) == 0x22FCFF)
				{
					// m_zeusbase[0x00] = color
					// m_zeusbase[0x02] = ??? = 0x000C0000
					// m_zeusbase[0x04] = ??? = 0x00000E01
					// m_zeusbase[0x06] = ??? = 0xFFFF0030
					// m_zeusbase[0x08] = vert[0] = (y0 << 16) | x0
					// m_zeusbase[0x0a] = vert[1] = (y1 << 16) | x1
					// m_zeusbase[0x0c] = vert[2] = (y2 << 16) | x2
					// m_zeusbase[0x0e] = vert[3] = (y3 << 16) | x3
					// m_zeusbase[0x18] = ??? = 0xFFFFFFFF
					// m_zeusbase[0x1a] = ??? = 0xFFFFFFFF
					// m_zeusbase[0x1c] = ??? = 0xFFFFFFFF
					// m_zeusbase[0x1e] = ??? = 0xFFFFFFFF
					// m_zeusbase[0x20] = ??? = 0x00000000
					// m_zeusbase[0x22] = ??? = 0x00000000
					// m_zeusbase[0x24] = ??? = 0x00000000
					// m_zeusbase[0x26] = ??? = 0x00000000
					// m_zeusbase[0x40] = ??? = 0x00000000
					// m_zeusbase[0x42] = ??? = 0x00000000
					// m_zeusbase[0x44] = ??? = 0x00000000
					// m_zeusbase[0x46] = ??? = 0x00000000
					// m_zeusbase[0x4c] = ??? = 0x00808080 (brightness?)
					// m_zeusbase[0x4e] = ??? = 0x00808080 (brightness?)
					mz_poly_extra_data& extra = poly->object_data_alloc();
					poly_vertex vert[4];

					vert[0].x = (INT16)m_zeusbase[0x08];
					vert[0].y = (INT16)(m_zeusbase[0x08] >> 16);
					vert[1].x = (INT16)m_zeusbase[0x0a];
					vert[1].y = (INT16)(m_zeusbase[0x0a] >> 16);
					vert[2].x = (INT16)m_zeusbase[0x0c];
					vert[2].y = (INT16)(m_zeusbase[0x0c] >> 16);
					vert[3].x = (INT16)m_zeusbase[0x0e];
					vert[3].y = (INT16)(m_zeusbase[0x0e] >> 16);

					extra.solidcolor = m_zeusbase[0x00];
					extra.zoffset = 0x7fff;

					poly->zeus_draw_debug_quad(zeus_cliprect, vert);
					poly->wait("Normal");
				}
				else
					logerror("Execute unknown command\n");
			}
			break;

		case 0x70:
			zeus_point[0] = m_zeusbase[0x70] << 16;
			break;

		case 0x72:
			zeus_point[1] = m_zeusbase[0x72] << 16;
			break;

		case 0x74:
			zeus_point[2] = m_zeusbase[0x74] << 16;
			break;

		case 0x80:
			/* this bit enables the "FIFO empty" IRQ; since our virtual FIFO is always empty,
			    we simply assert immediately if this is enabled. invasn needs this for proper
			    operations */
			if (m_zeusbase[0x80] & 0x02000000)
				m_maincpu->set_input_line(2, ASSERT_LINE);
			else
				m_maincpu->set_input_line(2, CLEAR_LINE);
			break;

		case 0x84:
			/* MK4: Written in tandem with 0xcc */
			/* MK4: Writes either 0x80 (and 0x000000 to 0xcc) or 0x00 (and 0x800000 to 0xcc) */
			zeus_renderbase = waveram1_ptr_from_expanded_addr(m_zeusbase[0x84] << 16);
			break;

		case 0xb0:
		case 0xb2:
			if ((m_zeusbase[0xb6] >> 16) != 0)
			{
				if ((offset == 0xb0 && (m_zeusbase[0xb6] & 0x02000000) == 0) ||
					(offset == 0xb2 && (m_zeusbase[0xb6] & 0x02000000) != 0))
				{
					void *dest;

					if (m_zeusbase[0xb6] & 0x80000000)
						dest = waveram1_ptr_from_expanded_addr(m_zeusbase[0xb4]);
					else
						dest = waveram0_ptr_from_expanded_addr(m_zeusbase[0xb4]);

					if (m_zeusbase[0xb6] & 0x00100000)
						WAVERAM_WRITE16(dest, 0, m_zeusbase[0xb0]);
					if (m_zeusbase[0xb6] & 0x00200000)
						WAVERAM_WRITE16(dest, 1, m_zeusbase[0xb0] >> 16);
					if (m_zeusbase[0xb6] & 0x00400000)
						WAVERAM_WRITE16(dest, 2, m_zeusbase[0xb2]);
					if (m_zeusbase[0xb6] & 0x00800000)
						WAVERAM_WRITE16(dest, 3, m_zeusbase[0xb2] >> 16);
					if (m_zeusbase[0xb6] & 0x00020000)
						m_zeusbase[0xb4]++;
				}
			}
			break;

		case 0xb4:
			if (m_zeusbase[0xb6] & 0x00010000)
			{
				const UINT32 *src;

				if (m_zeusbase[0xb6] & 0x80000000)
					src = (const UINT32 *)waveram1_ptr_from_expanded_addr(m_zeusbase[0xb4]);
				else
					src = (const UINT32 *)waveram0_ptr_from_expanded_addr(m_zeusbase[0xb4]);

				poly->wait("vram_read");
				m_zeusbase[0xb0] = WAVERAM_READ32(src, 0);
				m_zeusbase[0xb2] = WAVERAM_READ32(src, 1);
			}
			break;

		case 0xc0:
		case 0xc2:
		case 0xc4:
		case 0xc6:
		case 0xc8:
		case 0xca:
			m_screen->update_partial(m_screen->vpos());
			{
				int vtotal = m_zeusbase[0xca] >> 16;
				int htotal = m_zeusbase[0xc6] >> 16;

				rectangle visarea(m_zeusbase[0xc6] & 0xffff, htotal - 3, 0, m_zeusbase[0xc8] & 0xffff);
				if (htotal > 0 && vtotal > 0 && visarea.min_x < visarea.max_x && visarea.max_y < vtotal)
				{
					m_screen->configure(htotal, vtotal, visarea, HZ_TO_ATTOSECONDS((double)MIDZEUS_VIDEO_CLOCK / 8.0 / (htotal * vtotal)));
					zeus_cliprect = visarea;
					zeus_cliprect.max_x -= zeus_cliprect.min_x;
					zeus_cliprect.min_x = 0;
				}
			}
			break;

		case 0xcc:
			m_screen->update_partial(m_screen->vpos());
			log_fifo = machine().input().code_pressed(KEYCODE_L);
			break;

		case 0xe0:
			zeus_fifo[zeus_fifo_words++] = m_zeusbase[0xe0];
			if (zeus_fifo_process(zeus_fifo, zeus_fifo_words))
				zeus_fifo_words = 0;
			break;
	}
}



/*************************************
 *
 *  Process the FIFO
 *
 *************************************/

int midzeus_state::zeus_fifo_process(const UINT32 *data, int numwords)
{
	/* handle logging */
	switch (data[0] >> 24)
	{
		/* 0x00/0x01: set pointer */
		/* in model data, this is 0x0C */
		case 0x00:
		case 0x01:
			if (numwords < 2 && data[0] != 0)
				return FALSE;
			if (log_fifo)
				log_fifo_command(data, numwords, "");
			zeus_pointer_w(data[0] & 0xffffff, data[1], log_fifo);
			break;

		/* 0x13: render model based on previously set information */
		case 0x13:  /* invasn */
			if (log_fifo)
				log_fifo_command(data, numwords, "");
			zeus_draw_model((m_zeusbase[0x06] << 16), log_fifo);
			break;

		/* 0x17: write 16-bit value to low registers */
		case 0x17:
			if (log_fifo)
				log_fifo_command(data, numwords, " -- reg16");
			zeus_register16_w((data[0] >> 16) & 0x7f, data[0], log_fifo);
			break;

		/* 0x18: write 32-bit value to low registers */
		/* in model data, this is 0x19 */
		case 0x18:
			if (numwords < 2)
				return FALSE;
			if (log_fifo)
				log_fifo_command(data, numwords, " -- reg32");
			zeus_register32_w((data[0] >> 16) & 0x7f, data[1], log_fifo);
			break;

		/* 0x1A/0x1B: sync pipeline(?) */
		case 0x1a:
		case 0x1b:
			if (log_fifo)
				log_fifo_command(data, numwords, " -- sync\n");
			break;

		/* 0x1C/0x1E: write matrix and translation vector */
		case 0x1c:
		case 0x1e:

			/* single matrix form */
			if ((data[0] & 0xffff) != 0x7fff)
			{
				/* requires 8 words total */
				if (numwords < 8)
					return FALSE;
				if (log_fifo)
				{
					log_fifo_command(data, numwords, "");
					logerror("\n\t\tmatrix ( %04X %04X %04X ) ( %04X %04X %04X ) ( %04X %04X %04X )\n\t\tvector %8.2f %8.2f %8.5f\n",
						data[2] & 0xffff,   data[2] >> 16,      data[0] & 0xffff,
						data[3] & 0xffff,   data[3] >> 16,      data[1] >> 16,
						data[4] & 0xffff,   data[4] >> 16,      data[1] & 0xffff,
						(double)(INT32)data[5] * (1.0 / 65536.0),
						(double)(INT32)data[6] * (1.0 / 65536.0),
						(double)(INT32)data[7] * (1.0 / (65536.0 * 512.0)));
				}

				/* extract the matrix from the raw data */
				zeus_matrix[0][0] = data[2];    zeus_matrix[0][1] = data[2] >> 16;  zeus_matrix[0][2] = data[0];
				zeus_matrix[1][0] = data[3];    zeus_matrix[1][1] = data[3] >> 16;  zeus_matrix[1][2] = data[1] >> 16;
				zeus_matrix[2][0] = data[4];    zeus_matrix[2][1] = data[4] >> 16;  zeus_matrix[2][2] = data[1];

				/* extract the translation point from the raw data */
				zeus_point[0] = data[5];
				zeus_point[1] = data[6];
				zeus_point[2] = data[7];
			}

			/* double matrix form */
			else
			{
				INT16 matrix1[3][3];
				INT16 matrix2[3][3];

				/* requires 13 words total */
				if (numwords < 13)
					return FALSE;
				if (log_fifo)
				{
					log_fifo_command(data, numwords, "");
					logerror("\n\t\tmatrix ( %04X %04X %04X ) ( %04X %04X %04X ) ( %04X %04X %04X )\n\t\tmatrix ( %04X %04X %04X ) ( %04X %04X %04X ) ( %04X %04X %04X )\n\t\tvector %8.2f %8.2f %8.5f\n",
						data[4] & 0xffff,   data[4] >> 16,      data[5] >> 16,
						data[8] & 0xffff,   data[8] >> 16,      data[6] >> 16,
						data[9] & 0xffff,   data[9] >> 16,      data[7] >> 16,
						data[1] & 0xffff,   data[2] & 0xffff,   data[3] & 0xffff,
						data[1] >> 16,      data[2] >> 16,      data[3] >> 16,
						data[5] & 0xffff,   data[6] & 0xffff,   data[7] & 0xffff,
						(double)(INT32)data[10] * (1.0 / 65536.0),
						(double)(INT32)data[11] * (1.0 / 65536.0),
						(double)(INT32)data[12] * (1.0 / (65536.0 * 512.0)));
				}

				/* extract the first matrix from the raw data */
				matrix1[0][0] = data[4];        matrix1[0][1] = data[4] >> 16;  matrix1[0][2] = data[5] >> 16;
				matrix1[1][0] = data[8];        matrix1[1][1] = data[8] >> 16;  matrix1[1][2] = data[6] >> 16;
				matrix1[2][0] = data[9];        matrix1[2][1] = data[9] >> 16;  matrix1[2][2] = data[7] >> 16;

				/* extract the second matrix from the raw data */
				matrix2[0][0] = data[1];        matrix2[0][1] = data[2];        matrix2[0][2] = data[3];
				matrix2[1][0] = data[1] >> 16;  matrix2[1][1] = data[2] >> 16;  matrix2[1][2] = data[3] >> 16;
				matrix2[2][0] = data[5];        matrix2[2][1] = data[6];        matrix2[2][2] = data[7];

				/* multiply them together to get the final matrix */
				zeus_matrix[0][0] = ((INT64)(matrix1[0][0] * matrix2[0][0]) + (INT64)(matrix1[0][1] * matrix2[1][0]) + (INT64)(matrix1[0][2] * matrix2[2][0])) >> 16;
				zeus_matrix[0][1] = ((INT64)(matrix1[0][0] * matrix2[0][1]) + (INT64)(matrix1[0][1] * matrix2[1][1]) + (INT64)(matrix1[0][2] * matrix2[2][1])) >> 16;
				zeus_matrix[0][2] = ((INT64)(matrix1[0][0] * matrix2[0][2]) + (INT64)(matrix1[0][1] * matrix2[1][2]) + (INT64)(matrix1[0][2] * matrix2[2][2])) >> 16;
				zeus_matrix[1][0] = ((INT64)(matrix1[1][0] * matrix2[0][0]) + (INT64)(matrix1[1][1] * matrix2[1][0]) + (INT64)(matrix1[1][2] * matrix2[2][0])) >> 16;
				zeus_matrix[1][1] = ((INT64)(matrix1[1][0] * matrix2[0][1]) + (INT64)(matrix1[1][1] * matrix2[1][1]) + (INT64)(matrix1[1][2] * matrix2[2][1])) >> 16;
				zeus_matrix[1][2] = ((INT64)(matrix1[1][0] * matrix2[0][2]) + (INT64)(matrix1[1][1] * matrix2[1][2]) + (INT64)(matrix1[1][2] * matrix2[2][2])) >> 16;
				zeus_matrix[2][0] = ((INT64)(matrix1[2][0] * matrix2[0][0]) + (INT64)(matrix1[2][1] * matrix2[1][0]) + (INT64)(matrix1[2][2] * matrix2[2][0])) >> 16;
				zeus_matrix[2][1] = ((INT64)(matrix1[2][0] * matrix2[0][1]) + (INT64)(matrix1[2][1] * matrix2[1][1]) + (INT64)(matrix1[2][2] * matrix2[2][1])) >> 16;
				zeus_matrix[2][2] = ((INT64)(matrix1[2][0] * matrix2[0][2]) + (INT64)(matrix1[2][1] * matrix2[1][2]) + (INT64)(matrix1[2][2] * matrix2[2][2])) >> 16;

				/* extract the translation point from the raw data */
				zeus_point[0] = data[10];
				zeus_point[1] = data[11];
				zeus_point[2] = data[12];
			}
			break;

		/* 0x23: some additional X,Y,Z coordinates */
		/* 0x2e: same for invasn */
		case 0x23:
		case 0x2e:
			if (numwords < 2)
				return FALSE;
			if (log_fifo)
			{
				log_fifo_command(data, numwords, "");
				logerror(" -- light xyz = %d,%d,%d\n", (INT16)data[1], (INT16)(data[1] >> 16), (INT16)data[0]);
			}

			zeus_light[0] = (INT16)(data[1] & 0xffff);
			zeus_light[1] = (INT16)(data[1] >> 16);
			zeus_light[2] = (INT16)(data[0] & 0xffff);
			break;
		/* 0x25: display control? */
		/* 0x28: same for mk4b */
		/* 0x30: same for invasn */
		case 0x25:
		{
			/* 0x25 is used differently in mk4b. What determines this? */
			if (is_mk4b)
			{
				if (numwords < 2)
					return FALSE;

				break;
			}
		}
		case 0x28:
		case 0x30:
			if (numwords < 4 || ((data[0] & 0x808000) && numwords < 10))
				return FALSE;

			if (log_fifo)
				log_fifo_command(data, numwords, " -- alt. quad and hack screen clear\n");

			if ((numwords < 10) && (data[0] & 0xffff7f) == 0)
			{
				/* not right -- just a hack */
				int x, y;
				for (y = zeus_cliprect.min_y; y <= zeus_cliprect.max_y; y++)
					for (x = zeus_cliprect.min_x; x <= zeus_cliprect.max_x; x++)
						waveram_plot_depth(y, x, 0, 0x7fff);
			}
			else
			{
				UINT32 texdata = (m_zeusbase[0x06] << 16) | (m_zeusbase[0x00] >> 16);
				poly->zeus_draw_quad(FALSE, data, texdata, log_fifo);
			}
			break;

		/* 0x2d: unknown - invasn */
		/* 0x70: same for mk4 */
		case 0x2d:
		case 0x70:
			if (log_fifo)
				log_fifo_command(data, numwords, "\n");
			break;

		/* 0x67: render model with inline texture info */
		case 0x67:
			if (numwords < 3)
				return FALSE;
			if (log_fifo)
				log_fifo_command(data, numwords, "");
			zeus_objdata = data[1];
			zeus_draw_model(data[2], log_fifo);
			break;

		default:
			printf("Unknown command %08X\n", data[0]);
			if (log_fifo)
				log_fifo_command(data, numwords, "\n");
			break;
	}
	return TRUE;
}



/*************************************
 *
 *  Draw a model in waveram
 *
 *************************************/

void midzeus_state::zeus_draw_model(UINT32 texdata, int logit)
{
	UINT32 databuffer[32];
	int databufcount = 0;
	int model_done = FALSE;

	if (logit)
		logerror(" -- model @ %08X\n", zeus_objdata);

	while (zeus_objdata != 0 && !model_done)
	{
		const void *base = waveram0_ptr_from_block_addr(zeus_objdata);
		int count = zeus_objdata >> 24;
		int curoffs;

		/* reset the objdata address */
		zeus_objdata = 0;

		/* loop until we run out of data */
		for (curoffs = 0; curoffs <= count; curoffs++)
		{
			int countneeded;
			UINT8 cmd;

			/* accumulate 2 words of data */
			databuffer[databufcount++] = WAVERAM_READ32(base, curoffs * 2 + 0);
			databuffer[databufcount++] = WAVERAM_READ32(base, curoffs * 2 + 1);

			/* if this is enough, process the command */
			cmd = databuffer[0] >> 24;
			countneeded = (cmd == 0x25 || cmd == 0x30 || cmd == 0x28) ? 14 : 2;
			if (databufcount == countneeded)
			{
				/* handle logging of the command */
				if (logit)
				{
					int offs;
					logerror("\t");
					for (offs = 0; offs < databufcount; offs++)
						logerror("%08X ", databuffer[offs]);
					logerror("-- ");
				}

				/* handle the command */
				switch (cmd)
				{
					case 0x08:
						if (logit)
							logerror("end of model\n");
						model_done = TRUE;
						break;

					case 0x0c:  /* mk4/invasn */
						zeus_pointer_w(databuffer[0] & 0xffffff, databuffer[1], logit);
						break;

					case 0x17:  /* mk4 */
						if (logit)
							logerror("reg16");
						zeus_register16_w((databuffer[0] >> 16) & 0x7f, databuffer[0], logit);
						if (((databuffer[0] >> 16) & 0x7f) == 0x06)
							texdata = (texdata & 0xffff) | (m_zeusbase[0x06] << 16);
						break;

					case 0x19:  /* invasn */
						if (logit)
							logerror("reg32");
						zeus_register32_w((databuffer[0] >> 16) & 0x7f, databuffer[1], logit);
						if (((databuffer[0] >> 16) & 0x7f) == 0x06)
							texdata = (texdata & 0xffff) | (m_zeusbase[0x06] << 16);
						break;

					case 0x25:  /* mk4 */
					case 0x28:  /* mk4r1 */
					case 0x30:  /* invasn */
						poly->zeus_draw_quad(TRUE, databuffer, texdata, logit);
						break;

					default:
						if (logit)
							logerror("unknown\n");
						break;
				}

				/* reset the count */
				databufcount = 0;
			}
		}
	}
}



/*************************************
 *
 *  Draw a quad
 *
 *************************************/

void midzeus_renderer::zeus_draw_quad(int long_fmt, const UINT32 *databuffer, UINT32 texdata, int logit)
{
	poly_vertex clipvert[8];
	poly_vertex vert[4];
	UINT32 ushift, vshift;
	float maxy, maxx;
	UINT32 texbase, texwshift;
	UINT32 numverts;

	UINT32 ctrl_word = databuffer[long_fmt ? 1 : 9];

	texbase = ((texdata >> 10) & 0x3f0000) | (texdata & 0xffff);
	texwshift = (texdata >> 22) & 7;

	ushift = 8 - ((m_state.m_zeusbase[0x04] >> 4) & 3);
	vshift = 8 - ((m_state.m_zeusbase[0x04] >> 6) & 3);

	int xy_offset = long_fmt ? 2 : 1;

	for (UINT32 i = 0; i < 4; i++)
	{
		UINT32 ixy = databuffer[xy_offset + i*2];
		UINT32 iuvz = databuffer[xy_offset + 1 + i*2];
		INT32 xo = (INT16)ixy;
		INT32 yo = (INT16)(ixy >> 16);
		INT32 zo = (INT16)iuvz;
		UINT8 u = iuvz >> 16;
		UINT8 v = iuvz >> 24;
		INT64 x, y, z;


		x = (INT64)(xo * zeus_matrix[0][0]) + (INT64)(yo * zeus_matrix[0][1]) + (INT64)(zo * zeus_matrix[0][2]) + zeus_point[0];
		y = (INT64)(xo * zeus_matrix[1][0]) + (INT64)(yo * zeus_matrix[1][1]) + (INT64)(zo * zeus_matrix[1][2]) + zeus_point[1];
		z = (INT64)(xo * zeus_matrix[2][0]) + (INT64)(yo * zeus_matrix[2][1]) + (INT64)(zo * zeus_matrix[2][2]) + zeus_point[2];

		// Rounding hack
		x = (x + 0x00004000) & ~0x00007fffULL;
		y = (y + 0x00004000) & ~0x00007fffULL;
		z = (z + 0x00004000) & ~0x00007fffULL;

		// back face cull using polygon normal and first vertex
		if (i == 0)
		{
			INT16 normal[3];
			INT32 rotnormal[3];

			normal[0] = (INT8)(databuffer[0] >> 0);
			normal[1] = (INT8)(databuffer[0] >> 8);
			normal[2] = (INT8)(databuffer[0] >> 16);

			rotnormal[0] = normal[0] * zeus_matrix[0][0] + normal[1] * zeus_matrix[0][1] + normal[2] * zeus_matrix[0][2];
			rotnormal[1] = normal[0] * zeus_matrix[1][0] + normal[1] * zeus_matrix[1][1] + normal[2] * zeus_matrix[1][2];
			rotnormal[2] = normal[0] * zeus_matrix[2][0] + normal[1] * zeus_matrix[2][1] + normal[2] * zeus_matrix[2][2];

			INT64 dot = rotnormal[0] * x + rotnormal[1] * y + rotnormal[2] * z;

			if (dot >= 0)
				return;
		}

		if (long_fmt)
		{
#if 0
			// TODO: Lighting
			UINT32 inormal = databuffer[10 + i];
			INT32 xn = (INT32)(((inormal >>  0) & 0x3ff) << 22) >> 22;
			INT32 yn = (INT32)(((inormal >> 10) & 0x3ff) << 22) >> 22;
			INT32 zn = (INT32)(((inormal >> 20) & 0x3ff) << 22) >> 22;
#endif
		}

		vert[i].x = x;
		vert[i].y = y;
		vert[i].p[0] = z;
		vert[i].p[1] = u << ushift;
		vert[i].p[2] = v << vshift;
		vert[i].p[3] = 0xffff;

		if (logit)
		{
			m_state.logerror("\t\t(%f,%f,%f) UV:(%02X,%02X) UV_SCALE:(%02X,%02X) (%03X,%03X,%03X) dot=%08X\n",
					(double) vert[i].x * (1.0 / 65536.0), (double) vert[i].y * (1.0 / 65536.0), (double) vert[i].p[0] * (1.0 / 65536.0),
					(iuvz >> 16) & 0xff, (iuvz >> 24) & 0xff,
					(int)(vert[i].p[1] / 256.0f), (int)(vert[i].p[2] / 256.0f),
					(databuffer[10 + i] >> 20) & 0x3ff, (databuffer[10 + i] >> 10) & 0x3ff, (databuffer[10 + i] >> 0) & 0x3ff,
					0);
		}
	}

	numverts = poly->zclip_if_less(4, &vert[0], &clipvert[0], 4, 512.0f);
	if (numverts < 3)
		return;

	maxx = maxy = -1000.0f;
	for (UINT32 i = 0; i < numverts; i++)
	{
		float ooz = 512.0f / clipvert[i].p[0];

		clipvert[i].x *= ooz;
		clipvert[i].y *= ooz;
		clipvert[i].x += 200.5f;
		clipvert[i].y += 128.5f;

		maxx = MAX(maxx, clipvert[i].x);
		maxy = MAX(maxy, clipvert[i].y);

		if (logit)
			m_state.logerror("\t\t\tTranslated=(%f,%f,%f)\n", (double) clipvert[i].x, (double) clipvert[i].y, (double) clipvert[i].p[0]);
	}
	for (UINT32 i = 0; i < numverts; i++)
	{
		if (clipvert[i].x == maxx)
			clipvert[i].x += 0.0005f;
		if (clipvert[i].y == maxy)
			clipvert[i].y += 0.0005f;
	}

	mz_poly_extra_data& extra = poly->object_data_alloc();

	if (ctrl_word & 0x01000000)
	{
		UINT32 tex_type = (texdata >> 16) & 3;
		extra.texwidth = 512 >> texwshift;
		extra.voffset = ctrl_word & 0xffff;

		extra.texbase = waveram0_ptr_from_texture_addr(texbase, extra.texwidth);

		if (tex_type == 1)
		{
			extra.get_texel = texdata & 0x00200000 ? get_texel_8bit : get_texel_4bit;
		}
		else if (tex_type == 2)
		{
			extra.get_texel = texdata & 0x00200000 ? get_texel_alt_8bit : get_texel_alt_4bit;
		}
		else
		{
			printf("Unknown texture type: %d\n", tex_type);
			return;
		}
	}

	extra.ctrl_word = ctrl_word;
	extra.solidcolor = m_state.m_zeusbase[0x00] & 0x7fff;
	extra.zoffset = m_state.m_zeusbase[0x7e] >> 16;
	extra.alpha = m_state.m_zeusbase[0x4e];
	extra.blend = m_state.m_zeusbase[0x5c];
	extra.depth_test_enable = !(m_state.m_zeusbase[0x04] & 0x800);
	extra.depth_write_enable = m_state.m_zeusbase[0x04] & 0x200;
	extra.transcolor = ((ctrl_word >> 16) & 1) ? 0 : 0x100;
	extra.palbase = waveram0_ptr_from_block_addr(zeus_palbase);

	// Note: Before being upgraded to the new polygon rasterizing code, this function call was
	//       a poly_render_quad_fan.  It appears as though the new code defaults to a fan if
	//       the template argument is 4, but keep an eye out for missing quads.
	poly->render_polygon<4>(zeus_cliprect,
							render_delegate(FUNC(midzeus_renderer::render_poly), this),
							4,
							clipvert);
}

void midzeus_renderer::zeus_draw_debug_quad(const rectangle& rect, const vertex_t *vert)
{
	poly->render_polygon<4>(rect, render_delegate(FUNC(midzeus_renderer::render_poly_solid_fixedz), this), 0, vert);
}


/*************************************
 *
 *  Rasterizers
 *
 *************************************/

void midzeus_renderer::render_poly(INT32 scanline, const extent_t& extent, const mz_poly_extra_data& object, int threadid)
{
	INT32 curz = extent.param[0].start;
	INT32 curu = extent.param[1].start;
	INT32 curv = extent.param[2].start;
	INT32 curi = extent.param[3].start;
	INT32 dzdx = extent.param[0].dpdx;
	INT32 dudx = extent.param[1].dpdx;
	INT32 dvdx = extent.param[2].dpdx;
	INT32 didx = extent.param[3].dpdx;
	const void *texbase = object.texbase;
	const void *palbase = object.palbase;
	UINT16 transcolor = object.transcolor;
	UINT32 texwidth = object.texwidth;

	for (UINT32 x = extent.startx; x < extent.stopx; x++)
	{
		UINT16 *depthptr = WAVERAM_PTRDEPTH(zeus_renderbase, scanline, x);
		INT32 depth = (curz >> 16) + object.zoffset;

		if (depth > 0x7fff)
			depth = 0x7fff;

		UINT32 i8 = curi >> 8;

		bool depth_pass;

		if (object.depth_test_enable)
			depth_pass = depth >= 0 && depth <= *depthptr;
		else
			depth_pass = true;

		if (depth_pass)
		{
			rgb_t src=0;

			bool src_valid = true;

			if ((object.ctrl_word & 0x000c0000) == 0x000c0000)
			{
				src.set_r(pal5bit(object.solidcolor >> 10));
				src.set_g(pal5bit(object.solidcolor >> 5));
				src.set_b(pal5bit(object.solidcolor));
			}
			else
			{
				UINT32 u0 = curu >> 8;
				UINT32 v0 = object.voffset + (curv >> 8);
				UINT32 u1 = u0 + 1;
				UINT32 v1 = v0 + 1;

				UINT8 texels[4];

				texels[0] = object.get_texel(texbase, v0, u0, texwidth);
				texels[1] = object.get_texel(texbase, v0, u1, texwidth);
				texels[2] = object.get_texel(texbase, v1, u0, texwidth);
				texels[3] = object.get_texel(texbase, v1, u1, texwidth);

				if (texels[0] != transcolor)
				{
					rgb_t color[4] = {0, 0, 0, 0};

					for (UINT32 i = 0; i < 4; ++i)
					{
						UINT16 pix = WAVERAM_READ16(palbase, texels[i]);

						color[i].set_r(pal5bit(pix >> 10));
						color[i].set_g(pal5bit(pix >> 5));
						color[i].set_b(pal5bit(pix));
					}

					src = rgbaint_t::bilinear_filter(color[0], color[1], color[2], color[3], curu & 0xff, curv & 0xff);
				}
				else
				{
					src_valid = false;
				}
			}

			if (src_valid)
			{
				UINT32 srcr = src.r();
				UINT32 srcg = src.g();
				UINT32 srcb = src.b();

				UINT32 dstr = 0;
				UINT32 dstg = 0;
				UINT32 dstb = 0;

				UINT32 outr = 0;
				UINT32 outg = 0;
				UINT32 outb = 0;

				UINT32 srca = object.alpha & 0xff;
				UINT32 dsta = (object.alpha >> 8) & 0xff;

				// Destination enable?
				if (object.blend & 0x00800000)
				{
					UINT16 dst = WAVERAM_READPIX(zeus_renderbase, scanline, x);

					dstr = (dst >> 10) & 0x1f;
					dstg = (dst >> 5) & 0x1f;
					dstb = dst & 0x1f;

					dstr = (dstr << 3) | (dstr >> 2);
					dstg = (dstg << 3) | (dstg >> 2);
					dstb = (dstb << 3) | (dstb >> 2);
				}

				switch (object.blend)
				{
					case BLEND_OPAQUE1:
					{
						outr = srcr;
						outg = srcg;
						outb = srcb;
						break;
					}

					case BLEND_OPAQUE2:
					{
						outr = (srcr * i8) >> 8;
						outg = (srcg * i8) >> 8;
						outb = (srcb * i8) >> 8;
						break;
					}

					case BLEND_OPAQUE3:
					{
						outr = (srcr * i8) >> 8;
						outg = (srcg * i8) >> 8;
						outb = (srcb * i8) >> 8;
						break;
					}

					case BLEND_OPAQUE4:
					{
						outr = srcr;
						outg = srcg;
						outb = srcb;
						break;
					}

					case BLEND_OPAQUE5:
					{
						// TODO: Fog factor?
						outr = (srcr * srca) >> 8;
						outg = (srcg * srca) >> 8;
						outb = (srcb * srca) >> 8;
						break;
					}

					case BLEND_ADD1:
					{
						outr = ((srcr * srca) >> 8) + dstr;
						outg = ((srcg * srca) >> 8) + dstg;
						outb = ((srcb * srca) >> 8) + dstb;
						break;
					}

					case BLEND_ADD2:
					{
						outr = ((srcr * srca) >> 8) + ((dstr * (dsta << 1)) >> 8);
						outg = ((srcg * srca) >> 8) + ((dstg * (dsta << 1)) >> 8);
						outb = ((srcb * srca) >> 8) + ((dstb * (dsta << 1)) >> 8);
						break;
					}

					case BLEND_MUL1:
					{
						outr = (((srcr * (srca << 1)) >> 8) * dstr) >> 8;
						outg = (((srcg * (srca << 1)) >> 8) * dstg) >> 8;
						outb = (((srcb * (srca << 1)) >> 8) * dstb) >> 8;
						break;
					}
					default:
					{
						outr = srcr;
						outg = srcg;
						outb = srcb;
						break;
					}
				}

				outr = outr > 0xff ? 0xff : outr;
				outg = outg > 0xff ? 0xff : outg;
				outb = outb > 0xff ? 0xff : outb;

				outr >>= 3;
				outg >>= 3;
				outb >>= 3;

				WAVERAM_WRITEPIX(zeus_renderbase, scanline, x, (outr << 10) | (outg << 5) | outb);

				if (object.depth_write_enable)
					*depthptr = depth;
			}
		}

		curz += dzdx;
		curu += dudx;
		curv += dvdx;
		curi += didx;
	}
}



void midzeus_renderer::render_poly_solid_fixedz(INT32 scanline, const extent_t& extent, const mz_poly_extra_data& object, int threadid)
{
	UINT16 color = object.solidcolor;
	UINT16 depth = object.zoffset;
	int x;

	for (x = extent.startx; x < extent.stopx; x++)
		waveram_plot_depth(scanline, x, color, depth);
}



/*************************************
 *
 *  Debugging tools
 *
 *************************************/

void midzeus_state::log_fifo_command(const UINT32 *data, int numwords, const char *suffix)
{
	int wordnum;

	logerror("Zeus cmd %02X :", data[0] >> 24);
	for (wordnum = 0; wordnum < numwords; wordnum++)
		logerror(" %08X", data[wordnum]);
	logerror("%s", suffix);
}


void midzeus_state::log_waveram(UINT32 length_and_base)
{
	static struct
	{
		UINT32 lab;
		UINT32 checksum;
	} recent_entries[100];

	UINT32 numoctets = (length_and_base >> 24) + 1;
	const UINT32 *ptr = (const UINT32 *)waveram0_ptr_from_block_addr(length_and_base);
	UINT32 checksum = length_and_base;
	int foundit = FALSE;
	int i;

	for (i = 0; i < numoctets; i++)
		checksum += ptr[i*2] + ptr[i*2+1];

	for (i = 0; i < ARRAY_LENGTH(recent_entries); i++)
		if (recent_entries[i].lab == length_and_base && recent_entries[i].checksum == checksum)
		{
			foundit = TRUE;
			break;
		}

	if (i == ARRAY_LENGTH(recent_entries))
		i--;
	if (i != 0)
	{
		memmove(&recent_entries[1], &recent_entries[0], i * sizeof(recent_entries[0]));
		recent_entries[0].lab = length_and_base;
		recent_entries[0].checksum = checksum;
	}
	if (foundit)
		return;

	for (i = 0; i < numoctets; i++)
		logerror("\t%02X: %08X %08X\n", i, ptr[i*2], ptr[i*2+1]);
}
