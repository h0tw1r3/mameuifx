dofile("modules.lua")

premake.make.linkoptions_after = false;
_OPTIONS["STRIP_SYMBOLS"] = "1"
_OPTIONS["DIRECTINPUT"] = "7"

function maintargetosdoptions(_target,_subtarget)
	kind "WindowedApp"

	osdmodulestargetconf()

	configuration { "mingw*-gcc" }
		linkoptions {
			"-municode",
			"-lmingw32",
			"-Wl,--allow-multiple-definition",		
		}

	configuration { "x64", "Release" }
		targetname "mameuifx"

	configuration { "x32", "Release" }
		targetname "mameuifx32"

	configuration { }

	if _OPTIONS["DIRECTINPUT"] == "8" then
		links {
			"dinput8",
		}
	else
		links {
			"dinput",
		}
	end


	if _OPTIONS["USE_SDL"] == "1" then
		links {
			"SDL.dll",
		}
	end

	links {
		"comctl32",
		"comdlg32",
		"shell32",
		"uxtheme",
	}

	override_resources = true;

	files {
		MAME_DIR .. "src/osd/winui/mameui.rc",
	}
	dependency {
		{ "$(OBJDIR)/mameui.res" ,  GEN_DIR  .. "/resource/" .. "arcadevers.rc", true  },
	}
end

newoption {
	trigger = "DIRECTINPUT",
	description = "Minimum DirectInput version to support",
	allowed = {
		{ "7",  "Support DirectInput 7 or later"  },
		{ "8",  "Support DirectInput 8 or later"  },
	},
}

if not _OPTIONS["DIRECTINPUT"] then
	_OPTIONS["DIRECTINPUT"] = "8"
end

newoption {
	trigger = "USE_SDL",
	description = "Enable SDL sound output",
	allowed = {
		{ "0",  "Disable SDL sound output"  },
		{ "1",  "Enable SDL sound output"   },
	},
}

if not _OPTIONS["USE_SDL"] then
	_OPTIONS["USE_SDL"] = "0"
end

newoption {
	trigger = "CYGWIN_BUILD",
	description = "Build with Cygwin tools",
	allowed = {
		{ "0",  "Build with MinGW tools"   },
		{ "1",  "Build with Cygwin tools"  },
	},
}

if not _OPTIONS["CYGWIN_BUILD"] then
	_OPTIONS["CYGWIN_BUILD"] = "0"
end


if _OPTIONS["CYGWIN_BUILD"] == "1" then
	buildoptions {
		"-mmo-cygwin",
	}
	linkoptions {
		"-mno-cygwin",
	}
end


project ("osd_" .. _OPTIONS["osd"])
	uuid (os.uuid("osd_" .. _OPTIONS["osd"]))
	kind "StaticLib"

	dofile("winui_cfg.lua")
	osdmodulesbuild()

	defines {
		"DIRECT3D_VERSION=0x0900",
	}

	if _OPTIONS["DIRECTINPUT"] == "8" then
		defines {
			"DIRECTINPUT_VERSION=0x0800",
		}
	else
		defines {
			"DIRECTINPUT_VERSION=0x0700",
		}
	end

	includedirs {
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "src/osd/modules/render",
		MAME_DIR .. "3rdparty",
	}

	includedirs {
		MAME_DIR .. "src/osd/windows",
	}

	files {
		MAME_DIR .. "src/osd/modules/render/drawd3d.c",
		MAME_DIR .. "src/osd/modules/render/d3d/d3d9intf.c",
		MAME_DIR .. "src/osd/modules/render/d3d/d3dhlsl.c",
		MAME_DIR .. "src/osd/modules/render/drawdd.c",
		MAME_DIR .. "src/osd/modules/render/drawgdi.c",
		MAME_DIR .. "src/osd/modules/render/drawnone.c",
		MAME_DIR .. "src/osd/windows/input.c",
		MAME_DIR .. "src/osd/windows/output.c",
		MAME_DIR .. "src/osd/windows/video.c",
		MAME_DIR .. "src/osd/windows/window.c",
		MAME_DIR .. "src/osd/windows/winmenu.c",
		MAME_DIR .. "src/osd/windows/winmain.c",
		MAME_DIR .. "src/osd/modules/debugger/win/consolewininfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/debugbaseinfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/debugviewinfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/debugwininfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/disasmbasewininfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/disasmviewinfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/disasmwininfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/editwininfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/logwininfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/memoryviewinfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/memorywininfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/pointswininfo.c",
		MAME_DIR .. "src/osd/modules/debugger/win/uimetrics.c",
		MAME_DIR .. "src/osd/winui/win_options.c",
		MAME_DIR .. "src/osd/winui/mui_util.c",
		MAME_DIR .. "src/osd/winui/directinput.c",
		MAME_DIR .. "src/osd/winui/dijoystick.c",
		MAME_DIR .. "src/osd/winui/directories.c",
		MAME_DIR .. "src/osd/winui/mui_audit.c",
		MAME_DIR .. "src/osd/winui/columnedit.c",
		MAME_DIR .. "src/osd/winui/screenshot.c",
		MAME_DIR .. "src/osd/winui/treeview.c",
		MAME_DIR .. "src/osd/winui/splitters.c",
		MAME_DIR .. "src/osd/winui/bitmask.c",
		MAME_DIR .. "src/osd/winui/datamap.c",
		MAME_DIR .. "src/osd/winui/dxdecode.c",
		MAME_DIR .. "src/osd/winui/picker.c",
		MAME_DIR .. "src/osd/winui/properties.c",
		MAME_DIR .. "src/osd/winui/tabview.c",
		MAME_DIR .. "src/osd/winui/history.c",
		MAME_DIR .. "src/osd/winui/dialogs.c",
		MAME_DIR .. "src/osd/winui/mui_opts.c",
		MAME_DIR .. "src/osd/winui/layout.c",
		MAME_DIR .. "src/osd/winui/datafile.c",
		MAME_DIR .. "src/osd/winui/winui.c",
		MAME_DIR .. "src/osd/winui/mui_main.c",
	}


project ("ocore_" .. _OPTIONS["osd"])
	uuid (os.uuid("ocore_" .. _OPTIONS["osd"]))
	kind "StaticLib"

	options {
		"ForceCPP",
	}
	removeflags {
		"SingleOutputDir",
	}

	dofile("winui_cfg.lua")

	includedirs {
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
	}

	BASE_TARGETOS = "win32"
	SDLOS_TARGETOS = "win32"
	SYNC_IMPLEMENTATION = "windows"

	includedirs {
		MAME_DIR .. "src/osd/windows",
		MAME_DIR .. "src/lib/winpcap",
	}

	files {
		MAME_DIR .. "src/osd/osdcore.c",
		MAME_DIR .. "src/osd/strconv.c",
		MAME_DIR .. "src/osd/windows/windir.c",
		MAME_DIR .. "src/osd/windows/winfile.c",
		MAME_DIR .. "src/osd/modules/sync/sync_windows.c",
		MAME_DIR .. "src/osd/windows/winutf8.c",
		MAME_DIR .. "src/osd/windows/winutil.c",
		MAME_DIR .. "src/osd/windows/winclip.c",
		MAME_DIR .. "src/osd/windows/winsocket.c",
		MAME_DIR .. "src/osd/windows/winptty.c",
		MAME_DIR .. "src/osd/modules/osdmodule.c",
		MAME_DIR .. "src/osd/modules/lib/osdlib_win32.c",
	}

	if _OPTIONS["NOASM"] == "1" then
		files {
			MAME_DIR .. "src/osd/modules/sync/work_mini.c",
		}
	else
		files {
			MAME_DIR .. "src/osd/modules/sync/work_osd.c",
		}
	end
