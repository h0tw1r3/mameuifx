-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   video.lua
--
--   Rules for building video cores
--
---------------------------------------------------------------------------

--------------------------------------------------
--
--@src/emu/video/315_5124.h,VIDEOS += SEGA315_5124
--------------------------------------------------

if (VIDEOS["SEGA315_5124"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/315_5124.c",
		MAME_DIR .. "src/emu/video/315_5124.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/315_5313.h,VIDEOS += SEGA315_5313
--------------------------------------------------

if (VIDEOS["SEGA315_5313"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/315_5313.c",
		MAME_DIR .. "src/emu/video/315_5313.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/bufsprite.h,VIDEOS += BUFSPRITE
--------------------------------------------------

if (VIDEOS["BUFSPRITE"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/bufsprite.c",
		MAME_DIR .. "src/emu/video/bufsprite.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/cdp1861.h,VIDEOS += CDP1861
--------------------------------------------------

if (VIDEOS["CDP1861"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/cdp1861.c",
		MAME_DIR .. "src/emu/video/cdp1861.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/cdp1862.h,VIDEOS += CDP1862
--------------------------------------------------

if (VIDEOS["CDP1862"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/cdp1862.c",
		MAME_DIR .. "src/emu/video/cdp1862.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/crt9007.h,VIDEOS += CRT9007
--------------------------------------------------

if (VIDEOS["CRT9007"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/crt9007.c",
		MAME_DIR .. "src/emu/video/crt9007.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/crt9021.h,VIDEOS += CRT9021
--------------------------------------------------

if (VIDEOS["CRT9021"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/crt9021.c",
		MAME_DIR .. "src/emu/video/crt9021.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/crt9212.h,VIDEOS += CRT9212
--------------------------------------------------

if (VIDEOS["CRT9212"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/crt9212.c",
		MAME_DIR .. "src/emu/video/crt9212.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/dl1416.h,VIDEOS += DL1416
--------------------------------------------------

if (VIDEOS["DL1416"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/dl1416.c",
		MAME_DIR .. "src/emu/video/dl1416.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/dm9368.h,VIDEOS += DM9368
--------------------------------------------------

if (VIDEOS["DM9368"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/dm9368.c",
		MAME_DIR .. "src/emu/video/dm9368.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/ef9340_1.h,VIDEOS += EF9340_1
--------------------------------------------------

if (VIDEOS["EF9340_1"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/ef9340_1.c",
		MAME_DIR .. "src/emu/video/ef9340_1.h",
		MAME_DIR .. "src/emu/video/ef9341_chargen.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/ef9345.h,VIDEOS += EF9345
--------------------------------------------------

if (VIDEOS["EF9345"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/ef9345.c",
		MAME_DIR .. "src/emu/video/ef9345.h",
	}
end

--------------------------------------------------
--@src/emu/video/epic12.h,VIDEOS += EPIC12
--------------------------------------------------

if (VIDEOS["EPIC12"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/epic12.c",
		MAME_DIR .. "src/emu/video/epic12.h",
		MAME_DIR .. "src/emu/video/epic12_blit0.c",
		MAME_DIR .. "src/emu/video/epic12_blit1.c",
		MAME_DIR .. "src/emu/video/epic12_blit2.c",
		MAME_DIR .. "src/emu/video/epic12_blit3.c",
		MAME_DIR .. "src/emu/video/epic12_blit4.c",
		MAME_DIR .. "src/emu/video/epic12_blit5.c",
		MAME_DIR .. "src/emu/video/epic12_blit6.c",
		MAME_DIR .. "src/emu/video/epic12_blit7.c",
		MAME_DIR .. "src/emu/video/epic12_blit8.c",
		MAME_DIR .. "src/emu/video/epic12in.inc",
		MAME_DIR .. "src/emu/video/epic12pixel.inc",
	}
end

--------------------------------------------------
--
--@src/emu/video/fixfreq.h,VIDEOS += FIXFREQ
--------------------------------------------------

if (VIDEOS["FIXFREQ"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/fixfreq.c",
		MAME_DIR .. "src/emu/video/fixfreq.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/gf4500.h,VIDEOS += GF4500
--------------------------------------------------

if (VIDEOS["GF4500"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/gf4500.c",
		MAME_DIR .. "src/emu/video/gf4500.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/gf7600gs.h,VIDEOS += GF7600GS
--------------------------------------------------

if (VIDEOS["GF7600GS"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/gf7600gs.c",
		MAME_DIR .. "src/emu/video/gf7600gs.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/h63484.h,VIDEOS += H63484
--------------------------------------------------

if (VIDEOS["H63484"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/h63484.c",
		MAME_DIR .. "src/emu/video/h63484.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/hd44102.h,VIDEOS += HD44102
--------------------------------------------------

if (VIDEOS["HD44102"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/hd44102.c",
		MAME_DIR .. "src/emu/video/hd44102.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/hd44352.h,VIDEOS += HD44352
--------------------------------------------------

if (VIDEOS["HD44352"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/hd44352.c",
		MAME_DIR .. "src/emu/video/hd44352.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/hd44780.h,VIDEOS += HD44780
--------------------------------------------------

if (VIDEOS["HD44780"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/hd44780.c",
		MAME_DIR .. "src/emu/video/hd44780.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/hd61830.h,VIDEOS += HD61830
--------------------------------------------------

if (VIDEOS["HD61830"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/hd61830.c",
		MAME_DIR .. "src/emu/video/hd61830.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/hd63484.h,VIDEOS += HD63484
--------------------------------------------------

if (VIDEOS["HD63484"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/hd63484.c",
		MAME_DIR .. "src/emu/video/hd63484.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/hd66421.h,VIDEOS += HD66421
--------------------------------------------------

if (VIDEOS["HD66421"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/hd66421.c",
		MAME_DIR .. "src/emu/video/hd66421.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/huc6202.h,VIDEOS += HUC6202
--------------------------------------------------

if (VIDEOS["HUC6202"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/huc6202.c",
		MAME_DIR .. "src/emu/video/huc6202.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/huc6260.h,VIDEOS += HUC6260
--------------------------------------------------

if (VIDEOS["HUC6260"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/huc6260.c",
		MAME_DIR .. "src/emu/video/huc6260.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/huc6261.h,VIDEOS += HUC6261
--------------------------------------------------

if (VIDEOS["HUC6261"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/huc6261.c",
		MAME_DIR .. "src/emu/video/huc6261.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/huc6270.h,VIDEOS += HUC6270
--------------------------------------------------

if (VIDEOS["HUC6270"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/huc6270.c",
		MAME_DIR .. "src/emu/video/huc6270.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/huc6272.h,VIDEOS += HUC6272
--------------------------------------------------

if (VIDEOS["HUC6272"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/huc6272.c",
		MAME_DIR .. "src/emu/video/huc6272.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/i8244.h,VIDEOS += I8244
--------------------------------------------------

if (VIDEOS["I8244"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/i8244.c",
		MAME_DIR .. "src/emu/video/i8244.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/i8275.h,VIDEOS += I8275
--------------------------------------------------

if (VIDEOS["I8275"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/i8275.c",
		MAME_DIR .. "src/emu/video/i8275.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/m50458.h,VIDEOS += M50458
--------------------------------------------------

if (VIDEOS["M50458"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/m50458.c",
		MAME_DIR .. "src/emu/video/m50458.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/mb90082.h,VIDEOS += MB90082
--------------------------------------------------

if (VIDEOS["MB90082"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/mb90082.c",
		MAME_DIR .. "src/emu/video/mb90082.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/mb_vcu.h,VIDEOS += MB_VCU
--------------------------------------------------

if (VIDEOS["MB_VCU"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/mb_vcu.c",
		MAME_DIR .. "src/emu/video/mb_vcu.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/mc6845.h,VIDEOS += MC6845
--------------------------------------------------

if (VIDEOS["MC6845"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/mc6845.c",
		MAME_DIR .. "src/emu/video/mc6845.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/mc6847.h,VIDEOS += MC6847
--------------------------------------------------

if (VIDEOS["MC6847"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/mc6847.c",
		MAME_DIR .. "src/emu/video/mc6847.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/msm6222b.h,VIDEOS += MSM6222B
--------------------------------------------------

if (VIDEOS["MSM6222B"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/msm6222b.c",
		MAME_DIR .. "src/emu/video/msm6222b.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/msm6255.h,VIDEOS += MSM6255
--------------------------------------------------

if (VIDEOS["MSM6255"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/msm6255.c",
		MAME_DIR .. "src/emu/video/msm6255.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/mos6566.h,VIDEOS += MOS6566
--------------------------------------------------

if (VIDEOS["MOS6566"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/mos6566.c",
		MAME_DIR .. "src/emu/video/mos6566.h",
	}
end


files {	
	MAME_DIR .. "src/emu/video/cgapal.c",
	MAME_DIR .. "src/emu/video/cgapal.h",	
}

--------------------------------------------------
--
--@src/emu/video/pc_vga.h,VIDEOS += PC_VGA
--------------------------------------------------

if (VIDEOS["PC_VGA"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/pc_vga.c",
		MAME_DIR .. "src/emu/video/pc_vga.h",
		MAME_DIR .. "src/emu/bus/isa/trident.c",
		MAME_DIR .. "src/emu/bus/isa/trident.h",
		MAME_DIR .. "src/emu/video/clgd542x.c",
		MAME_DIR .. "src/emu/video/blgd542x.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/polylgcy.h,VIDEOS += POLY
--------------------------------------------------

if (VIDEOS["POLY"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/polylgcy.c",
		MAME_DIR .. "src/emu/video/polylgcy.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/psx.h,VIDEOS += PSX
--------------------------------------------------

if (VIDEOS["PSX"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/psx.c",
		MAME_DIR .. "src/emu/video/psx.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/ramdac.h,VIDEOS += RAMDAC
--------------------------------------------------

if (VIDEOS["RAMDAC"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/ramdac.c",
		MAME_DIR .. "src/emu/video/ramdac.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/saa5050.h,VIDEOS += SAA5050
--------------------------------------------------

if (VIDEOS["SAA5050"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/saa5050.c",
		MAME_DIR .. "src/emu/video/saa5050.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/sed1200.h,VIDEOS += SED1200
--------------------------------------------------
if (VIDEOS["SED1200"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/sed1200.c",
		MAME_DIR .. "src/emu/video/sed1200.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/sed1330.h,VIDEOS += SED1330
--------------------------------------------------
if (VIDEOS["SED1330"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/sed1330.c",
		MAME_DIR .. "src/emu/video/sed1330.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/sed1520.h,VIDEOS += SED1520
--------------------------------------------------
if (VIDEOS["SED1520"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/sed1520.c",
		MAME_DIR .. "src/emu/video/sed1520.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/scn2674.h,VIDEOS += SCN2674
--------------------------------------------------
if (VIDEOS["SCN2674"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/scn2674.c",
		MAME_DIR .. "src/emu/video/scn2674.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/snes_ppu.h,VIDEOS += SNES_PPU
--------------------------------------------------
if (VIDEOS["SNES_PPU"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/snes_ppu.c",
		MAME_DIR .. "src/emu/video/snes_ppu.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/stvvdp1.h,VIDEOS += STVVDP
--@src/emu/video/stvvdp2.h,VIDEOS += STVVDP
--------------------------------------------------

if (VIDEOS["STVVDP"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/stvvdp1.c",
		MAME_DIR .. "src/emu/video/stvvdp2.c",
	}
end

--------------------------------------------------
--
--@src/emu/video/t6a04.h,VIDEOS += T6A04
--------------------------------------------------

if (VIDEOS["T6A04"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/t6a04.c",
		MAME_DIR .. "src/emu/video/t6a04.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/tea1002.h,VIDEOS += TEA1002
--------------------------------------------------

if (VIDEOS["TEA1002"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/tea1002.c",
		MAME_DIR .. "src/emu/video/tea1002.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/tlc34076.h,VIDEOS += TLC34076
--------------------------------------------------

if (VIDEOS["TLC34076"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/tlc34076.c",
		MAME_DIR .. "src/emu/video/tlc34076.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/tms34061.h,VIDEOS += TMS34061
--------------------------------------------------

if (VIDEOS["TMS34061"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/tms34061.c",
		MAME_DIR .. "src/emu/video/tms34061.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/tms3556.h,VIDEOS += TMS3556
--------------------------------------------------

if (VIDEOS["TMS3556"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/tms3556.c",
		MAME_DIR .. "src/emu/video/tms3556.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/tms9927.h,VIDEOS += TMS9927
--------------------------------------------------

if (VIDEOS["TMS9927"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/tms9927.c",
		MAME_DIR .. "src/emu/video/tms9927.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/tms9928a.h,VIDEOS += TMS9928A
--------------------------------------------------

if (VIDEOS["TMS9928A"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/tms9928a.c",
		MAME_DIR .. "src/emu/video/tms9928a.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/upd3301.h,VIDEOS += UPD3301
--------------------------------------------------

if (VIDEOS["UPD3301"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/upd3301.c",
		MAME_DIR .. "src/emu/video/upd3301.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/upd7220.h,VIDEOS += UPD7220
--------------------------------------------------

if (VIDEOS["UPD7220"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/upd7220.c",
		MAME_DIR .. "src/emu/video/upd7220.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/upd7227.h,VIDEOS += UPD7227
--------------------------------------------------

if (VIDEOS["UPD7227"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/upd7227.c",
		MAME_DIR .. "src/emu/video/upd7227.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/vic4567.h,VIDEOS += VIC4567
--------------------------------------------------

if (VIDEOS["VIC4567"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/vic4567.c",
		MAME_DIR .. "src/emu/video/vic4567.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/v9938.h,VIDEOS += V9938
--------------------------------------------------

if (VIDEOS["V9938"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/v9938.c",
		MAME_DIR .. "src/emu/video/v9938.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/voodoo.h,VIDEOS += VOODOO
--------------------------------------------------

if (VIDEOS["VOODOO"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/voodoo.c",
		MAME_DIR .. "src/emu/video/voodoo.h",
		MAME_DIR .. "src/emu/video/vooddefs.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/voodoo_pci.h,VIDEOS += VOODOO_PCI
--------------------------------------------------

if (VIDEOS["VOODOO_PCI"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/voodoo_pci.c",
		MAME_DIR .. "src/emu/video/voodoo_pci.h",
	}
end

--------------------------------------------------
--
--@src/emu/video/crtc_ega.h,VIDEOS += CRTC_EGA
--------------------------------------------------

if (VIDEOS["CRTC_EGA"]~=null) then
	files {
		MAME_DIR .. "src/emu/video/crtc_ega.c",
		MAME_DIR .. "src/emu/video/crtc_ega.h",
	}
end
