#pragma once

#include"tsptw_solution.h"

namespace tsptw_tph
{
	/**
* The CreateOne class
*/
	class create_one
	{
		static std::string name() { return "Create-1"; } //create a tour with a single customer

	public:

		using move_type = move<2, 2, name>;

		static auto get_move(const int i, tsptw_solution& sol, move_type& m) -> bool
		{
			const auto v = i;

			const auto v_minus = sol.previous(v);
			const auto v_plus = sol.next(v);

			if (!sol.inst().is_arc_feasible({ 0, v }) || !sol.inst().is_arc_feasible({ v, sol.inst()._num_node })) return false;

			if (v_minus != v_plus) return false;

			m.removed[0] = { v_minus, v };
			m.removed[1] = { v, v_plus };

			m.inserted[0] = { 0, v };
			m.inserted[1] = { v, sol.inst()._num_node };

			m.costs = 0.0;

			m.costs -= sol.inst().cost(v_minus, v);
			m.costs -= sol.inst().cost(v, v_plus);

			m.costs += sol.inst().cost(0, v);
			m.costs += sol.inst().cost(v, sol.inst()._num_node);

			const instance::tour::subtours<3> w_seqs{ { {0}, {v}, {sol.inst()._num_node} } };

			bool check = sol.evaluate_tw(w_seqs);

			if (!check)
			{
				return false;
			}

			auto times = sol.time_updates();

			m.tw_violation = times[0];
			m.penalty = times[1];
			m.CT = times[2];
			m.TD = times[3];
			m.min_slack = times[4];
			m.min_budget = times[5];
			

			return true;
		}
	};
}

