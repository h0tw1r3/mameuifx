// license:BSD-3-Clause
// copyright-holders:Juergen Buchmueller
/*****************************************************************************
 *
 *   Portable uPD7810/11, 7810H/11H, 78C10/C11/C14 disassembler
 *
 *   NS20030112: added 7807.
 *
 *****************************************************************************/

#include "emu.h"
#include "debugger.h"
#include "upd7810.h"

struct dasm_s {
	UINT8 token;
	const void *args;
};

enum
{
	illegal=1,
	ACI,
	ADC,
	ADCW,
	ADCX,
	ADD,
	ADDNC,
	ADDNCW,
	ADDNCX,
	ADDW,
	ADDX,
	ADI,
	ADINC,
	ANA,
	ANAW,
	ANAX,
	AND,
	ANI,
	ANIW,
	BIT,
	BLOCK,
	CALB,
	CALF,
	CALL,
	CALT,
	CLC,
	CLR,    /* 7807 */
	CMC,    /* 7807 */
	DAA,
	DADC,
	DADD,
	DADDNC,
	DAN,
	DCR,
	DCRW,
	DCX,
	DEQ,
	DGT,
	DI,
	DIV,
	DLT,
	DMOV,
	DNE,
	DOFF,
	DON,
	DOR,
	DRLL,
	DRLR,
	DSBB,
	DSLL,
	DSLR,
	DSUB,
	DSUBNB,
	DXR,
	EADD,
	EI,
	EQA,
	EQAW,
	EQAX,
	EQI,
	EQIW,
	ESUB,
	EX,     /* 7801 */
	EXA,
	EXH,
	EXX,
	EXR,    /* 7807 */
	GTA,
	GTAW,
	GTAX,
	GTI,
	GTIW,
	HALT,
	IN,     /* 7801 */
	INR,
	INRW,
	INX,
	JB,
	JEA,
	JMP,
	JR,
	JRE,
	LBCD,
	LDAW,
	LDAX,
	LDEAX,
	LDED,
	LHLD,
	LSPD,
	LTA,
	LTAW,
	LTAX,
	LTI,
	LTIW,
	LXI,
	MOV,
	MUL,
	MVI,
	MVIW,
	MVIX,
	NEA,
	NEAW,
	NEAX,
	NEGA,
	NEI,
	NEIW,
	NOP,
	NOT,    /* 7807 */
	OFFA,
	OFFAW,
	OFFAX,
	OFFI,
	OFFIW,
	ONA,
	ONAW,
	ONAX,
	ONI,
	ONIW,
	OR, /* 7807 */
	ORA,
	ORAW,
	ORAX,
	ORI,
	ORIW,
	OUT,    /* 7801 */
	PER,    /* 7801 */
	PEX,    /* 7801 */
	POP,
	PUSH,
	RET,
	RETI,
	RETS,
	RLD,
	RLL,
	RLR,
	RRD,
	SBB,
	SBBW,
	SBBX,
	SBCD,
	SBI,
	SDED,
	SETB,   /* 7807 */
	SHLD,
	SIO,    /* 7801 */
	SK,
	SKIT,
	SKN,
	SKNIT,
	SLL,
	SLLC,
	SLR,
	SLRC,
	SOFTI,
	SSPD,
	STAW,
	STAX,
	STC,
	STEAX,
	STM,    /* 7801 */
	STOP,
	SUB,
	SUBNB,
	SUBNBW,
	SUBNBX,
	SUBW,
	SUBX,
	SUI,
	SUINB,
	TABLE,
	XOR,    /* 7807 */
	XRA,
	XRAW,
	XRAX,
	XRI
};

static const char *const token[] =
{
	"",
	"illegal",
	"ACI",
	"ADC",
	"ADCW",
	"ADCX",
	"ADD",
	"ADDNC",
	"ADDNCW",
	"ADDNCX",
	"ADDW",
	"ADDX",
	"ADI",
	"ADINC",
	"ANA",
	"ANAW",
	"ANAX",
	"AND",
	"ANI",
	"ANIW",
	"BIT",
	"BLOCK",
	"CALB",
	"CALF",
	"CALL",
	"CALT",
	"CLC",
	"CLR",  /* 7807 */
	"CMC",  /* 7807 */
	"DAA",
	"DADC",
	"DADD",
	"DADDNC",
	"DAN",
	"DCR",
	"DCRW",
	"DCX",
	"DEQ",
	"DGT",
	"DI",
	"DIV",
	"DLT",
	"DMOV",
	"DNE",
	"DOFF",
	"DON",
	"DOR",
	"DRLL",
	"DRLR",
	"DSBB",
	"DSLL",
	"DSLR",
	"DSUB",
	"DSUBNB",
	"DXR",
	"EADD",
	"EI",
	"EQA",
	"EQAW",
	"EQAX",
	"EQI",
	"EQIW",
	"ESUB",
	"EX",   /* 7801 */
	"EXA",
	"EXH",
	"EXX",
	"EXR",  /* 7807 */
	"GTA",
	"GTAW",
	"GTAX",
	"GTI",
	"GTIW",
	"HALT",
	"IN",   /* 7801 */
	"INR",
	"INRW",
	"INX",
	"JB",
	"JEA",
	"JMP",
	"JR",
	"JRE",
	"LBCD",
	"LDAW",
	"LDAX",
	"LDEAX",
	"LDED",
	"LHLD",
	"LSPD",
	"LTA",
	"LTAW",
	"LTAX",
	"LTI",
	"LTIW",
	"LXI",
	"MOV",
	"MUL",
	"MVI",
	"MVIW",
	"MVIX",
	"NEA",
	"NEAW",
	"NEAX",
	"NEGA",
	"NEI",
	"NEIW",
	"NOP",
	"NOT",  /* 7807 */
	"OFFA",
	"OFFAW",
	"OFFAX",
	"OFFI",
	"OFFIW",
	"ONA",
	"ONAW",
	"ONAX",
	"ONI",
	"ONIW",
	"OR",   /* 7807 */
	"ORA",
	"ORAW",
	"ORAX",
	"ORI",
	"ORIW",
	"OUT",  /* 7801 */
	"PER",  /* 7801 */
	"PEX",  /* 7801 */
	"POP",
	"PUSH",
	"RET",
	"RETI",
	"RETS",
	"RLD",
	"RLL",
	"RLR",
	"RRD",
	"SBB",
	"SBBW",
	"SBBX",
	"SBCD",
	"SBI",
	"SDED",
	"SETB", /* 7807 */
	"SHLD",
	"SIO",  /* 7801 */
	"SK",
	"SKIT",
	"SKN",
	"SKNIT",
	"SLL",
	"SLLC",
	"SLR",
	"SLRC",
	"SOFTI",
	"SSPD",
	"STAW",
	"STAX",
	"STC",
	"STEAX",
	"STM",  /*7801 */
	"STOP",
	"SUB",
	"SUBNB",
	"SUBNBW",
	"SUBNBX",
	"SUBW",
	"SUBX",
	"SUI",
	"SUINB",
	"TABLE",
	"XOR",  /* 7807 */
	"XRA",
	"XRAW",
	"XRAX",
	"XRI"
};


/* prefix 48 */
static const struct dasm_s dasm48_7810[256] =
{
	{illegal,nullptr   }, /* 00: 0100 1000 0000 0000                      */
	{SLRC,"A"       }, /* 01: 0100 1000 0000 0001                      */
	{SLRC,"B"       }, /* 02: 0100 1000 0000 0010                      */
	{SLRC,"C"       }, /* 03: 0100 1000 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0100 1000 0000 0100                      */
	{SLLC,"A"       }, /* 05: 0100 1000 0000 0101                      */
	{SLLC,"B"       }, /* 06: 0100 1000 0000 0110                      */
	{SLLC,"C"       }, /* 07: 0100 1000 0000 0111                      */
	{SK,"NV"        }, /* 08: 0100 1000 0000 1000                      */
	{illegal,nullptr   }, /* 09: 0100 1000 0000 1001                      */
	{SK,"CY"        }, /* 0a: 0100 1000 0000 1010                      */
	{SK,"HC"        }, /* 0b: 0100 1000 0000 1011                      */
	{SK,"Z"         }, /* 0c: 0100 1000 0000 1100                      */
	{illegal,nullptr   }, /* 0d: 0100 1000 0000 1101                      */
	{illegal,nullptr   }, /* 0e: 0100 1000 0000 1110                      */
	{illegal,nullptr   }, /* 0f: 0100 1000 0000 1111                      */

	{illegal,nullptr   }, /* 10: 0100 1000 0001 0000                      */
	{illegal,nullptr   }, /* 11: 0100 1000 0001 0001                      */
	{illegal,nullptr   }, /* 12: 0100 1000 0001 0010                      */
	{illegal,nullptr   }, /* 13: 0100 1000 0001 0011                      */
	{illegal,nullptr   }, /* 14: 0100 1000 0001 0100                      */
	{illegal,nullptr   }, /* 15: 0100 1000 0001 0101                      */
	{illegal,nullptr   }, /* 16: 0100 1000 0001 0110                      */
	{illegal,nullptr   }, /* 17: 0100 1000 0001 0111                      */
	{SKN,"NV"       }, /* 18: 0100 1000 0001 1000                      */
	{illegal,nullptr   }, /* 19: 0100 1000 0001 1001                      */
	{SKN,"CY"       }, /* 1a: 0100 1000 0001 1010                      */
	{SKN,"HC"       }, /* 1b: 0100 1000 0001 1011                      */
	{SKN,"Z"        }, /* 1c: 0100 1000 0001 1100                      */
	{illegal,nullptr   }, /* 1d: 0100 1000 0001 1101                      */
	{illegal,nullptr   }, /* 1e: 0100 1000 0001 1110                      */
	{illegal,nullptr   }, /* 1f: 0100 1000 0001 1111                      */

	{illegal,nullptr   }, /* 20: 0100 1000 0010 0000                      */
	{SLR,"A"        }, /* 21: 0100 1000 0010 0001                      */
	{SLR,"B"        }, /* 22: 0100 1000 0010 0010                      */
	{SLR,"C"        }, /* 23: 0100 1000 0010 0011                      */
	{illegal,nullptr   }, /* 24: 0100 1000 0010 0100                      */
	{SLL,"A"        }, /* 25: 0100 1000 0010 0101                      */
	{SLL,"B"        }, /* 26: 0100 1000 0010 0110                      */
	{SLL,"C"        }, /* 27: 0100 1000 0010 0111                      */
	{JEA,nullptr       }, /* 28: 0100 1000 0010 1000                      */
	{CALB,nullptr      }, /* 29: 0100 1000 0010 1001                      */
	{CLC,nullptr       }, /* 2a: 0100 1000 0010 1010                      */
	{STC,nullptr       }, /* 2b: 0100 1000 0010 1011                      */
	{illegal,nullptr   }, /* 2c: 0100 1000 0010 1100                      */
	{MUL,"A"        }, /* 2d: 0100 1000 0010 1101                      */
	{MUL,"B"        }, /* 2e: 0100 1000 0010 1110                      */
	{MUL,"C"        }, /* 2f: 0100 1000 0010 1111                      */

	{illegal,nullptr   }, /* 30: 0100 1000 0011 0000                      */
	{RLR,"A"        }, /* 31: 0100 1000 0011 0001                      */
	{RLR,"B"        }, /* 32: 0100 1000 0011 0010                      */
	{RLR,"C"        }, /* 33: 0100 1000 0011 0011                      */
	{illegal,nullptr   }, /* 34: 0100 1000 0011 0100                      */
	{RLL,"A"        }, /* 35: 0100 1000 0011 0101                      */
	{RLL,"B"        }, /* 36: 0100 1000 0011 0110                      */
	{RLL,"C"        }, /* 37: 0100 1000 0011 0111                      */
	{RLD,nullptr       }, /* 38: 0100 1000 0011 1000                      */
	{RRD,nullptr       }, /* 39: 0100 1000 0011 1001                      */
	{NEGA,nullptr      }, /* 3a: 0100 1000 0011 1010                      */
	{HALT,nullptr      }, /* 3b: 0100 1000 0011 1011                      */
	{illegal,nullptr   }, /* 3c: 0100 1000 0011 1100                      */
	{DIV,"A"        }, /* 3d: 0100 1000 0011 1101                      */
	{DIV,"B"        }, /* 3e: 0100 1000 0011 1110                      */
	{DIV,"C"        }, /* 3f: 0100 1000 0011 1111                      */

	{SKIT,"NMI"     }, /* 40: 0100 1000 0100 0000                      */
	{SKIT,"FT0"     }, /* 41: 0100 1000 0100 0001                      */
	{SKIT,"FT1"     }, /* 42: 0100 1000 0100 0010                      */
	{SKIT,"F1"      }, /* 43: 0100 1000 0100 0011                      */
	{SKIT,"F2"      }, /* 44: 0100 1000 0100 0100                      */
	{SKIT,"FE0"     }, /* 45: 0100 1000 0100 0101                      */
	{SKIT,"FE1"     }, /* 46: 0100 1000 0100 0110                      */
	{SKIT,"FEIN"    }, /* 47: 0100 1000 0100 0111                      */
	{SKIT,"FAD"     }, /* 48: 0100 1000 0100 1000                      */
	{SKIT,"FSR"     }, /* 49: 0100 1000 0100 1001                      */
	{SKIT,"FST"     }, /* 4a: 0100 1000 0100 1010                      */
	{SKIT,"ER"      }, /* 4b: 0100 1000 0100 1011                      */
	{SKIT,"OV"      }, /* 4c: 0100 1000 0100 1100                      */
	{illegal,nullptr   }, /* 4d: 0100 1000 0100 1101                      */
	{illegal,nullptr   }, /* 4e: 0100 1000 0100 1110                      */
	{illegal,nullptr   }, /* 4f: 0100 1000 0100 1111                      */

	{SKIT,"AN4"     }, /* 50: 0100 1000 0101 0000                      */
	{SKIT,"AN5"     }, /* 51: 0100 1000 0101 0001                      */
	{SKIT,"AN6"     }, /* 52: 0100 1000 0101 0010                      */
	{SKIT,"AN7"     }, /* 53: 0100 1000 0101 0011                      */
	{SKIT,"SB"      }, /* 54: 0100 1000 0101 0100                      */
	{illegal,nullptr   }, /* 55: 0100 1000 0101 0101                      */
	{illegal,nullptr   }, /* 56: 0100 1000 0101 0110                      */
	{illegal,nullptr   }, /* 57: 0100 1000 0101 0111                      */
	{illegal,nullptr   }, /* 58: 0100 1000 0101 1000                      */
	{illegal,nullptr   }, /* 59: 0100 1000 0101 1001                      */
	{illegal,nullptr   }, /* 5a: 0100 1000 0101 1010                      */
	{illegal,nullptr   }, /* 5b: 0100 1000 0101 1011                      */
	{illegal,nullptr   }, /* 5c: 0100 1000 0101 1100                      */
	{illegal,nullptr   }, /* 5d: 0100 1000 0101 1101                      */
	{illegal,nullptr   }, /* 5e: 0100 1000 0101 1110                      */
	{illegal,nullptr   }, /* 5f: 0100 1000 0101 1111                      */

	{SKNIT,"NMI"    }, /* 60: 0100 1000 0110 0000                      */
	{SKNIT,"FT0"    }, /* 61: 0100 1000 0110 0001                      */
	{SKNIT,"FT1"    }, /* 62: 0100 1000 0110 0010                      */
	{SKNIT,"F1"     }, /* 63: 0100 1000 0110 0011                      */
	{SKNIT,"F2"     }, /* 64: 0100 1000 0110 0100                      */
	{SKNIT,"FE0"    }, /* 65: 0100 1000 0110 0101                      */
	{SKNIT,"FE1"    }, /* 66: 0100 1000 0110 0110                      */
	{SKNIT,"FEIN"   }, /* 67: 0100 1000 0110 0111                      */
	{SKNIT,"FAD"    }, /* 68: 0100 1000 0110 1000                      */
	{SKNIT,"FSR"    }, /* 69: 0100 1000 0110 1001                      */
	{SKNIT,"FST"    }, /* 6a: 0100 1000 0110 1010                      */
	{SKNIT,"ER"     }, /* 6b: 0100 1000 0110 1011                      */
	{SKNIT,"OV"     }, /* 6c: 0100 1000 0110 1100                      */
	{illegal,nullptr   }, /* 6d: 0100 1000 0110 1101                      */
	{illegal,nullptr   }, /* 6e: 0100 1000 0110 1110                      */
	{illegal,nullptr   }, /* 6f: 0100 1000 0110 1111                      */

	{SKNIT,"AN4"    }, /* 70: 0100 1000 0111 0000                      */
	{SKNIT,"AN5"    }, /* 71: 0100 1000 0111 0001                      */
	{SKNIT,"AN6"    }, /* 72: 0100 1000 0111 0010                      */
	{SKNIT,"AN7"    }, /* 73: 0100 1000 0111 0011                      */
	{SKNIT,"SB"     }, /* 74: 0100 1000 0111 0100                      */
	{illegal,nullptr   }, /* 75: 0100 1000 0111 0101                      */
	{illegal,nullptr   }, /* 76: 0100 1000 0111 0110                      */
	{illegal,nullptr   }, /* 77: 0100 1000 0111 0111                      */
	{illegal,nullptr   }, /* 78: 0100 1000 0111 1000                      */
	{illegal,nullptr   }, /* 79: 0100 1000 0111 1001                      */
	{illegal,nullptr   }, /* 7a: 0100 1000 0111 1010                      */
	{illegal,nullptr   }, /* 7b: 0100 1000 0111 1011                      */
	{illegal,nullptr   }, /* 7c: 0100 1000 0111 1100                      */
	{illegal,nullptr   }, /* 7d: 0100 1000 0111 1101                      */
	{illegal,nullptr   }, /* 7e: 0100 1000 0111 1110                      */
	{illegal,nullptr   }, /* 7f: 0100 1000 0111 1111                      */

	{illegal,nullptr   }, /* 80: 0100 1000 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0100 1000 1000 0001                      */
	{LDEAX,"(DE)"   }, /* 82: 0100 1000 1000 0010                      */
	{LDEAX,"(HL)"   }, /* 83: 0100 1000 1000 0011                      */
	{LDEAX,"(DE++)" }, /* 84: 0100 1000 1000 0100                      */
	{LDEAX,"(HL++)" }, /* 85: 0100 1000 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0100 1000 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0100 1000 1000 0111                      */
	{illegal,nullptr   }, /* 88: 0100 1000 1000 1000                      */
	{illegal,nullptr   }, /* 89: 0100 1000 1000 1001                      */
	{illegal,nullptr   }, /* 8a: 0100 1000 1000 1010                      */
	{LDEAX,"(DE+%b)"}, /* 8b: 0100 1000 1000 1011 xxxx xxxx            */
	{LDEAX,"(HL+A)" }, /* 8c: 0100 1000 1000 1100                      */
	{LDEAX,"(HL+B)" }, /* 8d: 0100 1000 1000 1101                      */
	{LDEAX,"(HL+EA)"}, /* 8e: 0100 1000 1000 1110                      */
	{LDEAX,"(HL+%b)"}, /* 8f: 0100 1000 1000 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* 90: 0100 1000 1000 0000                      */
	{illegal,nullptr   }, /* 91: 0100 1000 1000 0001                      */
	{STEAX,"(DE)"   }, /* 92: 0100 1000 1000 0010                      */
	{STEAX,"(HL)"   }, /* 93: 0100 1000 1000 0011                      */
	{STEAX,"(DE++)" }, /* 94: 0100 1000 1000 0100                      */
	{STEAX,"(HL++)" }, /* 95: 0100 1000 1000 0101                      */
	{illegal,nullptr   }, /* 96: 0100 1000 1000 0110                      */
	{illegal,nullptr   }, /* 97: 0100 1000 1000 0111                      */
	{illegal,nullptr   }, /* 98: 0100 1000 1000 1000                      */
	{illegal,nullptr   }, /* 99: 0100 1000 1000 1001                      */
	{illegal,nullptr   }, /* 9a: 0100 1000 1000 1010                      */
	{STEAX,"(DE+%b)"}, /* 9b: 0100 1000 1000 1011 xxxx xxxx            */
	{STEAX,"(HL+A)" }, /* 9c: 0100 1000 1000 1100                      */
	{STEAX,"(HL+B)" }, /* 9d: 0100 1000 1000 1101                      */
	{STEAX,"(HL+EA)"}, /* 9e: 0100 1000 1000 1110                      */
	{STEAX,"(HL+%b)"}, /* 9f: 0100 1000 1000 1111 xxxx xxxx            */

	{DSLR,"EA"      }, /* a0: 0100 1000 1010 0000                      */
	{illegal,nullptr   }, /* a1: 0100 1000 1010 0001                      */
	{illegal,nullptr   }, /* a2: 0100 1000 1010 0010                      */
	{illegal,nullptr   }, /* a3: 0100 1000 1010 0011                      */
	{DSLL,"EA"      }, /* a4: 0100 1000 1010 0100                      */
	{illegal,nullptr   }, /* a5: 0100 1000 1010 0101                      */
	{illegal,nullptr   }, /* a6: 0100 1000 1010 0110                      */
	{illegal,nullptr   }, /* a7: 0100 1000 1010 0111                      */
	{TABLE,nullptr         }, /* a8: 0100 1000 1010 1000                      */
	{illegal,nullptr   }, /* a9: 0100 1000 1010 1001                      */
	{illegal,nullptr   }, /* aa: 0100 1000 1010 1010                      */
	{illegal,nullptr   }, /* ab: 0100 1000 1010 1011                      */
	{illegal,nullptr   }, /* ac: 0100 1000 1010 1100                      */
	{illegal,nullptr   }, /* ad: 0100 1000 1010 1101                      */
	{illegal,nullptr   }, /* ae: 0100 1000 1010 1110                      */
	{illegal,nullptr   }, /* af: 0100 1000 1010 1111                      */

	{DRLR,"EA"      }, /* b0: 0100 1000 1011 0000                      */
	{illegal,nullptr   }, /* b1: 0100 1000 1011 0001                      */
	{illegal,nullptr   }, /* b2: 0100 1000 1011 0010                      */
	{illegal,nullptr   }, /* b3: 0100 1000 1011 0011                      */
	{DRLL,"EA"      }, /* b4: 0100 1000 1011 0100                      */
	{illegal,nullptr   }, /* b5: 0100 1000 1011 0101                      */
	{illegal,nullptr   }, /* b6: 0100 1000 1011 0110                      */
	{illegal,nullptr   }, /* b7: 0100 1000 1011 0111                      */
	{illegal,nullptr   }, /* b8: 0100 1000 1011 1000                      */
	{illegal,nullptr   }, /* b9: 0100 1000 1011 1001                      */
	{illegal,nullptr   }, /* ba: 0100 1000 1011 1010                      */
	{STOP,nullptr      }, /* bb: 0100 1000 1011 1011                      */  /* 7810 */
	{illegal,nullptr   }, /* bc: 0100 1000 1011 1100                      */
	{illegal,nullptr   }, /* bd: 0100 1000 1011 1101                      */
	{illegal,nullptr   }, /* be: 0100 1000 1011 1110                      */
	{illegal,nullptr   }, /* bf: 0100 1000 1011 1111                      */

	{DMOV,"EA,ECNT" }, /* c0: 0100 1000 1100 0000                      */
	{DMOV,"EA,ECPT" }, /* c1: 0100 1000 1100 0001                      */   /* 7810 */
	{illegal,nullptr   }, /* c2: 0100 1000 1100 0010                      */
	{illegal,nullptr   }, /* c3: 0100 1000 1100 0011                      */
	{illegal,nullptr   }, /* c4: 0100 1000 1100 0100                      */
	{illegal,nullptr   }, /* c5: 0100 1000 1100 0101                      */
	{illegal,nullptr   }, /* c6: 0100 1000 1100 0110                      */
	{illegal,nullptr   }, /* c7: 0100 1000 1100 0111                      */
	{illegal,nullptr   }, /* c8: 0100 1000 1100 1000                      */
	{illegal,nullptr   }, /* c9: 0100 1000 1100 1001                      */
	{illegal,nullptr   }, /* ca: 0100 1000 1100 1010                      */
	{illegal,nullptr   }, /* cb: 0100 1000 1100 1011                      */
	{illegal,nullptr   }, /* cc: 0100 1000 1100 1100                      */
	{illegal,nullptr   }, /* cd: 0100 1000 1100 1101                      */
	{illegal,nullptr   }, /* ce: 0100 1000 1100 1110                      */
	{illegal,nullptr   }, /* cf: 0100 1000 1100 1111                      */

	{illegal,nullptr   }, /* d0: 0100 1000 1101 0000                      */
	{illegal,nullptr   }, /* d1: 0100 1000 1101 0001                      */
	{DMOV,"ETM0,EA" }, /* d2: 0100 1000 1101 0010                      */
	{DMOV,"ETM1,EA" }, /* d3: 0100 1000 1101 0011                      */
	{illegal,nullptr   }, /* d4: 0100 1000 1101 0100                      */
	{illegal,nullptr   }, /* d5: 0100 1000 1101 0101                      */
	{illegal,nullptr   }, /* d6: 0100 1000 1101 0110                      */
	{illegal,nullptr   }, /* d7: 0100 1000 1101 0111                      */
	{illegal,nullptr   }, /* d8: 0100 1000 1101 1000                      */
	{illegal,nullptr   }, /* d9: 0100 1000 1101 1001                      */
	{illegal,nullptr   }, /* da: 0100 1000 1101 1010                      */
	{illegal,nullptr   }, /* db: 0100 1000 1101 1011                      */
	{illegal,nullptr   }, /* dc: 0100 1000 1101 1100                      */
	{illegal,nullptr   }, /* dd: 0100 1000 1101 1101                      */
	{illegal,nullptr   }, /* de: 0100 1000 1101 1110                      */
	{illegal,nullptr   }, /* df: 0100 1000 1101 1111                      */

	{illegal,nullptr   }, /* e0: 0100 1000 1110 0000                      */
	{illegal,nullptr   }, /* e1: 0100 1000 1110 0001                      */
	{illegal,nullptr   }, /* e2: 0100 1000 1110 0010                      */
	{illegal,nullptr   }, /* e3: 0100 1000 1110 0011                      */
	{illegal,nullptr   }, /* e4: 0100 1000 1110 0100                      */
	{illegal,nullptr   }, /* e5: 0100 1000 1110 0101                      */
	{illegal,nullptr   }, /* e6: 0100 1000 1110 0110                      */
	{illegal,nullptr   }, /* e7: 0100 1000 1110 0111                      */
	{illegal,nullptr   }, /* e8: 0100 1000 1110 1000                      */
	{illegal,nullptr   }, /* e9: 0100 1000 1110 1001                      */
	{illegal,nullptr   }, /* ea: 0100 1000 1110 1010                      */
	{illegal,nullptr   }, /* eb: 0100 1000 1110 1011                      */
	{illegal,nullptr   }, /* ec: 0100 1000 1110 1100                      */
	{illegal,nullptr   }, /* ed: 0100 1000 1110 1101                      */
	{illegal,nullptr   }, /* ee: 0100 1000 1110 1110                      */
	{illegal,nullptr   }, /* ef: 0100 1000 1110 1111                      */

	{illegal,nullptr   }, /* f0: 0100 1000 1111 0000                      */
	{illegal,nullptr   }, /* f1: 0100 1000 1111 0001                      */
	{illegal,nullptr   }, /* f2: 0100 1000 1111 0010                      */
	{illegal,nullptr   }, /* f3: 0100 1000 1111 0011                      */
	{illegal,nullptr   }, /* f4: 0100 1000 1111 0100                      */
	{illegal,nullptr   }, /* f5: 0100 1000 1111 0101                      */
	{illegal,nullptr   }, /* f6: 0100 1000 1111 0110                      */
	{illegal,nullptr   }, /* f7: 0100 1000 1111 0111                      */
	{illegal,nullptr   }, /* f8: 0100 1000 1111 1000                      */
	{illegal,nullptr   }, /* f9: 0100 1000 1111 1001                      */
	{illegal,nullptr   }, /* fa: 0100 1000 1111 1010                      */
	{illegal,nullptr   }, /* fb: 0100 1000 1111 1011                      */
	{illegal,nullptr   }, /* fc: 0100 1000 1111 1100                      */
	{illegal,nullptr   }, /* fd: 0100 1000 1111 1101                      */
	{illegal,nullptr   }, /* fe: 0100 1000 1111 1110                      */
	{illegal,nullptr   }  /* ff: 0100 1000 1111 1111                      */
};

static const struct dasm_s dasm48_7807[256] =
{
	{illegal,nullptr   }, /* 00: 0100 1000 0000 0000                      */
	{SLRC,"A"       }, /* 01: 0100 1000 0000 0001                      */
	{SLRC,"B"       }, /* 02: 0100 1000 0000 0010                      */
	{SLRC,"C"       }, /* 03: 0100 1000 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0100 1000 0000 0100                      */
	{SLLC,"A"       }, /* 05: 0100 1000 0000 0101                      */
	{SLLC,"B"       }, /* 06: 0100 1000 0000 0110                      */
	{SLLC,"C"       }, /* 07: 0100 1000 0000 0111                      */
	{SK,"NV"        }, /* 08: 0100 1000 0000 1000                      */
	{illegal,nullptr   }, /* 09: 0100 1000 0000 1001                      */
	{SK,"CY"        }, /* 0a: 0100 1000 0000 1010                      */
	{SK,"HC"        }, /* 0b: 0100 1000 0000 1011                      */
	{SK,"Z"         }, /* 0c: 0100 1000 0000 1100                      */
	{illegal,nullptr   }, /* 0d: 0100 1000 0000 1101                      */
	{illegal,nullptr   }, /* 0e: 0100 1000 0000 1110                      */
	{illegal,nullptr   }, /* 0f: 0100 1000 0000 1111                      */

	{illegal,nullptr   }, /* 10: 0100 1000 0001 0000                      */
	{illegal,nullptr   }, /* 11: 0100 1000 0001 0001                      */
	{illegal,nullptr   }, /* 12: 0100 1000 0001 0010                      */
	{illegal,nullptr   }, /* 13: 0100 1000 0001 0011                      */
	{illegal,nullptr   }, /* 14: 0100 1000 0001 0100                      */
	{illegal,nullptr   }, /* 15: 0100 1000 0001 0101                      */
	{illegal,nullptr   }, /* 16: 0100 1000 0001 0110                      */
	{illegal,nullptr   }, /* 17: 0100 1000 0001 0111                      */
	{SKN,"NV"       }, /* 18: 0100 1000 0001 1000                      */
	{illegal,nullptr   }, /* 19: 0100 1000 0001 1001                      */
	{SKN,"CY"       }, /* 1a: 0100 1000 0001 1010                      */
	{SKN,"HC"       }, /* 1b: 0100 1000 0001 1011                      */
	{SKN,"Z"        }, /* 1c: 0100 1000 0001 1100                      */
	{illegal,nullptr   }, /* 1d: 0100 1000 0001 1101                      */
	{illegal,nullptr   }, /* 1e: 0100 1000 0001 1110                      */
	{illegal,nullptr   }, /* 1f: 0100 1000 0001 1111                      */

	{illegal,nullptr   }, /* 20: 0100 1000 0010 0000                      */
	{SLR,"A"        }, /* 21: 0100 1000 0010 0001                      */
	{SLR,"B"        }, /* 22: 0100 1000 0010 0010                      */
	{SLR,"C"        }, /* 23: 0100 1000 0010 0011                      */
	{illegal,nullptr   }, /* 24: 0100 1000 0010 0100                      */
	{SLL,"A"        }, /* 25: 0100 1000 0010 0101                      */
	{SLL,"B"        }, /* 26: 0100 1000 0010 0110                      */
	{SLL,"C"        }, /* 27: 0100 1000 0010 0111                      */
	{JEA,nullptr       }, /* 28: 0100 1000 0010 1000                      */
	{CALB,nullptr      }, /* 29: 0100 1000 0010 1001                      */
	{CLC,nullptr       }, /* 2a: 0100 1000 0010 1010                      */
	{STC,nullptr       }, /* 2b: 0100 1000 0010 1011                      */
	{illegal,nullptr   }, /* 2c: 0100 1000 0010 1100                      */
	{MUL,"A"        }, /* 2d: 0100 1000 0010 1101                      */
	{MUL,"B"        }, /* 2e: 0100 1000 0010 1110                      */
	{MUL,"C"        }, /* 2f: 0100 1000 0010 1111                      */

	{illegal,nullptr   }, /* 30: 0100 1000 0011 0000                      */
	{RLR,"A"        }, /* 31: 0100 1000 0011 0001                      */
	{RLR,"B"        }, /* 32: 0100 1000 0011 0010                      */
	{RLR,"C"        }, /* 33: 0100 1000 0011 0011                      */
	{illegal,nullptr   }, /* 34: 0100 1000 0011 0100                      */
	{RLL,"A"        }, /* 35: 0100 1000 0011 0101                      */
	{RLL,"B"        }, /* 36: 0100 1000 0011 0110                      */
	{RLL,"C"        }, /* 37: 0100 1000 0011 0111                      */
	{RLD,nullptr       }, /* 38: 0100 1000 0011 1000                      */
	{RRD,nullptr       }, /* 39: 0100 1000 0011 1001                      */
	{NEGA,nullptr      }, /* 3a: 0100 1000 0011 1010                      */
	{HALT,nullptr      }, /* 3b: 0100 1000 0011 1011                      */
	{illegal,nullptr   }, /* 3c: 0100 1000 0011 1100                      */
	{DIV,"A"        }, /* 3d: 0100 1000 0011 1101                      */
	{DIV,"B"        }, /* 3e: 0100 1000 0011 1110                      */
	{DIV,"C"        }, /* 3f: 0100 1000 0011 1111                      */

	{SKIT,"NMI"     }, /* 40: 0100 1000 0100 0000                      */
	{SKIT,"FT0"     }, /* 41: 0100 1000 0100 0001                      */
	{SKIT,"FT1"     }, /* 42: 0100 1000 0100 0010                      */
	{SKIT,"F1"      }, /* 43: 0100 1000 0100 0011                      */
	{SKIT,"F2"      }, /* 44: 0100 1000 0100 0100                      */
	{SKIT,"FE0"     }, /* 45: 0100 1000 0100 0101                      */
	{SKIT,"FE1"     }, /* 46: 0100 1000 0100 0110                      */
	{SKIT,"FEIN"    }, /* 47: 0100 1000 0100 0111                      */
	{SKIT,"FAD"     }, /* 48: 0100 1000 0100 1000                      */
	{SKIT,"FSR"     }, /* 49: 0100 1000 0100 1001                      */
	{SKIT,"FST"     }, /* 4a: 0100 1000 0100 1010                      */
	{SKIT,"ER"      }, /* 4b: 0100 1000 0100 1011                      */
	{SKIT,"OV"      }, /* 4c: 0100 1000 0100 1100                      */
	{illegal,nullptr   }, /* 4d: 0100 1000 0100 1101                      */
	{illegal,nullptr   }, /* 4e: 0100 1000 0100 1110                      */
	{illegal,nullptr   }, /* 4f: 0100 1000 0100 1111                      */

	{SKIT,"AN4"     }, /* 50: 0100 1000 0101 0000                      */
	{SKIT,"AN5"     }, /* 51: 0100 1000 0101 0001                      */
	{SKIT,"AN6"     }, /* 52: 0100 1000 0101 0010                      */
	{SKIT,"AN7"     }, /* 53: 0100 1000 0101 0011                      */
	{SKIT,"SB"      }, /* 54: 0100 1000 0101 0100                      */
	{illegal,nullptr   }, /* 55: 0100 1000 0101 0101                      */
	{illegal,nullptr   }, /* 56: 0100 1000 0101 0110                      */
	{illegal,nullptr   }, /* 57: 0100 1000 0101 0111                      */
	{illegal,nullptr   }, /* 58: 0100 1000 0101 1000                      */
	{illegal,nullptr   }, /* 59: 0100 1000 0101 1001                      */
	{illegal,nullptr   }, /* 5a: 0100 1000 0101 1010                      */
	{illegal,nullptr   }, /* 5b: 0100 1000 0101 1011                      */
	{illegal,nullptr   }, /* 5c: 0100 1000 0101 1100                      */
	{illegal,nullptr   }, /* 5d: 0100 1000 0101 1101                      */
	{illegal,nullptr   }, /* 5e: 0100 1000 0101 1110                      */
	{illegal,nullptr   }, /* 5f: 0100 1000 0101 1111                      */

	{SKNIT,"NMI"    }, /* 60: 0100 1000 0110 0000                      */
	{SKNIT,"FT0"    }, /* 61: 0100 1000 0110 0001                      */
	{SKNIT,"FT1"    }, /* 62: 0100 1000 0110 0010                      */
	{SKNIT,"F1"     }, /* 63: 0100 1000 0110 0011                      */
	{SKNIT,"F2"     }, /* 64: 0100 1000 0110 0100                      */
	{SKNIT,"FE0"    }, /* 65: 0100 1000 0110 0101                      */
	{SKNIT,"FE1"    }, /* 66: 0100 1000 0110 0110                      */
	{SKNIT,"FEIN"   }, /* 67: 0100 1000 0110 0111                      */
	{SKNIT,"FAD"    }, /* 68: 0100 1000 0110 1000                      */
	{SKNIT,"FSR"    }, /* 69: 0100 1000 0110 1001                      */
	{SKNIT,"FST"    }, /* 6a: 0100 1000 0110 1010                      */
	{SKNIT,"ER"     }, /* 6b: 0100 1000 0110 1011                      */
	{SKNIT,"OV"     }, /* 6c: 0100 1000 0110 1100                      */
	{illegal,nullptr   }, /* 6d: 0100 1000 0110 1101                      */
	{illegal,nullptr   }, /* 6e: 0100 1000 0110 1110                      */
	{illegal,nullptr   }, /* 6f: 0100 1000 0110 1111                      */

	{SKNIT,"AN4"    }, /* 70: 0100 1000 0111 0000                      */
	{SKNIT,"AN5"    }, /* 71: 0100 1000 0111 0001                      */
	{SKNIT,"AN6"    }, /* 72: 0100 1000 0111 0010                      */
	{SKNIT,"AN7"    }, /* 73: 0100 1000 0111 0011                      */
	{SKNIT,"SB"     }, /* 74: 0100 1000 0111 0100                      */
	{illegal,nullptr   }, /* 75: 0100 1000 0111 0101                      */
	{illegal,nullptr   }, /* 76: 0100 1000 0111 0110                      */
	{illegal,nullptr   }, /* 77: 0100 1000 0111 0111                      */
	{illegal,nullptr   }, /* 78: 0100 1000 0111 1000                      */
	{illegal,nullptr   }, /* 79: 0100 1000 0111 1001                      */
	{illegal,nullptr   }, /* 7a: 0100 1000 0111 1010                      */
	{illegal,nullptr   }, /* 7b: 0100 1000 0111 1011                      */
	{illegal,nullptr   }, /* 7c: 0100 1000 0111 1100                      */
	{illegal,nullptr   }, /* 7d: 0100 1000 0111 1101                      */
	{illegal,nullptr   }, /* 7e: 0100 1000 0111 1110                      */
	{illegal,nullptr   }, /* 7f: 0100 1000 0111 1111                      */

	{illegal,nullptr   }, /* 80: 0100 1000 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0100 1000 1000 0001                      */
	{LDEAX,"(DE)"   }, /* 82: 0100 1000 1000 0010                      */
	{LDEAX,"(HL)"   }, /* 83: 0100 1000 1000 0011                      */
	{LDEAX,"(DE++)" }, /* 84: 0100 1000 1000 0100                      */
	{LDEAX,"(HL++)" }, /* 85: 0100 1000 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0100 1000 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0100 1000 1000 0111                      */
	{illegal,nullptr   }, /* 88: 0100 1000 1000 1000                      */
	{illegal,nullptr   }, /* 89: 0100 1000 1000 1001                      */
	{illegal,nullptr   }, /* 8a: 0100 1000 1000 1010                      */
	{LDEAX,"(DE+%b)"}, /* 8b: 0100 1000 1000 1011 xxxx xxxx            */
	{LDEAX,"(HL+A)" }, /* 8c: 0100 1000 1000 1100                      */
	{LDEAX,"(HL+B)" }, /* 8d: 0100 1000 1000 1101                      */
	{LDEAX,"(HL+EA)"}, /* 8e: 0100 1000 1000 1110                      */
	{LDEAX,"(HL+%b)"}, /* 8f: 0100 1000 1000 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* 90: 0100 1000 1000 0000                      */
	{illegal,nullptr   }, /* 91: 0100 1000 1000 0001                      */
	{STEAX,"(DE)"   }, /* 92: 0100 1000 1000 0010                      */
	{STEAX,"(HL)"   }, /* 93: 0100 1000 1000 0011                      */
	{STEAX,"(DE++)" }, /* 94: 0100 1000 1000 0100                      */
	{STEAX,"(HL++)" }, /* 95: 0100 1000 1000 0101                      */
	{illegal,nullptr   }, /* 96: 0100 1000 1000 0110                      */
	{illegal,nullptr   }, /* 97: 0100 1000 1000 0111                      */
	{illegal,nullptr   }, /* 98: 0100 1000 1000 1000                      */
	{illegal,nullptr   }, /* 99: 0100 1000 1000 1001                      */
	{illegal,nullptr   }, /* 9a: 0100 1000 1000 1010                      */
	{STEAX,"(DE+%b)"}, /* 9b: 0100 1000 1000 1011 xxxx xxxx            */
	{STEAX,"(HL+A)" }, /* 9c: 0100 1000 1000 1100                      */
	{STEAX,"(HL+B)" }, /* 9d: 0100 1000 1000 1101                      */
	{STEAX,"(HL+EA)"}, /* 9e: 0100 1000 1000 1110                      */
	{STEAX,"(HL+%b)"}, /* 9f: 0100 1000 1000 1111 xxxx xxxx            */

	{DSLR,"EA"      }, /* a0: 0100 1000 1010 0000                      */
	{illegal,nullptr   }, /* a1: 0100 1000 1010 0001                      */
	{illegal,nullptr   }, /* a2: 0100 1000 1010 0010                      */
	{illegal,nullptr   }, /* a3: 0100 1000 1010 0011                      */
	{DSLL,"EA"      }, /* a4: 0100 1000 1010 0100                      */
	{illegal,nullptr   }, /* a5: 0100 1000 1010 0101                      */
	{illegal,nullptr   }, /* a6: 0100 1000 1010 0110                      */
	{illegal,nullptr   }, /* a7: 0100 1000 1010 0111                      */
	{TABLE,nullptr         }, /* a8: 0100 1000 1010 1000                      */
	{illegal,nullptr   }, /* a9: 0100 1000 1010 1001                      */
	{CMC,nullptr       }, /* aa: 0100 1000 1010 1010                      */  /* 7807 */
	{illegal,nullptr   }, /* ab: 0100 1000 1010 1011                      */
	{EXA,nullptr       }, /* ac: 0100 1000 1010 1100                      */  /* 7807 */
	{EXR,nullptr       }, /* ad: 0100 1000 1010 1101                      */  /* 7807 */
	{EXH,nullptr       }, /* ae: 0100 1000 1010 1110                      */  /* 7807 */
	{EXX,nullptr       }, /* af: 0100 1000 1010 1111                      */  /* 7807 */

	{DRLR,"EA"      }, /* b0: 0100 1000 1011 0000                      */
	{illegal,nullptr   }, /* b1: 0100 1000 1011 0001                      */
	{illegal,nullptr   }, /* b2: 0100 1000 1011 0010                      */
	{illegal,nullptr   }, /* b3: 0100 1000 1011 0011                      */
	{DRLL,"EA"      }, /* b4: 0100 1000 1011 0100                      */
	{illegal,nullptr   }, /* b5: 0100 1000 1011 0101                      */
	{illegal,nullptr   }, /* b6: 0100 1000 1011 0110                      */
	{illegal,nullptr   }, /* b7: 0100 1000 1011 0111                      */
	{illegal,nullptr   }, /* b8: 0100 1000 1011 1000                      */
	{illegal,nullptr   }, /* b9: 0100 1000 1011 1001                      */
	{illegal,nullptr   }, /* ba: 0100 1000 1011 1010                      */
	{illegal,nullptr       }, /* bb: 0100 1000 1011 1011                      */
	{illegal,nullptr   }, /* bc: 0100 1000 1011 1100                      */
	{illegal,nullptr   }, /* bd: 0100 1000 1011 1101                      */
	{illegal,nullptr   }, /* be: 0100 1000 1011 1110                      */
	{illegal,nullptr   }, /* bf: 0100 1000 1011 1111                      */

	{DMOV,"EA,ECNT" }, /* c0: 0100 1000 1100 0000                      */
	{DMOV,"EA,ECPT0"}, /* c1: 0100 1000 1100 0001                      */   /* 7807 */
	{DMOV,"EA,ECPT1"}, /* c2: 0100 1000 1100 0010                      */   /* 7807 */
	{illegal,nullptr   }, /* c3: 0100 1000 1100 0011                      */
	{illegal,nullptr   }, /* c4: 0100 1000 1100 0100                      */
	{illegal,nullptr   }, /* c5: 0100 1000 1100 0101                      */
	{illegal,nullptr   }, /* c6: 0100 1000 1100 0110                      */
	{illegal,nullptr   }, /* c7: 0100 1000 1100 0111                      */
	{illegal,nullptr   }, /* c8: 0100 1000 1100 1000                      */
	{illegal,nullptr   }, /* c9: 0100 1000 1100 1001                      */
	{illegal,nullptr   }, /* ca: 0100 1000 1100 1010                      */
	{illegal,nullptr   }, /* cb: 0100 1000 1100 1011                      */
	{illegal,nullptr   }, /* cc: 0100 1000 1100 1100                      */
	{illegal,nullptr   }, /* cd: 0100 1000 1100 1101                      */
	{illegal,nullptr   }, /* ce: 0100 1000 1100 1110                      */
	{illegal,nullptr   }, /* cf: 0100 1000 1100 1111                      */

	{illegal,nullptr   }, /* d0: 0100 1000 1101 0000                      */
	{illegal,nullptr   }, /* d1: 0100 1000 1101 0001                      */
	{DMOV,"ETM0,EA" }, /* d2: 0100 1000 1101 0010                      */
	{DMOV,"ETM1,EA" }, /* d3: 0100 1000 1101 0011                      */
	{illegal,nullptr   }, /* d4: 0100 1000 1101 0100                      */
	{illegal,nullptr   }, /* d5: 0100 1000 1101 0101                      */
	{illegal,nullptr   }, /* d6: 0100 1000 1101 0110                      */
	{illegal,nullptr   }, /* d7: 0100 1000 1101 0111                      */
	{illegal,nullptr   }, /* d8: 0100 1000 1101 1000                      */
	{illegal,nullptr   }, /* d9: 0100 1000 1101 1001                      */
	{illegal,nullptr   }, /* da: 0100 1000 1101 1010                      */
	{illegal,nullptr   }, /* db: 0100 1000 1101 1011                      */
	{illegal,nullptr   }, /* dc: 0100 1000 1101 1100                      */
	{illegal,nullptr   }, /* dd: 0100 1000 1101 1101                      */
	{illegal,nullptr   }, /* de: 0100 1000 1101 1110                      */
	{illegal,nullptr   }, /* df: 0100 1000 1101 1111                      */

	{illegal,nullptr   }, /* e0: 0100 1000 1110 0000                      */
	{illegal,nullptr   }, /* e1: 0100 1000 1110 0001                      */
	{illegal,nullptr   }, /* e2: 0100 1000 1110 0010                      */
	{illegal,nullptr   }, /* e3: 0100 1000 1110 0011                      */
	{illegal,nullptr   }, /* e4: 0100 1000 1110 0100                      */
	{illegal,nullptr   }, /* e5: 0100 1000 1110 0101                      */
	{illegal,nullptr   }, /* e6: 0100 1000 1110 0110                      */
	{illegal,nullptr   }, /* e7: 0100 1000 1110 0111                      */
	{illegal,nullptr   }, /* e8: 0100 1000 1110 1000                      */
	{illegal,nullptr   }, /* e9: 0100 1000 1110 1001                      */
	{illegal,nullptr   }, /* ea: 0100 1000 1110 1010                      */
	{illegal,nullptr   }, /* eb: 0100 1000 1110 1011                      */
	{illegal,nullptr   }, /* ec: 0100 1000 1110 1100                      */
	{illegal,nullptr   }, /* ed: 0100 1000 1110 1101                      */
	{illegal,nullptr   }, /* ee: 0100 1000 1110 1110                      */
	{illegal,nullptr   }, /* ef: 0100 1000 1110 1111                      */

	{illegal,nullptr   }, /* f0: 0100 1000 1111 0000                      */
	{illegal,nullptr   }, /* f1: 0100 1000 1111 0001                      */
	{illegal,nullptr   }, /* f2: 0100 1000 1111 0010                      */
	{illegal,nullptr   }, /* f3: 0100 1000 1111 0011                      */
	{illegal,nullptr   }, /* f4: 0100 1000 1111 0100                      */
	{illegal,nullptr   }, /* f5: 0100 1000 1111 0101                      */
	{illegal,nullptr   }, /* f6: 0100 1000 1111 0110                      */
	{illegal,nullptr   }, /* f7: 0100 1000 1111 0111                      */
	{illegal,nullptr   }, /* f8: 0100 1000 1111 1000                      */
	{illegal,nullptr   }, /* f9: 0100 1000 1111 1001                      */
	{illegal,nullptr   }, /* fa: 0100 1000 1111 1010                      */
	{illegal,nullptr   }, /* fb: 0100 1000 1111 1011                      */
	{illegal,nullptr   }, /* fc: 0100 1000 1111 1100                      */
	{illegal,nullptr   }, /* fd: 0100 1000 1111 1101                      */
	{illegal,nullptr   }, /* fe: 0100 1000 1111 1110                      */
	{illegal,nullptr   }  /* ff: 0100 1000 1111 1111                      */
};

/* prefix 4C */
static const struct dasm_s  dasm4C_7810[256] =
{
	{illegal,nullptr   }, /* 00: 0100 1100 0000 0000                      */
	{illegal,nullptr   }, /* 01: 0100 1100 0000 0001                      */
	{illegal,nullptr   }, /* 02: 0100 1100 0000 0010                      */
	{illegal,nullptr   }, /* 03: 0100 1100 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0100 1100 0000 0100                      */
	{illegal,nullptr   }, /* 05: 0100 1100 0000 0101                      */
	{illegal,nullptr   }, /* 06: 0100 1100 0000 0110                      */
	{illegal,nullptr   }, /* 07: 0100 1100 0000 0111                      */
	{illegal,nullptr   }, /* 08: 0100 1100 0000 1000                      */
	{illegal,nullptr   }, /* 09: 0100 1100 0000 1001                      */
	{illegal,nullptr   }, /* 0a: 0100 1100 0000 1010                      */
	{illegal,nullptr   }, /* 0b: 0100 1100 0000 1011                      */
	{illegal,nullptr   }, /* 0c: 0100 1100 0000 1100                      */
	{illegal,nullptr   }, /* 0d: 0100 1100 0000 1101                      */
	{illegal,nullptr   }, /* 0e: 0100 1100 0000 1110                      */
	{illegal,nullptr   }, /* 0f: 0100 1100 0000 1111                      */

	{illegal,nullptr   }, /* 10: 0100 1100 0001 0000                      */
	{illegal,nullptr   }, /* 11: 0100 1100 0001 0001                      */
	{illegal,nullptr   }, /* 12: 0100 1100 0001 0010                      */
	{illegal,nullptr   }, /* 13: 0100 1100 0001 0011                      */
	{illegal,nullptr   }, /* 14: 0100 1100 0001 0100                      */
	{illegal,nullptr   }, /* 15: 0100 1100 0001 0101                      */
	{illegal,nullptr   }, /* 16: 0100 1100 0001 0110                      */
	{illegal,nullptr   }, /* 17: 0100 1100 0001 0111                      */
	{illegal,nullptr   }, /* 18: 0100 1100 0001 1000                      */
	{illegal,nullptr   }, /* 19: 0100 1100 0001 1001                      */
	{illegal,nullptr   }, /* 1a: 0100 1100 0001 1010                      */
	{illegal,nullptr   }, /* 1b: 0100 1100 0001 1011                      */
	{illegal,nullptr   }, /* 1c: 0100 1100 0001 1100                      */
	{illegal,nullptr   }, /* 1d: 0100 1100 0001 1101                      */
	{illegal,nullptr   }, /* 1e: 0100 1100 0001 1110                      */
	{illegal,nullptr   }, /* 1f: 0100 1100 0001 1111                      */

	{illegal,nullptr   }, /* 20: 0100 1100 0010 0000                      */
	{illegal,nullptr   }, /* 21: 0100 1100 0010 0001                      */
	{illegal,nullptr   }, /* 22: 0100 1100 0010 0010                      */
	{illegal,nullptr   }, /* 23: 0100 1100 0010 0011                      */
	{illegal,nullptr   }, /* 24: 0100 1100 0010 0100                      */
	{illegal,nullptr   }, /* 25: 0100 1100 0010 0101                      */
	{illegal,nullptr   }, /* 26: 0100 1100 0010 0110                      */
	{illegal,nullptr   }, /* 27: 0100 1100 0010 0111                      */
	{illegal,nullptr   }, /* 28: 0100 1100 0010 1000                      */
	{illegal,nullptr   }, /* 29: 0100 1100 0010 1001                      */
	{illegal,nullptr   }, /* 2a: 0100 1100 0010 1010                      */
	{illegal,nullptr   }, /* 2b: 0100 1100 0010 1011                      */
	{illegal,nullptr   }, /* 2c: 0100 1100 0010 1100                      */
	{illegal,nullptr   }, /* 2d: 0100 1100 0010 1101                      */
	{illegal,nullptr   }, /* 2e: 0100 1100 0010 1110                      */
	{illegal,nullptr   }, /* 2f: 0100 1100 0010 1111                      */

	{illegal,nullptr   }, /* 30: 0100 1100 0011 0000                      */
	{illegal,nullptr   }, /* 31: 0100 1100 0011 0001                      */
	{illegal,nullptr   }, /* 32: 0100 1100 0011 0010                      */
	{illegal,nullptr   }, /* 33: 0100 1100 0011 0011                      */
	{illegal,nullptr   }, /* 34: 0100 1100 0011 0100                      */
	{illegal,nullptr   }, /* 35: 0100 1100 0011 0101                      */
	{illegal,nullptr   }, /* 36: 0100 1100 0011 0110                      */
	{illegal,nullptr   }, /* 37: 0100 1100 0011 0111                      */
	{illegal,nullptr   }, /* 38: 0100 1100 0011 1000                      */
	{illegal,nullptr   }, /* 39: 0100 1100 0011 1001                      */
	{illegal,nullptr   }, /* 3a: 0100 1100 0011 1010                      */
	{illegal,nullptr   }, /* 3b: 0100 1100 0011 1011                      */
	{illegal,nullptr   }, /* 3c: 0100 1100 0011 1100                      */
	{illegal,nullptr   }, /* 3d: 0100 1100 0011 1101                      */
	{illegal,nullptr   }, /* 3e: 0100 1100 0011 1110                      */
	{illegal,nullptr   }, /* 3f: 0100 1100 0011 1111                      */

	{illegal,nullptr   }, /* 40: 0100 1100 0100 0000                      */
	{illegal,nullptr   }, /* 41: 0100 1100 0100 0001                      */
	{illegal,nullptr   }, /* 42: 0100 1100 0100 0010                      */
	{illegal,nullptr   }, /* 43: 0100 1100 0100 0011                      */
	{illegal,nullptr   }, /* 44: 0100 1100 0100 0100                      */
	{illegal,nullptr   }, /* 45: 0100 1100 0100 0101                      */
	{illegal,nullptr   }, /* 46: 0100 1100 0100 0110                      */
	{illegal,nullptr   }, /* 47: 0100 1100 0100 0111                      */
	{illegal,nullptr   }, /* 48: 0100 1100 0100 1000                      */
	{illegal,nullptr   }, /* 49: 0100 1100 0100 1001                      */
	{illegal,nullptr   }, /* 4a: 0100 1100 0100 1010                      */
	{illegal,nullptr   }, /* 4b: 0100 1100 0100 1011                      */
	{illegal,nullptr   }, /* 4c: 0100 1100 0100 1100                      */
	{illegal,nullptr   }, /* 4d: 0100 1100 0100 1101                      */
	{illegal,nullptr   }, /* 4e: 0100 1100 0100 1110                      */
	{illegal,nullptr   }, /* 4f: 0100 1100 0100 1111                      */

	{illegal,nullptr   }, /* 50: 0100 1100 0101 0000                      */
	{illegal,nullptr   }, /* 51: 0100 1100 0101 0001                      */
	{illegal,nullptr   }, /* 52: 0100 1100 0101 0010                      */
	{illegal,nullptr   }, /* 53: 0100 1100 0101 0011                      */
	{illegal,nullptr   }, /* 54: 0100 1100 0101 0100                      */
	{illegal,nullptr   }, /* 55: 0100 1100 0101 0101                      */
	{illegal,nullptr   }, /* 56: 0100 1100 0101 0110                      */
	{illegal,nullptr   }, /* 57: 0100 1100 0101 0111                      */
	{illegal,nullptr   }, /* 58: 0100 1100 0101 1000                      */
	{illegal,nullptr   }, /* 59: 0100 1100 0101 1001                      */
	{illegal,nullptr   }, /* 5a: 0100 1100 0101 1010                      */
	{illegal,nullptr   }, /* 5b: 0100 1100 0101 1011                      */
	{illegal,nullptr   }, /* 5c: 0100 1100 0101 1100                      */
	{illegal,nullptr   }, /* 5d: 0100 1100 0101 1101                      */
	{illegal,nullptr   }, /* 5e: 0100 1100 0101 1110                      */
	{illegal,nullptr   }, /* 5f: 0100 1100 0101 1111                      */

	{illegal,nullptr   }, /* 60: 0100 1100 0110 0000                      */
	{illegal,nullptr   }, /* 61: 0100 1100 0110 0001                      */
	{illegal,nullptr   }, /* 62: 0100 1100 0110 0010                      */
	{illegal,nullptr   }, /* 63: 0100 1100 0110 0011                      */
	{illegal,nullptr   }, /* 64: 0100 1100 0110 0100                      */
	{illegal,nullptr   }, /* 65: 0100 1100 0110 0101                      */
	{illegal,nullptr   }, /* 66: 0100 1100 0110 0110                      */
	{illegal,nullptr   }, /* 67: 0100 1100 0110 0111                      */
	{illegal,nullptr   }, /* 68: 0100 1100 0110 1000                      */
	{illegal,nullptr   }, /* 69: 0100 1100 0110 1001                      */
	{illegal,nullptr   }, /* 6a: 0100 1100 0110 1010                      */
	{illegal,nullptr   }, /* 6b: 0100 1100 0110 1011                      */
	{illegal,nullptr   }, /* 6c: 0100 1100 0110 1100                      */
	{illegal,nullptr   }, /* 6d: 0100 1100 0110 1101                      */
	{illegal,nullptr   }, /* 6e: 0100 1100 0110 1110                      */
	{illegal,nullptr   }, /* 6f: 0100 1100 0110 1111                      */

	{illegal,nullptr   }, /* 70: 0100 1100 0111 0000                      */
	{illegal,nullptr   }, /* 71: 0100 1100 0111 0001                      */
	{illegal,nullptr   }, /* 72: 0100 1100 0111 0010                      */
	{illegal,nullptr   }, /* 73: 0100 1100 0111 0011                      */
	{illegal,nullptr   }, /* 74: 0100 1100 0111 0100                      */
	{illegal,nullptr   }, /* 75: 0100 1100 0111 0101                      */
	{illegal,nullptr   }, /* 76: 0100 1100 0111 0110                      */
	{illegal,nullptr   }, /* 77: 0100 1100 0111 0111                      */
	{illegal,nullptr   }, /* 78: 0100 1100 0111 1000                      */
	{illegal,nullptr   }, /* 79: 0100 1100 0111 1001                      */
	{illegal,nullptr   }, /* 7a: 0100 1100 0111 1010                      */
	{illegal,nullptr   }, /* 7b: 0100 1100 0111 1011                      */
	{illegal,nullptr   }, /* 7c: 0100 1100 0111 1100                      */
	{illegal,nullptr   }, /* 7d: 0100 1100 0111 1101                      */
	{illegal,nullptr   }, /* 7e: 0100 1100 0111 1110                      */
	{illegal,nullptr   }, /* 7f: 0100 1100 0111 1111                      */

	{illegal,nullptr   }, /* 80: 0100 1100 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0100 1100 1000 0001                      */
	{illegal,nullptr   }, /* 82: 0100 1100 1000 0010                      */
	{illegal,nullptr   }, /* 83: 0100 1100 1000 0011                      */
	{illegal,nullptr   }, /* 84: 0100 1100 1000 0100                      */
	{illegal,nullptr   }, /* 85: 0100 1100 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0100 1100 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0100 1100 1000 0111                      */
	{illegal,nullptr   }, /* 88: 0100 1100 1000 1000                      */
	{illegal,nullptr   }, /* 89: 0100 1100 1000 1001                      */
	{illegal,nullptr   }, /* 8a: 0100 1100 1000 1010                      */
	{illegal,nullptr   }, /* 8b: 0100 1100 1000 1011                      */
	{illegal,nullptr   }, /* 8c: 0100 1100 1000 1100                      */
	{illegal,nullptr   }, /* 8d: 0100 1100 1000 1101                      */
	{illegal,nullptr   }, /* 8e: 0100 1100 1000 1110                      */
	{illegal,nullptr   }, /* 8f: 0100 1100 1000 1111                      */

	{illegal,nullptr   }, /* 90: 0100 1100 1001 0000                      */
	{illegal,nullptr   }, /* 91: 0100 1100 1001 0001                      */
	{illegal,nullptr   }, /* 92: 0100 1100 1001 0010                      */
	{illegal,nullptr   }, /* 93: 0100 1100 1001 0011                      */
	{illegal,nullptr   }, /* 94: 0100 1100 1001 0100                      */
	{illegal,nullptr   }, /* 95: 0100 1100 1001 0101                      */
	{illegal,nullptr   }, /* 96: 0100 1100 1001 0110                      */
	{illegal,nullptr   }, /* 97: 0100 1100 1001 0111                      */
	{illegal,nullptr   }, /* 98: 0100 1100 1001 1000                      */
	{illegal,nullptr   }, /* 99: 0100 1100 1001 1001                      */
	{illegal,nullptr   }, /* 9a: 0100 1100 1001 1010                      */
	{illegal,nullptr   }, /* 9b: 0100 1100 1001 1011                      */
	{illegal,nullptr   }, /* 9c: 0100 1100 1001 1100                      */
	{illegal,nullptr   }, /* 9d: 0100 1100 1001 1101                      */
	{illegal,nullptr   }, /* 9e: 0100 1100 1001 1110                      */
	{illegal,nullptr   }, /* 9f: 0100 1100 1001 1111                      */

	{illegal,nullptr   }, /* a0: 0100 1100 1010 0000                      */
	{illegal,nullptr   }, /* a1: 0100 1100 1010 0001                      */
	{illegal,nullptr   }, /* a2: 0100 1100 1010 0010                      */
	{illegal,nullptr   }, /* a3: 0100 1100 1010 0011                      */
	{illegal,nullptr   }, /* a4: 0100 1100 1010 0100                      */
	{illegal,nullptr   }, /* a5: 0100 1100 1010 0101                      */
	{illegal,nullptr   }, /* a6: 0100 1100 1010 0110                      */
	{illegal,nullptr   }, /* a7: 0100 1100 1010 0111                      */
	{illegal,nullptr   }, /* a8: 0100 1100 1010 1000                      */
	{illegal,nullptr   }, /* a9: 0100 1100 1010 1001                      */
	{illegal,nullptr   }, /* aa: 0100 1100 1010 1010                      */
	{illegal,nullptr   }, /* ab: 0100 1100 1010 1011                      */
	{illegal,nullptr   }, /* ac: 0100 1100 1010 1100                      */
	{illegal,nullptr   }, /* ad: 0100 1100 1010 1101                      */
	{illegal,nullptr   }, /* ae: 0100 1100 1010 1110                      */
	{illegal,nullptr   }, /* af: 0100 1100 1010 1111                      */

	{illegal,nullptr   }, /* b0: 0100 1100 1011 0000                      */
	{illegal,nullptr   }, /* b1: 0100 1100 1011 0001                      */
	{illegal,nullptr   }, /* b2: 0100 1100 1011 0010                      */
	{illegal,nullptr   }, /* b3: 0100 1100 1011 0011                      */
	{illegal,nullptr   }, /* b4: 0100 1100 1011 0100                      */
	{illegal,nullptr   }, /* b5: 0100 1100 1011 0101                      */
	{illegal,nullptr   }, /* b6: 0100 1100 1011 0110                      */
	{illegal,nullptr   }, /* b7: 0100 1100 1011 0111                      */
	{illegal,nullptr   }, /* b8: 0100 1100 1011 1000                      */
	{illegal,nullptr   }, /* b9: 0100 1100 1011 1001                      */
	{illegal,nullptr   }, /* ba: 0100 1100 1011 1010                      */
	{illegal,nullptr   }, /* bb: 0100 1100 1011 1011                      */
	{illegal,nullptr   }, /* bc: 0100 1100 1011 1100                      */
	{illegal,nullptr   }, /* bd: 0100 1100 1011 1101                      */
	{illegal,nullptr   }, /* be: 0100 1100 1011 1110                      */
	{illegal,nullptr   }, /* bf: 0100 1100 1011 1111                      */

	{MOV,"A,PA"     }, /* c0: 0100 1100 1100 0000                      */
	{MOV,"A,PB"     }, /* c1: 0100 1100 1100 0001                      */
	{MOV,"A,PC"     }, /* c2: 0100 1100 1100 0010                      */
	{MOV,"A,PD"     }, /* c3: 0100 1100 1100 0011                      */
	{illegal,nullptr   }, /* c4: 0100 1100 1100 0100                      */
	{MOV,"A,PF"     }, /* c5: 0100 1100 1100 0101                      */
	{MOV,"A,MKH"    }, /* c6: 0100 1100 1100 0110                      */
	{MOV,"A,MKL"    }, /* c7: 0100 1100 1100 0111                      */
	{MOV,"A,ANM"    }, /* c8: 0100 1100 1100 1000                      */   /* 7810 */
	{MOV,"A,SMH"    }, /* c9: 0100 1100 1100 1001                      */
	{illegal,nullptr   }, /* ca: 0100 1100 1100 1010                      */
	{MOV,"A,EOM"    }, /* cb: 0100 1100 1100 1011                      */
	{illegal,nullptr   }, /* cc: 0100 1100 1100 1100                      */
	{MOV,"A,TMM"    }, /* cd: 0100 1100 1100 1101                      */
	{illegal,nullptr   }, /* ce: 0100 1100 1100 1110                      */
	{illegal,nullptr   }, /* cf: 0100 1100 1100 1111                      */

	{illegal,nullptr   }, /* d0: 0100 1100 1101 0000                      */
	{illegal,nullptr   }, /* d1: 0100 1100 1101 0001                      */
	{illegal,nullptr   }, /* d2: 0100 1100 1101 0010                      */
	{illegal,nullptr   }, /* d3: 0100 1100 1101 0011                      */
	{illegal,nullptr   }, /* d4: 0100 1100 1101 0100                      */
	{illegal,nullptr   }, /* d5: 0100 1100 1101 0101                      */
	{illegal,nullptr   }, /* d6: 0100 1100 1101 0110                      */
	{illegal,nullptr   }, /* d7: 0100 1100 1101 0111                      */
	{illegal,nullptr   }, /* d8: 0100 1100 1101 1000                      */
	{MOV,"A,RXB"    }, /* d9: 0100 1100 1101 1001                      */
	{illegal,nullptr   }, /* da: 0100 1100 1101 1010                      */
	{illegal,nullptr   }, /* db: 0100 1100 1101 1011                      */
	{illegal,nullptr   }, /* dc: 0100 1100 1101 1100                      */
	{illegal,nullptr   }, /* dd: 0100 1100 1101 1101                      */
	{illegal,nullptr   }, /* de: 0100 1100 1101 1110                      */
	{illegal,nullptr   }, /* df: 0100 1100 1101 1111                      */

	{MOV,"A,CR0"    }, /* e0: 0100 1100 1110 0000                      */   /* 7810 */
	{MOV,"A,CR1"    }, /* e1: 0100 1100 1110 0001                      */   /* 7810 */
	{MOV,"A,CR2"    }, /* e2: 0100 1100 1110 0010                      */   /* 7810 */
	{MOV,"A,CR3"    }, /* e3: 0100 1100 1110 0011                      */   /* 7810 */
	{illegal,nullptr   }, /* e4: 0100 1100 1110 0100                      */
	{illegal,nullptr   }, /* e5: 0100 1100 1110 0101                      */
	{illegal,nullptr   }, /* e6: 0100 1100 1110 0110                      */
	{illegal,nullptr   }, /* e7: 0100 1100 1110 0111                      */
	{illegal,nullptr   }, /* e8: 0100 1100 1110 1000                      */
	{illegal,nullptr   }, /* e9: 0100 1100 1110 1001                      */
	{illegal,nullptr   }, /* ea: 0100 1100 1110 1010                      */
	{illegal,nullptr   }, /* eb: 0100 1100 1110 1011                      */
	{illegal,nullptr   }, /* ec: 0100 1100 1110 1100                      */
	{illegal,nullptr   }, /* ed: 0100 1100 1110 1101                      */
	{illegal,nullptr   }, /* ee: 0100 1100 1110 1110                      */
	{illegal,nullptr   }, /* ef: 0100 1100 1110 1111                      */

	{illegal,nullptr   }, /* f0: 0100 1100 1111 0000                      */
	{illegal,nullptr   }, /* f1: 0100 1100 1111 0001                      */
	{illegal,nullptr   }, /* f2: 0100 1100 1111 0010                      */
	{illegal,nullptr   }, /* f3: 0100 1100 1111 0011                      */
	{illegal,nullptr   }, /* f4: 0100 1100 1111 0100                      */
	{illegal,nullptr   }, /* f5: 0100 1100 1111 0101                      */
	{illegal,nullptr   }, /* f6: 0100 1100 1111 0110                      */
	{illegal,nullptr   }, /* f7: 0100 1100 1111 0111                      */
	{illegal,nullptr   }, /* f8: 0100 1100 1111 1000                      */
	{illegal,nullptr   }, /* f9: 0100 1100 1111 1001                      */
	{illegal,nullptr   }, /* fa: 0100 1100 1111 1010                      */
	{illegal,nullptr   }, /* fb: 0100 1100 1111 1011                      */
	{illegal,nullptr   }, /* fc: 0100 1100 1111 1100                      */
	{illegal,nullptr   }, /* fd: 0100 1100 1111 1101                      */
	{illegal,nullptr   }, /* fe: 0100 1100 1111 1110                      */
	{illegal,nullptr   }  /* ff: 0100 1100 1111 1111                      */
};

static const struct dasm_s  dasm4C_7807[256] =
{
	{illegal,nullptr   }, /* 00: 0100 1100 0000 0000                      */
	{illegal,nullptr   }, /* 01: 0100 1100 0000 0001                      */
	{illegal,nullptr   }, /* 02: 0100 1100 0000 0010                      */
	{illegal,nullptr   }, /* 03: 0100 1100 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0100 1100 0000 0100                      */
	{illegal,nullptr   }, /* 05: 0100 1100 0000 0101                      */
	{illegal,nullptr   }, /* 06: 0100 1100 0000 0110                      */
	{illegal,nullptr   }, /* 07: 0100 1100 0000 0111                      */
	{illegal,nullptr   }, /* 08: 0100 1100 0000 1000                      */
	{illegal,nullptr   }, /* 09: 0100 1100 0000 1001                      */
	{illegal,nullptr   }, /* 0a: 0100 1100 0000 1010                      */
	{illegal,nullptr   }, /* 0b: 0100 1100 0000 1011                      */
	{illegal,nullptr   }, /* 0c: 0100 1100 0000 1100                      */
	{illegal,nullptr   }, /* 0d: 0100 1100 0000 1101                      */
	{illegal,nullptr   }, /* 0e: 0100 1100 0000 1110                      */
	{illegal,nullptr   }, /* 0f: 0100 1100 0000 1111                      */

	{illegal,nullptr   }, /* 10: 0100 1100 0001 0000                      */
	{illegal,nullptr   }, /* 11: 0100 1100 0001 0001                      */
	{illegal,nullptr   }, /* 12: 0100 1100 0001 0010                      */
	{illegal,nullptr   }, /* 13: 0100 1100 0001 0011                      */
	{illegal,nullptr   }, /* 14: 0100 1100 0001 0100                      */
	{illegal,nullptr   }, /* 15: 0100 1100 0001 0101                      */
	{illegal,nullptr   }, /* 16: 0100 1100 0001 0110                      */
	{illegal,nullptr   }, /* 17: 0100 1100 0001 0111                      */
	{illegal,nullptr   }, /* 18: 0100 1100 0001 1000                      */
	{illegal,nullptr   }, /* 19: 0100 1100 0001 1001                      */
	{illegal,nullptr   }, /* 1a: 0100 1100 0001 1010                      */
	{illegal,nullptr   }, /* 1b: 0100 1100 0001 1011                      */
	{illegal,nullptr   }, /* 1c: 0100 1100 0001 1100                      */
	{illegal,nullptr   }, /* 1d: 0100 1100 0001 1101                      */
	{illegal,nullptr   }, /* 1e: 0100 1100 0001 1110                      */
	{illegal,nullptr   }, /* 1f: 0100 1100 0001 1111                      */

	{illegal,nullptr   }, /* 20: 0100 1100 0010 0000                      */
	{illegal,nullptr   }, /* 21: 0100 1100 0010 0001                      */
	{illegal,nullptr   }, /* 22: 0100 1100 0010 0010                      */
	{illegal,nullptr   }, /* 23: 0100 1100 0010 0011                      */
	{illegal,nullptr   }, /* 24: 0100 1100 0010 0100                      */
	{illegal,nullptr   }, /* 25: 0100 1100 0010 0101                      */
	{illegal,nullptr   }, /* 26: 0100 1100 0010 0110                      */
	{illegal,nullptr   }, /* 27: 0100 1100 0010 0111                      */
	{illegal,nullptr   }, /* 28: 0100 1100 0010 1000                      */
	{illegal,nullptr   }, /* 29: 0100 1100 0010 1001                      */
	{illegal,nullptr   }, /* 2a: 0100 1100 0010 1010                      */
	{illegal,nullptr   }, /* 2b: 0100 1100 0010 1011                      */
	{illegal,nullptr   }, /* 2c: 0100 1100 0010 1100                      */
	{illegal,nullptr   }, /* 2d: 0100 1100 0010 1101                      */
	{illegal,nullptr   }, /* 2e: 0100 1100 0010 1110                      */
	{illegal,nullptr   }, /* 2f: 0100 1100 0010 1111                      */

	{illegal,nullptr   }, /* 30: 0100 1100 0011 0000                      */
	{illegal,nullptr   }, /* 31: 0100 1100 0011 0001                      */
	{illegal,nullptr   }, /* 32: 0100 1100 0011 0010                      */
	{illegal,nullptr   }, /* 33: 0100 1100 0011 0011                      */
	{illegal,nullptr   }, /* 34: 0100 1100 0011 0100                      */
	{illegal,nullptr   }, /* 35: 0100 1100 0011 0101                      */
	{illegal,nullptr   }, /* 36: 0100 1100 0011 0110                      */
	{illegal,nullptr   }, /* 37: 0100 1100 0011 0111                      */
	{illegal,nullptr   }, /* 38: 0100 1100 0011 1000                      */
	{illegal,nullptr   }, /* 39: 0100 1100 0011 1001                      */
	{illegal,nullptr   }, /* 3a: 0100 1100 0011 1010                      */
	{illegal,nullptr   }, /* 3b: 0100 1100 0011 1011                      */
	{illegal,nullptr   }, /* 3c: 0100 1100 0011 1100                      */
	{illegal,nullptr   }, /* 3d: 0100 1100 0011 1101                      */
	{illegal,nullptr   }, /* 3e: 0100 1100 0011 1110                      */
	{illegal,nullptr   }, /* 3f: 0100 1100 0011 1111                      */

	{illegal,nullptr   }, /* 40: 0100 1100 0100 0000                      */
	{illegal,nullptr   }, /* 41: 0100 1100 0100 0001                      */
	{illegal,nullptr   }, /* 42: 0100 1100 0100 0010                      */
	{illegal,nullptr   }, /* 43: 0100 1100 0100 0011                      */
	{illegal,nullptr   }, /* 44: 0100 1100 0100 0100                      */
	{illegal,nullptr   }, /* 45: 0100 1100 0100 0101                      */
	{illegal,nullptr   }, /* 46: 0100 1100 0100 0110                      */
	{illegal,nullptr   }, /* 47: 0100 1100 0100 0111                      */
	{illegal,nullptr   }, /* 48: 0100 1100 0100 1000                      */
	{illegal,nullptr   }, /* 49: 0100 1100 0100 1001                      */
	{illegal,nullptr   }, /* 4a: 0100 1100 0100 1010                      */
	{illegal,nullptr   }, /* 4b: 0100 1100 0100 1011                      */
	{illegal,nullptr   }, /* 4c: 0100 1100 0100 1100                      */
	{illegal,nullptr   }, /* 4d: 0100 1100 0100 1101                      */
	{illegal,nullptr   }, /* 4e: 0100 1100 0100 1110                      */
	{illegal,nullptr   }, /* 4f: 0100 1100 0100 1111                      */

	{illegal,nullptr   }, /* 50: 0100 1100 0101 0000                      */
	{illegal,nullptr   }, /* 51: 0100 1100 0101 0001                      */
	{illegal,nullptr   }, /* 52: 0100 1100 0101 0010                      */
	{illegal,nullptr   }, /* 53: 0100 1100 0101 0011                      */
	{illegal,nullptr   }, /* 54: 0100 1100 0101 0100                      */
	{illegal,nullptr   }, /* 55: 0100 1100 0101 0101                      */
	{illegal,nullptr   }, /* 56: 0100 1100 0101 0110                      */
	{illegal,nullptr   }, /* 57: 0100 1100 0101 0111                      */
	{illegal,nullptr   }, /* 58: 0100 1100 0101 1000                      */
	{illegal,nullptr   }, /* 59: 0100 1100 0101 1001                      */
	{illegal,nullptr   }, /* 5a: 0100 1100 0101 1010                      */
	{illegal,nullptr   }, /* 5b: 0100 1100 0101 1011                      */
	{illegal,nullptr   }, /* 5c: 0100 1100 0101 1100                      */
	{illegal,nullptr   }, /* 5d: 0100 1100 0101 1101                      */
	{illegal,nullptr   }, /* 5e: 0100 1100 0101 1110                      */
	{illegal,nullptr   }, /* 5f: 0100 1100 0101 1111                      */

	{illegal,nullptr   }, /* 60: 0100 1100 0110 0000                      */
	{illegal,nullptr   }, /* 61: 0100 1100 0110 0001                      */
	{illegal,nullptr   }, /* 62: 0100 1100 0110 0010                      */
	{illegal,nullptr   }, /* 63: 0100 1100 0110 0011                      */
	{illegal,nullptr   }, /* 64: 0100 1100 0110 0100                      */
	{illegal,nullptr   }, /* 65: 0100 1100 0110 0101                      */
	{illegal,nullptr   }, /* 66: 0100 1100 0110 0110                      */
	{illegal,nullptr   }, /* 67: 0100 1100 0110 0111                      */
	{illegal,nullptr   }, /* 68: 0100 1100 0110 1000                      */
	{illegal,nullptr   }, /* 69: 0100 1100 0110 1001                      */
	{illegal,nullptr   }, /* 6a: 0100 1100 0110 1010                      */
	{illegal,nullptr   }, /* 6b: 0100 1100 0110 1011                      */
	{illegal,nullptr   }, /* 6c: 0100 1100 0110 1100                      */
	{illegal,nullptr   }, /* 6d: 0100 1100 0110 1101                      */
	{illegal,nullptr   }, /* 6e: 0100 1100 0110 1110                      */
	{illegal,nullptr   }, /* 6f: 0100 1100 0110 1111                      */

	{illegal,nullptr   }, /* 70: 0100 1100 0111 0000                      */
	{illegal,nullptr   }, /* 71: 0100 1100 0111 0001                      */
	{illegal,nullptr   }, /* 72: 0100 1100 0111 0010                      */
	{illegal,nullptr   }, /* 73: 0100 1100 0111 0011                      */
	{illegal,nullptr   }, /* 74: 0100 1100 0111 0100                      */
	{illegal,nullptr   }, /* 75: 0100 1100 0111 0101                      */
	{illegal,nullptr   }, /* 76: 0100 1100 0111 0110                      */
	{illegal,nullptr   }, /* 77: 0100 1100 0111 0111                      */
	{illegal,nullptr   }, /* 78: 0100 1100 0111 1000                      */
	{illegal,nullptr   }, /* 79: 0100 1100 0111 1001                      */
	{illegal,nullptr   }, /* 7a: 0100 1100 0111 1010                      */
	{illegal,nullptr   }, /* 7b: 0100 1100 0111 1011                      */
	{illegal,nullptr   }, /* 7c: 0100 1100 0111 1100                      */
	{illegal,nullptr   }, /* 7d: 0100 1100 0111 1101                      */
	{illegal,nullptr   }, /* 7e: 0100 1100 0111 1110                      */
	{illegal,nullptr   }, /* 7f: 0100 1100 0111 1111                      */

	{illegal,nullptr   }, /* 80: 0100 1100 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0100 1100 1000 0001                      */
	{illegal,nullptr   }, /* 82: 0100 1100 1000 0010                      */
	{illegal,nullptr   }, /* 83: 0100 1100 1000 0011                      */
	{illegal,nullptr   }, /* 84: 0100 1100 1000 0100                      */
	{illegal,nullptr   }, /* 85: 0100 1100 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0100 1100 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0100 1100 1000 0111                      */
	{illegal,nullptr   }, /* 88: 0100 1100 1000 1000                      */
	{illegal,nullptr   }, /* 89: 0100 1100 1000 1001                      */
	{illegal,nullptr   }, /* 8a: 0100 1100 1000 1010                      */
	{illegal,nullptr   }, /* 8b: 0100 1100 1000 1011                      */
	{illegal,nullptr   }, /* 8c: 0100 1100 1000 1100                      */
	{illegal,nullptr   }, /* 8d: 0100 1100 1000 1101                      */
	{illegal,nullptr   }, /* 8e: 0100 1100 1000 1110                      */
	{illegal,nullptr   }, /* 8f: 0100 1100 1000 1111                      */

	{illegal,nullptr   }, /* 90: 0100 1100 1001 0000                      */
	{illegal,nullptr   }, /* 91: 0100 1100 1001 0001                      */
	{illegal,nullptr   }, /* 92: 0100 1100 1001 0010                      */
	{illegal,nullptr   }, /* 93: 0100 1100 1001 0011                      */
	{illegal,nullptr   }, /* 94: 0100 1100 1001 0100                      */
	{illegal,nullptr   }, /* 95: 0100 1100 1001 0101                      */
	{illegal,nullptr   }, /* 96: 0100 1100 1001 0110                      */
	{illegal,nullptr   }, /* 97: 0100 1100 1001 0111                      */
	{illegal,nullptr   }, /* 98: 0100 1100 1001 1000                      */
	{illegal,nullptr   }, /* 99: 0100 1100 1001 1001                      */
	{illegal,nullptr   }, /* 9a: 0100 1100 1001 1010                      */
	{illegal,nullptr   }, /* 9b: 0100 1100 1001 1011                      */
	{illegal,nullptr   }, /* 9c: 0100 1100 1001 1100                      */
	{illegal,nullptr   }, /* 9d: 0100 1100 1001 1101                      */
	{illegal,nullptr   }, /* 9e: 0100 1100 1001 1110                      */
	{illegal,nullptr   }, /* 9f: 0100 1100 1001 1111                      */

	{illegal,nullptr   }, /* a0: 0100 1100 1010 0000                      */
	{illegal,nullptr   }, /* a1: 0100 1100 1010 0001                      */
	{illegal,nullptr   }, /* a2: 0100 1100 1010 0010                      */
	{illegal,nullptr   }, /* a3: 0100 1100 1010 0011                      */
	{illegal,nullptr   }, /* a4: 0100 1100 1010 0100                      */
	{illegal,nullptr   }, /* a5: 0100 1100 1010 0101                      */
	{illegal,nullptr   }, /* a6: 0100 1100 1010 0110                      */
	{illegal,nullptr   }, /* a7: 0100 1100 1010 0111                      */
	{illegal,nullptr   }, /* a8: 0100 1100 1010 1000                      */
	{illegal,nullptr   }, /* a9: 0100 1100 1010 1001                      */
	{illegal,nullptr   }, /* aa: 0100 1100 1010 1010                      */
	{illegal,nullptr   }, /* ab: 0100 1100 1010 1011                      */
	{illegal,nullptr   }, /* ac: 0100 1100 1010 1100                      */
	{illegal,nullptr   }, /* ad: 0100 1100 1010 1101                      */
	{illegal,nullptr   }, /* ae: 0100 1100 1010 1110                      */
	{illegal,nullptr   }, /* af: 0100 1100 1010 1111                      */

	{illegal,nullptr   }, /* b0: 0100 1100 1011 0000                      */
	{illegal,nullptr   }, /* b1: 0100 1100 1011 0001                      */
	{illegal,nullptr   }, /* b2: 0100 1100 1011 0010                      */
	{illegal,nullptr   }, /* b3: 0100 1100 1011 0011                      */
	{illegal,nullptr   }, /* b4: 0100 1100 1011 0100                      */
	{illegal,nullptr   }, /* b5: 0100 1100 1011 0101                      */
	{illegal,nullptr   }, /* b6: 0100 1100 1011 0110                      */
	{illegal,nullptr   }, /* b7: 0100 1100 1011 0111                      */
	{illegal,nullptr   }, /* b8: 0100 1100 1011 1000                      */
	{illegal,nullptr   }, /* b9: 0100 1100 1011 1001                      */
	{illegal,nullptr   }, /* ba: 0100 1100 1011 1010                      */
	{illegal,nullptr   }, /* bb: 0100 1100 1011 1011                      */
	{illegal,nullptr   }, /* bc: 0100 1100 1011 1100                      */
	{illegal,nullptr   }, /* bd: 0100 1100 1011 1101                      */
	{illegal,nullptr   }, /* be: 0100 1100 1011 1110                      */
	{illegal,nullptr   }, /* bf: 0100 1100 1011 1111                      */

	{MOV,"A,PA"     }, /* c0: 0100 1100 1100 0000                      */
	{MOV,"A,PB"     }, /* c1: 0100 1100 1100 0001                      */
	{MOV,"A,PC"     }, /* c2: 0100 1100 1100 0010                      */
	{MOV,"A,PD"     }, /* c3: 0100 1100 1100 0011                      */
	{illegal,nullptr   }, /* c4: 0100 1100 1100 0100                      */
	{MOV,"A,PF"     }, /* c5: 0100 1100 1100 0101                      */
	{MOV,"A,MKH"    }, /* c6: 0100 1100 1100 0110                      */
	{MOV,"A,MKL"    }, /* c7: 0100 1100 1100 0111                      */
	{illegal,nullptr      }, /* c8: 0100 1100 1100 1000                      */
	{MOV,"A,SMH"    }, /* c9: 0100 1100 1100 1001                      */
	{illegal,nullptr   }, /* ca: 0100 1100 1100 1010                      */
	{MOV,"A,EOM"    }, /* cb: 0100 1100 1100 1011                      */
	{illegal,nullptr   }, /* cc: 0100 1100 1100 1100                      */
	{MOV,"A,TMM"    }, /* cd: 0100 1100 1100 1101                      */
	{MOV,"A,PT"      }, /* ce: 0100 1100 1100 1110                      */  /* 7807 */
	{illegal,nullptr   }, /* cf: 0100 1100 1100 1111                      */

	{illegal,nullptr   }, /* d0: 0100 1100 1101 0000                      */
	{illegal,nullptr   }, /* d1: 0100 1100 1101 0001                      */
	{illegal,nullptr   }, /* d2: 0100 1100 1101 0010                      */
	{illegal,nullptr   }, /* d3: 0100 1100 1101 0011                      */
	{illegal,nullptr   }, /* d4: 0100 1100 1101 0100                      */
	{illegal,nullptr   }, /* d5: 0100 1100 1101 0101                      */
	{illegal,nullptr   }, /* d6: 0100 1100 1101 0110                      */
	{illegal,nullptr   }, /* d7: 0100 1100 1101 0111                      */
	{illegal,nullptr   }, /* d8: 0100 1100 1101 1000                      */
	{MOV,"A,RXB"    }, /* d9: 0100 1100 1101 1001                      */
	{illegal,nullptr   }, /* da: 0100 1100 1101 1010                      */
	{illegal,nullptr   }, /* db: 0100 1100 1101 1011                      */
	{illegal,nullptr   }, /* dc: 0100 1100 1101 1100                      */
	{illegal,nullptr   }, /* dd: 0100 1100 1101 1101                      */
	{illegal,nullptr   }, /* de: 0100 1100 1101 1110                      */
	{illegal,nullptr   }, /* df: 0100 1100 1101 1111                      */

	{illegal,nullptr   }, /* e0: 0100 1100 1110 0000                      */
	{illegal,nullptr   }, /* e1: 0100 1100 1110 0001                      */
	{illegal,nullptr   }, /* e2: 0100 1100 1110 0010                      */
	{illegal,nullptr   }, /* e3: 0100 1100 1110 0011                      */
	{illegal,nullptr   }, /* e4: 0100 1100 1110 0100                      */
	{illegal,nullptr   }, /* e5: 0100 1100 1110 0101                      */
	{illegal,nullptr   }, /* e6: 0100 1100 1110 0110                      */
	{illegal,nullptr   }, /* e7: 0100 1100 1110 0111                      */
	{illegal,nullptr   }, /* e8: 0100 1100 1110 1000                      */
	{illegal,nullptr   }, /* e9: 0100 1100 1110 1001                      */
	{illegal,nullptr   }, /* ea: 0100 1100 1110 1010                      */
	{illegal,nullptr   }, /* eb: 0100 1100 1110 1011                      */
	{illegal,nullptr   }, /* ec: 0100 1100 1110 1100                      */
	{illegal,nullptr   }, /* ed: 0100 1100 1110 1101                      */
	{illegal,nullptr   }, /* ee: 0100 1100 1110 1110                      */
	{illegal,nullptr   }, /* ef: 0100 1100 1110 1111                      */

	{illegal,nullptr   }, /* f0: 0100 1100 1111 0000                      */
	{illegal,nullptr   }, /* f1: 0100 1100 1111 0001                      */
	{illegal,nullptr   }, /* f2: 0100 1100 1111 0010                      */
	{illegal,nullptr   }, /* f3: 0100 1100 1111 0011                      */
	{illegal,nullptr   }, /* f4: 0100 1100 1111 0100                      */
	{illegal,nullptr   }, /* f5: 0100 1100 1111 0101                      */
	{illegal,nullptr   }, /* f6: 0100 1100 1111 0110                      */
	{illegal,nullptr   }, /* f7: 0100 1100 1111 0111                      */
	{illegal,nullptr   }, /* f8: 0100 1100 1111 1000                      */
	{illegal,nullptr   }, /* f9: 0100 1100 1111 1001                      */
	{illegal,nullptr   }, /* fa: 0100 1100 1111 1010                      */
	{illegal,nullptr   }, /* fb: 0100 1100 1111 1011                      */
	{illegal,nullptr   }, /* fc: 0100 1100 1111 1100                      */
	{illegal,nullptr   }, /* fd: 0100 1100 1111 1101                      */
	{illegal,nullptr   }, /* fe: 0100 1100 1111 1110                      */
	{illegal,nullptr   }  /* ff: 0100 1100 1111 1111                      */
};

/* prefix 4D */
static const struct dasm_s  dasm4D_7810[256] =
{
	{illegal,nullptr   }, /* 00: 0100 1101 0000 0000                      */
	{illegal,nullptr   }, /* 01: 0100 1101 0000 0001                      */
	{illegal,nullptr   }, /* 02: 0100 1101 0000 0010                      */
	{illegal,nullptr   }, /* 03: 0100 1101 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0100 1101 0000 0100                      */
	{illegal,nullptr   }, /* 05: 0100 1101 0000 0101                      */
	{illegal,nullptr   }, /* 06: 0100 1101 0000 0110                      */
	{illegal,nullptr   }, /* 07: 0100 1101 0000 0111                      */
	{illegal,nullptr   }, /* 08: 0100 1101 0000 1000                      */
	{illegal,nullptr   }, /* 09: 0100 1101 0000 1001                      */
	{illegal,nullptr   }, /* 0a: 0100 1101 0000 1010                      */
	{illegal,nullptr   }, /* 0b: 0100 1101 0000 1011                      */
	{illegal,nullptr   }, /* 0c: 0100 1101 0000 1100                      */
	{illegal,nullptr   }, /* 0d: 0100 1101 0000 1101                      */
	{illegal,nullptr   }, /* 0e: 0100 1101 0000 1110                      */
	{illegal,nullptr   }, /* 0f: 0100 1101 0000 1111                      */

	{illegal,nullptr   }, /* 10: 0100 1101 0001 0000                      */
	{illegal,nullptr   }, /* 11: 0100 1101 0001 0001                      */
	{illegal,nullptr   }, /* 12: 0100 1101 0001 0010                      */
	{illegal,nullptr   }, /* 13: 0100 1101 0001 0011                      */
	{illegal,nullptr   }, /* 14: 0100 1101 0001 0100                      */
	{illegal,nullptr   }, /* 15: 0100 1101 0001 0101                      */
	{illegal,nullptr   }, /* 16: 0100 1101 0001 0110                      */
	{illegal,nullptr   }, /* 17: 0100 1101 0001 0111                      */
	{illegal,nullptr   }, /* 18: 0100 1101 0001 1000                      */
	{illegal,nullptr   }, /* 19: 0100 1101 0001 1001                      */
	{illegal,nullptr   }, /* 1a: 0100 1101 0001 1010                      */
	{illegal,nullptr   }, /* 1b: 0100 1101 0001 1011                      */
	{illegal,nullptr   }, /* 1c: 0100 1101 0001 1100                      */
	{illegal,nullptr   }, /* 1d: 0100 1101 0001 1101                      */
	{illegal,nullptr   }, /* 1e: 0100 1101 0001 1110                      */
	{illegal,nullptr   }, /* 1f: 0100 1101 0001 1111                      */

	{illegal,nullptr   }, /* 20: 0100 1101 0010 0000                      */
	{illegal,nullptr   }, /* 21: 0100 1101 0010 0001                      */
	{illegal,nullptr   }, /* 22: 0100 1101 0010 0010                      */
	{illegal,nullptr   }, /* 23: 0100 1101 0010 0011                      */
	{illegal,nullptr   }, /* 24: 0100 1101 0010 0100                      */
	{illegal,nullptr   }, /* 25: 0100 1101 0010 0101                      */
	{illegal,nullptr   }, /* 26: 0100 1101 0010 0110                      */
	{illegal,nullptr   }, /* 27: 0100 1101 0010 0111                      */
	{illegal,nullptr   }, /* 28: 0100 1101 0010 1000                      */
	{illegal,nullptr   }, /* 29: 0100 1101 0010 1001                      */
	{illegal,nullptr   }, /* 2a: 0100 1101 0010 1010                      */
	{illegal,nullptr   }, /* 2b: 0100 1101 0010 1011                      */
	{illegal,nullptr   }, /* 2c: 0100 1101 0010 1100                      */
	{illegal,nullptr   }, /* 2d: 0100 1101 0010 1101                      */
	{illegal,nullptr   }, /* 2e: 0100 1101 0010 1110                      */
	{illegal,nullptr   }, /* 2f: 0100 1101 0010 1111                      */

	{illegal,nullptr   }, /* 30: 0100 1101 0011 0000                      */
	{illegal,nullptr   }, /* 31: 0100 1101 0011 0001                      */
	{illegal,nullptr   }, /* 32: 0100 1101 0011 0010                      */
	{illegal,nullptr   }, /* 33: 0100 1101 0011 0011                      */
	{illegal,nullptr   }, /* 34: 0100 1101 0011 0100                      */
	{illegal,nullptr   }, /* 35: 0100 1101 0011 0101                      */
	{illegal,nullptr   }, /* 36: 0100 1101 0011 0110                      */
	{illegal,nullptr   }, /* 37: 0100 1101 0011 0111                      */
	{illegal,nullptr   }, /* 38: 0100 1101 0011 1000                      */
	{illegal,nullptr   }, /* 39: 0100 1101 0011 1001                      */
	{illegal,nullptr   }, /* 3a: 0100 1101 0011 1010                      */
	{illegal,nullptr   }, /* 3b: 0100 1101 0011 1011                      */
	{illegal,nullptr   }, /* 3c: 0100 1101 0011 1100                      */
	{illegal,nullptr   }, /* 3d: 0100 1101 0011 1101                      */
	{illegal,nullptr   }, /* 3e: 0100 1101 0011 1110                      */
	{illegal,nullptr   }, /* 3f: 0100 1101 0011 1111                      */

	{illegal,nullptr   }, /* 40: 0100 1101 0100 0000                      */
	{illegal,nullptr   }, /* 41: 0100 1101 0100 0001                      */
	{illegal,nullptr   }, /* 42: 0100 1101 0100 0010                      */
	{illegal,nullptr   }, /* 43: 0100 1101 0100 0011                      */
	{illegal,nullptr   }, /* 44: 0100 1101 0100 0100                      */
	{illegal,nullptr   }, /* 45: 0100 1101 0100 0101                      */
	{illegal,nullptr   }, /* 46: 0100 1101 0100 0110                      */
	{illegal,nullptr   }, /* 47: 0100 1101 0100 0111                      */
	{illegal,nullptr   }, /* 48: 0100 1101 0100 1000                      */
	{illegal,nullptr   }, /* 49: 0100 1101 0100 1001                      */
	{illegal,nullptr   }, /* 4a: 0100 1101 0100 1010                      */
	{illegal,nullptr   }, /* 4b: 0100 1101 0100 1011                      */
	{illegal,nullptr   }, /* 4c: 0100 1101 0100 1100                      */
	{illegal,nullptr   }, /* 4d: 0100 1101 0100 1101                      */
	{illegal,nullptr   }, /* 4e: 0100 1101 0100 1110                      */
	{illegal,nullptr   }, /* 4f: 0100 1101 0100 1111                      */

	{illegal,nullptr   }, /* 50: 0100 1101 0101 0000                      */
	{illegal,nullptr   }, /* 51: 0100 1101 0101 0001                      */
	{illegal,nullptr   }, /* 52: 0100 1101 0101 0010                      */
	{illegal,nullptr   }, /* 53: 0100 1101 0101 0011                      */
	{illegal,nullptr   }, /* 54: 0100 1101 0101 0100                      */
	{illegal,nullptr   }, /* 55: 0100 1101 0101 0101                      */
	{illegal,nullptr   }, /* 56: 0100 1101 0101 0110                      */
	{illegal,nullptr   }, /* 57: 0100 1101 0101 0111                      */
	{illegal,nullptr   }, /* 58: 0100 1101 0101 1000                      */
	{illegal,nullptr   }, /* 59: 0100 1101 0101 1001                      */
	{illegal,nullptr   }, /* 5a: 0100 1101 0101 1010                      */
	{illegal,nullptr   }, /* 5b: 0100 1101 0101 1011                      */
	{illegal,nullptr   }, /* 5c: 0100 1101 0101 1100                      */
	{illegal,nullptr   }, /* 5d: 0100 1101 0101 1101                      */
	{illegal,nullptr   }, /* 5e: 0100 1101 0101 1110                      */
	{illegal,nullptr   }, /* 5f: 0100 1101 0101 1111                      */

	{illegal,nullptr   }, /* 60: 0100 1101 0110 0000                      */
	{illegal,nullptr   }, /* 61: 0100 1101 0110 0001                      */
	{illegal,nullptr   }, /* 62: 0100 1101 0110 0010                      */
	{illegal,nullptr   }, /* 63: 0100 1101 0110 0011                      */
	{illegal,nullptr   }, /* 64: 0100 1101 0110 0100                      */
	{illegal,nullptr   }, /* 65: 0100 1101 0110 0101                      */
	{illegal,nullptr   }, /* 66: 0100 1101 0110 0110                      */
	{illegal,nullptr   }, /* 67: 0100 1101 0110 0111                      */
	{illegal,nullptr   }, /* 68: 0100 1101 0110 1000                      */
	{illegal,nullptr   }, /* 69: 0100 1101 0110 1001                      */
	{illegal,nullptr   }, /* 6a: 0100 1101 0110 1010                      */
	{illegal,nullptr   }, /* 6b: 0100 1101 0110 1011                      */
	{illegal,nullptr   }, /* 6c: 0100 1101 0110 1100                      */
	{illegal,nullptr   }, /* 6d: 0100 1101 0110 1101                      */
	{illegal,nullptr   }, /* 6e: 0100 1101 0110 1110                      */
	{illegal,nullptr   }, /* 6f: 0100 1101 0110 1111                      */

	{illegal,nullptr   }, /* 70: 0100 1101 0111 0000                      */
	{illegal,nullptr   }, /* 71: 0100 1101 0111 0001                      */
	{illegal,nullptr   }, /* 72: 0100 1101 0111 0010                      */
	{illegal,nullptr   }, /* 73: 0100 1101 0111 0011                      */
	{illegal,nullptr   }, /* 74: 0100 1101 0111 0100                      */
	{illegal,nullptr   }, /* 75: 0100 1101 0111 0101                      */
	{illegal,nullptr   }, /* 76: 0100 1101 0111 0110                      */
	{illegal,nullptr   }, /* 77: 0100 1101 0111 0111                      */
	{illegal,nullptr   }, /* 78: 0100 1101 0111 1000                      */
	{illegal,nullptr   }, /* 79: 0100 1101 0111 1001                      */
	{illegal,nullptr   }, /* 7a: 0100 1101 0111 1010                      */
	{illegal,nullptr   }, /* 7b: 0100 1101 0111 1011                      */
	{illegal,nullptr   }, /* 7c: 0100 1101 0111 1100                      */
	{illegal,nullptr   }, /* 7d: 0100 1101 0111 1101                      */
	{illegal,nullptr   }, /* 7e: 0100 1101 0111 1110                      */
	{illegal,nullptr   }, /* 7f: 0100 1101 0111 1111                      */

	{illegal,nullptr   }, /* 80: 0100 1101 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0100 1101 1000 0001                      */
	{illegal,nullptr   }, /* 82: 0100 1101 1000 0010                      */
	{illegal,nullptr   }, /* 83: 0100 1101 1000 0011                      */
	{illegal,nullptr   }, /* 84: 0100 1101 1000 0100                      */
	{illegal,nullptr   }, /* 85: 0100 1101 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0100 1101 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0100 1101 1000 0111                      */
	{illegal,nullptr   }, /* 88: 0100 1101 1000 1000                      */
	{illegal,nullptr   }, /* 89: 0100 1101 1000 1001                      */
	{illegal,nullptr   }, /* 8a: 0100 1101 1000 1010                      */
	{illegal,nullptr   }, /* 8b: 0100 1101 1000 1011                      */
	{illegal,nullptr   }, /* 8c: 0100 1101 1000 1100                      */
	{illegal,nullptr   }, /* 8d: 0100 1101 1000 1101                      */
	{illegal,nullptr   }, /* 8e: 0100 1101 1000 1110                      */
	{illegal,nullptr   }, /* 8f: 0100 1101 1000 1111                      */

	{illegal,nullptr   }, /* 90: 0100 1101 1001 0000                      */
	{illegal,nullptr   }, /* 91: 0100 1101 1001 0001                      */
	{illegal,nullptr   }, /* 92: 0100 1101 1001 0010                      */
	{illegal,nullptr   }, /* 93: 0100 1101 1001 0011                      */
	{illegal,nullptr   }, /* 94: 0100 1101 1001 0100                      */
	{illegal,nullptr   }, /* 95: 0100 1101 1001 0101                      */
	{illegal,nullptr   }, /* 96: 0100 1101 1001 0110                      */
	{illegal,nullptr   }, /* 97: 0100 1101 1001 0111                      */
	{illegal,nullptr   }, /* 98: 0100 1101 1001 1000                      */
	{illegal,nullptr   }, /* 99: 0100 1101 1001 1001                      */
	{illegal,nullptr   }, /* 9a: 0100 1101 1001 1010                      */
	{illegal,nullptr   }, /* 9b: 0100 1101 1001 1011                      */
	{illegal,nullptr   }, /* 9c: 0100 1101 1001 1100                      */
	{illegal,nullptr   }, /* 9d: 0100 1101 1001 1101                      */
	{illegal,nullptr   }, /* 9e: 0100 1101 1001 1110                      */
	{illegal,nullptr   }, /* 9f: 0100 1101 1001 1111                      */

	{illegal,nullptr   }, /* a0: 0100 1101 1010 0000                      */
	{illegal,nullptr   }, /* a1: 0100 1101 1010 0001                      */
	{illegal,nullptr   }, /* a2: 0100 1101 1010 0010                      */
	{illegal,nullptr   }, /* a3: 0100 1101 1010 0011                      */
	{illegal,nullptr   }, /* a4: 0100 1101 1010 0100                      */
	{illegal,nullptr   }, /* a5: 0100 1101 1010 0101                      */
	{illegal,nullptr   }, /* a6: 0100 1101 1010 0110                      */
	{illegal,nullptr   }, /* a7: 0100 1101 1010 0111                      */
	{illegal,nullptr   }, /* a8: 0100 1101 1010 1000                      */
	{illegal,nullptr   }, /* a9: 0100 1101 1010 1001                      */
	{illegal,nullptr   }, /* aa: 0100 1101 1010 1010                      */
	{illegal,nullptr   }, /* ab: 0100 1101 1010 1011                      */
	{illegal,nullptr   }, /* ac: 0100 1101 1010 1100                      */
	{illegal,nullptr   }, /* ad: 0100 1101 1010 1101                      */
	{illegal,nullptr   }, /* ae: 0100 1101 1010 1110                      */
	{illegal,nullptr   }, /* af: 0100 1101 1010 1111                      */

	{illegal,nullptr   }, /* b0: 0100 1101 1011 0000                      */
	{illegal,nullptr   }, /* b1: 0100 1101 1011 0001                      */
	{illegal,nullptr   }, /* b2: 0100 1101 1011 0010                      */
	{illegal,nullptr   }, /* b3: 0100 1101 1011 0011                      */
	{illegal,nullptr   }, /* b4: 0100 1101 1011 0100                      */
	{illegal,nullptr   }, /* b5: 0100 1101 1011 0101                      */
	{illegal,nullptr   }, /* b6: 0100 1101 1011 0110                      */
	{illegal,nullptr   }, /* b7: 0100 1101 1011 0111                      */
	{illegal,nullptr   }, /* b8: 0100 1101 1011 1000                      */
	{illegal,nullptr   }, /* b9: 0100 1101 1011 1001                      */
	{illegal,nullptr   }, /* ba: 0100 1101 1011 1010                      */
	{illegal,nullptr   }, /* bb: 0100 1101 1011 1011                      */
	{illegal,nullptr   }, /* bc: 0100 1101 1011 1100                      */
	{illegal,nullptr   }, /* bd: 0100 1101 1011 1101                      */
	{illegal,nullptr   }, /* be: 0100 1101 1011 1110                      */
	{illegal,nullptr   }, /* bf: 0100 1101 1011 1111                      */

	{MOV,"PA,A"     }, /* c0: 0100 1101 1100 0000                      */
	{MOV,"PB,A"     }, /* c1: 0100 1101 1100 0001                      */
	{MOV,"PC,A"     }, /* c2: 0100 1101 1100 0010                      */
	{MOV,"PD,A"     }, /* c3: 0100 1101 1100 0011                      */
	{illegal,nullptr   }, /* c4: 0100 1101 1100 0100                      */
	{MOV,"PF,A"     }, /* c5: 0100 1101 1100 0101                      */
	{MOV,"MKH,A"    }, /* c6: 0100 1101 1100 0110                      */
	{MOV,"MKL,A"    }, /* c7: 0100 1101 1100 0111                      */
	{MOV,"ANM,A"    }, /* c8: 0100 1101 1100 1000                      */   /* 7810 */
	{MOV,"SMH,A"    }, /* c9: 0100 1101 1100 1001                      */
	{MOV,"SML,A"    }, /* ca: 0100 1101 1100 1010                      */
	{MOV,"EOM,A"    }, /* cb: 0100 1101 1100 1011                      */
	{MOV,"ETMM,A"   }, /* cc: 0100 1101 1100 1100                      */
	{MOV,"TMM,A"    }, /* cd: 0100 1101 1100 1101                      */
	{illegal,nullptr   }, /* ce: 0100 1101 1100 1110                      */
	{illegal,nullptr   }, /* cf: 0100 1101 1100 1111                      */

	{MOV,"MM,A"     }, /* d0: 0100 1101 1101 0000                      */
	{MOV,"MCC,A"    }, /* d1: 0100 1101 1101 0001                      */
	{MOV,"MA,A"     }, /* d2: 0100 1101 1101 0010                      */
	{MOV,"MB,A"     }, /* d3: 0100 1101 1101 0011                      */
	{MOV,"MC,A"     }, /* d4: 0100 1101 1101 0100                      */
	{illegal,nullptr   }, /* d5: 0100 1101 1101 0101                      */
	{illegal,nullptr   }, /* d6: 0100 1101 1101 0110                      */
	{MOV,"MF,A"     }, /* d7: 0100 1101 1101 0111                      */
	{MOV,"TXB,A"    }, /* d8: 0100 1101 1101 1000                      */
	{illegal,nullptr   }, /* d9: 0100 1101 1101 1001                      */
	{MOV,"TM0,A"    }, /* da: 0100 1101 1101 1010                      */
	{MOV,"TM1,A"    }, /* db: 0100 1101 1101 1011                      */
	{illegal,nullptr   }, /* dc: 0100 1101 1101 1100                      */
	{illegal,nullptr   }, /* dd: 0100 1101 1101 1101                      */
	{illegal,nullptr   }, /* de: 0100 1101 1101 1110                      */
	{illegal,nullptr   }, /* df: 0100 1101 1101 1111                      */

	{illegal,nullptr   }, /* e0: 0100 1101 1110 0000                      */
	{illegal,nullptr   }, /* e1: 0100 1101 1110 0001                      */
	{illegal,nullptr   }, /* e2: 0100 1101 1110 0010                      */
	{illegal,nullptr   }, /* e3: 0100 1101 1110 0011                      */
	{illegal,nullptr   }, /* e4: 0100 1101 1110 0100                      */
	{illegal,nullptr   }, /* e5: 0100 1101 1110 0101                      */
	{illegal,nullptr   }, /* e6: 0100 1101 1110 0110                      */
	{illegal,nullptr   }, /* e7: 0100 1101 1110 0111                      */
	{MOV,"ZCM,A"    }, /* e8: 0100 1101 1110 1000                      */   /* 7810 */
	{illegal,nullptr   }, /* e9: 0100 1101 1110 1001                      */
	{illegal,nullptr   }, /* ea: 0100 1101 1110 1010                      */
	{illegal,nullptr   }, /* eb: 0100 1101 1110 1011                      */
	{illegal,nullptr   }, /* ec: 0100 1101 1110 1100                      */
	{illegal,nullptr   }, /* ed: 0100 1101 1110 1101                      */
	{illegal,nullptr   }, /* ee: 0100 1101 1110 1110                      */
	{illegal,nullptr   }, /* ef: 0100 1101 1110 1111                      */

	{illegal,nullptr   }, /* f0: 0100 1101 1111 0000                      */
	{illegal,nullptr   }, /* f1: 0100 1101 1111 0001                      */
	{illegal,nullptr   }, /* f2: 0100 1101 1111 0010                      */
	{illegal,nullptr   }, /* f3: 0100 1101 1111 0011                      */
	{illegal,nullptr   }, /* f4: 0100 1101 1111 0100                      */
	{illegal,nullptr   }, /* f5: 0100 1101 1111 0101                      */
	{illegal,nullptr   }, /* f6: 0100 1101 1111 0110                      */
	{illegal,nullptr   }, /* f7: 0100 1101 1111 0111                      */
	{illegal,nullptr   }, /* f8: 0100 1101 1111 1000                      */
	{illegal,nullptr   }, /* f9: 0100 1101 1111 1001                      */
	{illegal,nullptr   }, /* fa: 0100 1101 1111 1010                      */
	{illegal,nullptr   }, /* fb: 0100 1101 1111 1011                      */
	{illegal,nullptr   }, /* fc: 0100 1101 1111 1100                      */
	{illegal,nullptr   }, /* fd: 0100 1101 1111 1101                      */
	{illegal,nullptr   }, /* fe: 0100 1101 1111 1110                      */
	{illegal,nullptr   }  /* ff: 0100 1101 1111 1111                      */
};

static const struct dasm_s  dasm4D_7807[256] =
{
	{illegal,nullptr   }, /* 00: 0100 1101 0000 0000                      */
	{illegal,nullptr   }, /* 01: 0100 1101 0000 0001                      */
	{illegal,nullptr   }, /* 02: 0100 1101 0000 0010                      */
	{illegal,nullptr   }, /* 03: 0100 1101 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0100 1101 0000 0100                      */
	{illegal,nullptr   }, /* 05: 0100 1101 0000 0101                      */
	{illegal,nullptr   }, /* 06: 0100 1101 0000 0110                      */
	{illegal,nullptr   }, /* 07: 0100 1101 0000 0111                      */
	{illegal,nullptr   }, /* 08: 0100 1101 0000 1000                      */
	{illegal,nullptr   }, /* 09: 0100 1101 0000 1001                      */
	{illegal,nullptr   }, /* 0a: 0100 1101 0000 1010                      */
	{illegal,nullptr   }, /* 0b: 0100 1101 0000 1011                      */
	{illegal,nullptr   }, /* 0c: 0100 1101 0000 1100                      */
	{illegal,nullptr   }, /* 0d: 0100 1101 0000 1101                      */
	{illegal,nullptr   }, /* 0e: 0100 1101 0000 1110                      */
	{illegal,nullptr   }, /* 0f: 0100 1101 0000 1111                      */

	{illegal,nullptr   }, /* 10: 0100 1101 0001 0000                      */
	{illegal,nullptr   }, /* 11: 0100 1101 0001 0001                      */
	{illegal,nullptr   }, /* 12: 0100 1101 0001 0010                      */
	{illegal,nullptr   }, /* 13: 0100 1101 0001 0011                      */
	{illegal,nullptr   }, /* 14: 0100 1101 0001 0100                      */
	{illegal,nullptr   }, /* 15: 0100 1101 0001 0101                      */
	{illegal,nullptr   }, /* 16: 0100 1101 0001 0110                      */
	{illegal,nullptr   }, /* 17: 0100 1101 0001 0111                      */
	{illegal,nullptr   }, /* 18: 0100 1101 0001 1000                      */
	{illegal,nullptr   }, /* 19: 0100 1101 0001 1001                      */
	{illegal,nullptr   }, /* 1a: 0100 1101 0001 1010                      */
	{illegal,nullptr   }, /* 1b: 0100 1101 0001 1011                      */
	{illegal,nullptr   }, /* 1c: 0100 1101 0001 1100                      */
	{illegal,nullptr   }, /* 1d: 0100 1101 0001 1101                      */
	{illegal,nullptr   }, /* 1e: 0100 1101 0001 1110                      */
	{illegal,nullptr   }, /* 1f: 0100 1101 0001 1111                      */

	{illegal,nullptr   }, /* 20: 0100 1101 0010 0000                      */
	{illegal,nullptr   }, /* 21: 0100 1101 0010 0001                      */
	{illegal,nullptr   }, /* 22: 0100 1101 0010 0010                      */
	{illegal,nullptr   }, /* 23: 0100 1101 0010 0011                      */
	{illegal,nullptr   }, /* 24: 0100 1101 0010 0100                      */
	{illegal,nullptr   }, /* 25: 0100 1101 0010 0101                      */
	{illegal,nullptr   }, /* 26: 0100 1101 0010 0110                      */
	{illegal,nullptr   }, /* 27: 0100 1101 0010 0111                      */
	{illegal,nullptr   }, /* 28: 0100 1101 0010 1000                      */
	{illegal,nullptr   }, /* 29: 0100 1101 0010 1001                      */
	{illegal,nullptr   }, /* 2a: 0100 1101 0010 1010                      */
	{illegal,nullptr   }, /* 2b: 0100 1101 0010 1011                      */
	{illegal,nullptr   }, /* 2c: 0100 1101 0010 1100                      */
	{illegal,nullptr   }, /* 2d: 0100 1101 0010 1101                      */
	{illegal,nullptr   }, /* 2e: 0100 1101 0010 1110                      */
	{illegal,nullptr   }, /* 2f: 0100 1101 0010 1111                      */

	{illegal,nullptr   }, /* 30: 0100 1101 0011 0000                      */
	{illegal,nullptr   }, /* 31: 0100 1101 0011 0001                      */
	{illegal,nullptr   }, /* 32: 0100 1101 0011 0010                      */
	{illegal,nullptr   }, /* 33: 0100 1101 0011 0011                      */
	{illegal,nullptr   }, /* 34: 0100 1101 0011 0100                      */
	{illegal,nullptr   }, /* 35: 0100 1101 0011 0101                      */
	{illegal,nullptr   }, /* 36: 0100 1101 0011 0110                      */
	{illegal,nullptr   }, /* 37: 0100 1101 0011 0111                      */
	{illegal,nullptr   }, /* 38: 0100 1101 0011 1000                      */
	{illegal,nullptr   }, /* 39: 0100 1101 0011 1001                      */
	{illegal,nullptr   }, /* 3a: 0100 1101 0011 1010                      */
	{illegal,nullptr   }, /* 3b: 0100 1101 0011 1011                      */
	{illegal,nullptr   }, /* 3c: 0100 1101 0011 1100                      */
	{illegal,nullptr   }, /* 3d: 0100 1101 0011 1101                      */
	{illegal,nullptr   }, /* 3e: 0100 1101 0011 1110                      */
	{illegal,nullptr   }, /* 3f: 0100 1101 0011 1111                      */

	{illegal,nullptr   }, /* 40: 0100 1101 0100 0000                      */
	{illegal,nullptr   }, /* 41: 0100 1101 0100 0001                      */
	{illegal,nullptr   }, /* 42: 0100 1101 0100 0010                      */
	{illegal,nullptr   }, /* 43: 0100 1101 0100 0011                      */
	{illegal,nullptr   }, /* 44: 0100 1101 0100 0100                      */
	{illegal,nullptr   }, /* 45: 0100 1101 0100 0101                      */
	{illegal,nullptr   }, /* 46: 0100 1101 0100 0110                      */
	{illegal,nullptr   }, /* 47: 0100 1101 0100 0111                      */
	{illegal,nullptr   }, /* 48: 0100 1101 0100 1000                      */
	{illegal,nullptr   }, /* 49: 0100 1101 0100 1001                      */
	{illegal,nullptr   }, /* 4a: 0100 1101 0100 1010                      */
	{illegal,nullptr   }, /* 4b: 0100 1101 0100 1011                      */
	{illegal,nullptr   }, /* 4c: 0100 1101 0100 1100                      */
	{illegal,nullptr   }, /* 4d: 0100 1101 0100 1101                      */
	{illegal,nullptr   }, /* 4e: 0100 1101 0100 1110                      */
	{illegal,nullptr   }, /* 4f: 0100 1101 0100 1111                      */

	{illegal,nullptr   }, /* 50: 0100 1101 0101 0000                      */
	{illegal,nullptr   }, /* 51: 0100 1101 0101 0001                      */
	{illegal,nullptr   }, /* 52: 0100 1101 0101 0010                      */
	{illegal,nullptr   }, /* 53: 0100 1101 0101 0011                      */
	{illegal,nullptr   }, /* 54: 0100 1101 0101 0100                      */
	{illegal,nullptr   }, /* 55: 0100 1101 0101 0101                      */
	{illegal,nullptr   }, /* 56: 0100 1101 0101 0110                      */
	{illegal,nullptr   }, /* 57: 0100 1101 0101 0111                      */
	{illegal,nullptr   }, /* 58: 0100 1101 0101 1000                      */
	{illegal,nullptr   }, /* 59: 0100 1101 0101 1001                      */
	{illegal,nullptr   }, /* 5a: 0100 1101 0101 1010                      */
	{illegal,nullptr   }, /* 5b: 0100 1101 0101 1011                      */
	{illegal,nullptr   }, /* 5c: 0100 1101 0101 1100                      */
	{illegal,nullptr   }, /* 5d: 0100 1101 0101 1101                      */
	{illegal,nullptr   }, /* 5e: 0100 1101 0101 1110                      */
	{illegal,nullptr   }, /* 5f: 0100 1101 0101 1111                      */

	{illegal,nullptr   }, /* 60: 0100 1101 0110 0000                      */
	{illegal,nullptr   }, /* 61: 0100 1101 0110 0001                      */
	{illegal,nullptr   }, /* 62: 0100 1101 0110 0010                      */
	{illegal,nullptr   }, /* 63: 0100 1101 0110 0011                      */
	{illegal,nullptr   }, /* 64: 0100 1101 0110 0100                      */
	{illegal,nullptr   }, /* 65: 0100 1101 0110 0101                      */
	{illegal,nullptr   }, /* 66: 0100 1101 0110 0110                      */
	{illegal,nullptr   }, /* 67: 0100 1101 0110 0111                      */
	{illegal,nullptr   }, /* 68: 0100 1101 0110 1000                      */
	{illegal,nullptr   }, /* 69: 0100 1101 0110 1001                      */
	{illegal,nullptr   }, /* 6a: 0100 1101 0110 1010                      */
	{illegal,nullptr   }, /* 6b: 0100 1101 0110 1011                      */
	{illegal,nullptr   }, /* 6c: 0100 1101 0110 1100                      */
	{illegal,nullptr   }, /* 6d: 0100 1101 0110 1101                      */
	{illegal,nullptr   }, /* 6e: 0100 1101 0110 1110                      */
	{illegal,nullptr   }, /* 6f: 0100 1101 0110 1111                      */

	{illegal,nullptr   }, /* 70: 0100 1101 0111 0000                      */
	{illegal,nullptr   }, /* 71: 0100 1101 0111 0001                      */
	{illegal,nullptr   }, /* 72: 0100 1101 0111 0010                      */
	{illegal,nullptr   }, /* 73: 0100 1101 0111 0011                      */
	{illegal,nullptr   }, /* 74: 0100 1101 0111 0100                      */
	{illegal,nullptr   }, /* 75: 0100 1101 0111 0101                      */
	{illegal,nullptr   }, /* 76: 0100 1101 0111 0110                      */
	{illegal,nullptr   }, /* 77: 0100 1101 0111 0111                      */
	{illegal,nullptr   }, /* 78: 0100 1101 0111 1000                      */
	{illegal,nullptr   }, /* 79: 0100 1101 0111 1001                      */
	{illegal,nullptr   }, /* 7a: 0100 1101 0111 1010                      */
	{illegal,nullptr   }, /* 7b: 0100 1101 0111 1011                      */
	{illegal,nullptr   }, /* 7c: 0100 1101 0111 1100                      */
	{illegal,nullptr   }, /* 7d: 0100 1101 0111 1101                      */
	{illegal,nullptr   }, /* 7e: 0100 1101 0111 1110                      */
	{illegal,nullptr   }, /* 7f: 0100 1101 0111 1111                      */

	{illegal,nullptr   }, /* 80: 0100 1101 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0100 1101 1000 0001                      */
	{illegal,nullptr   }, /* 82: 0100 1101 1000 0010                      */
	{illegal,nullptr   }, /* 83: 0100 1101 1000 0011                      */
	{illegal,nullptr   }, /* 84: 0100 1101 1000 0100                      */
	{illegal,nullptr   }, /* 85: 0100 1101 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0100 1101 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0100 1101 1000 0111                      */
	{illegal,nullptr   }, /* 88: 0100 1101 1000 1000                      */
	{illegal,nullptr   }, /* 89: 0100 1101 1000 1001                      */
	{illegal,nullptr   }, /* 8a: 0100 1101 1000 1010                      */
	{illegal,nullptr   }, /* 8b: 0100 1101 1000 1011                      */
	{illegal,nullptr   }, /* 8c: 0100 1101 1000 1100                      */
	{illegal,nullptr   }, /* 8d: 0100 1101 1000 1101                      */
	{illegal,nullptr   }, /* 8e: 0100 1101 1000 1110                      */
	{illegal,nullptr   }, /* 8f: 0100 1101 1000 1111                      */

	{illegal,nullptr   }, /* 90: 0100 1101 1001 0000                      */
	{illegal,nullptr   }, /* 91: 0100 1101 1001 0001                      */
	{illegal,nullptr   }, /* 92: 0100 1101 1001 0010                      */
	{illegal,nullptr   }, /* 93: 0100 1101 1001 0011                      */
	{illegal,nullptr   }, /* 94: 0100 1101 1001 0100                      */
	{illegal,nullptr   }, /* 95: 0100 1101 1001 0101                      */
	{illegal,nullptr   }, /* 96: 0100 1101 1001 0110                      */
	{illegal,nullptr   }, /* 97: 0100 1101 1001 0111                      */
	{illegal,nullptr   }, /* 98: 0100 1101 1001 1000                      */
	{illegal,nullptr   }, /* 99: 0100 1101 1001 1001                      */
	{illegal,nullptr   }, /* 9a: 0100 1101 1001 1010                      */
	{illegal,nullptr   }, /* 9b: 0100 1101 1001 1011                      */
	{illegal,nullptr   }, /* 9c: 0100 1101 1001 1100                      */
	{illegal,nullptr   }, /* 9d: 0100 1101 1001 1101                      */
	{illegal,nullptr   }, /* 9e: 0100 1101 1001 1110                      */
	{illegal,nullptr   }, /* 9f: 0100 1101 1001 1111                      */

	{illegal,nullptr   }, /* a0: 0100 1101 1010 0000                      */
	{illegal,nullptr   }, /* a1: 0100 1101 1010 0001                      */
	{illegal,nullptr   }, /* a2: 0100 1101 1010 0010                      */
	{illegal,nullptr   }, /* a3: 0100 1101 1010 0011                      */
	{illegal,nullptr   }, /* a4: 0100 1101 1010 0100                      */
	{illegal,nullptr   }, /* a5: 0100 1101 1010 0101                      */
	{illegal,nullptr   }, /* a6: 0100 1101 1010 0110                      */
	{illegal,nullptr   }, /* a7: 0100 1101 1010 0111                      */
	{illegal,nullptr   }, /* a8: 0100 1101 1010 1000                      */
	{illegal,nullptr   }, /* a9: 0100 1101 1010 1001                      */
	{illegal,nullptr   }, /* aa: 0100 1101 1010 1010                      */
	{illegal,nullptr   }, /* ab: 0100 1101 1010 1011                      */
	{illegal,nullptr   }, /* ac: 0100 1101 1010 1100                      */
	{illegal,nullptr   }, /* ad: 0100 1101 1010 1101                      */
	{illegal,nullptr   }, /* ae: 0100 1101 1010 1110                      */
	{illegal,nullptr   }, /* af: 0100 1101 1010 1111                      */

	{illegal,nullptr   }, /* b0: 0100 1101 1011 0000                      */
	{illegal,nullptr   }, /* b1: 0100 1101 1011 0001                      */
	{illegal,nullptr   }, /* b2: 0100 1101 1011 0010                      */
	{illegal,nullptr   }, /* b3: 0100 1101 1011 0011                      */
	{illegal,nullptr   }, /* b4: 0100 1101 1011 0100                      */
	{illegal,nullptr   }, /* b5: 0100 1101 1011 0101                      */
	{illegal,nullptr   }, /* b6: 0100 1101 1011 0110                      */
	{illegal,nullptr   }, /* b7: 0100 1101 1011 0111                      */
	{illegal,nullptr   }, /* b8: 0100 1101 1011 1000                      */
	{illegal,nullptr   }, /* b9: 0100 1101 1011 1001                      */
	{illegal,nullptr   }, /* ba: 0100 1101 1011 1010                      */
	{illegal,nullptr   }, /* bb: 0100 1101 1011 1011                      */
	{illegal,nullptr   }, /* bc: 0100 1101 1011 1100                      */
	{illegal,nullptr   }, /* bd: 0100 1101 1011 1101                      */
	{illegal,nullptr   }, /* be: 0100 1101 1011 1110                      */
	{illegal,nullptr   }, /* bf: 0100 1101 1011 1111                      */

	{MOV,"PA,A"     }, /* c0: 0100 1101 1100 0000                      */
	{MOV,"PB,A"     }, /* c1: 0100 1101 1100 0001                      */
	{MOV,"PC,A"     }, /* c2: 0100 1101 1100 0010                      */
	{MOV,"PD,A"     }, /* c3: 0100 1101 1100 0011                      */
	{illegal,nullptr   }, /* c4: 0100 1101 1100 0100                      */
	{MOV,"PF,A"     }, /* c5: 0100 1101 1100 0101                      */
	{MOV,"MKH,A"    }, /* c6: 0100 1101 1100 0110                      */
	{MOV,"MKL,A"    }, /* c7: 0100 1101 1100 0111                      */
	{illegal,nullptr      }, /* c8: 0100 1101 1100 1000                      */
	{MOV,"SMH,A"    }, /* c9: 0100 1101 1100 1001                      */
	{MOV,"SML,A"    }, /* ca: 0100 1101 1100 1010                      */
	{MOV,"EOM,A"    }, /* cb: 0100 1101 1100 1011                      */
	{MOV,"ETMM,A"   }, /* cc: 0100 1101 1100 1100                      */
	{MOV,"TMM,A"    }, /* cd: 0100 1101 1100 1101                      */
	{illegal,nullptr   }, /* ce: 0100 1101 1100 1110                      */
	{illegal,nullptr   }, /* cf: 0100 1101 1100 1111                      */

	{MOV,"MM,A"     }, /* d0: 0100 1101 1101 0000                      */
	{MOV,"MCC,A"    }, /* d1: 0100 1101 1101 0001                      */
	{MOV,"MA,A"     }, /* d2: 0100 1101 1101 0010                      */
	{MOV,"MB,A"     }, /* d3: 0100 1101 1101 0011                      */
	{MOV,"MC,A"     }, /* d4: 0100 1101 1101 0100                      */
	{illegal,nullptr   }, /* d5: 0100 1101 1101 0101                      */
	{illegal,nullptr   }, /* d6: 0100 1101 1101 0110                      */
	{MOV,"MF,A"     }, /* d7: 0100 1101 1101 0111                      */
	{MOV,"TXB,A"    }, /* d8: 0100 1101 1101 1000                      */
	{illegal,nullptr   }, /* d9: 0100 1101 1101 1001                      */
	{MOV,"TM0,A"    }, /* da: 0100 1101 1101 1010                      */
	{MOV,"TM1,A"    }, /* db: 0100 1101 1101 1011                      */
	{illegal,nullptr   }, /* dc: 0100 1101 1101 1100                      */
	{illegal,nullptr   }, /* dd: 0100 1101 1101 1101                      */
	{illegal,nullptr   }, /* de: 0100 1101 1101 1110                      */
	{illegal,nullptr   }, /* df: 0100 1101 1101 1111                      */

	{illegal,nullptr   }, /* e0: 0100 1101 1110 0000                      */
	{illegal,nullptr   }, /* e1: 0100 1101 1110 0001                      */
	{illegal,nullptr   }, /* e2: 0100 1101 1110 0010                      */
	{illegal,nullptr   }, /* e3: 0100 1101 1110 0011                      */
	{illegal,nullptr   }, /* e4: 0100 1101 1110 0100                      */
	{MOV,"MT,A"      }, /* e5: 0100 1101 1110 0101                      */  /* 7807 */
	{illegal,nullptr   }, /* e6: 0100 1101 1110 0110                      */
	{illegal,nullptr   }, /* e7: 0100 1101 1110 0111                      */
	{illegal,nullptr   }, /* e8: 0100 1101 1110 1000                      */
	{illegal,nullptr   }, /* e9: 0100 1101 1110 1001                      */
	{illegal,nullptr   }, /* ea: 0100 1101 1110 1010                      */
	{illegal,nullptr   }, /* eb: 0100 1101 1110 1011                      */
	{illegal,nullptr   }, /* ec: 0100 1101 1110 1100                      */
	{illegal,nullptr   }, /* ed: 0100 1101 1110 1101                      */
	{illegal,nullptr   }, /* ee: 0100 1101 1110 1110                      */
	{illegal,nullptr   }, /* ef: 0100 1101 1110 1111                      */

	{illegal,nullptr   }, /* f0: 0100 1101 1111 0000                      */
	{illegal,nullptr   }, /* f1: 0100 1101 1111 0001                      */
	{illegal,nullptr   }, /* f2: 0100 1101 1111 0010                      */
	{illegal,nullptr   }, /* f3: 0100 1101 1111 0011                      */
	{illegal,nullptr   }, /* f4: 0100 1101 1111 0100                      */
	{illegal,nullptr   }, /* f5: 0100 1101 1111 0101                      */
	{illegal,nullptr   }, /* f6: 0100 1101 1111 0110                      */
	{illegal,nullptr   }, /* f7: 0100 1101 1111 0111                      */
	{illegal,nullptr   }, /* f8: 0100 1101 1111 1000                      */
	{illegal,nullptr   }, /* f9: 0100 1101 1111 1001                      */
	{illegal,nullptr   }, /* fa: 0100 1101 1111 1010                      */
	{illegal,nullptr   }, /* fb: 0100 1101 1111 1011                      */
	{illegal,nullptr   }, /* fc: 0100 1101 1111 1100                      */
	{illegal,nullptr   }, /* fd: 0100 1101 1111 1101                      */
	{illegal,nullptr   }, /* fe: 0100 1101 1111 1110                      */
	{illegal,nullptr   }  /* ff: 0100 1101 1111 1111                      */
};

/* prefix 60 */
static const struct dasm_s  dasm60[256] =
{
	{illegal,nullptr   }, /* 00: 0110 0000 0000 0000                      */
	{illegal,nullptr   }, /* 01: 0110 0000 0000 0001                      */
	{illegal,nullptr   }, /* 02: 0110 0000 0000 0010                      */
	{illegal,nullptr   }, /* 03: 0110 0000 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0110 0000 0000 0100                      */
	{illegal,nullptr   }, /* 05: 0110 0000 0000 0101                      */
	{illegal,nullptr   }, /* 06: 0110 0000 0000 0110                      */
	{illegal,nullptr   }, /* 07: 0110 0000 0000 0111                      */
	{ANA,"V,A"      }, /* 08: 0110 0000 0000 1000                      */
	{ANA,"A,A"      }, /* 09: 0110 0000 0000 1001                      */
	{ANA,"B,A"      }, /* 0a: 0110 0000 0000 1010                      */
	{ANA,"C,A"      }, /* 0b: 0110 0000 0000 1011                      */
	{ANA,"D,A"      }, /* 0c: 0110 0000 0000 1100                      */
	{ANA,"E,A"      }, /* 0d: 0110 0000 0000 1101                      */
	{ANA,"H,A"      }, /* 0e: 0110 0000 0000 1110                      */
	{ANA,"L,A"      }, /* 0f: 0110 0000 0000 1111                      */

	{XRA,"V,A"      }, /* 10: 0110 0000 0001 0000                      */
	{XRA,"A,A"      }, /* 11: 0110 0000 0001 0001                      */
	{XRA,"B,A"      }, /* 12: 0110 0000 0001 0010                      */
	{XRA,"C,A"      }, /* 13: 0110 0000 0001 0011                      */
	{XRA,"D,A"      }, /* 14: 0110 0000 0001 0100                      */
	{XRA,"E,A"      }, /* 15: 0110 0000 0001 0101                      */
	{XRA,"H,A"      }, /* 16: 0110 0000 0001 0110                      */
	{XRA,"L,A"      }, /* 17: 0110 0000 0001 0111                      */
	{ORA,"V,A"      }, /* 18: 0110 0000 0001 1000                      */
	{ORA,"A,A"      }, /* 19: 0110 0000 0001 1001                      */
	{ORA,"B,A"      }, /* 1a: 0110 0000 0001 1010                      */
	{ORA,"C,A"      }, /* 1b: 0110 0000 0001 1011                      */
	{ORA,"D,A"      }, /* 1c: 0110 0000 0001 1100                      */
	{ORA,"E,A"      }, /* 1d: 0110 0000 0001 1101                      */
	{ORA,"H,A"      }, /* 1e: 0110 0000 0001 1110                      */
	{ORA,"L,A"      }, /* 1f: 0110 0000 0001 1111                      */

	{ADDNC,"V,A"    }, /* 20: 0110 0000 0010 0000                      */
	{ADDNC,"A,A"    }, /* 21: 0110 0000 0010 0001                      */
	{ADDNC,"B,A"    }, /* 22: 0110 0000 0010 0010                      */
	{ADDNC,"C,A"    }, /* 23: 0110 0000 0010 0011                      */
	{ADDNC,"D,A"    }, /* 24: 0110 0000 0010 0100                      */
	{ADDNC,"E,A"    }, /* 25: 0110 0000 0010 0101                      */
	{ADDNC,"H,A"    }, /* 26: 0110 0000 0010 0110                      */
	{ADDNC,"L,A"    }, /* 27: 0110 0000 0010 0111                      */
	{GTA,"V,A"      }, /* 28: 0110 0000 0010 1000                      */
	{GTA,"A,A"      }, /* 29: 0110 0000 0010 1001                      */
	{GTA,"B,A"      }, /* 2a: 0110 0000 0010 1010                      */
	{GTA,"C,A"      }, /* 2b: 0110 0000 0010 1011                      */
	{GTA,"D,A"      }, /* 2c: 0110 0000 0010 1100                      */
	{GTA,"E,A"      }, /* 2d: 0110 0000 0010 1101                      */
	{GTA,"H,A"      }, /* 2e: 0110 0000 0010 1110                      */
	{GTA,"L,A"      }, /* 2f: 0110 0000 0010 1111                      */

	{SUBNB,"V,A"    }, /* 30: 0110 0000 0011 0000                      */
	{SUBNB,"A,A"    }, /* 31: 0110 0000 0011 0001                      */
	{SUBNB,"B,A"    }, /* 32: 0110 0000 0011 0010                      */
	{SUBNB,"C,A"    }, /* 33: 0110 0000 0011 0011                      */
	{SUBNB,"D,A"    }, /* 34: 0110 0000 0011 0100                      */
	{SUBNB,"E,A"    }, /* 35: 0110 0000 0011 0101                      */
	{SUBNB,"H,A"    }, /* 36: 0110 0000 0011 0110                      */
	{SUBNB,"L,A"    }, /* 37: 0110 0000 0011 0111                      */
	{LTA,"V,A"      }, /* 38: 0110 0000 0011 1000                      */
	{LTA,"A,A"      }, /* 39: 0110 0000 0011 1001                      */
	{LTA,"B,A"      }, /* 3a: 0110 0000 0011 1010                      */
	{LTA,"C,A"      }, /* 3b: 0110 0000 0011 1011                      */
	{LTA,"D,A"      }, /* 3c: 0110 0000 0011 1100                      */
	{LTA,"E,A"      }, /* 3d: 0110 0000 0011 1101                      */
	{LTA,"H,A"      }, /* 3e: 0110 0000 0011 1110                      */
	{LTA,"L,A"      }, /* 3f: 0110 0000 0011 1111                      */

	{ADD,"V,A"      }, /* 40: 0110 0000 0100 0000                      */
	{ADD,"A,A"      }, /* 41: 0110 0000 0100 0001                      */
	{ADD,"B,A"      }, /* 42: 0110 0000 0100 0010                      */
	{ADD,"C,A"      }, /* 43: 0110 0000 0100 0011                      */
	{ADD,"D,A"      }, /* 44: 0110 0000 0100 0100                      */
	{ADD,"E,A"      }, /* 45: 0110 0000 0100 0101                      */
	{ADD,"H,A"      }, /* 46: 0110 0000 0100 0110                      */
	{ADD,"L,A"      }, /* 47: 0110 0000 0100 0111                      */
	{illegal,nullptr   }, /* 48: 0110 0000 0100 1000                      */
	{illegal,nullptr   }, /* 49: 0110 0000 0100 1001                      */
	{illegal,nullptr   }, /* 4a: 0110 0000 0100 1010                      */
	{illegal,nullptr   }, /* 4b: 0110 0000 0100 1011                      */
	{illegal,nullptr   }, /* 4c: 0110 0000 0100 1100                      */
	{illegal,nullptr   }, /* 4d: 0110 0000 0100 1101                      */
	{illegal,nullptr   }, /* 4e: 0110 0000 0100 1110                      */
	{illegal,nullptr   }, /* 4f: 0110 0000 0100 1111                      */

	{ADC,"V,A"      }, /* 50: 0110 0000 0101 0000                      */
	{ADC,"A,A"      }, /* 51: 0110 0000 0101 0001                      */
	{ADC,"B,A"      }, /* 52: 0110 0000 0101 0010                      */
	{ADC,"C,A"      }, /* 53: 0110 0000 0101 0011                      */
	{ADC,"D,A"      }, /* 54: 0110 0000 0101 0100                      */
	{ADC,"E,A"      }, /* 55: 0110 0000 0101 0101                      */
	{ADC,"H,A"      }, /* 56: 0110 0000 0101 0110                      */
	{ADC,"L,A"      }, /* 57: 0110 0000 0101 0111                      */
	{illegal,nullptr   }, /* 58: 0110 0000 0101 1000                      */
	{illegal,nullptr   }, /* 59: 0110 0000 0101 1001                      */
	{illegal,nullptr   }, /* 5a: 0110 0000 0101 1010                      */
	{illegal,nullptr   }, /* 5b: 0110 0000 0101 1011                      */
	{illegal,nullptr   }, /* 5c: 0110 0000 0101 1100                      */
	{illegal,nullptr   }, /* 5d: 0110 0000 0101 1101                      */
	{illegal,nullptr   }, /* 5e: 0110 0000 0101 1110                      */
	{illegal,nullptr   }, /* 5f: 0110 0000 0101 1111                      */

	{SUB,"V,A"      }, /* 60: 0110 0000 0110 0000                      */
	{SUB,"A,A"      }, /* 61: 0110 0000 0110 0001                      */
	{SUB,"B,A"      }, /* 62: 0110 0000 0110 0010                      */
	{SUB,"C,A"      }, /* 63: 0110 0000 0110 0011                      */
	{SUB,"D,A"      }, /* 64: 0110 0000 0110 0100                      */
	{SUB,"E,A"      }, /* 65: 0110 0000 0110 0101                      */
	{SUB,"H,A"      }, /* 66: 0110 0000 0110 0110                      */
	{SUB,"L,A"      }, /* 67: 0110 0000 0110 0111                      */
	{NEA,"V,A"      }, /* 68: 0110 0000 0110 1000                      */
	{NEA,"A,A"      }, /* 69: 0110 0000 0110 1001                      */
	{NEA,"B,A"      }, /* 6a: 0110 0000 0110 1010                      */
	{NEA,"C,A"      }, /* 6b: 0110 0000 0110 1011                      */
	{NEA,"D,A"      }, /* 6c: 0110 0000 0110 1100                      */
	{NEA,"E,A"      }, /* 6d: 0110 0000 0110 1101                      */
	{NEA,"H,A"      }, /* 6e: 0110 0000 0110 1110                      */
	{NEA,"L,A"      }, /* 6f: 0110 0000 0110 1111                      */

	{SBB,"V,A"      }, /* 70: 0110 0000 0111 0000                      */
	{SBB,"A,A"      }, /* 71: 0110 0000 0111 0001                      */
	{SBB,"B,A"      }, /* 72: 0110 0000 0111 0010                      */
	{SBB,"C,A"      }, /* 73: 0110 0000 0111 0011                      */
	{SBB,"D,A"      }, /* 74: 0110 0000 0111 0100                      */
	{SBB,"E,A"      }, /* 75: 0110 0000 0111 0101                      */
	{SBB,"H,A"      }, /* 76: 0110 0000 0111 0110                      */
	{SBB,"L,A"      }, /* 77: 0110 0000 0111 0111                      */
	{EQA,"V,A"      }, /* 78: 0110 0000 0111 1000                      */
	{EQA,"A,A"      }, /* 79: 0110 0000 0111 1001                      */
	{EQA,"B,A"      }, /* 7a: 0110 0000 0111 1010                      */
	{EQA,"C,A"      }, /* 7b: 0110 0000 0111 1011                      */
	{EQA,"D,A"      }, /* 7c: 0110 0000 0111 1100                      */
	{EQA,"E,A"      }, /* 7d: 0110 0000 0111 1101                      */
	{EQA,"H,A"      }, /* 7e: 0110 0000 0111 1110                      */
	{EQA,"L,A"      }, /* 7f: 0110 0000 0111 1111                      */

	{illegal,nullptr   }, /* 80: 0110 0000 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0110 0000 1000 0001                      */
	{illegal,nullptr   }, /* 82: 0110 0000 1000 0010                      */
	{illegal,nullptr   }, /* 83: 0110 0000 1000 0011                      */
	{illegal,nullptr   }, /* 84: 0110 0000 1000 0100                      */
	{illegal,nullptr   }, /* 85: 0110 0000 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0110 0000 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0110 0000 1000 0111                      */
	{ANA,"A,V"      }, /* 88: 0110 0000 1000 1000                      */
	{ANA,"A,A"      }, /* 89: 0110 0000 1000 1001                      */
	{ANA,"A,B"      }, /* 8a: 0110 0000 1000 1010                      */
	{ANA,"A,C"      }, /* 8b: 0110 0000 1000 1011                      */
	{ANA,"A,D"      }, /* 8c: 0110 0000 1000 1100                      */
	{ANA,"A,E"      }, /* 8d: 0110 0000 1000 1101                      */
	{ANA,"A,H"      }, /* 8e: 0110 0000 1000 1110                      */
	{ANA,"A,L"      }, /* 8f: 0110 0000 1000 1111                      */

	{XRA,"A,V"      }, /* 90: 0110 0000 1001 0000                      */
	{XRA,"A,A"      }, /* 91: 0110 0000 1001 0001                      */
	{XRA,"A,B"      }, /* 92: 0110 0000 1001 0010                      */
	{XRA,"A,C"      }, /* 93: 0110 0000 1001 0011                      */
	{XRA,"A,D"      }, /* 94: 0110 0000 1001 0100                      */
	{XRA,"A,E"      }, /* 95: 0110 0000 1001 0101                      */
	{XRA,"A,H"      }, /* 96: 0110 0000 1001 0110                      */
	{XRA,"A,L"      }, /* 97: 0110 0000 1001 0111                      */
	{ORA,"A,V"      }, /* 98: 0110 0000 1001 1000                      */
	{ORA,"A,A"      }, /* 99: 0110 0000 1001 1001                      */
	{ORA,"A,B"      }, /* 9a: 0110 0000 1001 1010                      */
	{ORA,"A,C"      }, /* 9b: 0110 0000 1001 1011                      */
	{ORA,"A,D"      }, /* 9c: 0110 0000 1001 1100                      */
	{ORA,"A,E"      }, /* 9d: 0110 0000 1001 1101                      */
	{ORA,"A,H"      }, /* 9e: 0110 0000 1001 1110                      */
	{ORA,"A,L"      }, /* 9f: 0110 0000 1001 1111                      */

	{ADDNC,"A,V"    }, /* a0: 0110 0000 1010 0000                      */
	{ADDNC,"A,A"    }, /* a1: 0110 0000 1010 0001                      */
	{ADDNC,"A,B"    }, /* a2: 0110 0000 1010 0010                      */
	{ADDNC,"A,C"    }, /* a3: 0110 0000 1010 0011                      */
	{ADDNC,"A,D"    }, /* a4: 0110 0000 1010 0100                      */
	{ADDNC,"A,E"    }, /* a5: 0110 0000 1010 0101                      */
	{ADDNC,"A,H"    }, /* a6: 0110 0000 1010 0110                      */
	{ADDNC,"A,L"    }, /* a7: 0110 0000 1010 0111                      */
	{GTA,"A,V"      }, /* a8: 0110 0000 1010 1000                      */
	{GTA,"A,A"      }, /* a9: 0110 0000 1010 1001                      */
	{GTA,"A,B"      }, /* aa: 0110 0000 1010 1010                      */
	{GTA,"A,C"      }, /* ab: 0110 0000 1010 1011                      */
	{GTA,"A,D"      }, /* ac: 0110 0000 1010 1100                      */
	{GTA,"A,E"      }, /* ad: 0110 0000 1010 1101                      */
	{GTA,"A,H"      }, /* ae: 0110 0000 1010 1110                      */
	{GTA,"A,L"      }, /* af: 0110 0000 1010 1111                      */

	{SUBNB,"A,V"    }, /* b0: 0110 0000 1011 0000                      */
	{SUBNB,"A,A"    }, /* b1: 0110 0000 1011 0001                      */
	{SUBNB,"A,B"    }, /* b2: 0110 0000 1011 0010                      */
	{SUBNB,"A,C"    }, /* b3: 0110 0000 1011 0011                      */
	{SUBNB,"A,D"    }, /* b4: 0110 0000 1011 0100                      */
	{SUBNB,"A,E"    }, /* b5: 0110 0000 1011 0101                      */
	{SUBNB,"A,H"    }, /* b6: 0110 0000 1011 0110                      */
	{SUBNB,"A,L"    }, /* b7: 0110 0000 1011 0111                      */
	{LTA,"A,V"      }, /* b8: 0110 0000 1011 1000                      */
	{LTA,"A,A"      }, /* b9: 0110 0000 1011 1001                      */
	{LTA,"A,B"      }, /* ba: 0110 0000 1011 1010                      */
	{LTA,"A,C"      }, /* bb: 0110 0000 1011 1011                      */
	{LTA,"A,D"      }, /* bc: 0110 0000 1011 1100                      */
	{LTA,"A,E"      }, /* bd: 0110 0000 1011 1101                      */
	{LTA,"A,H"      }, /* be: 0110 0000 1011 1110                      */
	{LTA,"A,L"      }, /* bf: 0110 0000 1011 1111                      */

	{ADD,"A,V"      }, /* c0: 0110 0000 1100 0000                      */
	{ADD,"A,A"      }, /* c1: 0110 0000 1100 0001                      */
	{ADD,"A,B"      }, /* c2: 0110 0000 1100 0010                      */
	{ADD,"A,C"      }, /* c3: 0110 0000 1100 0011                      */
	{ADD,"A,D"      }, /* c4: 0110 0000 1100 0100                      */
	{ADD,"A,E"      }, /* c5: 0110 0000 1100 0101                      */
	{ADD,"A,H"      }, /* c6: 0110 0000 1100 0110                      */
	{ADD,"A,L"      }, /* c7: 0110 0000 1100 0111                      */
	{ONA,"A,V"      }, /* c8: 0110 0000 1100 1000                      */
	{ONA,"A,A"      }, /* c9: 0110 0000 1100 1001                      */
	{ONA,"A,B"      }, /* ca: 0110 0000 1100 1010                      */
	{ONA,"A,C"      }, /* cb: 0110 0000 1100 1011                      */
	{ONA,"A,D"      }, /* cc: 0110 0000 1100 1100                      */
	{ONA,"A,E"      }, /* cd: 0110 0000 1100 1101                      */
	{ONA,"A,H"      }, /* ce: 0110 0000 1100 1110                      */
	{ONA,"A,L"      }, /* cf: 0110 0000 1100 1111                      */

	{ADC,"A,V"      }, /* d0: 0110 0000 1101 0000                      */
	{ADC,"A,A"      }, /* d1: 0110 0000 1101 0001                      */
	{ADC,"A,B"      }, /* d2: 0110 0000 1101 0010                      */
	{ADC,"A,C"      }, /* d3: 0110 0000 1101 0011                      */
	{ADC,"A,D"      }, /* d4: 0110 0000 1101 0100                      */
	{ADC,"A,E"      }, /* d5: 0110 0000 1101 0101                      */
	{ADC,"A,H"      }, /* d6: 0110 0000 1101 0110                      */
	{ADC,"A,L"      }, /* d7: 0110 0000 1101 0111                      */
	{OFFA,"A,V"     }, /* d8: 0110 0000 1101 1000                      */
	{OFFA,"A,A"     }, /* d9: 0110 0000 1101 1001                      */
	{OFFA,"A,B"     }, /* da: 0110 0000 1101 1010                      */
	{OFFA,"A,C"     }, /* db: 0110 0000 1101 1011                      */
	{OFFA,"A,D"     }, /* dc: 0110 0000 1101 1100                      */
	{OFFA,"A,E"     }, /* dd: 0110 0000 1101 1101                      */
	{OFFA,"A,H"     }, /* de: 0110 0000 1101 1110                      */
	{OFFA,"A,L"     }, /* df: 0110 0000 1101 1111                      */

	{SUB,"A,V"      }, /* e0: 0110 0000 1110 0000                      */
	{SUB,"A,A"      }, /* e1: 0110 0000 1110 0001                      */
	{SUB,"A,B"      }, /* e2: 0110 0000 1110 0010                      */
	{SUB,"A,C"      }, /* e3: 0110 0000 1110 0011                      */
	{SUB,"A,D"      }, /* e4: 0110 0000 1110 0100                      */
	{SUB,"A,E"      }, /* e5: 0110 0000 1110 0101                      */
	{SUB,"A,H"      }, /* e6: 0110 0000 1110 0110                      */
	{SUB,"A,L"      }, /* e7: 0110 0000 1110 0111                      */
	{NEA,"A,V"      }, /* e8: 0110 0000 1110 1000                      */
	{NEA,"A,A"      }, /* e9: 0110 0000 1110 1001                      */
	{NEA,"A,B"      }, /* ea: 0110 0000 1110 1010                      */
	{NEA,"A,C"      }, /* eb: 0110 0000 1110 1011                      */
	{NEA,"A,D"      }, /* ec: 0110 0000 1110 1100                      */
	{NEA,"A,E"      }, /* ed: 0110 0000 1110 1101                      */
	{NEA,"A,H"      }, /* ee: 0110 0000 1110 1110                      */
	{NEA,"A,L"      }, /* ef: 0110 0000 1110 1111                      */

	{SBB,"A,V"      }, /* f0: 0110 0000 1111 0000                      */
	{SBB,"A,A"      }, /* f1: 0110 0000 1111 0001                      */
	{SBB,"A,B"      }, /* f2: 0110 0000 1111 0010                      */
	{SBB,"A,C"      }, /* f3: 0110 0000 1111 0011                      */
	{SBB,"A,D"      }, /* f4: 0110 0000 1111 0100                      */
	{SBB,"A,E"      }, /* f5: 0110 0000 1111 0101                      */
	{SBB,"A,H"      }, /* f6: 0110 0000 1111 0110                      */
	{SBB,"A,L"      }, /* f7: 0110 0000 1111 0111                      */
	{EQA,"A,V"      }, /* f8: 0110 0000 1111 1000                      */
	{EQA,"A,A"      }, /* f9: 0110 0000 1111 1001                      */
	{EQA,"A,B"      }, /* fa: 0110 0000 1111 1010                      */
	{EQA,"A,C"      }, /* fb: 0110 0000 1111 1011                      */
	{EQA,"A,D"      }, /* fc: 0110 0000 1111 1100                      */
	{EQA,"A,E"      }, /* fd: 0110 0000 1111 1101                      */
	{EQA,"A,H"      }, /* fe: 0110 0000 1111 1110                      */
	{EQA,"A,L"      }  /* ff: 0110 0000 1111 1111                      */
};

/* prefix 64 */
static const struct dasm_s  dasm64_7810[256] =
{
	{MVI,"PA,%b"    }, /* 00: 0110 0100 0000 0000 xxxx xxxx            */
	{MVI,"PB,%b"    }, /* 01: 0110 0100 0000 0001 xxxx xxxx            */
	{MVI,"PC,%b"    }, /* 02: 0110 0100 0000 0010 xxxx xxxx            */
	{MVI,"PD,%b"    }, /* 03: 0110 0100 0000 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 04: 0110 0100 0000 0100 xxxx xxxx            */
	{MVI,"PF,%b"    }, /* 05: 0110 0100 0000 0101 xxxx xxxx            */
	{MVI,"MKH,%b"   }, /* 06: 0110 0100 0000 0110 xxxx xxxx            */
	{MVI,"MKL,%b"   }, /* 07: 0110 0100 0000 0111 xxxx xxxx            */
	{ANI,"PA,%b"    }, /* 08: 0110 0100 0000 1000 xxxx xxxx            */
	{ANI,"PB,%b"    }, /* 09: 0110 0100 0000 1001 xxxx xxxx            */
	{ANI,"PC,%b"    }, /* 0a: 0110 0100 0000 1010 xxxx xxxx            */
	{ANI,"PD,%b"    }, /* 0b: 0110 0100 0000 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 0c: 0110 0100 0000 1100 xxxx xxxx            */
	{ANI,"PF,%b"    }, /* 0d: 0110 0100 0000 1101 xxxx xxxx            */
	{ANI,"MKH,%b"   }, /* 0e: 0110 0100 0000 1110 xxxx xxxx            */
	{ANI,"MKL,%b"   }, /* 0f: 0110 0100 0000 1111 xxxx xxxx            */

	{XRI,"PA,%b"    }, /* 10: 0110 0100 0001 0000 xxxx xxxx            */
	{XRI,"PB,%b"    }, /* 11: 0110 0100 0001 0001 xxxx xxxx            */
	{XRI,"PC,%b"    }, /* 12: 0110 0100 0001 0010 xxxx xxxx            */
	{XRI,"PD,%b"    }, /* 13: 0110 0100 0001 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 14: 0110 0100 0001 0100 xxxx xxxx            */
	{XRI,"PF,%b"    }, /* 15: 0110 0100 0001 0101 xxxx xxxx            */
	{XRI,"MKH,%b"   }, /* 16: 0110 0100 0001 0110 xxxx xxxx            */
	{XRI,"MKL,%b"   }, /* 17: 0110 0100 0001 0111 xxxx xxxx            */
	{ORI,"PA,%b"    }, /* 18: 0110 0100 0001 1000 xxxx xxxx            */
	{ORI,"PB,%b"    }, /* 19: 0110 0100 0001 1001 xxxx xxxx            */
	{ORI,"PC,%b"    }, /* 1a: 0110 0100 0001 1010 xxxx xxxx            */
	{ORI,"PD,%b"    }, /* 1b: 0110 0100 0001 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 1c: 0110 0100 0001 1100 xxxx xxxx            */
	{ORI,"PF,%b"    }, /* 1d: 0110 0100 0001 1101 xxxx xxxx            */
	{ORI,"MKH,%b"   }, /* 1e: 0110 0100 0001 1110 xxxx xxxx            */
	{ORI,"MKL,%b"   }, /* 1f: 0110 0100 0001 1111 xxxx xxxx            */

	{ADINC,"PA,%b"  }, /* 20: 0110 0100 0010 0000 xxxx xxxx            */
	{ADINC,"PB,%b"  }, /* 21: 0110 0100 0010 0001 xxxx xxxx            */
	{ADINC,"PC,%b"  }, /* 22: 0110 0100 0010 0010 xxxx xxxx            */
	{ADINC,"PD,%b"  }, /* 23: 0110 0100 0010 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 24: 0110 0100 0010 0100 xxxx xxxx            */
	{ADINC,"PF,%b"  }, /* 25: 0110 0100 0010 0101 xxxx xxxx            */
	{ADINC,"MKH,%b" }, /* 26: 0110 0100 0010 0110 xxxx xxxx            */
	{ADINC,"MKL,%b" }, /* 27: 0110 0100 0010 0111 xxxx xxxx            */
	{GTI,"PA,%b"    }, /* 28: 0110 0100 0010 1000 xxxx xxxx            */
	{GTI,"PB,%b"    }, /* 29: 0110 0100 0010 1001 xxxx xxxx            */
	{GTI,"PC,%b"    }, /* 2a: 0110 0100 0010 1010 xxxx xxxx            */
	{GTI,"PD,%b"    }, /* 2b: 0110 0100 0010 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 2c: 0110 0100 0010 1100 xxxx xxxx            */
	{GTI,"PF,%b"    }, /* 2d: 0110 0100 0010 1101 xxxx xxxx            */
	{GTI,"MKH,%b"   }, /* 2e: 0110 0100 0010 1110 xxxx xxxx            */
	{GTI,"MKL,%b"   }, /* 2f: 0110 0100 0010 1111 xxxx xxxx            */

	{SUINB,"PA,%b"  }, /* 30: 0110 0100 0011 0000 xxxx xxxx            */
	{SUINB,"PB,%b"  }, /* 31: 0110 0100 0011 0001 xxxx xxxx            */
	{SUINB,"PC,%b"  }, /* 32: 0110 0100 0011 0010 xxxx xxxx            */
	{SUINB,"PD,%b"  }, /* 33: 0110 0100 0011 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 34: 0110 0100 0011 0100 xxxx xxxx            */
	{SUINB,"PF,%b"  }, /* 35: 0110 0100 0011 0101 xxxx xxxx            */
	{SUINB,"MKH,%b" }, /* 36: 0110 0100 0011 0110 xxxx xxxx            */
	{SUINB,"MKL,%b" }, /* 37: 0110 0100 0011 0111 xxxx xxxx            */
	{LTI,"PA,%b"    }, /* 38: 0110 0100 0011 1000 xxxx xxxx            */
	{LTI,"PB,%b"    }, /* 39: 0110 0100 0011 1001 xxxx xxxx            */
	{LTI,"PC,%b"    }, /* 3a: 0110 0100 0011 1010 xxxx xxxx            */
	{LTI,"PD,%b"    }, /* 3b: 0110 0100 0011 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 3c: 0110 0100 0011 1100 xxxx xxxx            */
	{LTI,"PF,%b"    }, /* 3d: 0110 0100 0011 1101 xxxx xxxx            */
	{LTI,"MKH,%b"   }, /* 3e: 0110 0100 0011 1110 xxxx xxxx            */
	{LTI,"MKL,%b"   }, /* 3f: 0110 0100 0011 1111 xxxx xxxx            */

	{ADI,"PA,%b"    }, /* 40: 0110 0100 0100 0000 xxxx xxxx            */
	{ADI,"PB,%b"    }, /* 41: 0110 0100 0100 0001 xxxx xxxx            */
	{ADI,"PC,%b"    }, /* 42: 0110 0100 0100 0010 xxxx xxxx            */
	{ADI,"PD,%b"    }, /* 43: 0110 0100 0100 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 44: 0110 0100 0100 0100 xxxx xxxx            */
	{ADI,"PF,%b"    }, /* 45: 0110 0100 0100 0101 xxxx xxxx            */
	{ADI,"MKH,%b"   }, /* 46: 0110 0100 0100 0110 xxxx xxxx            */
	{ADI,"MKL,%b"   }, /* 47: 0110 0100 0100 0111 xxxx xxxx            */
	{ONI,"PA,%b"    }, /* 48: 0110 0100 0100 1000 xxxx xxxx            */
	{ONI,"PB,%b"    }, /* 49: 0110 0100 0100 1001 xxxx xxxx            */
	{ONI,"PC,%b"    }, /* 4a: 0110 0100 0100 1010 xxxx xxxx            */
	{ONI,"PD,%b"    }, /* 4b: 0110 0100 0100 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 4c: 0110 0100 0100 1100 xxxx xxxx            */
	{ONI,"PF,%b"    }, /* 4d: 0110 0100 0100 1101 xxxx xxxx            */
	{ONI,"MKH,%b"   }, /* 4e: 0110 0100 0100 1110 xxxx xxxx            */
	{ONI,"MKL,%b"   }, /* 4f: 0110 0100 0100 1111 xxxx xxxx            */

	{ACI,"PA,%b"    }, /* 50: 0110 0100 0101 0000 xxxx xxxx            */
	{ACI,"PB,%b"    }, /* 51: 0110 0100 0101 0001 xxxx xxxx            */
	{ACI,"PC,%b"    }, /* 52: 0110 0100 0101 0010 xxxx xxxx            */
	{ACI,"PD,%b"    }, /* 53: 0110 0100 0101 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 54: 0110 0100 0101 0100 xxxx xxxx            */
	{ACI,"PF,%b"    }, /* 55: 0110 0100 0101 0101 xxxx xxxx            */
	{ACI,"MKH,%b"   }, /* 56: 0110 0100 0101 0110 xxxx xxxx            */
	{ACI,"MKL,%b"   }, /* 57: 0110 0100 0101 0111 xxxx xxxx            */
	{OFFI,"PA,%b"   }, /* 58: 0110 0100 0101 1000 xxxx xxxx            */
	{OFFI,"PB,%b"   }, /* 59: 0110 0100 0101 1001 xxxx xxxx            */
	{OFFI,"PC,%b"   }, /* 5a: 0110 0100 0101 1010 xxxx xxxx            */
	{OFFI,"PD,%b"   }, /* 5b: 0110 0100 0101 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 5c: 0110 0100 0101 1100 xxxx xxxx            */
	{OFFI,"PF,%b"   }, /* 5d: 0110 0100 0101 1101 xxxx xxxx            */
	{OFFI,"MKH,%b"  }, /* 5e: 0110 0100 0101 1110 xxxx xxxx            */
	{OFFI,"MKL,%b"  }, /* 5f: 0110 0100 0101 1111 xxxx xxxx            */

	{SUI,"PA,%b"    }, /* 60: 0110 0100 0110 0000 xxxx xxxx            */
	{SUI,"PB,%b"    }, /* 61: 0110 0100 0110 0001 xxxx xxxx            */
	{SUI,"PC,%b"    }, /* 62: 0110 0100 0110 0010 xxxx xxxx            */
	{SUI,"PD,%b"    }, /* 63: 0110 0100 0110 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 64: 0110 0100 0110 0100 xxxx xxxx            */
	{SUI,"PF,%b"    }, /* 65: 0110 0100 0110 0101 xxxx xxxx            */
	{SUI,"MKH,%b"   }, /* 66: 0110 0100 0110 0110 xxxx xxxx            */
	{SUI,"MKL,%b"   }, /* 67: 0110 0100 0110 0111 xxxx xxxx            */
	{NEI,"PA,%b"    }, /* 68: 0110 0100 0110 1000 xxxx xxxx            */
	{NEI,"PB,%b"    }, /* 69: 0110 0100 0110 1001 xxxx xxxx            */
	{NEI,"PC,%b"    }, /* 6a: 0110 0100 0110 1010 xxxx xxxx            */
	{NEI,"PD,%b"    }, /* 6b: 0110 0100 0110 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 6c: 0110 0100 0110 1100 xxxx xxxx            */
	{NEI,"PF,%b"    }, /* 6d: 0110 0100 0110 1101 xxxx xxxx            */
	{NEI,"MKH,%b"   }, /* 6e: 0110 0100 0110 1110 xxxx xxxx            */
	{NEI,"MKL,%b"   }, /* 6f: 0110 0100 0110 1111 xxxx xxxx            */

	{SBI,"PA,%b"    }, /* 70: 0110 0100 0111 0000 xxxx xxxx            */
	{SBI,"PB,%b"    }, /* 71: 0110 0100 0111 0001 xxxx xxxx            */
	{SBI,"PC,%b"    }, /* 72: 0110 0100 0111 0010 xxxx xxxx            */
	{SBI,"PD,%b"    }, /* 73: 0110 0100 0111 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 74: 0110 0100 0111 0100 xxxx xxxx            */
	{SBI,"PF,%b"    }, /* 75: 0110 0100 0111 0101 xxxx xxxx            */
	{SBI,"MKH,%b"   }, /* 76: 0110 0100 0111 0110 xxxx xxxx            */
	{SBI,"MKL,%b"   }, /* 77: 0110 0100 0111 0111 xxxx xxxx            */
	{EQI,"PA,%b"    }, /* 78: 0110 0100 0111 1000 xxxx xxxx            */
	{EQI,"PB,%b"    }, /* 79: 0110 0100 0111 1001 xxxx xxxx            */
	{EQI,"PC,%b"    }, /* 7a: 0110 0100 0111 1010 xxxx xxxx            */
	{EQI,"PD,%b"    }, /* 7b: 0110 0100 0111 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 7c: 0110 0100 0111 1100 xxxx xxxx            */
	{EQI,"PF,%b"    }, /* 7d: 0110 0100 0111 1101 xxxx xxxx            */
	{EQI,"MKH,%b"   }, /* 7e: 0110 0100 0111 1110 xxxx xxxx            */
	{EQI,"MKL,%b"   }, /* 7f: 0110 0100 0111 1111 xxxx xxxx            */

	{MVI,"ANM,%b"   }, /* 80: 0110 0100 1000 0000 xxxx xxxx            */   /* 7810 */
	{MVI,"SMH,%b"   }, /* 81: 0110 0100 1000 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* 82: 0110 0100 1000 0010 xxxx xxxx            */
	{MVI,"EOM,%b"   }, /* 83: 0110 0100 1000 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 84: 0110 0100 1000 0100 xxxx xxxx            */
	{MVI,"TMM,%b"   }, /* 85: 0110 0100 1000 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* 86: 0110 0100 1000 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* 87: 0110 0100 1000 0111 xxxx xxxx            */
	{ANI,"ANM,%b"   }, /* 88: 0110 0100 1000 1000 xxxx xxxx            */   /* 7810 */
	{ANI,"SMH,%b"   }, /* 89: 0110 0100 1000 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* 8a: 0110 0100 1000 1010 xxxx xxxx            */
	{ANI,"EOM,%b"   }, /* 8b: 0110 0100 1000 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 8c: 0110 0100 1000 1100 xxxx xxxx            */
	{ANI,"TMM,%b"   }, /* 8d: 0110 0100 1000 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* 8e: 0110 0100 1000 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* 8f: 0110 0100 1000 1111 xxxx xxxx            */

	{XRI,"ANM,%b"   }, /* 90: 0110 0100 1001 0000 xxxx xxxx            */   /* 7810 */
	{XRI,"SMH,%b"   }, /* 91: 0110 0100 1001 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* 92: 0110 0100 1001 0010 xxxx xxxx            */
	{XRI,"EOM,%b"   }, /* 93: 0110 0100 1001 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 94: 0110 0100 1001 0100 xxxx xxxx            */
	{XRI,"TMM,%b"   }, /* 95: 0110 0100 1001 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* 96: 0110 0100 1001 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* 97: 0110 0100 1001 0111 xxxx xxxx            */
	{ORI,"ANM,%b"   }, /* 98: 0110 0100 1001 1000 xxxx xxxx            */   /* 7810 */
	{ORI,"SMH,%b"   }, /* 99: 0110 0100 1001 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* 9a: 0110 0100 1001 1010 xxxx xxxx            */
	{ORI,"EOM,%b"   }, /* 9b: 0110 0100 1001 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 9c: 0110 0100 1001 1100 xxxx xxxx            */
	{ORI,"TMM,%b"   }, /* 9d: 0110 0100 1001 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* 9e: 0110 0100 1001 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* 9f: 0110 0100 1001 1111 xxxx xxxx            */

	{ADINC,"ANM,%b" }, /* a0: 0110 0100 1010 0000 xxxx xxxx            */   /* 7810 */
	{ADINC,"SMH,%b" }, /* a1: 0110 0100 1010 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* a2: 0110 0100 1010 0010 xxxx xxxx            */
	{ADINC,"EOM,%b" }, /* a3: 0110 0100 1010 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* a4: 0110 0100 1010 0100 xxxx xxxx            */
	{ADINC,"TMM,%b" }, /* a5: 0110 0100 1010 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* a6: 0110 0100 1010 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* a7: 0110 0100 1010 0111 xxxx xxxx            */
	{GTI,"ANM,%b"   }, /* a8: 0110 0100 1010 1000 xxxx xxxx            */   /* 7810 */
	{GTI,"SMH,%b"   }, /* a9: 0110 0100 1010 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* aa: 0110 0100 1010 1010 xxxx xxxx            */
	{GTI,"EOM,%b"   }, /* ab: 0110 0100 1010 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* ac: 0110 0100 1010 1100 xxxx xxxx            */
	{GTI,"TMM,%b"   }, /* ad: 0110 0100 1010 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* ae: 0110 0100 1010 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* af: 0110 0100 1010 1111 xxxx xxxx            */

	{SUINB,"ANM,%b" }, /* b0: 0110 0100 1011 0000 xxxx xxxx            */   /* 7810 */
	{SUINB,"SMH,%b" }, /* b1: 0110 0100 1011 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* b2: 0110 0100 1011 0010 xxxx xxxx            */
	{SUINB,"EOM,%b" }, /* b3: 0110 0100 1011 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* b4: 0110 0100 1011 0100 xxxx xxxx            */
	{SUINB,"TMM,%b" }, /* b5: 0110 0100 1011 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* b6: 0110 0100 1011 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* b7: 0110 0100 1011 0111 xxxx xxxx            */
	{LTI,"ANM,%b"   }, /* b8: 0110 0100 1011 1000 xxxx xxxx            */   /* 7810 */
	{LTI,"SMH,%b"   }, /* b9: 0110 0100 1011 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* ba: 0110 0100 1011 1010 xxxx xxxx            */
	{LTI,"EOM,%b"   }, /* bb: 0110 0100 1011 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* bc: 0110 0100 1011 1100 xxxx xxxx            */
	{LTI,"TMM,%b"   }, /* bd: 0110 0100 1011 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* be: 0110 0100 1011 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* bf: 0110 0100 1011 1111 xxxx xxxx            */

	{ADI,"ANM,%b"   }, /* c0: 0110 0100 1100 0000 xxxx xxxx            */   /* 7810 */
	{ADI,"SMH,%b"   }, /* c1: 0110 0100 1100 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* c2: 0110 0100 1100 0010 xxxx xxxx            */
	{ADI,"EOM,%b"   }, /* c3: 0110 0100 1100 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* c4: 0110 0100 1100 0100 xxxx xxxx            */
	{ADI,"TMM,%b"   }, /* c5: 0110 0100 1100 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* c6: 0110 0100 1100 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* c7: 0110 0100 1100 0111 xxxx xxxx            */
	{ONI,"ANM,%b"   }, /* c8: 0110 0100 1100 1000 xxxx xxxx            */   /* 7810 */
	{ONI,"SMH,%b"   }, /* c9: 0110 0100 1100 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* ca: 0110 0100 1100 1010 xxxx xxxx            */
	{ONI,"EOM,%b"   }, /* cb: 0110 0100 1100 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* cc: 0110 0100 1100 1100 xxxx xxxx            */
	{ONI,"TMM,%b"   }, /* cd: 0110 0100 1100 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* ce: 0110 0100 1100 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* cf: 0110 0100 1100 1111 xxxx xxxx            */

	{ACI,"ANM,%b"   }, /* d0: 0110 0100 1101 0000 xxxx xxxx            */   /* 7810 */
	{ACI,"SMH,%b"   }, /* d1: 0110 0100 1101 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* d2: 0110 0100 1101 0010 xxxx xxxx            */
	{ACI,"EOM,%b"   }, /* d3: 0110 0100 1101 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* d4: 0110 0100 1101 0100 xxxx xxxx            */
	{ACI,"TMM,%b"   }, /* d5: 0110 0100 1101 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* d6: 0110 0100 1101 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* d7: 0110 0100 1101 0111 xxxx xxxx            */
	{OFFI,"ANM,%b"  }, /* d8: 0110 0100 1101 1000 xxxx xxxx            */   /* 7810 */
	{OFFI,"SMH,%b"  }, /* d9: 0110 0100 1101 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* da: 0110 0100 1101 1010 xxxx xxxx            */
	{OFFI,"EOM,%b"  }, /* db: 0110 0100 1101 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* dc: 0110 0100 1101 1100 xxxx xxxx            */
	{OFFI,"TMM,%b"  }, /* dd: 0110 0100 1101 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* de: 0110 0100 1101 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* df: 0110 0100 1101 1111 xxxx xxxx            */

	{SUI,"ANM,%b"   }, /* e0: 0110 0100 1110 0000 xxxx xxxx            */   /* 7810 */
	{SUI,"SMH,%b"   }, /* e1: 0110 0100 1110 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* e2: 0110 0100 1110 0010 xxxx xxxx            */
	{SUI,"EOM,%b"   }, /* e3: 0110 0100 1110 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* e4: 0110 0100 1110 0100 xxxx xxxx            */
	{SUI,"TMM,%b"   }, /* e5: 0110 0100 1110 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* e6: 0110 0100 1110 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* e7: 0110 0100 1110 0111 xxxx xxxx            */
	{NEI,"ANM,%b"   }, /* e8: 0110 0100 1110 1000 xxxx xxxx            */   /* 7810 */
	{NEI,"SMH,%b"   }, /* e9: 0110 0100 1110 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* ea: 0110 0100 1110 1010 xxxx xxxx            */
	{NEI,"EOM,%b"   }, /* eb: 0110 0100 1110 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* ec: 0110 0100 1110 1100 xxxx xxxx            */
	{NEI,"TMM,%b"   }, /* ed: 0110 0100 1110 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* ee: 0110 0100 1110 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* ef: 0110 0100 1110 1111 xxxx xxxx            */

	{SBI,"ANM,%b"   }, /* f0: 0110 0100 1111 0000 xxxx xxxx            */   /* 7810 */
	{SBI,"SMH,%b"   }, /* f1: 0110 0100 1111 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* f2: 0110 0100 1111 0010 xxxx xxxx            */
	{SBI,"EOM,%b"   }, /* f3: 0110 0100 1111 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* f4: 0110 0100 1111 0100 xxxx xxxx            */
	{SBI,"TMM,%b"   }, /* f5: 0110 0100 1111 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* f6: 0110 0100 1111 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* f7: 0110 0100 1111 0111 xxxx xxxx            */
	{EQI,"ANM,%b"   }, /* f8: 0110 0100 1111 1000 xxxx xxxx            */   /* 7810 */
	{EQI,"SMH,%b"   }, /* f9: 0110 0100 1111 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* fa: 0110 0100 1111 1010 xxxx xxxx            */
	{EQI,"EOM,%b"   }, /* fb: 0110 0100 1111 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* fc: 0110 0100 1111 1100 xxxx xxxx            */
	{EQI,"TMM,%b"   }, /* fd: 0110 0100 1111 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* fe: 0110 0100 1111 1110 xxxx xxxx            */
	{illegal,nullptr   }  /* ff: 0110 0100 1111 1111 xxxx xxxx            */
};

static const struct dasm_s  dasm64_7807[256] =
{
	{MVI,"PA,%b"    }, /* 00: 0110 0100 0000 0000 xxxx xxxx            */
	{MVI,"PB,%b"    }, /* 01: 0110 0100 0000 0001 xxxx xxxx            */
	{MVI,"PC,%b"    }, /* 02: 0110 0100 0000 0010 xxxx xxxx            */
	{MVI,"PD,%b"    }, /* 03: 0110 0100 0000 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 04: 0110 0100 0000 0100 xxxx xxxx            */
	{MVI,"PF,%b"    }, /* 05: 0110 0100 0000 0101 xxxx xxxx            */
	{MVI,"MKH,%b"   }, /* 06: 0110 0100 0000 0110 xxxx xxxx            */
	{MVI,"MKL,%b"   }, /* 07: 0110 0100 0000 0111 xxxx xxxx            */
	{ANI,"PA,%b"    }, /* 08: 0110 0100 0000 1000 xxxx xxxx            */
	{ANI,"PB,%b"    }, /* 09: 0110 0100 0000 1001 xxxx xxxx            */
	{ANI,"PC,%b"    }, /* 0a: 0110 0100 0000 1010 xxxx xxxx            */
	{ANI,"PD,%b"    }, /* 0b: 0110 0100 0000 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 0c: 0110 0100 0000 1100 xxxx xxxx            */
	{ANI,"PF,%b"    }, /* 0d: 0110 0100 0000 1101 xxxx xxxx            */
	{ANI,"MKH,%b"   }, /* 0e: 0110 0100 0000 1110 xxxx xxxx            */
	{ANI,"MKL,%b"   }, /* 0f: 0110 0100 0000 1111 xxxx xxxx            */

	{XRI,"PA,%b"    }, /* 10: 0110 0100 0001 0000 xxxx xxxx            */
	{XRI,"PB,%b"    }, /* 11: 0110 0100 0001 0001 xxxx xxxx            */
	{XRI,"PC,%b"    }, /* 12: 0110 0100 0001 0010 xxxx xxxx            */
	{XRI,"PD,%b"    }, /* 13: 0110 0100 0001 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 14: 0110 0100 0001 0100 xxxx xxxx            */
	{XRI,"PF,%b"    }, /* 15: 0110 0100 0001 0101 xxxx xxxx            */
	{XRI,"MKH,%b"   }, /* 16: 0110 0100 0001 0110 xxxx xxxx            */
	{XRI,"MKL,%b"   }, /* 17: 0110 0100 0001 0111 xxxx xxxx            */
	{ORI,"PA,%b"    }, /* 18: 0110 0100 0001 1000 xxxx xxxx            */
	{ORI,"PB,%b"    }, /* 19: 0110 0100 0001 1001 xxxx xxxx            */
	{ORI,"PC,%b"    }, /* 1a: 0110 0100 0001 1010 xxxx xxxx            */
	{ORI,"PD,%b"    }, /* 1b: 0110 0100 0001 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 1c: 0110 0100 0001 1100 xxxx xxxx            */
	{ORI,"PF,%b"    }, /* 1d: 0110 0100 0001 1101 xxxx xxxx            */
	{ORI,"MKH,%b"   }, /* 1e: 0110 0100 0001 1110 xxxx xxxx            */
	{ORI,"MKL,%b"   }, /* 1f: 0110 0100 0001 1111 xxxx xxxx            */

	{ADINC,"PA,%b"  }, /* 20: 0110 0100 0010 0000 xxxx xxxx            */
	{ADINC,"PB,%b"  }, /* 21: 0110 0100 0010 0001 xxxx xxxx            */
	{ADINC,"PC,%b"  }, /* 22: 0110 0100 0010 0010 xxxx xxxx            */
	{ADINC,"PD,%b"  }, /* 23: 0110 0100 0010 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 24: 0110 0100 0010 0100 xxxx xxxx            */
	{ADINC,"PF,%b"  }, /* 25: 0110 0100 0010 0101 xxxx xxxx            */
	{ADINC,"MKH,%b" }, /* 26: 0110 0100 0010 0110 xxxx xxxx            */
	{ADINC,"MKL,%b" }, /* 27: 0110 0100 0010 0111 xxxx xxxx            */
	{GTI,"PA,%b"    }, /* 28: 0110 0100 0010 1000 xxxx xxxx            */
	{GTI,"PB,%b"    }, /* 29: 0110 0100 0010 1001 xxxx xxxx            */
	{GTI,"PC,%b"    }, /* 2a: 0110 0100 0010 1010 xxxx xxxx            */
	{GTI,"PD,%b"    }, /* 2b: 0110 0100 0010 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 2c: 0110 0100 0010 1100 xxxx xxxx            */
	{GTI,"PF,%b"    }, /* 2d: 0110 0100 0010 1101 xxxx xxxx            */
	{GTI,"MKH,%b"   }, /* 2e: 0110 0100 0010 1110 xxxx xxxx            */
	{GTI,"MKL,%b"   }, /* 2f: 0110 0100 0010 1111 xxxx xxxx            */

	{SUINB,"PA,%b"  }, /* 30: 0110 0100 0011 0000 xxxx xxxx            */
	{SUINB,"PB,%b"  }, /* 31: 0110 0100 0011 0001 xxxx xxxx            */
	{SUINB,"PC,%b"  }, /* 32: 0110 0100 0011 0010 xxxx xxxx            */
	{SUINB,"PD,%b"  }, /* 33: 0110 0100 0011 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 34: 0110 0100 0011 0100 xxxx xxxx            */
	{SUINB,"PF,%b"  }, /* 35: 0110 0100 0011 0101 xxxx xxxx            */
	{SUINB,"MKH,%b" }, /* 36: 0110 0100 0011 0110 xxxx xxxx            */
	{SUINB,"MKL,%b" }, /* 37: 0110 0100 0011 0111 xxxx xxxx            */
	{LTI,"PA,%b"    }, /* 38: 0110 0100 0011 1000 xxxx xxxx            */
	{LTI,"PB,%b"    }, /* 39: 0110 0100 0011 1001 xxxx xxxx            */
	{LTI,"PC,%b"    }, /* 3a: 0110 0100 0011 1010 xxxx xxxx            */
	{LTI,"PD,%b"    }, /* 3b: 0110 0100 0011 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 3c: 0110 0100 0011 1100 xxxx xxxx            */
	{LTI,"PF,%b"    }, /* 3d: 0110 0100 0011 1101 xxxx xxxx            */
	{LTI,"MKH,%b"   }, /* 3e: 0110 0100 0011 1110 xxxx xxxx            */
	{LTI,"MKL,%b"   }, /* 3f: 0110 0100 0011 1111 xxxx xxxx            */

	{ADI,"PA,%b"    }, /* 40: 0110 0100 0100 0000 xxxx xxxx            */
	{ADI,"PB,%b"    }, /* 41: 0110 0100 0100 0001 xxxx xxxx            */
	{ADI,"PC,%b"    }, /* 42: 0110 0100 0100 0010 xxxx xxxx            */
	{ADI,"PD,%b"    }, /* 43: 0110 0100 0100 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 44: 0110 0100 0100 0100 xxxx xxxx            */
	{ADI,"PF,%b"    }, /* 45: 0110 0100 0100 0101 xxxx xxxx            */
	{ADI,"MKH,%b"   }, /* 46: 0110 0100 0100 0110 xxxx xxxx            */
	{ADI,"MKL,%b"   }, /* 47: 0110 0100 0100 0111 xxxx xxxx            */
	{ONI,"PA,%b"    }, /* 48: 0110 0100 0100 1000 xxxx xxxx            */
	{ONI,"PB,%b"    }, /* 49: 0110 0100 0100 1001 xxxx xxxx            */
	{ONI,"PC,%b"    }, /* 4a: 0110 0100 0100 1010 xxxx xxxx            */
	{ONI,"PD,%b"    }, /* 4b: 0110 0100 0100 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 4c: 0110 0100 0100 1100 xxxx xxxx            */
	{ONI,"PF,%b"    }, /* 4d: 0110 0100 0100 1101 xxxx xxxx            */
	{ONI,"MKH,%b"   }, /* 4e: 0110 0100 0100 1110 xxxx xxxx            */
	{ONI,"MKL,%b"   }, /* 4f: 0110 0100 0100 1111 xxxx xxxx            */

	{ACI,"PA,%b"    }, /* 50: 0110 0100 0101 0000 xxxx xxxx            */
	{ACI,"PB,%b"    }, /* 51: 0110 0100 0101 0001 xxxx xxxx            */
	{ACI,"PC,%b"    }, /* 52: 0110 0100 0101 0010 xxxx xxxx            */
	{ACI,"PD,%b"    }, /* 53: 0110 0100 0101 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 54: 0110 0100 0101 0100 xxxx xxxx            */
	{ACI,"PF,%b"    }, /* 55: 0110 0100 0101 0101 xxxx xxxx            */
	{ACI,"MKH,%b"   }, /* 56: 0110 0100 0101 0110 xxxx xxxx            */
	{ACI,"MKL,%b"   }, /* 57: 0110 0100 0101 0111 xxxx xxxx            */
	{OFFI,"PA,%b"   }, /* 58: 0110 0100 0101 1000 xxxx xxxx            */
	{OFFI,"PB,%b"   }, /* 59: 0110 0100 0101 1001 xxxx xxxx            */
	{OFFI,"PC,%b"   }, /* 5a: 0110 0100 0101 1010 xxxx xxxx            */
	{OFFI,"PD,%b"   }, /* 5b: 0110 0100 0101 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 5c: 0110 0100 0101 1100 xxxx xxxx            */
	{OFFI,"PF,%b"   }, /* 5d: 0110 0100 0101 1101 xxxx xxxx            */
	{OFFI,"MKH,%b"  }, /* 5e: 0110 0100 0101 1110 xxxx xxxx            */
	{OFFI,"MKL,%b"  }, /* 5f: 0110 0100 0101 1111 xxxx xxxx            */

	{SUI,"PA,%b"    }, /* 60: 0110 0100 0110 0000 xxxx xxxx            */
	{SUI,"PB,%b"    }, /* 61: 0110 0100 0110 0001 xxxx xxxx            */
	{SUI,"PC,%b"    }, /* 62: 0110 0100 0110 0010 xxxx xxxx            */
	{SUI,"PD,%b"    }, /* 63: 0110 0100 0110 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 64: 0110 0100 0110 0100 xxxx xxxx            */
	{SUI,"PF,%b"    }, /* 65: 0110 0100 0110 0101 xxxx xxxx            */
	{SUI,"MKH,%b"   }, /* 66: 0110 0100 0110 0110 xxxx xxxx            */
	{SUI,"MKL,%b"   }, /* 67: 0110 0100 0110 0111 xxxx xxxx            */
	{NEI,"PA,%b"    }, /* 68: 0110 0100 0110 1000 xxxx xxxx            */
	{NEI,"PB,%b"    }, /* 69: 0110 0100 0110 1001 xxxx xxxx            */
	{NEI,"PC,%b"    }, /* 6a: 0110 0100 0110 1010 xxxx xxxx            */
	{NEI,"PD,%b"    }, /* 6b: 0110 0100 0110 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 6c: 0110 0100 0110 1100 xxxx xxxx            */
	{NEI,"PF,%b"    }, /* 6d: 0110 0100 0110 1101 xxxx xxxx            */
	{NEI,"MKH,%b"   }, /* 6e: 0110 0100 0110 1110 xxxx xxxx            */
	{NEI,"MKL,%b"   }, /* 6f: 0110 0100 0110 1111 xxxx xxxx            */

	{SBI,"PA,%b"    }, /* 70: 0110 0100 0111 0000 xxxx xxxx            */
	{SBI,"PB,%b"    }, /* 71: 0110 0100 0111 0001 xxxx xxxx            */
	{SBI,"PC,%b"    }, /* 72: 0110 0100 0111 0010 xxxx xxxx            */
	{SBI,"PD,%b"    }, /* 73: 0110 0100 0111 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 74: 0110 0100 0111 0100 xxxx xxxx            */
	{SBI,"PF,%b"    }, /* 75: 0110 0100 0111 0101 xxxx xxxx            */
	{SBI,"MKH,%b"   }, /* 76: 0110 0100 0111 0110 xxxx xxxx            */
	{SBI,"MKL,%b"   }, /* 77: 0110 0100 0111 0111 xxxx xxxx            */
	{EQI,"PA,%b"    }, /* 78: 0110 0100 0111 1000 xxxx xxxx            */
	{EQI,"PB,%b"    }, /* 79: 0110 0100 0111 1001 xxxx xxxx            */
	{EQI,"PC,%b"    }, /* 7a: 0110 0100 0111 1010 xxxx xxxx            */
	{EQI,"PD,%b"    }, /* 7b: 0110 0100 0111 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 7c: 0110 0100 0111 1100 xxxx xxxx            */
	{EQI,"PF,%b"    }, /* 7d: 0110 0100 0111 1101 xxxx xxxx            */
	{EQI,"MKH,%b"   }, /* 7e: 0110 0100 0111 1110 xxxx xxxx            */
	{EQI,"MKL,%b"   }, /* 7f: 0110 0100 0111 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* 80: 0110 0100 1000 0000 xxxx xxxx            */
	{MVI,"SMH,%b"   }, /* 81: 0110 0100 1000 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* 82: 0110 0100 1000 0010 xxxx xxxx            */
	{MVI,"EOM,%b"   }, /* 83: 0110 0100 1000 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 84: 0110 0100 1000 0100 xxxx xxxx            */
	{MVI,"TMM,%b"   }, /* 85: 0110 0100 1000 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* 86: 0110 0100 1000 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* 87: 0110 0100 1000 0111 xxxx xxxx            */
	{illegal,nullptr   }, /* 88: 0110 0100 1000 1000 xxxx xxxx            */
	{ANI,"SMH,%b"   }, /* 89: 0110 0100 1000 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* 8a: 0110 0100 1000 1010 xxxx xxxx            */
	{ANI,"EOM,%b"   }, /* 8b: 0110 0100 1000 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 8c: 0110 0100 1000 1100 xxxx xxxx            */
	{ANI,"TMM,%b"   }, /* 8d: 0110 0100 1000 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* 8e: 0110 0100 1000 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* 8f: 0110 0100 1000 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* 90: 0110 0100 1001 0000 xxxx xxxx            */
	{XRI,"SMH,%b"   }, /* 91: 0110 0100 1001 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* 92: 0110 0100 1001 0010 xxxx xxxx            */
	{XRI,"EOM,%b"   }, /* 93: 0110 0100 1001 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* 94: 0110 0100 1001 0100 xxxx xxxx            */
	{XRI,"TMM,%b"   }, /* 95: 0110 0100 1001 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* 96: 0110 0100 1001 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* 97: 0110 0100 1001 0111 xxxx xxxx            */
	{illegal,nullptr   }, /* 98: 0110 0100 1001 1000 xxxx xxxx            */
	{ORI,"SMH,%b"   }, /* 99: 0110 0100 1001 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* 9a: 0110 0100 1001 1010 xxxx xxxx            */
	{ORI,"EOM,%b"   }, /* 9b: 0110 0100 1001 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* 9c: 0110 0100 1001 1100 xxxx xxxx            */
	{ORI,"TMM,%b"   }, /* 9d: 0110 0100 1001 1101 xxxx xxxx            */
	{illegal,nullptr   }, /* 9e: 0110 0100 1001 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* 9f: 0110 0100 1001 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* a0: 0110 0100 1010 0000 xxxx xxxx            */
	{ADINC,"SMH,%b" }, /* a1: 0110 0100 1010 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* a2: 0110 0100 1010 0010 xxxx xxxx            */
	{ADINC,"EOM,%b" }, /* a3: 0110 0100 1010 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* a4: 0110 0100 1010 0100 xxxx xxxx            */
	{ADINC,"TMM,%b" }, /* a5: 0110 0100 1010 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* a6: 0110 0100 1010 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* a7: 0110 0100 1010 0111 xxxx xxxx            */
	{illegal,nullptr   }, /* a8: 0110 0100 1010 1000 xxxx xxxx            */
	{GTI,"SMH,%b"   }, /* a9: 0110 0100 1010 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* aa: 0110 0100 1010 1010 xxxx xxxx            */
	{GTI,"EOM,%b"   }, /* ab: 0110 0100 1010 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* ac: 0110 0100 1010 1100 xxxx xxxx            */
	{GTI,"TMM,%b"   }, /* ad: 0110 0100 1010 1101 xxxx xxxx            */
	{GTI,"PT,%b"    }, /* ae: 0110 0100 1010 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* af: 0110 0100 1010 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* b0: 0110 0100 1011 0000 xxxx xxxx            */
	{SUINB,"SMH,%b" }, /* b1: 0110 0100 1011 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* b2: 0110 0100 1011 0010 xxxx xxxx            */
	{SUINB,"EOM,%b" }, /* b3: 0110 0100 1011 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* b4: 0110 0100 1011 0100 xxxx xxxx            */
	{SUINB,"TMM,%b" }, /* b5: 0110 0100 1011 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* b6: 0110 0100 1011 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* b7: 0110 0100 1011 0111 xxxx xxxx            */
	{illegal,nullptr   }, /* b8: 0110 0100 1011 1000 xxxx xxxx            */
	{LTI,"SMH,%b"   }, /* b9: 0110 0100 1011 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* ba: 0110 0100 1011 1010 xxxx xxxx            */
	{LTI,"EOM,%b"   }, /* bb: 0110 0100 1011 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* bc: 0110 0100 1011 1100 xxxx xxxx            */
	{LTI,"TMM,%b"   }, /* bd: 0110 0100 1011 1101 xxxx xxxx            */
	{LTI,"PT,%b"    }, /* be: 0110 0100 1011 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* bf: 0110 0100 1011 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* c0: 0110 0100 1100 0000 xxxx xxxx            */
	{ADI,"SMH,%b"   }, /* c1: 0110 0100 1100 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* c2: 0110 0100 1100 0010 xxxx xxxx            */
	{ADI,"EOM,%b"   }, /* c3: 0110 0100 1100 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* c4: 0110 0100 1100 0100 xxxx xxxx            */
	{ADI,"TMM,%b"   }, /* c5: 0110 0100 1100 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* c6: 0110 0100 1100 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* c7: 0110 0100 1100 0111 xxxx xxxx            */
	{illegal,nullptr   }, /* c8: 0110 0100 1100 1000 xxxx xxxx            */
	{ONI,"SMH,%b"   }, /* c9: 0110 0100 1100 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* ca: 0110 0100 1100 1010 xxxx xxxx            */
	{ONI,"EOM,%b"   }, /* cb: 0110 0100 1100 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* cc: 0110 0100 1100 1100 xxxx xxxx            */
	{ONI,"TMM,%b"   }, /* cd: 0110 0100 1100 1101 xxxx xxxx            */
	{ONI,"PT,%b"    }, /* ce: 0110 0100 1100 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* cf: 0110 0100 1100 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* d0: 0110 0100 1101 0000 xxxx xxxx            */
	{ACI,"SMH,%b"   }, /* d1: 0110 0100 1101 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* d2: 0110 0100 1101 0010 xxxx xxxx            */
	{ACI,"EOM,%b"   }, /* d3: 0110 0100 1101 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* d4: 0110 0100 1101 0100 xxxx xxxx            */
	{ACI,"TMM,%b"   }, /* d5: 0110 0100 1101 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* d6: 0110 0100 1101 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* d7: 0110 0100 1101 0111 xxxx xxxx            */
	{illegal,nullptr   }, /* d8: 0110 0100 1101 1000 xxxx xxxx            */
	{OFFI,"SMH,%b"  }, /* d9: 0110 0100 1101 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* da: 0110 0100 1101 1010 xxxx xxxx            */
	{OFFI,"EOM,%b"  }, /* db: 0110 0100 1101 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* dc: 0110 0100 1101 1100 xxxx xxxx            */
	{OFFI,"TMM,%b"  }, /* dd: 0110 0100 1101 1101 xxxx xxxx            */
	{OFFI,"PT,%b"   }, /* de: 0110 0100 1101 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* df: 0110 0100 1101 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* e0: 0110 0100 1110 0000 xxxx xxxx            */
	{SUI,"SMH,%b"   }, /* e1: 0110 0100 1110 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* e2: 0110 0100 1110 0010 xxxx xxxx            */
	{SUI,"EOM,%b"   }, /* e3: 0110 0100 1110 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* e4: 0110 0100 1110 0100 xxxx xxxx            */
	{SUI,"TMM,%b"   }, /* e5: 0110 0100 1110 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* e6: 0110 0100 1110 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* e7: 0110 0100 1110 0111 xxxx xxxx            */
	{illegal,nullptr   }, /* e8: 0110 0100 1110 1000 xxxx xxxx            */
	{NEI,"SMH,%b"   }, /* e9: 0110 0100 1110 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* ea: 0110 0100 1110 1010 xxxx xxxx            */
	{NEI,"EOM,%b"   }, /* eb: 0110 0100 1110 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* ec: 0110 0100 1110 1100 xxxx xxxx            */
	{NEI,"TMM,%b"   }, /* ed: 0110 0100 1110 1101 xxxx xxxx            */
	{NEI,"PT,%b"    }, /* ee: 0110 0100 1110 1110 xxxx xxxx            */
	{illegal,nullptr   }, /* ef: 0110 0100 1110 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* f0: 0110 0100 1111 0000 xxxx xxxx            */
	{SBI,"SMH,%b"   }, /* f1: 0110 0100 1111 0001 xxxx xxxx            */
	{illegal,nullptr   }, /* f2: 0110 0100 1111 0010 xxxx xxxx            */
	{SBI,"EOM,%b"   }, /* f3: 0110 0100 1111 0011 xxxx xxxx            */
	{illegal,nullptr   }, /* f4: 0110 0100 1111 0100 xxxx xxxx            */
	{SBI,"TMM,%b"   }, /* f5: 0110 0100 1111 0101 xxxx xxxx            */
	{illegal,nullptr   }, /* f6: 0110 0100 1111 0110 xxxx xxxx            */
	{illegal,nullptr   }, /* f7: 0110 0100 1111 0111 xxxx xxxx            */
	{illegal,nullptr   }, /* f8: 0110 0100 1111 1000 xxxx xxxx            */
	{EQI,"SMH,%b"   }, /* f9: 0110 0100 1111 1001 xxxx xxxx            */
	{illegal,nullptr   }, /* fa: 0110 0100 1111 1010 xxxx xxxx            */
	{EQI,"EOM,%b"   }, /* fb: 0110 0100 1111 1011 xxxx xxxx            */
	{illegal,nullptr   }, /* fc: 0110 0100 1111 1100 xxxx xxxx            */
	{EQI,"TMM,%b"   }, /* fd: 0110 0100 1111 1101 xxxx xxxx            */
	{EQI,"PT,%b"    }, /* fe: 0110 0100 1111 1110 xxxx xxxx            */
	{illegal,nullptr   }  /* ff: 0110 0100 1111 1111 xxxx xxxx            */
};

/* prefix 70 */
static const struct dasm_s  dasm70[256] =
{
	{illegal,nullptr   }, /* 00: 0111 0000 0000 0000                      */
	{illegal,nullptr   }, /* 01: 0111 0000 0000 0001                      */
	{illegal,nullptr   }, /* 02: 0111 0000 0000 0010                      */
	{illegal,nullptr   }, /* 03: 0111 0000 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0111 0000 0000 0100                      */
	{illegal,nullptr   }, /* 05: 0111 0000 0000 0101                      */
	{illegal,nullptr   }, /* 06: 0111 0000 0000 0110                      */
	{illegal,nullptr   }, /* 07: 0111 0000 0000 0111                      */
	{illegal,nullptr   }, /* 08: 0111 0000 0000 1000                      */
	{illegal,nullptr   }, /* 09: 0111 0000 0000 1001                      */
	{illegal,nullptr   }, /* 0a: 0111 0000 0000 1010                      */
	{illegal,nullptr   }, /* 0b: 0111 0000 0000 1011                      */
	{illegal,nullptr   }, /* 0c: 0111 0000 0000 1100                      */
	{illegal,nullptr   }, /* 0d: 0111 0000 0000 1101                      */
	{SSPD,"%w"      }, /* 0e: 0111 0000 0000 1110 llll llll hhhh hhhh  */
	{LSPD,"%w"      }, /* 0f: 0111 0000 0000 1111 llll llll hhhh hhhh  */

	{illegal,nullptr   }, /* 10: 0111 0000 0001 0000                      */
	{illegal,nullptr   }, /* 11: 0111 0000 0001 0001                      */
	{illegal,nullptr   }, /* 12: 0111 0000 0001 0010                      */
	{illegal,nullptr   }, /* 13: 0111 0000 0001 0011                      */
	{illegal,nullptr   }, /* 14: 0111 0000 0001 0100                      */
	{illegal,nullptr   }, /* 15: 0111 0000 0001 0101                      */
	{illegal,nullptr   }, /* 16: 0111 0000 0001 0110                      */
	{illegal,nullptr   }, /* 17: 0111 0000 0001 0111                      */
	{illegal,nullptr   }, /* 18: 0111 0000 0001 1000                      */
	{illegal,nullptr   }, /* 19: 0111 0000 0001 1001                      */
	{illegal,nullptr   }, /* 1a: 0111 0000 0001 1010                      */
	{illegal,nullptr   }, /* 1b: 0111 0000 0001 1011                      */
	{illegal,nullptr   }, /* 1c: 0111 0000 0001 1100                      */
	{illegal,nullptr   }, /* 1d: 0111 0000 0001 1101                      */
	{SBCD,"%w"      }, /* 1e: 0111 0000 0001 1110 llll llll hhhh hhhh  */
	{LBCD,"%w"      }, /* 1f: 0111 0000 0001 1111 llll llll hhhh hhhh  */

	{illegal,nullptr   }, /* 20: 0111 0000 0010 0000                      */
	{illegal,nullptr   }, /* 21: 0111 0000 0010 0001                      */
	{illegal,nullptr   }, /* 22: 0111 0000 0010 0010                      */
	{illegal,nullptr   }, /* 23: 0111 0000 0010 0011                      */
	{illegal,nullptr   }, /* 24: 0111 0000 0010 0100                      */
	{illegal,nullptr   }, /* 25: 0111 0000 0010 0101                      */
	{illegal,nullptr   }, /* 26: 0111 0000 0010 0110                      */
	{illegal,nullptr   }, /* 27: 0111 0000 0010 0111                      */
	{illegal,nullptr   }, /* 28: 0111 0000 0010 1000                      */
	{illegal,nullptr   }, /* 29: 0111 0000 0010 1001                      */
	{illegal,nullptr   }, /* 2a: 0111 0000 0010 1010                      */
	{illegal,nullptr   }, /* 2b: 0111 0000 0010 1011                      */
	{illegal,nullptr   }, /* 2c: 0111 0000 0010 1100                      */
	{illegal,nullptr   }, /* 2d: 0111 0000 0010 1101                      */
	{SDED,"%w"      }, /* 2e: 0111 0000 0010 1110 llll llll hhhh hhhh  */
	{LDED,"%w"      }, /* 2f: 0111 0000 0010 1111 llll llll hhhh hhhh  */

	{illegal,nullptr   }, /* 30: 0111 0000 0011 0000                      */
	{illegal,nullptr   }, /* 31: 0111 0000 0011 0001                      */
	{illegal,nullptr   }, /* 32: 0111 0000 0011 0010                      */
	{illegal,nullptr   }, /* 33: 0111 0000 0011 0011                      */
	{illegal,nullptr   }, /* 34: 0111 0000 0011 0100                      */
	{illegal,nullptr   }, /* 35: 0111 0000 0011 0101                      */
	{illegal,nullptr   }, /* 36: 0111 0000 0011 0110                      */
	{illegal,nullptr   }, /* 37: 0111 0000 0011 0111                      */
	{illegal,nullptr   }, /* 38: 0111 0000 0011 1000                      */
	{illegal,nullptr   }, /* 39: 0111 0000 0011 1001                      */
	{illegal,nullptr   }, /* 3a: 0111 0000 0011 1010                      */
	{illegal,nullptr   }, /* 3b: 0111 0000 0011 1011                      */
	{illegal,nullptr   }, /* 3c: 0111 0000 0011 1100                      */
	{illegal,nullptr   }, /* 3d: 0111 0000 0011 1101                      */
	{SHLD,"%w"      }, /* 3e: 0111 0000 0011 1110 llll llll hhhh hhhh  */
	{LHLD,"%w"      }, /* 3f: 0111 0000 0011 1111 llll llll hhhh hhhh  */

	{EADD,"EA,V"    }, /* 40: 0111 0000 0100 0000                      */
	{EADD,"EA,A"    }, /* 41: 0111 0000 0100 0001                      */
	{EADD,"EA,B"    }, /* 42: 0111 0000 0100 0010                      */
	{EADD,"EA,C"    }, /* 43: 0111 0000 0100 0011                      */
	{illegal,nullptr   }, /* 44: 0111 0000 0100 0100                      */
	{illegal,nullptr   }, /* 45: 0111 0000 0100 0101                      */
	{illegal,nullptr   }, /* 46: 0111 0000 0100 0110                      */
	{illegal,nullptr   }, /* 47: 0111 0000 0100 0111                      */
	{illegal,nullptr   }, /* 48: 0111 0000 0100 1000                      */
	{illegal,nullptr   }, /* 49: 0111 0000 0100 1001                      */
	{illegal,nullptr   }, /* 4a: 0111 0000 0100 1010                      */
	{illegal,nullptr   }, /* 4b: 0111 0000 0100 1011                      */
	{illegal,nullptr   }, /* 4c: 0111 0000 0100 1100                      */
	{illegal,nullptr   }, /* 4d: 0111 0000 0100 1101                      */
	{illegal,nullptr   }, /* 4e: 0111 0000 0100 1110                      */
	{illegal,nullptr   }, /* 4f: 0111 0000 0100 1111                      */

	{illegal,nullptr   }, /* 50: 0111 0000 0101 0000                      */
	{illegal,nullptr   }, /* 51: 0111 0000 0101 0001                      */
	{illegal,nullptr   }, /* 52: 0111 0000 0101 0010                      */
	{illegal,nullptr   }, /* 53: 0111 0000 0101 0011                      */
	{illegal,nullptr   }, /* 54: 0111 0000 0101 0100                      */
	{illegal,nullptr   }, /* 55: 0111 0000 0101 0101                      */
	{illegal,nullptr   }, /* 56: 0111 0000 0101 0110                      */
	{illegal,nullptr   }, /* 57: 0111 0000 0101 0111                      */
	{illegal,nullptr   }, /* 58: 0111 0000 0101 1000                      */
	{illegal,nullptr   }, /* 59: 0111 0000 0101 1001                      */
	{illegal,nullptr   }, /* 5a: 0111 0000 0101 1010                      */
	{illegal,nullptr   }, /* 5b: 0111 0000 0101 1011                      */
	{illegal,nullptr   }, /* 5c: 0111 0000 0101 1100                      */
	{illegal,nullptr   }, /* 5d: 0111 0000 0101 1101                      */
	{illegal,nullptr   }, /* 5e: 0111 0000 0101 1110                      */
	{illegal,nullptr   }, /* 5f: 0111 0000 0101 1111                      */

	{ESUB,"EA,V"    }, /* 60: 0111 0000 0110 0000                      */
	{ESUB,"EA,A"    }, /* 61: 0111 0000 0110 0001                      */
	{ESUB,"EA,B"    }, /* 62: 0111 0000 0110 0010                      */
	{ESUB,"EA,C"    }, /* 63: 0111 0000 0110 0011                      */
	{illegal,nullptr   }, /* 64: 0111 0000 0110 0100                      */
	{illegal,nullptr   }, /* 65: 0111 0000 0110 0101                      */
	{illegal,nullptr   }, /* 66: 0111 0000 0110 0110                      */
	{illegal,nullptr   }, /* 67: 0111 0000 0110 0111                      */
	{MOV,"V,(%w)"   }, /* 68: 0111 0000 0110 1000 llll llll hhhh hhhh  */
	{MOV,"A,(%w)"   }, /* 69: 0111 0000 0110 1001 llll llll hhhh hhhh  */
	{MOV,"B,(%w)"   }, /* 6a: 0111 0000 0110 1010 llll llll hhhh hhhh  */
	{MOV,"C,(%w)"   }, /* 6b: 0111 0000 0110 1011 llll llll hhhh hhhh  */
	{MOV,"D,(%w)"   }, /* 6c: 0111 0000 0110 1100 llll llll hhhh hhhh  */
	{MOV,"E,(%w)"   }, /* 6d: 0111 0000 0110 1101 llll llll hhhh hhhh  */
	{MOV,"H,(%w)"   }, /* 6e: 0111 0000 0110 1110 llll llll hhhh hhhh  */
	{MOV,"L,(%w)"   }, /* 6f: 0111 0000 0110 1111 llll llll hhhh hhhh  */

	{illegal,nullptr   }, /* 70: 0111 0000 0111 0000                      */
	{illegal,nullptr   }, /* 71: 0111 0000 0111 0001                      */
	{illegal,nullptr   }, /* 72: 0111 0000 0111 0010                      */
	{illegal,nullptr   }, /* 73: 0111 0000 0111 0011                      */
	{illegal,nullptr   }, /* 74: 0111 0000 0111 0100                      */
	{illegal,nullptr   }, /* 75: 0111 0000 0111 0101                      */
	{illegal,nullptr   }, /* 76: 0111 0000 0111 0110                      */
	{illegal,nullptr   }, /* 77: 0111 0000 0111 0111                      */
	{MOV,"(%w),V"   }, /* 78: 0111 0000 0111 1000 llll llll hhhh hhhh  */
	{MOV,"(%w),A"   }, /* 79: 0111 0000 0111 1001 llll llll hhhh hhhh  */
	{MOV,"(%w),B"   }, /* 7a: 0111 0000 0111 1010 llll llll hhhh hhhh  */
	{MOV,"(%w),C"   }, /* 7b: 0111 0000 0111 1011 llll llll hhhh hhhh  */
	{MOV,"(%w),D"   }, /* 7c: 0111 0000 0111 1100 llll llll hhhh hhhh  */
	{MOV,"(%w),E"   }, /* 7d: 0111 0000 0111 1101 llll llll hhhh hhhh  */
	{MOV,"(%w),H"   }, /* 7e: 0111 0000 0111 1110 llll llll hhhh hhhh  */
	{MOV,"(%w),L"   }, /* 7f: 0111 0000 0111 1111 llll llll hhhh hhhh  */

	{illegal,nullptr   }, /* 80: 0111 0000 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0111 0000 1000 0001                      */
	{illegal,nullptr   }, /* 82: 0111 0000 1000 0010                      */
	{illegal,nullptr   }, /* 83: 0111 0000 1000 0011                      */
	{illegal,nullptr   }, /* 84: 0111 0000 1000 0100                      */
	{illegal,nullptr   }, /* 85: 0111 0000 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0111 0000 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0111 0000 1000 0111                      */
	{illegal,nullptr   }, /* 88: 0111 0000 1000 1000                      */
	{ANAX,"(BC)"    }, /* 89: 0111 0000 1000 1001                      */
	{ANAX,"(DE)"    }, /* 8a: 0111 0000 1000 1010                      */
	{ANAX,"(HL)"    }, /* 8b: 0111 0000 1000 1011                      */
	{ANAX,"(DE+)"   }, /* 8c: 0111 0000 1000 1100                      */
	{ANAX,"(HL+)"   }, /* 8d: 0111 0000 1000 1101                      */
	{ANAX,"(DE-)"   }, /* 8e: 0111 0000 1000 1110                      */
	{ANAX,"(HL-)"   }, /* 8f: 0111 0000 1000 1111                      */

	{illegal,nullptr   }, /* 90: 0111 0000 1001 0000                      */
	{XRAX,"(BC)"    }, /* 91: 0111 0000 1001 0001                      */
	{XRAX,"(DE)"    }, /* 92: 0111 0000 1001 0010                      */
	{XRAX,"(HL)"    }, /* 93: 0111 0000 1001 0011                      */
	{XRAX,"(DE+)"   }, /* 94: 0111 0000 1001 0100                      */
	{XRAX,"(HL+)"   }, /* 95: 0111 0000 1001 0101                      */
	{XRAX,"(DE-)"   }, /* 96: 0111 0000 1001 0110                      */
	{XRAX,"(HL-)"   }, /* 97: 0111 0000 1001 0111                      */
	{illegal,nullptr   }, /* 98: 0111 0000 1001 1000                      */
	{ORAX,"(BC)"     }, /* 99: 0111 0000 1001 1001                      */
	{ORAX,"(DE)"     }, /* 9a: 0111 0000 1001 1010                      */
	{ORAX,"(HL)"     }, /* 9b: 0111 0000 1001 1011                      */
	{ORAX,"(DE+)"    }, /* 9c: 0111 0000 1001 1100                      */
	{ORAX,"(HL+)"    }, /* 9d: 0111 0000 1001 1101                      */
	{ORAX,"(DE-)"    }, /* 9e: 0111 0000 1001 1110                      */
	{ORAX,"(HL-)"    }, /* 9f: 0111 0000 1001 1111                      */

	{illegal,nullptr   }, /* a0: 0111 0000 1010 0000                      */
	{ADDNCX,"(BC)"  }, /* a1: 0111 0000 1010 0001                      */
	{ADDNCX,"(DE)"  }, /* a2: 0111 0000 1010 0010                      */
	{ADDNCX,"(HL)"  }, /* a3: 0111 0000 1010 0011                      */
	{ADDNCX,"(DE+)" }, /* a4: 0111 0000 1010 0100                      */
	{ADDNCX,"(HL+)" }, /* a5: 0111 0000 1010 0101                      */
	{ADDNCX,"(DE-)" }, /* a6: 0111 0000 1010 0110                      */
	{ADDNCX,"(HL-)" }, /* a7: 0111 0000 1010 0111                      */
	{illegal,nullptr   }, /* a8: 0111 0000 1010 1000                      */
	{GTAX,"(BC)"    }, /* a9: 0111 0000 1010 1001                      */
	{GTAX,"(DE)"    }, /* aa: 0111 0000 1010 1010                      */
	{GTAX,"(HL)"    }, /* ab: 0111 0000 1010 1011                      */
	{GTAX,"(DE+)"   }, /* ac: 0111 0000 1010 1100                      */
	{GTAX,"(HL+)"   }, /* ad: 0111 0000 1010 1101                      */
	{GTAX,"(DE-)"   }, /* ae: 0111 0000 1010 1110                      */
	{GTAX,"(HL-)"   }, /* af: 0111 0000 1010 1111                      */

	{illegal,nullptr   }, /* b0: 0111 0000 1011 0000                      */
	{SUBNBX,"(BC)"  }, /* b1: 0111 0000 1011 0001                      */
	{SUBNBX,"(DE)"  }, /* b2: 0111 0000 1011 0010                      */
	{SUBNBX,"(HL)"  }, /* b3: 0111 0000 1011 0011                      */
	{SUBNBX,"(DE+)" }, /* b4: 0111 0000 1011 0100                      */
	{SUBNBX,"(HL+)" }, /* b5: 0111 0000 1011 0101                      */
	{SUBNBX,"(DE-)" }, /* b6: 0111 0000 1011 0110                      */
	{SUBNBX,"(HL-)" }, /* b7: 0111 0000 1011 0111                      */
	{illegal,nullptr   }, /* b8: 0111 0000 1011 1000                      */
	{LTAX,"(BC)"    }, /* b9: 0111 0000 1011 1001                      */
	{LTAX,"(DE)"    }, /* ba: 0111 0000 1011 1010                      */
	{LTAX,"(HL)"    }, /* bb: 0111 0000 1011 1011                      */
	{LTAX,"(DE+)"   }, /* bc: 0111 0000 1011 1100                      */
	{LTAX,"(HL+)"   }, /* bd: 0111 0000 1011 1101                      */
	{LTAX,"(DE-)"   }, /* be: 0111 0000 1011 1110                      */
	{LTAX,"(HL-)"   }, /* bf: 0111 0000 1011 1111                      */

	{illegal,nullptr   }, /* c0: 0111 0000 1100 0000                      */
	{ADDX,"(BC)"    }, /* c1: 0111 0000 1100 0001                      */
	{ADDX,"(DE)"    }, /* c2: 0111 0000 1100 0010                      */
	{ADDX,"(HL)"    }, /* c3: 0111 0000 1100 0011                      */
	{ADDX,"(DE+)"   }, /* c4: 0111 0000 1100 0100                      */
	{ADDX,"(HL+)"   }, /* c5: 0111 0000 1100 0101                      */
	{ADDX,"(DE-)"   }, /* c6: 0111 0000 1100 0110                      */
	{ADDX,"(HL-)"   }, /* c7: 0111 0000 1100 0111                      */
	{illegal,nullptr   }, /* c8: 0111 0000 1100 1000                      */
	{ONAX,"(BC)"    }, /* c9: 0111 0000 1100 1001                      */
	{ONAX,"(DE)"    }, /* ca: 0111 0000 1100 1010                      */
	{ONAX,"(HL)"    }, /* cb: 0111 0000 1100 1011                      */
	{ONAX,"(DE+)"   }, /* cc: 0111 0000 1100 1100                      */
	{ONAX,"(HL+)"   }, /* cd: 0111 0000 1100 1101                      */
	{ONAX,"(DE-)"   }, /* ce: 0111 0000 1100 1110                      */
	{ONAX,"(HL-)"   }, /* cf: 0111 0000 1100 1111                      */

	{illegal,nullptr   }, /* d0: 0111 0000 1101 0000                      */
	{ADCX,"(BC)"    }, /* d1: 0111 0000 1101 0001                      */
	{ADCX,"(DE)"    }, /* d2: 0111 0000 1101 0010                      */
	{ADCX,"(HL)"    }, /* d3: 0111 0000 1101 0011                      */
	{ADCX,"(DE+)"   }, /* d4: 0111 0000 1101 0100                      */
	{ADCX,"(HL+)"   }, /* d5: 0111 0000 1101 0101                      */
	{ADCX,"(DE-)"   }, /* d6: 0111 0000 1101 0110                      */
	{ADCX,"(HL-)"   }, /* d7: 0111 0000 1101 0111                      */
	{illegal,nullptr   }, /* d8: 0111 0000 1101 1000                      */
	{OFFAX,"(BC)"   }, /* d9: 0111 0000 1101 1001                      */
	{OFFAX,"(DE)"   }, /* da: 0111 0000 1101 1010                      */
	{OFFAX,"(HL)"   }, /* db: 0111 0000 1101 1011                      */
	{OFFAX,"(DE+)"  }, /* dc: 0111 0000 1101 1100                      */
	{OFFAX,"(HL+)"  }, /* dd: 0111 0000 1101 1101                      */
	{OFFAX,"(DE-)"  }, /* de: 0111 0000 1101 1110                      */
	{OFFAX,"(HL-)"  }, /* df: 0111 0000 1101 1111                      */

	{illegal,nullptr   }, /* e0: 0111 0000 1110 0000                      */
	{SUBX,"(BC)"    }, /* e1: 0111 0000 1110 0001                      */
	{SUBX,"(DE)"    }, /* e2: 0111 0000 1110 0010                      */
	{SUBX,"(HL)"    }, /* e3: 0111 0000 1110 0011                      */
	{SUBX,"(DE+)"   }, /* e4: 0111 0000 1110 0100                      */
	{SUBX,"(HL+)"   }, /* e5: 0111 0000 1110 0101                      */
	{SUBX,"(DE-)"   }, /* e6: 0111 0000 1110 0110                      */
	{SUBX,"(HL-)"   }, /* e7: 0111 0000 1110 0111                      */
	{illegal,nullptr   }, /* e8: 0111 0000 1110 1000                      */
	{NEAX,"(BC)"    }, /* e9: 0111 0000 1110 1001                      */
	{NEAX,"(DE)"    }, /* ea: 0111 0000 1110 1010                      */
	{NEAX,"(HL)"    }, /* eb: 0111 0000 1110 1011                      */
	{NEAX,"(DE+)"   }, /* ec: 0111 0000 1110 1100                      */
	{NEAX,"(HL+)"   }, /* ed: 0111 0000 1110 1101                      */
	{NEAX,"(DE-)"   }, /* ee: 0111 0000 1110 1110                      */
	{NEAX,"(HL-)"   }, /* ef: 0111 0000 1110 1111                      */

	{illegal,nullptr   }, /* f0: 0111 0000 1111 0000                      */
	{SBBX,"(BC)"    }, /* f1: 0111 0000 1111 0001                      */
	{SBBX,"(DE)"    }, /* f2: 0111 0000 1111 0010                      */
	{SBBX,"(HL)"    }, /* f3: 0111 0000 1111 0011                      */
	{SBBX,"(DE+)"   }, /* f4: 0111 0000 1111 0100                      */
	{SBBX,"(HL+)"   }, /* f5: 0111 0000 1111 0101                      */
	{SBBX,"(DE-)"   }, /* f6: 0111 0000 1111 0110                      */
	{SBBX,"(HL-)"   }, /* f7: 0111 0000 1111 0111                      */
	{illegal,nullptr   }, /* f8: 0111 0000 1111 1000                      */
	{EQAX,"(BC)"    }, /* f9: 0111 0000 1111 1001                      */
	{EQAX,"(DE)"    }, /* fa: 0111 0000 1111 1010                      */
	{EQAX,"(HL)"    }, /* fb: 0111 0000 1111 1011                      */
	{EQAX,"(DE+)"   }, /* fc: 0111 0000 1111 1100                      */
	{EQAX,"(HL+)"   }, /* fd: 0111 0000 1111 1101                      */
	{EQAX,"(DE-)"   }, /* fe: 0111 0000 1111 1110                      */
	{EQAX,"(HL-)"   }  /* ff: 0111 0000 1111 1111                      */
};

/* prefix 74 */
static const struct dasm_s  dasm74[256] =
{
	{illegal,nullptr   }, /* 00: 0111 0100 0000 0000                      */
	{illegal,nullptr   }, /* 01: 0111 0100 0000 0001                      */
	{illegal,nullptr   }, /* 02: 0111 0100 0000 0010                      */
	{illegal,nullptr   }, /* 03: 0111 0100 0000 0011                      */
	{illegal,nullptr   }, /* 04: 0111 0100 0000 0100                      */
	{illegal,nullptr   }, /* 05: 0111 0100 0000 0101                      */
	{illegal,nullptr   }, /* 06: 0111 0100 0000 0110                      */
	{illegal,nullptr   }, /* 07: 0111 0100 0000 0111                      */
	{ANI,"V,%b"     }, /* 08: 0111 0100 0000 1000 xxxx xxxx            */
	{ANI,"A,%b"     }, /* 09: 0111 0100 0000 1001 xxxx xxxx            */
	{ANI,"B,%b"     }, /* 0a: 0111 0100 0000 1010 xxxx xxxx            */
	{ANI,"C,%b"     }, /* 0b: 0111 0100 0000 1011 xxxx xxxx            */
	{ANI,"D,%b"     }, /* 0c: 0111 0100 0000 1100 xxxx xxxx            */
	{ANI,"E,%b"     }, /* 0d: 0111 0100 0000 1101 xxxx xxxx            */
	{ANI,"H,%b"     }, /* 0e: 0111 0100 0000 1110 xxxx xxxx            */
	{ANI,"L,%b"     }, /* 0f: 0111 0100 0000 1111 xxxx xxxx            */

	{XRI,"V,%b"     }, /* 10: 0111 0100 0001 0000 xxxx xxxx            */
	{XRI,"A,%b"     }, /* 11: 0111 0100 0001 0001 xxxx xxxx            */
	{XRI,"B,%b"     }, /* 12: 0111 0100 0001 0010 xxxx xxxx            */
	{XRI,"C,%b"     }, /* 13: 0111 0100 0001 0011 xxxx xxxx            */
	{XRI,"D,%b"     }, /* 14: 0111 0100 0001 0100 xxxx xxxx            */
	{XRI,"E,%b"     }, /* 15: 0111 0100 0001 0101 xxxx xxxx            */
	{XRI,"H,%b"     }, /* 16: 0111 0100 0001 0110 xxxx xxxx            */
	{XRI,"L,%b"     }, /* 17: 0111 0100 0001 0111 xxxx xxxx            */
	{ORI,"V,%b"     }, /* 18: 0111 0100 0001 1000 xxxx xxxx            */
	{ORI,"A,%b"     }, /* 19: 0111 0100 0001 1001 xxxx xxxx            */
	{ORI,"B,%b"     }, /* 1a: 0111 0100 0001 1010 xxxx xxxx            */
	{ORI,"C,%b"     }, /* 1b: 0111 0100 0001 1011 xxxx xxxx            */
	{ORI,"D,%b"     }, /* 1c: 0111 0100 0001 1100 xxxx xxxx            */
	{ORI,"E,%b"     }, /* 1d: 0111 0100 0001 1101 xxxx xxxx            */
	{ORI,"H,%b"     }, /* 1e: 0111 0100 0001 1110 xxxx xxxx            */
	{ORI,"L,%b"     }, /* 1f: 0111 0100 0001 1111 xxxx xxxx            */

	{ADINC,"V,%b"   }, /* 20: 0111 0100 0010 0000 xxxx xxxx            */
	{ADINC,"A,%b"   }, /* 21: 0111 0100 0010 0001 xxxx xxxx            */
	{ADINC,"B,%b"   }, /* 22: 0111 0100 0010 0010 xxxx xxxx            */
	{ADINC,"C,%b"   }, /* 23: 0111 0100 0010 0011 xxxx xxxx            */
	{ADINC,"D,%b"   }, /* 24: 0111 0100 0010 0100 xxxx xxxx            */
	{ADINC,"E,%b"   }, /* 25: 0111 0100 0010 0101 xxxx xxxx            */
	{ADINC,"H,%b"   }, /* 26: 0111 0100 0010 0110 xxxx xxxx            */
	{ADINC,"L,%b"   }, /* 27: 0111 0100 0010 0111 xxxx xxxx            */
	{GTI,"V,%b"     }, /* 28: 0111 0100 0010 1000 xxxx xxxx            */
	{GTI,"A,%b"     }, /* 29: 0111 0100 0010 1001 xxxx xxxx            */
	{GTI,"B,%b"     }, /* 2a: 0111 0100 0010 1010 xxxx xxxx            */
	{GTI,"C,%b"     }, /* 2b: 0111 0100 0010 1011 xxxx xxxx            */
	{GTI,"D,%b"     }, /* 2c: 0111 0100 0010 1100 xxxx xxxx            */
	{GTI,"E,%b"     }, /* 2d: 0111 0100 0010 1101 xxxx xxxx            */
	{GTI,"H,%b"     }, /* 2e: 0111 0100 0010 1110 xxxx xxxx            */
	{GTI,"L,%b"     }, /* 2f: 0111 0100 0010 1111 xxxx xxxx            */

	{SUINB,"V,%b"   }, /* 30: 0111 0100 0011 0000 xxxx xxxx            */
	{SUINB,"A,%b"   }, /* 31: 0111 0100 0011 0001 xxxx xxxx            */
	{SUINB,"B,%b"   }, /* 32: 0111 0100 0011 0010 xxxx xxxx            */
	{SUINB,"C,%b"   }, /* 33: 0111 0100 0011 0011 xxxx xxxx            */
	{SUINB,"D,%b"   }, /* 34: 0111 0100 0011 0100 xxxx xxxx            */
	{SUINB,"E,%b"   }, /* 35: 0111 0100 0011 0101 xxxx xxxx            */
	{SUINB,"H,%b"   }, /* 36: 0111 0100 0011 0110 xxxx xxxx            */
	{SUINB,"L,%b"   }, /* 37: 0111 0100 0011 0111 xxxx xxxx            */
	{LTI,"V,%b"     }, /* 38: 0111 0100 0011 1000 xxxx xxxx            */
	{LTI,"A,%b"     }, /* 39: 0111 0100 0011 1001 xxxx xxxx            */
	{LTI,"B,%b"     }, /* 3a: 0111 0100 0011 1010 xxxx xxxx            */
	{LTI,"C,%b"     }, /* 3b: 0111 0100 0011 1011 xxxx xxxx            */
	{LTI,"D,%b"     }, /* 3c: 0111 0100 0011 1100 xxxx xxxx            */
	{LTI,"E,%b"     }, /* 3d: 0111 0100 0011 1101 xxxx xxxx            */
	{LTI,"H,%b"     }, /* 3e: 0111 0100 0011 1110 xxxx xxxx            */
	{LTI,"L,%b"     }, /* 3f: 0111 0100 0011 1111 xxxx xxxx            */

	{ADI,"V,%b"     }, /* 40: 0111 0100 0100 0000 xxxx xxxx            */
	{ADI,"A,%b"     }, /* 41: 0111 0100 0100 0001 xxxx xxxx            */
	{ADI,"B,%b"     }, /* 42: 0111 0100 0100 0010 xxxx xxxx            */
	{ADI,"C,%b"     }, /* 43: 0111 0100 0100 0011 xxxx xxxx            */
	{ADI,"D,%b"     }, /* 44: 0111 0100 0100 0100 xxxx xxxx            */
	{ADI,"E,%b"     }, /* 45: 0111 0100 0100 0101 xxxx xxxx            */
	{ADI,"H,%b"     }, /* 46: 0111 0100 0100 0110 xxxx xxxx            */
	{ADI,"L,%b"     }, /* 47: 0111 0100 0100 0111 xxxx xxxx            */
	{ONI,"V,%b"     }, /* 48: 0111 0100 0100 1000 xxxx xxxx            */
	{ONI,"A,%b"     }, /* 49: 0111 0100 0100 1001 xxxx xxxx            */
	{ONI,"B,%b"     }, /* 4a: 0111 0100 0100 1010 xxxx xxxx            */
	{ONI,"C,%b"     }, /* 4b: 0111 0100 0100 1011 xxxx xxxx            */
	{ONI,"D,%b"     }, /* 4c: 0111 0100 0100 1100 xxxx xxxx            */
	{ONI,"E,%b"     }, /* 4d: 0111 0100 0100 1101 xxxx xxxx            */
	{ONI,"H,%b"     }, /* 4e: 0111 0100 0100 1110 xxxx xxxx            */
	{ONI,"L,%b"     }, /* 4f: 0111 0100 0100 1111 xxxx xxxx            */

	{ACI,"V,%b"     }, /* 50: 0111 0100 0101 0000 xxxx xxxx            */
	{ACI,"A,%b"     }, /* 51: 0111 0100 0101 0001 xxxx xxxx            */
	{ACI,"B,%b"     }, /* 52: 0111 0100 0101 0010 xxxx xxxx            */
	{ACI,"C,%b"     }, /* 53: 0111 0100 0101 0011 xxxx xxxx            */
	{ACI,"D,%b"     }, /* 54: 0111 0100 0101 0100 xxxx xxxx            */
	{ACI,"E,%b"     }, /* 55: 0111 0100 0101 0101 xxxx xxxx            */
	{ACI,"H,%b"     }, /* 56: 0111 0100 0101 0110 xxxx xxxx            */
	{ACI,"L,%b"     }, /* 57: 0111 0100 0101 0111 xxxx xxxx            */
	{OFFI,"V,%b"    }, /* 58: 0111 0100 0101 1000 xxxx xxxx            */
	{OFFI,"A,%b"    }, /* 59: 0111 0100 0101 1001 xxxx xxxx            */
	{OFFI,"B,%b"    }, /* 5a: 0111 0100 0101 1010 xxxx xxxx            */
	{OFFI,"C,%b"    }, /* 5b: 0111 0100 0101 1011 xxxx xxxx            */
	{OFFI,"D,%b"    }, /* 5c: 0111 0100 0101 1100 xxxx xxxx            */
	{OFFI,"E,%b"    }, /* 5d: 0111 0100 0101 1101 xxxx xxxx            */
	{OFFI,"H,%b"    }, /* 5e: 0111 0100 0101 1110 xxxx xxxx            */
	{OFFI,"L,%b"    }, /* 5f: 0111 0100 0101 1111 xxxx xxxx            */

	{SUI,"V,%b"     }, /* 60: 0111 0100 0110 0000 xxxx xxxx            */
	{SUI,"A,%b"     }, /* 61: 0111 0100 0110 0001 xxxx xxxx            */
	{SUI,"B,%b"     }, /* 62: 0111 0100 0110 0010 xxxx xxxx            */
	{SUI,"C,%b"     }, /* 63: 0111 0100 0110 0011 xxxx xxxx            */
	{SUI,"D,%b"     }, /* 64: 0111 0100 0110 0100 xxxx xxxx            */
	{SUI,"E,%b"     }, /* 65: 0111 0100 0110 0101 xxxx xxxx            */
	{SUI,"H,%b"     }, /* 66: 0111 0100 0110 0110 xxxx xxxx            */
	{SUI,"L,%b"     }, /* 67: 0111 0100 0110 0111 xxxx xxxx            */
	{NEI,"V,%b"     }, /* 68: 0111 0100 0110 1000 xxxx xxxx            */
	{NEI,"A,%b"     }, /* 69: 0111 0100 0110 1001 xxxx xxxx            */
	{NEI,"B,%b"     }, /* 6a: 0111 0100 0110 1010 xxxx xxxx            */
	{NEI,"C,%b"     }, /* 6b: 0111 0100 0110 1011 xxxx xxxx            */
	{NEI,"D,%b"     }, /* 6c: 0111 0100 0110 1100 xxxx xxxx            */
	{NEI,"E,%b"     }, /* 6d: 0111 0100 0110 1101 xxxx xxxx            */
	{NEI,"H,%b"     }, /* 6e: 0111 0100 0110 1110 xxxx xxxx            */
	{NEI,"L,%b"     }, /* 6f: 0111 0100 0110 1111 xxxx xxxx            */

	{SBI,"V,%b"     }, /* 70: 0111 0100 0111 0000 xxxx xxxx            */
	{SBI,"A,%b"     }, /* 71: 0111 0100 0111 0001 xxxx xxxx            */
	{SBI,"B,%b"     }, /* 72: 0111 0100 0111 0010 xxxx xxxx            */
	{SBI,"C,%b"     }, /* 73: 0111 0100 0111 0011 xxxx xxxx            */
	{SBI,"D,%b"     }, /* 74: 0111 0100 0111 0100 xxxx xxxx            */
	{SBI,"E,%b"     }, /* 75: 0111 0100 0111 0101 xxxx xxxx            */
	{SBI,"H,%b"     }, /* 76: 0111 0100 0111 0110 xxxx xxxx            */
	{SBI,"L,%b"     }, /* 77: 0111 0100 0111 0111 xxxx xxxx            */
	{EQI,"V,%b"     }, /* 78: 0111 0100 0111 1000 xxxx xxxx            */
	{EQI,"A,%b"     }, /* 79: 0111 0100 0111 1001 xxxx xxxx            */
	{EQI,"B,%b"     }, /* 7a: 0111 0100 0111 1010 xxxx xxxx            */
	{EQI,"C,%b"     }, /* 7b: 0111 0100 0111 1011 xxxx xxxx            */
	{EQI,"D,%b"     }, /* 7c: 0111 0100 0111 1100 xxxx xxxx            */
	{EQI,"E,%b"     }, /* 7d: 0111 0100 0111 1101 xxxx xxxx            */
	{EQI,"H,%b"     }, /* 7e: 0111 0100 0111 1110 xxxx xxxx            */
	{EQI,"L,%b"     }, /* 7f: 0111 0100 0111 1111 xxxx xxxx            */

	{illegal,nullptr   }, /* 80: 0111 0100 1000 0000                      */
	{illegal,nullptr   }, /* 81: 0111 0100 1000 0001                      */
	{illegal,nullptr   }, /* 82: 0111 0100 1000 0010                      */
	{illegal,nullptr   }, /* 83: 0111 0100 1000 0011                      */
	{illegal,nullptr   }, /* 84: 0111 0100 1000 0100                      */
	{illegal,nullptr   }, /* 85: 0111 0100 1000 0101                      */
	{illegal,nullptr   }, /* 86: 0111 0100 1000 0110                      */
	{illegal,nullptr   }, /* 87: 0111 0100 1000 0111                      */
	{ANAW,"%a"      }, /* 88: 0111 0100 1000 1000 oooo oooo            */
	{illegal,nullptr   }, /* 89: 0111 0100 1000 1001                      */
	{illegal,nullptr   }, /* 8a: 0111 0100 1000 1010                      */
	{illegal,nullptr   }, /* 8b: 0111 0100 1000 1011                      */
	{illegal,nullptr   }, /* 8c: 0111 0100 1000 1100                      */
	{DAN,"EA,BC"    }, /* 8d: 0111 0100 1000 1101                      */
	{DAN,"EA,DE"    }, /* 8e: 0111 0100 1000 1110                      */
	{DAN,"EA,HL"    }, /* 8f: 0111 0100 1000 1111                      */

	{XRAW,"%a"      }, /* 90: 0111 0100 1001 0000 oooo oooo            */
	{illegal,nullptr   }, /* 91: 0111 0100 1001 0001                      */
	{illegal,nullptr   }, /* 92: 0111 0100 1001 0010                      */
	{illegal,nullptr   }, /* 93: 0111 0100 1001 0011                      */
	{illegal,nullptr   }, /* 94: 0111 0100 1001 0100                      */
	{DXR,"EA,BC"    }, /* 95: 0111 0100 1001 0101                      */
	{DXR,"EA,DE"    }, /* 96: 0111 0100 1001 0110                      */
	{DXR,"EA,HL"    }, /* 97: 0111 0100 1001 0111                      */
	{ORAW,"%a"      }, /* 98: 0111 0100 1001 1000 oooo oooo            */
	{illegal,nullptr   }, /* 99: 0111 0100 1001 1001                      */
	{illegal,nullptr   }, /* 9a: 0111 0100 1001 1010                      */
	{illegal,nullptr   }, /* 9b: 0111 0100 1001 1011                      */
	{illegal,nullptr   }, /* 9c: 0111 0100 1001 1100                      */
	{DOR,"EA,BC"    }, /* 9d: 0111 0100 1001 1101                      */
	{DOR,"EA,DE"    }, /* 9e: 0111 0100 1001 1110                      */
	{DOR,"EA,HL"    }, /* 9f: 0111 0100 1001 1111                      */

	{ADDNCW,"%a"    }, /* a0: 0111 0100 1010 0000 oooo oooo            */
	{illegal,nullptr   }, /* a1: 0111 0100 1010 0001                      */
	{illegal,nullptr   }, /* a2: 0111 0100 1010 0010                      */
	{illegal,nullptr   }, /* a3: 0111 0100 1010 0011                      */
	{illegal,nullptr   }, /* a4: 0111 0100 1010 0100                      */
	{DADDNC,"EA,BC" }, /* a5: 0111 0100 1010 0101                      */
	{DADDNC,"EA,DE" }, /* a6: 0111 0100 1010 0110                      */
	{DADDNC,"EA,HL" }, /* a7: 0111 0100 1010 0111                      */
	{GTAW,"%a"      }, /* a8: 0111 0100 1010 1000 oooo oooo            */
	{illegal,nullptr   }, /* a9: 0111 0100 1010 1001                      */
	{illegal,nullptr   }, /* aa: 0111 0100 1010 1010                      */
	{illegal,nullptr   }, /* ab: 0111 0100 1010 1011                      */
	{illegal,nullptr   }, /* ac: 0111 0100 1010 1100                      */
	{DGT,"EA,BC"    }, /* ad: 0111 0100 1010 1101                      */
	{DGT,"EA,DE"    }, /* ae: 0111 0100 1010 1110                      */
	{DGT,"EA,HL"    }, /* af: 0111 0100 1010 1111                      */

	{SUBNBW,"%a"    }, /* b0: 0111 0100 1011 0000 oooo oooo            */
	{illegal,nullptr   }, /* b1: 0111 0100 1011 0001                      */
	{illegal,nullptr   }, /* b2: 0111 0100 1011 0010                      */
	{illegal,nullptr   }, /* b3: 0111 0100 1011 0011                      */
	{illegal,nullptr   }, /* b4: 0111 0100 1011 0100                      */
	{DSUBNB,"EA,BC" }, /* b5: 0111 0100 1011 0101                      */
	{DSUBNB,"EA,DE" }, /* b6: 0111 0100 1011 0110                      */
	{DSUBNB,"EA,HL" }, /* b7: 0111 0100 1011 0111                      */
	{LTAW,"%a"      }, /* b8: 0111 0100 1011 1000 oooo oooo            */
	{illegal,nullptr   }, /* b9: 0111 0100 1011 1001                      */
	{illegal,nullptr   }, /* ba: 0111 0100 1011 1010                      */
	{illegal,nullptr   }, /* bb: 0111 0100 1011 1011                      */
	{illegal,nullptr   }, /* bc: 0111 0100 1011 1100                      */
	{DLT,"EA,BC"    }, /* bd: 0111 0100 1011 1101                      */
	{DLT,"EA,DE"    }, /* be: 0111 0100 1011 1110                      */
	{DLT,"EA,HL"    }, /* bf: 0111 0100 1011 1111                      */

	{ADDW,"%a"      }, /* c0: 0111 0100 1100 0000 oooo oooo            */
	{illegal,nullptr   }, /* c1: 0111 0100 1100 0001                      */
	{illegal,nullptr   }, /* c2: 0111 0100 1100 0010                      */
	{illegal,nullptr   }, /* c3: 0111 0100 1100 0011                      */
	{illegal,nullptr   }, /* c4: 0111 0100 1100 0100                      */
	{DADD,"EA,BC"   }, /* c5: 0111 0100 1100 0101                      */
	{DADD,"EA,DE"   }, /* c6: 0111 0100 1100 0110                      */
	{DADD,"EA,HL"   }, /* c7: 0111 0100 1100 0111                      */
	{ONAW,"%a"      }, /* c8: 0111 0100 1100 1000 oooo oooo            */
	{illegal,nullptr   }, /* c9: 0111 0100 1100 1001                      */
	{illegal,nullptr   }, /* ca: 0111 0100 1100 1010                      */
	{illegal,nullptr   }, /* cb: 0111 0100 1100 1011                      */
	{illegal,nullptr   }, /* cc: 0111 0100 1100 1100                      */
	{DON,"EA,BC"    }, /* cd: 0111 0100 1100 1101                      */
	{DON,"EA,DE"    }, /* ce: 0111 0100 1100 1110                      */
	{DON,"EA,HL"    }, /* cf: 0111 0100 1100 1111                      */

	{ADCW,"%a"      }, /* d0: 0111 0100 1101 0000 oooo oooo            */
	{illegal,nullptr   }, /* d1: 0111 0100 1101 0001                      */
	{illegal,nullptr   }, /* d2: 0111 0100 1101 0010                      */
	{illegal,nullptr   }, /* d3: 0111 0100 1101 0011                      */
	{illegal,nullptr   }, /* d4: 0111 0100 1101 0100                      */
	{DADC,"EA,BC"   }, /* d5: 0111 0100 1101 0101                      */
	{DADC,"EA,DE"   }, /* d6: 0111 0100 1101 0110                      */
	{DADC,"EA,HL"   }, /* d7: 0111 0100 1101 0111                      */
	{OFFAW,"%a"     }, /* d8: 0111 0100 1101 1000 oooo oooo            */
	{illegal,nullptr   }, /* d9: 0111 0100 1101 1001                      */
	{illegal,nullptr   }, /* da: 0111 0100 1101 1010                      */
	{illegal,nullptr   }, /* db: 0111 0100 1101 1011                      */
	{illegal,nullptr   }, /* dc: 0111 0100 1101 1100                      */
	{DOFF,"EA,BC"   }, /* dd: 0111 0100 1101 1101                      */
	{DOFF,"EA,DE"   }, /* de: 0111 0100 1101 1110                      */
	{DOFF,"EA,HL"   }, /* df: 0111 0100 1101 1111                      */

	{SUBW,"%a"      }, /* e0: 0111 0100 1110 0000 oooo oooo            */
	{illegal,nullptr   }, /* e1: 0111 0100 1110 0001                      */
	{illegal,nullptr   }, /* e2: 0111 0100 1110 0010                      */
	{illegal,nullptr   }, /* e3: 0111 0100 1110 0011                      */
	{illegal,nullptr   }, /* e4: 0111 0100 1110 0100                      */
	{DSUB,"EA,BC"   }, /* e5: 0111 0100 1110 0101                      */
	{DSUB,"EA,DE"   }, /* e6: 0111 0100 1110 0110                      */
	{DSUB,"EA,HL"   }, /* e7: 0111 0100 1110 0111                      */
	{NEAW,"%a"      }, /* e8: 0111 0100 1110 1000 oooo oooo            */
	{illegal,nullptr   }, /* e9: 0111 0100 1110 1001                      */
	{illegal,nullptr   }, /* ea: 0111 0100 1110 1010                      */
	{illegal,nullptr   }, /* eb: 0111 0100 1110 1011                      */
	{illegal,nullptr   }, /* ec: 0111 0100 1110 1100                      */
	{DNE,"EA,BC"    }, /* ed: 0111 0100 1110 1101                      */
	{DNE,"EA,DE"    }, /* ee: 0111 0100 1110 1110                      */
	{DNE,"EA,HL"    }, /* ef: 0111 0100 1110 1111                      */

	{SBBW,"%a"      }, /* f0: 0111 0100 1111 0000 oooo oooo            */
	{illegal,nullptr   }, /* f1: 0111 0100 1111 0001                      */
	{illegal,nullptr   }, /* f2: 0111 0100 1111 0010                      */
	{illegal,nullptr   }, /* f3: 0111 0100 1111 0011                      */
	{illegal,nullptr   }, /* f4: 0111 0100 1111 0100                      */
	{DSBB,"EA,BC"   }, /* f5: 0111 0100 1111 0101                      */
	{DSBB,"EA,DE"   }, /* f6: 0111 0100 1111 0110                      */
	{DSBB,"EA,HL"   }, /* f7: 0111 0100 1111 0111                      */
	{EQAW,"%a"      }, /* f8: 0111 0100 1111 1000 oooo oooo            */
	{illegal,nullptr   }, /* f9: 0111 0100 1111 1001                      */
	{illegal,nullptr   }, /* fa: 0111 0100 1111 1010                      */
	{illegal,nullptr   }, /* fb: 0111 0100 1111 1011                      */
	{illegal,nullptr   }, /* fc: 0111 0100 1111 1100                      */
	{DEQ,"EA,BC"    }, /* fd: 0111 0100 1111 1101                      */
	{DEQ,"EA,DE"    }, /* fe: 0111 0100 1111 1110                      */
	{DEQ,"EA,HL"    }  /* ff: 0111 0100 1111 1111                      */
};

/* main opcodes */
static const struct dasm_s  dasmXX_7810[256] =
{
	{NOP,nullptr       }, /* 00: 0000 0000                                */
	{LDAW,"%a"      }, /* 01: 0000 0001 oooo oooo                      */
	{INX,"SP"       }, /* 02: 0000 0010                                */
	{DCX,"SP"       }, /* 03: 0000 0011                                */
	{LXI,"SP,%w"    }, /* 04: 0000 0100 llll llll hhhh hhhh            */
	{ANIW,"%a,%b"   }, /* 05: 0000 0101 oooo oooo xxxx xxxx            */
	{illegal,nullptr   }, /* 06:                                          */
	{ANI,"A,%b"     }, /* 07: 0000 0111 xxxx xxxx                      */
	{MOV,"A,EAH"    }, /* 08: 0000 1000                                */
	{MOV,"A,EAL"    }, /* 09: 0000 1001                                */
	{MOV,"A,B"      }, /* 0a: 0000 1010                                */
	{MOV,"A,C"      }, /* 0b: 0000 1011                                */
	{MOV,"A,D"      }, /* 0c: 0000 1100                                */
	{MOV,"A,E"      }, /* 0d: 0000 1101                                */
	{MOV,"A,H"      }, /* 0e: 0000 1110                                */
	{MOV,"A,L"      }, /* 0f: 0000 1111                                */

	{EXA,nullptr       }, /* 10: 0001 0000                                */  /* 7810 */
	{EXX,nullptr       }, /* 11: 0001 0001                                */  /* 7810 */
	{INX,"BC"       }, /* 12: 0001 0010                                */
	{DCX,"BC"       }, /* 13: 0001 0011                                */
	{LXI,"BC,%w"    }, /* 14: 0001 0100 llll llll hhhh hhhh            */
	{ORIW,"%a,%b"   }, /* 15: 0001 0101 oooo oooo xxxx xxxx            */
	{XRI,"A,%b"     }, /* 16: 0001 0110 xxxx xxxx                      */
	{ORI,"A,%b"     }, /* 17: 0001 0111 xxxx xxxx                      */
	{MOV,"EAH,A"    }, /* 18: 0001 1000                                */
	{MOV,"EAL,A"    }, /* 19: 0001 1001                                */
	{MOV,"B,A"      }, /* 1a: 0001 1010                                */
	{MOV,"C,A"      }, /* 1b: 0001 1011                                */
	{MOV,"D,A"      }, /* 1c: 0001 1100                                */
	{MOV,"E,A"      }, /* 1d: 0001 1101                                */
	{MOV,"H,A"      }, /* 1e: 0001 1110                                */
	{MOV,"L,A"      }, /* 1f: 0001 1111                                */

	{INRW,"%a"      }, /* 20: 0010 0000 oooo oooo                      */
	{JB,nullptr            }, /* 21: 0010 0001                                */
	{INX,"DE"       }, /* 22: 0010 0010                                */
	{DCX,"DE"       }, /* 23: 0010 0011                                */
	{LXI,"DE,%w"    }, /* 24: 0010 0100 llll llll hhhh hhhh            */
	{GTIW,"%a,%b"   }, /* 25: 0010 0101 oooo oooo xxxx xxxx            */
	{ADINC,"A,%b"   }, /* 26: 0010 0110 xxxx xxxx                      */
	{GTI,"A,%b"     }, /* 27: 0010 0111 xxxx xxxx                      */
	{illegal,nullptr   }, /* 28: 0010 1000                                */
	{LDAX,"(BC)"    }, /* 29: 0010 1001                                */
	{LDAX,"(DE)"    }, /* 2a: 0010 1010                                */
	{LDAX,"(HL)"    }, /* 2b: 0010 1011                                */
	{LDAX,"(DE+)"   }, /* 2c: 0010 1100                                */
	{LDAX,"(HL+)"   }, /* 2d: 0010 1101                                */
	{LDAX,"(DE-)"   }, /* 2e: 0010 1110                                */
	{LDAX,"(HL-)"   }, /* 2f: 0010 1111                                */

	{DCRW,"%a"      }, /* 30: 0011 0000 oooo oooo                      */
	{BLOCK,nullptr         }, /* 31: 0011 0001                                */  /* 7810 */
	{INX,"HL",       }, /* 32: 0011 0010                                */
	{DCX,"HL",       }, /* 33: 0011 0011                                */
	{LXI,"HL,%w"    }, /* 34: 0011 0100 llll llll hhhh hhhh            */
	{LTIW,"%a,%b"   }, /* 35: 0011 0101 oooo oooo xxxx xxxx            */
	{SUINB,"A,%b"   }, /* 36: 0011 0110 xxxx xxxx                      */
	{LTI,"A,%b"     }, /* 37: 0011 0111 xxxx xxxx                      */
	{illegal,nullptr   }, /* 38:                                          */
	{STAX,"(BC)"    }, /* 39: 0011 1001                                */
	{STAX,"(DE)"    }, /* 3a: 0011 1010                                */
	{STAX,"(HL)"    }, /* 3b: 0011 1011                                */
	{STAX,"(DE+)"   }, /* 3c: 0011 1100                                */
	{STAX,"(HL+)"   }, /* 3d: 0011 1101                                */
	{STAX,"(DE-)"   }, /* 3e: 0011 1110                                */
	{STAX,"(HL-)"   }, /* 3f: 0011 1111                                */

	{CALL,"%w"      }, /* 40: 0100 0000 llll llll hhhh hhhh            */
	{INR,"A"        }, /* 41: 0100 0001                                */
	{INR,"B"        }, /* 42: 0100 0010                                */
	{INR,"C"        }, /* 43: 0100 0011                                */
	{LXI,"EA,%w"    }, /* 44: 0100 0100 llll llll hhhh hhhh            */
	{ONIW,"%a,%b"   }, /* 45: 0100 0101 oooo oooo xxxx xxxx            */
	{ADI,"A,%b"     }, /* 46: 0100 0110 xxxx xxxx                      */
	{ONI,"A,%b"     }, /* 47: 0100 0111 xxxx xxxx                      */
	{0,dasm48_7810  }, /* 48: prefix                                   */
	{MVIX,"BC,%b"   }, /* 49: 0100 1001 xxxx xxxx                      */
	{MVIX,"DE,%b"   }, /* 4a: 0100 1010 xxxx xxxx                      */
	{MVIX,"HL,%b"   }, /* 4b: 0100 1011 xxxx xxxx                      */
	{0,dasm4C_7810  }, /* 4c: prefix                                   */
	{0,dasm4D_7810  }, /* 4d: prefix                                   */
	{JRE,"%d"       }, /* 4e: 0100 111d dddd dddd                      */
	{JRE,"%d"       }, /* 4f: 0100 111d dddd dddd                      */

	{EXH,nullptr       }, /* 50: 0101 0000                                */  /* 7810 */
	{DCR,"A"        }, /* 51: 0101 0001                                */
	{DCR,"B"        }, /* 52: 0101 0010                                */
	{DCR,"C"        }, /* 53: 0101 0011                                */
	{JMP,"%w"       }, /* 54: 0101 0100 llll llll hhhh hhhh            */
	{OFFIW,"%a,%b"  }, /* 55: 0101 0101 oooo oooo xxxx xxxx            */
	{ACI,"A,%b"     }, /* 56: 0101 0110 xxxx xxxx                      */
	{OFFI,"A,%b"    }, /* 57: 0101 0111 xxxx xxxx                      */
	{BIT,"0,%a",     }, /* 58: 0101 1000 oooo oooo                      */  /* 7810 */
	{BIT,"1,%a",     }, /* 59: 0101 1001 oooo oooo                      */  /* 7810 */
	{BIT,"2,%a",     }, /* 5a: 0101 1010 oooo oooo                      */  /* 7810 */
	{BIT,"3,%a",     }, /* 5b: 0101 1011 oooo oooo                      */  /* 7810 */
	{BIT,"4,%a",     }, /* 5c: 0101 1100 oooo oooo                      */  /* 7810 */
	{BIT,"5,%a",     }, /* 5d: 0101 1101 oooo oooo                      */  /* 7810 */
	{BIT,"6,%a",     }, /* 5e: 0101 1110 oooo oooo                      */  /* 7810 */
	{BIT,"7,%a",     }, /* 5f: 0101 1111 oooo oooo                      */  /* 7810 */

	{0,dasm60        }, /* 60: prefix                                   */
	{DAA,nullptr       }, /* 61: 0110 0001                                */
	{RETI,nullptr      }, /* 62: 0110 0010                                */
	{STAW,"%a"      }, /* 63: 0110 0011 oooo oooo                      */
	{0,dasm64_7810   }, /* 64: prefix                                   */
	{NEIW,"%a,%b"   }, /* 65: 0110 0101 oooo oooo xxxx xxxx            */
	{SUI,"A,%b"     }, /* 66: 0110 0110 xxxx xxxx                      */
	{NEI,"A,%b"     }, /* 67: 0110 0111 xxxx xxxx                      */
	{MVI,"V,%b"     }, /* 68: 0110 1000 xxxx xxxx                      */
	{MVI,"A,%b"     }, /* 69: 0110 1001 xxxx xxxx                      */
	{MVI,"B,%b"     }, /* 6a: 0110 1010 xxxx xxxx                      */
	{MVI,"C,%b"     }, /* 6b: 0110 1011 xxxx xxxx                      */
	{MVI,"D,%b"     }, /* 6c: 0110 1100 xxxx xxxx                      */
	{MVI,"E,%b"     }, /* 6d: 0110 1101 xxxx xxxx                      */
	{MVI,"H,%b"     }, /* 6e: 0110 1110 xxxx xxxx                      */
	{MVI,"L,%b"     }, /* 6f: 0110 1111 xxxx xxxx                      */

	{0,dasm70        }, /* 70: prefix                                   */
	{MVIW,"%a,%b"   }, /* 71: 0111 0001 oooo oooo xxxx xxxx            */
	{SOFTI,nullptr         }, /* 72: 0111 0010                                */
	{illegal,nullptr   }, /* 73:                                          */
	{0,dasm74        }, /* 74: prefix                                   */
	{EQIW,"%a,%b"   }, /* 75: 0111 0101 oooo oooo xxxx xxxx            */
	{SBI,"A,%b"     }, /* 76: 0111 0110 xxxx xxxx                      */
	{EQI,"A,%b"     }, /* 77: 0111 0111 xxxx xxxx                      */
	{CALF,"%f"      }, /* 78: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 79: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7a: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7b: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7c: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7d: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7e: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7f: 0111 1ddd dddd dddd                      */

	{CALT,"%t"      }, /* 80: 100t tttt                                */
	{CALT,"%t"      }, /* 81: 100t tttt                                */
	{CALT,"%t"      }, /* 82: 100t tttt                                */
	{CALT,"%t"      }, /* 83: 100t tttt                                */
	{CALT,"%t"      }, /* 84: 100t tttt                                */
	{CALT,"%t"      }, /* 85: 100t tttt                                */
	{CALT,"%t"      }, /* 86: 100t tttt                                */
	{CALT,"%t"      }, /* 87: 100t tttt                                */
	{CALT,"%t"      }, /* 88: 100t tttt                                */
	{CALT,"%t"      }, /* 89: 100t tttt                                */
	{CALT,"%t"      }, /* 8a: 100t tttt                                */
	{CALT,"%t"      }, /* 8b: 100t tttt                                */
	{CALT,"%t"      }, /* 8c: 100t tttt                                */
	{CALT,"%t"      }, /* 8d: 100t tttt                                */
	{CALT,"%t"      }, /* 8e: 100t tttt                                */
	{CALT,"%t"      }, /* 8f: 100t tttt                                */

	{CALT,"%t"      }, /* 90: 100t tttt                                */
	{CALT,"%t"      }, /* 91: 100t tttt                                */
	{CALT,"%t"      }, /* 92: 100t tttt                                */
	{CALT,"%t"      }, /* 93: 100t tttt                                */
	{CALT,"%t"      }, /* 94: 100t tttt                                */
	{CALT,"%t"      }, /* 95: 100t tttt                                */
	{CALT,"%t"      }, /* 96: 100t tttt                                */
	{CALT,"%t"      }, /* 97: 100t tttt                                */
	{CALT,"%t"      }, /* 98: 100t tttt                                */
	{CALT,"%t"      }, /* 99: 100t tttt                                */
	{CALT,"%t"      }, /* 9a: 100t tttt                                */
	{CALT,"%t"      }, /* 9b: 100t tttt                                */
	{CALT,"%t"      }, /* 9c: 100t tttt                                */
	{CALT,"%t"      }, /* 9d: 100t tttt                                */
	{CALT,"%t"      }, /* 9e: 100t tttt                                */
	{CALT,"%t"      }, /* 9f: 100t tttt                                */

	{POP,"VA"       }, /* a0: 1010 0000                                */
	{POP,"BC"       }, /* a1: 1010 0001                                */
	{POP,"DE"       }, /* a2: 1010 0010                                */
	{POP,"HL",       }, /* a3: 1010 0011                                */
	{POP,"EA"       }, /* a4: 1010 0100                                */
	{DMOV,"EA,BC"   }, /* a5: 1010 0101                                */
	{DMOV,"EA,DE"   }, /* a6: 1010 0110                                */
	{DMOV,"EA,HL"   }, /* a7: 1010 0111                                */
	{INX,"EA"       }, /* a8: 1010 1000                                */
	{DCX,"EA"       }, /* a9: 1010 1001                                */
	{EI,nullptr            }, /* aa: 1010 1010                                */
	{LDAX,"(DE+%b)" }, /* ab: 1010 1011 dddd dddd                      */
	{LDAX,"(HL+A)"  }, /* ac: 1010 1100                                */
	{LDAX,"(HL+B)"  }, /* ad: 1010 1101                                */
	{LDAX,"(HL+EA)" }, /* ae: 1010 1110                                */
	{LDAX,"(HL+%b)" }, /* af: 1010 1111 dddd dddd                      */

	{PUSH,"VA"      }, /* b0: 1011 0000                                */
	{PUSH,"BC"      }, /* b1: 1011 0001                                */
	{PUSH,"DE"      }, /* b2: 1011 0010                                */
	{PUSH,"HL",      }, /* b3: 1011 0011                                */
	{PUSH,"EA"      }, /* b4: 1011 0100                                */
	{DMOV,"BC,EA"   }, /* b5: 1011 0101                                */
	{DMOV,"DE,EA"   }, /* b6: 1011 0110                                */
	{DMOV,"HL,EA"   }, /* b7: 1011 0111                                */
	{RET,nullptr       }, /* b8: 1011 1000                                */
	{RETS,nullptr      }, /* b9: 1011 1001                                */
	{DI,nullptr            }, /* ba: 1011 1010                                */
	{STAX,"(DE+%b)" }, /* bb: 1011 1011 dddd dddd                      */
	{STAX,"(HL+A)"  }, /* bc: 1011 1100                                */
	{STAX,"(HL+B)"  }, /* bd: 1011 1101                                */
	{STAX,"(HL+EA)" }, /* be: 1011 1110                                */
	{STAX,"(HL+%b)" }, /* bf: 1011 1111 dddd dddd                      */

	{JR,"%o"        }, /* c0: 11oo oooo                                */
	{JR,"%o"        }, /* c1: 11oo oooo                                */
	{JR,"%o"        }, /* c2: 11oo oooo                                */
	{JR,"%o"        }, /* c3: 11oo oooo                                */
	{JR,"%o"        }, /* c4: 11oo oooo                                */
	{JR,"%o"        }, /* c5: 11oo oooo                                */
	{JR,"%o"        }, /* c6: 11oo oooo                                */
	{JR,"%o"        }, /* c7: 11oo oooo                                */
	{JR,"%o"        }, /* c8: 11oo oooo                                */
	{JR,"%o"        }, /* c9: 11oo oooo                                */
	{JR,"%o"        }, /* ca: 11oo oooo                                */
	{JR,"%o"        }, /* cb: 11oo oooo                                */
	{JR,"%o"        }, /* cc: 11oo oooo                                */
	{JR,"%o"        }, /* cd: 11oo oooo                                */
	{JR,"%o"        }, /* ce: 11oo oooo                                */
	{JR,"%o"        }, /* cf: 11oo oooo                                */

	{JR,"%o"        }, /* d0: 11oo oooo                                */
	{JR,"%o"        }, /* d1: 11oo oooo                                */
	{JR,"%o"        }, /* d2: 11oo oooo                                */
	{JR,"%o"        }, /* d3: 11oo oooo                                */
	{JR,"%o"        }, /* d4: 11oo oooo                                */
	{JR,"%o"        }, /* d5: 11oo oooo                                */
	{JR,"%o"        }, /* d6: 11oo oooo                                */
	{JR,"%o"        }, /* d7: 11oo oooo                                */
	{JR,"%o"        }, /* d8: 11oo oooo                                */
	{JR,"%o"        }, /* d9: 11oo oooo                                */
	{JR,"%o"        }, /* da: 11oo oooo                                */
	{JR,"%o"        }, /* db: 11oo oooo                                */
	{JR,"%o"        }, /* dc: 11oo oooo                                */
	{JR,"%o"        }, /* dd: 11oo oooo                                */
	{JR,"%o"        }, /* de: 11oo oooo                                */
	{JR,"%o"        }, /* df: 11oo oooo                                */

	{JR,"%o"        }, /* e0: 11oo oooo                                */
	{JR,"%o"        }, /* e1: 11oo oooo                                */
	{JR,"%o"        }, /* e2: 11oo oooo                                */
	{JR,"%o"        }, /* e3: 11oo oooo                                */
	{JR,"%o"        }, /* e4: 11oo oooo                                */
	{JR,"%o"        }, /* e5: 11oo oooo                                */
	{JR,"%o"        }, /* e6: 11oo oooo                                */
	{JR,"%o"        }, /* e7: 11oo oooo                                */
	{JR,"%o"        }, /* e8: 11oo oooo                                */
	{JR,"%o"        }, /* e9: 11oo oooo                                */
	{JR,"%o"        }, /* ea: 11oo oooo                                */
	{JR,"%o"        }, /* eb: 11oo oooo                                */
	{JR,"%o"        }, /* ec: 11oo oooo                                */
	{JR,"%o"        }, /* ed: 11oo oooo                                */
	{JR,"%o"        }, /* ee: 11oo oooo                                */
	{JR,"%o"        }, /* ef: 11oo oooo                                */

	{JR,"%o"        }, /* f0: 11oo oooo                                */
	{JR,"%o"        }, /* f1: 11oo oooo                                */
	{JR,"%o"        }, /* f2: 11oo oooo                                */
	{JR,"%o"        }, /* f3: 11oo oooo                                */
	{JR,"%o"        }, /* f4: 11oo oooo                                */
	{JR,"%o"        }, /* f5: 11oo oooo                                */
	{JR,"%o"        }, /* f6: 11oo oooo                                */
	{JR,"%o"        }, /* f7: 11oo oooo                                */
	{JR,"%o"        }, /* f8: 11oo oooo                                */
	{JR,"%o"        }, /* f9: 11oo oooo                                */
	{JR,"%o"        }, /* fa: 11oo oooo                                */
	{JR,"%o"        }, /* fb: 11oo oooo                                */
	{JR,"%o"        }, /* fc: 11oo oooo                                */
	{JR,"%o"        }, /* fd: 11oo oooo                                */
	{JR,"%o"        }, /* fe: 11oo oooo                                */
	{JR,"%o"        }  /* ff: 11oo oooo                                */
};

static const struct dasm_s  dasmXX_7807[256] =
{
	{NOP,nullptr       }, /* 00: 0000 0000                                */
	{LDAW,"%a"      }, /* 01: 0000 0001 oooo oooo                      */
	{INX,"SP"       }, /* 02: 0000 0010                                */
	{DCX,"SP"       }, /* 03: 0000 0011                                */
	{LXI,"SP,%w"    }, /* 04: 0000 0100 llll llll hhhh hhhh            */
	{ANIW,"%a,%b"   }, /* 05: 0000 0101 oooo oooo xxxx xxxx            */
	{illegal,nullptr   }, /* 06:                                          */
	{ANI,"A,%b"     }, /* 07: 0000 0111 xxxx xxxx                      */
	{MOV,"A,EAH"    }, /* 08: 0000 1000                                */
	{MOV,"A,EAL"    }, /* 09: 0000 1001                                */
	{MOV,"A,B"      }, /* 0a: 0000 1010                                */
	{MOV,"A,C"      }, /* 0b: 0000 1011                                */
	{MOV,"A,D"      }, /* 0c: 0000 1100                                */
	{MOV,"A,E"      }, /* 0d: 0000 1101                                */
	{MOV,"A,H"      }, /* 0e: 0000 1110                                */
	{MOV,"A,L"      }, /* 0f: 0000 1111                                */

	{BLOCK,"D+"  }, /* 10: 0001 0000                                */  /* 7807 */
	{BLOCK,"D-"  }, /* 11: 0001 0001                                */  /* 7807 */
	{INX,"BC"       }, /* 12: 0001 0010                                */
	{DCX,"BC"       }, /* 13: 0001 0011                                */
	{LXI,"BC,%w"    }, /* 14: 0001 0100 llll llll hhhh hhhh            */
	{ORIW,"%a,%b"   }, /* 15: 0001 0101 oooo oooo xxxx xxxx            */
	{XRI,"A,%b"     }, /* 16: 0001 0110 xxxx xxxx                      */
	{ORI,"A,%b"     }, /* 17: 0001 0111 xxxx xxxx                      */
	{MOV,"EAH,A"    }, /* 18: 0001 1000                                */
	{MOV,"EAL,A"    }, /* 19: 0001 1001                                */
	{MOV,"B,A"      }, /* 1a: 0001 1010                                */
	{MOV,"C,A"      }, /* 1b: 0001 1011                                */
	{MOV,"D,A"      }, /* 1c: 0001 1100                                */
	{MOV,"E,A"      }, /* 1d: 0001 1101                                */
	{MOV,"H,A"      }, /* 1e: 0001 1110                                */
	{MOV,"L,A"      }, /* 1f: 0001 1111                                */

	{INRW,"%a"      }, /* 20: 0010 0000 oooo oooo                      */
	{JB,nullptr            }, /* 21: 0010 0001                                */
	{INX,"DE"       }, /* 22: 0010 0010                                */
	{DCX,"DE"       }, /* 23: 0010 0011                                */
	{LXI,"DE,%w"    }, /* 24: 0010 0100 llll llll hhhh hhhh            */
	{GTIW,"%a,%b"   }, /* 25: 0010 0101 oooo oooo xxxx xxxx            */
	{ADINC,"A,%b"   }, /* 26: 0010 0110 xxxx xxxx                      */
	{GTI,"A,%b"     }, /* 27: 0010 0111 xxxx xxxx                      */
	{illegal,nullptr   }, /* 28: 0010 1000                                */
	{LDAX,"(BC)"    }, /* 29: 0010 1001                                */
	{LDAX,"(DE)"    }, /* 2a: 0010 1010                                */
	{LDAX,"(HL)"    }, /* 2b: 0010 1011                                */
	{LDAX,"(DE+)"   }, /* 2c: 0010 1100                                */
	{LDAX,"(HL+)"   }, /* 2d: 0010 1101                                */
	{LDAX,"(DE-)"   }, /* 2e: 0010 1110                                */
	{LDAX,"(HL-)"   }, /* 2f: 0010 1111                                */

	{DCRW,"%a"      }, /* 30: 0011 0000 oooo oooo                      */
	{AND,"CY,%i"     }, /* 31: 0011 0001 bbbb bbbb                      */  /* 7807 */
	{INX,"HL",       }, /* 32: 0011 0010                                */
	{DCX,"HL",       }, /* 33: 0011 0011                                */
	{LXI,"HL,%w"    }, /* 34: 0011 0100 llll llll hhhh hhhh            */
	{LTIW,"%a,%b"   }, /* 35: 0011 0101 oooo oooo xxxx xxxx            */
	{SUINB,"A,%b"   }, /* 36: 0011 0110 xxxx xxxx                      */
	{LTI,"A,%b"     }, /* 37: 0011 0111 xxxx xxxx                      */
	{illegal,nullptr   }, /* 38:                                          */
	{STAX,"(BC)"    }, /* 39: 0011 1001                                */
	{STAX,"(DE)"    }, /* 3a: 0011 1010                                */
	{STAX,"(HL)"    }, /* 3b: 0011 1011                                */
	{STAX,"(DE+)"   }, /* 3c: 0011 1100                                */
	{STAX,"(HL+)"   }, /* 3d: 0011 1101                                */
	{STAX,"(DE-)"   }, /* 3e: 0011 1110                                */
	{STAX,"(HL-)"   }, /* 3f: 0011 1111                                */

	{CALL,"%w"      }, /* 40: 0100 0000 llll llll hhhh hhhh            */
	{INR,"A"        }, /* 41: 0100 0001                                */
	{INR,"B"        }, /* 42: 0100 0010                                */
	{INR,"C"        }, /* 43: 0100 0011                                */
	{LXI,"EA,%w"    }, /* 44: 0100 0100 llll llll hhhh hhhh            */
	{ONIW,"%a,%b"   }, /* 45: 0100 0101 oooo oooo xxxx xxxx            */
	{ADI,"A,%b"     }, /* 46: 0100 0110 xxxx xxxx                      */
	{ONI,"A,%b"     }, /* 47: 0100 0111 xxxx xxxx                      */
	{0,dasm48_7807   }, /* 48: prefix                                   */
	{MVIX,"BC,%b"   }, /* 49: 0100 1001 xxxx xxxx                      */
	{MVIX,"DE,%b"   }, /* 4a: 0100 1010 xxxx xxxx                      */
	{MVIX,"HL,%b"   }, /* 4b: 0100 1011 xxxx xxxx                      */
	{0,dasm4C_7807   }, /* 4c: prefix                                   */
	{0,dasm4D_7807   }, /* 4d: prefix                                   */
	{JRE,"%d"       }, /* 4e: 0100 111d dddd dddd                      */
	{JRE,"%d"       }, /* 4f: 0100 111d dddd dddd                      */
	{SKN,"%i"        }, /* 50: 0101 0000 bbbb bbbb                      */  /* 7807 */
	{DCR,"A"        }, /* 51: 0101 0001                                */
	{DCR,"B"        }, /* 52: 0101 0010                                */
	{DCR,"C"        }, /* 53: 0101 0011                                */
	{JMP,"%w"       }, /* 54: 0101 0100 llll llll hhhh hhhh            */
	{OFFIW,"%a,%b"  }, /* 55: 0101 0101 oooo oooo xxxx xxxx            */
	{ACI,"A,%b"     }, /* 56: 0101 0110 xxxx xxxx                      */
	{OFFI,"A,%b"    }, /* 57: 0101 0111 xxxx xxxx                      */
	{SETB,"%i"      }, /* 58: 0101 1000 bbbb bbbb                      */   /* 7807 */
	{NOT,"%i"       }, /* 59: 0101 1001 bbbb bbbb                      */   /* 7807 */
	{MOV,"%i,CY"    }, /* 5a: 0101 1010 bbbb bbbb                      */   /* 7807 */
	{CLR,"%i"       }, /* 5b: 0101 1011 bbbb bbbb                      */   /* 7807 */
	{OR,"CY,%i"     }, /* 5c: 0101 1100 bbbb bbbb                      */   /* 7807 */
	{SK,"%i"        }, /* 5d: 0101 1101 bbbb bbbb                      */   /* 7807 */
	{XOR,"CY,%i"    }, /* 5e: 0101 1110 bbbb bbbb                      */   /* 7807 */
	{MOV,"CY,%i"    }, /* 5f: 0101 1111 bbbb bbbb                      */   /* 7807 */

	{0,dasm60        }, /* 60: prefix                                   */
	{DAA,nullptr       }, /* 61: 0110 0001                                */
	{RETI,nullptr      }, /* 62: 0110 0010                                */
	{STAW,"%a"      }, /* 63: 0110 0011 oooo oooo                      */
	{0,dasm64_7807   }, /* 64: prefix                                   */
	{NEIW,"%a,%b"   }, /* 65: 0110 0101 oooo oooo xxxx xxxx            */
	{SUI,"A,%b"     }, /* 66: 0110 0110 xxxx xxxx                      */
	{NEI,"A,%b"     }, /* 67: 0110 0111 xxxx xxxx                      */
	{MVI,"V,%b"     }, /* 68: 0110 1000 xxxx xxxx                      */
	{MVI,"A,%b"     }, /* 69: 0110 1001 xxxx xxxx                      */
	{MVI,"B,%b"     }, /* 6a: 0110 1010 xxxx xxxx                      */
	{MVI,"C,%b"     }, /* 6b: 0110 1011 xxxx xxxx                      */
	{MVI,"D,%b"     }, /* 6c: 0110 1100 xxxx xxxx                      */
	{MVI,"E,%b"     }, /* 6d: 0110 1101 xxxx xxxx                      */
	{MVI,"H,%b"     }, /* 6e: 0110 1110 xxxx xxxx                      */
	{MVI,"L,%b"     }, /* 6f: 0110 1111 xxxx xxxx                      */

	{0,dasm70        }, /* 70: prefix                                   */
	{MVIW,"%a,%b"   }, /* 71: 0111 0001 oooo oooo xxxx xxxx            */
	{SOFTI,nullptr         }, /* 72: 0111 0010                                */
	{illegal,nullptr   }, /* 73:                                          */
	{0,dasm74        }, /* 74: prefix                                   */
	{EQIW,"%a,%b"   }, /* 75: 0111 0101 oooo oooo xxxx xxxx            */
	{SBI,"A,%b"     }, /* 76: 0111 0110 xxxx xxxx                      */
	{EQI,"A,%b"     }, /* 77: 0111 0111 xxxx xxxx                      */
	{CALF,"%f"      }, /* 78: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 79: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7a: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7b: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7c: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7d: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7e: 0111 1ddd dddd dddd                      */
	{CALF,"%f"      }, /* 7f: 0111 1ddd dddd dddd                      */

	{CALT,"%t"      }, /* 80: 100t tttt                                */
	{CALT,"%t"      }, /* 81: 100t tttt                                */
	{CALT,"%t"      }, /* 82: 100t tttt                                */
	{CALT,"%t"      }, /* 83: 100t tttt                                */
	{CALT,"%t"      }, /* 84: 100t tttt                                */
	{CALT,"%t"      }, /* 85: 100t tttt                                */
	{CALT,"%t"      }, /* 86: 100t tttt                                */
	{CALT,"%t"      }, /* 87: 100t tttt                                */
	{CALT,"%t"      }, /* 88: 100t tttt                                */
	{CALT,"%t"      }, /* 89: 100t tttt                                */
	{CALT,"%t"      }, /* 8a: 100t tttt                                */
	{CALT,"%t"      }, /* 8b: 100t tttt                                */
	{CALT,"%t"      }, /* 8c: 100t tttt                                */
	{CALT,"%t"      }, /* 8d: 100t tttt                                */
	{CALT,"%t"      }, /* 8e: 100t tttt                                */
	{CALT,"%t"      }, /* 8f: 100t tttt                                */

	{CALT,"%t"      }, /* 90: 100t tttt                                */
	{CALT,"%t"      }, /* 91: 100t tttt                                */
	{CALT,"%t"      }, /* 92: 100t tttt                                */
	{CALT,"%t"      }, /* 93: 100t tttt                                */
	{CALT,"%t"      }, /* 94: 100t tttt                                */
	{CALT,"%t"      }, /* 95: 100t tttt                                */
	{CALT,"%t"      }, /* 96: 100t tttt                                */
	{CALT,"%t"      }, /* 97: 100t tttt                                */
	{CALT,"%t"      }, /* 98: 100t tttt                                */
	{CALT,"%t"      }, /* 99: 100t tttt                                */
	{CALT,"%t"      }, /* 9a: 100t tttt                                */
	{CALT,"%t"      }, /* 9b: 100t tttt                                */
	{CALT,"%t"      }, /* 9c: 100t tttt                                */
	{CALT,"%t"      }, /* 9d: 100t tttt                                */
	{CALT,"%t"      }, /* 9e: 100t tttt                                */
	{CALT,"%t"      }, /* 9f: 100t tttt                                */

	{POP,"VA"       }, /* a0: 1010 0000                                */
	{POP,"BC"       }, /* a1: 1010 0001                                */
	{POP,"DE"       }, /* a2: 1010 0010                                */
	{POP,"HL",       }, /* a3: 1010 0011                                */
	{POP,"EA"       }, /* a4: 1010 0100                                */
	{DMOV,"EA,BC"   }, /* a5: 1010 0101                                */
	{DMOV,"EA,DE"   }, /* a6: 1010 0110                                */
	{DMOV,"EA,HL"   }, /* a7: 1010 0111                                */
	{INX,"EA"       }, /* a8: 1010 1000                                */
	{DCX,"EA"       }, /* a9: 1010 1001                                */
	{EI,nullptr            }, /* aa: 1010 1010                                */
	{LDAX,"(DE+%b)" }, /* ab: 1010 1011 dddd dddd                      */
	{LDAX,"(HL+A)"  }, /* ac: 1010 1100                                */
	{LDAX,"(HL+B)"  }, /* ad: 1010 1101                                */
	{LDAX,"(HL+EA)" }, /* ae: 1010 1110                                */
	{LDAX,"(HL+%b)" }, /* af: 1010 1111 dddd dddd                      */

	{PUSH,"VA"      }, /* b0: 1011 0000                                */
	{PUSH,"BC"      }, /* b1: 1011 0001                                */
	{PUSH,"DE"      }, /* b2: 1011 0010                                */
	{PUSH,"HL",      }, /* b3: 1011 0011                                */
	{PUSH,"EA"      }, /* b4: 1011 0100                                */
	{DMOV,"BC,EA"   }, /* b5: 1011 0101                                */
	{DMOV,"DE,EA"   }, /* b6: 1011 0110                                */
	{DMOV,"HL,EA"   }, /* b7: 1011 0111                                */
	{RET,nullptr       }, /* b8: 1011 1000                                */
	{RETS,nullptr      }, /* b9: 1011 1001                                */
	{DI,nullptr            }, /* ba: 1011 1010                                */
	{STAX,"(DE+%b)" }, /* bb: 1011 1011 dddd dddd                      */
	{STAX,"(HL+A)"  }, /* bc: 1011 1100                                */
	{STAX,"(HL+B)"  }, /* bd: 1011 1101                                */
	{STAX,"(HL+EA)" }, /* be: 1011 1110                                */
	{STAX,"(HL+%b)" }, /* bf: 1011 1111 dddd dddd                      */

	{JR,"%o"        }, /* c0: 11oo oooo                                */
	{JR,"%o"        }, /* c1: 11oo oooo                                */
	{JR,"%o"        }, /* c2: 11oo oooo                                */
	{JR,"%o"        }, /* c3: 11oo oooo                                */
	{JR,"%o"        }, /* c4: 11oo oooo                                */
	{JR,"%o"        }, /* c5: 11oo oooo                                */
	{JR,"%o"        }, /* c6: 11oo oooo                                */
	{JR,"%o"        }, /* c7: 11oo oooo                                */
	{JR,"%o"        }, /* c8: 11oo oooo                                */
	{JR,"%o"        }, /* c9: 11oo oooo                                */
	{JR,"%o"        }, /* ca: 11oo oooo                                */
	{JR,"%o"        }, /* cb: 11oo oooo                                */
	{JR,"%o"        }, /* cc: 11oo oooo                                */
	{JR,"%o"        }, /* cd: 11oo oooo                                */
	{JR,"%o"        }, /* ce: 11oo oooo                                */
	{JR,"%o"        }, /* cf: 11oo oooo                                */

	{JR,"%o"        }, /* d0: 11oo oooo                                */
	{JR,"%o"        }, /* d1: 11oo oooo                                */
	{JR,"%o"        }, /* d2: 11oo oooo                                */
	{JR,"%o"        }, /* d3: 11oo oooo                                */
	{JR,"%o"        }, /* d4: 11oo oooo                                */
	{JR,"%o"        }, /* d5: 11oo oooo                                */
	{JR,"%o"        }, /* d6: 11oo oooo                                */
	{JR,"%o"        }, /* d7: 11oo oooo                                */
	{JR,"%o"        }, /* d8: 11oo oooo                                */
	{JR,"%o"        }, /* d9: 11oo oooo                                */
	{JR,"%o"        }, /* da: 11oo oooo                                */
	{JR,"%o"        }, /* db: 11oo oooo                                */
	{JR,"%o"        }, /* dc: 11oo oooo                                */
	{JR,"%o"        }, /* dd: 11oo oooo                                */
	{JR,"%o"        }, /* de: 11oo oooo                                */
	{JR,"%o"        }, /* df: 11oo oooo                                */

	{JR,"%o"        }, /* e0: 11oo oooo                                */
	{JR,"%o"        }, /* e1: 11oo oooo                                */
	{JR,"%o"        }, /* e2: 11oo oooo                                */
	{JR,"%o"        }, /* e3: 11oo oooo                                */
	{JR,"%o"        }, /* e4: 11oo oooo                                */
	{JR,"%o"        }, /* e5: 11oo oooo                                */
	{JR,"%o"        }, /* e6: 11oo oooo                                */
	{JR,"%o"        }, /* e7: 11oo oooo                                */
	{JR,"%o"        }, /* e8: 11oo oooo                                */
	{JR,"%o"        }, /* e9: 11oo oooo                                */
	{JR,"%o"        }, /* ea: 11oo oooo                                */
	{JR,"%o"        }, /* eb: 11oo oooo                                */
	{JR,"%o"        }, /* ec: 11oo oooo                                */
	{JR,"%o"        }, /* ed: 11oo oooo                                */
	{JR,"%o"        }, /* ee: 11oo oooo                                */
	{JR,"%o"        }, /* ef: 11oo oooo                                */

	{JR,"%o"        }, /* f0: 11oo oooo                                */
	{JR,"%o"        }, /* f1: 11oo oooo                                */
	{JR,"%o"        }, /* f2: 11oo oooo                                */
	{JR,"%o"        }, /* f3: 11oo oooo                                */
	{JR,"%o"        }, /* f4: 11oo oooo                                */
	{JR,"%o"        }, /* f5: 11oo oooo                                */
	{JR,"%o"        }, /* f6: 11oo oooo                                */
	{JR,"%o"        }, /* f7: 11oo oooo                                */
	{JR,"%o"        }, /* f8: 11oo oooo                                */
	{JR,"%o"        }, /* f9: 11oo oooo                                */
	{JR,"%o"        }, /* fa: 11oo oooo                                */
	{JR,"%o"        }, /* fb: 11oo oooo                                */
	{JR,"%o"        }, /* fc: 11oo oooo                                */
	{JR,"%o"        }, /* fd: 11oo oooo                                */
	{JR,"%o"        }, /* fe: 11oo oooo                                */
	{JR,"%o"        }  /* ff: 11oo oooo                                */
};

/*********************************************************
 *
 * uPD7801
 *
 *********************************************************/

static const struct dasm_s dasm48_7801[256] = {
	/* 0x00 - 0x3F */
	{ SKIT, "F0" },     { SKIT, "FT" },     { SKIT, "F1" },     { SKIT, "F2" },
	{ SKIT, "FS" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { SK, "CY" },       { illegal, nullptr },
	{ SK, "Z" },        { illegal, nullptr },     { PUSH, "VA" },     { POP, "VA" },

	{ SKNIT, "F0" },    { SKNIT, "FT" },    { SKNIT, "F1" },    { SKNIT, "F2" },
	{ SKNIT, "FS" },    { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { SKN, "CY" },      { illegal, nullptr },
	{ SKN, "Z" },       { illegal, nullptr },     { PUSH, "BC" },     { POP, "BC" },

	{ EI, nullptr },          { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ DI, nullptr },          { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { CLC, nullptr },         { STC, nullptr },
	{ PER, nullptr },         { PEX, nullptr },         { PUSH, "DE" },     { POP, "DE" },

	{ RLL, "A" },       { RLR, "A" },       { RLL, "C" },       { RLR, "C" },
	{ SLL, "A" },       { SLR, "A" },       { SLL, "C" },       { SLR, "C" },
	{ RLD, nullptr },         { RRD, nullptr },         { illegal, nullptr },     { illegal, nullptr },
	{ PER, nullptr },         { illegal, nullptr },     { PUSH, "HL" },     { POP, "HL" },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0xC0 - 0xFF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr }
};

static const struct dasm_s dasm4c_7801[256] = {
	/* 0x00 - 0x3F */
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	/* 0x40 - 0x7F */
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	/* 0x80 - 0xBF */
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },
	{ IN, nullptr },          { IN, nullptr },          { IN, nullptr },          { IN, nullptr },

	/* 0xC0 - 0xFF */
	{ MOV, "A,PA" },    { MOV, "A,PB" },    { MOV, "A,PC" },    { MOV, "A,MK" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ MOV, "A,S" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr }
};

static const struct dasm_s dasm4d_7801[256] = {
	/* 0x00 - 0x3F */
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	/* 0x40 - 0x7F */
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	/* 0x80 - 0xBF */
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },
	{ OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr }, { OUT, nullptr },

	/* 0xC0 - 0xFF */
	{ MOV, "PA,A" },    { MOV, "PB,A" },    { MOV, "PC,A" },    { MOV, "MK,A" },
	{ MOV, "MB,A" },    { MOV, "MC,A" },    { MOV, "TM0,A" },   { MOV, "TM1,A" },
	{ MOV, "S,A" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }
};

static const struct dasm_s dasm60_7801[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr },     { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ANA, "V,A" },     { ANA, "A,A" }, { ANA, "B,A" }, { ANA, "C,A" },
	{ ANA, "D,A" },     { ANA, "E,A" }, { ANA, "H,A" }, { ANA, "L,A" },

	{ XRA, "V,A" },     { XRA, "A,A" }, { XRA, "B,A" }, { XRA, "C,A" },
	{ XRA, "D,A" },     { XRA, "E,A" }, { XRA, "H,A" }, { XRA, "L,A" },
	{ ORA, "V,A" },     { ORA, "A,A" }, { ORA, "B,A" }, { ORA, "C,A" },
	{ ORA, "D,A" },     { ORA, "E,A" }, { ORA, "H,A" }, { ORA, "L,A" },

	{ ADDNC, "V,A" },   { ADDNC, "A,A" }, { ADDNC, "B,A" }, { ADDNC, "C,A" },
	{ ADDNC, "D,A" },   { ADDNC, "E,A" }, { ADDNC, "H,A" }, { ADDNC, "L,A" },
	{ GTA, "V,A" },     { GTA, "A,A" }, { GTA, "B,A" }, { GTA, "C,A" },
	{ GTA, "D,A" },     { GTA, "E,A" }, { GTA, "H,A" }, { GTA, "L,A" },

	{ SUBNB, "V,A" },   { SUBNB, "A,A" }, { SUBNB, "B,A" }, { SUBNB, "C,A" },
	{ SUBNB, "D,A" },   { SUBNB, "E,A" }, { SUBNB, "H,A" }, { SUBNB, "L,A" },
	{ LTA, "V,A" },     { LTA, "A,A" }, { LTA, "B,A" }, { LTA, "C,A" },
	{ LTA, "D,A" },     { LTA, "E,A" }, { LTA, "H,A" }, { LTA, "L,A" },

	/* 0x40 - 0x7F */
	{ ADD, "V,A" },     { ADD, "A,A" }, { ADD, "B,A" }, { ADD, "C,A" },
	{ ADD, "D,A" },     { ADD, "E,A" }, { ADD, "H,A" }, { ADD, "L,A" },
	{ illegal, nullptr },     { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ ADC, "V,A" },     { ADC, "A,A" }, { ADC, "B,A" }, { ADC, "C,A" },
	{ ADC, "D,A" },     { ADC, "E,A" }, { ADC, "H,A" }, { ADC, "L,A" },
	{ illegal, nullptr },     { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ SUB, "V,A" },     { SUB, "A,A" }, { SUB, "B,A" }, { SUB, "C,A" },
	{ SUB, "D,A" },     { SUB, "E,A" }, { SUB, "H,A" }, { SUB, "L,A" },
	{ NEA, "V,A" },     { NEA, "A,A" }, { NEA, "B,A" }, { NEA, "C,A" },
	{ NEA, "D,A" },     { NEA, "E,A" }, { NEA, "H,A" }, { NEA, "L,A" },

	{ SBB, "V,A" },     { SBB, "A,A" }, { SBB, "B,A" }, { SBB, "C,A" },
	{ SBB, "D,A" },     { SBB, "E,A" }, { SBB, "H,A" }, { SBB, "L,A" },
	{ EQA, "V,A" },     { EQA, "A,A" }, { EQA, "B,A" }, { EQA, "C,A" },
	{ EQA, "D,A" },     { EQA, "E,A" }, { EQA, "H,A" }, { EQA, "L,A" },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ANA, "A,V" },     { ANA, "A,A" }, { ANA, "A,B" }, { ANA, "A,C" },
	{ ANA, "A,D" },     { ANA, "A,E" }, { ANA, "A,H" }, { ANA, "A,L" },

	{ XRA, "A,V" },     { XRA, "A,A" }, { XRA, "A,B" }, { XRA, "A,C" },
	{ XRA, "A,D" },     { XRA, "A,E" }, { XRA, "A,H" }, { XRA, "A,L" },
	{ ORA, "A,V" },     { ORA, "A,A" }, { ORA, "A,B" }, { ORA, "A,C" },
	{ ORA, "A,D" },     { ORA, "A,E" }, { ORA, "A,H" }, { ORA, "A,L" },

	{ ADDNC, "A,V" },   { ADDNC, "A,A" }, { ADDNC, "A,B" }, { ADDNC, "A,C" },
	{ ADDNC, "A,D" },   { ADDNC, "A,E" }, { ADDNC, "A,H" }, { ADDNC, "A,L" },
	{ GTA, "A,V" },     { GTA, "A,A" }, { GTA, "A,B" }, { GTA, "A,C" },
	{ GTA, "A,D" },     { GTA, "A,E" }, { GTA, "A,H" }, { GTA, "A,L" },

	{ SUBNB, "A,V" },   { SUBNB, "A,A" }, { SUBNB, "A,B" }, { SUBNB, "A,C" },
	{ SUBNB, "A,D" },   { SUBNB, "A,E" }, { SUBNB, "A,H" }, { SUBNB, "A,L" },
	{ LTA, "A,V" },     { LTA, "A,A" }, { LTA, "A,B" }, { LTA, "A,C" },
	{ LTA, "A,D" },     { LTA, "A,E" }, { LTA, "A,H" }, { LTA, "A,L" },

	/* 0xC0 - 0xFF */
	{ ADD, "A,V" },     { ADD, "A,A" }, { ADD, "A,B" }, { ADD, "A,C" },
	{ ADD, "A,D" },     { ADD, "A,E" }, { ADD, "A,H" }, { ADD, "A,L" },
	{ ONA, "A,V" },     { ONA, "A,A" }, { ONA, "A,B" }, { ONA, "A,C" },
	{ ONA, "A,D" },     { ONA, "A,E" }, { ONA, "A,H" }, { ONA, "A,L" },

	{ ADC, "A,V" },     { ADC, "A,A" }, { ADC, "A,B" }, { ADC, "A,C" },
	{ ADC, "A,D" },     { ADC, "A,E" }, { ADC, "A,H" }, { ADC, "A,L" },
	{ OFFA, "A,V" },    { OFFA, "A,A" }, { OFFA, "A,B" }, { OFFA, "A,C" },
	{ OFFA, "A,D" },    { OFFA, "A,E" }, { OFFA, "A,H" }, { OFFA, "A,L" },

	{ SUB, "A,V" },     { SUB, "A,A" }, { SUB, "A,B" }, { SUB, "A,C" },
	{ SUB, "A,D" },     { SUB, "A,E" }, { SUB, "A,H" }, { SUB, "A,L" },
	{ NEA, "A,V" },     { NEA, "A,A" }, { NEA, "A,B" }, { NEA, "A,C" },
	{ NEA, "A,D" },     { NEA, "A,E" }, { NEA, "A,H" }, { NEA, "A,L" },

	{ SBB, "A,V" },     { SBB, "A,A" }, { SBB, "A,B" }, { SBB, "A,C" },
	{ SBB, "A,D" },     { SBB, "A,E" }, { SBB, "A,H" }, { SBB, "A,L" },
	{ EQA, "A,V" },     { EQA, "A,A" }, { EQA, "A,B" }, { EQA, "A,C" },
	{ EQA, "A,D" },     { EQA, "A,E" }, { EQA, "A,H" }, { EQA, "A,L" }
};

static const struct dasm_s dasm64_7801[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ANI, "V,%b" }, { ANI, "A,%b" }, { ANI, "B,%b" }, { ANI, "C,%b" },
	{ ANI, "D,%b" }, { ANI, "E,%b" }, { ANI, "H,%b" }, { ANI, "L,%b" },

	{ XRI, "V,%b" }, { XRI, "A,%b" }, { XRI, "B,%b" }, { XRI, "C,%b" },
	{ XRI, "D,%b" }, { XRI, "E,%b" }, { XRI, "H,%b" }, { XRI, "L,%b" },
	{ ORI, "V,%b" }, { ORI, "A,%b" }, { ORI, "B,%b" }, { ORI, "C,%b" },
	{ ORI, "D,%b" }, { ORI, "E,%b" }, { ORI, "H,%b" }, { ORI, "L,%b" },

	{ ADINC, "V,%b" }, { ADINC, "A,%b" }, { ADINC, "B,%b" }, { ADINC, "C,%b" },
	{ ADINC, "D,%b" }, { ADINC, "E,%b" }, { ADINC, "H,%b" }, { ADINC, "L,%b" },
	{ GTI, "V,%b" }, { GTI, "A,%b" }, { GTI, "B,%b" }, { GTI, "C,%b" },
	{ GTI, "D,%b" }, { GTI, "E,%b" }, { GTI, "H,%b" }, { GTI, "L,%b" },

	{ SUINB, "V,%b" }, { SUINB, "A,%b" }, { SUINB, "B,%b" }, { SUINB, "C,%b" },
	{ SUINB, "D,%b" }, { SUINB, "E,%b" }, { SUINB, "H,%b" }, { SUINB, "L,%b" },
	{ LTI, "V,%b" }, { LTI, "A,%b" }, { LTI, "B,%b" }, { LTI, "C,%b" },
	{ LTI, "D,%b" }, { LTI, "E,%b" }, { LTI, "H,%b" }, { LTI, "L,%b" },

	/* 0x40 - 0x7F */
	{ ADI, "V,%b" }, { ADI, "A,%b" }, { ADI, "B,%b" }, { ADI, "C,%b" },
	{ ADI, "D,%b" }, { ADI, "E,%b" }, { ADI, "H,%b" }, { ADI, "L,%b" },
	{ ONI, "V,%b" }, { ONI, "A,%b" }, { ONI, "B,%b" }, { ONI, "C,%b" },
	{ ONI, "D,%b" }, { ONI, "E,%b" }, { ONI, "H,%b" }, { ONI, "L,%b" },

	{ ACI, "V,%b" }, { ACI, "A,%b" }, { ACI, "B,%b" }, { ACI, "C,%b" },
	{ ACI, "D,%b" }, { ACI, "E,%b" }, { ACI, "H,%b" }, { ACI, "L,%b" },
	{ OFFI, "V,%b" }, { OFFI, "A,%b" }, { OFFI, "B,%b" }, { OFFI, "C,%b" },
	{ OFFI, "D,%b" }, { OFFI, "E,%b" }, { OFFI, "H,%b" }, { OFFI, "L,%b" },

	{ SUI, "V,%b" }, { SUI, "A,%b" }, { SUI, "B,%b" }, { SUI, "C,%b" },
	{ SUI, "D,%b" }, { SUI, "E,%b" }, { SUI, "H,%b" }, { SUI, "L,%b" },
	{ NEI, "V,%b" }, { NEI, "A,%b" }, { NEI, "B,%b" }, { NEI, "C,%b" },
	{ NEI, "D,%b" }, { NEI, "E,%b" }, { NEI, "H,%b" }, { NEI, "L,%b" },

	{ SBI, "V,%b" }, { SBI, "A,%b" }, { SBI, "B,%b" }, { SBI, "C,%b" },
	{ SBI, "D,%b" }, { SBI, "E,%b" }, { SBI, "H,%b" }, { SBI, "L,%b" },
	{ EQI, "V,%b" }, { EQI, "A,%b" }, { EQI, "B,%b" }, { EQI, "C,%b" },
	{ EQI, "D,%b" }, { EQI, "E,%b" }, { EQI, "H,%b" }, { EQI, "L,%b" },

	/* 0x80 - 0xBF */
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ANI, "PA,%b" }, { ANI, "PB,%b" }, { ANI, "PC,%b" }, { ANI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ XRI, "PA,%b" }, { XRI, "PB,%b" }, { XRI, "PC,%b" }, { XRI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ORI, "PA,%b" }, { ORI, "PB,%b" }, { ORI, "PC,%b" }, { ORI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ ADINC, "PA,%b" }, { ADINC, "PB,%b" }, { ADINC, "PC,%b" }, { ADINC, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ GTI, "PA,%b" }, { GTI, "PB,%b" }, { GTI, "PC,%b" }, { GTI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ SUINB, "PA,%b" }, { SUINB, "PB,%b" }, { SUINB, "PC,%b" }, { SUINB, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ LTI, "PA,%b" }, { LTI, "PB,%b" }, { LTI, "PC,%b" }, { LTI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	/* 0xC0 - 0xFF */
	{ ADI, "PA,%b" }, { ADI, "PB,%b" }, { ADI, "PC,%b" }, { ADI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ONI, "PA,%b" }, { ONI, "PB,%b" }, { ONI, "PC,%b" }, { ONI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ ACI, "PA,%b" }, { ACI, "PB,%b" }, { ACI, "PC,%b" }, { ACI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ OFFI, "PA,%b" }, { OFFI, "PB,%b" }, { OFFI, "PC,%b" }, { OFFI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ SUI, "PA,%b" }, { SUI, "PB,%b" }, { SUI, "PC,%b" }, { SUI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ NEI, "PA,%b" }, { NEI, "PB,%b" }, { NEI, "PC,%b" }, { NEI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ SBI, "PA,%b" }, { SBI, "PB,%b" }, { SBI, "PC,%b" }, { SBI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ EQI, "PA,%b" }, { EQI, "PB,%b" }, { EQI, "PC,%b" }, { EQI, "MK,%b" },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }
};

static const struct dasm_s dasm70_7801[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { SSPD, "%w" }, { LSPD, "%w" },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { SBCD, "%w" }, { LBCD, "%w" },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { SDED, "%w" }, { LDED, "%w" },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { SHLD, "%w" }, { LHLD, "%w" },

	/* 0x40 - 0x7F */
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ MOV, "V,%w" }, { MOV, "A,%w" }, { MOV, "B,%w" }, { MOV, "C,%w" },
	{ MOV, "D,%w" }, { MOV, "E,%w" }, { MOV, "H,%w" }, { MOV, "L,%w" },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ MOV, "%w,V" }, { MOV, "%w,A" }, { MOV, "%w,B" }, { MOV, "%w,C" },
	{ MOV, "%w,D" }, { MOV, "%w,E" }, { MOV, "%w,H" }, { MOV, "%w,L" },

	/* 0x80 - 0xBF */
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { ANAX, "BC" }, { ANAX, "DE" }, { ANAX, "HL" },
	{ ANAX, "DE+" }, { ANAX, "HL+" }, { ANAX, "DE-" }, { ANAX, "HL-" },

	{ illegal, nullptr }, { XRAX, "BC" }, { XRAX, "DE" }, { XRAX, "HL" },
	{ XRAX, "DE+" }, { XRAX, "HL+" }, { XRAX, "DE-" }, { XRAX, "HL-" },
	{ illegal, nullptr }, { ORAX, "BC" }, { ORAX, "DE" }, { ORAX, "HL" },
	{ ORAX, "DE+" }, { ORAX, "HL+" }, { ORAX, "DE-" }, { ORAX, "HL-" },

	{ illegal, nullptr }, { ADDNCX, "BC" }, { ADDNCX, "DE" }, { ADDNCX, "HL" },
	{ ADDNCX, "DE+" }, { ADDNCX, "HL+" }, { ADDNCX, "DE-" }, { ADDNCX, "HL-" },
	{ illegal, nullptr }, { GTAX, "BC" }, { GTAX, "DE" }, { GTAX, "HL" },
	{ GTAX, "DE+" }, { GTAX, "HL+" }, { GTAX, "DE-" }, { GTAX, "HL-" },

	{ illegal, nullptr }, { SUBNBX, "BC" }, { SUBNBX, "DE" }, { SUBNBX, "HL" },
	{ SUBNBX, "DE+" }, { SUBNBX, "HL+" }, { SUBNBX,  }, { SUBNBX, "HL-" },
	{ illegal, nullptr }, { LTAX, "BC" }, { LTAX, "DE" }, { LTAX, "HL" },
	{ LTAX, "DE+" }, { LTAX, "HL+" }, { LTAX, "DE-" }, { LTAX, "HL-" },

	/* 0xC0 - 0xFF */
	{ illegal, nullptr }, { ADDX, "BC" }, { ADDX, "DE" }, { ADDX, "HL" },
	{ ADDX, "DE+" }, { ADDX, "HL+" }, { ADDX, "DE-" }, { ADDX, "HL-" },
	{ illegal, nullptr }, { ONAX, "BC" }, { ONAX, "DE" }, { ONAX, "HL" },
	{ ONAX, "DE+" }, { ONAX, "HL+" }, { ONAX, "DE-" }, { ONAX, "HL-" },

	{ illegal, nullptr }, { ADCX, "BC" }, { ADCX, "DE" }, { ADCX, "HL" },
	{ ADCX, "DE+" }, { ADCX, "HL+" }, { ADCX, "DE-" }, { ADCX, "HL-" },
	{ illegal, nullptr }, { OFFAX, "BC" }, { OFFAX, "DE" }, { OFFAX, "HL" },
	{ OFFAX, "DE+" }, { OFFAX, "HL+" }, { OFFAX, "DE-" }, { OFFAX, "HL-" },

	{ illegal, nullptr }, { SUBX, "BC" }, { SUBX, "DE" }, { SUBX, "HL" },
	{ SUBX, "DE+" }, { SUBX, "HL+" }, { SUBX, "DE-" }, { SUBX, "HL-" },
	{ illegal, nullptr }, { NEAX, "BC" }, { NEAX, "DE" }, { NEAX, "HL" },
	{ NEAX, "DE+" }, { NEAX, "HL+" }, { NEAX, "DE-" }, { NEAX, "HL-" },

	{ illegal, nullptr }, { SBBX, "BC" }, { SBBX, "DE" }, { SBBX, "HL" },
	{ SBBX, "DE+" }, { SBBX, "HL+" }, { SBBX, "DE-" }, { SBBX, "HL-" },
	{ illegal, nullptr }, { EQAX, "BC" }, { EQAX, "DE" }, { EQAX, "HL" },
	{ EQAX, "DE+" }, { EQAX, "HL+" }, { EQAX, "DE-" }, { EQAX, "HL-" },
};

static const struct dasm_s dasm74_7801[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	/* 0x40 - 0x7F */
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	/* 0x80 - 0xBF */
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ANAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ XRAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ORAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ ADDNCW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ GTAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ SUBNBW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ LTAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	/* 0xC0 - 0xFF */
	{ ADDW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ ONAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ ADCW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ OFFAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ SUBW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ NEAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },

	{ SBBW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ EQAW, "%a" }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr },
	{ illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }, { illegal, nullptr }
};

static const struct dasm_s dasmXX_7801[256] = {
	/* 0x00 - 0x3F */
	{ NOP, nullptr },         { HALT, nullptr },        { INX, "SP" },      { DCX, "SP" },
	{ LXI, "SP,%w" },   { ANIW, "%a,%b" },  { illegal, nullptr },     { ANI, "A,%b" },
	{ RET, nullptr },         { SIO, nullptr },         { MOV, "A,B" },     { MOV, "A,C" },
	{ MOV, "A,D" },     { MOV, "A,E" },     { MOV, "A,H" },     { MOV, "A,L" },

	{ EX, nullptr },          { EXX, nullptr },         { INX, "BC" },      { DCX, "BC" },
	{ LXI, "BC,%w" },   { ORIW, "%a,%b" },  { XRI, "A,%b" },    { ORI, "A,%b" },
	{ RETS, nullptr },        { STM, nullptr },         { MOV, "B,A" },     { MOV, "C,A" },
	{ MOV, "D,A" },     { MOV, "E,A" },     { MOV, "H,A" },     { MOV, "L,A" },

	{ INRW, "%a" },     { TABLE, nullptr },       { INX, "DE" },      { DCX, "DE" },
	{ LXI, "DE,%w" },   { GTIW, "%a,%b" },  { ADINC, "A,%b" },  { GTI, "A,%b" },
	{ LDAW, "%a" },     { LDAX, "BC" },     { LDAX, "DE" },     { LDAX, "HL" },
	{ LDAX, "DE+" },    { LDAX, "HL+" },    { LDAX, "DE-" },    { LDAX, "HL-" },

	{ DCRW, "%a" },     { BLOCK, nullptr },       { INX, "HL" },      { DCX, "HL" },
	{ LXI, "HL,%w" },   { LTIW, "%a,%b" },  { SUINB, "A,%b" },  { LTI, "A,%b" },
	{ STAW, "%a" },     { STAX, "BC" },     { STAX, "DE" },     { STAX, "HL" },
	{ STAX, "DE+" },    { STAX, "HL+" },    { STAX, "DE-" },    { STAX, "HL-" },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { INR, "A" },       { INR, "B" },       { INR, "C" },
	{ CALL, "%w" },     { ONIW, "%a,%b" },  { ADI, "A,%b" },    { ONI, "A,%b" },
	{ 0, dasm48_7801 }, { MVIX, "BC,%b" },  { MVIX, "DE,%b" },  { MVIX, "HL,%b" },
	{ 0, dasm4c_7801 }, { 0, dasm4d_7801 }, { JRE, "%d" },      { JRE, "%d" },

	{ illegal, nullptr },     { DCR, "A" },       { DCR, "B" },       { DCR, "C" },
	{ JMP, "%w" },      { OFFIW, "%a,%b" }, { ACI, "A,%b" },    { OFFI, "A,%b" },
	{ BIT, "0,%a" },    { BIT, "1,%a" },    { BIT," 2,%a" },    { BIT, "3,%a" },
	{ BIT, "4,%a" },    { BIT, "5,%a" },    { BIT, "6,%a" },    { BIT, "7,%a" },

	{ 0, dasm60_7801 }, { DAA, nullptr },         { RETI, nullptr },        { CALB, nullptr },
	{ 0, dasm64_7801 }, { NEIW, "%a,%b" },  { SUI, "A,%b" },    { NEI, "A,%b" },
	{ MVI, "V,%b" },    { MVI, "A,%b" },    { MVI, "B,%b" },    { MVI, "C,%b" },
	{ MVI, "D,%b" },    { MVI, "E,%b" },    { MVI, "H,%b" },    { MVI, "L,%b" },

	{ 0, dasm70_7801 }, { MVIW, "%a,%b" },  { SOFTI, nullptr },       { JB, nullptr },
	{ 0, dasm74_7801 }, { EQIW, "%a,%b" },  { SBI, "A,%b" },    { EQI, "A,%b" },
	{ CALF, "%f" },     { CALF, "%f" },     { CALF, "%f" },     { CALF, "%f" },
	{ CALF, "%f" },     { CALF, "%f" },     { CALF, "%f" },     { CALF, "%f" },

	/* 0x80 - 0xBF */
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },

	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },

	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },

	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },

	/* 0xC0 - 0xFF */
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },

	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },

	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },

	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" }
};

/*********************************************************
 *
 * uPD7801
 *
 *********************************************************/

static const struct dasm_s dasm48_78c05[256] = {
	/* 0x00 - 0x3F */
	{ SKIT, "F0" },     { SKIT, "FT" },     { SKIT, "F1" },     { illegal, nullptr }, /* TODO: PDF doesn't mention SKIT and SK */
	{ SKIT, "FS" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { SK, "CY" },       { illegal, nullptr },
	{ SK, "Z" },        { illegal, nullptr },     { PUSH, "VA" },     { POP, "VA" },

	{ SKNIT, "F0" },    { SKNIT, "FT" },    { SKNIT, "F1" },    { illegal, nullptr },
	{ SKNIT, "FS" },    { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { SKN, "CY" },      { illegal, nullptr },
	{ SKN, "Z" },       { illegal, nullptr },     { PUSH, "BC" },     { POP, "BC" },

	{ EI, nullptr },          { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ DI, nullptr },          { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { CLC, nullptr },         { STC, nullptr },
	{ illegal, nullptr },     { PEX, nullptr },         { PUSH, "DE" },     { POP, "DE" },

	{ RLL, "A" },       { RLR, "A" },       { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ RLD, nullptr },         { RRD, nullptr },         { illegal, nullptr },     { illegal, nullptr },
	{ PER, nullptr },         { illegal, nullptr },     { PUSH, "HL" },     { POP, "HL" },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0xC0 - 0xFF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr }
};

static const struct dasm_s dasm4c_78c05[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0xC0 - 0xFF */
	{ MOV, "A,PA" },    { MOV, "A,PB" },    { MOV, "A,PC" },    { MOV, "A,MK" },
	{ MOV, "A,MB" },    { MOV, "A,MC" },    { MOV, "A,TM0" },   { MOV, "A,TM1" },
	{ MOV, "A,S" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr }, /* TODO: Figure out what regsiter C9 indicates */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr }
};

static const struct dasm_s dasm4d_78c05[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0xC0 - 0xFF */
	{ MOV, "PA,A" },    { MOV, "PB,A" },    { MOV, "PC,A" },    { MOV, "MK,A" },
	{ MOV, "MB,A" },    { MOV, "MC,A" },    { MOV, "TM0,A" },   { MOV, "TM1,A" },
	{ MOV, "S,A" },     { MOV, "TMM,A" },   { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr }
};

static const struct dasm_s dasm60_78c05[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { ANA, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { XRA, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { ORA, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { ADDNC, "A,A" },   { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { GTA, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { SUBNB, "A,A" },   { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { LTA, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { ADD, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { ADC, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { SUB, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { NEA, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { SBB, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { EQA, "A,A" },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { ANA, "A,A" },     { ANA, "A,B" },     { ANA, "A,C" },
	{ ANA, "A,D" },     { ANA, "A,E" },     { ANA, "A,H" },     { ANA, "A,L" },

	{ illegal, nullptr },     { XRA, "A,A" },     { XRA, "A,B" },     { XRA, "A,C" },
	{ XRA, "A,D" },     { XRA, "A,E" },     { XRA, "A,H" },     { XRA, "A,L" },
	{ illegal, nullptr },     { ORA, "A,A" },     { ORA, "A,B" },     { ORA, "A,C" },
	{ ORA, "A,D" },     { ORA, "A,E" },     { ORA, "A,H" },     { ORA, "A,L" },

	{ illegal, nullptr },     { ADDNC, "A,A" },   { ADDNC, "A,B" },   { ADDNC, "A,C" },
	{ ADDNC, "A,D" },   { ADDNC, "A,E" },   { ADDNC, "A,H" },   { ADDNC, "A,L" },
	{ illegal, nullptr },     { GTA, "A,A" },     { GTA, "A,B" },     { GTA, "A,C" },
	{ GTA, "A,D" },     { GTA, "A,E" },     { GTA, "A,H" },     { GTA, "A,L" },

	{ illegal, nullptr },     { SUBNB, "A,A" },   { SUBNB, "A,B" },   { SUBNB, "A,C" },
	{ SUBNB, "A,D" },   { SUBNB, "A,E" },   { SUBNB, "A,H" },   { SUBNB, "A,L" },
	{ illegal, nullptr },     { LTA, "A,A" },     { LTA, "A,B" },     { LTA, "A,C" },
	{ LTA, "A,D" },     { LTA, "A,E" },     { LTA, "A,H" },     { LTA, "A,L" },

	/* 0xC0 - 0xFF */
	{ illegal, nullptr },     { ADD, "A,A" },     { ADD, "A,B" },     { ADD, "A,C" },
	{ ADD, "A,D" },     { ADD, "A,E" },     { ADD, "A,H" },     { ADD, "A,L" },
	{ illegal, nullptr },     { ONA, "A,A" },     { ONA, "A,B" },     { ONA, "A,C" },
	{ ONA, "A,D" },     { ONA, "A,E" },     { ONA, "A,H" },     { ONA, "A,L" },

	{ illegal, nullptr },     { ADC, "A,A" },     { ADC, "A,B" },     { ADC, "A,C" },
	{ ADC, "A,D" },     { ADC, "A,E" },     { ADC, "A,H" },     { ADC, "A,L" },
	{ illegal, nullptr },     { OFFA, "A,A" },    { OFFA, "A,B" },    { OFFA, "A,C" },
	{ OFFA, "A,D" },    { OFFA, "A,E" },    { OFFA, "A,H" },    { OFFA, "A,L" },

	{ illegal, nullptr },     { SUB, "A,A" },     { SUB, "A,B" },     { SUB, "A,C" },
	{ SUB, "A,D" },     { SUB, "A,E" },     { SUB, "A,H" },     { SUB, "A,L" },
	{ illegal, nullptr },     { NEA, "A,A" },     { NEA, "A,B" },     { NEA, "A,C" },
	{ NEA, "A,D" },     { NEA, "A,E" },     { NEA, "A,H" },     { NEA, "A,L" },

	{ illegal, nullptr },     { SBB, "A,A" },     { SBB, "A,B" },     { SBB, "A,C" },
	{ SBB, "A,D" },     { SBB, "A,E" },     { SBB, "A,H" },     { SBB, "A,L" },
	{ illegal, nullptr },     { EQA, "A,A" },     { EQA, "A,B" },     { EQA, "A,C" },
	{ EQA, "A,D" },     { EQA, "A,E" },     { EQA, "A,H" },     { EQA, "A,L" }
};

static const struct dasm_s dasm64_78c05[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { ANI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { XRI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { ORI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { ADINC, "A,%b" },  { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { GTI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { SUINB, "A,%b" },  { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { LTI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { ADI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { ONI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { ACI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { OFFI, "A,%b" },   { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { SUI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { NEI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { SBI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { EQI, "A,%b" },    { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ ANI, "PA,%b" },   { ANI, "PB,%b" },   { ANI, "PC,%b" },   { ANI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ XRI, "PA,%b" },   { XRI, "PB,%b" },   { XRI, "PC,%b" },   { XRI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ ORI, "PA,%b" },   { ORI, "PB,%b" },   { ORI, "PC,%b" },   { ORI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ ADINC, "PA,%b" }, { ADINC, "PB,%b" }, { ADINC, "PC,%b" }, { ADINC, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ GTI, "PA,%b" },   { GTI, "PB,%b" },   { GTI, "PC,%b" },   { GTI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ SUINB, "PA,%b" }, { SUINB, "PB,%b" }, { SUINB, "PC,%b" }, { SUINB, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ LTI, "PA,%b" },   { LTI, "PB,%b" },   { LTI, "PC,%b" },   { LTI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0xC0 - 0xFF */
	{ ADI, "PA,%b" },   { ADI, "PB,%b" },   { ADI, "PC,%b" },   { ADI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ ONI, "PA,%b" },   { ONI, "PB,%b" },   { ONI, "PC,%b" },   { ONI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ ACI, "PA,%b" },   { ACI, "PB,%b" },   { ACI, "PC,%b" },   { ACI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ SUI, "PA,%b" },   { SUI, "PB,%b" },   { SUI, "PC,%b" },   { SUI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ NEI, "PA,%b" },   { NEI, "PB,%b" },   { NEI, "PC,%b" },   { NEI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ SBI, "PA,%b" },   { SBI, "PB,%b" },   { SBI, "PC,%b" },   { SBI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ SBI, "PA,%b" },   { SBI, "PB,%b" },   { SBI, "PC,%b" },   { SBI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ EQI, "PA,%b" },   { EQI, "PB,%b" },   { EQI, "PC,%b" },   { EQI, "MK,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr }
};

static const struct dasm_s dasm70_78c05[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { SSPD, "%w" },     { LSPD, "%w" },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { SBCD, "%w" },     { LBCD, "%w" },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { SDED, "%w" },     { LDED, "%w" },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { SHLD, "%w" },     { LHLD, "%w" },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { MOV, "A,%w" },    { MOV, "B,%w" },    { MOV, "C,%w" },
	{ MOV, "D,%w" },    { MOV, "E,%w" },    { MOV, "H,%w" },    { MOV, "L,%w" },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { MOV, "%w,A" },    { MOV, "%w,B" },    { MOV, "%w,C" },
	{ MOV, "%w,D" },    { MOV, "%w,E" },    { MOV, "%w,H" },    { MOV, "%w,L" },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { ANAX, "BC" },     { ANAX, "DE" },     { ANAX, "HL" },
	{ ANAX, "DE+" },    { ANAX, "HL+" },    { ANAX, "DE-" },    { ANAX, "HL-" },

	{ illegal, nullptr },     { XRAX, "BC" },     { XRAX, "DE" },     { XRAX, "HL" },
	{ XRAX, "DE+" },    { XRAX, "HL+" },    { XRAX, "DE-" },    { XRAX, "HL-" },
	{ illegal, nullptr },     { ORAX, "BC" },     { ORAX, "DE" },     { ORAX, "HL" },
	{ ORAX, "DE+" },    { ORAX, "HL+" },    { ORAX, "DE-" },    { ORAX, "HL-" },

	{ illegal, nullptr },     { ADDNCX, "BC" },   { ADDNCX, "DE" },   { ADDNCX, "HL" },
	{ ADDNCX, "DE+" },  { ADDNCX, "HL+" },  { ADDNCX, "DE-" },  { ADDNCX, "HL-" },
	{ illegal, nullptr },     { GTAX, "BC" },     { GTAX, "DE" },     { GTAX, "HL" },
	{ GTAX, "DE+" },    { GTAX, "HL+" },    { GTAX, "DE-" },    { GTAX, "HL-" },

	{ illegal, nullptr },     { SUBNBX, "BC" },   { SUBNBX, "DE" },   { SUBNBX, "HL" },
	{ SUBNBX, "DE+" },  { SUBNBX, "HL+" },  { SUBNBX, "DE-" },  { SUBNBX, "HL-" },
	{ illegal, nullptr },     { LTAX, "BC" },     { LTAX, "DE" },     { LTAX, "HL" },
	{ LTAX, "DE+" },    { LTAX, "HL+" },    { LTAX, "DE-" },    { LTAX, "HL-" },

	/* 0xC0 - 0xFF */
	{ illegal, nullptr },     { ADDX, "BC" },     { ADDX, "DE" },     { ADDX, "HL" },
	{ ADDX, "DE+" },    { ADDX, "HL+" },    { ADDX, "DE-" },    { ADDX, "HL-" },
	{ illegal, nullptr },     { ONAX, "BC" },     { ONAX, "DE" },     { ONAX, "HL" },
	{ ONAX, "DE+" },    { ONAX, "HL+" },    { ONAX, "DE-" },    { ONAX, "HL-" },

	{ illegal, nullptr },     { ADCX, "BC" },     { ADCX, "DE" },     { ADCX, "HL" },
	{ ADCX, "DE+" },    { ADCX, "HL+" },    { ADCX, "DE-" },    { ADCX, "HL-" },
	{ illegal, nullptr },     { OFFAX, "BC" },    { OFFAX, "DE" },    { OFFAX, "HL" },
	{ OFFAX, "DE+" },   { OFFAX, "HL+" },   { OFFAX, "DE-" },   { OFFAX, "HL-" },

	{ illegal, nullptr },     { SUBX, "BC" },     { SUBX, "DE" },     { SUBX, "HL" },
	{ SUBX, "DE+" },    { SUBX, "HL+" },    { SUBX, "DE-" },    { SUBX, "HL-" },
	{ illegal, nullptr },     { NEAX, "BC" },     { NEAX, "DE" },     { NEAX, "HL" },
	{ NEAX, "DE+" },    { NEAX, "HL+" },    { NEAX, "DE-" },    { NEAX, "HL-" },

	{ illegal, nullptr },     { SBBX, "BC" },     { SBBX, "DE" },     { SBBX, "HL" },
	{ SBBX, "DE+" },    { SBBX, "HL+" },    { SBBX, "DE-" },    { SBBX, "HL-" },
	{ illegal, nullptr },     { EQAX, "BC" },     { EQAX, "DE" },     { EQAX, "HL" },
	{ EQAX, "DE+" },    { EQAX, "HL+" },    { EQAX, "DE-" },    { EQAX, "HL-" },
};

static const struct dasm_s dasm74_78c05[256] = {
	/* 0x00 - 0x3F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0x80 - 0xBF */
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ ANAW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ XRAW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ ORAW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ ADDNCW, "%a" },   { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ GTAW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ SUBNBW, "%a" },   { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ LTAW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	/* 0xC0 - 0xFF */
	{ ADDW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ ONAW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ ADCW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ OFFAW, "%a" },    { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ SUBW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ NEAW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ SBBW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ EQAW, "%a" },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
};

static const struct dasm_s dasmXX_78c05[256] = {
	/* 0x00 - 0x3F */
	{ NOP, nullptr },         { HALT, nullptr },        { INX, "SP" },      { DCX, "SP" },
	{ LXI, "SP,%w" },   { ANIW, "%a,%b" },  { illegal, nullptr },     { ANI, "A,%b" },
	{ RET, nullptr },         { SIO, nullptr },         { MOV, "A,B" },     { MOV, "A,C" },
	{ MOV, "A,D" },     { MOV, "A,E" },     { MOV, "A,H" },     { MOV, "A,L" },

	{ illegal, nullptr },     { illegal, nullptr },     { INX, "BC" },      { DCX, "BC" },
	{ LXI, "BC,%w" },   { ORIW, "%a,%b" },  { XRI, "A,%b" },    { ORI, "A,%b" },
	{ RETS, nullptr },        { STM, nullptr },         { MOV, "B,A" },     { MOV, "C,A" },
	{ MOV, "D,A" },     { MOV, "E,A" },     { MOV, "H,A" },     { MOV, "L,A" },

	{ INRW, "%a" },     { illegal, nullptr },     { INX, "DE" },      { DCX, "DE" },
	{ LXI, "DE,%w" },   { GTIW, "%a,%b" },  { ADINC, "A,%b" },  { GTI, "A,%b" },
	{ LDAW, "%a" },     { LDAX, "BC" },     { LDAX, "DE" },     { LDAX, "HL" },
	{ LDAX, "DE+" },    { LDAX, "HL+" },    { LDAX, "DE-" },    { LDAX, "HL-" },

	{ DCRW, "%a" },     { illegal, nullptr },     { INX, "HL" },      { DCX, "HL" },
	{ LXI, "HL,%w" },   { LTIW, "%a,%b" },  { SUINB, "A,%b" },  { LTI, "A,%b" },
	{ STAW, "%a" },     { STAX, "BC" },     { STAX, "DE" },     { STAX, "HL" },
	{ STAX, "DE+" },    { STAX, "HL+" },    { STAX, "DE-" },    { STAX, "HL-" },

	/* 0x40 - 0x7F */
	{ illegal, nullptr },     { INR, "A" },       { INR, "B" },       { INR, "C" },
	{ CALL, "%w" },     { ONIW, "%a,%b" },  { ADI, "A,%b" },    { ONI, "A,%b" },
	{ 0, dasm48_78c05 },    { illegal, nullptr }, { illegal, nullptr },     { illegal, nullptr },
	{ 0, dasm4c_78c05 },    { 0, dasm4d_78c05 },    { JRE, "%d" },      { JRE, "%d" },

	{ illegal, nullptr },     { DCR, "A" },       { DCR, "B" },       { DCR, "C" },
	{ JMP, "%w" },      { OFFIW, "%a,%b" }, { ACI, "A,%b" },    { OFFI, "A,%b" },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },
	{ illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },     { illegal, nullptr },

	{ 0, dasm60_78c05 },    { DAA, nullptr },         { RETI, nullptr },        { CALB, nullptr },
	{ 0, dasm64_78c05 },    { NEIW, "%a,%b" },  { SUI, "A,%b" },    { NEI, "A,%b" },
	{ illegal, nullptr },     { MVI, "A,%b" },    { MVI, "B,%b" },    { MVI, "C,%b" },
	{ MVI, "D,%b" },    { MVI, "E,%b" },    { MVI, "H,%b" },    { MVI, "L,%b" },

	{ 0, dasm70_78c05 },    { MVIW, "%a,%b" },  { SOFTI, nullptr },       { JB, nullptr },
	{ 0, dasm74_78c05 },    { EQIW, "%a,%b" },  { SBI, "A,%b" },    { EQI, "A,%b" },
	{ CALF, "%f" },     { CALF, "%f" },     { CALF, "%f" },     { CALF, "%f" },
	{ CALF, "%f" },     { CALF, "%f" },     { CALF, "%f" },     { CALF, "%f" },

	/* 0x80 - 0xBF */
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },

	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },

	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },

	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },
	{ CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },     { CALT, "%t" },

	/* 0xC0 - 0xFF */
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },

	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },

	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },

	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" },
	{ JR, "%o" },       { JR, "%o" },       { JR, "%o" },       { JR, "%o" }
};


/* register names for bit manipulation instructions */
static const char *const regname[32] =
{
	"illegal", "illegal", "illegal", "illegal",
	"illegal", "illegal", "illegal", "illegal",
	"illegal", "illegal", "illegal", "illegal",
	"illegal", "illegal", "illegal", "illegal",
	"PA",      "PB",      "PC",      "PD",
	"illegal", "PF",      "MKH",     "MKL",
	"illegal", "SMH",     "illegal", "EOM",
	"illegal", "TMM",     "PT",      "illegal"
};

static offs_t Dasm( char *buffer, offs_t pc, const struct dasm_s *dasmXX, const UINT8 *oprom, const UINT8 *opram, int is_7810 )
{
	int idx = 0;
	UINT8 op = oprom[idx++], op2, t;
	int offset;
	UINT16 ea;
	const char *a;
	UINT32 flags = 0;

	t = dasmXX[op].token;
	a = (const char *)dasmXX[op].args;

	/* 0 token means prefix opcode (use table from args) */
	if (0 == t)
	{
		struct dasm_s *p_dasm = (struct dasm_s *)a;

		op2 = oprom[idx++];
		t = p_dasm[op2].token;
		a = (const char *)p_dasm[op2].args;
	}

	buffer += sprintf(buffer, "%-8.8s", token[t]);

	if (t == CALB || t == CALF || t == CALL || t == CALT)
		flags = DASMFLAG_STEP_OVER;
	else if (t == RET || t == RETI)
		flags = DASMFLAG_STEP_OUT;

	while (a && *a)
	{
		if ('%' == *a)
		{
			a++;
			switch (*a)
			{
			case 'a':   /* address V * 256 + offset */
				op2 = opram[idx++];
				buffer += sprintf(buffer, "VV:%02X", op2);
				break;
			case 'b':   /* immediate byte */
				buffer += sprintf(buffer, "$%02X", opram[idx++]);
				break;
			case 'w':   /* immediate word */
				ea = opram[idx++];
				ea += opram[idx++] << 8;
				buffer += sprintf(buffer, "$%04X", ea);
				break;
			case 'd':   /* JRE address */
				op2 = oprom[idx++];
				offset = (op & 1) ? -(256 - op2): + op2;
				buffer += sprintf(buffer, "$%04X", ( pc + idx + offset ) & 0xFFFF );
				break;
			case 't':   /* CALT address */
				ea = 0x80 + 2 * (op & (is_7810 ? 0x1f : 0x3f));
				buffer += sprintf(buffer, "($%04X)", ea);
				break;
			case 'f':   /* CALF address */
				op2 = oprom[idx++];
				ea = 0x800 + 0x100 * (op & 0x07) + op2;
				buffer += sprintf(buffer, "$%04X", ea);
				break;
			case 'o':   /* JR offset */
				offset = ( ( op & 0x20 ) ? -0x20 : 0 ) + ( op & 0x1F );
				buffer += sprintf(buffer, "$%04X", ( pc + idx + offset ) & 0xFFFF );
				break;
			case 'i':   /* bit manipulation */
				op2 = oprom[idx++];
				buffer += sprintf(buffer, "%s,%d", regname[op2 & 0x1f], op2 >> 5);
				break;
			default:
				*buffer++ = *a;
			}
		}
		else
			*buffer++ = *a;
		a++;
	}
	*buffer = '\0';

	return idx | flags | DASMFLAG_SUPPORTED;
}

CPU_DISASSEMBLE( upd7810 )
{
	return Dasm( buffer, pc, dasmXX_7810, oprom, opram, 1 );
}

CPU_DISASSEMBLE( upd7807 )
{
	return Dasm( buffer, pc, dasmXX_7807, oprom, opram, 1 );
}

CPU_DISASSEMBLE( upd7801 )
{
	return Dasm( buffer, pc, dasmXX_7801, oprom, opram, 0 );
}

CPU_DISASSEMBLE( upd78c05 )
{
	return Dasm( buffer, pc, dasmXX_78c05, oprom, opram, 0 );
}
