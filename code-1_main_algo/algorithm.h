#pragma once

#include "binheap.h"
#include "tsptw.tph/tph_heuristics.h"

/*
** This class holds all information used for store 
** the pareto algorithm in the multi-directional search
** or the rectangel related data structure in balanced box method
*/

class algorithm
{
public:
	algorithm();
	~algorithm();

	//int primobj_opt;

	/*initialize*/
	void init(int num);
	void reset();

	/*functions needed for MDLS*/
	auto unchecked_solution() const -> const vector<nondominate_list::solution_list>&;
	auto get_initial_list(const instance& inst, nondominate_list& non, int seed) ->bool;
	auto bidirectional_search(const instance& inst, nondominate_list& non, int seed) ->void;
	auto update(nondominate_list& non) ->void;
	auto random_selection(nondominate_list& non, int seed, int index) ->void;

	/*functions needed for rectangle method*/
	/*----------------*/
	auto balanced_box_method(const instance& inst, nondominate_list& non, binheap& bhp, int seed, double TL) ->void;
	auto in_rectangle_selection(nondominate_list& non, const double left_p, const double left_s, const double right_p, const double right_s, const double bound) ->bool;
	auto define_rectangles_from_list(nondominate_list& non, binheap& bhp) ->bool;
	auto balanced_box_method_predefine(const instance& inst, nondominate_list& non, binheap& bhp, int seed, double TL) ->void;

	/*----------------*/
	auto determin_new_sol(const instance& inst, nondominate_list& non, const int upside, int seed) ->void;
	auto store_update_rectangle(nondominate_list& non, binheap& bhp)->bool;

protected:

	vector<nondominate_list::solution_list> _unchecked_sol;

	/*starting solution for local search*/
	struct starting_solution
	{
		double primobj;
		double secobj;
		double checkobj;
		bool check;
		vector<int> tour;
	};
	starting_solution start_sol;

	vector<starting_solution> sol_down;
	vector<starting_solution> sol_up;

	/*Balanced box method: rectangles*/
	int _num_rectangle;

	struct rectangle
	{
		vector<double> _obj1_left;		//zc_l
		vector<double> _obj1_right;		//zc_r
		vector<double> _obj2_left;		//zl_l
		vector<double> _obj2_right;		//zl_r
		vector <vector<int>> _tour_on_left;
		vector <vector<int>> _tour_on_right;
		vector<double> _area_of_rec;
	};
	rectangle rectangle_list;

	double cur_lb_obj1;
	double cur_ub_obj1;
	double cur_lb_obj2;
	double cur_ub_obj2;

};
