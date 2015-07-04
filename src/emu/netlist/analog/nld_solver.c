// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_solver.c
 *
 */

/* Commented out for now. Relatively low number of terminals / nets make
 * the vectorizations fast-math enables pretty expensive
 */

#if 0
#pragma GCC optimize "-ffast-math"
#pragma GCC optimize "-ftree-parallelize-loops=4"
//#pragma GCC optimize "-funroll-loops"
#pragma GCC optimize "-funswitch-loops"
#pragma GCC optimize "-fvariable-expansion-in-unroller"
#pragma GCC optimize "-funsafe-loop-optimizations"
#pragma GCC optimize "-fvect-cost-model"
#pragma GCC optimize "-fvariable-expansion-in-unroller"
#pragma GCC optimize "-ftree-loop-if-convert-stores"
#pragma GCC optimize "-ftree-loop-distribution"
#pragma GCC optimize "-ftree-loop-im"
#pragma GCC optimize "-ftree-loop-ivcanon"
#pragma GCC optimize "-fivopts"
#endif

#include <algorithm>
#include "nld_solver.h"
#include "nld_ms_direct.h"
#include "nld_ms_direct1.h"
#include "nld_ms_direct2.h"
#include "nld_ms_sor.h"
#include "nld_ms_sor_mat.h"
#include "nld_twoterm.h"
#include "../nl_lists.h"

#if HAS_OPENMP
#include "omp.h"
#endif

ATTR_COLD void terms_t::add(netlist_terminal_t *term, int net_other)
{
	m_term.add(term);
	m_net_other.add(net_other);
	m_gt.add(0.0);
	m_go.add(0.0);
	m_Idr.add(0.0);
	m_other_curanalog.add(NULL);
}

ATTR_COLD void terms_t::set_pointers()
{
	for (int i = 0; i < count(); i++)
	{
		m_term[i]->m_gt1 = &m_gt[i];
		m_term[i]->m_go1 = &m_go[i];
		m_term[i]->m_Idr1 = &m_Idr[i];
		m_other_curanalog[i] = &m_term[i]->m_otherterm->net().as_analog().m_cur_Analog;
	}
}

// ----------------------------------------------------------------------------------------
// netlist_matrix_solver
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_matrix_solver_t::netlist_matrix_solver_t(const eSolverType type, const netlist_solver_parameters_t &params)
: m_stat_calculations(0),
	m_stat_newton_raphson(0),
	m_stat_vsolver_calls(0),
	m_params(params),
	m_cur_ts(0),
	m_type(type)
{
}

ATTR_COLD netlist_matrix_solver_t::~netlist_matrix_solver_t()
{
	m_inps.clear_and_free();
}

ATTR_COLD void netlist_matrix_solver_t::setup(netlist_analog_net_t::list_t &nets)
{
	NL_VERBOSE_OUT(("New solver setup\n"));

	m_nets.clear();

	for (std::size_t k = 0; k < nets.size(); k++)
	{
		m_nets.add(nets[k]);
	}

	for (std::size_t k = 0; k < nets.size(); k++)
	{
		NL_VERBOSE_OUT(("setting up net\n"));

		netlist_analog_net_t *net = nets[k];

		net->m_solver = this;

		for (std::size_t i = 0; i < net->m_core_terms.size(); i++)
		{
			netlist_core_terminal_t *p = net->m_core_terms[i];
			NL_VERBOSE_OUT(("%s %s %d\n", p->name().cstr(), net->name().cstr(), (int) net->isRailNet()));
			switch (p->type())
			{
				case netlist_terminal_t::TERMINAL:
					switch (p->netdev().family())
					{
						case netlist_device_t::CAPACITOR:
							if (!m_step_devices.contains(&p->netdev()))
								m_step_devices.add(&p->netdev());
							break;
						case netlist_device_t::BJT_EB:
						case netlist_device_t::DIODE:
						//case netlist_device_t::VCVS:
						case netlist_device_t::BJT_SWITCH:
							NL_VERBOSE_OUT(("found BJT/Diode\n"));
							if (!m_dynamic_devices.contains(&p->netdev()))
								m_dynamic_devices.add(&p->netdev());
							break;
						default:
							break;
					}
					{
						netlist_terminal_t *pterm = dynamic_cast<netlist_terminal_t *>(p);
						add_term(k, pterm);
					}
					NL_VERBOSE_OUT(("Added terminal\n"));
					break;
				case netlist_terminal_t::INPUT:
					{
						netlist_analog_output_t *net_proxy_output = NULL;
						for (std::size_t i = 0; i < m_inps.size(); i++)
							if (m_inps[i]->m_proxied_net == &p->net().as_analog())
							{
								net_proxy_output = m_inps[i];
								break;
							}

						if (net_proxy_output == NULL)
						{
							net_proxy_output = palloc(netlist_analog_output_t);
							net_proxy_output->init_object(*this, this->name() + "." + pstring::sprintf("m%" SIZETFMT, m_inps.size()));
							m_inps.add(net_proxy_output);
							net_proxy_output->m_proxied_net = &p->net().as_analog();
						}
						net_proxy_output->net().register_con(*p);
						// FIXME: repeated
						net_proxy_output->net().rebuild_list();
						NL_VERBOSE_OUT(("Added input\n"));
					}
					break;
				default:
					netlist().error("unhandled element found\n");
					break;
			}
		}
		NL_VERBOSE_OUT(("added net with %d populated connections\n", net->m_core_terms.size()));
	}
}


ATTR_HOT void netlist_matrix_solver_t::update_inputs()
{
	// avoid recursive calls. Inputs are updated outside this call
	for (std::size_t i=0; i<m_inps.size(); i++)
		m_inps[i]->set_Q(m_inps[i]->m_proxied_net->m_cur_Analog);

}

ATTR_HOT void netlist_matrix_solver_t::update_dynamic()
{
	/* update all non-linear devices  */
	for (std::size_t i=0; i < m_dynamic_devices.size(); i++)
		switch (m_dynamic_devices[i]->family())
		{
			case netlist_device_t::DIODE:
				static_cast<NETLIB_NAME(D) *>(m_dynamic_devices[i])->update_terminals();
				break;
			default:
				m_dynamic_devices[i]->update_terminals();
				break;
		}
}

ATTR_COLD void netlist_matrix_solver_t::start()
{
	register_output("Q_sync", m_Q_sync);
	register_input("FB_sync", m_fb_sync);
	connect(m_fb_sync, m_Q_sync);
}

ATTR_COLD void netlist_matrix_solver_t::reset()
{
	m_last_step = netlist_time::zero;
}

ATTR_COLD void netlist_matrix_solver_t::update()
{
	const nl_double new_timestep = solve();

	if (m_params.m_dynamic && is_timestep() && new_timestep > 0)
		m_Q_sync.net().reschedule_in_queue(netlist_time::from_double(new_timestep));
}

ATTR_COLD void netlist_matrix_solver_t::update_forced()
{
	ATTR_UNUSED const nl_double new_timestep = solve();

	if (m_params.m_dynamic && is_timestep())
		m_Q_sync.net().reschedule_in_queue(netlist_time::from_double(m_params.m_min_timestep));
}

ATTR_HOT void netlist_matrix_solver_t::step(const netlist_time delta)
{
	const nl_double dd = delta.as_double();
	for (std::size_t k=0; k < m_step_devices.size(); k++)
		m_step_devices[k]->step_time(dd);
}

template<class C >
void netlist_matrix_solver_t::solve_base(C *p)
{
	m_stat_vsolver_calls++;
	if (is_dynamic())
	{
		int this_resched;
		int newton_loops = 0;
		do
		{
			update_dynamic();
			// Gauss-Seidel will revert to Gaussian elemination if steps exceeded.
			this_resched = p->vsolve_non_dynamic(true);
			newton_loops++;
		} while (this_resched > 1 && newton_loops < m_params.m_nr_loops);

		m_stat_newton_raphson += newton_loops;
		// reschedule ....
		if (this_resched > 1 && !m_Q_sync.net().is_queued())
		{
			netlist().warning("NEWTON_LOOPS exceeded ... reschedule");
			m_Q_sync.net().reschedule_in_queue(m_params.m_nt_sync_delay);
		}
	}
	else
	{
		p->vsolve_non_dynamic(false);
	}
}

ATTR_HOT nl_double netlist_matrix_solver_t::solve()
{
	netlist_time now = netlist().time();
	netlist_time delta = now - m_last_step;

	// We are already up to date. Avoid oscillations.
	// FIXME: Make this a parameter!
	if (delta < netlist_time::from_nsec(1))
		return -1.0;

	/* update all terminals for new time step */
	m_last_step = now;
	m_cur_ts = delta.as_double();

	step(delta);

	const nl_double next_time_step = vsolve();

	update_inputs();
	return next_time_step;
}


// ----------------------------------------------------------------------------------------
// netlist_matrix_solver - Direct base
// ----------------------------------------------------------------------------------------

ATTR_COLD int netlist_matrix_solver_t::get_net_idx(netlist_net_t *net)
{
	for (std::size_t k = 0; k < m_nets.size(); k++)
		if (m_nets[k] == net)
			return k;
	return -1;
}







// ----------------------------------------------------------------------------------------
// solver
// ----------------------------------------------------------------------------------------



NETLIB_START(solver)
{
	register_output("Q_step", m_Q_step);

	register_param("SYNC_DELAY", m_sync_delay, NLTIME_FROM_NS(10).as_double());

	register_param("FREQ", m_freq, 48000.0);

	register_param("ACCURACY", m_accuracy, 1e-7);
	register_param("GS_LOOPS", m_gs_loops, 9);              // Gauss-Seidel loops
	register_param("GS_THRESHOLD", m_gs_threshold, 6);      // below this value, gaussian elimination is used
	register_param("NR_LOOPS", m_nr_loops, 25);             // Newton-Raphson loops
	register_param("PARALLEL", m_parallel, 0);
	register_param("SOR_FACTOR", m_sor, 1.059);
	register_param("GMIN", m_gmin, NETLIST_GMIN_DEFAULT);
	register_param("DYNAMIC_TS", m_dynamic, 0);
	register_param("LTE", m_lte, 5e-5);                     // diff/timestep
	register_param("MIN_TIMESTEP", m_min_timestep, 1e-6);   // nl_double timestep resolution

	register_param("LOG_STATS", m_log_stats, 1);   // nl_double timestep resolution

	// internal staff

	register_input("FB_step", m_fb_step);
	connect(m_fb_step, m_Q_step);

}

NETLIB_RESET(solver)
{
	for (std::size_t i = 0; i < m_mat_solvers.size(); i++)
		m_mat_solvers[i]->reset();
}


NETLIB_UPDATE_PARAM(solver)
{
	//m_inc = netlist_time::from_hz(m_freq.Value());
}

NETLIB_STOP(solver)
{
	for (std::size_t i = 0; i < m_mat_solvers.size(); i++)
		m_mat_solvers[i]->log_stats();
}

NETLIB_NAME(solver)::~NETLIB_NAME(solver)()
{
	m_mat_solvers.clear_and_free();
}

NETLIB_UPDATE(solver)
{
	if (m_params.m_dynamic)
		return;

	const std::size_t t_cnt = m_mat_solvers.size();

#if HAS_OPENMP && USE_OPENMP
	if (m_parallel.Value())
	{
		omp_set_num_threads(4);
		omp_set_dynamic(0);
		#pragma omp parallel
		{
			#pragma omp for
			for (int i = 0; i <  t_cnt; i++)
				if (m_mat_solvers[i]->is_timestep())
				{
					// Ignore return value
					ATTR_UNUSED const nl_double ts = m_mat_solvers[i]->solve();
				}
		}
	}
	else
		for (int i = 0; i < t_cnt; i++)
			if (m_mat_solvers[i]->is_timestep())
			{
				// Ignore return value
				ATTR_UNUSED const nl_double ts = m_mat_solvers[i]->solve();
			}
#else
	for (std::size_t i = 0; i < t_cnt; i++)
	{
		if (m_mat_solvers[i]->is_timestep())
		{
			// Ignore return value
			ATTR_UNUSED const nl_double ts = m_mat_solvers[i]->solve();
		}
	}
#endif

	/* step circuit */
	if (!m_Q_step.net().is_queued())
	{
		m_Q_step.net().push_to_queue(netlist_time::from_double(m_params.m_max_timestep));
	}
}

template <int m_N, int _storage_N>
netlist_matrix_solver_t * NETLIB_NAME(solver)::create_solver(int size, const int gs_threshold, const bool use_specific)
{
	if (use_specific && m_N == 1)
		return palloc(netlist_matrix_solver_direct1_t, m_params);
	else if (use_specific && m_N == 2)
		return palloc(netlist_matrix_solver_direct2_t, m_params);
	else
	{
		if (size >= gs_threshold)
		{
			if (USE_MATRIX_GS)
			{
				typedef netlist_matrix_solver_SOR_mat_t<m_N,_storage_N> solver_mat;
				return palloc(solver_mat, m_params, size);
			}
			else
			{
				typedef netlist_matrix_solver_SOR_t<m_N,_storage_N> solver_GS;
				return palloc(solver_GS, m_params, size);
			}
		}
		else
		{
			typedef netlist_matrix_solver_direct_t<m_N,_storage_N> solver_D;
			return palloc(solver_D, m_params, size);
		}
	}
}

ATTR_COLD void NETLIB_NAME(solver)::post_start()
{
	netlist_analog_net_t::list_t groups[100];
	int cur_group = -1;
	const int gs_threshold = m_gs_threshold.Value();
	const bool use_specific = true;

	m_params.m_accuracy = m_accuracy.Value();
	m_params.m_gs_loops = m_gs_loops.Value();
	m_params.m_nr_loops = m_nr_loops.Value();
	m_params.m_nt_sync_delay = m_sync_delay.Value();
	m_params.m_lte = m_lte.Value();
	m_params.m_sor = m_sor.Value();

	m_params.m_min_timestep = m_min_timestep.Value();
	m_params.m_dynamic = (m_dynamic.Value() == 1 ? true : false);
	m_params.m_max_timestep = netlist_time::from_hz(m_freq.Value()).as_double();

	if (m_params.m_dynamic)
	{
		m_params.m_max_timestep *= NL_FCONST(1000.0);
	}
	else
	{
		m_params.m_min_timestep = m_params.m_max_timestep;
	}

	// Override log statistics
	pstring p = nl_util::environment("NL_STATS");
	if (p != "")
		m_params.m_log_stats = (bool) p.as_long();
	else
		m_params.m_log_stats = (bool) m_log_stats.Value();

	netlist().log("Scanning net groups ...");
	// determine net groups
	for (std::size_t i=0; i<netlist().m_nets.size(); i++)
	{
		SOLVER_VERBOSE_OUT(("processing %s\n", netlist().m_nets[i]->name().cstr()));
		if (!netlist().m_nets[i]->isRailNet())
		{
			SOLVER_VERBOSE_OUT(("   ==> not a rail net\n"));
			netlist_analog_net_t *n = &netlist().m_nets[i]->as_analog();
			if (!n->already_processed(groups, cur_group))
			{
				cur_group++;
				n->process_net(groups, cur_group);
			}
		}
	}

	// setup the solvers
	netlist().log("Found %d net groups in %" SIZETFMT " nets\n", cur_group + 1, netlist().m_nets.size());
	for (int i = 0; i <= cur_group; i++)
	{
		netlist_matrix_solver_t *ms;
		std::size_t net_count = groups[i].size();

		switch (net_count)
		{
			case 1:
				ms = create_solver<1,1>(1, gs_threshold, use_specific);
				break;
			case 2:
				ms = create_solver<2,2>(2, gs_threshold, use_specific);
				break;
			case 3:
				ms = create_solver<3,3>(3, gs_threshold, use_specific);
				break;
			case 4:
				ms = create_solver<4,4>(4, gs_threshold, use_specific);
				break;
			case 5:
				ms = create_solver<5,5>(5, gs_threshold, use_specific);
				break;
			case 6:
				ms = create_solver<6,6>(6, gs_threshold, use_specific);
				break;
			case 7:
				ms = create_solver<7,7>(7, gs_threshold, use_specific);
				break;
			case 8:
				ms = create_solver<8,8>(8, gs_threshold, use_specific);
				break;
			case 12:
				ms = create_solver<12,12>(12, gs_threshold, use_specific);
				break;
			default:
				if (net_count <= 16)
				{
					ms = create_solver<0,16>(net_count, gs_threshold, use_specific);
				}
				else if (net_count <= 32)
				{
					ms = create_solver<0,32>(net_count, gs_threshold, use_specific);
				}
				else if (net_count <= 64)
				{
					ms = create_solver<0,64>(net_count, gs_threshold, use_specific);
				}
				else
				{
					netlist().error("Encountered netgroup with > 64 nets");
					ms = NULL; /* tease compilers */
				}

				break;
		}

		register_sub(pstring::sprintf("Solver_%" SIZETFMT,m_mat_solvers.size()), *ms);

		ms->vsetup(groups[i]);

		m_mat_solvers.add(ms);

		netlist().log("Solver %s", ms->name().cstr());
		netlist().log("       # %d ==> %" SIZETFMT " nets", i, groups[i].size()); //, (*(*groups[i].first())->m_core_terms.first())->name().cstr());
		netlist().log("       has %s elements", ms->is_dynamic() ? "dynamic" : "no dynamic");
		netlist().log("       has %s elements", ms->is_timestep() ? "timestep" : "no timestep");
		for (std::size_t j=0; j<groups[i].size(); j++)
		{
			netlist().log("Net %" SIZETFMT ": %s", j, groups[i][j]->name().cstr());
			netlist_net_t *n = groups[i][j];
			for (std::size_t k = 0; k < n->m_core_terms.size(); k++)
			{
				const netlist_core_terminal_t *p = n->m_core_terms[k];
				netlist().log("   %s", p->name().cstr());
			}
		}
	}
}
