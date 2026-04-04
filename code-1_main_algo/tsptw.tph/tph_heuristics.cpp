#include"tph_heuristics.h"
//#include"neighborhoods.cpp"
//#include"tsptw_solution.cpp"

namespace tsptw_tph
{
	bool tour_construction::run(tsptw_solution& sol, int seed)
	{
		sol.phase_feasi = 0;

		sol.set_alpha(10);
		sol.set_alpha_set(10);

		mt19937 randomgenerator(seed);

		auto cuts = sol.unrouted_nodes();

		bool tour_status = false;

		time_t start, end;
		time(&start);

		double duration = 0;
		int iter = 0;
		while (!tour_status && (duration < 60))
		{
			/*get the initial tour by using random order*/
			std::shuffle(cuts.begin(), cuts.end(), randomgenerator);

			/*avoiding incompatible arcs (inf arcs)*/
			bool infarc = false;
			if (sol.inst().is_arc_feasible({ 0, cuts.front() }) && sol.inst().is_arc_feasible({ cuts.back(), sol.inst()._num_node }))
			{
				tour_status = true;

				bool arc_status = false;
				while (!arc_status)
				{
					bool checkarc = true;
					for (int it = 0; it < cuts.size() - 1; it++)
					{
						int it_plus = it + 1;

						if (!sol.inst().is_arc_feasible({ cuts[it], cuts[it_plus] }))
						{
							if (sol.inst().is_arc_feasible({ cuts[it_plus], cuts[it] }))
							{
								checkarc = false;

								int temp = cuts[it];
								cuts[it] = cuts[it_plus];
								cuts[it_plus] = temp;
							}
							else
							{
								infarc = true;
								break;
							}
						}

						if (!checkarc)
						{
							arc_status = false;
							break;
						}
						else
						{
							arc_status = true;
						}
					}

					if (infarc)
					{
						break;
					}
				}
			}
			else
			{
				infarc = true;
			}

			if (infarc)
			{
				tour_status = false;
				iter++;
			}

			time(&end);
			duration = difftime(end, start);

		}

		if (tour_status)
		{
			/*construct the initial solution*/
			auto*i = &cuts.front();
			auto first = *i;
			int last = first;

			create_one::move_type m_c;

			if (create_one::get_move(first, sol, m_c))
			{
				sol.apply_move(m_c);

				last = first;
			}

			while (sol.unrouted_nodes().size() > 0)
			{
				insert_one::move_type m_i;

				i++;

				auto next = *i;

				if (insert_one::get_move({ last, next }, sol, m_i))
				{
					sol.apply_move(m_i);

					last = next;
				}
			}
		}

		return tour_status;
	}

	auto gvns_search::vns_run(tsptw_solution& best, const params& params, nondominate_list& non) -> void
	{
		tsptw_solution sol = best;

		sol.phase_feasi = 0;

		sol.set_alpha(sol.penalty_init);
		sol.set_alpha_set(sol.penalty_init);

		bool first_improve = false;//should be true

		time_t start, end;
		time(&start);

		sol.alphaIterationInfeasible = 0;

		int seed = 1;

		double elasped = 0.0;
		while (elasped <= params.time_limit_vns)
		{
			bool in_time = true;

			bool good_shake = true;

			neighborhood neigh{ sol, best, non };

			/*search the vns neighborhoods*/
			in_time = neigh.one_shift_ls(first_improve, params.time_limit_vns);

			//update current and best solution
			if (sol.feasible())
			{
				best = sol;
				return;
			}
			else if (sol < best)
			{
				best = sol;
			}

			if (!in_time) return;

			/*combination of shake and ls*/
			int k = 1;
			while (k <= sol.shake_iter_max())
			{
				/*k-level shake*/
				tsptw_solution original_sol = sol;
				auto level = k * 5;
				good_shake = neigh.shake_phase(level, seed, params.time_limit_vns);//apply move inside
				seed++;
				if (good_shake)
				{
					if (sol.feasible())
					{
						best = sol;
						return;
					}
					else if (sol < best)
					{
						best = sol;
					}

					/*1-shift ls*/
					in_time = neigh.one_shift_ls(first_improve, params.time_limit_vns);

					if (sol.feasible())
					{
						best = sol;
						return;
					}
					else if (sol < best)
					{
						best = sol;
						k = 1;
					}
					else
					{
						k++;
					}
				}
				else
				{
					sol = original_sol;
					k++;
				}

				if (!in_time) return;
			}

			/*update penalties*/
			//sol.update_penalty(sol.penalty_max, sol.penalty_min, sol.penalty_delta, sol.penalty_steps);
			sol.update_penalty_set(sol.penalty_max, sol.penalty_min, sol.penalty_delta, sol.penalty_steps);

			time(&end);
			elasped = difftime(end, start);
		}
	}

	auto gvns_search::gvns_run(tsptw_solution& best, const params& params, nondominate_list& non, int seed) -> void
	{
		best.set_alpha(best.penalty_init);
		best.set_alpha_set(best.penalty_init);
		best.phase_feasi = 1;

		tsptw_solution sol = best;

		bool first_improve = false;

		double best_value = best.objective();
		double last_best_value = best_value;

		time_t start, end;
		time(&start);
		double elasped = 0.0;

		int iteration = 0;
		while (elasped <= params.time_limit_gvns && (iteration < params.iteration_without_improvement))
		{
			/*search the gvns neighborhoods*/
			bool in_time = true;
			bool good_shake = true;

			sol = best;
			neighborhood neigh{ sol, best, non };

			/*combination of shake and ls*/
			int k = 1;
			while (k <= sol.shake_iter_max() && (elasped <= params.time_limit_gvns))
			{
				/*k-level shake*/
				tsptw_solution original_sol = sol;
				auto level = k * 5;
				good_shake = neigh.shake_phase(level, seed, params.time_limit_gvns);//apply move inside
				seed++;

				if (good_shake)
				{
					bool within_bound = non.is_within_bound(sol.objective(), sol.objective2(), sol.OPTprim);

					if (sol < best)
					{
						if (within_bound)
						{
							best = sol;
							best_value = best.objective();
						}
					}

					/*vnd*/
					in_time = neigh.vnd(first_improve, params.time_limit_gvns);

					within_bound = non.is_within_bound(sol.objective(), sol.objective2(), sol.OPTprim);

					if (sol < best)
					{
						if (within_bound)
						{
							best = sol;
							best_value = best.objective();
							k = 1;
						}
						else
						{
							k++;
						}
					}
					else
					{
						k++;
					}
				}
				else
				{
					sol = original_sol;
					k++;
				}

				if (!in_time) return;

				time(&end);
				elasped = difftime(end, start);
			}

			/*update the best overall solution*/
			if (best_value < last_best_value)
			{
				last_best_value = best_value;
				iteration = 0;
			}
			else
			{
				iteration++;
			}

			time(&end);
			elasped = difftime(end, start);
		}
	}

	auto gvns_search::gvns_one_run(tsptw_solution& best, const params& params, nondominate_list& non, int seed) -> void
	{
		best.set_alpha(best.penalty_init);
		best.set_alpha_set(best.penalty_init);
		best.phase_feasi = 1;

		bool first_improve = false;

		time_t start, end;
		time(&start);
		double elasped = 0.0;

		/*search the gvns neighborhoods*/
		bool in_time = true;
		bool good_shake = true;

		tsptw_solution sol = best;
		neighborhood neigh{ sol, best, non };

		/*combination of shake and ls*/
		int k = 1;
		while (k <= sol.shake_iter_max() && (elasped <= params.time_limit_gvns))
		{
			/*k-level shake*/
			tsptw_solution original_sol = sol;
			auto level = k * 5;
			good_shake = neigh.shake_phase(level, seed, params.time_limit_gvns);//apply move inside
			seed++;

			if (good_shake)
			{
				bool within_bound = non.is_within_bound(sol.objective(), sol.objective2(), sol.OPTprim);

				if (sol < best)
				{
					if (within_bound)
					{
						best = sol;
					}
				}

				/*vnd*/
				in_time = neigh.vnd(first_improve, params.time_limit_gvns);

				within_bound = non.is_within_bound(sol.objective(), sol.objective2(), sol.OPTprim);

				if (sol < best)
				{
					if (within_bound)
					{
						best = sol;
						k = 1;
					}
					else
					{
						k++;
					}
				}
				else
				{
					k++;
				}
			}
			else
			{
				sol = original_sol;
				k++;
			}

			if (!in_time) return;

			time(&end);
			elasped = difftime(end, start);
		}
	}

	auto gvns_search::ls_run(tsptw_solution& best, const params& params, nondominate_list& non, const int seed) -> void
	{
		best.set_alpha(best.penalty_init);
		best.set_alpha_set(best.penalty_init);
		best.phase_feasi = 1;

		bool first_improve = false;

		/*search the gvns neighborhoods*/
		bool in_time = true;
		bool good_shake = true;

		tsptw_solution sol = best;
		neighborhood neigh{ sol, best, non };

		/*combination of shake and ls*/
		mt19937 randomgenerator(seed);
		uniform_int_distribution<int> knum(1, sol.shake_iter_max());
		int k = knum(randomgenerator);

		/*k-level shake*/
		tsptw_solution original_sol = sol;
		auto level = k * 5;
		good_shake = neigh.shake_phase(level, seed, params.time_limit_gvns);

		if (good_shake)
		{
			bool within_bound = non.is_within_bound(sol.objective(), sol.objective2(), sol.OPTprim);
			if (sol < best)
			{
				if (within_bound)
				{
					best = sol;
				}
			}

			/*vnd*/
			in_time = neigh.vnd(first_improve, params.time_limit_gvns);

			within_bound = non.is_within_bound(sol.objective(), sol.objective2(), sol.OPTprim);

			if (sol < best)
			{
				if (within_bound)
				{
					best = sol;
				}
			}
		}
		else
		{
			sol = original_sol;
		}

		if (!in_time) return;
	}
}