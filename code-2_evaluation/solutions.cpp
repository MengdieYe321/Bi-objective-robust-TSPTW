#include"solutions.h"

solution_set::solution_set(instance& inst)
	: name_sols("none")
	, num_sols(0)
	, time(0)
	, name_ref("none")
	, num_sols_ref(0)
	, num_sols_method2(0)
	, _inst(&inst)
{
	//empty
}

solution_set::~solution_set()
{
	//empty
}

void solution_set::reset()
{
	num_sols = 0;
	num_sols_ref = 0;
	num_sols_method2 = 0;

	reference_set.clear();
	solutions_method2.clear();
	nadir_points.clear();
}

bool solution_set::readBiobjSol(std::string input, bool checkobj)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		std::cout << "Error. Bi-objective solution file not found. Terminate." << std::endl;
		return false;
	}

	in >> name_sols >> num_sols;
	solutions.resize(num_sols);

	int num = 0;
	for (int i = 0; i < (num_sols * 2); i++)
	{	
		if (i % 2 == 0)
		{
			num = (int)i / 2;
			in >> solutions[num].primobj >> solutions[num].secobj;
			if (checkobj)
			{
				in >> solutions[num].checkobj;
			}
			solutions[num].nondomi = false;
		}
		else
		{
			solutions[num].tour.resize(_inst->_num_node + 1);//+1
			for (int j = 0; j < _inst->_num_node; j++)
			{
				in >> solutions[num].tour[j];
			}
			solutions[num].tour[_inst->_num_node] = _inst->_num_node;
		}
	}
	in >> time;
	in.close();

	return true;
}

bool solution_set::readMultiBiobjSol(std::string input, bool checkobj, int num_sets)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		std::cout << "Error. Bi-objective solution file not found. Terminate." << std::endl;
		return false;
	}

	in >> name_sols;

	subset_num_sols.resize(num_sets);
	all_solutions.resize(num_sets, std::vector<solution>(0));

	int bmin = 0, bmax = 0;
	int counter = 0;
	bool subinterval_short = false;
	while (counter < num_sets)
	{
		if (!ALL_SOLUTION_MIXED)
		{
			in >> bmin >> bmax;
		}
		
		if (bmin > 300)
		{
			subinterval_short = true;
			break;
		}
		else
		{
			in >> subset_num_sols[counter];
			if (subset_num_sols[counter] > 0)
			{
				all_solutions[counter].resize(subset_num_sols[counter]);

				int num = 0;
				for (int i = 0; i < (subset_num_sols[counter] * 2); i++)
				{
					if (i % 2 == 0)
					{
						num = (int)i / 2;
						in >> all_solutions[counter][num].primobj >> all_solutions[counter][num].secobj;
						if (checkobj)
						{
							in >> all_solutions[counter][num].checkobj;
						}
						all_solutions[counter][num].nondomi = false;
					}
					else
					{
						all_solutions[counter][num].tour.resize(_inst->_num_node + 1);//+1
						for (int j = 0; j < _inst->_num_node; j++)
						{
							in >> all_solutions[counter][num].tour[j];
						}
						all_solutions[counter][num].tour[_inst->_num_node] = _inst->_num_node;
					}
				}
				subset_num_sols[counter] = checkRepeatSolutions(all_solutions[counter]);
			}
			else
			{
				num_subintervals -= 1;
				break;
			}
		}
		counter++;
	}

	if (subinterval_short)
	{
		int num_short = num_sets - counter;
		num_subintervals = num_subintervals - num_short;

		time = bmin;
	}
	else
	{
		in >> time;
	}
	in.close();

	/*delete repeat tours for different subintervals*/
	if (DELETE_SAMETOUR)
	{
		for (int set = 1; set < num_subintervals; ++set)
		{
			int pre_set = 0;
			while (pre_set < set)
			{
				for (auto& sol : all_solutions[pre_set])
				{
					auto compare_tour = sol.tour;

					all_solutions[set].erase(
						std::remove_if(all_solutions[set].begin(), all_solutions[set].end(), [&](const solution_set::solution& list)
							{
								return list.tour == compare_tour;
							}),
						all_solutions[set].end());
				}
				pre_set++;
			}
		}

		for (int set = 0; set < num_subintervals; ++set)
		{
			subset_num_sols[set] = all_solutions[set].size();
		}
	}
	
	return true;
}

bool solution_set::readMultiReferenceSet(std::string input, int num_sets)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		std::cout << "Error. Reference set file / Another solution set file not found." << std::endl;
		return false;
	}

	in >> name_ref;

	if (REFERENCE_EXIT)
	{
		subset_num_sols_ref.resize(num_sets);
		all_solutions_ref.resize(num_sets, std::vector<solution>(0));
	}
	else
	{
		subset_num_sols2.resize(num_sets);
		all_solutions2.resize(num_sets, std::vector<solution>(0));
	}
	
	int bmin = 0, bmax = 0;
	int counter = 0;
	while (counter < num_sets)
	{
		if (REFERENCE_EXIT)
		{
			in >> subset_num_sols_ref[counter];
			all_solutions_ref[counter].resize(subset_num_sols_ref[counter]);

			for (int i = 0; i < subset_num_sols_ref[counter]; i++)
			{
				in >> all_solutions_ref[counter][i].primobj >> all_solutions_ref[counter][i].secobj;
				all_solutions_ref[counter][i].nondomi = true;
			}
		}
		else
		{
			if (!ALL_SOLUTION_MIXED)
			{
				in >> bmin >> bmax; 
			}

			in >> subset_num_sols2[counter];
			all_solutions2[counter].resize(subset_num_sols2[counter]);

			int num = 0;
			for (int i = 0; i < (subset_num_sols2[counter] * 2); i++)
			{
				if (i % 2 == 0)
				{
					num = (int)i / 2;
					in >> all_solutions2[counter][num].primobj >> all_solutions2[counter][num].secobj >> all_solutions2[counter][num].checkobj;
					all_solutions2[counter][num].nondomi = false;
				}
				else
				{
					all_solutions2[counter][num].tour.resize(_inst->_num_node + 1);
					for (int j = 0; j < _inst->_num_node; j++)
					{
						in >> all_solutions2[counter][num].tour[j];
					}
					all_solutions2[counter][num].tour[_inst->_num_node] = _inst->_num_node;
				}
			}
			subset_num_sols2[counter] = checkRepeatSolutions(all_solutions2[counter]);
		}
		counter++;
	}
	in.close();

	/*delete repeat tours for different subintervals*/
	if (DELETE_SAMETOUR)
	{
		if (!REFERENCE_EXIT)
		{
			for (int set = 1; set < num_subintervals; ++set)
			{
				int pre_set = 0;
				while (pre_set < set)
				{
					for (auto& sol : all_solutions2[pre_set])
					{
						auto compare_tour = sol.tour;

						all_solutions2[set].erase(
							std::remove_if(all_solutions2[set].begin(), all_solutions2[set].end(), [&](const solution_set::solution& list)
								{
									return list.tour == compare_tour;
								}),
							all_solutions2[set].end());
					}
					pre_set++;
				}
			}

			for (int set = 0; set < num_subintervals; ++set)
			{
				subset_num_sols2[set] = all_solutions2[set].size();
			}
		}
	}

	return true;
}

void solution_set::generateMultiReferenceSet(int num_sets)
{
	all_solutions_ref.resize(num_sets, std::vector<solution>(0));
	subset_num_sols_ref.resize(num_sets);

	for (int set = 0; set < num_sets; ++set)
	{
		for (int i = 0; i < subset_num_sols[set]; i++)
		{
			all_solutions_ref[set].push_back(all_solutions[set][i]);
		}

		for (int i = 0; i < subset_num_sols2[set]; i++)
		{
			all_solutions_ref[set].push_back(all_solutions2[set][i]);
		}

		if (all_solutions_ref[set].size() > 0)
		{
			/*sort all solutions*/
			std::sort(all_solutions_ref[set].begin(), all_solutions_ref[set].end(), [&](const solution& a, const solution& b)
				{
					if (a.primobj == b.primobj)
					{
						return a.secobj > b.secobj;
					}
					else
					{
						return a.primobj < b.primobj;
					}
				});

			/*delete dominated solutions*/
			std::size_t counter = 0;
			while (counter < all_solutions_ref[set].size() - 1)
			{
				auto current_size = all_solutions_ref[set].size();
				auto cur_primobj = all_solutions_ref[set][counter].primobj;
				auto cur_secobj = all_solutions_ref[set][counter].secobj;

				all_solutions_ref[set].erase(
					std::remove_if(all_solutions_ref[set].begin() + (counter + 1), all_solutions_ref[set].end(), [&](const solution& list)
						{
							if (list.primobj == cur_primobj)
							{
								return list.secobj <= cur_secobj;
							}
							else
							{
								return list.secobj <= cur_secobj;
							}
						}),
					all_solutions_ref[set].end());

				if (current_size == all_solutions_ref[set].size())
				{
					counter++;
				}
			}

			for (auto& r : all_solutions_ref[set])
			{
				r.nondomi = true;
			}

			subset_num_sols_ref[set] = (int)all_solutions_ref[set].size();
		}
	}
}

bool solution_set::readMultipleIntervals(std::string input)
{
	if (!ALL_SOLUTION_MIXED)
	{
		std::ifstream in;
		in.open(input, std::ifstream::in);
		if (!in.is_open())
		{
			std::cout << "Error. Budget subintervals file not found. Terminate." << std::endl;
			return false;
		}

		std::string name;
		in >> name;

		int counter = 0;
		int value = 0;
		while (in >> value)
		{
			counter++;
		}

		num_subintervals = counter / 2;
	}
	else
	{
		num_subintervals = 1;
	}

	return true;
}

bool solution_set::readTSPTWSol(std::string input, int sample_index)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		std::cout << "Error openning TSPTW solution file: " << input << "." << std::endl;
		return false;
	}

	std::string name;
	in >> name;

	in >> cost_tspsol_scenario[sample_index];

	std::vector<int> tour;
	tour.resize(_inst->_num_node + 1);
	for (int i = 0; i < _inst->_num_node; ++i)
	{
		in >> tour[i];
	}
	tour[_inst->_num_node] = _inst->_num_node; // include the destination depot

	in.close();
	return true;
}

bool solution_set::readRTSPTWSol(std::string input, int num)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		std::cout << "Error openning RTSPTW solution file: " << input << "." << std::endl;
		return false;
	}

	std::string name;
	in >> name;

	in >> rtsptw_solutions[num].secobj >> name >> rtsptw_solutions[num].primobj;
	rtsptw_solutions[num].tour.resize(_inst->_num_node+1);
	for (int i = 0; i < _inst->_num_node; ++i)
	{
		in >> rtsptw_solutions[num].tour[i];
	}
	rtsptw_solutions[num].tour[_inst->_num_node] = _inst->_num_node;

	in.close();
	return true;
}

int solution_set::checkRepeatSolutions(std::vector<solution>& sols) //only if evaluating algorithm pperformance
{
	int sols_size = 0;

	/*delete repeat solution values*/
	std::size_t counter = 0;
	while (counter < sols.size() - 1)
	{
		auto current_size = sols.size();

		auto cur_primobj = sols[counter].primobj;
		auto cur_secobj = sols[counter].secobj;
		//auto current_tour = sols[counter].tour;

		sols.erase(std::remove_if(sols.begin() + (counter + 1), sols.end(), [&](const solution& sol)
			{
				return ((sol.primobj == cur_primobj) && (sol.secobj <= cur_secobj)); //&& sol.tour==current_tour
			}), sols.end());

		if (current_size == sols.size())
		{
			counter++;
		}
		sols_size = (int)sols.size();
	}

	return sols_size;
}

bool solution_set::readReferenceSet(std::string input)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		std::cout << "Error. Reference set file / another solution set file not found." << std::endl;
		return false;
	}

	in >> name_ref;

	if (REFERENCE_EXIT)
	{
		in >> num_sols_ref;
		reference_set.resize(num_sols_ref);
		for (int i = 0; i < num_sols_ref; i++)
		{
			in >> reference_set[i].primobj >> reference_set[i].secobj;
			reference_set[i].nondomi = true;
		}
	}
	else
	{
		in >> num_sols_method2;
		solutions_method2.resize(num_sols_method2);

		int num = 0;
		for (int i = 0; i < (num_sols_method2 * 2); i++)
		{
			if (i % 2 == 0)
			{
				num = (int)i / 2;
				in >> solutions_method2[num].primobj >> solutions_method2[num].secobj >> solutions_method2[num].checkobj;
				solutions_method2[num].nondomi = false;
			}
			else
			{
				solutions_method2[num].tour.resize(_inst->_num_node + 1);
				for (int j = 0; j < _inst->_num_node; j++)
				{
					in >> solutions_method2[num].tour[j];
				}
				solutions_method2[num].tour[_inst->_num_node] = _inst->_num_node;
			}
		}

		/*delete repeat solution values*/
		num_sols_method2 = checkRepeatSolutions(solutions_method2);
	}

	in.close();
	return true;
}

void solution_set::normalization()
{
	auto prim_max = nadir_points[0][0];
	auto prim_min = nadir_points[1][0];

	auto sec_max = nadir_points[1][1];
	auto sec_min = nadir_points[0][1];

	/*normalize ref set*/
	for (auto& r : reference_set)
	{
		auto normalized_value_prim = (r.primobj - prim_min) / (prim_max - prim_min);
		auto normalized_value_sec = (r.secobj - sec_min) / (sec_max - sec_min);

		r.primobj = normalized_value_prim;
		r.secobj = normalized_value_sec;
	}

	/*normalize eva set*/
	for (auto& s : solutions)
	{
		auto normalized_value_prim = (s.primobj - prim_min) / (prim_max - prim_min);
		auto normalized_value_sec = (s.secobj - sec_min) / (sec_max - sec_min);

		s.primobj = normalized_value_prim;
		s.secobj = normalized_value_sec;
	}

	/*modify the nadir point*/
	nadir_points[0][0] = 1;
	nadir_points[0][1] = 0;
	nadir_points[1][0] = 0;
	nadir_points[1][1] = 1;
}

void solution_set::solutionSetsRestriction()
{
	/*first get the nadir point from the reference set*/
	getNadirPoint();

	/*only pick subset that corresponds to the reference set*/
	solutions.erase(std::remove_if(solutions.begin(), solutions.end(), [&](const solution& sol)
		{
			return ((sol.primobj > nadir_points[0][0]) || (sol.primobj < nadir_points[1][0]));
		}),
		solutions.end());

	num_sols = (int)solutions.size();

	/*get # non-dominated sols of reference set*/
	if (LIMIT_REFERENCE)
	{
		auto& last_point_ref = reference_set.back();
		auto& last_point_eva = solutions.back();

		if ((last_point_ref.primobj == last_point_eva.primobj) && (last_point_ref.secobj < last_point_eva.secobj))
		{
			last_point_ref.nondomi = false;
		}
	}

	/*only pick subset that corresponds to the reference set*/
	solutions.erase(std::remove_if(solutions.begin(), solutions.end(), [&](const solution& sol)
		{
			return (((sol.primobj == nadir_points[0][0]) && (sol.secobj > nadir_points[1][1])) || ((sol.primobj == nadir_points[1][0]) && (sol.secobj < nadir_points[0][1])));
		}),
		solutions.end());

	/*modify the evaluation set*/
	num_sols = (int)solutions.size();
}

void solution_set::generateReferenceSet()
{
	for (int i = 0; i < num_sols; i++)
	{
		reference_set.push_back(solutions[i]);
	}

	for (int i = 0; i < num_sols_method2; i++)
	{
		reference_set.push_back(solutions_method2[i]);
	}

	/*sort all solutions*/
	std::sort(reference_set.begin(), reference_set.end(), [&](const solution& a, const solution& b)
		{
			if (a.primobj == b.primobj)
			{
				return a.secobj > b.secobj;
			}
			else
			{
				return a.primobj < b.primobj;
			}
		});

	/*delete dominated solutions*/
	std::size_t counter = 0;
	while (counter < reference_set.size() - 1)
	{
		auto current_size = reference_set.size();
		auto cur_primobj = reference_set[counter].primobj;
		auto cur_secobj = reference_set[counter].secobj;

		reference_set.erase(
			std::remove_if(reference_set.begin() + (counter + 1), reference_set.end(), [&](const solution& list)
				{
					if (list.primobj == cur_primobj)
					{
						return list.secobj <= cur_secobj;
					}
					else
					{
						return list.secobj <= cur_secobj;
					}
				}),
			reference_set.end());

		if (current_size == reference_set.size())
		{
			counter++;
		}
	}

	for (auto& r : reference_set)
	{
		r.nondomi = true;
	}

	num_sols_ref = (int)reference_set.size();
}

void solution_set::getNadirPoint()
{
	nadir_points.resize(2, std::vector<double>(2));

	auto prim = reference_set.back().primobj; //cmax
	auto sec = reference_set.front().secobj; //bmin

	nadir_points[0][0] = prim;
	nadir_points[0][1] = sec;

	prim = reference_set.front().primobj;//cmin
	sec = reference_set.back().secobj;//bmax

	nadir_points[1][0] = prim;
	nadir_points[1][1] = sec;
}

void solution_set::resetPerformanceMeasures()
{
	for (auto& sol : solutions)
	{
		sol.eri = 0;
		sol.sri = 0;
		sol.mean_time = 0;
		sol.max_explate = 0;
		sol.max_lateprob = 0;
		sol.lateprob = 0;
		sol.sum_explate = 0;
		sol.sum_lateprob = 0;
		sol.num_latenode = 0;
		sol.sum_experaly = 0;
		sol.sum_earlyprob = 0;
		sol.lateness_avg = 0;
		sol.lateness_wst = 0;
		sol.latenode_avg = 0;
		sol.latenode_wst = 0;

		sol.increased_cost = 0;
		sol.violation_measure = 0;
	}
}

void solution_set::restrictSolutionWithinCostLimit(double max_cost)
{
	/*only pick subset that corresponds to the reference set*/
	solutions.erase(std::remove_if(solutions.begin(), solutions.end(), [&](const solution& sol)
		{
			return (sol.primobj > max_cost);
		}),
		solutions.end());

	num_sols = (int)solutions.size();
}



