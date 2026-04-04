#pragma once

#include<vector>
#include"instance.h"

using namespace std;

/*This class holds information for interacting between bi- and single-objective algorithms
 * boundary valuesused for guide the search
 * best objectives values returned from the single-obj algorithm
 * primary and secondary objectives are w.r.t. the problem setting
 */

class nondominate_list
{
public:

	nondominate_list();
	~nondominate_list();

	int rectangle_active;

	struct solution_list
	{
		double prim;
		double sec;
		double check;
		vector<int> tour;
		bool checked;
		bool defined;
	};

	vector<solution_list> sol_on_list;
	vector<solution_list> all_sols; // for multiple budget intervals

	struct boundary
	{
		double bound;
		double bound_sec;		//ub_obj2

		double lb_primobj;		//c_lb
		double ub_primobj;		//c_ub
		double lb_secobj;		//l_lb
		double ub_secobj;		//l_ub
	};
	boundary bounds;

	struct corners
	{
		double right_prim;		//cmin_c
		double right_sec;		//cmin_l

		double left_prim;		//lmin_c
		double left_sec;		//lmin_l
		vector<int> right_tour;	//cmin_tour
		vector<int> left_tour;	//lmin_tour
	};
	corners corner_sols;

	void reset();

	auto reset_bounds()->void;

	auto is_within_bound(const double obj1, const double obj2, const int optflag)->bool;

	auto is_below_obj2(const double obj2)->bool;

	auto add_new_solution(const double obj1, const double obj2, const double check_obj, const vector<int>& tour)->void;

	auto update_solution_list()->void;

	auto update_all_solutions()->void;


protected:

	//instance& _inst;

	double _primobj;
	double _secobj;
	vector<int> _tour;
};
