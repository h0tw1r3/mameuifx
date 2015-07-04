-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   machine.lua
--
--   Rules for building machine cores
--
----------------------------------------------------------------------------


---------------------------------------------------
--
--@src/emu/machine/akiko.h,MACHINES += AKIKO
---------------------------------------------------

if (MACHINES["AKIKO"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/akiko.c",
		MAME_DIR .. "src/emu/machine/akiko.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/autoconfig.h,MACHINES += AUTOCONFIG
---------------------------------------------------

if (MACHINES["AUTOCONFIG"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/autoconfig.c",
		MAME_DIR .. "src/emu/machine/autoconfig.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/cr511b.h,MACHINES += CR511B
---------------------------------------------------

if (MACHINES["CR511B"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/cr511b.c",
		MAME_DIR .. "src/emu/machine/cr511b.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/dmac.h,MACHINES += DMAC
---------------------------------------------------

if (MACHINES["DMAC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/dmac.c",
		MAME_DIR .. "src/emu/machine/dmac.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/gayle.h,MACHINES += GAYLE
---------------------------------------------------

if (MACHINES["GAYLE"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/gayle.c",
		MAME_DIR .. "src/emu/machine/gayle.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/40105.h,MACHINES += CMOS40105
---------------------------------------------------

if (MACHINES["CMOS40105"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/40105.c",
		MAME_DIR .. "src/emu/machine/40105.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/53c7xx.h,MACHINES += NCR53C7XX
---------------------------------------------------

if (MACHINES["NCR53C7XX"]~=null) then
	MACHINES["NSCSI"] = true
	files {
		MAME_DIR .. "src/emu/machine/53c7xx.c",
		MAME_DIR .. "src/emu/machine/53c7xx.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/53c810.h,MACHINES += LSI53C810
---------------------------------------------------

if (MACHINES["LSI53C810"]~=null) then
	MACHINES["SCSI"] = true
	files {
		MAME_DIR .. "src/emu/machine/53c810.c",
		MAME_DIR .. "src/emu/machine/53c810.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/6522via.h,MACHINES += 6522VIA
---------------------------------------------------

if (MACHINES["6522VIA"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/6522via.c",
		MAME_DIR .. "src/emu/machine/6522via.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/6525tpi.h,MACHINES += TPI6525
---------------------------------------------------

if (MACHINES["TPI6525"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/6525tpi.c",
		MAME_DIR .. "src/emu/machine/6525tpi.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/6532riot.h,MACHINES += RIOT6532
---------------------------------------------------

if (MACHINES["RIOT6532"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/6532riot.c",
		MAME_DIR .. "src/emu/machine/6532riot.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/6821pia.h,MACHINES += 6821PIA
---------------------------------------------------

if (MACHINES["6821PIA"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/6821pia.c",
		MAME_DIR .. "src/emu/machine/6821pia.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/6840ptm.h,MACHINES += 6840PTM
---------------------------------------------------

if (MACHINES["6840PTM"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/6840ptm.c",
		MAME_DIR .. "src/emu/machine/6840ptm.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/6850acia.h,MACHINES += ACIA6850
---------------------------------------------------

if (MACHINES["ACIA6850"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/6850acia.c",
		MAME_DIR .. "src/emu/machine/6850acia.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/68561mpcc.h,MACHINES += 68561MPCC
---------------------------------------------------

if (MACHINES["68561MPCC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/68561mpcc.c",
		MAME_DIR .. "src/emu/machine/68561mpcc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc68681.h,MACHINES += 68681
---------------------------------------------------

if (MACHINES["68681"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc68681.c",
		MAME_DIR .. "src/emu/machine/mc68681.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/7200fifo.h,MACHINES += 7200FIFO
---------------------------------------------------

if (MACHINES["7200FIFO"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/7200fifo.c",
		MAME_DIR .. "src/emu/machine/7200fifo.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/74123.h,MACHINES += TTL74123
---------------------------------------------------

if (MACHINES["TTL74123"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/74123.c",
		MAME_DIR .. "src/emu/machine/74123.h",
		MAME_DIR .. "src/emu/machine/rescap.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/74145.h,MACHINES += TTL74145
---------------------------------------------------

if (MACHINES["TTL74145"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/74145.c",
		MAME_DIR .. "src/emu/machine/74145.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/74148.h,MACHINES += TTL74148
---------------------------------------------------

if (MACHINES["TTL74148"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/74148.c",
		MAME_DIR .. "src/emu/machine/74148.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/74153.h,MACHINES += TTL74153
---------------------------------------------------

if (MACHINES["TTL74153"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/74153.c",
		MAME_DIR .. "src/emu/machine/74153.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/74181.h,MACHINES += TTL74181
---------------------------------------------------

if (MACHINES["TTL74181"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/74181.c",
		MAME_DIR .. "src/emu/machine/74181.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/7474.h,MACHINES += TTL7474
---------------------------------------------------

if (MACHINES["TTL7474"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/7474.c",
		MAME_DIR .. "src/emu/machine/7474.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/8042kbdc.h,MACHINES += KBDC8042
---------------------------------------------------

if (MACHINES["KBDC8042"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/8042kbdc.c",
		MAME_DIR .. "src/emu/machine/8042kbdc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/8530scc.h,MACHINES += 8530SCC
---------------------------------------------------

if (MACHINES["8530SCC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/8530scc.c",
		MAME_DIR .. "src/emu/machine/8530scc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/aakart.h,MACHINES += AAKARTDEV
---------------------------------------------------

if (MACHINES["AAKARTDEV"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/aakart.c",
		MAME_DIR .. "src/emu/machine/aakart.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/adc0808.h,MACHINES += ADC0808
---------------------------------------------------

if (MACHINES["ADC0808"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/adc0808.c",
		MAME_DIR .. "src/emu/machine/adc0808.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/adc083x.h,MACHINES += ADC083X
---------------------------------------------------

if (MACHINES["ADC083X"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/adc083x.c",
		MAME_DIR .. "src/emu/machine/adc083x.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/adc1038.h,MACHINES += ADC1038
---------------------------------------------------

if (MACHINES["ADC1038"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/adc1038.c",
		MAME_DIR .. "src/emu/machine/adc1038.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/adc1213x.h,MACHINES += ADC1213X
---------------------------------------------------

if (MACHINES["ADC1213X"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/adc1213x.c",
		MAME_DIR .. "src/emu/machine/adc1213x.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/aicartc.h,MACHINES += AICARTC
---------------------------------------------------

if (MACHINES["AICARTC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/aicartc.c",
		MAME_DIR .. "src/emu/machine/aicartc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/am53cf96.h,MACHINES += AM53CF96
---------------------------------------------------

if (MACHINES["AM53CF96"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/am53cf96.c",
		MAME_DIR .. "src/emu/machine/am53cf96.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/am9517a.h,MACHINES += AM9517A
---------------------------------------------------

if (MACHINES["AM9517A"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/am9517a.c",
		MAME_DIR .. "src/emu/machine/am9517a.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/amigafdc.h,MACHINES += AMIGAFDC
---------------------------------------------------

if (MACHINES["AMIGAFDC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/amigafdc.c",
		MAME_DIR .. "src/emu/machine/amigafdc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/at28c16.h,MACHINES += AT28C16
---------------------------------------------------

if (MACHINES["AT28C16"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/at28c16.c",
		MAME_DIR .. "src/emu/machine/at28c16.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/at29040a.h,MACHINES += AT29040
---------------------------------------------------

if (MACHINES["AT29040"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/at29040a.c",
		MAME_DIR .. "src/emu/machine/at29040a.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/at45dbxx.h,MACHINES += AT45DBXX
---------------------------------------------------

if (MACHINES["AT45DBXX"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/at45dbxx.c",
		MAME_DIR .. "src/emu/machine/at45dbxx.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ataflash.h,MACHINES += ATAFLASH
---------------------------------------------------

if (MACHINES["ATAFLASH"]~=null) then
	MACHINES["IDE"] = true
	MACHINES["PCCARD"] = true
	files {
		MAME_DIR .. "src/emu/machine/ataflash.c",
		MAME_DIR .. "src/emu/machine/ataflash.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ay31015.h,MACHINES += AY31015
---------------------------------------------------

if (MACHINES["AY31015"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ay31015.c",
		MAME_DIR .. "src/emu/machine/ay31015.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/bankdev.h,MACHINES += BANKDEV
---------------------------------------------------

if (MACHINES["BANKDEV"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/bankdev.c",
		MAME_DIR .. "src/emu/machine/bankdev.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/cdp1852.h,MACHINES += CDP1852
---------------------------------------------------

if (MACHINES["CDP1852"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/cdp1852.c",
		MAME_DIR .. "src/emu/machine/cdp1852.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/cdp1871.h,MACHINES += CDP1871
---------------------------------------------------

if (MACHINES["CDP1871"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/cdp1871.c",
		MAME_DIR .. "src/emu/machine/cdp1871.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/com8116.h,MACHINES += COM8116
---------------------------------------------------

if (MACHINES["COM8116"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/com8116.c",
		MAME_DIR .. "src/emu/machine/com8116.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/cr589.h,MACHINES += CR589
---------------------------------------------------

if (MACHINES["CR589"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/cr589.c",
		MAME_DIR .. "src/emu/machine/cr589.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/cs4031.h,MACHINES += CS4031
---------------------------------------------------

if (MACHINES["CS4031"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/cs4031.c",
		MAME_DIR .. "src/emu/machine/cs4031.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/cs8221.h,MACHINES += CS8221
---------------------------------------------------

if (MACHINES["CS8221"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/cs8221.c",
		MAME_DIR .. "src/emu/machine/cs8221.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ds1204.h,MACHINES += DS1204
---------------------------------------------------

if (MACHINES["DS1204"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ds1204.c",
		MAME_DIR .. "src/emu/machine/ds1204.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ds1302.h,MACHINES += DS1302
---------------------------------------------------

if (MACHINES["DS1302"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ds1302.c",
		MAME_DIR .. "src/emu/machine/ds1302.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ds1315.h,MACHINES += DS1315
---------------------------------------------------

if (MACHINES["DS1315"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ds1315.c",
		MAME_DIR .. "src/emu/machine/ds1315.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ds2401.h,MACHINES += DS2401
---------------------------------------------------

if (MACHINES["DS2401"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ds2401.c",
		MAME_DIR .. "src/emu/machine/ds2401.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ds2404.h,MACHINES += DS2404
---------------------------------------------------

if (MACHINES["DS2404"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ds2404.c",
		MAME_DIR .. "src/emu/machine/ds2404.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ds75160a.h,MACHINES += DS75160A
---------------------------------------------------

if (MACHINES["DS75160A"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ds75160a.c",
		MAME_DIR .. "src/emu/machine/ds75160a.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ds75161a.h,MACHINES += DS75161A
---------------------------------------------------

if (MACHINES["DS75161A"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ds75161a.c",
		MAME_DIR .. "src/emu/machine/ds75161a.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/e0516.h,MACHINES += E0516
---------------------------------------------------

if (MACHINES["E0516"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/e0516.c",
		MAME_DIR .. "src/emu/machine/e0516.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/e05a03.h,MACHINES += E05A03
---------------------------------------------------

if (MACHINES["E05A03"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/e05a03.c",
		MAME_DIR .. "src/emu/machine/e05a03.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/e05a30.h,MACHINES += E05A30
---------------------------------------------------

if (MACHINES["E05A30"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/e05a30.c",
		MAME_DIR .. "src/emu/machine/e05a30.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/eeprom.h,MACHINES += EEPROMDEV
--@src/emu/machine/eepromser.h,MACHINES += EEPROMDEV
--@src/emu/machine/eeprompar.h,MACHINES += EEPROMDEV
---------------------------------------------------

if (MACHINES["EEPROMDEV"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/eeprom.c",
		MAME_DIR .. "src/emu/machine/eeprom.h",
		MAME_DIR .. "src/emu/machine/eepromser.c",
		MAME_DIR .. "src/emu/machine/eepromser.h",
		MAME_DIR .. "src/emu/machine/eeprompar.c",
		MAME_DIR .. "src/emu/machine/eeprompar.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/er2055.h,MACHINES += ER2055
---------------------------------------------------

if (MACHINES["ER2055"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/er2055.c",
		MAME_DIR .. "src/emu/machine/er2055.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/f3853.h,MACHINES += F3853
---------------------------------------------------

if (MACHINES["F3853"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/f3853.c",
		MAME_DIR .. "src/emu/machine/f3853.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/hd63450.h,MACHINES += HD63450
---------------------------------------------------

if (MACHINES["HD63450"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/hd63450.c",
		MAME_DIR .. "src/emu/machine/hd63450.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/hd64610.h,MACHINES += HD64610
---------------------------------------------------

if (MACHINES["HD64610"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/hd64610.c",
		MAME_DIR .. "src/emu/machine/hd64610.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i2cmem.h,MACHINES += I2CMEM
---------------------------------------------------

if (MACHINES["I2CMEM"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i2cmem.c",
		MAME_DIR .. "src/emu/machine/i2cmem.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8155.h,MACHINES += I8155
---------------------------------------------------

if (MACHINES["I8155"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8155.c",
		MAME_DIR .. "src/emu/machine/i8155.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8212.h,MACHINES += I8212
---------------------------------------------------

if (MACHINES["I8212"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8212.c",
		MAME_DIR .. "src/emu/machine/i8212.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8214.h,MACHINES += I8214
---------------------------------------------------

if (MACHINES["I8214"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8214.c",
		MAME_DIR .. "src/emu/machine/i8214.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8243.h,MACHINES += I8243
---------------------------------------------------

if (MACHINES["I8243"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8243.c",
		MAME_DIR .. "src/emu/machine/i8243.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8251.h,MACHINES += I8251
---------------------------------------------------

if (MACHINES["I8251"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8251.c",
		MAME_DIR .. "src/emu/machine/i8251.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8257.h,MACHINES += I8257
---------------------------------------------------

if (MACHINES["I8257"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8257.c",
		MAME_DIR .. "src/emu/machine/i8257.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/i8271.h,MACHINES += I8271
---------------------------------------------------

if (MACHINES["I8271"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8271.c",
		MAME_DIR .. "src/emu/machine/i8271.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8279.h,MACHINES += I8279
---------------------------------------------------

if (MACHINES["I8279"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8279.c",
		MAME_DIR .. "src/emu/machine/i8279.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8355.h,MACHINES += I8355
---------------------------------------------------

if (MACHINES["I8355"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8355.c",
		MAME_DIR .. "src/emu/machine/i8355.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i80130.h,MACHINES += I80130
---------------------------------------------------

if (MACHINES["I80130"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i80130.c",
		MAME_DIR .. "src/emu/machine/i80130.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/atadev.h,MACHINES += IDE
--@src/emu/machine/ataintf.h,MACHINES += IDE
---------------------------------------------------

if (MACHINES["IDE"]~=null) then
	MACHINES["T10"] = true
	files {
		MAME_DIR .. "src/emu/machine/atadev.c",
		MAME_DIR .. "src/emu/machine/atadev.h",
		MAME_DIR .. "src/emu/machine/atahle.c",
		MAME_DIR .. "src/emu/machine/atahle.h",
		MAME_DIR .. "src/emu/machine/ataintf.c",
		MAME_DIR .. "src/emu/machine/ataintf.h",
		MAME_DIR .. "src/emu/machine/atapicdr.c",
		MAME_DIR .. "src/emu/machine/atapicdr.h",
		MAME_DIR .. "src/emu/machine/atapihle.c",
		MAME_DIR .. "src/emu/machine/atapihle.h",
		MAME_DIR .. "src/emu/machine/idectrl.c",
		MAME_DIR .. "src/emu/machine/idectrl.h",
		MAME_DIR .. "src/emu/machine/idehd.c",
		MAME_DIR .. "src/emu/machine/idehd.h",
		MAME_DIR .. "src/emu/machine/vt83c461.c",
		MAME_DIR .. "src/emu/machine/vt83c461.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/im6402.h,MACHINES += IM6402
---------------------------------------------------

if (MACHINES["IM6402"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/im6402.c",
		MAME_DIR .. "src/emu/machine/im6402.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ins8154.h,MACHINES += INS8154
---------------------------------------------------

if (MACHINES["INS8154"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ins8154.c",
		MAME_DIR .. "src/emu/machine/ins8154.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ins8250.h,MACHINES += INS8250
---------------------------------------------------

if (MACHINES["INS8250"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ins8250.c",
		MAME_DIR .. "src/emu/machine/ins8250.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/intelfsh.h,MACHINES += INTELFLASH
---------------------------------------------------

if (MACHINES["INTELFLASH"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/intelfsh.c",
		MAME_DIR .. "src/emu/machine/intelfsh.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/jvsdev.h,MACHINES += JVS
--@src/emu/machine/jvshost.h,MACHINES += JVS
---------------------------------------------------

if (MACHINES["JVS"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/jvsdev.c",
		MAME_DIR .. "src/emu/machine/jvsdev.h",
		MAME_DIR .. "src/emu/machine/jvshost.c",
		MAME_DIR .. "src/emu/machine/jvshost.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/k033906.h,MACHINES += K033906
---------------------------------------------------

if (MACHINES["K033906"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/k033906.c",
		MAME_DIR .. "src/emu/machine/k033906.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/k053252.h,MACHINES += K053252
---------------------------------------------------

if (MACHINES["K053252"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/k053252.c",
		MAME_DIR .. "src/emu/machine/k053252.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/k056230.h,MACHINES += K056230
---------------------------------------------------

if (MACHINES["K056230"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/k056230.c",
		MAME_DIR .. "src/emu/machine/k056230.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/kb3600.h,MACHINES += KB3600
---------------------------------------------------

if (MACHINES["KB3600"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/kb3600.c",
		MAME_DIR .. "src/emu/machine/kb3600.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/kr2376.h,MACHINES += KR2376
---------------------------------------------------

if (MACHINES["KR2376"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/kr2376.c",
		MAME_DIR .. "src/emu/machine/kr2376.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/latch8.h,MACHINES += LATCH8
---------------------------------------------------

if (MACHINES["LATCH8"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/latch8.c",
		MAME_DIR .. "src/emu/machine/latch8.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/lc89510.h,MACHINES += LC89510
---------------------------------------------------

if (MACHINES["LC89510"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/lc89510.c",
		MAME_DIR .. "src/emu/machine/lc89510.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ldpr8210.h,MACHINES += LDPR8210
---------------------------------------------------

if (MACHINES["LDPR8210"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ldpr8210.c",
		MAME_DIR .. "src/emu/machine/ldpr8210.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ldstub.h,MACHINES += LDSTUB
---------------------------------------------------

if (MACHINES["LDSTUB"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ldstub.c",
		MAME_DIR .. "src/emu/machine/ldstub.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ldv1000.h,MACHINES += LDV1000
---------------------------------------------------

if (MACHINES["LDV1000"]~=null) then
	MACHINES["Z80CTC"] = true
	MACHINES["I8255"] = true
	files {
		MAME_DIR .. "src/emu/machine/ldv1000.c",
		MAME_DIR .. "src/emu/machine/ldv1000.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ldvp931.h,MACHINES += LDVP931
---------------------------------------------------

if (MACHINES["LDVP931"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ldvp931.c",
		MAME_DIR .. "src/emu/machine/ldvp931.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/lh5810.h,MACHINES += LH5810
---------------------------------------------------

if (MACHINES["LH5810"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/lh5810.c",
		MAME_DIR .. "src/emu/machine/lh5810.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/linflash.h,MACHINES += LINFLASH
---------------------------------------------------

if (MACHINES["LINFLASH"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/linflash.c",
		MAME_DIR .. "src/emu/machine/linflash.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/m6m80011ap.h,MACHINES += M6M80011AP
---------------------------------------------------

if (MACHINES["M6M80011AP"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/m6m80011ap.c",
		MAME_DIR .. "src/emu/machine/m6m80011ap.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/matsucd.h,MACHINES += MATSUCD
---------------------------------------------------

if (MACHINES["MATSUCD"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/matsucd.c",
		MAME_DIR .. "src/emu/machine/matsucd.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mb14241.h,MACHINES += MB14241
---------------------------------------------------

if (MACHINES["MB14241"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mb14241.c",
		MAME_DIR .. "src/emu/machine/mb14241.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mb3773.h,MACHINES += MB3773
---------------------------------------------------

if (MACHINES["MB3773"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mb3773.c",
		MAME_DIR .. "src/emu/machine/mb3773.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mb8421.h,MACHINES += MB8421
---------------------------------------------------

if (MACHINES["MB8421"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mb8421.c",
		MAME_DIR .. "src/emu/machine/mb8421.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mb87078.h,MACHINES += MB87078
---------------------------------------------------

if (MACHINES["MB87078"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mb87078.c",
		MAME_DIR .. "src/emu/machine/mb87078.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mb8795.h,MACHINES += MB8795
---------------------------------------------------

if (MACHINES["MB8795"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mb8795.c",
		MAME_DIR .. "src/emu/machine/mb8795.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mb89352.h,MACHINES += MB89352
---------------------------------------------------

if (MACHINES["MB89352"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mb89352.c",
		MAME_DIR .. "src/emu/machine/mb89352.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mb89371.h,MACHINES += MB89371
---------------------------------------------------

if (MACHINES["MB89371"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mb89371.c",
		MAME_DIR .. "src/emu/machine/mb89371.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc146818.h,MACHINES += MC146818
---------------------------------------------------

if (MACHINES["MC146818"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc146818.c",
		MAME_DIR .. "src/emu/machine/mc146818.h",
		MAME_DIR .. "src/emu/machine/ds128x.c",
		MAME_DIR .. "src/emu/machine/ds128x.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc2661.h,MACHINES += MC2661
---------------------------------------------------

if (MACHINES["MC2661"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc2661.c",
		MAME_DIR .. "src/emu/machine/mc2661.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc6843.h,MACHINES += MC6843
---------------------------------------------------

if (MACHINES["MC6843"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc6843.c",
		MAME_DIR .. "src/emu/machine/mc6843.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc6846.h,MACHINES += MC6846
---------------------------------------------------

if (MACHINES["MC6846"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc6846.c",
		MAME_DIR .. "src/emu/machine/mc6846.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc6852.h,MACHINES += MC6852
---------------------------------------------------

if (MACHINES["MC6852"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc6852.c",
		MAME_DIR .. "src/emu/machine/mc6852.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc6854.h,MACHINES += MC6854
---------------------------------------------------

if (MACHINES["MC6854"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc6854.c",
		MAME_DIR .. "src/emu/machine/mc6854.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc68328.h,MACHINES += MC68328
---------------------------------------------------

if (MACHINES["MC68328"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc68328.c",
		MAME_DIR .. "src/emu/machine/mc68328.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mc68901.h,MACHINES += MC68901
---------------------------------------------------

if (MACHINES["MC68901"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mc68901.c",
		MAME_DIR .. "src/emu/machine/mc68901.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mccs1850.h,MACHINES += MCCS1850
---------------------------------------------------

if (MACHINES["MCCS1850"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mccs1850.c",
		MAME_DIR .. "src/emu/machine/mccs1850.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/68307.h,MACHINES += M68307
---------------------------------------------------

if (MACHINES["M68307"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/68307.c",
		MAME_DIR .. "src/emu/machine/68307.h",
		MAME_DIR .. "src/emu/machine/68307sim.c",
		MAME_DIR .. "src/emu/machine/68307sim.h",
		MAME_DIR .. "src/emu/machine/68307bus.c",
		MAME_DIR .. "src/emu/machine/68307bus.h",
		MAME_DIR .. "src/emu/machine/68307tmu.c",
		MAME_DIR .. "src/emu/machine/68307tmu.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/68340.h,MACHINES += M68340
---------------------------------------------------

if (MACHINES["M68340"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/68340.c",
		MAME_DIR .. "src/emu/machine/68340.h",
		MAME_DIR .. "src/emu/machine/68340sim.c",
		MAME_DIR .. "src/emu/machine/68340sim.h",
		MAME_DIR .. "src/emu/machine/68340dma.c",
		MAME_DIR .. "src/emu/machine/68340dma.h",
		MAME_DIR .. "src/emu/machine/68340ser.c",
		MAME_DIR .. "src/emu/machine/68340ser.h",
		MAME_DIR .. "src/emu/machine/68340tmu.c",
		MAME_DIR .. "src/emu/machine/68340tmu.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mcf5206e.h,MACHINES += MCF5206E
---------------------------------------------------

if (MACHINES["MCF5206E"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mcf5206e.c",
		MAME_DIR .. "src/emu/machine/mcf5206e.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/microtch.h,MACHINES += MICROTOUCH
---------------------------------------------------

if (MACHINES["MICROTOUCH"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/microtch.c",
		MAME_DIR .. "src/emu/machine/microtch.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mm58274c.h,MACHINES += MM58274C
---------------------------------------------------

if (MACHINES["MM58274C"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mm58274c.c",
		MAME_DIR .. "src/emu/machine/mm58274c.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mm74c922.h,MACHINES += MM74C922
---------------------------------------------------

if (MACHINES["MM74C922"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mm74c922.c",
		MAME_DIR .. "src/emu/machine/mm74c922.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mos6526.h,MACHINES += MOS6526
---------------------------------------------------

if (MACHINES["MOS6526"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mos6526.c",
		MAME_DIR .. "src/emu/machine/mos6526.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mos6529.h,MACHINES += MOS6529
---------------------------------------------------

if (MACHINES["MOS6529"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mos6529.c",
		MAME_DIR .. "src/emu/machine/mos6529.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mos6702.h,MACHINES += MOS6702
---------------------------------------------------

if (MACHINES["MOS6702"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mos6702.c",
		MAME_DIR .. "src/emu/machine/mos6702.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mos8706.h,MACHINES += MOS8706
---------------------------------------------------

if (MACHINES["MOS8706"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mos8706.c",
		MAME_DIR .. "src/emu/machine/mos8706.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mos8722.h,MACHINES += MOS8722
---------------------------------------------------

if (MACHINES["MOS8722"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mos8722.c",
		MAME_DIR .. "src/emu/machine/mos8722.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mos8726.h,MACHINES += MOS8726
---------------------------------------------------

if (MACHINES["MOS8726"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mos8726.c",
		MAME_DIR .. "src/emu/machine/mos8726.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mos6530.h,MACHINES += MIOT6530
---------------------------------------------------

if (MACHINES["MIOT6530"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mos6530.c",
		MAME_DIR .. "src/emu/machine/mos6530.h",
		MAME_DIR .. "src/emu/machine/mos6530n.c",
		MAME_DIR .. "src/emu/machine/mos6530n.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mos6551.h,MACHINES += MOS6551
---------------------------------------------------

if (MACHINES["MOS6551"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mos6551.c",
		MAME_DIR .. "src/emu/machine/mos6551.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/msm5832.h,MACHINES += MSM5832
---------------------------------------------------

if (MACHINES["MSM5832"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/msm5832.c",
		MAME_DIR .. "src/emu/machine/msm5832.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/msm58321.h,MACHINES += MSM58321
---------------------------------------------------

if (MACHINES["MSM58321"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/msm58321.c",
		MAME_DIR .. "src/emu/machine/msm58321.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/msm6242.h,MACHINES += MSM6242
---------------------------------------------------

if (MACHINES["MSM6242"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/msm6242.c",
		MAME_DIR .. "src/emu/machine/msm6242.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ncr539x.h,MACHINES += NCR539x
---------------------------------------------------

if (MACHINES["NCR539x"]~=null) then
	MACHINES["SCSI"] = true
	files {
		MAME_DIR .. "src/emu/machine/ncr539x.c",
		MAME_DIR .. "src/emu/machine/ncr539x.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/nmc9306.h,MACHINES += NMC9306
---------------------------------------------------

if (MACHINES["NMC9306"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/nmc9306.c",
		MAME_DIR .. "src/emu/machine/nmc9306.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/nscsi_bus.h,MACHINES += NSCSI
--@src/emu/machine/nscsi_cb.h,MACHINES += NSCSI
--@src/emu/machine/nscsi_cd.h,MACHINES += NSCSI
--@src/emu/machine/nscsi_hd.h,MACHINES += NSCSI
--@src/emu/machine/nscsi_s1410.h,MACHINES += NSCSI
---------------------------------------------------

if (MACHINES["NSCSI"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/nscsi_bus.c",
		MAME_DIR .. "src/emu/machine/nscsi_bus.h",
		MAME_DIR .. "src/emu/machine/nscsi_cb.c",
		MAME_DIR .. "src/emu/machine/nscsi_cb.h",
		MAME_DIR .. "src/emu/machine/nscsi_cd.c",
		MAME_DIR .. "src/emu/machine/nscsi_cd.h",
		MAME_DIR .. "src/emu/machine/nscsi_hd.c",
		MAME_DIR .. "src/emu/machine/nscsi_hd.h",
		MAME_DIR .. "src/emu/machine/nscsi_s1410.c",
		MAME_DIR .. "src/emu/machine/nscsi_s1410.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pcf8593.h,MACHINES += PCF8593
---------------------------------------------------

if (MACHINES["PCF8593"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pcf8593.c",
		MAME_DIR .. "src/emu/machine/pcf8593.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/lpci.h,MACHINES += LPCI
---------------------------------------------------

if (MACHINES["LPCI"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/lpci.c",
		MAME_DIR .. "src/emu/machine/lpci.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pci.h,MACHINES += PCI
---------------------------------------------------

if (MACHINES["PCI"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pci.c",
		MAME_DIR .. "src/emu/machine/pci.h",
		MAME_DIR .. "src/emu/machine/pci-usb.c",
		MAME_DIR .. "src/emu/machine/pci-usb.h",
		MAME_DIR .. "src/emu/machine/pci-sata.c",
		MAME_DIR .. "src/emu/machine/pci-sata.h",
		MAME_DIR .. "src/emu/machine/pci-apic.c",
		MAME_DIR .. "src/emu/machine/pci-apic.h",
		MAME_DIR .. "src/emu/machine/pci-smbus.c",
		MAME_DIR .. "src/emu/machine/pci-smbus.h",
		MAME_DIR .. "src/emu/machine/i82541.c",
		MAME_DIR .. "src/emu/machine/i82541.h",
		MAME_DIR .. "src/emu/machine/i82875p.c",
		MAME_DIR .. "src/emu/machine/i82875p.h",
		MAME_DIR .. "src/emu/machine/i6300esb.c",
		MAME_DIR .. "src/emu/machine/i6300esb.h",
		MAME_DIR .. "src/emu/machine/lpc.c",
		MAME_DIR .. "src/emu/machine/lpc.h",
		MAME_DIR .. "src/emu/machine/lpc-acpi.c",
		MAME_DIR .. "src/emu/machine/lpc-acpi.h",
		MAME_DIR .. "src/emu/machine/lpc-rtc.c",
		MAME_DIR .. "src/emu/machine/lpc-rtc.h",
		MAME_DIR .. "src/emu/machine/lpc-pit.c",
		MAME_DIR .. "src/emu/machine/lpc-pit.h",
		MAME_DIR .. "src/emu/machine/vrc4373.c",
		MAME_DIR .. "src/emu/machine/vrc4373.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pckeybrd.h,MACHINES += PCKEYBRD
---------------------------------------------------

if (MACHINES["PCKEYBRD"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pckeybrd.c",
		MAME_DIR .. "src/emu/machine/pckeybrd.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pic8259.h,MACHINES += PIC8259
---------------------------------------------------

if (MACHINES["PIC8259"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pic8259.c",
		MAME_DIR .. "src/emu/machine/pic8259.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pit8253.h,MACHINES += PIT8253
---------------------------------------------------

if (MACHINES["PIT8253"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pit8253.c",
		MAME_DIR .. "src/emu/machine/pit8253.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pla.h,MACHINES += PLA
---------------------------------------------------

if (MACHINES["PLA"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pla.c",
		MAME_DIR .. "src/emu/machine/pla.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/r10696.h,MACHINES += R10696
---------------------------------------------------

if (MACHINES["R10696"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/r10696.c",
		MAME_DIR .. "src/emu/machine/r10696.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/r10788.h,MACHINES += R10788
---------------------------------------------------

if (MACHINES["R10788"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/r10788.c",
		MAME_DIR .. "src/emu/machine/r10788.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ra17xx.h,MACHINES += RA17XX
---------------------------------------------------

if (MACHINES["RA17XX"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ra17xx.c",
		MAME_DIR .. "src/emu/machine/ra17xx.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/rf5c296.h,MACHINES += RF5C296
---------------------------------------------------

if (MACHINES["RF5C296"]~=null) then
	MACHINES["PCCARD"] = true
	files {	
		MAME_DIR .. "src/emu/machine/rf5c296.c",
		MAME_DIR .. "src/emu/machine/rf5c296.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/roc10937.h,MACHINES += ROC10937
---------------------------------------------------

if (MACHINES["ROC10937"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/roc10937.c",
		MAME_DIR .. "src/emu/machine/roc10937.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/rp5c01.h,MACHINES += RP5C01
---------------------------------------------------

if (MACHINES["RP5C01"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/rp5c01.c",
		MAME_DIR .. "src/emu/machine/rp5c01.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/rp5c15.h,MACHINES += RP5C15
---------------------------------------------------

if (MACHINES["RP5C15"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/rp5c15.c",
		MAME_DIR .. "src/emu/machine/rp5c15.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/rp5h01.h,MACHINES += RP5H01
---------------------------------------------------

if (MACHINES["RP5H01"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/rp5h01.c",
		MAME_DIR .. "src/emu/machine/rp5h01.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/64h156.h,MACHINES += R64H156
---------------------------------------------------

if (MACHINES["R64H156"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/64h156.c",
		MAME_DIR .. "src/emu/machine/64h156.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/rtc4543.h,MACHINES += RTC4543
---------------------------------------------------

if (MACHINES["RTC4543"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/rtc4543.c",
		MAME_DIR .. "src/emu/machine/rtc4543.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/rtc65271.h,MACHINES += RTC65271
---------------------------------------------------

if (MACHINES["RTC65271"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/rtc65271.c",
		MAME_DIR .. "src/emu/machine/rtc65271.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/rtc9701.h,MACHINES += RTC9701
---------------------------------------------------

if (MACHINES["RTC9701"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/rtc9701.c",
		MAME_DIR .. "src/emu/machine/rtc9701.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/s2636.h,MACHINES += S2636
---------------------------------------------------

if (MACHINES["S2636"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/s2636.c",
		MAME_DIR .. "src/emu/machine/s2636.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/s3520cf.h,MACHINES += S3520CF
---------------------------------------------------

if (MACHINES["S3520CF"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/s3520cf.c",
		MAME_DIR .. "src/emu/machine/s3520cf.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/s3c2400.h,MACHINES += S3C2400
---------------------------------------------------

if (MACHINES["S3C2400"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/s3c2400.c",
		MAME_DIR .. "src/emu/machine/s3c2400.h",
		MAME_DIR .. "src/emu/machine/s3c24xx.inc",
	}
end

---------------------------------------------------
--
--@src/emu/machine/s3c2410.h,MACHINES += S3C2410
---------------------------------------------------

if (MACHINES["S3C2410"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/s3c2410.c",
		MAME_DIR .. "src/emu/machine/s3c2410.h",
		MAME_DIR .. "src/emu/machine/s3c24xx.inc",
	}
end

---------------------------------------------------
--
--@src/emu/machine/s3c44b0.h,MACHINES += S3C44B0
---------------------------------------------------

if (MACHINES["S3C44B0"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/s3c44b0.c",
		MAME_DIR .. "src/emu/machine/s3c44b0.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/s3c2440.h,MACHINES += S3C2440
---------------------------------------------------

if (MACHINES["S3C2440"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/s3c2440.c",
		MAME_DIR .. "src/emu/machine/s3c2440.h",
		MAME_DIR .. "src/emu/machine/s3c24xx.inc",
	}
end

---------------------------------------------------
--
--@src/emu/machine/saturn.h,MACHINES += SATURN
---------------------------------------------------

if (MACHINES["SATURN"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/saturn.c",
	}
end

---------------------------------------------------
--
--@src/emu/machine/serflash.h,MACHINES += SERFLASH
---------------------------------------------------

if (MACHINES["SERFLASH"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/serflash.c",
		MAME_DIR .. "src/emu/machine/serflash.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/smc91c9x.h,MACHINES += SMC91C9X
---------------------------------------------------

if (MACHINES["SMC91C9X"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/smc91c9x.c",
		MAME_DIR .. "src/emu/machine/smc91c9x.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/smpc.h,MACHINES += SMPC
---------------------------------------------------

if (MACHINES["SMPC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/smpc.c",
		MAME_DIR .. "src/emu/machine/smpc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/stvcd.h,MACHINES += STVCD
---------------------------------------------------

if (MACHINES["STVCD"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/stvcd.c",
	}
end

---------------------------------------------------
--
--
---------------------------------------------------

if (BUSES["SCSI"]~=null) then
	MACHINES["T10"] = true
end

if (MACHINES["T10"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/t10mmc.c",
		MAME_DIR .. "src/emu/machine/t10mmc.h",
		MAME_DIR .. "src/emu/machine/t10sbc.c",
		MAME_DIR .. "src/emu/machine/t10sbc.h",
		MAME_DIR .. "src/emu/machine/t10spc.c",
		MAME_DIR .. "src/emu/machine/t10spc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/tc009xlvc.h,MACHINES += TC0091LVC
---------------------------------------------------

if (MACHINES["TC0091LVC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/tc009xlvc.c",
		MAME_DIR .. "src/emu/machine/tc009xlvc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/timekpr.h,MACHINES += TIMEKPR
---------------------------------------------------

if (MACHINES["TIMEKPR"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/timekpr.c",
		MAME_DIR .. "src/emu/machine/timekpr.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/tmp68301.h,MACHINES += TMP68301
---------------------------------------------------

if (MACHINES["TMP68301"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/tmp68301.c",
		MAME_DIR .. "src/emu/machine/tmp68301.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/tms1024.h,MACHINES += TMS1024
---------------------------------------------------

if (MACHINES["TMS1024"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/tms1024.c",
		MAME_DIR .. "src/emu/machine/tms1024.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/tms5501.h,MACHINES += TMS5501
---------------------------------------------------

if (MACHINES["TMS5501"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/tms5501.c",
		MAME_DIR .. "src/emu/machine/tms5501.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/tms6100.h,MACHINES += TMS6100
---------------------------------------------------

if (MACHINES["TMS6100"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/tms6100.c",
		MAME_DIR .. "src/emu/machine/tms6100.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/tms9901.h,MACHINES += TMS9901
---------------------------------------------------

if (MACHINES["TMS9901"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/tms9901.c",
		MAME_DIR .. "src/emu/machine/tms9901.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/tms9902.h,MACHINES += TMS9902
---------------------------------------------------

if (MACHINES["TMS9902"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/tms9902.c",
		MAME_DIR .. "src/emu/machine/tms9902.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/upd1990a.h,MACHINES += UPD1990A
---------------------------------------------------

if (MACHINES["UPD1990A"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/upd1990a.c",
		MAME_DIR .. "src/emu/machine/upd1990a.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/upd4992.h,MACHINES += UPD4992
---------------------------------------------------

if (MACHINES["UPD4992"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/upd4992.c",
		MAME_DIR .. "src/emu/machine/upd4992.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/upd4701.h,MACHINES += UPD4701
---------------------------------------------------

if (MACHINES["UPD4701"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/upd4701.c",
		MAME_DIR .. "src/emu/machine/upd4701.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/upd7002.h,MACHINES += UPD7002
---------------------------------------------------

if (MACHINES["UPD7002"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/upd7002.c",
		MAME_DIR .. "src/emu/machine/upd7002.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/upd71071.h,MACHINES += UPD71071
---------------------------------------------------

if (MACHINES["UPD71071"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/upd71071.c",
		MAME_DIR .. "src/emu/machine/upd71071.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/upd765.h,MACHINES += UPD765
---------------------------------------------------

if (MACHINES["UPD765"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/upd765.c",
		MAME_DIR .. "src/emu/machine/upd765.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/v3021.h,MACHINES += V3021
---------------------------------------------------

if (MACHINES["V3021"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/v3021.c",
		MAME_DIR .. "src/emu/machine/v3021.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/wd_fdc.h,MACHINES += WD_FDC
---------------------------------------------------

if (MACHINES["WD_FDC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/wd_fdc.c",
		MAME_DIR .. "src/emu/machine/wd_fdc.h",
		MAME_DIR .. "src/emu/machine/fdc_pll.c",
		MAME_DIR .. "src/emu/machine/fdc_pll.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/wd11c00_17.h,MACHINES += WD11C00_17
---------------------------------------------------

if (MACHINES["WD11C00_17"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/wd11c00_17.c",
		MAME_DIR .. "src/emu/machine/wd11c00_17.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/wd17xx.h,MACHINES += WD17XX
---------------------------------------------------

if (MACHINES["WD17XX"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/wd17xx.c",
		MAME_DIR .. "src/emu/machine/wd17xx.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/wd2010.h,MACHINES += WD2010
---------------------------------------------------

if (MACHINES["WD2010"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/wd2010.c",
		MAME_DIR .. "src/emu/machine/wd2010.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/wd33c93.h,MACHINES += WD33C93
---------------------------------------------------

if (MACHINES["WD33C93"]~=null) then
	MACHINES["SCSI"] = true
	files {
		MAME_DIR .. "src/emu/machine/wd33c93.c",
		MAME_DIR .. "src/emu/machine/wd33c93.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/wd7600.h,MACHINES += WD7600
---------------------------------------------------

if (MACHINES["WD7600"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/wd7600.c",
		MAME_DIR .. "src/emu/machine/wd7600.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/x2212.h,MACHINES += X2212
---------------------------------------------------

if (MACHINES["X2212"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/x2212.c",
		MAME_DIR .. "src/emu/machine/x2212.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/x76f041.h,MACHINES += X76F041
---------------------------------------------------

if (MACHINES["X76F041"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/x76f041.c",
		MAME_DIR .. "src/emu/machine/x76f041.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/x76f100.h,MACHINES += X76F100
---------------------------------------------------

if (MACHINES["X76F100"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/x76f100.c",
		MAME_DIR .. "src/emu/machine/x76f100.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ym2148.h,MACHINES += YM2148
---------------------------------------------------

if (MACHINES["YM2148"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ym2148.c",
		MAME_DIR .. "src/emu/machine/ym2148.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/z80ctc.h,MACHINES += Z80CTC
---------------------------------------------------

if (MACHINES["Z80CTC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/z80ctc.c",
		MAME_DIR .. "src/emu/machine/z80ctc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/z80dart.h,MACHINES += Z80DART
---------------------------------------------------

if (MACHINES["Z80DART"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/z80dart.c",
		MAME_DIR .. "src/emu/machine/z80dart.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/z80dma.h,MACHINES += Z80DMA
---------------------------------------------------

if (MACHINES["Z80DMA"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/z80dma.c",
		MAME_DIR .. "src/emu/machine/z80dma.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/z80pio.h,MACHINES += Z80PIO
---------------------------------------------------

if (MACHINES["Z80PIO"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/z80pio.c",
		MAME_DIR .. "src/emu/machine/z80pio.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/z80sti.h,MACHINES += Z80STI
---------------------------------------------------

if (MACHINES["Z80STI"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/z80sti.c",
		MAME_DIR .. "src/emu/machine/z80sti.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/z8536.h,MACHINES += Z8536
---------------------------------------------------

if (MACHINES["Z8536"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/z8536.c",
		MAME_DIR .. "src/emu/machine/z8536.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pccard.h,MACHINES += PCCARD
---------------------------------------------------

if (MACHINES["PCCARD"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pccard.c",
		MAME_DIR .. "src/emu/machine/pccard.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/i8255.h,MACHINES += I8255
---------------------------------------------------

if (MACHINES["I8255"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/i8255.c",
		MAME_DIR .. "src/emu/machine/i8255.h",
		MAME_DIR .. "src/emu/machine/mb89363b.c",
		MAME_DIR .. "src/emu/machine/mb89363b.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ncr5380.h,MACHINES += NCR5380
---------------------------------------------------

if (MACHINES["NCR5380"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ncr5380.c",
		MAME_DIR .. "src/emu/machine/ncr5380.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ncr5380n.h,MACHINES += NCR5380N
---------------------------------------------------

if (MACHINES["NCR5380N"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ncr5380n.c",
		MAME_DIR .. "src/emu/machine/ncr5380n.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ncr5389.h,MACHINES += NCR5390
---------------------------------------------------

if (MACHINES["NCR5390"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ncr5390.c",
		MAME_DIR .. "src/emu/machine/ncr5390.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mm58167.h,MACHINES += MM58167
---------------------------------------------------

if (MACHINES["MM58167"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mm58167.c",
		MAME_DIR .. "src/emu/machine/mm58167.h",
	}
end


---------------------------------------------------
--
--@src/emu/machine/dp8390.h,MACHINES += DP8390
---------------------------------------------------

if (MACHINES["DP8390"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/dp8390.c",
		MAME_DIR .. "src/emu/machine/dp8390.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pc_lpt.h,MACHINES += PC_LPT
---------------------------------------------------

if (MACHINES["PC_LPT"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pc_lpt.c",
		MAME_DIR .. "src/emu/machine/pc_lpt.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pc_fdc.h,MACHINES += PC_FDC
---------------------------------------------------

if (MACHINES["PC_FDC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pc_fdc.c",
		MAME_DIR .. "src/emu/machine/pc_fdc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/mpu401.h,MACHINES += MPU401
---------------------------------------------------

if (MACHINES["MPU401"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/mpu401.c",
		MAME_DIR .. "src/emu/machine/mpu401.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/at_keybc.h,MACHINES += AT_KEYBC
---------------------------------------------------

if (MACHINES["AT_KEYBC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/at_keybc.c",
		MAME_DIR .. "src/emu/machine/at_keybc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/smc92x4.h,MACHINES += SMC92X4
---------------------------------------------------

if (MACHINES["SMC92X4"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/smc92x4.c",
		MAME_DIR .. "src/emu/machine/smc92x4.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/hdc9234.h,MACHINES += HDC9234
---------------------------------------------------

if (MACHINES["HDC9234"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/hdc9234.c",
		MAME_DIR .. "src/emu/machine/hdc9234.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/ti99_hd.h,MACHINES += TI99_HD
---------------------------------------------------

if (MACHINES["TI99_HD"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/ti99_hd.c",
		MAME_DIR .. "src/emu/machine/ti99_hd.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/strata.h,MACHINES += STRATA
---------------------------------------------------

if (MACHINES["STRATA"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/strata.c",
		MAME_DIR .. "src/emu/machine/strata.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/steppers.h,MACHINES += STEPPERS
---------------------------------------------------

if (MACHINES["STEPPERS"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/steppers.c",
		MAME_DIR .. "src/emu/machine/steppers.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/corvushd.h,MACHINES += CORVUSHD
---------------------------------------------------
if (MACHINES["CORVUSHD"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/corvushd.c",
		MAME_DIR .. "src/emu/machine/corvushd.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/wozfdc.h,MACHINES += WOZFDC
---------------------------------------------------
if (MACHINES["WOZFDC"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/wozfdc.c",
		MAME_DIR .. "src/emu/machine/wozfdc.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/diablo_hd.h,MACHINES += DIABLO_HD
---------------------------------------------------
if (MACHINES["DIABLO_HD"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/diablo_hd.c",
		MAME_DIR .. "src/emu/machine/diablo_hd.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/fdc37c665gt.h,MACHINES += FDC37C665GT
---------------------------------------------------

if (MACHINES["FDC37C665GT"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/fdc37c665gt.c",
		MAME_DIR .. "src/emu/machine/fdc37c665gt.h",
	}
end

---------------------------------------------------
--
--@src/emu/machine/pci9050.h,MACHINES += PCI9050
---------------------------------------------------

if (MACHINES["PCI9050"]~=null) then
	files {
		MAME_DIR .. "src/emu/machine/pci9050.c",
		MAME_DIR .. "src/emu/machine/pci9050.h",
	}
end

