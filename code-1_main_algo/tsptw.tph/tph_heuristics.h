#pragma once

#include"neighborhoods.hpp"


namespace tsptw_tph
{
	class tour_construction
	{
	public:

		static bool run(tsptw_solution& sol, int seed);
	};

	class gvns_search
	{
	public:
		struct params
		{
			explicit params()
				: time_limit_vns(30)
				, time_limit_gvns(60)
				, iteration_without_improvement(1)
				, level_max(30)
				, level_max_ls(30)
				, neighborhood_params()
			{
				//empty
			}

			int iteration_without_improvement;
			int level_max;
			int level_max_ls;

			double time_limit_vns;
			double time_limit_gvns;

			neighborhood::params neighborhood_params;
		};

		/*run the vns to find a feasible solution*/
		static auto vns_run(tsptw_solution& best, const params& params, nondominate_list& non) -> void;

		/*run the gvns to improve the feasible solution*/
		static auto gvns_run(tsptw_solution& best, const params& params, nondominate_list& non, int seed) -> void;

		static auto gvns_one_run(tsptw_solution& best, const params& params, nondominate_list& non, int seed) -> void;

		static auto ls_run(tsptw_solution& best, const params& params, nondominate_list& non, const int seed) -> void;
	};
}

auto tph_run(const instance& inst, tsptw_tph::tsptw_solution& best, nondominate_list& non, const int seed) -> bool;
