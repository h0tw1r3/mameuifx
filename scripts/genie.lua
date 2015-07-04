premake.check_paths = true
premake.make.override = { "TARGET" }
MAME_DIR = (path.getabsolute("..") .. "/")
local MAME_BUILD_DIR = (MAME_DIR .. "build/")
local naclToolchain = ""


function backtick(cmd)
	result = string.gsub(string.gsub(os.outputof(cmd), "\r?\n$", ""), " $", "")
	return result
end

function str_to_version(str)
	local val = 0
	if (str == nil or str == '') then
		return val
	end
	local cnt = 10000
	for word in string.gmatch(str, '([^.]+)') do
		val = val + tonumber(word) * cnt
		cnt = cnt / 100
	end
    return val
end

function findfunction(x)
  assert(type(x) == "string")
  local f=_G
  for v in x:gmatch("[^%.]+") do
    if type(f) ~= "table" then
       return nil, "looking for '"..v.."' expected table, not "..type(f)
    end
    f=f[v]
  end
  if type(f) == "function" then
    return f
  else
    return nil, "expected function, not "..type(f)
  end
end

function layoutbuildtask(_folder, _name)
	return { MAME_DIR .. "src/".._folder.."/".. _name ..".lay" ,    GEN_DIR .. _folder .. "/".._name..".lh",   
		{  MAME_DIR .. "src/build/file2str.py" }, {"@echo Converting src/".._folder.."/".._name..".lay...",    PYTHON .. " $(1) $(<) $(@) layout_".._name }};
end

CPUS = {}
SOUNDS  = {}
MACHINES  = {}
VIDEOS = {}
BUSES  = {}

newoption {
	trigger = "with-tools",
	description = "Enable building tools.",
}

newoption {
	trigger = "osd",
	description = "Choose OSD layer implementation",
}

newoption {
	trigger = "targetos",
	description = "Choose target OS",
	allowed = {
		{ "android-arm",   "Android - ARM"          },
		{ "android-mips",  "Android - MIPS"         },
		{ "android-x86",   "Android - x86"          },
		{ "asmjs",         "Emscripten/asm.js"      },
		{ "freebsd",       "FreeBSD"                },
		{ "netbsd",        "NetBSD"                 },
		{ "openbsd",       "OpenBSD"                },
		{ "nacl",          "Native Client"          },
		{ "nacl-arm",      "Native Client - ARM"    },
		{ "pnacl",         "Native Client - PNaCl"  },
		{ "linux",     	   "Linux"                  },
		{ "ios",           "iOS"                    },
		{ "macosx",        "OSX"                    },
		{ "windows",       "Windows"                },
		{ "os2",           "OS/2 eComStation"       },
		{ "haiku",         "Haiku"                  },
		{ "solaris",       "Solaris SunOS"          },
	},
}

newoption {
	trigger = "distro",
	description = "Choose distribution",
	allowed = {
		{ "generic", 		   "generic"         	},
		{ "debian-stable",     "debian-stable"      },
		{ "ubuntu-intrepid",   "ubuntu-intrepid"    },
	},
}

newoption {
	trigger = "target",
	description = "Building target",
}

newoption {
	trigger = "subtarget",
	description = "Building subtarget",
}

newoption {
	trigger = "gcc_version",
	description = "GCC compiler version",
}

newoption {
	trigger = "CC",
	description = "CC replacement",
}

newoption {
	trigger = "CXX",
	description = "CXX replacement",
}

newoption {
	trigger = "LD",
	description = "LD replacement",
}

newoption {
	trigger = "PROFILE",
	description = "Enable profiling.",
}

newoption {
	trigger = "SYMBOLS",
	description = "Enable symbols.",
}

newoption {
	trigger = "SYMLEVEL",
	description = "Symbols level.",
}

newoption {
	trigger = "PROFILER",
	description = "Include the internal profiler.",
}

newoption {
	trigger = "OPTIMIZE",
	description = "Optimization level.",
}

newoption {
	trigger = "ARCHOPTS",
	description = "ARCHOPTS.",
}

newoption {
	trigger = "LDOPTS",
	description = "Additional linker options",
}

newoption {
	trigger = "MAP",
	description = "Generate a link map.",
}

newoption {
	trigger = "NOASM",
	description = "Disable implementations based on assembler code",
	allowed = {
		{ "0",  "Enable assembler code"   },
		{ "1",  "Disable assembler code"  },
	},
}

newoption {
	trigger = "BIGENDIAN",
	description = "Build for big endian target",
	allowed = {
		{ "0",  "Little endian target"   },
		{ "1",  "Big endian target"  },
	},
}

newoption {
	trigger = "FORCE_DRC_C_BACKEND",
	description = "Force DRC C backend.",
}

newoption {
	trigger = "NOWERROR",
	description = "NOWERROR",
}

newoption {
	trigger = "USE_BGFX",
	description = "Use of BGFX.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "DEPRECATED",
	description = "Generate deprecation warnings during compilation.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "LTO",
	description = "Clang link time optimization.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "SSE2",
	description = "SSE2 optimized code and SSE2 code generation.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "OPENMP",
	description = "OpenMP optimized code.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "CPP11",
	description = "Compile c++ code as C++11.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "FASTDEBUG",
	description = "Fast DEBUG.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "FILTER_DEPS",
	description = "Filter dependency files.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "SEPARATE_BIN",
	description = "Use separate bin folders.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "PYTHON_EXECUTABLE",
	description = "Python executable.",
}

newoption {
	trigger = "SHADOW_CHECK",
	description = "Shadow checks.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}

newoption {
	trigger = "STRIP_SYMBOLS",
	description = "Symbols stripping.",
	allowed = {
		{ "0",   "Disabled" 	},
		{ "1",   "Enabled"      },
	}
}


PYTHON = "python"

if _OPTIONS["PYTHON_EXECUTABLE"]~=nil then
	PYTHON = _OPTIONS["PYTHON_EXECUTABLE"]
end

if not _OPTIONS["BIGENDIAN"] then
	_OPTIONS["BIGENDIAN"] = "0"
end

if not _OPTIONS["NOASM"] then
	if _OPTIONS["targetos"]=="emscripten" then
		_OPTIONS["NOASM"] = "1"
	else
		_OPTIONS["NOASM"] = "0"
	end
end

if _OPTIONS["NOASM"]=="1" and not _OPTIONS["FORCE_DRC_C_BACKEND"] then
	_OPTIONS["FORCE_DRC_C_BACKEND"] = "1"
end

USE_BGFX = 1
if(_OPTIONS["USE_BGFX"]~=nil) then
	USE_BGFX = tonumber(_OPTIONS["USE_BGFX"])
end

GEN_DIR = MAME_BUILD_DIR .. "generated/"

if (_OPTIONS["target"] == nil) then return false end
if (_OPTIONS["subtarget"] == nil) then return false end

if (_OPTIONS["target"] == _OPTIONS["subtarget"]) then
	solution (_OPTIONS["target"])
else
	solution (_OPTIONS["target"] .. _OPTIONS["subtarget"])
end

configurations {
	"Debug",
	"Release",
}

platforms {
	"x32",
	"x64",
	"Native", -- for targets where bitness is not specified
}

language "C++"

flags {
	"StaticRuntime",
	"NoPCH",
}

configuration { "vs*" }
	flags {
		"ExtraWarnings",
	}
	if not _OPTIONS["NOWERROR"] then
		flags{
			"FatalWarnings",
		}
	end


configuration { "Debug", "vs*" }
	flags {
		"Symbols",
	}
	
configuration { "Release", "vs*" }
	flags {
		"Optimize",
	}

configuration {}

local AWK = ""
if (os.is("windows")) then
	AWK_TEST = backtick("awk --version 2> NUL")
	if (AWK_TEST~='') then
		AWK = "awk"
	else
		AWK_TEST = backtick("gawk --version 2> NUL")
		if (AWK_TEST~='') then
			AWK = "gawk"
		end
	end
else
	AWK_TEST = backtick("awk --version 2> /dev/null")
	if (AWK_TEST~='') then
		AWK = "awk"
	else
		AWK_TEST = backtick("gawk --version 2> /dev/null")
		if (AWK_TEST~='') then
			AWK = "gawk"
		end
	end
end

if (_OPTIONS["FILTER_DEPS"]=="1") and (AWK~='') then
	postcompiletasks {
		AWK .. " -f ../../../../../scripts/depfilter.awk $(@:%.o=%.d) > $(@:%.o=%.dep)",
		"mv $(@:%.o=%.dep) $(@:%.o=%.d)",
	}
end

msgcompile ("Compiling $(subst ../,,$<)...")

msgcompile_objc ("Objective-C compiling $(subst ../,,$<)...")

msgresource ("Compiling resources $(subst ../,,$<)...")

msglinking ("Linking $(notdir $@)...")

msgarchiving ("Archiving $(notdir $@)...")

messageskip { "SkipCreatingMessage", "SkipBuildingMessage", "SkipCleaningMessage" }

if (not os.isfile(path.join("target", _OPTIONS["target"],_OPTIONS["subtarget"] .. ".lua"))) then
	error("File definition for TARGET=" .. _OPTIONS["target"] .. " SUBTARGET=" .. _OPTIONS["subtarget"] .. " does not exist")
end
dofile (path.join("target", _OPTIONS["target"],_OPTIONS["subtarget"] .. ".lua"))

configuration { "gmake" }
	flags {
		"SingleOutputDir",
	}

dofile ("toolchain.lua")


if _OPTIONS["targetos"]=="windows" or _OPTIONS["targetos"]=="winui" then
	configuration { "x64" }
		defines {
			"X64_WINDOWS_ABI",
		}
	configuration { }
end

-- Avoid error when invoking genie --help.
if (_ACTION == nil) then return false end

-- define PTR64 if we are a 64-bit target
configuration { "x64" }
	defines { "PTR64=1" }

-- map the INLINE to something digestible by GCC
configuration { "gmake" }
	buildoptions_cpp {
		"-DINLINE=\"static inline\"",
	}
	buildoptions_objc {
		"-DINLINE=\"static inline\"",
	}
configuration { "xcode4*" }
	buildoptions {
		"-DINLINE=\"static inline\"",
	}

configuration { "vs*" }
	defines {
		"INLINE=static inline",
	}

-- define MAME_DEBUG if we are a debugging build
configuration { "Debug" }
	defines {
		"MAME_DEBUG",
		"MAME_PROFILER",
	}
if _OPTIONS["FASTDEBUG"]=="1" then
	defines {
		"MAME_DEBUG_FAST"
	}
end

configuration { }	

if _OPTIONS["PROFILER"]=="1" then
	defines{
		"MAME_PROFILER", -- define MAME_PROFILER if we are a profiling build
	}
end

configuration { "Release" }
	defines {
		"NDEBUG",
	}

configuration { }

-- CR/LF setup: use both on win32/os2, CR only on everything else
if _OPTIONS["targetos"]=="windows" or _OPTIONS["targetos"]=="winui" or _OPTIONS["targetos"]=="os2" then
	defines {
		"CRLF=3",
	}
else
	defines {
		"CRLF=2",
	}
end


if _OPTIONS["BIGENDIAN"]=="1" then
	if _OPTIONS["targetos"]=="macosx" then
		defines {
			"OSX_PPC",
		}
		buildoptions {
			"-Wno-unused-label",
		}
		if _OPTIONS["SYMBOLS"] then
			buildoptions {
				"-mlong-branch",
			}
		end
		configuration { "x64" }
			buildoptions {
				"-arch ppc64",
			}
			linkoptions {
				"-arch ppc64",
			}
		configuration { "x32" }
			buildoptions {
				"-arch ppc",
			}
			linkoptions {
				"-arch ppc",
			}
		configuration { }
	end
else
	defines {
		"LSB_FIRST",
	}
	if _OPTIONS["targetos"]=="macosx" then
		configuration { "x64" }
			buildoptions {
				"-arch x86_64",
			}
			linkoptions {
				"-arch x86_64",
			}
		configuration { "x32" }
			buildoptions {
				"-arch i386",
			}
			linkoptions {
				"-arch i386",
			}
		configuration { }
	end
end

-- need to ensure FLAC functions are statically linked
defines {
	"FLAC__NO_DLL",
}

if _OPTIONS["NOASM"]=="1" then
	defines {
		"MAME_NOASM"
	}
end

if not _OPTIONS["FORCE_DRC_C_BACKEND"] then
	if _OPTIONS["BIGENDIAN"]~="1" then
		configuration { "x64" }
			defines {
				"NATIVE_DRC=drcbe_x64",
			}
		configuration { "x32" }
			defines {
				"NATIVE_DRC=drcbe_x86",
			}
		configuration {  }
	end
end

-- define USE_SYSTEM_JPEGLIB if library shipped with MAME is not used
--ifneq ($(BUILD_JPEGLIB),1)
--DEFS += -DUSE_SYSTEM_JPEGLIB
--endif

	--To support casting in Lua 5.3
	defines {
		"LUA_COMPAT_APIINTCASTS",
	}

	if _ACTION == "gmake" then

	--we compile C-only to C89 standard with GNU extensions
if (_OPTIONS["targetos"]=="solaris") then
	buildoptions_c {
		"-std=gnu99",
	}
else
	buildoptions_c {
		"-std=gnu89",

	}
end	

	
if _OPTIONS["CPP11"]=="1" then
	buildoptions_cpp {
		"-x c++",
		"-std=gnu++11",
	}
else
	--we compile C++ code to C++98 standard with GNU extensions
	buildoptions_cpp {
		"-x c++",
		"-std=gnu++98",
	}
end

	buildoptions_objc {
		"-x objective-c++",
	}


-- this speeds it up a bit by piping between the preprocessor/compiler/assembler
	if not ("pnacl" == _OPTIONS["gcc"]) then
		buildoptions {
			"--pipe",
		}
	end
-- add -g if we need symbols, and ensure we have frame pointers
if _OPTIONS["SYMBOLS"]~=nil then
	buildoptions {
		"-g" .. _OPTIONS["SYMLEVEL"],
		"-fno-omit-frame-pointer",
		"-fno-optimize-sibling-calls",
	}
end

--# we need to disable some additional implicit optimizations for profiling
if _OPTIONS["PROFILE"] then
	buildoptions {
		"-mno-omit-leaf-frame-pointer",
	}
end
-- add -v if we need verbose build information
if _OPTIONS["VERBOSE"] then
	buildoptions {
		"-v",
	}
end

-- only show deprecation warnings when enabled
if _OPTIONS["DEPRECATED"]~="1" then
	buildoptions {
		"-Wno-deprecated-declarations"
	}
end

-- add profiling information for the compiler
if _OPTIONS["PROFILE"] then
	buildoptions {
		"-pg",
	}
	linkoptions {
		"-pg",
	}
end

if _OPTIONS["SYMBOLS"]~=nil then
	flags {
		"Symbols",
	}
end

--# add the optimization flag
	buildoptions {
		"-O".. _OPTIONS["OPTIMIZE"],
		"-fno-strict-aliasing"
	}

	-- add the error warning flag
if _OPTIONS["NOWERROR"]==nil then
	buildoptions {
		"-Werror",
	}
end

-- if we are optimizing, include optimization options
if _OPTIONS["OPTIMIZE"] then
	buildoptions {
		"-fno-strict-aliasing"
	}
	if _OPTIONS["ARCHOPTS"] then
		buildoptions {
			_OPTIONS["ARCHOPTS"]
		}
	end
	if _OPTIONS["LTO"]=="1" then
		buildoptions {
			"-flto",
		}
		linkoptions {
			"-flto",
		}
	end
end

if _OPTIONS["SSE2"]=="1" then
	buildoptions {
		"-msse2",
	}
end

if _OPTIONS["OPENMP"]=="1" then
	buildoptions {
		"-fopenmp",
	}
	linkoptions {
		"-fopenmp"
	}
	defines {
		"USE_OPENMP=1",
	}
	
else
	buildoptions {
		"-Wno-unknown-pragmas",
	}
end

if _OPTIONS["LDOPTS"] then
	linkoptions {
		_OPTIONS["LDOPTS"]
	}
end

if _OPTIONS["MAP"] then
	if (_OPTIONS["target"] == _OPTIONS["subtarget"]) then
		linkoptions {
			"-Wl,-Map," .. "../../../../" .. _OPTIONS["target"] .. ".map"
		}
	else
		linkoptions {
			"-Wl,-Map," .. "../../../../"  .. _OPTIONS["target"] .. _OPTIONS["subtarget"] .. ".map"
		}

	end
end


-- add a basic set of warnings
	buildoptions {
		"-Wall",
		"-Wcast-align",
		"-Wundef",
		"-Wformat-security",
		"-Wwrite-strings",
		"-Wno-sign-compare",
		"-Wno-conversion",
	}
-- warnings only applicable to C compiles
	buildoptions_c {
		"-Wpointer-arith",
		"-Wstrict-prototypes",
	}
	
if _OPTIONS["targetos"]~="freebsd" then
	buildoptions_c {
		"-Wbad-function-cast",
	}
end

-- warnings only applicable to OBJ-C compiles
	buildoptions_objc {
		"-Wpointer-arith",
	}

-- warnings only applicable to C++ compiles
	buildoptions_cpp {
		"-Woverloaded-virtual",
	}

--ifdef SANITIZE
--CCOMFLAGS += -fsanitize=$(SANITIZE)

--ifneq (,$(findstring thread,$(SANITIZE)))
--CCOMFLAGS += -fPIE
--endif
--endif



		local version = str_to_version(_OPTIONS["gcc_version"])
		if string.find(_OPTIONS["gcc"], "clang") then
			buildoptions {
				"-Wno-cast-align",
				"-Wno-tautological-compare",
				"-Wno-dynamic-class-memaccess",
				"-Wno-self-assign-field",
			}
			if (version >= 30200) then
				buildoptions {
					"-Wno-unused-value",
				}
			end
			if (version >= 30400) then
				buildoptions {
					"-Wno-inline-new-delete",
					"-Wno-constant-logical-operand",
				}
			end
			if (version >= 30500) then
				buildoptions {
					"-Wno-absolute-value",
					"-Wno-unknown-warning-option",
					"-Wno-extern-c-compat",
				}
			end
		else
			if (_OPTIONS["SHADOW_CHECK"]=="1") then
				buildoptions {
					"-Wshadow"
				}			
			end
			if (version == 40201) then
				buildoptions {
					"-Wno-cast-align"
				}
			end
			if (version >= 40400) then
				buildoptions {
					"-Wno-unused-result",
				}
			end

			if (version >= 40700) then
				buildoptions {
					"-Wno-narrowing",
					"-Wno-attributes"
				}
			end
			if (version >= 40800) then
				-- array bounds checking seems to be buggy in 4.8.1 (try it on video/stvvdp1.c and video/model1.c without -Wno-array-bounds)
				buildoptions {
					"-Wno-unused-variable",
					"-Wno-array-bounds"
				}
			end
		end
	end
--ifeq ($(findstring arm,$(UNAME)),arm)
--	CCOMFLAGS += -Wno-cast-align
--endif

local subdir
if (_OPTIONS["target"] == _OPTIONS["subtarget"]) then
	subdir = _OPTIONS["osd"] .. "/" .. _OPTIONS["target"]
else
	subdir = _OPTIONS["osd"] .. "/" .. _OPTIONS["target"] .. _OPTIONS["subtarget"]
end

if not toolchain(MAME_BUILD_DIR, subdir) then
	return -- no action specified
end

configuration { "asmjs" }
	buildoptions {
		"-std=gnu89",
		"-Wno-implicit-function-declaration",
	}
	buildoptions_cpp {
		"-x c++",
		"-std=gnu++98",
	}
	archivesplit_size "20"

configuration { "android*" }
	buildoptions {
		"-Wno-undef",
	}
	buildoptions_cpp {
		"-x c++",
		"-std=gnu++98",
	}
	archivesplit_size "20"

configuration { "pnacl" }
	buildoptions {
		"-std=gnu89",
		"-Wno-inline-new-delete",
	}
	buildoptions_cpp {
		"-x c++",
		"-std=gnu++98",
	}
	archivesplit_size "20"

configuration { "nacl*" }
	buildoptions_cpp {
		"-x c++",
		"-std=gnu++98",
	}
	archivesplit_size "20"

configuration { "linux-*" }
		links {
			"dl",
		}
		if _OPTIONS["distro"]=="debian-stable" then
			defines
			{
				"NO_AFFINITY_NP",
			}
		end


configuration { "osx*" }
		links {
			"pthread",
		}

configuration { "mingw*" }
		linkoptions {
			"-static-libgcc",
			"-static-libstdc++",
		}
		links {
			"user32",
			"winmm",
			"advapi32",
			"shlwapi",
			"wsock32",
		}

configuration { "vs*" }
		defines {
			"XML_STATIC",
			"WIN32",
			"_WIN32",
			"_CRT_NONSTDC_NO_DEPRECATE",
			"_CRT_SECURE_NO_DEPRECATE",
		}
		links {
			"user32",
			"winmm",
			"advapi32",
			"shlwapi",
			"wsock32",
		}

		buildoptions {
			"/wd4025", -- warning C4025: 'number' : based pointer passed to function with variable arguments: parameter number
			"/wd4003", -- warning C4003: not enough actual parameters for macro 'xxx'
			"/wd4018", -- warning C4018: 'x' : signed/unsigned mismatch 
			"/wd4061", -- warning C4061: enumerator 'xxx' in switch of enum 'xxx' is not explicitly handled by a case label
			"/wd4100", -- warning C4100: 'xxx' : unreferenced formal parameter
			"/wd4127", -- warning C4127: conditional expression is constant
			"/wd4131", -- warning C4131: 'xxx' : uses old-style declarator
			"/wd4141", -- warning C4141: 'xxx' : used more than once
			"/wd4146", -- warning C4146: unary minus operator applied to unsigned type, result still unsigned
			"/wd4150", -- warning C4150: deletion of pointer to incomplete type 'xxx'; no destructor called
			"/wd4189", -- warning C4189: 'xxx' : local variable is initialized but not referenced
			"/wd4191", -- warning C4191: 'type cast' : unsafe conversion from 'xxx' to 'xxx' // 64-bit only
			"/wd4201", -- warning C4201: nonstandard extension used : nameless struct/union
			"/wd4232", -- warning C4232: nonstandard extension used : 'xxx' : address of dllimport 'xxx' is not static, identity not guaranteed
			"/wd4242", -- warning C4242: 'x' : conversion from 'xxx' to 'xxx', possible loss of data
			"/wd4244", -- warning C4244: 'argument' : conversion from 'xxx' to 'xxx', possible loss of data
			"/wd4250", -- warning C4250: 'xxx' : inherits 'xxx' via dominance
			"/wd4255", -- warning C4255: 'xxx' : no function prototype given: converting '()' to '(void)'
			"/wd4296", -- warning C4296: 'x' : expression is always false
			"/wd4306", -- warning C4306: 'xxx': conversion from 'type1' to 'type2' of greater size // 64-bit only
			"/wd4310", -- warning C4310: cast truncates constant value
			"/wd4312", -- warning C4312: 'type cast' : conversion from 'xxx' to 'xxx' of greater size
			"/wd4324", -- warning C4324: 'xxx' : structure was padded due to __declspec(align())
			"/wd4347", -- warning C4347: behavior change: 'xxx' is called instead of 'xxx' // obsolete VS2005 - VS2010 only
			"/wd4435", -- warning C4435: 'xxx' : Object layout under /vd2 will change due to virtual base 'xxx'
			"/wd4510", -- warning C4510: 'xxx' : default constructor could not be generated
			"/wd4512", -- warning C4512: 'xxx' : assignment operator could not be generated
			"/wd4514", -- warning C4514: 'xxx' : unreferenced inline function has been removed
			"/wd4571", -- warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable
			"/wd4610", -- warning C4619: #pragma warning : there is no warning number 'xxx'
			"/wd4611", -- warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
			"/wd4619", -- warning C4610: struct 'xxx' can never be instantiated - user defined constructor required
			"/wd4625", -- warning C4625: 'xxx' : copy constructor could not be generated because a base class copy constructor is inaccessible or deleted
			"/wd4626", -- warning C4626: 'xxx' : assignment operator could not be generated because a base class assignment operator is inaccessible or deleted
			"/wd4640", -- warning C4640: 'xxx' : construction of local static object is not thread-safe
			"/wd4668", -- warning C4668: 'xxx' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
			"/wd4702", -- warning C4702: unreachable code
			"/wd4706", -- warning C4706: assignment within conditional expression
			"/wd4710", -- warning C4710: 'xxx' : function not inlined
			"/wd4711", -- warning C4711: function 'xxx' selected for automatic inline expansion // optimized only
			"/wd4805", -- warning C4805: 'x' : unsafe mix of type 'xxx' and type 'xxx' in operation
			"/wd4820", -- warning C4820: 'xxx' : 'x' bytes padding added after data member 'xxx'
			"/wd4826", -- warning C4826: Conversion from 'type1 ' to 'type_2' is sign-extended. This may cause unexpected runtime behavior. // 32-bit only
			"/wd4365", -- warning C4365: 'action' : conversion from 'type_1' to 'type_2', signed/unsigned mismatch
			"/wd4389", -- warning C4389: 'operator' : signed/unsigned mismatch
			"/wd4245", -- warning C4245: 'conversion' : conversion from 'type1' to 'type2', signed/unsigned mismatch
			"/wd4388", -- warning C4388: 
			"/wd4267", -- warning C4267: 'var' : conversion from 'size_t' to 'type', possible loss of data
			"/wd4005", -- warning C4005: The macro identifier is defined twice. The compiler uses the second macro definition
			"/wd4350", -- warning C4350: behavior change: 'member1' called instead of 'member2'
			"/wd4996", -- warning C4996: 'function': was declared deprecated
			"/wd4191", -- warning C4191: 'operator/operation' : unsafe conversion from 'type of expression' to 'type required'
			"/wd4060", -- warning C4060: switch statement contains no 'case' or 'default' labels
			"/wd4065", -- warning C4065: switch statement contains 'default' but no 'case' labels
			"/wd4640", -- warning C4640: 'instance' : construction of local static object is not thread-safe
			"/wd4290", -- warning C4290: 
			"/wd4355", -- warning C4355: 'this' : used in base member initializer list
			"/wd4800", -- warning C4800: 'type' : forcing value to bool 'true' or 'false' (performance warning)
			"/wd4371", -- warning C4371: 
			"/wd4548", -- warning C4548: expression before comma has no effect; expected expression with side-effect
		}
if _OPTIONS["vs"]=="intel-15" then
		buildoptions {
			"/Qwd9",    			-- remark #9: nested comment is not allowed
			"/Qwd82",   			-- remark #82: storage class is not first
			"/Qwd111",  			-- remark #111: statement is unreachable
			"/Qwd128",  			-- remark #128: loop is not reachable
			"/Qwd177",  			-- remark #177: function "xxx" was declared but never referenced
			"/Qwd181",  			-- remark #181: argument of type "UINT32={unsigned int}" is incompatible with format "%d", expecting argument of type "int"
			"/Qwd185",  			-- remark #185: dynamic initialization in unreachable code
			"/Qwd280",  			-- remark #280: selector expression is constant
			"/Qwd344",  			-- remark #344: typedef name has already been declared (with same type)
			"/Qwd411",  			-- remark #411: class "xxx" defines no constructor to initialize the following
			"/Qwd869",  			-- remark #869: parameter "xxx" was never referenced
			"/Qwd2545", 			-- remark #2545: empty dependent statement in "else" clause of if - statement
			"/Qwd2553", 			-- remark #2553: nonstandard second parameter "TCHAR={WCHAR = { __wchar_t } } **" of "main", expected "char *[]" or "char **" extern "C" int _tmain(int argc, TCHAR **argv)
			"/Qwd2557", 			-- remark #2557: comparison between signed and unsigned operands
			"/Qwd3280", 			-- remark #3280: declaration hides member "attotime::seconds" (declared at line 126) static attotime from_seconds(INT32 seconds) { return attotime(seconds, 0); }

			"/Qwd170",  			-- error #170: pointer points outside of underlying object
			"/Qwd188",  			-- error #188: enumerated type mixed with another type

			"/Qwd63",   			-- warning #63: shift count is too large
			"/Qwd177",  			-- warning #177: label "xxx" was declared but never referenced
			"/Qwd186",  			-- warning #186: pointless comparison of unsigned integer with zero
			"/Qwd488",  			-- warning #488: template parameter "_FunctionClass" is not used in declaring the parameter types of function template "device_delegate<_Signature>::device_delegate<_FunctionClass>(delegate<_Signature>:
			"/Qwd1478", 			-- warning #1478: function "xxx" (declared at line yyy of "zzz") was declared deprecated
			"/Qwd1879", 			-- warning #1879: unimplemented pragma ignored
			"/Qwd3291", 			-- warning #3291: invalid narrowing conversion from "double" to "int"
			"/Qwd1195",
			"/Qwd1786",
			"/Qwd592", -- For lua, false positive?
		}
end

		linkoptions {
			"/ignore:4221", -- LNK4221: This object file does not define any previously undefined public symbols, so it will not be used by any link operation that consumes this library
		}
		includedirs {
			MAME_DIR .. "3rdparty/dxsdk/Include"
		}
configuration { "vs2015" }
		buildoptions {
			"/wd4456", -- warning C4456: declaration of 'xxx' hides previous local declaration
			"/wd4457", -- warning C4457: declaration of 'xxx' hides function parameter
			"/wd4458", -- warning C4458: declaration of 'xxx' hides class member
			"/wd4459", -- warning C4459: declaration of 'xxx' hides global declaration
			"/wd4838", -- warning C4838: conversion from 'xxx' to 'yyy' requires a narrowing conversion
			"/wd4091", -- warning C4091: 'typedef ': ignored on left of '' when no variable is declared
			"/wd4463", -- warning C4463: overflow; assigning 1 to bit-field that can only hold values from -1 to 0
			"/wd4297", -- warning C4297: 'xxx::~xxx': function assumed not to throw an exception but does
		}
configuration { "vs2010" }
		buildoptions {
			"/wd4481", -- warning C4481: nonstandard extension used: override specifier 'override'
		}

configuration { "x32", "vs*" }
		libdirs {
			MAME_DIR .. "3rdparty/dxsdk/lib/x86",
		}

configuration { "x64", "vs*" }
		libdirs {
			MAME_DIR .. "3rdparty/dxsdk/lib/x64",
		}

configuration { }


group "libs"

if (not os.isfile(path.join("src", "osd",  _OPTIONS["osd"] .. ".lua"))) then
	error("Unsupported value '" .. _OPTIONS["osd"] .. "' for OSD")
end
dofile(path.join("src", "osd", _OPTIONS["osd"] .. ".lua"))
dofile(path.join("src", "lib.lua"))

group "3rdparty"
dofile(path.join("src", "3rdparty.lua"))


group "core"

dofile(path.join("src", "emu.lua"))
emuProject(_OPTIONS["target"],_OPTIONS["subtarget"])

group "drivers"
findfunction("createProjects_" .. _OPTIONS["target"] .. "_" .. _OPTIONS["subtarget"])(_OPTIONS["target"], _OPTIONS["subtarget"])

group "emulator"
dofile(path.join("src", "main.lua"))
if (_OPTIONS["target"] == _OPTIONS["subtarget"]) then
	startproject (_OPTIONS["target"])
else
	startproject (_OPTIONS["target"] .. _OPTIONS["subtarget"])
end
mainProject(_OPTIONS["target"],_OPTIONS["subtarget"])

if (_OPTIONS["STRIP_SYMBOLS"]=="1") then
	strip()
end

if _OPTIONS["with-tools"] then
	group "tools"
	dofile(path.join("src", "tools.lua"))
end

