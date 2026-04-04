#pragma once

#include"algorithm.h"
//#include "algorithm.cpp"

/*
* This class holds the information 
*/

class runner 
{
public:
	runner();
	~runner();

	double runtime;

	void runBiObjRTSPTW(std::string filename, PrimObjType prim_obj, SecObjType sec_obj, MethodType method, double time_limit);

	void runMDLS(const instance& inst, nondominate_list& non, algorithm& algo, int seed, int num_iteration, double time_limit);

	void runMBBM(const instance& inst, nondominate_list& non, algorithm& algo, int seed, int num_iteration, double time_MDLS, double time_BBM);

	void output_to_file(std::string filename, instance& inst, nondominate_list& non);
};
