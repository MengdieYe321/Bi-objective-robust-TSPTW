#include"performance.h"

algoperform::algoperform()
	: num_nondomi_sol_eva(0)
	, num_nondomi_sol_ref(0)
	, hypervolume_ref(0)
	, hypervolume_eva(0)
	, epsilon(1)
	, average_distance(0)
	, uniform_spacing_indicator(0)
{

}

algoperform::~algoperform()
{
	//empty
}

void algoperform::performanceIndicatorGenerator(solution_set& solset)
{
	/*performance indicators*/
	/*--cardinality*/
	countNondominatedSolutions(solset);

	solset.normalization();

	/*--Hypervolume (Ih)*/
	computeHypervolume(solset);

	/*--epsilon (Ie)*/
	computeUnaryEpsilon(solset);

	/*--convergence metric (Ic)*/
	computeConvergenceMetric(solset);

	/*--spaceing (Is)*/
	computeSpacingIndicator(solset);
}

/*check for the approximation set its non-dominated solutions*/
void algoperform::countNondominatedSolutions(solution_set& sol)
{
	/*reference set*/
	num_nondomi_sol_ref = sol.num_sols_ref;

	auto& last_ref = sol.reference_set.back();
	if (!last_ref.nondomi)
	{
		num_nondomi_sol_ref--;
	}

	/*approximation set*/
	num_nondomi_sol_eva = 0;

	for (auto& s : sol.solutions)
	{
		int index = 0;
		while (index < sol.reference_set.size())
		{
			if ((s.primobj == sol.reference_set[index].primobj) && (s.secobj == sol.reference_set[index].secobj))
			{
				s.nondomi = true;
				break;
			}

			index++;
		}

		if (s.nondomi)
		{
			num_nondomi_sol_eva++;
		}
	}
}

void algoperform::computeHypervolume(solution_set& sol)
{
	/*compute whole area of the rectangle*/
	hypervolume_ref = hypervolume_eva = 0;

	double area = (sol.nadir_points[0][0] - sol.nadir_points[1][0]) * (sol.nadir_points[1][1] - sol.nadir_points[0][1]);

	/*compute H for ref set*/
	/*for (int i = 1; i < sol.reference_set.size(); i++)
	{
		auto pre = i - 1;

		auto prim_sum = sol.nadir_point[0][0] - sol.reference_set[i].prim_obj;
		auto sec_sum = sol.reference_set[i].sec_obj - sol.reference_set[pre].sec_obj;

		auto hyperplan = prim_sum * sec_sum;

		hypervolume_ref += hyperplan;
	}*/

	//modified
	for (int i = 0; i < sol.reference_set.size(); i++)
	{
		auto prim_sum = sol.nadir_points[0][0] - sol.reference_set[i].primobj;

		double sec_sum = 0.0;
		if (i == 0)
		{
			sec_sum = sol.reference_set[i].secobj - sol.nadir_points[0][1];
		}
		else
		{
			auto pre = i - 1;
			sec_sum = sol.reference_set[i].secobj - sol.reference_set[pre].secobj;
		}

		auto hyperplan = prim_sum * sec_sum;

		hypervolume_ref += hyperplan;
	}

	hypervolume_ref = hypervolume_ref / area;

	/*compute H for eva set*/
	/*for (int i = 1; i < sol.solution_set_eva.size(); i++)
	{
		auto pre = i - 1;

		auto prim_sum = sol.nadir_point[0][0] - sol.solution_set_eva[i].prim_obj;
		auto sec_sum = sol.solution_set_eva[i].sec_obj - sol.solution_set_eva[pre].sec_obj;

		auto hyperplan = prim_sum * sec_sum;

		hypervolume_eva += hyperplan;
	}*/

	//modified
	for (int i = 0; i < sol.solutions.size(); i++)
	{
		auto prim_sum = sol.nadir_points[0][0] - sol.solutions[i].primobj;

		double sec_sum = 0.0;
		if (i == 0)
		{
			sec_sum = sol.solutions[i].secobj - sol.nadir_points[0][1];
		}
		else
		{
			auto pre = i - 1;
			sec_sum = sol.solutions[i].secobj - sol.solutions[pre].secobj;
		}

		auto hyperplan = prim_sum * sec_sum;

		hypervolume_eva += hyperplan;
	}

	hypervolume_eva = hypervolume_eva / area;
}

void algoperform::computeUnaryEpsilon(solution_set& sol)
{
	epsilon = 1;

	for (const auto& r : sol.reference_set)
	{
		//if (r.nondomi)
		//{
		double factor_one_sol = BigM;

		for (const auto& s : sol.solutions)
		{
			/*if ((s.prim_obj >= r.prim_obj) && (s.sec_obj <= r.sec_obj))
			{
				double factor1 = s.prim_obj / r.prim_obj;
				double factor2 = (double)(1 / (s.sec_obj / r.sec_obj));

				double factor = std::max(factor1, factor2);

				if (factor < factor_one_sol)
				{
					factor_one_sol = factor;
				}
			}*/

			double factor1 = s.primobj / r.primobj;
			double factor2 = (double)(1 / (s.secobj / r.secobj));

			double factor = std::max(factor1, factor2);

			if ((factor >= 1) && (factor < factor_one_sol))
			{
				factor_one_sol = factor;
			}
		}

		if ((factor_one_sol != BigM) && (factor_one_sol > epsilon))
		{
			epsilon = factor_one_sol;
		}
		//}
	}
}

double algoperform::calculateEuclideanDistanceToRefSet(solution_set::solution& s, solution_set& sol)
{
	double distance = BigM;

	for (const auto& r : sol.reference_set)
	{
		//if (r.nondomi)
		//{
		double dist_square = std::pow((s.primobj - r.primobj), 2) + std::pow((s.secobj - r.secobj), 2);

		double dist = std::sqrt(dist_square);

		if (dist < distance)
		{
			distance = dist;
		}
		//}
	}

	return distance;
}

double algoperform::calculateEuclideanDistanceInner(solution_set::solution& s, solution_set& sol)
{
	double distance = BigM;

	for (const auto& r : sol.solutions)
	{
		if (r.primobj != s.primobj && r.secobj != s.secobj)
		{
			double dist_square = std::pow((s.primobj - r.primobj), 2) + std::pow((s.secobj - r.secobj), 2);

			double dist = std::sqrt(dist_square);

			if (dist < distance)
			{
				distance = dist;
			}
		}
	}

	return distance;
}

double algoperform::calculateL1Distance(solution_set::solution& s, solution_set& sol)
{
	double distance = BigM;

	for (const auto& r : sol.solutions)
	{
		if (r.primobj != s.primobj && r.secobj != s.secobj)
		{
			double dist_l1 = std::fabs(s.primobj - r.primobj) + std::fabs(s.secobj - r.secobj);

			if (dist_l1 < distance)
			{
				distance = dist_l1;
			}
		}
	}

	return distance;
}

void algoperform::computeConvergenceMetric(solution_set& sol)
{
	euclidean_dist_to_refset.clear();

	double total_dist = 0.0;

	for (auto& s : sol.solutions)
	{
		double dist = calculateEuclideanDistanceToRefSet(s, sol);

		euclidean_dist_to_refset.push_back(dist);

		total_dist += dist;
	}

	average_distance = (double)(total_dist / (double)sol.solutions.size());
}

void algoperform::computeSpacingIndicator(solution_set& sol)
{
	uniform_spacing_indicator = 0.0;

	if (sol.solutions.size() > 1)
	{
		//euclidean_dist_inner.clear();
		l1_distance.clear();

		double total_dist = 0.0;

		for (auto& s : sol.solutions)
		{
			//double dist = calculate_euclidean_distance_inner(s, sol);
			double dist = calculateL1Distance(s, sol);

			//euclidean_dist_inner.push_back(dist);
			l1_distance.push_back(dist);

			total_dist += dist;
		}

		double average = (double)(total_dist / (double)sol.solutions.size());

		total_dist = 0.0;

		for (int i = 0; i < l1_distance.size(); i++)
		{
			//total_dist += average - euclidean_dist_inner[i];
			//total_dist += (average - l1_distance[i]);
			total_dist += std::pow((average - l1_distance[i]), 2);
		}

		double square = total_dist / (double)(sol.solutions.size() - 1);

		uniform_spacing_indicator = std::sqrt(square);
	}
}
