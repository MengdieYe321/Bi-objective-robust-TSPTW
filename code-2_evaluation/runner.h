#pragma once

#ifndef runner_H_
#define runner_H_

#include "performance.h"
#include "evaluation.h"

class runner
{
public:
	runner();
	~runner();

	void runAlgoPerformceAssessment(std::string filename, PrimObjType prim_obj, SecObjType sec_obj, MethodType method, std::string reftype, std::string distribution);
	void runSolEvaluationRiskiness(std::string filename, PrimObjType prim_obj, SecObjType sec_obj, MethodType meth, std::string eva_rule, std::string distribution, double cost_limit);
	void runSubintervalGenerator(std::string input, MethodType meth, std::string distribution, bool inclgen);
	void runSolEvaluationTradeoff(std::string filename, PrimObjType prim_obj, SecObjType sec_obj, MethodType method, std::string tradeoff, std::string distribution, bool out_sample);

	void runMergesolutionsManually(std::string input, MethodType meth, std::string distribution);
};

#endif // !runner_H_

