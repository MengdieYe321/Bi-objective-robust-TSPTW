#pragma once

#include "tsptw_solution.h"

namespace tsptw_tph
{
	/**
* The InsertOne class
*/
	class insert_one
	{
		static std::string name() { return "Insert-1"; }

	public:

		using move_type = move<2, 1, name>;

		static auto get_move(const instance::arc& a, tsptw_solution& sol, move_type& m) -> bool
		{
			const auto v = a.i;
			const auto w = a.j;

			const auto v_plus = sol.next(v);
			const auto w_minus = sol.previous(w);
			const auto w_plus = sol.next(w);

			if (w_minus != w_plus) return false;

			if (!sol.inst().is_arc_feasible({ v, w }) || !sol.inst().is_arc_feasible({ w, v_plus })) return false;

			m.removed[0] = { v, v_plus };

			m.inserted[0] = { v, w };
			m.inserted[1] = { w, v_plus };

			m.costs = 0.0;

			m.costs -= sol.inst().cost(v, v_plus);

			m.costs += sol.inst().cost(v, w);
			m.costs += sol.inst().cost(w, v_plus);

			const instance::tour::subtours<3> w_seqs{ { { 0, v }, { w }, { v_plus, sol.inst()._num_node } } };

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
