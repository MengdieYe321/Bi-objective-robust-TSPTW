#pragma once

#include "solutions.h"

class algoperform
{
public:

	algoperform();
	~algoperform();

	int num_nondomi_sol_ref;
	int num_nondomi_sol_eva;

	/*hypervolume: bigger better*/
	double hypervolume_ref;
	double hypervolume_eva;

	/*unary mulplicative epsilon >= 1: smaller better*/
	double epsilon;

	std::vector<double> euclidean_dist_to_refset;
	std::vector<double> euclidean_dist_inner;
	std::vector<double> l1_distance;

	double average_distance; //best value=0, smaller better
	double uniform_spacing_indicator;//smaller better, requires at least two solutions 

	
	void performanceIndicatorGenerator(solution_set& solset);
	void countNondominatedSolutions(solution_set& solset);
	void computeHypervolume(solution_set& solset);
	void computeUnaryEpsilon(solution_set& solset);
	void computeConvergenceMetric(solution_set& solset);
	void computeSpacingIndicator(solution_set& solset);

	double calculateEuclideanDistanceToRefSet(solution_set::solution& s, solution_set& sol);
	double calculateEuclideanDistanceInner(solution_set::solution& s, solution_set& sol);
	double calculateL1Distance(solution_set::solution& s, solution_set& sol);
};
