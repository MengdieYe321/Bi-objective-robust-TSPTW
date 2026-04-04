#include"runner.h"

int main(int argc, char* argv[])
{
	/*------------------------- desktop setting ------------------------*/
	runner run = runner();

	/*load the base instance*/
	//std::string filename = paths::input + "instances/" + "rbg010asep.txt";
	std::string filename = paths::input + "base_instances/" + "n60w120.001.txt"; //original instance //argv[1]

	/*----------run algorithmic performance assessment-----------*/	
	run.runAlgoPerformceAssessment(filename, PrimObjType::distance, SecObjType::delta, MethodType::rectangle, "heuristic", "discrete");
	//run.runAlgoPerformceAssessment(filename, PrimObjType::distance, SecObjType::delta, MethodType::rectangle, "heuristic", "uniform");


	/*----------run subinterval generator-----------*/
	//run.runSubintervalGenerator(filename, MethodType::rectangle, "discrete", true);
	//run.runSubintervalGenerator(filename, MethodType::multi_direction, "discrete", true);



	/*----------run solution merge (cost-slack with multiple intervals) manually -----------*/
	/*run.runMergesolutionsManually(filename, MethodType::rectangle, "discrete");
	run.runMergesolutionsManually(filename, MethodType::rectangle, "uniform");
	run.runMergesolutionsManually(filename, MethodType::multi_direction, "discrete");
	run.runMergesolutionsManually(filename, MethodType::multi_direction, "uniform");*/



	/*----------run solution evaluation w.r.t. riskiness indices--------------*/
	/*run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "cost", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "rmeasure", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "eri", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "sri", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "lateProb", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "sum_expLate", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "max_expLate", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "sum_lateProb", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "max_lateProb", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "num_lateNode", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "lateness_avg", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "lateness_wst", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "latenode_avg", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "latenode_wst", "discrete", 0.05);

	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "cost", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "rmeasure", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "eri", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "sri", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "lateProb", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "sum_expLate", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "max_expLate", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "sum_lateProb", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "max_lateProb", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "num_lateNode", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "lateness_avg", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "lateness_wst", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "latenode_avg", "discrete", 0.05);
	run.runSolEvaluationRiskiness(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "latenode_wst", "discrete", 0.05);*/


	/*---------------run solution evaluation w.r.t. cost-reliability tradeoff------------*/ /*test*/
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Vio", "discrete", false); //IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Nvio", "discrete", false);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Wvio", "discrete", false);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Wnvio", "discrete", false);

	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Vio", "uniform", false); //IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Nvio", "uniform", false);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Wvio", "uniform", false);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Wnvio", "uniform", false);

	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Vio", "discrete", false); //IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Nvio", "discrete", false);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Wvio", "discrete", false);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Wnvio", "discrete", false);

	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Vio", "uniform", false); //IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Nvio", "uniform", false);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Wvio", "uniform", false);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Wnvio", "uniform", false);

	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Vio", "discrete", true); //IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Nvio", "discrete", true);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Wvio", "discrete", true);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Wnvio", "discrete", true);

	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Vio", "uniform", true); //IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Nvio", "uniform", true);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Wvio", "uniform", true);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::rectangle, "IC-Wnvio", "uniform", true);

	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Vio", "discrete", true); //IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Nvio", "discrete", true);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Wvio", "discrete", true);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Wnvio", "discrete", true);

	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Vio", "uniform", true); //IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Nvio", "uniform", true);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Wvio", "uniform", true);
	//run.runSolEvaluationTradeoff(filename, PrimObjType::distance, SecObjType::slack, MethodType::multi_direction, "IC-Wnvio", "uniform", true);

	return 0;
}