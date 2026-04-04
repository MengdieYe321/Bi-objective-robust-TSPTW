#pragma once

#include<string>
#include<vector>
#include<array>
#include<climits>
#include<algorithm>
#include<math.h>

#include "config.h"

using namespace parameters;
using namespace constants;

/*
* This header holds all the information of the problem data
*/
class instance
{
public:

	instance(const std::string inputfile, CostType cost_type);
	~instance();

	std::string _inst_name;
	int _num_node;

	/*--- for original instance ---*/
	bool readOriData(std::string input, CostType cost_type);
	auto nodes() const -> const std::vector<int>&;
	auto getOriCost(int i, int j) const ->double;
	auto getOriArcTime(int i, int j) const ->double; /*arc travel time*/
	auto getOriSevTime(int i) const ->double;
	auto getOriTime(int i, int j) const ->double; /*arc travel time (incl. service time)*/
	auto getEarliest(int i) const ->double;
	auto getLatest(int i) const ->double;


	/*---- for scenarios ----*/
	std::vector<std::vector<std::vector<double>>> _cost_scen_insample;
	std::vector<std::vector<std::vector<double>>> _time_scen_insample;
	std::vector<std::vector<std::vector<double>>> _cost_scen_outsample;
	std::vector<std::vector<std::vector<double>>> _time_scen_outsample;

	bool readScenarioData(std::string inputfile, CostType cost_type, int sample_index, bool out_sample);
	auto getInScenTime(int s, int i, int j) const ->double;
	auto getOutScenTime(int s, int i, int j) const ->double;
	auto getInScenCost(int s, int i, int j) const ->double;
	auto getOutScenCost(int s, int i, int j) const ->double;


	/*--- for robust/bi-objective instances ---*/
	std::string _prob_name;
	PrimObjType _primobj_type;
	SecObjType _secobj_type;
	double delta = BigM;
	/*the budget size is not considered here*/

	/*arc structure*/
	struct arc
	{
		arc()
			: i(-1)
			, j(-1)
		{
			//empty
		}

		/* Constructs a new arc with given indices*/
		arc(int i, int j)
			: i(i)
			, j(j)
		{
			// empty
		}

		//! Source node.
		int i, j;
	};

	bool readRobustData(std::string input, CostType cost_type);
	auto getCost(int i, int j) const ->double;
	auto getMinTime(int i, int j) const ->double;
	auto getMaxTime(int i, int j) const ->double;
	auto getMaxDelay(int i, int j) const ->double;
	auto getEarlTW(int i) const->double;
	auto getLatTW(int i) const->double;

	/*the data processing step is not included here*/
	void arcElimination();
	void narrowTimeWindow();
	void floydwarshall();
	void dataPreprocessing();

protected:
	std::vector<int> _nodes;
	std::vector<double> _ori_sev_time;
	std::vector<std::vector<double>> _ori_cost;
	std::vector<std::vector<double>> _ori_pur_time;
	std::vector<std::vector<double>> _ori_time; //original travel time
	std::vector<double> _earl;
	std::vector<double> _lat;

	std::vector<std::vector<double>> _cost;
	std::vector<std::vector<double>> _t_min;
	std::vector<std::vector<double>> _t_max;
	std::vector<std::vector<double>> _max_delay;
	std::vector<double> _earliest;
	std::vector<double> _latest;
	std::vector<std::vector<double>> _shp_c;
	std::vector<std::vector<double>> _shp_t;
};

