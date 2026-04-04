#pragma once

#include "tsptw_solution.h"

namespace tsptw_tph
{
	class remove_one
	{
		static std::string name() { return "remove-1"; } //remove a customer or a tp on the main route from the solution

	public:
		using move_type = move<1, 2, name>;

		static auto get_move(const int i, tsptw_solution& sol, move_type& m) -> bool
		{
			const auto v = i;

			if (v == 0 || v == sol.inst()._num_node) return false;

			const auto v_plus = sol.next(v);
			const auto v_minus = sol.previous(v);

			if (!sol.inst().is_arc_feasible({ v_minus, v_plus })) return false;

			m.removed[0] = { v, v_plus};
			m.removed[1] = { v, v_minus };

			m.inserted[0] = { v_minus, v_plus };

			m.costs = 0.0;

			m.costs -= sol.inst().cost(v, v_plus);
			m.costs -= sol.inst().cost(v_minus, v);

			m.costs += sol.inst().cost(v_minus, v_plus);


			const instance::tour::subtours<2> v_seqs{ { { 0, v_minus }, { v_plus, sol.inst()._num_node } } };

			bool check = sol.evaluate_tw(v_seqs);

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