-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

function maintargetosdoptions(_target,_subtarget)
end


project ("osd_" .. _OPTIONS["osd"])
	uuid (os.uuid("osd_" .. _OPTIONS["osd"]))
	kind (LIBTYPE)

	removeflags {
		"SingleOutputDir",
	}
	
	options {
		"ForceCPP",
	}

	dofile("osdmini_cfg.lua")
	
	includedirs {
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "src/osd/sdl",
		MAME_DIR .. "src/osd/modules/render",
		MAME_DIR .. "3rdparty",
		MAME_DIR .. "3rdparty/winpcap/Include",
		MAME_DIR .. "3rdparty/bgfx/include",
		MAME_DIR .. "3rdparty/bx/include",
	}

	files {
		MAME_DIR .. "src/osd/osdnet.c",
		MAME_DIR .. "src/osd/osdmini/minimain.c",
		MAME_DIR .. "src/osd/modules/lib/osdobj_common.c",
		MAME_DIR .. "src/osd/modules/font/font_sdl.c",
		MAME_DIR .. "src/osd/modules/font/font_windows.c",
		MAME_DIR .. "src/osd/modules/font/font_osx.c",
		MAME_DIR .. "src/osd/modules/font/font_none.c",
		MAME_DIR .. "src/osd/modules/netdev/taptun.c",
		MAME_DIR .. "src/osd/modules/netdev/pcap.c",
		MAME_DIR .. "src/osd/modules/netdev/none.c",
		MAME_DIR .. "src/osd/modules/midi/portmidi.c",
		MAME_DIR .. "src/osd/modules/midi/none.c",
		MAME_DIR .. "src/osd/modules/sound/js_sound.c",
		MAME_DIR .. "src/osd/modules/sound/direct_sound.c",
		MAME_DIR .. "src/osd/modules/sound/coreaudio_sound.c",		
		MAME_DIR .. "src/osd/modules/sound/sdl_sound.c",
		MAME_DIR .. "src/osd/modules/sound/none.c",
	}
	
project ("ocore_" .. _OPTIONS["osd"])
	uuid (os.uuid("ocore_" .. _OPTIONS["osd"]))
	kind (LIBTYPE)

	options {
		"ForceCPP",
	}

	removeflags {
		"SingleOutputDir",	
	}

	dofile("osdmini_cfg.lua")
	
	includedirs {
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
	}

	if _OPTIONS["targetos"]=="linux" then
		BASE_TARGETOS = "unix"
		SDLOS_TARGETOS = "unix"
		SYNC_IMPLEMENTATION = "tc"
	end

	if _OPTIONS["targetos"]=="windows" then
		BASE_TARGETOS = "win32"
		SDLOS_TARGETOS = "win32"
		SYNC_IMPLEMENTATION = "windows"
	end

	if _OPTIONS["targetos"]=="macosx" then
		BASE_TARGETOS = "unix"
		SDLOS_TARGETOS = "macosx"
		SYNC_IMPLEMENTATION = "ntc"
	end

	files {
		MAME_DIR .. "src/osd/osdcore.c",
		MAME_DIR .. "src/osd/modules/osdmodule.c",
		MAME_DIR .. "src/osd/osdmini/minidir.c",
		MAME_DIR .. "src/osd/osdmini/minifile.c",
		MAME_DIR .. "src/osd/osdmini/minimisc.c",
		MAME_DIR .. "src/osd/osdmini/minisync.c",
		MAME_DIR .. "src/osd/osdmini/minitime.c",
		MAME_DIR .. "src/osd/modules/sync/work_mini.c",
	}
