-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

--------------------------------------------------
-- MAME tests
--------------------------------------------------

project("tests")
uuid ("66d4c639-196b-4065-a411-7ee9266564f5")
kind "ConsoleApp"	

options {
	"ForceCPP",
}

flags {
	"Symbols", -- always include minimum symbols for executables 	
}

if _OPTIONS["SEPARATE_BIN"]~="1" then 
	targetdir(MAME_DIR)
end

links {
	"unittest-cpp",
	"utils",
	"expat",
	"zlib",
	"ocore_" .. _OPTIONS["osd"],
}

includedirs {
	MAME_DIR .. "3rdparty/unittest-cpp",
	MAME_DIR .. "src/osd",
	MAME_DIR .. "src/lib/util",
}

files {
	MAME_DIR .. "tests/main.c",
	MAME_DIR .. "tests/lib/util/corestr.c",
}

