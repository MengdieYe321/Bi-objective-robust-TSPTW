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

	std::string _prob_name;
	int _num_node; //incl. end depot n+1

	double delta;
	double delta_min;
	double delta_max;
	std::vector<double> delta_min_values;
	std::vector<double> delta_max_values;

	PrimObjType _primobj_type;
	SecObjType _secobj_type;

	instance(const std::string inputfile, PrimObjType obj1, SecObjType obj2);
	~instance();

	bool read_data(std::string input);
	auto read_budget_intervals(std::string input) ->bool;

	/*return / set budget interval values*/
	//auto budget_min() const -> double;
	//auto budget_max() const -> double;
	//auto set_budget_min(double min) -> void;
	//auto set_budget_max(double max) -> void;

	/*return the set of nodes of the instance (incl. depot)*/
	auto nodes() const -> const std::vector<int>&;

	/*--- matrices ---*/
	auto cost(int i, int j) const ->double;

	auto min_time(int i, int j) const ->double;

	auto max_time(int i, int j) const ->double;

	auto maxdelay(int i, int j) const ->double;

	auto min_sev_time(int i) const ->double;

	auto max_sev_time(int i) const ->double;

	auto shp_min_time(int i, int j) const ->double;

	auto shp_min_cost(int i, int j) const ->double;

	auto earliest(int i) const ->double;

	auto latest(int i) const ->double;

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

	/*tour structure*/
	struct tour
	{
		/*define subtours of a tour*/
		struct subtour
		{
			subtour()
			{
				// empty
			}

			subtour(int i)
				: first{ i }
				, last{ i }
			{
				// empty
			}

			subtour(int i, int j)
				: first{ i }
				, last{ j }
			{
				// empty
			}

			auto is_node() const
			{
				return first == last;
			}

			int first = -1;
			int last = -1;
		};

		template <std::size_t N>
		using subtours = std::array<subtour, N>;

		tour()
		{
			//empty
		};
	};

	/*--- preprocessing ---*/
	auto is_arc_feasible(const arc& a) const ->bool;

	void arc_eliminate();

	void timewindow();

	void floydwarshall();

	void data_preprocessing();

protected:

	std::vector<int> _nodes;

	std::vector<std::vector<double>> _cost;
	std::vector<std::vector<double>> _t_min;
	std::vector<std::vector<double>> _t_max;
	//std::vector<double> _sev_t_min;
	//std::vector<double> _sev_t_max;
	std::vector<std::vector<double>> _max_delay;
	std::vector<double> _earl;
	std::vector<double> _lat;
	std::vector<std::vector<double>> _shp_t;
	std::vector<std::vector<double>> _shp_c;
};

