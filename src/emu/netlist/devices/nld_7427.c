// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7427.c
 *
 */

#include "nld_7427.h"

#include "nld_7402.h"

#if (USE_TRUTHTABLE)
nld_7427::truthtable_t nld_7427::m_ttbl;
const char *nld_7427::m_desc[] = {
		"A,B,C|Q ",
		"0,0,0|1|22",
		"1,X,X|0|15",
		"X,1,X|0|15",
		"X,X,1|0|15",
		""
};
#endif

NETLIB_START(7427_dip)
{
	register_sub("1", m_1);
	register_sub("2", m_2);
	register_sub("3", m_3);

	register_subalias("1", m_1.m_i[0]);
	register_subalias("2", m_1.m_i[1]);
	register_subalias("3", m_2.m_i[0]);
	register_subalias("4", m_2.m_i[1]);
	register_subalias("5", m_2.m_i[2]);
	register_subalias("6", m_2.m_Q[0]);

	register_subalias("8", m_3.m_Q[0]);
	register_subalias("9", m_3.m_i[0]);
	register_subalias("10", m_3.m_i[1]);
	register_subalias("11", m_3.m_i[2]);

	register_subalias("12", m_1.m_Q[0]);
	register_subalias("13", m_1.m_i[2]);
}

NETLIB_UPDATE(7427_dip)
{
	/* only called during startup */
	m_1.update_dev();
	m_2.update_dev();
	m_3.update_dev();
}

NETLIB_RESET(7427_dip)
{
	m_1.do_reset();
	m_2.do_reset();
	m_3.do_reset();
}
