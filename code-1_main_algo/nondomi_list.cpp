#include"nondomi_list.h"

nondominate_list::nondominate_list()
	: rectangle_active(0)
	, _primobj(BigM)
	, _secobj(BigM)
	, bounds({ BigM, BigM, -BigM , BigM , -BigM , BigM })
	, corner_sols({ BigM, BigM,BigM, BigM })
{
	//empty
	//auto nplus = inst._num_node + 1;
	//_tour.resize(nplus);
	//corner_sols.prim_tour.resize(nplus);
	//corner_sols.sec_tour.resize(nplus);
}

nondominate_list::~nondominate_list()
{
	//empty
}

void nondominate_list::reset()
{
	_primobj = BigM;
	_secobj = BigM;
	bounds = { BigM, BigM, -BigM , BigM , -BigM , BigM };
	corner_sols = { BigM, BigM,BigM, BigM };

	sol_on_list.clear();
}

/*add newly found feasible solution into solution list*/
/*obj1: cost/TD; obj2: budget(-); check_obj: slack(-)*/
auto nondominate_list::add_new_solution(const double obj1, const double obj2, const double check_obj, const vector<int>& tour)->void
{
	if (sol_on_list.size() == 0)
	{
		sol_on_list.push_back({ obj1, obj2, check_obj, tour, false, false });
	}
	else
	{
		std::vector<int> sameobj2_index;

		bool twoobjs_same = false;

		int counter = 0;
		while (counter < sol_on_list.size())/*check whether solution with same prim obj already exists*/
		{
			if (obj1 == sol_on_list[counter].prim)
			{
				if (obj2 == sol_on_list[counter].sec)
				{
					twoobjs_same = true;
					sameobj2_index.push_back(counter);
				}
			}
			counter++;
		}

		if (twoobjs_same)
		{
			bool check_same = false;

			for (int i = 0; i < sameobj2_index.size(); i++)
			{
				auto index = sameobj2_index[i];

				//old dominance: w.r.t. check-obj
				/*if (check_obj == sol_on_list[index].check)
				{
					check_same = true;
					break;
				}*/

				//new dominance: w.r.t. tour
				if (tour == sol_on_list[index].tour)
				{
					check_same = true;
					break;
				}
			}

			if (!check_same)
			{
				sol_on_list.push_back({ obj1, obj2, check_obj, tour, false, false });
			}
		}
		else
		{
			sol_on_list.push_back({ obj1, obj2, check_obj, tour, false, false });
		}
	}

	/*check dominance*/
	update_solution_list();
}

/*update the solution list w.r.t. dominance rules*/
auto nondominate_list::update_solution_list()->void
{
	/*sort the list by increasing order of the primary obj*/
	std::sort(sol_on_list.begin(), sol_on_list.end(), [&](const solution_list& a, const solution_list& b)
		{
			if (a.prim == b.prim)
			{
				if (a.sec == b.sec)
				{
					return a.check < b.check;
				}
				else
				{
					return a.sec < b.sec;
				}
			}
			else
			{
				return a.prim < b.prim;
			}
		});

	/*delete dominated solutions*/
	std::size_t counter = 0;
	while (counter < sol_on_list.size() - 1)
	{
		auto current_size = sol_on_list.size();
		auto cur_primobj = sol_on_list[counter].prim;
		auto cur_secobj = sol_on_list[counter].sec;
		auto cur_checkobj = sol_on_list[counter].check;
		auto cur_tour = sol_on_list[counter].tour;

		if (!TRI_OBJ)
		{
			//old dominance: w.r.t. check-obj value
			/*sol_on_list.erase(
				std::remove_if(sol_on_list.begin() + (counter + 1), sol_on_list.end(), [&](const solution_list& list)
					{
						if (list.prim == cur_primobj)
						{
							if (list.sec == cur_secobj)
							{
								return list.check == cur_checkobj;
							}
							else
							{
								return list.sec > cur_secobj;
							}
						}
						else
						{
							return list.sec >= cur_secobj;
						}
					}),
				sol_on_list.end());*/

			//new dominance: w.r.t. solution tour
			sol_on_list.erase(
				std::remove_if(sol_on_list.begin() + (counter + 1), sol_on_list.end(), [&](const solution_list& list)
					{
						if (list.prim == cur_primobj)
						{
							if (list.sec == cur_secobj)
							{
								return list.tour == cur_tour;
							}
							else
							{
								return list.sec > cur_secobj;
							}
						}
						else
						{
							return list.sec >= cur_secobj;
						}
					}),
				sol_on_list.end());
		}
		else
		{
			sol_on_list.erase(
				std::remove_if(sol_on_list.begin() + (counter + 1), sol_on_list.end(), [&](const solution_list& list)
					{
						if (list.prim == cur_primobj)
						{
							if (list.sec >= cur_secobj)
							{
								return list.check >= cur_checkobj;
							}
							else
							{
								return false;
							}
						}
						else
						{
							if (list.sec >= cur_secobj)
							{
								return list.check >= cur_checkobj;
							}
							else
							{
								return false;
							}
						}
					}),
				sol_on_list.end());
		}

		if (current_size == sol_on_list.size())
		{
			counter++;
		}
	}
}

auto nondominate_list::update_all_solutions()->void
{
	for (const auto& s : sol_on_list)
	{
		all_sols.push_back({ s.prim, s.sec, s.check, s.tour, false, false });
	}

	/*delete repeat solutions -- tour is the same*/
	std::size_t counter = 0;
	while (counter < all_sols.size() - 1)
	{
		auto current_size = all_sols.size();
		auto current_tour = all_sols[counter].tour;

		all_sols.erase(
			std::remove_if(all_sols.begin() + (counter + 1), all_sols.end(), [&](const solution_list& list)
				{
					return list.tour == current_tour;
				}),
			all_sols.end());

		if (current_size == all_sols.size())
		{
			counter++;
		}
	}
}

auto nondominate_list::reset_bounds()->void
{
	bounds.bound = bounds.bound_sec = BigM;
	bounds.lb_primobj = bounds.lb_secobj = -BigM;
	bounds.ub_primobj = bounds.ub_secobj = BigM;
}

auto nondominate_list::is_within_bound(const double obj1, const double obj2, const int optflag)->bool
{
	bool check = true;

	if (optflag)
	{
		if (obj1 <= bounds.bound)
		{
			check = true;
		}
		else
		{
			check = false;
		}
	}
	else
	{
		if (obj2 <= bounds.bound) //real value not minus
		{
			check = true;
		}
		else
		{
			check = false;
		}
	}

	if (rectangle_active && check)/*rectangle active*/
	{
		if ((obj1 >= bounds.lb_primobj) && (obj1 <= bounds.ub_primobj) && (obj2 >= bounds.lb_secobj) && (obj2 <= bounds.ub_secobj))
		{
			check = true;
		}
		else
		{
			check = false;
		}
	}

	return check;
}

auto nondominate_list::is_below_obj2(const double obj2)->bool
{
	if (obj2 <= bounds.bound_sec)
	{
		return true;
	}
	else
	{
		return false;
	}
}
