#pragma once

#ifndef evaluate_H_
#define evaluate_H_

#include "solutions.h"
#include <cmath>

class evaluate
{
public:

	evaluate();
	~evaluate();

	/*for evaluating the lateness probabilities*/
	void generateRiskIndices(solution_set& sols, bool out_sample);
	int pickBestSolution(solution_set& sols, std::string eva_rule);

	double calculateMeanTime(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateERI(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateNodeERI(std::vector<double>& delay);
	double calculateSRI(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateNodeSRI(std::vector<double>& delay);
	double calculateNodeCVaR(std::vector<double>& delay, double epsilon);	
	double calculateExpLate(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateSumLateProb(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateLateProb(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateMaxExpLate(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateMaxLateProb(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateNodeExpLate(std::vector<double>& delay);
	double calculateNodeLateProb(std::vector<double>& delay);
	double calculateNumLateNode(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateExpEarly(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateEarlyProb(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateWorstTWViolation(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateWorstNumViolation(solution_set& solset, solution_set::solution& sol, bool out);

	/*for evaluating the tradeoff between cost and reliability*/
	void generateTradeOffMeasures(solution_set& sols, std::string tradeoff, bool out_sample);
	void computeNonDominatedMeasures(solution_set& solset, std::vector<solution_set::solution>& sols);
	double calculateTWViolation(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateNumViolation(solution_set& solset, solution_set::solution& sol, bool out);
	double calculateCostIncrease(solution_set& solset, solution_set::solution& sol, bool out);
	auto calculateScenarioCostIncrease(solution_set& solset, solution_set::solution& sol, bool out)->std::vector<double>;
	auto calculateScenarioTWViolation(solution_set& solset, solution_set::solution& sol, bool out) -> std::vector<double>;
	auto calculateScenarioNumViolation(solution_set& solset, solution_set::solution& sol, bool out) -> std::vector<double>;
	void evaluateWorstTWViolation(solution_set& solset, solution_set::solution& sol, bool out);
	void evaluateWorstNumViolation(solution_set& solset, solution_set::solution& sol, bool out);

	std::vector<double> agg_primobj;
	std::vector<double> agg_secobj;
	std::vector<double> agg_checkobj;
	std::vector<double> agg_ic;
	std::vector<double> agg_vio;

	std::vector<double> curve_ic;
	std::vector<double> curve_vio;

	void generateAggregatedMeasures(std::vector<std::vector<solution_set::solution>>& all_sols, double measure_gap);
	void generateTradeoffCureInfo();

	/*modification of the exact tradeoff*/
	void reSortExactTradeoffMeasures(std::string filename);
	std::vector<double> getMeasures(const std::string line);
	void aggregateExactMeasures(std::vector<double>& measure_ic, std::vector<double>& measure_vio, double measure_gap);
	std::vector<double> ic_vio_exact;
	std::vector<double> vio_exact;
	std::vector<double> ic_nvio_exact;
	std::vector<double> nvio_exact;
	std::vector<double> ic_wvio_exact;
	std::vector<double> wvio_exact;
	std::vector<double> ic_wnvio_exact;
	std::vector<double> wnvio_exact;

	std::vector<double> ic_vio_exact_curve;
	std::vector<double> vio_exact_curve;
	std::vector<double> ic_nvio_exact_curve;
	std::vector<double> nvio_exact_curve;
	std::vector<double> ic_wvio_exact_curve;
	std::vector<double> wvio_exact_curve;
	std::vector<double> ic_wnvio_exact_curve;
	std::vector<double> wnvio_exact_curve;

	
};

#endif // !evaluate_H_

