#include "runner.h"

runner::runner()
	: runtime(0.0)
{
	//empty
}

runner::~runner()
{
	//empty
}

/*Run bi-objective models 
* two algorithms -- MDLS or MBBM
*/
void runner::runBiObjRTSPTW(std::string filename, PrimObjType prim_obj, SecObjType sec_obj, MethodType method, double time_limit)
{
	/*load the instance*/
	instance inst(filename, prim_obj, sec_obj);
	std::cout << "Solve bi-objective " << prim_obj << "-" << sec_obj << " using " << method << " for instance " << inst._prob_name << std::endl;
	nondominate_list non;
	algorithm algo;

	time_t start, end;
	std::time(&start);
	runtime = 0.0;
	double time_MDLS = 300;
	double time_BBM = time_limit - time_MDLS;

	/*output info*/
	//std::string folder_name;// = paths::output + "N60/discrete/size_" + std::to_string(num_insample) + "/0-60";
	std::string folder_name = paths::output;
	/*if (!std::filesystem::is_directory(folder_name) || !std::filesystem::exists(folder_name))
	{
		std::filesystem::create_directories(folder_name);
	}*/
	
	std::string prim_name = prim_obj == PrimObjType::distance ? "cost" : "TD";
	std::string sec_name = sec_obj == SecObjType::delta ? "delta" : "slack";
	std::string obj_name = prim_name + "-" + sec_name;
	std::string method_name = method == MethodType::multi_direction ? "MDLS" : "MBBM";
	std::string outputfilename = folder_name + "/" + obj_name + "_" + method_name + "_" + inst._prob_name + ".txt";
	
	if (!MULTIPLE_INTERVALS)
	{
		if (method == MethodType::multi_direction)/*run MDLS*/
		{
			non.rectangle_active = 0;
			runMDLS(inst, non, algo, 1, 500, time_limit);
		}
		else/*run MBBM*/
		{
			non.rectangle_active = 1;
			runMBBM(inst, non, algo, 1, 250, time_MDLS, time_BBM);
		}
		std::time(&end);
		runtime = difftime(end, start);

		non.update_all_solutions();
	}
	else 
	{
		std::vector<double> obj1;
		std::vector<double> obj2;
		std::vector<double> check_obj;
		std::vector<std::vector<int>> tours;

		folder_name = folder_name + "/all";
		if (!std::filesystem::is_directory(folder_name) || !std::filesystem::exists(folder_name))
		{
			std::filesystem::create_directory(folder_name);
		}
		std::string outputfilename_all = folder_name + "/" + obj_name + "_" + method_name + "_" + inst._prob_name + "_all.txt";
		ofstream output;
		output.open(outputfilename_all, ofstream::out);
		output << inst._prob_name << std::endl;

		for (int i = 0; i < inst.delta_min_values.size(); i++)
		{
			inst.delta_min = inst.delta_min_values[i];
			inst.delta_max = inst.delta_max_values[i];

			algo.reset();
			non.reset();

			/* --- run the algo ---*/
			if (method == MethodType::multi_direction)
			{
				non.rectangle_active = 0;
				runMDLS(inst, non, algo, 1, 500, time_limit);
			}
			else
			{
				non.rectangle_active = 1;
				runMBBM(inst, non, algo, 1, 250, time_MDLS, time_BBM);
			}

			non.update_all_solutions();

			output << inst.delta_min << " " << inst.delta_max << std::endl << non.sol_on_list.size() << std::endl;
			if (non.sol_on_list.size() > 0)
			{
				for (const auto& s : non.sol_on_list)/*cost / budget / slack*/
				{
					output << s.prim << " " << (0.0 - s.sec) << " " << (0.0 - s.check) << std::endl;
					for (const auto i : s.tour)
					{
						if (i != inst._num_node)
						{
							output << i << " ";
						}
					}
					output << std::endl;
				}
				output << std::endl;
			}		
		}
		std::time(&end);
		runtime = difftime(end, start);
		output << std::endl << runtime << " s" << std::endl;
		output.close();
	}

	output_to_file(outputfilename, inst, non);
}

void runner::output_to_file(std::string filename, instance& inst, nondominate_list& non)
{
	ofstream output;
	output.open(filename, ofstream::out);

	output << inst._prob_name << std::endl;
	/*if (!MULTIPLE_INTERVALS)
	{
		output << inst.delta_min << " " << inst.delta_max << std::endl;
	}
	else
	{
		output << inst.delta_min_values.size() << "intervals" << std::endl;
	}*/
	output << non.all_sols.size() << std::endl;

	for (const auto& s : non.all_sols)
	{/*cost / budget / slack*/
		output << s.prim << " " << (0.0 - s.sec) << " " << (0.0 - s.check) << std::endl;
		for (const auto i : s.tour)
		{
			if (i != inst._num_node)
			{
				output << i << " ";
			}
		}
		output << std::endl;
	}
	output << std::endl << runtime << " s" << std::endl;

	/*output << std::endl << "----------info------------" << std::endl << "table&";
	output << non.sol_on_list.size() << "&";
	for (const auto& s : non.sol_on_list)
	{
		output << s.prim << " " << (0.0 - s.sec) << "&";
	}
	output << runtime << std::endl;*/

	output.close();
}

void runner::runMDLS(const instance& inst, nondominate_list& non, algorithm& algo, int seed, int num_iter, double time_limit)
{
	time_t start, end;
	std::time(&start);
	double runtime = 0.0;

	non.reset_bounds();

	bool initial_list = algo.get_initial_list(inst,non, seed);
	if (initial_list || (non.sol_on_list.size() > 0))
	{
		std::cout << "Initial list: " << non.sol_on_list.size() << " solutions." << std::endl << std::endl << "Repeat MDLS iteratively:" << std::endl;

		algo.update(non);

		int iter = 1;
		while ((iter <= num_iter) && (runtime <= time_limit))
		{
			std::cout << "-Iter " << iter << ": " << std::endl;

			auto index = iter % (int)non.sol_on_list.size();
			algo.random_selection(non, iter, index);
			algo.bidirectional_search(inst, non, iter);
			algo.update(non);

			std::time(&end);
			runtime = difftime(end, start);
			iter++;
		}
	}
	else
	{
		std::cout << "TERMINATE: FAIR TO GET INITIAL SOLUTION IN THIS RUN." << std::endl;
	}
}

void runner::runMBBM(const instance& inst, nondominate_list& non, algorithm& algo, int seed, int num_iter, double time_MDLS, double time_BBM)
{
	time_t start, end, start_BBM;
	std::time(&start);
	double runtime = 0.0, runtime_BBM = 0.0;

	binheap bhp(Nmax);
	non.reset_bounds();

	bool initial_list = algo.get_initial_list(inst, non, seed);
	if (initial_list || (non.sol_on_list.size() > 0))
	{
		std::cout << "Initial list: " << non.sol_on_list.size() << " solutions." << std::endl << std::endl << "Repeat MDLS iteratively:" << std::endl;

		algo.update(non);

		int iter = 1;
		while ((iter <= num_iter) && (runtime <= time_MDLS))
		{
			std::cout << "-Iter " << iter << ": " << std::endl;

			auto index = iter % (int)non.sol_on_list.size();
			algo.random_selection(non, iter, index);
			algo.bidirectional_search(inst, non, iter);
			algo.update(non);

			std::time(&end);
			runtime = difftime(end, start);
			iter++;
		}

		/*Do MBBM*/
		std::cout << std::endl << "Improve using MBBM:" << std::endl;
		std::time(&start_BBM);

		non.reset_bounds();

		/*form rectangels using the exiting solutions from list*/
		/*-----------*/
		//fron.balanced_box_method(non, bhp, random_seed, time_limit_BBM);

		iter = 1;
		while ((runtime_BBM <= time_BBM) && (runtime <= time_MDLS + time_BBM))
		{
			std::cout << "-Iter " << iter << ": " << std::endl;

			if (algo.define_rectangles_from_list(non, bhp))
			{
				std::cout << "--Explore rectangles iteratively: " << std::endl;
				algo.balanced_box_method_predefine(inst, non, bhp, seed, time_BBM);
				iter++;
			}
			else
			{
				std::cout << "---No new rectangles." << std::endl;
				break;
			}

			std::time(&end);
			runtime_BBM = difftime(end, start_BBM);
			runtime = difftime(end, start);
		}

		if (runtime < time_MDLS + time_BBM)
		{
			std::cout << std::endl << "Check remaining unchecked solutions:" << std::endl;
			if (algo.unchecked_solution().size() == 0)
			{
				std::cout << "-No unchecked solution on the list." << std::endl;
			}
			else
			{
				iter = 1;
				while (algo.unchecked_solution().size() > 0 && (runtime <= time_MDLS + time_BBM))
				{
					std::cout << "--Iter: " << iter << ": " << std::endl;

					auto index = iter % (int)non.sol_on_list.size();
					algo.random_selection(non, iter, index);
					algo.bidirectional_search(inst, non, iter);
					algo.update(non);

					iter++;

					std::time(&end);
					runtime = difftime(end, start);
				}
			}
		}
	}
	else
	{
		std::cout << "TERMINATE: FAIR TO GET INITIAL SOLUTION IN THIS RUN." << std::endl;
	}
}
