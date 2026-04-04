#pragma once

#include "instance.h"

/*NOTE: 
 * solution_set1 only used when reference set is not known
 * if no reference set exists, first generate one and output
 * if the reference set is limited (exact), need to check whether the final solution in R is nondominated or not
 * reference set: exact or the whole set of nondomi sols found by two algorithms
 * solution_set2: approximation set to be evaluated
 */
class solution_set
{
public:

	solution_set(instance& inst);
	~solution_set();

	instance* _inst;
	std::string name_sols;
	int num_sols;
	double time;

	struct solution
	{
		double primobj = BigM;
		double secobj = BigM;
		double checkobj = BigM;
		bool nondomi = false;
		std::vector<int> tour;

		/*for riskiness indices*/
		double eri;
		double sri;
		double mean_time;
		double max_explate;//measures from Zhang et.al
		double max_lateprob;
		double sum_explate;
		double sum_lateprob;
		double lateprob;
		double num_latenode;
		double sum_experaly;
		double sum_earlyprob;
		double lateness_avg;//measures from Bartolini et al.
		double lateness_wst;
		double latenode_avg;
		double latenode_wst;

		/*for cost-robustness tradeoff*/
		double increased_cost;
		double violation_measure;
		//double tw_violation;
		//double num_violation;
		//double worst_increased_cost_vio;
		//double worst_increased_cost_numvio;
		//double worst_tw_violation;
		//double worst_num_violation;
	};

	std::vector<solution> solutions;

	void reset();

	bool readBiobjSol(std::string input, bool checkobj);

	/*--- Algorithmic performance ---*/
	std::string name_ref;
	int num_sols_ref;
	int num_sols_method2;
	std::vector<solution> reference_set;
	std::vector<solution> solutions_method2;
	std::vector<std::vector<double>> nadir_points; /*nadir points for hypervolume 0:(cmax, bmin), 1:(cmin, bmax)*/

	int checkRepeatSolutions(std::vector<solution>& sols);
	bool readReferenceSet(std::string input);
	void normalization();
	void solutionSetsRestriction();
	void generateReferenceSet();
	void getNadirPoint();

	std::vector<int> subset_num_sols;
	std::vector<int> subset_num_sols2;
	std::vector<int> subset_num_sols_ref;
	std::vector<std::vector<solution>> all_solutions;
	std::vector<std::vector<solution>> all_solutions2;
	std::vector<std::vector<solution>> all_solutions_ref;
	int num_subintervals;
	bool readMultipleIntervals(std::string input);
	bool readMultiBiobjSol(std::string input, bool checkobj, int num_sets);
	bool readMultiReferenceSet(std::string input, int num_sets);
	void generateMultiReferenceSet(int num_sets);

	/*--- Riskiness indices ---*/
	void resetPerformanceMeasures();
	void restrictSolutionWithinCostLimit(double max_cost);

	/*--- Solution evaluation ---*/
	std::vector<solution> tradeoff_sols;
	//std::vector<solution> tradeoff_sols_violation;
	//std::vector<solution> tradeoff_sols_numviolation;
	//std::vector<solution> tradeoff_sols_worst_violation;
	//std::vector<solution> tradeoff_sols_worst_numviolation;
	std::vector<double> cost_tspsol_scenario;
	bool readTSPTWSol(std::string input, int sample_index);

	/*--- solution evaluate exact rtsptw ---*/
	std::vector<solution> rtsptw_solutions;
	bool readRTSPTWSol(std::string input, int num);
};

