// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_9310.h
 *
 *  DM9310: Synchronous 4-Bit Counters
 *
 *  FIXME: This should be merged with the 9316. The only difference is MAXCNT!
 *
 *          +--------------+
 *    CLEAR |1     ++    16| VCC
 *    CLOCK |2           15| RC (Ripple Carry)
 *        A |3           14| QA
 *        B |4    9310   13| QB
 *        C |5           12| QC
 *        D |6           11| QD
 * Enable P |7           10| Enable T
 *      GND |8            9| LOAD
 *          +--------------+
 *
 *          Counter Sequence
 *
 *          +-------++----+----+----+----+----+
 *          | COUNT || QD | QC | QB | QA | RC |
 *          +=======++====+====+====+====+====+
 *          |    0  ||  0 |  0 |  0 |  0 |  0 |
 *          |    1  ||  0 |  0 |  0 |  1 |  0 |
 *          |    2  ||  0 |  0 |  1 |  0 |  0 |
 *          |    3  ||  0 |  0 |  1 |  1 |  0 |
 *          |    4  ||  0 |  1 |  0 |  0 |  0 |
 *          |    5  ||  0 |  1 |  0 |  1 |  0 |
 *          |    6  ||  0 |  1 |  1 |  0 |  0 |
 *          |    7  ||  0 |  1 |  1 |  1 |  0 |
 *          |    8  ||  1 |  0 |  0 |  0 |  0 |
 *          |    9  ||  1 |  0 |  0 |  1 |  0 |
 *          +-------++----+----+----+----+----+
 *
 *          Reset count function: Please refer to
 *          National Semiconductor datasheet (timing diagramm)
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */

#ifndef NLD_9310_H_
#define NLD_9310_H_

#include "nl_base.h"

#define TTL_9310(_name, _CLK, _ENP, _ENT, _CLRQ, _LOADQ, _A, _B, _C, _D)            \
		NET_REGISTER_DEV(TTL_9310, _name)                                               \
		NET_CONNECT(_name, CLK, _CLK)                                               \
		NET_CONNECT(_name, ENP,  _ENP)                                              \
		NET_CONNECT(_name, ENT,  _ENT)                                              \
		NET_CONNECT(_name, CLRQ, _CLRQ)                                             \
		NET_CONNECT(_name, LOADQ,_LOADQ)                                            \
		NET_CONNECT(_name, A,    _A)                                                \
		NET_CONNECT(_name, B,    _B)                                                \
		NET_CONNECT(_name, C,    _C)                                                \
		NET_CONNECT(_name, D,    _D)

#define TTL_9310_DIP(_name)                                                         \
		NET_REGISTER_DEV(TTL_9310_DIP, _name)

NETLIB_NAMESPACE_DEVICES_START()

NETLIB_SUBDEVICE(9310_subABCD,
	logic_input_t m_A;
	logic_input_t m_B;
	logic_input_t m_C;
	logic_input_t m_D;

	ATTR_HOT inline UINT8 read_ABCD() const
	{
		//return (INPLOGIC_PASSIVE(m_D) << 3) | (INPLOGIC_PASSIVE(m_C) << 2) | (INPLOGIC_PASSIVE(m_B) << 1) | (INPLOGIC_PASSIVE(m_A) << 0);
		return (INPLOGIC(m_D) << 3) | (INPLOGIC(m_C) << 2) | (INPLOGIC(m_B) << 1) | (INPLOGIC(m_A) << 0);
	}
);

NETLIB_SUBDEVICE(9310_sub,
	ATTR_HOT inline void update_outputs_all(const UINT8 cnt, const netlist_time out_delay);
	ATTR_HOT inline void update_outputs(const UINT8 cnt);

	logic_input_t m_CLK;

	UINT8 m_cnt;
	NETLIB_NAME(9310_subABCD) *m_ABCD;
	netlist_sig_t m_loadq;
	netlist_sig_t m_ent;

	logic_output_t m_QA;
	logic_output_t m_QB;
	logic_output_t m_QC;
	logic_output_t m_QD;
	logic_output_t m_RC;
);

NETLIB_DEVICE(9310,
	NETLIB_NAME(9310_sub) sub;
	NETLIB_NAME(9310_subABCD) subABCD;
	logic_input_t m_ENP;
	logic_input_t m_ENT;
	logic_input_t m_CLRQ;
	logic_input_t m_LOADQ;
);

NETLIB_DEVICE_DERIVED_PURE(9310_dip, 9310);

NETLIB_NAMESPACE_DEVICES_END()

#endif /* NLD_9310_H_ */
