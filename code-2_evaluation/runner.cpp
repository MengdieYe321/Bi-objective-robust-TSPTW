#include "runner.h"

runner::runner()
{
	//empty
}

runner::~runner()
{
	//empty
}

/*calculate the algorithmic performance indicators for the selected algorithm*/
void runner::runAlgoPerformceAssessment(std::string input, PrimObjType prim_obj, SecObjType sec_obj, MethodType meth, std::string reftype, std::string distri)
{
	/*load the original instance file*/
	instance inst(input, CostType::time);

	std::string method = (meth == MethodType::multi_direction) ? "MDLS" : "MBBM";
	std::string other_method = (meth == MethodType::multi_direction) ? "MBBM" : "MDLS";
	std::string objtype = (sec_obj == SecObjType::delta) ? "cost-delta" : "cost-slack";
	std::string interval = "";
	if (sec_obj == SecObjType::slack)
	{
		interval = MULTIPLE_INTERVALS ? "_multi" : "_single";
	}
	std::string distype = (distri == "discrete") ? "dis-" : "uni-";

	//instance-wise
	std::string folder_inst = paths::input + "scenarios/" + inst._inst_name + "/" + distri + "/size_80/robust/";
	std::string folder_results = paths::results_input + "biobj_sols/N60/size_80/" + objtype + interval + "/"; 
	std::string folder_reference = paths::results_input + "reference_sets/N60/size_80/" + reftype + "/" + objtype + interval + "/";
	std::string folder_output = paths::output + "indicators/N60/size_80/" + reftype + "/" + objtype + interval + "/";

	/*output all experiments*/
	std::string filename = folder_output + objtype + "_" + method + "_" + distype + inst._inst_name + ".txt";
	std::ofstream outputall;
	outputall.open(filename, std::ofstream::out);
	outputall.setf(std::ios::fixed, std::ios::floatfield);
	outputall.precision(4);
	outputall << "& Inst & front(r) & I_H(r) & front & NDR & %NDR & %NDR(F) & I_H  & I_H(diff) & I_e & I_c & I_s & time" << std::endl;

	double num_ndr_ref = 0, hyper_ref = 0;
	double num_sol_front = 0, num_ndr_front = 0;
	double percent_ndr = 0, percent_ndrf = 0;
	double hypervolume = 0, hypervolume_diff = 0;
	double epsilon = 0, average_dis = 0, spacing = 0, runtime = 0;

	int exp = 1;
	while (exp <= num_exp)
	{
		solution_set solset(inst);

		/*load the robust instance*/
		std::string expnum = std::to_string(exp);
		std::string biobj_name = distype + inst._inst_name + "_" + expnum;
		filename = folder_inst + biobj_name + ".txt";
		inst.readRobustData(filename, CostType::distance);
		inst.dataPreprocessing();

		/*load the results file to be evaluated*/
		filename = folder_results + objtype + "_" + method + "_" + biobj_name + ".txt";
		solset.readBiobjSol(filename, true);
		std::cout << "Load Biobjective results file " << solset.name_sols << " successfully." << std::endl;

		solset.num_sols = solset.checkRepeatSolutions(solset.solutions);

		/*load the reference set*/
		if (REFERENCE_EXIT)
		{
			filename = folder_reference + biobj_name + ".txt";
			solset.readReferenceSet(filename);
		}
		else
		{
			/*first generate the reference set*/
			filename = folder_results + objtype + "_" + other_method + "_" + biobj_name + ".txt";
			solset.readReferenceSet(filename);

			solset.generateReferenceSet();

			std::ofstream output;
			filename = folder_reference + biobj_name + ".txt";
			output.open(filename, std::ofstream::out);
			output << biobj_name << std::endl << solset.reference_set.size() << std::endl;
			for (const auto& s : solset.reference_set)
			{
				output << s.primobj << " " << s.secobj << std::endl;
			}
			output.close();
		}

		solset.solutionSetsRestriction();

		/*assess the algorithmic performance*/
		algoperform perform = algoperform();
		perform.performanceIndicatorGenerator(solset);

		double ndr_percent = (double)perform.num_nondomi_sol_eva / perform.num_nondomi_sol_ref;
		double ndrf_percent = (double)perform.num_nondomi_sol_eva / solset.num_sols;
		double hyper_diff = perform.hypervolume_eva - perform.hypervolume_ref;

		num_ndr_ref += perform.num_nondomi_sol_ref;
		hyper_ref += perform.hypervolume_ref;
		num_sol_front += solset.num_sols;
		num_ndr_front += perform.num_nondomi_sol_eva;
		percent_ndr += ndr_percent;
		percent_ndrf += ndrf_percent;
		hypervolume += perform.hypervolume_eva;
		hypervolume_diff += hyper_diff;
		epsilon += perform.epsilon;
		average_dis += perform.average_distance;
		spacing += perform.uniform_spacing_indicator;
		runtime += solset.time;

		/*output results separetely*/
		/*filename = folder_output + objtype + "_" + method + "_" + biobj_name + "_test.txt";
		std::ofstream output;
		output.open(filename, std::ofstream::out);
		output.setf(std::ios::fixed, std::ios::floatfield);
		output.precision(4);
		output << "Inst & front(r) & I_H(r) & front & NDR & %NDR & %NDR(F) & I_H  & I_H(diff) & I_e & I_c & I_s & time" << std::endl;
		output << biobj_name << "&" << perform.num_nondomi_sol_ref << "&" << perform.hypervolume_ref << "&" 
			<< solset.num_sols << "&" << perform.num_nondomi_sol_eva << "&" << ndr_percent << "&" << ndrf_percent << "&" << perform.hypervolume_eva << "&" << hyper_diff << "&" << perform.epsilon << "&" << perform.average_distance << "&" << perform.uniform_spacing_indicator << "&" << solset.time << std::endl;
		output.close();*/

		/*output all experiments*/
		outputall << "results & " << biobj_name << "&" << perform.num_nondomi_sol_ref << "&" << perform.hypervolume_ref << "&"
			<< solset.num_sols << "&" << perform.num_nondomi_sol_eva << "&" << ndr_percent << "&" << ndrf_percent << "&" << perform.hypervolume_eva << "&" << hyper_diff << "&" << perform.epsilon << "&" << perform.average_distance << "&" << perform.uniform_spacing_indicator << "&" << solset.time << std::endl;

		++exp;
	}

	/*average of all experiments*/
	num_ndr_ref /= num_exp;
	hyper_ref /= num_exp;
	num_sol_front /= num_exp;
	num_ndr_front /= num_exp;
	percent_ndr /= num_exp;
	percent_ndrf /= num_exp;
	hypervolume /= num_exp;
	hypervolume_diff /= num_exp;
	epsilon /= num_exp;
	average_dis /= num_exp;
	spacing /= num_exp;
	runtime /= num_exp;

	outputall << "---------- aggregated ----------" << std::endl;
	outputall << "Inst & distribution & objtype & method & reftype & front(r) & I_H(r) & front & NDR & %NDR & %NDR(F) & I_H  & I_H(diff) & I_e & I_c & I_s & time" << std::endl;
	outputall << inst._inst_name << " & " << distri << " & " << objtype << " & " << method << " & " << reftype << " & "
		<< num_ndr_ref << " & " << hyper_ref << " & " << num_sol_front << " & " << num_ndr_front << " & " << percent_ndr << " & " << percent_ndrf << " & " << hypervolume << " & " << hypervolume_diff << " & " << epsilon << " & " << average_dis << " & " << spacing << " & " << runtime << std::endl;

	outputall.close();
}

/*calculate the riskiness index of each solution and select one with the smallest value*/
void runner::runSolEvaluationRiskiness(std::string input, PrimObjType prim_obj, SecObjType sec_obj, MethodType meth, std::string eva_rule, std::string distri, double cost_limit)
{
	/*load the original instance file*/
	instance inst(input, CostType::time);
	solution_set sols(inst);

	std::string method = (meth == MethodType::multi_direction) ? "MDLS" : "MBBM";
	std::string objtype = (sec_obj == SecObjType::delta) ? "cost-delta" : "cost-slack";
	std::string interval = "";
	if (sec_obj == SecObjType::slack)
	{
		interval = MULTIPLE_INTERVALS ? "_multi" : "_single";
	}
	std::string distype = (distri == "discrete") ? "dis-" : "uni-";
	std::string size_name = "size_" + std::to_string(num_insample);

	std::string folder_inst = paths::input + "scenarios/" + inst._inst_name + "/" + distri + "/";
	std::string folder_results = paths::results_input + "biobj_sols/N12/" + size_name + "/" + objtype + interval + "/"; // rbg010a
	//std::string folder_results = paths::results_input + "biobj_sols/N60/" + size_name + "/" + objtype + interval + "/";//GDE_n60

	/*load the general out-sample*/
	for (int sam = 0; sam < num_outsample; ++sam)
	{
		std::string nsam = std::to_string((sam + 1));
		std::string filename = folder_inst + "/out_sample/" + inst._inst_name + "_out" + "-" + nsam + ".txt";

		inst._cost_scen_outsample.resize(num_outsample, std::vector<std::vector<double> >(inst._num_node, std::vector<double>(inst._num_node)));
		inst._time_scen_outsample.resize(num_outsample, std::vector<std::vector<double> >(inst._num_node, std::vector<double>(inst._num_node)));
		if (!inst.readScenarioData(filename, CostType::distance, sam, true))
		{
			std::cout << "Error loading out-sample scenario #" << sam + 1 << std::endl;
			return;
		}
	}
	std::cout << "Load all out-sample scenarios successfully." << std::endl;

	/*--- create output files ---*/
	std::string folder_output = paths::output + "eva_riskindex/N12/" + size_name + "/" + objtype + interval + "/";//rbg010a
	//std::string folder_output = paths::output + "eva_riskindex/N60/" + size_name + "/" + objtype + interval + "/";//GDE_n60

	std::ofstream output;// output_detail;
	std::string output_file = folder_output + objtype + "_" + method + "_" + eva_rule + "_" + distype + inst._inst_name + "_in.txt";
	output.open(output_file, std::ofstream::out);
	output << "Exp & cost & delta & slack & Mean_TT & ERI & SRI & Sum_ExpLate & Max_ExpLate & LateProb & Sum_LateProb & Max_LateProb & Num_LateNode & Sum_ExpEarly & Sum_EarlyProb & Lateness_avg & Lateness_wst & Latenode_avg & Latenode_wst & time" << std::endl;

	std::ofstream output_out;// output_detail;
	output_file = folder_output + objtype + "_" + method + "_" + eva_rule +"_" + distype + inst._inst_name + "_out" + ".txt";
	output_out.open(output_file, std::ofstream::out);
	output_out << "Exp & cost & delta & slack & Mean_TT & ERI & SRI & Sum_ExpLate & Max_ExpLate & LateProb & Sum_LateProb & Max_LateProb & Num_LateNode & Sum_ExpEarly & Sum_EarlyProb &  Lateness_avg & Lateness_wst & Latenode_avg & Latenode_wst & time" << std::endl;

	/*--- consider all experiments if more than one has been done*/
	double cost = 0.0, delta = 0.0, slack = 0.0;
	double eri = 0.0, sri = 0.0;
	double mean_time = 0.0;
	double sum_explate = 0.0, max_explate = 0.0;
	double sum_lateprob = 0.0, max_lateprob = 0.0, lateprob = 0.0;
	double num_latenode = 0.0;
	double sum_expearly = 0.0, sum_earlyprob = 0.0;
	double lateness_avg = 0.0, lateness_wst = 0.0;
	double latenode_avg = 0.0, latenode_wst = 0.0;

	double sum_explate_in = 0.0, max_explate_in = 0.0;
	double eri_in = 0.0, sri_in = 0.0;
	double mean_time_in = 0.0;
	double sum_lateprob_in = 0.0, max_lateprob_in = 0.0, lateprob_in = 0.0;
	double num_latenode_in = 0.0;
	double sum_expearly_in = 0.0, sum_earlyprob_in = 0.0;
	double lateness_avg_in = 0.0, lateness_wst_in = 0.0;
	double latenode_avg_in = 0.0, latenode_wst_in = 0.0;

	double runtime = 0;

	for (int exp = 0; exp < num_exp; ++exp)
	{
		std::string nexp = std::to_string((exp + 1));
		
		/*--- load the in-sample scenarios ---*/
		for (int sam = 0; sam < num_insample; ++sam)
		{
			std::string nsam = std::to_string((sam + 1));
			std::string scenario_file = folder_inst + size_name + "/in_sample/" + inst._inst_name + "_" + nexp + "-" + nsam + ".txt";

			inst._cost_scen_insample.resize(num_insample, std::vector<std::vector<double> >(inst._num_node, std::vector<double>(inst._num_node)));
			inst._time_scen_insample.resize(num_insample, std::vector<std::vector<double> >(inst._num_node, std::vector<double>(inst._num_node)));
			if (!inst.readScenarioData(scenario_file, CostType::distance, sam, false))
			{
				std::cout << "Error loading in-sample scenario #" << sam + 1 << std::endl;
				return;
			}
		}
		std::cout << "Load all in-sample scenarios successfully." << std::endl;

		/*--- load the biobj results file ---*/
		std::string result_name = objtype + "_" + method + "_" + distype + inst._inst_name + "_" + nexp;
		std::string results_file =  folder_results + result_name + ".txt";
		sols.readBiobjSol(results_file, true);
		std::cout << "Load Biobjective results file " << sols.name_sols << " successfully." << std::endl;

		/*--- restrict the solution tange if necessary ---*/
		if (cost_limit > 0)
		{
			double min_cost = sols.solutions[0].primobj;
			double max_cost = min_cost * (1 + cost_limit);

			sols.restrictSolutionWithinCostLimit(max_cost);
		}

		/*--- evaluate in-sample scenarios ---*/
		evaluate eva = evaluate();
		eva.generateRiskIndices(sols, false);

		int min_index = eva.pickBestSolution(sols, eva_rule);

		double deltaobj = (sec_obj == SecObjType::delta ? sols.solutions[min_index].secobj : sols.solutions[min_index].checkobj);
		double slackobj = (sec_obj == SecObjType::slack ? sols.solutions[min_index].secobj : sols.solutions[min_index].checkobj);
		std::cout << "Selected solution using " << eva_rule << ": " << std::endl << sols.solutions[min_index].primobj << ", " << deltaobj << ", " << slackobj << std::endl;
		for (auto i : sols.solutions[min_index].tour)
		{
			std::cout << i << " ";
		}

		eri_in += sols.solutions[min_index].eri;
		sri_in += sols.solutions[min_index].sri;
		mean_time_in += sols.solutions[min_index].mean_time;
		sum_explate_in += sols.solutions[min_index].sum_explate;
		sum_lateprob_in += sols.solutions[min_index].sum_lateprob;
		max_explate_in += sols.solutions[min_index].max_explate;
		max_lateprob_in += sols.solutions[min_index].max_lateprob;
		lateprob_in += sols.solutions[min_index].lateprob;
		num_latenode_in += sols.solutions[min_index].num_latenode;
		sum_expearly_in += sols.solutions[min_index].sum_experaly;
		sum_earlyprob_in += sols.solutions[min_index].sum_earlyprob;
		lateness_avg_in += sols.solutions[min_index].lateness_avg;
		lateness_wst_in += sols.solutions[min_index].lateness_wst;
		latenode_avg_in += sols.solutions[min_index].latenode_avg;
		latenode_wst_in += sols.solutions[min_index].latenode_wst;

		std::cout << std::endl << "In-sample performance: mean_tt=" << sols.solutions[min_index].mean_time << "eri=" << sols.solutions[min_index].eri << ", sri=" << sols.solutions[min_index].sri 
			<< ", ExpLate=" << sols.solutions[min_index].sum_explate << ", MaxExpLate=" << sols.solutions[min_index].max_explate
			<< ", LateProb=" << sols.solutions[min_index].lateprob << ", SumLateProb=" << sols.solutions[min_index].sum_lateprob << ", MaxLatProb=" << sols.solutions[min_index].max_lateprob
			<< ", NumLateNode=" << sols.solutions[min_index].num_latenode << ", ExpEarly=" << sols.solutions[min_index].sum_experaly << ", EarlyProb=" << sols.solutions[min_index].sum_earlyprob 
			<< ", Lateness_avg=" << sols.solutions[min_index].lateness_avg << ", Lateness_wst=" << sols.solutions[min_index].lateness_wst << ", Latenode_avg=" << sols.solutions[min_index].latenode_avg << ", Latenode_wst=" << sols.solutions[min_index].latenode_wst << std::endl;
		/*--- output aggregated info -- in-sample ---*/
		output << "Exp-" << nexp << " & " << sols.solutions[min_index].primobj << " & " << deltaobj << " & " << slackobj << " & " << sols.solutions[min_index].mean_time 
			<< " & " << sols.solutions[min_index].eri << " & " << sols.solutions[min_index].sri << " & " << sols.solutions[min_index].sum_explate << " & " << sols.solutions[min_index].max_explate 
			<< " & " << sols.solutions[min_index].lateprob << " & " << sols.solutions[min_index].sum_lateprob << " & " << sols.solutions[min_index].max_lateprob
			<< " & " << sols.solutions[min_index].num_latenode << " & " << sols.solutions[min_index].sum_experaly << " & " << sols.solutions[min_index].sum_earlyprob 
			<< " & " << sols.solutions[min_index].lateness_avg << " & " << sols.solutions[min_index].lateness_wst << " & " << sols.solutions[min_index].latenode_avg << " & " << sols.solutions[min_index].latenode_wst  << " & " << sols.time << std::endl;

		/*evaluate out-sample scenarios*/
		sols.resetPerformanceMeasures();
		eva.generateRiskIndices(sols, true);

		cost += sols.solutions[min_index].primobj;
		delta += deltaobj;
		slack += slackobj;
		eri += sols.solutions[min_index].eri;
		sri += sols.solutions[min_index].sri;
		mean_time += sols.solutions[min_index].mean_time;
		sum_explate += sols.solutions[min_index].sum_explate;
		sum_lateprob += sols.solutions[min_index].sum_lateprob;
		max_explate += sols.solutions[min_index].max_explate;
		max_lateprob += sols.solutions[min_index].max_lateprob;
		lateprob += sols.solutions[min_index].lateprob;
		num_latenode += sols.solutions[min_index].num_latenode;
		sum_expearly += sols.solutions[min_index].sum_experaly;
		sum_earlyprob += sols.solutions[min_index].sum_earlyprob;
		lateness_avg += sols.solutions[min_index].lateness_avg;
		lateness_wst += sols.solutions[min_index].lateness_wst;
		latenode_avg += sols.solutions[min_index].latenode_avg;
		latenode_wst += sols.solutions[min_index].latenode_wst;

		std::cout << "Out-sample perfromance: mean_tt=" << sols.solutions[min_index].mean_time << "eri=" << sols.solutions[min_index].eri << ", sri=" << sols.solutions[min_index].sri 
			<< ", ExpLate=" << sols.solutions[min_index].sum_explate << ", MaxExpLate=" << sols.solutions[min_index].max_explate
			<< ", LateProb=" << sols.solutions[min_index].lateprob << ", SumLateProb=" << sols.solutions[min_index].sum_lateprob << ", MaxLatProb=" << sols.solutions[min_index].max_lateprob
			<< ", NumLateNode=" << sols.solutions[min_index].num_latenode << ", ExpEarly=" << sols.solutions[min_index].sum_experaly << ", EarlyProb=" << sols.solutions[min_index].sum_earlyprob 
			<< ", Lateness_avg=" << sols.solutions[min_index].lateness_avg << ", Lateness_wst=" << sols.solutions[min_index].lateness_wst << ", Latenode_avg=" << sols.solutions[min_index].latenode_avg << ", Latenode_wst=" << sols.solutions[min_index].latenode_wst << std::endl << std::endl;
		/*--- output aggregated info -- out-sample ---*/
		output_out << "Exp-" << nexp << " & " << sols.solutions[min_index].primobj << " & " << deltaobj << " & " << slackobj << " & " << sols.solutions[min_index].mean_time 
			<< " & " << sols.solutions[min_index].eri << " & " << sols.solutions[min_index].sri << " & " << sols.solutions[min_index].sum_explate << " & " << sols.solutions[min_index].max_explate 
			<< " & " << sols.solutions[min_index].lateprob << " & " << sols.solutions[min_index].sum_lateprob << " & " << sols.solutions[min_index].max_lateprob
			<< " & " << sols.solutions[min_index].num_latenode << " & " << sols.solutions[min_index].sum_experaly << " & " << sols.solutions[min_index].sum_earlyprob 
			<< " & " << sols.solutions[min_index].lateness_avg << " & " << sols.solutions[min_index].lateness_wst << " & " << sols.solutions[min_index].latenode_avg << " & " << sols.solutions[min_index].latenode_wst << " & " << sols.time << std::endl;

		runtime += sols.time;

		/*--- output detail file for each experiment ---*/
		/*string output_file_detail = folder_inner + "/" + result_name + "_riskindex" + ".txt";
		output_detail.open(output_file_detail, ofstream::out);
		output_detail << "cost & delta & slack & Mean_TT & ERI & SRI & Sum_ExpLate & Max_ExpLate & Sum_LateProb & Max_LateProb & Num_LateNode & Sum_ExpEarly & Sum_EarlyProb" << endl;
		output_detail << "---- In-sample performance ----" << endl;
		for (auto& sol : sols.solutions)
		{
			output_detail << sol.cost << " & " << sol.delta << " & " << sol.slack << " & " << sol.mean_time_in << " & " << sol.eri_in << " & " << sol.sri_in << " & " << sol.sum_explate_in << " & " << sol.max_explate_in << " & " << sol.sum_lateprob_in
				<<  " & " << sol.max_lateprob_in << " & " << sol.num_latenode_in << " & " << sol.sum_experaly_in << " & " << sol.sum_earlyprob_in << endl;
		}
		output_detail << "---- Out-sample performance ----" << endl;
		for (auto& sol : sols.solutions)
		{
			output_detail << sol.cost << " & " << sol.delta << " & " << sol.slack << " & " << sol.mean_time << " & " << sol.eri << " & " << sol.sri << " & " << sol.sum_explate << " & " << sol.max_explate << " & " << sol.sum_lateprob
				<< " & " << sol.max_lateprob << " & " << sol.num_latenode << " & " << sol.sum_experaly << " & " << sol.sum_earlyprob << endl;
		}
		output_detail << "------------------------------------------" << endl;
		output_detail << "Selected solution using " << eva_rule << ": " << endl;
		output_detail << sols.solutions[min_index].cost << ", " << sols.solutions[min_index].delta << ", " << sols.solutions[min_index].slack << endl;
		for (auto i : sols.solutions[min_index].tour)
		{
			output_detail << i << " ";
		}
		output_detail << endl << result_name << " & in-sample & " << sols.solutions[min_index].mean_time_in << " & " << sols.solutions[min_index].eri_in << " & " << sols.solutions[min_index].sri_in << " & " << sols.solutions[min_index].sum_explate_in << " & " << sols.solutions[min_index].max_explate_in 
			<< " & " << sols.solutions[min_index].sum_lateprob_in << " & " << sols.solutions[min_index].max_lateprob_in << " & " << sols.solutions[min_index].num_latenode_in << " & " << sols.solutions[min_index].sum_experaly_in << " & " << sols.solutions[min_index].sum_earlyprob_in << endl;

		output_detail << result_name << " & out-sample & " << sols.solutions[min_index].mean_time << " & " << sols.solutions[min_index].eri << " & " << sols.solutions[min_index].sri << " & " << sols.solutions[min_index].sum_explate << " & " << sols.solutions[min_index].max_explate 
			<< " & " << sols.solutions[min_index].sum_lateprob << " & " << sols.solutions[min_index].max_lateprob << " & " << sols.solutions[min_index].num_latenode << " & " << sols.solutions[min_index].sum_experaly << " & " << sols.solutions[min_index].sum_earlyprob << endl;
		output_detail.close();*/
	}//end of all experiments

	cost /= num_exp;
	delta /= num_exp;
	slack /= num_exp;
	eri /= num_exp;
	sri /= num_exp;
	mean_time /= num_exp;
	sum_explate /= num_exp;
	sum_lateprob /= num_exp;
	max_explate /= num_exp;
	max_lateprob /= num_exp;
	lateprob /= num_exp;
	num_latenode /= num_exp;
	sum_expearly /= num_exp;
	sum_earlyprob /= num_exp;
	lateness_avg /= num_exp;
	lateness_wst /= num_exp;
	latenode_avg /= num_exp;
	latenode_wst /= num_exp;

	eri_in /= num_exp;
	sri_in /= num_exp;
	mean_time_in /= num_exp;
	sum_explate_in /= num_exp;
	sum_lateprob_in /= num_exp;
	max_explate_in /= num_exp;
	max_lateprob_in /= num_exp;
	lateprob_in /= num_exp;
	num_latenode_in /= num_exp;
	sum_expearly_in /= num_exp;
	sum_earlyprob_in /= num_exp;
	lateness_avg_in /= num_exp;
	lateness_wst_in /= num_exp;
	latenode_avg_in /= num_exp;
	latenode_wst_in /= num_exp;

	runtime /= num_exp;

	output << std::endl << "-----------------------------------------------" << std::endl;
	output << "& name & size & sam_type & objtype & method & eva_rule & cost & delta & slack & Mean_TT & ERI & SRI & Sum_ExpLate & Max_ExpLate & LateProb & Sum_LateProb & Max_LateProb & Num_LateNode & Sum_ExpEarly & Sum_EarlyProb & Lateness_avg & Lateness_wst & Latebnode_avg & Latenode_wst & time" << std::endl;
	output << inst._inst_name << " & " << size_name << " & in & " << objtype << " & " << method << " & " << eva_rule << " & " << cost << " & " << delta << " & " << slack << " & " << mean_time_in << " & " << eri_in << " & " << sri_in
		<< " & " << sum_explate_in << " & " << max_explate_in << " & " << lateprob_in << " & " << sum_lateprob_in << " & " << max_lateprob_in << " & " << num_latenode_in << " & " << sum_expearly_in << " & " << sum_earlyprob_in 
		<< " & " << lateness_avg_in << " & " << lateness_wst_in << " & " << latenode_avg_in << " & " << latenode_wst_in << " & " << runtime << std::endl;
	output.close();
	
	output_out << std::endl << "-----------------------------------------------" << std::endl;
	output_out << "& name & size & sam_tyep & objtype & method & eva_rule & cost & delta & slack & Mean_TT & ERI & SRI & Sum_ExpLate & Max_ExpLate & LateProb & Sum_LateProb & Max_LateProb & Num_LateNode & Sum_ExpEarly & Sum_EarlyProb & Lateness_avg & Lateness_wst & Latebnode_avg & Latenode_wst & time" << std::endl;
	output_out << inst._inst_name << " & " << size_name << " & out & " << objtype << " & " << method << " & " << eva_rule << " & " << cost << " & " << delta << " & " << slack << " & " << mean_time << " & " << eri << " & " << sri
		<< " & " << sum_explate << " & " << max_explate << " & " << lateprob << " & " << sum_lateprob << " & " << max_lateprob << " & " << num_latenode << " & " << sum_expearly << " & " << sum_earlyprob 
		<< " & " << lateness_avg << " & " << lateness_wst << " & " << latenode_avg << " & " << latenode_wst << " & " << runtime << std::endl;
	output_out.close();	
}

/*form the budget sub-intervals using the solutions of BRTSPTW-B*/
void runner::runSubintervalGenerator(std::string input, MethodType meth, std::string distri, bool inclgen)
{
	/*load the original instance file*/
	instance inst(input, CostType::time);

	std::string method = (meth == MethodType::multi_direction) ? "MDLS" : "MBBM";
	std::string objtype = "cost-delta";
	std::string distype = (distri == "discrete") ? "dis-" : "uni-";
	std::string size_name = "size_" + std::to_string(num_insample);

	/*N60*/
	//std::string folder_inst = paths::input + "scenarios/" + inst._inst_name + "/" + distri + "/size_80/robust/";//robust instance
	//std::string folder_biresults = paths::results_input + "biobj_sols/N60/size_80/" + objtype + "/";//bi-objective results
	//std::string folder_output = paths::input + "budget_intervals/N60/size_80/" + method + "/";// target budget subintervals

	/*N12*/
	std::string folder_inst = paths::input + "scenarios/" + inst._inst_name + "/" + distri + "/size_" + std::to_string(num_insample) + "/robust/";//robust instance
	std::string folder_biresults = paths::results_input + "biobj_sols/N12/size_" + std::to_string(num_insample) + "/" + objtype + "/";//bi-objective results
	std::string folder_output = paths::input + "budget_intervals/N12/size_" + std::to_string(num_insample) + "/" + method + "/";// target budget subintervals

	for (int exp = 1; exp <= num_exp; ++exp)
	{
		solution_set solset(inst);

		/*load the robust instance*/
		std::string expnum = std::to_string(exp);
		std::string biobj_name = distype + inst._inst_name + "_" + expnum;
		std::string filename = folder_inst + biobj_name + ".txt";
		inst.readRobustData(filename, CostType::distance);
		inst.dataPreprocessing();

		/*load the results file to be evaluated*/
		filename = folder_biresults + objtype + "_" + method + "_" + biobj_name + ".txt";
		solset.readBiobjSol(filename, true);
		std::cout << "Load Biobjective results file " << solset.name_sols << " successfully." << std::endl;

		/*delete repeat solutions*/
		solset.num_sols = solset.checkRepeatSolutions(solset.solutions);

		/*generate delay budget subintervals and output the results*/
		filename = distype + inst._inst_name + "_" + std::to_string(exp);
		std::ofstream output;
		output.open(folder_output + filename + ".txt", std::ofstream::out);
		output << filename << std::endl;

		if (solset.solutions[0].secobj > 0)
		{
			output << "0 " << solset.solutions[0].secobj << std::endl;
		}

		if (solset.solutions.size() > 1)
		{
			for (int i = 0; i < solset.solutions.size() - 1; ++i)
			{
				output << solset.solutions[i].secobj << " " << solset.solutions[i + 1].secobj << std::endl;
			}
		}

		if (inclgen)
		{
			output << "0 60" << std::endl; // N60 [0,150], N12 [0,60]
		}

		output.close();
	}
}

void runner::runMergesolutionsManually(std::string input, MethodType meth, std::string distribution)
{
	/*load the original instance file*/
	instance inst(input, CostType::time);

	std::string method = (meth == MethodType::multi_direction) ? "MDLS" : "MBBM";
	std::string objtype = "cost-slack";
	std::string distype = (distribution == "discrete") ? "dis-" : "uni-";
	std::string size_name = "size_" + std::to_string(num_insample);

	std::string folder_biresults_slack = paths::results_input + "biobj_sols/N60/" + size_name + "/" + objtype + "_single/";//cost-slack results
	std::string folder_biresults_multi = paths::results_input + "biobj_sols/N60/" + size_name + "/" + objtype + "_multi/";//cost-slack multi-interval results
	std::string folder_output = paths::output + "biobj_sols/N60/" + size_name + "/" + objtype + "_multi/all/";

	for (int exp = 1; exp <= num_exp; ++exp)
	{
		/*load the single cost-slack results*/
		solution_set solset_single(inst);
		std::string expnum = std::to_string(exp);
		std::string biobj_name = distype + inst._inst_name + "_" + expnum;

		std::string filename = folder_biresults_slack + objtype + "_" + method + "_" + biobj_name + ".txt";
		solset_single.readBiobjSol(filename, true);
		std::cout << "Load cost-slack (single) results file " << solset_single.name_sols << " successfully." << std::endl;

		/*load the multi cost-slack results*/
		solution_set solset_multi(inst);
		filename = folder_biresults_multi + objtype + "_" + method + "_" + biobj_name + ".txt";
		solset_multi.readBiobjSol(filename, true);
		std::cout << "Load cost-slack (multi) results file " << solset_multi.name_sols << " successfully." << std::endl;

		/*merge all the solutions -- put the single results at first*/
		std::vector<solution_set::solution> all_solutions;
		
		for (const auto& s : solset_single.solutions)
		{
			all_solutions.push_back({ s.primobj, s.secobj, s.checkobj, false, s.tour});
		}

		for (const auto& s : solset_multi.solutions)
		{
			all_solutions.push_back({ s.primobj, s.secobj, s.checkobj, false, s.tour });
		}

		/*delete repeat solutions -- tour is the same*/
		std::size_t counter = 0;
		while (counter < all_solutions.size() - 1)
		{
			auto current_size = all_solutions.size();
			auto current_tour = all_solutions[counter].tour;

			all_solutions.erase(
				std::remove_if(all_solutions.begin() + (counter + 1), all_solutions.end(), [&](const solution_set::solution& list)
					{
						return list.tour == current_tour;
					}),
				all_solutions.end());

			if (current_size == all_solutions.size())
			{
				counter++;
			}
		}

		/*output the results*/
		filename = objtype + "_" + method + "_" + biobj_name + ".txt";
		std::ofstream output;
		output.open(folder_output + filename, std::ofstream::out);
		output << biobj_name << std::endl;

		output << all_solutions.size() << std::endl;

		for (const auto& s : all_solutions)
		{
			output << s.primobj << " " << s.secobj << " " << s.checkobj << std::endl;
			for (const auto i : s.tour)
			{
				if (i != inst._num_node)
				{
					output << i << " ";
				}
			}
			output << std::endl;
		}
		output << std::endl << solset_single.time + solset_multi.time << " s" << std::endl;

		output.close();
	}
}

/*calculate the cost-reliability tradeoff measures*/
void runner::runSolEvaluationTradeoff(std::string input, PrimObjType prim_obj, SecObjType sec_obj, MethodType meth, std::string tradeoff, std::string distribution, bool out_sample)
{
	/*load the original instance file*/
	instance inst(input, CostType::time);
	solution_set solset(inst);
	evaluate eva = evaluate();

	std::string method = (meth == MethodType::multi_direction) ? "MDLS" : "MBBM";
	std::string objtype = (sec_obj == SecObjType::delta) ? "cost-delta" : "cost-slack";
	std::string distype = (distribution == "discrete") ? "dis-" : "uni-";
	std::string size_name = "size_" + std::to_string(num_insample);
	std::string interval = "";
	std::string test_class = "";
	//std::string tradeoff_folder = "";
	if (sec_obj == SecObjType::slack)
	{
		interval = MULTIPLE_INTERVALS ? "_multi" : "_single";
		test_class = MULTIPLE_INTERVALS ? "all/" : "";
		//tradeoff_folder = MULTIPLE_INTERVALS ? (tradeoff + "/") : "";
	}
	
	std::string sample_folder_name_inst = out_sample ? "out_sample/" : (size_name + "/in_sample/");
	std::string sample_folder_name_result = out_sample ? "out_sample/" : (size_name + "/");
	std::string folder_inst = paths::input + "scenarios/" + inst._inst_name + "/" + distribution + "/" + sample_folder_name_inst;
	std::string folder_tspresults = paths::results_input + "tsptw_sols/" + inst._inst_name + "/" + distribution + "/" + sample_folder_name_result;
	std::string folder_biresults = paths::results_input + "biobj_sols/N60/" + size_name + "/" + objtype + interval + "/" + test_class;

	/*----------------------------------------*/
	/*create output info*/
	std::string sample_folder = out_sample ? "out_sample" : "in_sample";
	std::string folder_output = paths::output + "eva_tradeoff/N60/" + size_name + "/" + objtype + interval + "/" + test_class;
	std::string sample_name = out_sample ? "_out" : "_in";
	std::string new_name = objtype + "_" + method + "_" + tradeoff + "_" + distype + inst._inst_name + sample_name + ".txt";
	/*aggregated info*/
	std::ofstream output; 
	output.open(folder_output + new_name, std::ofstream::out);
	output << "Results of " << inst._inst_name << " with " << size_name << " "<< distribution <<": " << objtype << ", " << method << ", "<< tradeoff << std::endl;

	/*---------------------------------------*/
	/*load the general out-sample scenarios and tsp solutions*/
	if (out_sample)
	{
		for (int sam = 0; sam < num_outsample; ++sam)
		{
			new_name = inst._inst_name + "_out-" + std::to_string((sam + 1));

			/*scenarios*/
			inst._cost_scen_outsample.resize(num_outsample, std::vector<std::vector<double> >(inst._num_node, std::vector<double>(inst._num_node)));
			inst._time_scen_outsample.resize(num_outsample, std::vector<std::vector<double> >(inst._num_node, std::vector<double>(inst._num_node)));
			std::string filename = folder_inst + new_name + ".txt";
			if (!inst.readScenarioData(filename, CostType::distance, sam, true))
			{
				std::cout << "Error loading out-sample scenario #" << sam + 1 << std::endl;
				return;
			}

			/*tsp solutions*/
			solset.cost_tspsol_scenario.resize(num_outsample);

			filename = folder_tspresults + "tph_" + new_name + ".txt"; //GDE
			//filename = folder_tspresults + new_name + ".txt";//rbg
			if (!solset.readTSPTWSol(filename, sam))
			{
				std::cout << "Error loading out-sample TSPTW solution #" << sam + 1 << std::endl;
				return;
			}
		}
		std::cout << "Load all out-sample scenarios and TSPTW solutions successfully." << std::endl;
	}
	
	/*---------------------------------------*/
	/*consider all experiments if more than one has been done*/
	std::vector<std::vector<solution_set::solution>> all_solution_measures;
	for (int exp = 0; exp < num_exp; ++exp)
	{
		/*------------------------*/
		/*load the in-sample scenarios and tsp solutions*/
		if (!out_sample)
		{
			for (int sam = 0; sam < num_insample; ++sam)
			{
				new_name = inst._inst_name + "_" + std::to_string((exp + 1)) + "-" + std::to_string((sam + 1));

				/*scenarios*/
				inst._cost_scen_insample.resize(num_insample, std::vector<std::vector<double> >(inst._num_node, std::vector<double>(inst._num_node)));
				inst._time_scen_insample.resize(num_insample, std::vector<std::vector<double> >(inst._num_node, std::vector<double>(inst._num_node)));
				std::string filename = folder_inst + new_name + ".txt";
				if (!inst.readScenarioData(filename, CostType::distance, sam, false))
				{
					std::cout << "Error loading in-sample scenario #" << sam + 1 << std::endl;
					return;
				}

				/*tsp solutions*/
				solset.cost_tspsol_scenario.resize(num_insample);
				filename = folder_tspresults + "tph_" + new_name + ".txt";//GDE
				//filename = folder_tspresults + new_name + ".txt";//rbg
				if (!solset.readTSPTWSol(filename, sam))
				{
					std::cout << "Error loading in-sample TSPTW solution #" << sam + 1 << std::endl;
					return;
				}
			}
			std::cout << "Load all in-sample scenarios and TSPTW solutions successfully." << std::endl;
		}
		
		/*------------------------*/
		/*load the biobj results file*/
		new_name = objtype + "_" + method + "_" + distype + inst._inst_name + "_" + std::to_string((exp + 1));
		solset.readBiobjSol(folder_biresults + new_name + ".txt", true);
		std::cout << "Load Biobjective results file " << solset.name_sols << " successfully." << std::endl;

		/*evaluate the solutions using all scenarios*/
		eva.generateTradeOffMeasures(solset, tradeoff, out_sample);

		/*for (auto& s : solset.tradeoff_sols)
		{
			std::cout << s.increased_cost << " " << s.violation_measure << std::endl;
		}*/

		/*generate multiple budget intervals for the slack model*/
		/*if (sec_obj == SecObjType::delta)
		{
			folder_output = paths::input + "budget_intervals/rbg010asep/" + size_name + "/0-60/" + sample_folder + "/" + method + "/";
			new_name = distype + inst._inst_name + "_" + std::to_string((exp + 1));

			std::ofstream output_budget;
			output_budget.open(folder_output + new_name + "_" + tradeoff + ".txt", std::ofstream::out);
			output_budget << new_name << std::endl;
			if (solset.tradeoff_sols.size() < 2)
			{
				output_budget << "0 " << solset.tradeoff_sols[0].secobj << std::endl;
			}
			else
			{
				for (int i = 0; i < solset.tradeoff_sols.size() - 1; ++i)
				{
					output_budget << solset.tradeoff_sols[i].secobj << " " << solset.tradeoff_sols[i + 1].secobj << std::endl;
				}
			}
			output_budget.close();
		}*/

		/*-------------------------*/
		/*store the info of all experiments*/
		all_solution_measures.push_back(solset.tradeoff_sols);

		/*-------------------------*/
		/*output the measures of each experiment*/
		output << "Exp-" << (exp + 1) << "& IC & ";
		for (auto& sol : solset.tradeoff_sols)
		{
			output << sol.increased_cost << "&";
		}
		output << std::endl << "&" << tradeoff << "&";
		for (auto& sol : solset.tradeoff_sols)
		{
			output << sol.violation_measure << "&";
		}
		output << std::endl << "&Primobj&";
		for (auto& sol : solset.tradeoff_sols)
		{
			output << sol.primobj << "&";
		}
		output << std::endl << "&Secobj&";
		for (auto& sol : solset.tradeoff_sols)
		{
			output << sol.secobj << "&";
		}
		output << std::endl << "&Checkobj&";
		for (auto& sol : solset.tradeoff_sols)
		{
			output << sol.checkobj << "&";
		}
		output << std::endl << std::endl;
	}

	/*---------------------------------------*/
	/*aggregated info over all experiments*/
	eva.generateAggregatedMeasures(all_solution_measures, cost_increase_unit);

	output << std::endl << "------------------- Aggregated info --------------------" << std::endl;
	output << inst._inst_name << "&" << distribution << "&" << objtype << "&" << method << "&" << tradeoff << "&" << sample_folder << std::endl;
	output << objtype << "&IC&";
	for (auto& c : eva.agg_ic)
	{
		output << c << " &&";
	}
	output << std::endl << "&" << tradeoff << "&";
	for (auto& v : eva.agg_vio)
	{
		output << v << "&&";
	}
	output << std::endl << "&Primobj&";
	for (auto& p : eva.agg_primobj)
	{
		output << p << "&&";
	}
	output << std::endl << "&Secobj&";
	for (auto& s : eva.agg_secobj)
	{
		output << s << "&&";
	}
	output << std::endl << "&Checkobj&";
	for (auto& c : eva.agg_checkobj)
	{
		output << c << "&&";
	}

	/*--- output modified curve info ---*/
	eva.generateTradeoffCureInfo();
	output << std::endl << "&Curve-IC&";
	for (auto& ic : eva.curve_ic)
	{
		output << ic << "&";
	}
	output << std::endl << "&Curve-Vio&";
	for (auto& vio : eva.curve_vio)
	{
		output << vio << "&";
	}

	output.close();
}