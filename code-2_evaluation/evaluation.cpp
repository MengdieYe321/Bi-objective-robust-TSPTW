#include "evaluation.h"


evaluate::evaluate()
{
	//empty
}

evaluate::~evaluate()
{
	//empty
}

/*average over all scenarios*/
void evaluate::generateRiskIndices(solution_set& sols, bool out_sample)
{
	for (auto& sol : sols.solutions)
	{
		/*evaluate out-sample*/
		sol.eri = calculateERI(sols, sol, out_sample);
		sol.sri = calculateSRI(sols, sol, out_sample);
		sol.mean_time = calculateMeanTime(sols, sol, out_sample);
		sol.sum_explate = calculateExpLate(sols, sol, out_sample);
		sol.sum_lateprob = calculateSumLateProb(sols, sol, out_sample);
		sol.max_explate = std::max(sol.max_explate, calculateMaxExpLate(sols, sol, out_sample));
		sol.max_lateprob = std::max(sol.max_lateprob, calculateMaxLateProb(sols, sol, out_sample));
		sol.lateprob = calculateLateProb(sols, sol, out_sample);
		sol.num_latenode = calculateNumLateNode(sols, sol, out_sample);
		sol.sum_experaly = calculateExpEarly(sols, sol, out_sample);
		sol.sum_earlyprob = calculateEarlyProb(sols, sol, out_sample);

		sol.lateness_avg = calculateTWViolation(sols, sol, out_sample);
		sol.lateness_wst = calculateWorstTWViolation(sols, sol, out_sample);
		sol.latenode_avg = calculateNumViolation(sols, sol, out_sample);
		sol.latenode_wst = calculateWorstNumViolation(sols, sol, out_sample);
		//cout << "sol: cost=" << sol.cost << ", delta=" << sol.delta << ", slack=" << sol.slack << ", eri=" << sol.eri << ", sri=" << sol.sri << ", ExpLate=" << sol.max_explate << ", LateProb=" << sol.max_lateprob
		//	<< ", MaxExpLate=" << sol.max_explate << ", MaxLatProb=" << sol.max_lateprob << ", Num_LateNode=" << sol.num_latenode << ", ExpEarly=" << sol.sum_experaly << ", EarlyProb=" << sol.sum_earlyprob << endl;
	}
}

int evaluate::pickBestSolution(solution_set& sols, std::string eva_rule)
{
	double min_value = BigM;
	int min_index = 0;
	double max_value2 = 0;

	for (int i = 0; i < sols.solutions.size(); i++)
	{
		double check_value = 0.0;
		double check_value2 = 0.0;
		if (eva_rule ==  "cost")
		{
			check_value = sols.solutions[i].primobj;
		}
		else if (eva_rule == "rmeasure")
		{
			check_value = - sols.solutions[i].secobj;
			check_value2 = sols.solutions[i].checkobj;
		}
		else if (eva_rule == "eri")
		{
			check_value = sols.solutions[i].eri;
		}
		else if (eva_rule == "sri")
		{
			check_value = sols.solutions[i].sri;
		}
		else if (eva_rule == "sum_expLate")
		{
			check_value = sols.solutions[i].sum_explate;
		}
		else if (eva_rule == "max_expLate")
		{
			check_value = sols.solutions[i].max_explate;
		}
		else if (eva_rule == "lateProb")
		{
			check_value = sols.solutions[i].lateprob;
		}
		else if (eva_rule == "sum_lateProb")
		{
			check_value = sols.solutions[i].sum_lateprob;
		}
		else if (eva_rule == "max_lateProb")
		{
			check_value = sols.solutions[i].max_lateprob;
		}
		else if (eva_rule == "num_lateNode")
		{
			check_value = sols.solutions[i].num_latenode;
		}
		else if (eva_rule == "lateness_avg")
		{
			check_value = sols.solutions[i].lateness_avg;
		}
		else if (eva_rule == "lateness_wst")
		{
			check_value = sols.solutions[i].lateness_wst;
		}
		else if (eva_rule == "latenode_avg")
		{
			check_value = sols.solutions[i].latenode_avg;
		}
		else 
		{
			check_value = sols.solutions[i].latenode_wst;
		}
			
		if (check_value < min_value)
		{
			min_value = check_value;
			min_index = i;

			if (eva_rule == "rmeasure")
			{
				max_value2 = check_value2;
			}
		}

		if (eva_rule == "rmeasure")
		{
			if ((check_value == min_value) && (check_value2 > max_value2))
			{
				min_value = check_value;
				min_index = i;
				max_value2 = check_value2;
			}
		}
	}

	return min_index;
}

double evaluate::calculateMeanTime(solution_set& solset, solution_set::solution& sol, bool out)
{
	double time = 0.0;
	int sample_size = out ? num_outsample : num_insample;

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double tour_time = 0.0;

		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			tour_time += travel_time;
		}
		time += tour_time;
	}
	time /= sample_size;

	return time;
}

double evaluate::calculateERI(solution_set& solset, solution_set::solution& sol, bool out)
{
	double eri = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> delay(sol.tour.size(), std::vector<double>(sample_size)); //delay[i][j]: delay of each node i on the tour in j-th scenario

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);

		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			delay[i][sam] = depT - solset._inst->getLatest(curr);
		}
	}

	for (int i = 1; i < sol.tour.size(); ++i)
	{
		bool is_late = false;
		double mean_delay = 0;
		for (int sam = 0; sam < sample_size; ++sam)
		{
			if (delay[i][sam] > 0)
			{
				is_late = true;
			}

			mean_delay += delay[i][sam];
		}

		if (!is_late)
		{
			continue;
		}

		if (mean_delay > 0)
		{
			eri = BigM;
			break;
		}

		double eri_node = calculateNodeERI(delay[i]);
		eri += eri_node;
	}

	return eri;
}

double evaluate::calculateNodeERI(std::vector<double>& delay)
{
	double eri = 0;
	int sample_size = int(delay.size());
	std::sort(delay.begin(), delay.end(), std::greater<double>());//decreasing order

	double sum = 0;
	for (int s = 0; s + 1 < sample_size; ++s)
	{
		sum += delay[s];
		double value = sum / (sample_size - s - 1);

		if (value > eri)
		{
			eri = value;
		}
		else
		{
			return eri;		// because eri is unimodal in s
		}
	}

	return eri;
}

double evaluate::calculateSRI(solution_set& solset, solution_set::solution& sol, bool out)
{
	double sri = 0;
	double wconstant_tmp = 0;
	double radius_tmp = 0;

	if (out)
	{
		wconstant_tmp = wconstant;
		wconstant = 0;
		radius_tmp = radius;
		radius = 0;
	}
	
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> delay(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size();++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			delay[i][sam] = depT - solset._inst->getLatest(curr) + wconstant;
		}
	}

	for (int i = 1; i < sol.tour.size(); ++i)
	{
		double sri_node = calculateNodeSRI(delay[i]);

		if (sri_node >= BigM - Epsilon)
		{
			sri = BigM;
			break;
		}

		sri += sri_node;
	}

	if (out)
	{
		wconstant = wconstant_tmp;
		radius = radius_tmp;
	}

	return sri;
}

double evaluate::calculateNodeSRI(std::vector<double>& delay)
{
	double sri = 0;
	int sample_size = int(delay.size());
	bool is_late = false;

	double mean_delay = 0;
	for (int s = 0; s < sample_size; ++s)
	{
		if (delay[s] > 0)
		{
			is_late = true;
		}

		mean_delay += delay[s];
	}

	if (!is_late)
	{
		return wconstant;
	}

	if (mean_delay > 0)
	{
		return BigM;
	}

	double cvar_rtail = 1 - cvar_ltail;		// the right-tail probability in CVaR
	double cvar = calculateNodeCVaR(delay, cvar_rtail);	// vec is sorted in this function
	if (cvar > Epsilon)		// infeasible
	{
		return BigM;
	}

	double cvar_rtial_idx = cvar_rtail * sample_size;
	int tmp_idx = (int) floor(cvar_rtial_idx);
	double tmp_frac = 1 - tmp_idx / cvar_rtial_idx;

	double sum = 0;
	if (cvar_rtial_idx - (double)tmp_idx < Epsilon) --tmp_idx;	// otherwise divided by 0

	for (int i = 0; i < tmp_idx; ++i)
	{
		sum += delay[i];

		double value = sum / (cvar_rtial_idx - i - 1);

		if (value > sri)
		{
			sri = value;
		}
	}

	return sri + wconstant;
}

double evaluate::calculateNodeCVaR(std::vector<double>& delay, double epsilon)
{
	double cvar = 0;
	int sample_size = int(delay.size());
	std::sort(delay.begin(), delay.end(), std::greater<double>());	// decreasing order

	double pos = (double)sample_size * epsilon;
	if (pos < Epsilon)
	{
		return delay[0];	// the maximum value
	}

	int pos_floor = (int) floor(pos);
	for (int s = 0; s < pos_floor; ++s)
	{
		cvar += delay[s];
	}
	cvar /= pos;

	if (pos_floor < sample_size)
	{
		cvar += (1.0 - (double)pos_floor / pos) * delay[pos_floor];
	}

	return cvar;
}

double evaluate::calculateExpLate(solution_set& solset, solution_set::solution& sol, bool out)
{
	double exp_late = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> delay(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			delay[i][sam] = depT - solset._inst->getLatest(curr);
		}
	}

	for (int i = 1; i < sol.tour.size(); ++i)
	{
		double late = 0;
		for (int sam = 0; sam < sample_size; sam++)
		{
			if (delay[i][sam] > 0)
			{
				late += delay[i][sam];
			}
		}
		exp_late += late;
	}
	exp_late /= sample_size;

	return exp_late;
}

double evaluate::calculateSumLateProb(solution_set& solset, solution_set::solution& sol, bool out)
{
	double late_prob = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> delay(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			delay[i][sam] = depT - solset._inst->getLatest(curr);
		}
	}

	for (int i = 1; i < sol.tour.size(); ++i)
	{
		double late = 0;

		for (int sam = 0; sam < sample_size; ++sam)
		{
			if (delay[i][sam] > 0)
			{
				++late;
			}
		}

		late_prob += late;
	}
	late_prob /= sample_size;

	return late_prob;
}

double evaluate::calculateMaxExpLate(solution_set& solset, solution_set::solution& sol, bool out)
{
	double max_exp_late = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> delay(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; sam++)
	{
		double depT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			delay[i][sam] = depT - solset._inst->getLatest(curr);
		}
	}

	for (int i = 1; i < sol.tour.size();++i)
	{
		max_exp_late = std::max(max_exp_late, calculateNodeExpLate(delay[i]));
	}

	return max_exp_late;
}

double evaluate::calculateNodeExpLate(std::vector<double>& delay)
{
	double expLate = 0;

	int sample_size = int(delay.size());
	for (int s = 0; s < sample_size; ++s)
	{
		if (delay[s] > 0)
		{
			expLate += delay[s];
		}
	}

	expLate /= sample_size;

	return expLate;
}

double evaluate::calculateMaxLateProb(solution_set& solset, solution_set::solution& sol, bool out)
{
	double max_late_prob = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> delay(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);

		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			delay[i][sam] = depT - solset._inst->getLatest(curr);
		}
	}

	for (int i = 1; i < sol.tour.size(); ++i)
	{
		max_late_prob = std::max(max_late_prob, calculateNodeLateProb(delay[i]));
	}

	return max_late_prob;
}

double evaluate::calculateLateProb(solution_set& solset, solution_set::solution& sol, bool out)
{
	double late_prob = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> delay(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			delay[i][sam] = depT - solset._inst->getLatest(curr);
		}
	}

	for (int sam = 0; sam < sample_size; sam++)
	{
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			if (delay[i][sam] > 0)
			{
				late_prob++;
				break;
			}
		}	
	}
	late_prob /= sample_size;

	return late_prob;
}

double evaluate::calculateNodeLateProb(std::vector<double>& delay)
{
	double lateProb = 0.0;

	int sample_size = int(delay.size());

	for (int s = 0; s < sample_size; ++s)
	{
		if (delay[s] > 0)
		{
			lateProb++;
		}
	}

	lateProb /= sample_size;

	return lateProb;
}

double evaluate::calculateNumLateNode(solution_set& solset, solution_set::solution& sol, bool out)
{
	int num_late_nodes = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> delay(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			delay[i][sam] = depT - solset._inst->getLatest(curr);
		}
	}

	for (int i = 1; i < sol.tour.size(); ++i)
	{
		double mean_delay = 0;
		for (int s = 0; s < sample_size; ++s)
		{
			mean_delay += delay[i][s];
		}

		if (mean_delay > 0)
		{
			num_late_nodes++;
		}
	}

	return num_late_nodes;
}

double evaluate::calculateExpEarly(solution_set& solset, solution_set::solution& sol, bool out)
{
	double exp_early = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> early(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double arrT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			arrT += travel_time;
			early[i][sam] = std::max(solset._inst->getEarliest(curr) - arrT, 0.0);
			arrT= std::max(arrT, solset._inst->getEarliest(curr));
		}
	}

	for (int i = 1; i < sol.tour.size(); ++i)
	{
		double earliness = 0;

		for (int s = 0; s < sample_size; ++s)
		{
			if (early[i][s] > 0)
			{
				earliness += early[i][s];
			}
		}
		exp_early += earliness;
	}
	exp_early /= sample_size;

	return exp_early;
}

double evaluate::calculateEarlyProb(solution_set& solset, solution_set::solution& sol, bool out)
{
	double early_prob = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<std::vector<double>> early(sol.tour.size(), std::vector<double>(sample_size));

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double arrT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			arrT += travel_time;
			early[i][sam] = std::max(solset._inst->getEarliest(curr) - arrT, 0.0);
			arrT = std::max(arrT, solset._inst->getEarliest(curr));
		}
	}

	for (int i = 1; i < sol.tour.size(); ++i)
	{
		double earliness = 0;

		for (int s = 0; s < sample_size; ++s)
		{
			if (early[i][s] > 0)
			{
				++earliness;
			}
		}
		early_prob += earliness;
	}
	early_prob /= sample_size;

	return early_prob;
}


/*tradeoff evaluation --- over all scenarios*/
void evaluate::generateTradeOffMeasures(solution_set& solset, std::string tradeoff, bool out_sample)
{
	/*compute the measure for each solution*/
	for (auto& sol : solset.solutions)
	{
		if (tradeoff == "IC-Vio")
		{
			sol.violation_measure = calculateTWViolation(solset, sol, out_sample);
			sol.increased_cost = calculateCostIncrease(solset, sol, out_sample);
		}
		else if (tradeoff == "IC-Nvio")
		{
			sol.violation_measure = calculateNumViolation(solset, sol, out_sample);
			sol.increased_cost = calculateCostIncrease(solset, sol, out_sample);
		}
		else if (tradeoff == "IC-Wvio")
		{
			evaluateWorstTWViolation(solset, sol, out_sample);
		}
		else
		{
			evaluateWorstNumViolation(solset, sol, out_sample);
		}
	}

	/*analysing the measures of the solutions*/
	/*tradeoff cost-violation*/
	solset.tradeoff_sols = solset.solutions;
	computeNonDominatedMeasures(solset, solset.tradeoff_sols);
}

void evaluate::computeNonDominatedMeasures(solution_set& solset, std::vector<solution_set::solution>& sols)
{
	/*sort the solutions by increasing cost*/
	std::sort(sols.begin(), sols.end(),
		[&](const solution_set::solution& a, const solution_set::solution& b)
		{
			if (a.increased_cost == b.increased_cost)
			{
				return a.violation_measure < b.violation_measure;
			}
			else
			{
				return a.increased_cost < b.increased_cost;
			}
		});

	/*eliminate solutions with dominated measures -- i.e., a larger cost increase but a larger violation*/
	std::size_t counter = 0;
	while (counter < sols.size() - 1)
	{
		std::size_t current_size = sols.size();
		double current_cost = 0, current_vio_measure = 0;

		current_cost = sols[counter].increased_cost;
		current_vio_measure = sols[counter].violation_measure;

		sols.erase(
			std::remove_if(sols.begin() + (counter + 1), sols.end(), [&](const solution_set::solution& list)
				{
					return list.violation_measure >= current_vio_measure;
				}),
			sols.end());

		if (current_size == sols.size())
		{
			counter++;
		}
	}
}

auto evaluate::calculateScenarioCostIncrease(solution_set& solset, solution_set::solution& sol, bool out) -> std::vector<double>
{
	int sample_size = out ? num_outsample : num_insample;
	std::vector<double> costup_in_scenarios(sample_size);

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double cost_scenario = solset.cost_tspsol_scenario[sam];
		costup_in_scenarios[sam] = (sol.primobj - cost_scenario) / cost_scenario;
	}

	return costup_in_scenarios;
}

auto evaluate::calculateScenarioTWViolation(solution_set& solset, solution_set::solution& sol, bool out) -> std::vector<double>
{
	int sample_size = out ? num_outsample : num_insample;
	std::vector<double> violation_in_scenarios(sample_size);

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));

			violation_in_scenarios[sam] += std::max(0.0, depT - solset._inst->getLatest(curr));
		}
	}

	return violation_in_scenarios;
}

auto evaluate::calculateScenarioNumViolation(solution_set& solset,  solution_set::solution& sol, bool out) -> std::vector<double>
{
	int sample_size = out ? num_outsample : num_insample;
	std::vector<double> num_violation_in_scenarios(sample_size);

	for (int sam = 0; sam < sample_size; ++sam)
	{
		double depT = solset._inst->getEarliest(0);
		for (int i = 1; i < sol.tour.size(); ++i)
		{
			int curr = sol.tour[i];
			int pred = sol.tour[i - 1];

			double travel_time = out ? solset._inst->getOutScenTime(sam, pred, curr) : solset._inst->getInScenTime(sam, pred, curr);
			depT += travel_time;
			depT = std::max(depT, solset._inst->getEarliest(curr));
			double delay = depT - solset._inst->getLatest(curr);
			if (delay > 0)
			{
				num_violation_in_scenarios[sam]++;
			}
		}
	}

	return num_violation_in_scenarios;
}

double evaluate::calculateTWViolation(solution_set& solset, solution_set::solution& sol, bool out)
{
	double tw_violation = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<double> violation(sample_size);

	violation = calculateScenarioTWViolation(solset, sol, out);

	for (int s = 0; s < sample_size; ++s)
	{
		if (violation[s] > 0)
		{
			tw_violation += violation[s];
		}
	}
	tw_violation /= sample_size;

	return tw_violation;
}

double evaluate::calculateNumViolation(solution_set& solset, solution_set::solution& sol, bool out)
{
	double num_violation = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<double> violation_nodes(sample_size);

	violation_nodes = calculateScenarioNumViolation(solset, sol, out);

	for (int s = 0; s < sample_size; ++s)
	{
		num_violation += violation_nodes[s];
	}
	num_violation /= sample_size;

	return num_violation;
}

double evaluate::calculateCostIncrease(solution_set& solset, solution_set::solution& sol, bool out)
{
	double cost_increase = 0;
	int sample_size = out ? num_outsample : num_insample;
	std::vector<double> costs_up(sample_size);

	costs_up = calculateScenarioCostIncrease(solset, sol, out);

	for (int sam = 0; sam < sample_size; ++sam)
	{
		cost_increase += costs_up[sam];
	}
	cost_increase /= sample_size;

	//limit to 3 digits
	//cost_increase = round(cost_increase * 10000) / 10000;

	return cost_increase;
}

double evaluate::calculateWorstTWViolation(solution_set& solset, solution_set::solution& sol, bool out)
{
	double worst_vio = 0;

	int sample_size = out ? num_outsample : num_insample;
	int worst_size = (int)(sample_size * worst_perventage);
	std::vector<double> violation(sample_size);

	violation = calculateScenarioTWViolation(solset, sol, out);

	/*sort according the desceding order of the violation*/
	std::sort(violation.begin(), violation.end(),
		[](double a, double b)
		{
			return a > b;
		});

	violation.resize(worst_size);

	for (int s = 0; s < worst_size; ++s)
	{
		worst_vio += violation[s];
	}
	worst_vio /= worst_size;

	return worst_vio;
}

double evaluate::calculateWorstNumViolation(solution_set& solset, solution_set::solution& sol, bool out)
{
	double worst_numvio = 0;

	int sample_size = out ? num_outsample : num_insample;
	int worst_size = (int)(sample_size * worst_perventage);
	std::vector<double> num_violation(sample_size);

	num_violation = calculateScenarioNumViolation(solset, sol, out);

	/*sort according the desceding order of the violation*/
	std::sort(num_violation.begin(), num_violation.end(),
		[](double a, double b)
		{
			return a > b;
		});

	num_violation.resize(worst_size);

	for (int s = 0; s < worst_size; ++s)
	{
		worst_numvio += num_violation[s];
	}

	worst_numvio /= worst_size;

	return worst_numvio;
}

void evaluate::evaluateWorstTWViolation(solution_set& solset, solution_set::solution& sol, bool out)
{
	double worst_costs = 0, worst_vio = 0;

	int sample_size = out ? num_outsample : num_insample;
	int worst_size = (int)(sample_size * worst_perventage);
	std::vector<double> costs_up(sample_size);
	std::vector<double> violation(sample_size);

	costs_up = calculateScenarioCostIncrease(solset, sol, out);
	violation = calculateScenarioTWViolation(solset, sol, out);

	/*pair the two vectors*/
	std::vector<std::pair<double, double>> worst_violation(sample_size);
	for (int s = 0; s < sample_size; ++s)
	{
		worst_violation[s].first = costs_up[s];
		worst_violation[s].second = violation[s];
	}

	/*sort according the desceding order of the violation*/
	std::sort(worst_violation.begin(), worst_violation.end(),
		[&](const std::pair<double, double>& a, const std::pair<double, double>& b)
		{
			return a.second > b.second;
		});

	worst_violation.resize(worst_size);

	for (int s = 0; s < worst_size; ++s)
	{
		worst_costs += worst_violation[s].first;
		worst_vio += worst_violation[s].second;
	}
	worst_costs /= worst_size;
	worst_vio /= worst_size;

	// limit to 3 digits
	//worst_costs = round(worst_costs * 10000) / 10000;

	sol.increased_cost = worst_costs;
	sol.violation_measure = worst_vio;
}

void evaluate::evaluateWorstNumViolation(solution_set& solset, solution_set::solution& sol, bool out)
{
	double worst_costs = 0, worst_numvio = 0;

	int sample_size = out ? num_outsample : num_insample;
	int worst_size = (int)(sample_size * worst_perventage);
	std::vector<double> costs_up(sample_size);
	std::vector<double> num_violation(sample_size);

	costs_up = calculateScenarioCostIncrease(solset, sol, out);
	num_violation = calculateScenarioNumViolation(solset, sol, out);

	/*pair the two vectors*/
	std::vector<std::pair<double, double>> worst_numviolation(sample_size);
	for (int s = 0; s < sample_size; ++s)
	{
		worst_numviolation[s].first = costs_up[s];
		worst_numviolation[s].second = num_violation[s];
	}

	/*sort according the desceding order of the violation*/
	std::sort(worst_numviolation.begin(), worst_numviolation.end(),
		[&](const std::pair<double, double>& a, std::pair<double, double>& b)
		{
			return a.second > b.second;
		});

	worst_numviolation.resize(worst_size);

	for (int s = 0; s < worst_size; ++s)
	{
		worst_costs += worst_numviolation[s].first;
		worst_numvio += worst_numviolation[s].second;
	}
	worst_costs /= worst_size;
	worst_numvio /= worst_size;

	// limit to 3 digits
	//worst_costs = round(worst_costs * 10000) / 10000;

	sol.increased_cost = worst_costs;
	sol.violation_measure = worst_numvio;
}

void evaluate::generateAggregatedMeasures(std::vector<std::vector<solution_set::solution>>& all_sols, double measure_gap)
{
	double multiplier = (int)(1 / measure_gap);

	/*determine the min and max cost increase over all experiments*/
	double min_ic = 1.0, max_ic = 0.0;
	for (auto& sols : all_sols)
	{
		if (sols.front().increased_cost < min_ic)
		{
			min_ic = sols.front().increased_cost;
		}

		if (sols.back().increased_cost > max_ic)
		{
			max_ic = sols.back().increased_cost;
		}
	}

	int size_measures = (int) (std::ceil(max_ic * multiplier) - std::floor(min_ic * multiplier));

	std::vector<std::vector<double>> primobj_agg(num_exp, std::vector<double>(size_measures));
	std::vector<std::vector<double>> secobj_agg(num_exp, std::vector<double>(size_measures));
	std::vector<std::vector<double>> checkobj_agg(num_exp, std::vector<double>(size_measures));
	std::vector<std::vector<double>> cost_up_agg(num_exp, std::vector<double>(size_measures));
	std::vector<std::vector<double>> violation_agg(num_exp, std::vector<double>(size_measures));
	std::vector<double> sum_prim(size_measures);
	std::vector<double> sum_sec(size_measures);
	std::vector<double> sum_check(size_measures);
	std::vector<double> sum_ic(size_measures);
	std::vector<double> sum_vio(size_measures);

	for (int exp = 0; exp < num_exp; ++exp)
	{
		double lower_bound = std::floor(min_ic * multiplier) / multiplier;
		double upper_bound = lower_bound + measure_gap;

		double curr_ic = all_sols[exp][0].increased_cost;
		double curr_vio = all_sols[exp][0].violation_measure;
		double curr_prim = all_sols[exp][0].primobj;
		double curr_sec = all_sols[exp][0].secobj;
		double curr_check = all_sols[exp][0].checkobj;

		int index_agg = 0; // used to count the index of the aggregated measures: index_agg >= index
		while (index_agg < size_measures)
		{
			double possible_ic = 0, possible_vio = 0, possible_prim = 0, possible_sec = 0, possible_check = 0;
			int counter = 0;
			for (int index = 0; index < all_sols[exp].size(); ++index)
			{
				if ((all_sols[exp][index].increased_cost >= lower_bound) && (all_sols[exp][index].increased_cost < upper_bound))
				{
					possible_ic += all_sols[exp][index].increased_cost;
					possible_vio += all_sols[exp][index].violation_measure;
					possible_prim += all_sols[exp][index].primobj;
					possible_sec += all_sols[exp][index].secobj;
					possible_check += all_sols[exp][index].checkobj;
					counter++;
				}
			}
			cost_up_agg[exp][index_agg] = counter > 0 ? (possible_ic / counter) : lower_bound;
			violation_agg[exp][index_agg] = counter > 0 ? (possible_vio / counter) : curr_vio;
			primobj_agg[exp][index_agg] = counter > 0 ? (possible_prim / counter) : curr_prim;
			secobj_agg[exp][index_agg] = counter > 0 ? (possible_sec / counter) : curr_sec;
			checkobj_agg[exp][index_agg] = counter > 0 ? (possible_check / counter) : curr_check;

			curr_ic = cost_up_agg[exp][index_agg];
			curr_vio = violation_agg[exp][index_agg];
			curr_prim = primobj_agg[exp][index_agg];
			curr_sec = secobj_agg[exp][index_agg];
			curr_check = checkobj_agg[exp][index_agg];

			sum_ic[index_agg] += cost_up_agg[exp][index_agg];
			sum_vio[index_agg] += violation_agg[exp][index_agg];
			sum_prim[index_agg] += primobj_agg[exp][index_agg];
			sum_sec[index_agg] += secobj_agg[exp][index_agg];
			sum_check[index_agg] += checkobj_agg[exp][index_agg];

			lower_bound = upper_bound;
			upper_bound = lower_bound + measure_gap;
			index_agg++;
		}
	}

	for (int i = 0; i < size_measures; ++i)
	{
		//limit to 3 digits
		double avg_ic = (sum_ic[i] / num_exp);
		//avg_ic = round(avg_ic * 100) / 100;

		agg_ic.push_back(avg_ic);//((sum_ic[i] / num_exp));
		agg_vio.push_back((sum_vio[i] / num_exp));
		agg_primobj.push_back((sum_prim[i] / num_exp));
		agg_secobj.push_back((sum_sec[i] / num_exp));
		agg_checkobj.push_back((sum_check[i] / num_exp));	
	}
}

void evaluate::generateTradeoffCureInfo()
{
	//generate atiffical node that creats a curve (cost goes back, TW goes forward)
	for (int i = 0; i < agg_ic.size(); ++i)
	{
		curve_ic.push_back(agg_ic[i]);
		if (i < agg_ic.size() - 1)
		{
			curve_ic.push_back(agg_ic[i + 1]);
		}
	}

	curve_vio.push_back(agg_vio[0]);
	for (int i = 1; i < agg_vio.size(); ++i)
	{
		curve_vio.push_back(agg_vio[i - 1]);
		curve_vio.push_back(agg_vio[i]);
	}
}

void evaluate::reSortExactTradeoffMeasures(std::string input)
{
	const auto content = paths::read_from_file(input);
	const auto lines = paths::read_lines(content);
	size_t num_lines = lines.size();

	std::vector<double> ic_vio;
	std::vector<double> vio;
	std::vector<double> ic_nvio;
	std::vector<double> nvio;
	std::vector<double> ic_wvio;
	std::vector<double> wvio;
	std::vector<double> ic_wnvio;
	std::vector<double> wnvio;

	int num = 0;
	ic_vio = getMeasures(lines[num++]);
	vio = getMeasures(lines[num++]);
	ic_nvio = getMeasures(lines[num++]);
	nvio = getMeasures(lines[num++]);
	ic_wvio = getMeasures(lines[num++]);
	wvio = getMeasures(lines[num++]);
	ic_wnvio = getMeasures(lines[num++]);
	wnvio = getMeasures(lines[num]);

	/*generate info according to cost gap*/
	aggregateExactMeasures(ic_vio, vio, cost_increase_unit);
	ic_vio_exact = agg_ic;
	vio_exact = agg_vio;
	generateTradeoffCureInfo();
	ic_vio_exact_curve = curve_ic;
	vio_exact_curve = curve_vio;

	agg_ic.clear();
	agg_vio.clear();
	curve_ic.clear();
	curve_vio.clear();
	aggregateExactMeasures(ic_nvio, nvio, cost_increase_unit);
	ic_nvio_exact = agg_ic;
	nvio_exact = agg_vio;
	generateTradeoffCureInfo();
	ic_nvio_exact_curve = curve_ic;
	nvio_exact_curve = curve_vio;

	agg_ic.clear();
	agg_vio.clear();
	curve_ic.clear();
	curve_vio.clear();
	aggregateExactMeasures(ic_wvio, wvio, cost_increase_unit);
	ic_wvio_exact = agg_ic;
	wvio_exact = agg_vio;
	generateTradeoffCureInfo();
	ic_wvio_exact_curve = curve_ic;
	wvio_exact_curve = curve_vio;

	agg_ic.clear();
	agg_vio.clear();
	curve_ic.clear();
	curve_vio.clear();
	aggregateExactMeasures(ic_wnvio, wnvio, cost_increase_unit);
	ic_wnvio_exact = agg_ic;
	wnvio_exact = agg_vio;
	generateTradeoffCureInfo();
	ic_wnvio_exact_curve = curve_ic;
	wnvio_exact_curve = curve_vio;
}

std::vector<double> evaluate::getMeasures(const std::string line)
{
	std::vector<double> measure;

	auto token = paths::tokenize(line, '&');
	for (int t = 1; t < token.size() - 1; ++t) //the first is legend
	{
		measure.push_back(std::stod(token[t]));
	}

	return measure;
}

void evaluate::aggregateExactMeasures(std::vector<double>& measure_ic, std::vector<double>& measure_vio, double measure_gap)
{
	/*delete repeat measures*/
	/*pair the two vectors*/
	std::vector<std::pair<double, double>> measures(measure_ic.size());
	for (int s = 0; s < measure_ic.size(); ++s)
	{
		measures[s].first = measure_ic[s];
		measures[s].second = measure_vio[s];
	}

	std::size_t counter = 0;
	while (counter < measures.size() - 1)
	{
		std::size_t current_size = measures.size();
		double curr_ic = measures[counter].first;
		double curr_vio = measures[counter].second;

		measures.erase(
			std::remove_if(measures.begin() + (counter + 1), measures.end(), [&](std::pair<double, double>& list)
				{
					return (list.first == curr_ic && list.second == curr_vio);
				}),
			measures.end());

		if (current_size == measures.size())
		{
			counter++;
		}
	}

	measure_ic.resize(measures.size());
	measure_vio.resize(measures.size());
	for (int i = 0; i < measures.size(); ++i)
	{
		measure_ic[i] = measures[i].first;
		measure_vio[i] = measures[i].second;
	}

	double multiplier = (int)(1 / measure_gap);
	double min_ic = measure_ic.front();
	double max_ic = measure_ic.back();
	int size_measures = (int)(std::ceil(max_ic * multiplier) - std::floor(min_ic * multiplier));

	std::vector<double> cost_up_agg(size_measures);
	std::vector<double> violation_agg(size_measures);

	double lower_bound = std::floor(min_ic * multiplier) / multiplier;
	double upper_bound = lower_bound + measure_gap;
	double curr_ic = measure_ic[0];
	double curr_vio = measure_vio[0];

	int index_agg = 0; // used to count the index of the aggregated measures: index_agg >= index
	while (index_agg < size_measures)
	{
		double possible_ic = 0, possible_vio = 0;
		int counter = 0;
		for (int index = 0; index < measure_ic.size(); ++index)
		{
			if ((measure_ic[index] >= lower_bound) && (measure_ic[index] < upper_bound))
			{
				possible_ic += measure_ic[index];
				possible_vio += measure_vio[index];

				counter++;
			}
		}
		cost_up_agg[index_agg] = counter > 0 ? (possible_ic / counter) : lower_bound;
		violation_agg[index_agg] = counter > 0 ? (possible_vio / counter) : curr_vio;

		curr_ic = cost_up_agg[index_agg];
		curr_vio = violation_agg[index_agg];

		lower_bound = upper_bound;
		upper_bound = lower_bound + measure_gap;
		index_agg++;
	}

	for (int i = 0; i < size_measures; ++i)
	{
		agg_ic.push_back(cost_up_agg[i]);
		agg_vio.push_back(violation_agg[i]);
	}
}