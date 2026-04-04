#pragma once

#include "tsptw_solution.h"

namespace tsptw_tph
{
	class one_opt /*only considers main arcs on the route*/
	{
		static std::string name() { return "1-opt"; }  // swap two consecurtive nodes

	public:

		using move_type = move<3, 3, name>;

		static auto get_move(const instance::arc& a, tsptw_solution& sol, move_type& m, move<dynamic>& best_mov) -> bool
		{
			const auto v = a.i;
			const auto w = a.j;

			if (v == w || v == 0 || v == sol.inst()._num_node || w == sol.inst()._num_node) return false;
			if (sol.position(v) >= sol.position(w)) return false;

			const auto v_minus = sol.previous(v);
			const auto w_plus = sol.next(w);

			m.removed[0] = { v_minus, v };
			m.removed[1] = { w, w_plus };
			m.removed[2] = { v, w };

			m.inserted[0] = { v_minus, w };
			m.inserted[1] = { w, v };
			m.inserted[2] = { v, w_plus };

			m.costs = 0.0;

			m.costs -= sol.inst().cost(v_minus, v);
			m.costs -= sol.inst().cost(v, w);
			m.costs -= sol.inst().cost(w, w_plus);

			m.costs += sol.inst().cost(v_minus, w);
			m.costs += sol.inst().cost(w, v);
			m.costs += sol.inst().cost(v, w_plus);

			if (sol.feasible() && sol.OPTprim && (sol.inst()._primobj_type == PrimObjType::distance))
			{
				if (m.costs > best_mov.gen_objective) return false;
			}

			const instance::tour::subtours<4> w_seqs{ { { 0, v_minus }, { w }, { v }, { w_plus, sol.inst()._num_node } } };

			bool check = sol.evaluate_tw(w_seqs);

			if (!check) return false;

			auto times = sol.time_updates();

			m.tw_violation = -sol.tw_violation() + times[0];
			m.penalty = -sol.total_penalty() + times[1];
			m.CT = -sol.makespan() + times[2];
			m.TD = -sol.tour_duration() + times[3];
			m.min_slack = -sol.min_slack() + times[4];
			m.min_budget = -sol.min_budget() + times[5];

			return true;
		}
	};
}