#include "algorithm.h"

/*this file holds all the functions that are needed for perform the MDLS or BBM*/
algorithm::algorithm()
	//: primobj_opt(1)
	: _num_rectangle(0)
	, start_sol({ BigM, BigM, BigM, false })
	, cur_lb_obj1(-BigM)
	, cur_ub_obj1(BigM)
	, cur_lb_obj2(-BigM)
	, cur_ub_obj2(BigM)
{
	init(100);
}

algorithm::~algorithm()
{
	//empty
}

/*initialize*/
void algorithm::init(int num)
{
	rectangle_list._obj1_left.resize(num);
	rectangle_list._obj1_right.resize(num);
	rectangle_list._obj2_left.resize(num);
	rectangle_list._obj2_right.resize(num);
	rectangle_list._area_of_rec.resize(num);
	rectangle_list._tour_on_left.resize(num);
	rectangle_list._tour_on_right.resize(num);
}

void algorithm::reset()
{
	//primobj_opt = 1;
	_num_rectangle = 0;
	start_sol = { BigM, BigM, BigM, false };
	cur_lb_obj1 = cur_lb_obj2 = -BigM;
	cur_ub_obj1 = cur_ub_obj2 = BigM;

	_unchecked_sol.clear();
	sol_up.clear();
	sol_down.clear();

	init(100);
}

/*------------------------------*/
auto algorithm::unchecked_solution() const -> const vector<nondominate_list::solution_list>&
{
	return _unchecked_sol;
}

/*Get the initial solution list if in MDLS and get the rectangle if in BBM*/
auto algorithm::get_initial_list(const instance& inst, nondominate_list& non, int seed) ->bool
{
	std::cout << endl << "Get inital solution list:" << std::endl;

	tsptw_tph::tsptw_solution best{ inst };

	/*for each one of the objective: perform 1 run of tph heuristic*/
	/*primaryobj*/
	best.OPTprim = 1;
	bool solvable = tph_run(inst, best, non, seed);

	if (!solvable || (best.objective() == BigM && best.objective2() == BigM))
	{
		std::cout << "-CANNOT FIND SOLUTION WITH OBJ ONE." << std::endl;
	}

	/*secondary obj*/
	best.reset_empty();
	best.OPTprim = 0;
	solvable = tph_run(inst, best, non, seed);

	if (!solvable || (best.objective() == BigM && best.objective2() == BigM))
	{
		std::cout << "-CANNOT FIND SOLUTION WITH OBJ TWO." << std::endl;
	}

	if (non.sol_on_list.size() == 0)
	{
		return false;
	}
	
	return true;
}

/*do MDLS sarch starting from a given solution*/
auto algorithm::bidirectional_search(const instance& inst, nondominate_list& non, int seed) ->void
{
	/*represent the selected solution*/
	tsptw_tph::tsptw_solution best(inst);
	tsptw_tph::tsptw_solution sol = best;

	/*do LS for each of the objectives*/
	tsptw_tph::gvns_search::params gvns_params;

	for (const auto& s : sol_down)
	{
		std::cout << "--Starting: " << s.primobj << " / " << s.secobj << "; ";

		best.reset_empty();
		best.OPTprim = 1;
		best.solution_represent(s.tour);

		sol = best;
		sol.OPTprim = 0;
		sol.reset_objective(s.secobj, s.primobj);

		/*set boundaries for objectives*/
		non.bounds.bound = s.primobj;
		tsptw_tph::gvns_search::ls_run(best, gvns_params, non, seed);
		//tsptw_tph::gvns_search::gvns_one_run(best, gvns_params, non, seed);
		std::cout << "Prim opt: " << best.objective() << " / " << best.objective2() << "; ";

		non.bounds.bound = s.secobj;
		tsptw_tph::gvns_search::ls_run(sol, gvns_params, non, seed);
		//tsptw_tph::gvns_search::gvns_one_run(sol, gvns_params, non, seed);
		std::cout << "Sec opt: " << sol.objective() << " / " << sol.objective2() << std::endl;
	}
}

/*update for search procedure after each iteration
1. update unchecked non-dominated solution list
2. select new starting solution*/
auto algorithm::update(nondominate_list& non) ->void
{
	_unchecked_sol.clear();

	for (const auto& s : non.sol_on_list)
	{
		if (!s.checked)
		{
			_unchecked_sol.push_back(s);
		}
	}

	sol_down.clear();

	if (non.rectangle_active)
	{
		sol_up.clear();
	}

	std::cout << "--List: " << non.sol_on_list.size() << " solutions; " << _unchecked_sol.size() << " unchecked." << std::endl;
}

auto algorithm::random_selection(nondominate_list& non, int seed, int index) ->void
{
	if (_unchecked_sol.size() > 0)
	{
		std::mt19937 rng(seed);

		std::shuffle(_unchecked_sol.begin(), _unchecked_sol.end(), rng);

		auto sol = _unchecked_sol.front();

		sol_down.push_back({ sol.prim, sol.sec, sol.check, sol.checked, sol.tour });

		for (auto& s : non.sol_on_list)
		{
			if ((s.prim == sol.prim) && (s.sec == sol.sec) && (s.check == sol.check))
			{
				s.checked = true;
			}
		}
	}
	else
	{
		auto sol = non.sol_on_list[index];

		sol_down.push_back({ sol.prim, sol.sec, sol.check, sol.checked, sol.tour });
	}
}

auto algorithm::in_rectangle_selection(nondominate_list& non, const double left_prim, const double left_sec, const double right_prim, const double right_sec, const double bound) ->bool
{
	bool exist_down = false;
	bool exist_up = false;

	cur_lb_obj1 = cur_lb_obj2 = cur_ub_obj1 = cur_ub_obj2 = 0.0;

	for (int i = 0; i < non.sol_on_list.size(); i++)
	{
		if (non.sol_on_list[i].prim > bound)
		{
			cur_ub_obj1 = non.sol_on_list[i].prim;
			cur_lb_obj2 = non.sol_on_list[i].sec;

			auto prev = i - 1;
			cur_lb_obj1 = non.sol_on_list[prev].prim;
			cur_ub_obj2 = non.sol_on_list[prev].sec;

			break;
		}
	}

	if (cur_lb_obj1 != right_prim && cur_ub_obj2 != right_sec)
	{
		exist_down = true;
	}

	if (cur_ub_obj1 != left_prim && cur_lb_obj2 != left_sec)
	{
		exist_up = true;
	}

	for (auto& s : non.sol_on_list)
	{
		if (exist_down)
		{
			if ((s.prim == cur_lb_obj1) && (s.sec == cur_ub_obj2))
			{
				s.defined = true;

				sol_down.push_back({ s.prim, s.sec, s.check, s.checked, s.tour });
			}
		}
		
		if (exist_up)
		{
			if ((s.prim == cur_ub_obj1) && (s.sec == cur_lb_obj2))
			{
				s.defined = true;

				sol_up.push_back({ s.prim, s.sec, s.check, s.checked, s.tour });
			}
		}
	}

	if (!exist_down && !exist_up)
	{
		return false;
	}
	
	return true;
}


/*run tph to get two one corner solution*/
auto  algorithm::determin_new_sol(const instance& inst, nondominate_list& non, const int upside, int seed) ->void
{
	auto objflag = upside;

	/*bound of second obj*/
	auto bound1 = objflag > 0 ? non.bounds.ub_primobj : non.bounds.ub_secobj;
	auto bound2 = objflag > 0 ? non.bounds.ub_secobj : non.bounds.ub_primobj;
	non.bounds.bound = std::min((double)BigM, bound1);
	non.bounds.bound_sec = std::min(double(BigM), bound2);

	/*solve separately single objective*/
	tsptw_tph::tsptw_solution best(inst);

	best.OPTprim = objflag;

	/*start from corner solution-------*/
	if (!objflag)
	{
		best.solution_represent(non.corner_sols.right_tour);
	}
	else
	{
		best.solution_represent(non.corner_sols.left_tour);
	}
	best.reset_objective(BigM, BigM);

	tsptw_tph::gvns_search::params gvns_params;
	tsptw_tph::gvns_search::gvns_one_run(best, gvns_params, non, seed);
	//tsptw_tph::gvns_search::ls_run(best, gvns_params, non, seed);

	if (best.objective() == BigM && best.objective2() == BigM)
	{
		if (!objflag)
		{
			best.reset_objective(non.corner_sols.right_sec, non.corner_sols.right_prim);
		}
		else
		{
			best.reset_objective(non.corner_sols.left_prim, non.corner_sols.left_sec);
		}
	}

	if (objflag)
	{
		non.corner_sols.right_prim = best.objective(); //right node is minimizing primobj
		non.corner_sols.right_sec = best.objective2(); //keep the minus value
		non.corner_sols.right_tour = best.tour();

		non.bounds.bound = std::min(non.corner_sols.right_sec, non.bounds.ub_secobj);
		non.bounds.bound_sec = std::min(non.corner_sols.right_prim, non.bounds.ub_primobj);
	}
	else
	{
		non.corner_sols.left_prim = best.objective2();
		non.corner_sols.left_sec = best.objective();
		non.corner_sols.left_tour = best.tour();

		non.bounds.bound = std::min(non.corner_sols.left_prim, non.bounds.ub_primobj);
		non.bounds.bound_sec = std::min(non.corner_sols.left_sec, non.bounds.ub_secobj);
	}

	best.reset_objective(best.objective2(), best.objective());
	best.OPTprim = 1 - objflag;

	/*start from corner sol-----*/
	tsptw_tph::gvns_search::gvns_one_run(best, gvns_params, non, seed);
	//tsptw_tph::gvns_search::ls_run(best, gvns_params, non, seed);

	if (objflag)
	{
		non.corner_sols.right_prim = best.objective2();
		non.corner_sols.right_sec = best.objective();
		non.corner_sols.right_tour = best.tour();

		non.bounds.ub_secobj = best.objective2() - Epsilon;
	}
	else
	{
		non.corner_sols.left_prim = best.objective();  //obj1 always means primobj
		non.corner_sols.left_sec = best.objective2();
		non.corner_sols.left_tour = best.tour();

		non.bounds.ub_secobj = best.objective2() - Epsilon;
	}
}

/*update the list of rectangles*/
auto algorithm::store_update_rectangle(nondominate_list& non, binheap& bhp)->bool
{
	/*update rectangle*/
	bool check = true;
	
	if (non.corner_sols.left_prim == BigM || non.corner_sols.right_prim == BigM)
	{
		check = false;
	}

	/*check repeatness of corner solutions*/
	if ((non.corner_sols.left_prim == non.corner_sols.right_prim) && (non.corner_sols.left_sec == non.corner_sols.right_sec))
	{
		check = false;
	}
	
	/*check existance*/
	for (int i = 0; i < _num_rectangle; i++)
	{
		if ((non.corner_sols.left_prim == rectangle_list._obj1_left[i]) && (non.corner_sols.left_sec == rectangle_list._obj2_left[i]))
		{
			if ((non.corner_sols.right_prim == rectangle_list._obj1_right[i]) && (non.corner_sols.right_sec == rectangle_list._obj2_right[i]))
			{
				check = false;
				break;
			}
		}
	}

	if (check)
	{
		rectangle_list._obj1_left[_num_rectangle] = non.corner_sols.left_prim; //zc_l
		rectangle_list._obj1_right[_num_rectangle] = non.corner_sols.right_prim; //zc_r
		rectangle_list._obj2_left[_num_rectangle] = non.corner_sols.left_sec; //zl_l
		rectangle_list._obj2_right[_num_rectangle] = non.corner_sols.right_sec;//zl_r

		rectangle_list._tour_on_left[_num_rectangle] = non.corner_sols.left_tour;
		rectangle_list._tour_on_right[_num_rectangle] = non.corner_sols.right_tour;

		cout << "---New rec: " << _num_rectangle << "(" << rectangle_list._obj2_left[_num_rectangle] << ", " << rectangle_list._obj1_left[_num_rectangle] << ")--(" << rectangle_list._obj2_right[_num_rectangle] << ", " << rectangle_list._obj1_right[_num_rectangle] << ")" << endl;

		/*store the rec area*/
		auto area = -(rectangle_list._obj2_right[_num_rectangle] - rectangle_list._obj2_left[_num_rectangle]) * (rectangle_list._obj1_left[_num_rectangle] - rectangle_list._obj1_right[_num_rectangle]);

		rectangle_list._area_of_rec[_num_rectangle] = area;

		bhp.add(rectangle_list._area_of_rec.data(), _num_rectangle);

		_num_rectangle++;
	}

	non.corner_sols.left_prim = non.corner_sols.right_prim = BigM;
	non.corner_sols.left_sec = non.corner_sols.right_sec = BigM;
	non.corner_sols.left_tour.clear();
	non.corner_sols.right_tour.clear();

	return check;
}

auto algorithm::balanced_box_method(const instance& inst, nondominate_list& non, binheap& bhp, int seed, double TL) ->void
{
	time_t start;
	time_t end;
	time(&start);
	double duration = 0.0;

	/*form initial rectangle-----------*/
	const auto& primsol = non.sol_on_list.front();
	const auto& secsol = non.sol_on_list.back();
	
	non.corner_sols.right_prim = primsol.prim; //right node is minimizing primobj
	non.corner_sols.right_sec = primsol.sec; //keep the minus value
	non.corner_sols.right_tour = primsol.tour;

	non.corner_sols.left_prim = secsol.prim;
	non.corner_sols.left_sec = secsol.sec;
	non.corner_sols.left_tour = secsol.tour;

	_num_rectangle = 0;
	store_update_rectangle(non, bhp); //reset corner sols

	/*select the biggest rectangle---------*/
	int current = bhp.choose(rectangle_list._area_of_rec.data());

	while (current != -1 && (duration <= TL))
	{
		/*check info---------------*/
		cout << "--Current rec: (" << rectangle_list._obj2_left[current] << ", " << rectangle_list._obj1_left[current] << ")--(" << rectangle_list._obj2_right[current] << ", " << rectangle_list._obj1_right[current] << ")" << endl;
		/*-------------------------*/

		/*explore rectangles only if the edge is wider than 1 unit*/
		if (fabs(rectangle_list._obj1_left[current] - rectangle_list._obj1_right[current]) > 1)
		{
			auto left_prim = rectangle_list._obj1_left[current];
			auto left_sec = rectangle_list._obj2_left[current];
			auto left_tour = rectangle_list._tour_on_left[current];

			auto right_prim = rectangle_list._obj1_right[current];
			auto right_sec = rectangle_list._obj2_right[current];
			auto right_tour = rectangle_list._tour_on_right[current];

			auto half_bound = (left_prim + right_prim) / 2;

			/*rectangle exploring*/
			/*1. check whether already exists solutions within rectangle*/
			bool found = in_rectangle_selection(non, left_prim, left_sec, right_prim, right_sec, half_bound);

			if (found)
			{
				/*define new rectangle directly*/
				/*downside*/
				if (sol_down.size() == 0)
				{
					sol_down = sol_up;
				}
				else if (sol_up.size() == 0)
				{
					sol_up = sol_down;
				}
				
				auto& down = sol_down.front();
				non.corner_sols.right_prim = right_prim;
				non.corner_sols.right_sec = right_sec;
				non.corner_sols.right_tour = right_tour;

				non.corner_sols.left_prim = down.primobj;
				non.corner_sols.left_sec = down.secobj;
				non.corner_sols.left_tour = down.tour;

				store_update_rectangle(non, bhp);

				/*upside*/
				auto& up = sol_up.front();
				non.corner_sols.left_prim = left_prim;
				non.corner_sols.left_sec = left_sec;
				non.corner_sols.left_tour = left_tour;

				non.corner_sols.right_prim = up.primobj;
				non.corner_sols.right_sec = up.secobj;
				non.corner_sols.right_tour = up.tour;

				store_update_rectangle(non, bhp);
			}
			else
			{
				/*explore the rectangle to find new solution*/
				/*2. downside ----------*/
				/*2.1 set the bounds*/
				non.bounds.lb_primobj = right_prim;
				non.bounds.ub_primobj = half_bound;
				non.bounds.lb_secobj = left_sec;
				non.bounds.ub_secobj = right_sec;
				cout << "---bound(down): " << non.bounds.lb_primobj << "--" << non.bounds.ub_primobj << ", " << non.bounds.lb_secobj << "--" << non.bounds.ub_secobj << endl;

				/*2.2. search for non-domi solution in downside rectangle*/
				non.corner_sols.right_prim = right_prim;
				non.corner_sols.right_sec = right_sec;
				non.corner_sols.right_tour = right_tour;

				determin_new_sol(inst, non, 0, seed);

				store_update_rectangle(non, bhp);

				/*3. upperside ----------*/
				/*3.1 set bounds*/
				non.bounds.lb_primobj = half_bound;
				non.bounds.ub_primobj = left_prim;
				non.bounds.lb_secobj = left_sec;
				cout << "---bound(up): " << non.bounds.lb_primobj << "--" << non.bounds.ub_primobj << ", " << non.bounds.lb_secobj << "--" << non.bounds.ub_secobj << endl;

				/*3.2. search for new solution in upside rectangle*/
				non.corner_sols.left_prim = left_prim;
				non.corner_sols.left_sec = left_sec;
				non.corner_sols.left_tour = left_tour;

				determin_new_sol(inst, non, 1, seed);

				store_update_rectangle(non, bhp);
			}

			update(non);
		}

		current = bhp.choose(rectangle_list._area_of_rec.data());

		time(&end);
		duration = difftime(end, start);
	}
}

auto algorithm::define_rectangles_from_list(nondominate_list& non, binheap& bhp) ->bool
{
	cout << "--Define rectangles: " << endl;

	auto current_num_rec = _num_rectangle;

	int end_count = 1;

	while (end_count < non.sol_on_list.size())
	{
		auto pre_count = end_count - 1;
		auto& pre = non.sol_on_list[pre_count];
		auto& next = non.sol_on_list[end_count];

		if (!pre.defined || !next.defined) /*if the rectangle is already defined once, then both points are marked as true*/
		{
			pre.defined = next.defined = true;

			if (next.prim - pre.prim > 1)
			{
				non.corner_sols.right_prim = pre.prim;
				non.corner_sols.right_sec = pre.sec;
				non.corner_sols.right_tour = pre.tour;

				non.corner_sols.left_prim = next.prim;
				non.corner_sols.left_sec = next.sec;
				non.corner_sols.left_tour = next.tour;

				store_update_rectangle(non, bhp);
			}
		}

		end_count++;
	}

	if (_num_rectangle > current_num_rec)
	{
		return true;
	}
	else
	{
		return false;
	}
}

auto algorithm::balanced_box_method_predefine(const instance& inst, nondominate_list& non, binheap& bhp, int seed, double TL) ->void
{
	time_t start;
	time_t end;
	time(&start);
	double duration = 0.0;

	/*select the biggest rectangle---------*/
	int current = bhp.choose(rectangle_list._area_of_rec.data());

	while (current != -1 && (duration <= TL))
	{
		/*check info---------------*/
		cout << "---Current rec: (" << rectangle_list._obj2_left[current] << ", " << rectangle_list._obj1_left[current] << ")--(" << rectangle_list._obj2_right[current] << ", " << rectangle_list._obj1_right[current] << ")" << endl;
		/*-------------------------*/

		/*explore rectangles only if the edge is wider than 1 unit*/
		if (fabs(rectangle_list._obj1_left[current] - rectangle_list._obj1_right[current]) > 1)
		{
			auto left_prim = rectangle_list._obj1_left[current];
			auto left_sec = rectangle_list._obj2_left[current];
			auto left_tour = rectangle_list._tour_on_left[current];

			auto right_prim = rectangle_list._obj1_right[current];
			auto right_sec = rectangle_list._obj2_right[current];
			auto right_tour = rectangle_list._tour_on_right[current];

			auto half_bound = (left_prim + right_prim) / 2;

			/*rectangle exploring*/
			/*explore the rectangle to find new solution*/
			/*1. downside ----------*/
			/*1.1 set the bounds*/
			non.bounds.lb_primobj = right_prim;
			non.bounds.ub_primobj = half_bound;
			non.bounds.lb_secobj = left_sec;
			non.bounds.ub_secobj = right_sec;
			cout << "----bound(down): " << non.bounds.lb_primobj << "--" << non.bounds.ub_primobj << ", " << non.bounds.lb_secobj << "--" << non.bounds.ub_secobj << endl;

			/*1.2. search for non-domi solution in downside rectangle*/
			non.corner_sols.right_prim = right_prim;
			non.corner_sols.right_sec = right_sec;
			non.corner_sols.right_tour = right_tour;

			determin_new_sol(inst, non, 0, seed);

			store_update_rectangle(non, bhp);

			time(&end);
			duration = difftime(end, start);
			if (duration > TL)
			{
				break;
			}

			/*2. upperside ----------*/
			/*2.1 set bounds*/
			non.bounds.lb_primobj = half_bound;
			non.bounds.ub_primobj = left_prim;
			non.bounds.lb_secobj = left_sec;
			cout << "----bound(up): " << non.bounds.lb_primobj << "--" << non.bounds.ub_primobj << ", " << non.bounds.lb_secobj << "--" << non.bounds.ub_secobj << endl;

			/*2.2. search for new solution in upside rectangle*/
			non.corner_sols.left_prim = left_prim;
			non.corner_sols.left_sec = left_sec;
			non.corner_sols.left_tour = left_tour;

			determin_new_sol(inst, non, 1, seed);

			store_update_rectangle(non, bhp);
		}

		current = bhp.choose(rectangle_list._area_of_rec.data());

		time(&end);
		duration = difftime(end, start);
	}
}

