#include"./tsptw.tph/tph_heuristics.h"
//#include"./tsptw.tph/tph_heuristics.cpp"

using namespace tsptw_tph;

/*Run the TPH: multiple runs possible
1. inserting initial solution;
2. VNS search for feasible solution;
3. GVNS search for improvement
*/
auto tph_run(const instance& inst, tsptw_solution& best, nondominate_list& non, int seed) -> bool
{
	/*do N runs, one seed for each run*/
	tsptw_solution sol{ inst };

	bool status = true;

	time_t start, end;
	time(&start);

	sol.reset_empty();
	sol.OPTprim = best.OPTprim;

	/*construct initial solution*/
	status = tour_construction::run(sol, seed);

	if (status)
	{
		std::cout << "--Run " << seed << "(" << sol.OPTprim << "): ";

		/*phase1: vns search for a feasible solution*/
		gvns_search::params gvns_params;
		gvns_search::vns_run(sol, gvns_params, non);

		if (sol.feasible())
		{
			/*--------------------------------------------------------*/
			/*phase2: improve the solution*/
			gvns_search::gvns_run(sol, gvns_params, non, seed);

			//this run counts only if a feasible solution is found
			time(&end);
			double duration = difftime(end, start);
			sol.elasped_time = duration;

			bool check = true;
			if (non.rectangle_active)
			{
				check = non.is_below_obj2(sol.objective2());
			}

			if ((sol < best) && check)
			{
				best = sol;
			}
		}
		else
		{
			std::cout << "CANNOT FIND FEASIBLE SOLUTION IN THIS RUN." << std::endl;
		}

		std::cout << (best.feasible() ? "(+)" : "(-)") << ": " << best.objective() << "/" << best.objective2() << " (tw_vio: " << best.tw_violation() << ")";
		if (best.feasible())
		{
			std::cout << " {cost: " << best.cost() << ", CT: " << best.makespan() << ", TD:" << best.tour_duration() << ", MS: " << best.min_slack() << ", MD: " << best.min_budget() << "}" << std::endl;
		}
	}

	if (status)
	{
		if (best.feasible())
		{
			std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
			std::cout.precision(0);

			double primobj = 0.0, secobj = 0.0;
			if (best.OPTprim)
			{
				primobj = best.objective();
				secobj = 0.0 - best.objective2();
			}
			else
			{
				primobj = best.objective2();
				secobj = 0.0 - best.objective();
			}

			std::cout << "-TPH" << "(" << sol.OPTprim << "): " << inst._primobj_type << ": " << primobj << " / " << inst._secobj_type << ": " << secobj << std::endl;
			/*for (const auto i : best.tour())
			{
				std::cout << i << " ";
			}*/
			//std::cout << std::endl;
		}
		else
		{
			std::cout << "-TPH: CANNOT FIND FEASIBLE SOLUTION." << std::endl << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "-INSTANSE ERROR: INFEASIBLE INSTANCE" << std::endl << std::endl;
		return false;
	}

	return true;
}