#pragma once

#include "tsptw_solution.h"

namespace tsptw_tph
{
	class two_opt /*only considers main arcs on the route*/
	{
		static std::string name() { return "2-Opt"; }  // exchange two node in two separate route

	public:
		using move_type = move<2, 2, name>;

		static auto get_move(const instance::arc& a, tsptw_solution& sol, move_type& m, move<dynamic>& best_mov) -> bool
		{
			/*v, w are both possibly be tps*/
			const auto v = a.i;
			if (v == 0 || (sol.position(v) >= sol.inst()._num_node - 2)) return false;

			const auto w = a.j;
			if (sol.position(w) < (sol.position(v) + 2))return false;

			const auto w_plus = sol.next(w);
			const auto v_plus = sol.next(v);

			if (!sol.inst().is_arc_feasible({ v, w }) || !sol.inst().is_arc_feasible({v_plus, w_plus})) return false;

			m.removed[0] = { w, w_plus };
			m.removed[1] = { v, v_plus };

			m.inserted[0] = { v, w };
			m.inserted[1] = { v_plus, w_plus }; //inverted arcs not shown here: no influence on cost

			m.costs = 0.0;

			m.costs -= sol.inst().cost(w, w_plus);
			m.costs -= sol.inst().cost(v, v_plus);

			m.costs += sol.inst().cost(v, w);
			m.costs += sol.inst().cost(v_plus, w_plus);

			std::vector<instance::tour::subtour> seqs;

			seqs.push_back({ 0, v });
			seqs.push_back({ w });

			m.inverted_seqs.clear();
			m.inverted_seqs.push_back({ w });

			const auto pos_diff = sol.position(w) - sol.position(v) - 2;

			auto last = w;

			if (pos_diff > 0)
			{
				int i = 0;
				while (i < pos_diff)
				{
					auto w_minus = sol.previous(last);

					if (sol.inst().is_arc_feasible({last, w_minus}))
					{
						seqs.push_back({ w_minus });

						m.inverted_seqs.push_back({ w_minus });

						m.costs -= sol.inst().cost(w_minus, last);
						m.costs += sol.inst().cost(last, w_minus);

						last = w_minus;

						i++;
					}
					else
					{
						return false;
					}
					
				}
			}

			if (sol.inst().is_arc_feasible({ last, v_plus }))
			{
				seqs.push_back({ v_plus });
				m.inverted_seqs.push_back({ v_plus });

				m.costs -= sol.inst().cost(v_plus, last);
				m.costs += sol.inst().cost(last, v_plus);
			}
			else
			{
				return false;
			}
			
			seqs.push_back({ w_plus, sol.inst()._num_node });

			if (sol.feasible() && sol.OPTprim && (sol.inst()._primobj_type == PrimObjType::distance))
			{
				if (m.costs > best_mov.gen_objective) return false;
			}

			bool check = sol.evaluate_tw(seqs);

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