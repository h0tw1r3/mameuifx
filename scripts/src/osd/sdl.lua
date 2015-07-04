-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

dofile("modules.lua")


function maintargetosdoptions(_target,_subtarget)
	osdmodulestargetconf()

	if _OPTIONS["USE_DISPATCH_GL"]~="1" and _OPTIONS["MESA_INSTALL_ROOT"] then
		libdirs {
			path.join(_OPTIONS["MESA_INSTALL_ROOT"],"lib"),
		}
		linkoptions {
			"-Wl,-rpath=" .. path.join(_OPTIONS["MESA_INSTALL_ROOT"],"lib"),
		}
	end

	if _OPTIONS["NO_X11"]~="1" then
		links {
			"X11",
			"Xinerama",
		}
	end

	if _OPTIONS["NO_USE_XINPUT"]~="1" then
		links {
			"Xext",
			"Xi",
		}
	end

	if BASE_TARGETOS=="unix" and _OPTIONS["targetos"]~="macosx" then
		if _OPTIONS["SDL_LIBVER"]=="sdl2" then
			links {
				"SDL2_ttf",
			}
		else
			links {
				"SDL_ttf",
			}
		end
		local str = backtick("pkg-config --libs fontconfig")
		addlibfromstring(str)
		addoptionsfromstring(str)
	end

	if _OPTIONS["targetos"]=="windows" then
		if _OPTIONS["SDL_LIBVER"]=="sdl2" then
			links {
				"SDL2.dll",
			}
		else
			links {
				"SDL.dll",
			}
		end

		configuration { "mingw*-gcc" }
			linkoptions{
				"-municode",
			}
		configuration { "vs*" }
			flags {
				"Unicode",
			}
		configuration { "x32", "vs*" }
			libdirs {
				path.join(_OPTIONS["SDL_INSTALL_ROOT"],"lib","x86")
			}
		configuration { "x64", "vs*" }
			libdirs {
				path.join(_OPTIONS["SDL_INSTALL_ROOT"],"lib","x64")
			}
		configuration {}
	elseif _OPTIONS["targetos"]=="haiku" then
		links {
			"network",
			"bsd",
		}
	end

	configuration { "mingw*" or "vs*" }
		targetprefix "sdl"

	configuration { }
end


function sdlconfigcmd()
	if not _OPTIONS["SDL_INSTALL_ROOT"] then
		return _OPTIONS["SDL_LIBVER"] .. "-config"
	else
		return path.join(_OPTIONS["SDL_INSTALL_ROOT"],"bin",_OPTIONS["SDL_LIBVER"]) .. "-config"
	end
end


newoption {
	trigger = "MESA_INSTALL_ROOT",
	description = "link against specific GL-Library - also adds rpath to executable (overridden by USE_DISPATCH_GL)",
}

newoption {
    trigger = "SDL_INI_PATH",
    description = "Default search path for .ini files",
}

newoption {
	trigger = "NO_X11",
	description = "Disable use of X11",
	allowed = {
		{ "0",  "Enable X11"  },
		{ "1",  "Disable X11" },
	},
}

if not _OPTIONS["NO_X11"] then
	if _OPTIONS["targetos"]=="windows" or _OPTIONS["targetos"]=="macosx" or _OPTIONS["targetos"]=="haiku" or _OPTIONS["targetos"]=="asmjs" or _OPTIONS["targetos"]=="os2" then
		_OPTIONS["NO_X11"] = "1"
	else
		_OPTIONS["NO_X11"] = "0"
	end
end

newoption {
	trigger = "NO_USE_XINPUT",
	description = "Disable use of Xinput",
	allowed = {
		{ "0",  "Enable Xinput"  },
		{ "1",  "Disable Xinput" },
	},
}

if not _OPTIONS["NO_USE_XINPUT"] then
	_OPTIONS["NO_USE_XINPUT"] = "1"
end

newoption {
	trigger = "SDL_LIBVER",
	description = "Choose SDL version",
	allowed = {
		{ "sdl",   "SDL"   },
		{ "sdl2",  "SDL 2" },
	},
}

if not _OPTIONS["SDL_LIBVER"] then
	if _OPTIONS["targetos"]=="os2" then
		_OPTIONS["SDL_LIBVER"] = "sdl"
	else
		_OPTIONS["SDL_LIBVER"] = "sdl2"
	end
end

newoption {
	trigger = "SDL2_MULTIAPI",
	description = "Use couriersud's multi-keyboard patch for SDL 2.1? (this API was removed prior to the 2.0 release)",
	allowed = {
		{ "0",  "Use single-keyboard API"  },
		{ "1",  "Use multi-keyboard API"   },
	},
}

if not _OPTIONS["SDL2_MULTIAPI"] then
	_OPTIONS["SDL2_MULTIAPI"] = "0"
end

newoption {
	trigger = "SDL_INSTALL_ROOT",
	description = "Equivalent to the ./configure --prefix=<path>",
}

newoption {
	trigger = "SDL_FRAMEWORK_PATH",
	description = "Location of SDL framework for custom OS X installations",
}

if not _OPTIONS["SDL_FRAMEWORK_PATH"] then
	_OPTIONS["SDL_FRAMEWORK_PATH"] = "/Library/Frameworks/"
end

newoption {
	trigger = "MACOSX_USE_LIBSDL",
	description = "Use SDL library on OS (rather than framework)",
	allowed = {
		{ "0",  "Use framework"  },
		{ "1",  "Use library" },
	},
}

if not _OPTIONS["MACOSX_USE_LIBSDL"] then
	_OPTIONS["MACOSX_USE_LIBSDL"] = "0"
end


BASE_TARGETOS       = "unix"
SDLOS_TARGETOS      = "unix"
SYNC_IMPLEMENTATION = "tc"
SDL_NETWORK         = ""
if _OPTIONS["targetos"]=="linux" then
	SDL_NETWORK         = "taptun"
elseif _OPTIONS["targetos"]=="openbsd" then
	SYNC_IMPLEMENTATION = "ntc"
elseif _OPTIONS["targetos"]=="netbsd" then
	SYNC_IMPLEMENTATION = "ntc"
	SDL_NETWORK         = "pcap"
elseif _OPTIONS["targetos"]=="haiku" then
	SYNC_IMPLEMENTATION = "ntc"
elseif _OPTIONS["targetos"]=="asmjs" then
	SYNC_IMPLEMENTATION = "mini"
elseif _OPTIONS["targetos"]=="windows" then
	BASE_TARGETOS       = "win32"
	SDLOS_TARGETOS      = "win32"
	SYNC_IMPLEMENTATION = "windows"
	SDL_NETWORK         = "pcap"
elseif _OPTIONS["targetos"]=="macosx" then
	SDLOS_TARGETOS      = "macosx"
	SYNC_IMPLEMENTATION = "ntc"
	SDL_NETWORK         = "pcap"
elseif _OPTIONS["targetos"]=="os2" then
	BASE_TARGETOS       = "os2"
	SDLOS_TARGETOS      = "os2"
	SYNC_IMPLEMENTATION = "os2"
end

if _OPTIONS["SDL_LIBVER"]=="sdl" then
	USE_BGFX = 0
end

if BASE_TARGETOS=="unix" then
	if _OPTIONS["targetos"]=="macosx" then
		links {
			"Cocoa.framework",
		}
		if _OPTIONS["MACOSX_USE_LIBSDL"]~="1" then
			linkoptions {
				"-F" .. _OPTIONS["SDL_FRAMEWORK_PATH"],
			}
			if _OPTIONS["SDL_LIBVER"]=="sdl2" then
				links {
					"SDL2.framework",
				}
			else
				links {
					"SDL.framework",
				}
			end
		else
			local str = backtick(sdlconfigcmd() .. " --libs | sed 's/-lSDLmain//'")
			addlibfromstring(str)
			addoptionsfromstring(str)
		end
	else
		if _OPTIONS["NO_X11"]=="1" then
			_OPTIONS["USE_QTDEBUG"] = "0"
			USE_BGFX = 0
		else
			libdirs {
				"/usr/X11/lib",
				"/usr/X11R6/lib",
				"/usr/openwin/lib",
			}
			if _OPTIONS["SDL_LIBVER"]=="sdl" then
				links {
					"X11",
				}
			end
		end
		local str = backtick(sdlconfigcmd() .. " --libs")
		addlibfromstring(str)
		addoptionsfromstring(str)
		if _OPTIONS["targetos"]~="haiku" then
			links {
				"m",
				"pthread",
			}
			if _OPTIONS["targetos"]=="solaris" then
				links {
					"socket",
					"nsl",
				}
			else
				links {
					"util",
				}
			end
		end
	end
elseif BASE_TARGETOS=="os2" then
	local str = backtick(sdlconfigcmd() .. " --libs")
	addlibfromstring(str)
	addoptionsfromstring(str)
	links {
		"pthread"
	}
end


project ("osd_" .. _OPTIONS["osd"])
	targetsubdir(_OPTIONS["target"] .."_" .._OPTIONS["subtarget"])
	uuid (os.uuid("osd_" .. _OPTIONS["osd"]))
	kind (LIBTYPE)

	dofile("sdl_cfg.lua")
	osdmodulesbuild()

	includedirs {
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "src/osd/modules/render",
		MAME_DIR .. "3rdparty",
		MAME_DIR .. "src/osd/sdl",
	}

	if _OPTIONS["targetos"]=="windows" then
		files {
			MAME_DIR .. "src/osd/sdl/main.c",
		}
	end

	if _OPTIONS["targetos"]=="macosx" then
		files {
			MAME_DIR .. "src/osd/modules/debugger/debugosx.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/breakpointsview.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/consoleview.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/debugcommandhistory.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/debugconsole.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/debugview.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/debugwindowhandler.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/deviceinfoviewer.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/devicesviewer.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/disassemblyview.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/disassemblyviewer.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/errorlogview.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/errorlogviewer.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/memoryview.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/memoryviewer.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/pointsviewer.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/registersview.m",
			MAME_DIR .. "src/osd/modules/debugger/osx/watchpointsview.m",
		}
		if _OPTIONS["SDL_LIBVER"]=="sdl" then
			-- SDLMain_tmpl isn't necessary for SDL2
			files {
				MAME_DIR .. "src/osd/sdl/SDLMain_tmpl.m",
			}
		end
	end

	files {
		MAME_DIR .. "src/osd/sdl/sdlmain.c",
		MAME_DIR .. "src/osd/sdl/input.c",
		MAME_DIR .. "src/osd/sdl/video.c",
		MAME_DIR .. "src/osd/sdl/window.c",
		MAME_DIR .. "src/osd/sdl/output.c",
		MAME_DIR .. "src/osd/sdl/watchdog.c",
		MAME_DIR .. "src/osd/modules/render/drawsdl.c",
	}
	if _OPTIONS["SDL_LIBVER"]=="sdl2" then
		files {
			MAME_DIR .. "src/osd/modules/render/draw13.c",
		}
	end


project ("ocore_" .. _OPTIONS["osd"])
	targetsubdir(_OPTIONS["target"] .."_" .. _OPTIONS["subtarget"])
	uuid (os.uuid("ocore_" .. _OPTIONS["osd"]))
	kind (LIBTYPE)

	options {
		"ForceCPP",
	}

	removeflags {
		"SingleOutputDir",
	}

	dofile("sdl_cfg.lua")

	includedirs {
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "src/osd/sdl",
	}

	files {
		MAME_DIR .. "src/osd/osdcore.c",
		MAME_DIR .. "src/osd/strconv.c",
		MAME_DIR .. "src/osd/sdl/sdldir.c",
		MAME_DIR .. "src/osd/sdl/sdlfile.c",
		MAME_DIR .. "src/osd/sdl/sdlptty_" .. BASE_TARGETOS ..".c",
		MAME_DIR .. "src/osd/sdl/sdlsocket.c",
		MAME_DIR .. "src/osd/sdl/sdlos_" .. SDLOS_TARGETOS .. ".c",
		MAME_DIR .. "src/osd/modules/osdmodule.c",
		MAME_DIR .. "src/osd/modules/lib/osdlib_" .. SDLOS_TARGETOS .. ".c",
		MAME_DIR .. "src/osd/modules/sync/sync_" .. SYNC_IMPLEMENTATION .. ".c",
	}

	if _OPTIONS["NOASM"]=="1" then
		files {
			MAME_DIR .. "src/osd/modules/sync/work_mini.c",
		}
	else
		files {
			MAME_DIR .. "src/osd/modules/sync/work_osd.c",
		}
	end

	if _OPTIONS["targetos"]=="macosx" then
		files {
			MAME_DIR .. "src/osd/sdl/osxutils.m",
		}
	end


--------------------------------------------------
-- testkeys
--------------------------------------------------

if _OPTIONS["with-tools"] then
	project("testkeys")
		uuid ("744cec21-c3b6-4d69-93cb-6811fed0ffe3")
		kind "ConsoleApp"

		options {
			"ForceCPP",
		}

		flags {
			"Symbols", -- always include minimum symbols for executables 	
		}

		dofile("sdl_cfg.lua")

		includedirs {
			MAME_DIR .. "src/osd",
			MAME_DIR .. "src/lib/util",
		}

		if _OPTIONS["SEPARATE_BIN"]~="1" then 
			targetdir(MAME_DIR)
		end

		links {
			"utils",
			"ocore_" .. _OPTIONS["osd"],
		}

		files {
			MAME_DIR .. "src/osd/sdl/testkeys.c",
		}

		if _OPTIONS["targetos"] == "windows" then
			if _OPTIONS["SDL_LIBVER"] == "sdl2" then
				links {
					"SDL2.dll",
				}
			else
				links {
					"SDL.dll",
				}
			end
			linkoptions{
				"-municode",
			}
			files {
				MAME_DIR .. "src/osd/sdl/main.c",
			}
		elseif _OPTIONS["targetos"] == "macosx" and _OPTIONS["SDL_LIBVER"] == "sdl" then
			-- SDLMain_tmpl isn't necessary for SDL2
			files {
				MAME_DIR .. "src/osd/sdl/SDLMain_tmpl.m",
			}
		end
end


--------------------------------------------------
-- aueffectutil
--------------------------------------------------

if _OPTIONS["targetos"] == "macosx" and _OPTIONS["with-tools"] then
	project("aueffectutil")
		uuid ("3db8316d-fad7-4f5b-b46a-99373c91550e")
		kind "ConsoleApp"

		options {
			"ForceCPP",
		}

		flags {
			"Symbols", -- always include minimum symbols for executables 	
		}

		dofile("sdl_cfg.lua")

		if _OPTIONS["SEPARATE_BIN"]~="1" then 
			targetdir(MAME_DIR)
		end

		linkoptions {
			"-sectcreate __TEXT __info_plist " .. MAME_DIR .. "src/osd/sdl/aueffectutil-Info.plist",
		}

		dependency {
			{ "aueffectutil",  MAME_DIR .. "src/osd/sdl/aueffectutil-Info.plist", true  },
		}

		links {
			"AudioUnit.framework",
			"AudioToolbox.framework",
			"CoreAudio.framework",
			"CoreAudioKit.framework",
			"CoreServices.framework",
		}

		files {
			MAME_DIR .. "src/osd/sdl/aueffectutil.m",
		}
end
