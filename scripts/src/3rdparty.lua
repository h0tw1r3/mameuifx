--------------------------------------------------
-- expat library objects
--------------------------------------------------

project "expat"
	uuid "f4cd40b1-c37c-452d-9785-640f26f0bf54"
	kind "StaticLib"

	options {
		"ForceCPP",
	}

	files {
		MAME_DIR .. "3rdparty/expat/lib/xmlparse.c",
		MAME_DIR .. "3rdparty/expat/lib/xmlrole.c",
		MAME_DIR .. "3rdparty/expat/lib/xmltok.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- zlib library objects
--------------------------------------------------

project "zlib"
	uuid "3d78bd2a-2bd0-4449-8087-42ddfaef7ec9"
	kind "StaticLib"

	configuration "Debug"
		defines {
			"verbose=-1",
		}

	configuration { "gmake" }
		buildoptions_c {
			"-Wno-strict-prototypes",
		}

	configuration { }
		defines {
			"ZLIB_CONST",
		}

	files {
		MAME_DIR .. "3rdparty/zlib/adler32.c",
		MAME_DIR .. "3rdparty/zlib/compress.c",
		MAME_DIR .. "3rdparty/zlib/crc32.c",
		MAME_DIR .. "3rdparty/zlib/deflate.c",
		MAME_DIR .. "3rdparty/zlib/inffast.c",
		MAME_DIR .. "3rdparty/zlib/inflate.c",
		MAME_DIR .. "3rdparty/zlib/infback.c",
		MAME_DIR .. "3rdparty/zlib/inftrees.c",
		MAME_DIR .. "3rdparty/zlib/trees.c",
		MAME_DIR .. "3rdparty/zlib/uncompr.c",
		MAME_DIR .. "3rdparty/zlib/zutil.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- SoftFloat library objects
--------------------------------------------------
	
project "softfloat"
	uuid "04fbf89e-4761-4cf2-8a12-64500cf0c5c5"
	kind "StaticLib"

	options {
		"ForceCPP",
	}

	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "3rdparty",
		MAME_DIR .. "3rdparty/expat/lib/",
	}
	
	files {
		MAME_DIR .. "3rdparty/softfloat/softfloat.c",
		MAME_DIR .. "3rdparty/softfloat/fsincos.c",
		MAME_DIR .. "3rdparty/softfloat/fyl2x.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- libJPEG library objects
--------------------------------------------------

project "jpeg"
	uuid "447c6800-dcfd-4c48-b72a-a8223bb409ca"
	kind "StaticLib"

	files {
		MAME_DIR .. "3rdparty/libjpeg/jaricom.c",
		MAME_DIR .. "3rdparty/libjpeg/jcapimin.c",
		MAME_DIR .. "3rdparty/libjpeg/jcapistd.c",
		MAME_DIR .. "3rdparty/libjpeg/jcarith.c",
		MAME_DIR .. "3rdparty/libjpeg/jccoefct.c",
		MAME_DIR .. "3rdparty/libjpeg/jccolor.c",
		MAME_DIR .. "3rdparty/libjpeg/jcdctmgr.c",
		MAME_DIR .. "3rdparty/libjpeg/jchuff.c",
		MAME_DIR .. "3rdparty/libjpeg/jcinit.c",
		MAME_DIR .. "3rdparty/libjpeg/jcmainct.c",
		MAME_DIR .. "3rdparty/libjpeg/jcmarker.c",
		MAME_DIR .. "3rdparty/libjpeg/jcmaster.c",
		MAME_DIR .. "3rdparty/libjpeg/jcomapi.c",
		MAME_DIR .. "3rdparty/libjpeg/jcparam.c",
		MAME_DIR .. "3rdparty/libjpeg/jcprepct.c",
		MAME_DIR .. "3rdparty/libjpeg/jcsample.c",
		MAME_DIR .. "3rdparty/libjpeg/jctrans.c",
		MAME_DIR .. "3rdparty/libjpeg/jdapimin.c",
		MAME_DIR .. "3rdparty/libjpeg/jdapistd.c",
		MAME_DIR .. "3rdparty/libjpeg/jdarith.c",
		MAME_DIR .. "3rdparty/libjpeg/jdatadst.c",
		MAME_DIR .. "3rdparty/libjpeg/jdatasrc.c",
		MAME_DIR .. "3rdparty/libjpeg/jdcoefct.c",
		MAME_DIR .. "3rdparty/libjpeg/jdcolor.c",
		MAME_DIR .. "3rdparty/libjpeg/jddctmgr.c",
		MAME_DIR .. "3rdparty/libjpeg/jdhuff.c",
		MAME_DIR .. "3rdparty/libjpeg/jdinput.c",
		MAME_DIR .. "3rdparty/libjpeg/jdmainct.c",
		MAME_DIR .. "3rdparty/libjpeg/jdmarker.c",
		MAME_DIR .. "3rdparty/libjpeg/jdmaster.c",
		MAME_DIR .. "3rdparty/libjpeg/jdmerge.c",
		MAME_DIR .. "3rdparty/libjpeg/jdpostct.c",
		MAME_DIR .. "3rdparty/libjpeg/jdsample.c",
		MAME_DIR .. "3rdparty/libjpeg/jdtrans.c",
		MAME_DIR .. "3rdparty/libjpeg/jerror.c",
		MAME_DIR .. "3rdparty/libjpeg/jfdctflt.c",
		MAME_DIR .. "3rdparty/libjpeg/jfdctfst.c",
		MAME_DIR .. "3rdparty/libjpeg/jfdctint.c",
		MAME_DIR .. "3rdparty/libjpeg/jidctflt.c",
		MAME_DIR .. "3rdparty/libjpeg/jidctfst.c",
		MAME_DIR .. "3rdparty/libjpeg/jidctint.c",
		MAME_DIR .. "3rdparty/libjpeg/jquant1.c",
		MAME_DIR .. "3rdparty/libjpeg/jquant2.c",
		MAME_DIR .. "3rdparty/libjpeg/jutils.c",
		MAME_DIR .. "3rdparty/libjpeg/jmemmgr.c",
		MAME_DIR .. "3rdparty/libjpeg/jmemansi.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- libflac library objects
--------------------------------------------------

project "flac"
	uuid "b6fc19e8-073a-4541-bb7b-d24b548d424a"
	kind "StaticLib"

	configuration { }
		defines {
			"WORDS_BIGENDIAN=0",
			"FLAC__NO_ASM",
			"_LARGEFILE_SOURCE",
			"_FILE_OFFSET_BITS=64",
			"FLAC__HAS_OGG=0",
			"HAVE_CONFIG_H=1",
		}

	configuration { "gmake"}
		buildoptions_c {
			"-Wno-unused-function",
			"-O0",
		}

	configuration { }

	includedirs {
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/include",
		MAME_DIR .. "3rdparty/libflac/include",
	}

	files {
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/bitmath.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/bitreader.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/bitwriter.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/cpu.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/crc.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/fixed.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/float.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/format.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/lpc.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/md5.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/memory.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/stream_decoder.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/stream_encoder.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/stream_encoder_framing.c",
		MAME_DIR .. "3rdparty/libflac/src/libFLAC/window.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- lib7z library objects
--------------------------------------------------

project "7z"
	uuid "ad573d62-e76a-4b11-ae34-5110a6789a42"
	kind "StaticLib"

	configuration { }
		defines {
			"_7ZIP_PPMD_SUPPPORT",
			"_7ZIP_ST",
		}

	files {
			MAME_DIR .. "3rdparty/lzma/C/7zBuf.c",
			MAME_DIR .. "3rdparty/lzma/C/7zBuf2.c",
			MAME_DIR .. "3rdparty/lzma/C/7zCrc.c",
			MAME_DIR .. "3rdparty/lzma/C/7zCrcOpt.c",
			MAME_DIR .. "3rdparty/lzma/C/7zDec.c",
			MAME_DIR .. "3rdparty/lzma/C/7zIn.c",
			MAME_DIR .. "3rdparty/lzma/C/CpuArch.c",
			MAME_DIR .. "3rdparty/lzma/C/LzmaDec.c",
			MAME_DIR .. "3rdparty/lzma/C/Lzma2Dec.c",
			MAME_DIR .. "3rdparty/lzma/C/LzmaEnc.c",
			MAME_DIR .. "3rdparty/lzma/C/Lzma2Enc.c",
			MAME_DIR .. "3rdparty/lzma/C/LzFind.c",
			MAME_DIR .. "3rdparty/lzma/C/Bra.c",
			MAME_DIR .. "3rdparty/lzma/C/Bra86.c",
			MAME_DIR .. "3rdparty/lzma/C/Bcj2.c",
			MAME_DIR .. "3rdparty/lzma/C/Ppmd7.c",
			MAME_DIR .. "3rdparty/lzma/C/Ppmd7Dec.c",
			MAME_DIR .. "3rdparty/lzma/C/7zStream.c",
		}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- LUA library objects
--------------------------------------------------

project "lua"
	uuid "d9e2eed1-f1ab-4737-a6ac-863700b1a5a9"
	kind "StaticLib"

	configuration { }
		defines {
			"LUA_COMPAT_ALL",
		}
	if not (_OPTIONS["targetos"]=="windows") then
		defines {
			"LUA_USE_POSIX",
		}
	end
	if ("pnacl" == _OPTIONS["gcc"]) then
		defines {
			"LUA_32BITS",
		}
	end
	
	configuration { }

	includedirs {
		MAME_DIR .. "3rdparty",
	}

	files {
		MAME_DIR .. "3rdparty/lua/src/lapi.c",
		MAME_DIR .. "3rdparty/lua/src/lcode.c",
		MAME_DIR .. "3rdparty/lua/src/lctype.c",
		MAME_DIR .. "3rdparty/lua/src/ldebug.c",
		MAME_DIR .. "3rdparty/lua/src/ldo.c",
		MAME_DIR .. "3rdparty/lua/src/ldump.c",
		MAME_DIR .. "3rdparty/lua/src/lfunc.c",
		MAME_DIR .. "3rdparty/lua/src/lgc.c",
		MAME_DIR .. "3rdparty/lua/src/llex.c",
		MAME_DIR .. "3rdparty/lua/src/lmem.c",
		MAME_DIR .. "3rdparty/lua/src/lobject.c",
		MAME_DIR .. "3rdparty/lua/src/lopcodes.c",
		MAME_DIR .. "3rdparty/lua/src/lparser.c",
		MAME_DIR .. "3rdparty/lua/src/lstate.c",
		MAME_DIR .. "3rdparty/lua/src/lstring.c",
		MAME_DIR .. "3rdparty/lua/src/ltable.c",
		MAME_DIR .. "3rdparty/lua/src/ltm.c",
		MAME_DIR .. "3rdparty/lua/src/lundump.c",
		MAME_DIR .. "3rdparty/lua/src/lvm.c",
		MAME_DIR .. "3rdparty/lua/src/lzio.c",
		MAME_DIR .. "3rdparty/lua/src/lauxlib.c",
		MAME_DIR .. "3rdparty/lua/src/lbaselib.c",
		MAME_DIR .. "3rdparty/lua/src/lbitlib.c",
		MAME_DIR .. "3rdparty/lua/src/lcorolib.c",
		MAME_DIR .. "3rdparty/lua/src/ldblib.c",
		MAME_DIR .. "3rdparty/lua/src/liolib.c",
		MAME_DIR .. "3rdparty/lua/src/lmathlib.c",
		MAME_DIR .. "3rdparty/lua/src/loslib.c",
		MAME_DIR .. "3rdparty/lua/src/lstrlib.c",
		MAME_DIR .. "3rdparty/lua/src/ltablib.c",
		MAME_DIR .. "3rdparty/lua/src/loadlib.c",
		MAME_DIR .. "3rdparty/lua/src/linit.c",
		MAME_DIR .. "3rdparty/lua/src/lutf8lib.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- sqlite3 lua library objects
--------------------------------------------------
	
project "lsqlite3"
	uuid "1d84edab-94cf-48fb-83ee-b75bc697660e"
	kind "StaticLib"

	configuration { }
		defines {
			"LUA_COMPAT_ALL",
		}

	includedirs {
		MAME_DIR .. "3rdparty",
		MAME_DIR .. "3rdparty/lua/src",
	}

	files {
		MAME_DIR .. "3rdparty/lsqlite3/lsqlite3.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- mongoose library objects
--------------------------------------------------

project "mongoose"
	uuid "ff05b529-2b6f-4166-9dff-5fe2aef89c40"
	kind "StaticLib"

	options {
		"ForceCPP",
	}
	defines {
		"MONGOOSE_ENABLE_THREADS",
		"NS_STACK_SIZE=0"
	}

	includedirs {
		MAME_DIR .. "3rdparty/mongoose",
	}

	files {
		MAME_DIR .. "3rdparty/mongoose/mongoose.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- jsoncpp library objects
--------------------------------------------------

project "jsoncpp"
	uuid "ae023ff3-d712-4e54-adc5-3b56a148650f"
	kind "StaticLib"

	options {
		"ForceCPP",
	}

	includedirs {
		MAME_DIR .. "3rdparty/jsoncpp/include",
	}

	files {
		MAME_DIR .. "3rdparty/jsoncpp/src/lib_json/json_reader.cpp",
		MAME_DIR .. "3rdparty/jsoncpp/src/lib_json/json_value.cpp",
		MAME_DIR .. "3rdparty/jsoncpp/src/lib_json/json_writer.cpp",
		
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- SQLite3 library objects
--------------------------------------------------

project "sqllite3"
	uuid "5cb3d495-57ed-461c-81e5-80dc0857517d"
	kind "StaticLib"

	configuration { "gmake" }
		buildoptions_c {
			"-Wno-bad-function-cast",
			"-Wno-undef",
		}

	configuration { }

	files {
		MAME_DIR .. "3rdparty/sqlite3/sqlite3.c",
	}
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end

--------------------------------------------------
-- portmidi library objects
--------------------------------------------------
if _OPTIONS["NO_USE_MIDI"]=="0" then
project "portmidi"
	uuid "587f2da6-3274-4a65-86a2-f13ea315bb98"
	kind "StaticLib"

	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "3rdparty/portmidi/pm_common",
		MAME_DIR .. "3rdparty/portmidi/porttime",
	}
		
	configuration { "linux*" }
		defines {
			"PMALSA=1",
		}

	configuration { }

	files {
		MAME_DIR .. "3rdparty/portmidi/pm_common/portmidi.c",
		MAME_DIR .. "3rdparty/portmidi/pm_common/pmutil.c",
	}

	if _OPTIONS["targetos"]=="windows" or _OPTIONS["targetos"]=="winui" then
		files {
			MAME_DIR .. "3rdparty/portmidi/porttime/ptwinmm.c",
			MAME_DIR .. "3rdparty/portmidi/pm_win/pmwin.c",
			MAME_DIR .. "3rdparty/portmidi/pm_win/pmwinmm.c",
			MAME_DIR .. "3rdparty/portmidi/porttime/ptwinmm.c",
		}
	end

	if _OPTIONS["targetos"]=="linux" then
		files {
			MAME_DIR .. "3rdparty/portmidi/pm_linux/pmlinux.c",
			MAME_DIR .. "3rdparty/portmidi/pm_linux/pmlinuxalsa.c",
			MAME_DIR .. "3rdparty/portmidi/pm_linux/finddefault.c",
			MAME_DIR .. "3rdparty/portmidi/porttime/ptlinux.c",
		}
	end
	if _OPTIONS["targetos"]=="macosx" then
		files {
			MAME_DIR .. "3rdparty/portmidi/pm_mac/pmmac.c",
			MAME_DIR .. "3rdparty/portmidi/pm_mac/pmmacosxcm.c",
			MAME_DIR .. "3rdparty/portmidi/pm_mac/finddefault.c",
			MAME_DIR .. "3rdparty/portmidi/pm_mac/readbinaryplist.c",
			MAME_DIR .. "3rdparty/portmidi/pm_mac/osxsupport.m",
			MAME_DIR .. "3rdparty/portmidi/porttime/ptmacosx_mach.c",
		}
	end
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end
end	
--------------------------------------------------
-- BGFX library objects
--------------------------------------------------
	
if (USE_BGFX == 1) then
project "bgfx"
	uuid "d3e7e119-35cf-4f4f-aba0-d3bdcd1b879a"
	kind "StaticLib"

	includedirs {		
		MAME_DIR .. "3rdparty/bgfx/include",
		MAME_DIR .. "3rdparty/bgfx/3rdparty",
		MAME_DIR .. "3rdparty/bx/include",
		MAME_DIR .. "3rdparty/bgfx/3rdparty/khronos",
	}

	configuration { "vs*" }
		includedirs {
			MAME_DIR .. "3rdparty/dxsdk/Include",
			MAME_DIR .. "3rdparty/bx/include/compat/msvc",
		}
	configuration { "mingw*" }
		includedirs {
			MAME_DIR .. "3rdparty/bx/include/compat/mingw",
		}

	configuration { "osx*" }
		includedirs {
			MAME_DIR .. "3rdparty/bx/include/compat/osx",
		}
		
	configuration { "freebsd" }
		includedirs {
			MAME_DIR .. "3rdparty/bx/include/compat/freebsd",
		}

	configuration { "gmake" }
		buildoptions {		
			"-Wno-uninitialized",
		}
			
	configuration { }

	defines {
		"__STDC_LIMIT_MACROS",
		"__STDC_FORMAT_MACROS",
		"__STDC_CONSTANT_MACROS",
	}
	files {
		MAME_DIR .. "3rdparty/bgfx/src/bgfx.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/glcontext_egl.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/glcontext_glx.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/glcontext_ppapi.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/glcontext_wgl.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/image.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/ovr.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/renderer_d3d12.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/renderer_d3d11.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/renderer_d3d9.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/renderer_gl.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/renderer_null.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/renderer_vk.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/renderdoc.cpp",
		MAME_DIR .. "3rdparty/bgfx/src/vertexdecl.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/bgfx_utils.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/bounds.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/camera.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/cube_atlas.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/font/font_manager.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/font/text_buffer_manager.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/font/text_metrics.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/font/utf8.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/imgui/imgui.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/nanovg/nanovg.cpp",
		MAME_DIR .. "3rdparty/bgfx/examples/common/nanovg/nanovg_bgfx.cpp",
	}
	if _OPTIONS["targetos"]=="macosx" then
		files {
			MAME_DIR .. "3rdparty/bgfx/src/glcontext_eagl.mm",
			MAME_DIR .. "3rdparty/bgfx/src/glcontext_nsgl.mm",
			
		}
	end
	if (_OPTIONS["SHADOW_CHECK"]=="1") then
		removebuildoptions {
			"-Wshadow"
		}
	end
end
