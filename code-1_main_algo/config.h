#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

/*parameter setting*/
#define TRIANGLE_INEQUALITY 0 //only activate (1) if the triangle inequality is further restricted

#define TIME_UNCERTAIN      1 //only activate (1) if the instance includes t_max
#define PRE_BUDGET_VALUE    0 //only activate (1) if the problem is robust and the budget value is given in the instance (e.g. RTSPTW with cost minimization)
#define BUDGET_INTERVAL     0 //only activate (1) if we use budget interval [min, max]
#define MULTIPLE_INTERVALS	0 //only activate (1) if we use separate budget intervals for slack model

#define BUDGET_COMPUTE      1 //only activate (1) if the problem needs to compute the budget value as an objective (e.g. budget model)
#define TRI_OBJ             0

namespace paths
{
	const std::string input = "../data_input/scenarios/";
	const std::string input_interval = "../data_input/budget_intervals/";
	const std::string output = "../data_output/biobj_sols/";
	inline auto find_files(const std::filesystem::path&, const std::string& ext = "*", const std::vector<std::string>& filter = {})->std::vector<std::filesystem::path>;

	inline auto find_files(const std::filesystem::path& p, const std::string& ext, const std::vector<std::string>& filter) -> std::vector<std::filesystem::path>
	{
		namespace fs = std::filesystem;

		std::vector<std::filesystem::path> files;

		const auto is_accepted = [&](const fs::path& name) -> bool
		{
			return filter.empty() || find(filter.begin(), filter.end(), name.filename()) != filter.end();
		};

		if (fs::exists(p))
		{
			if (fs::is_regular_file(p) && (p.extension() == ext || ext == "*") && is_accepted(p))
			{
				files.push_back(p);
			}
			else if (fs::is_directory(p))
			{
				fs::directory_iterator end_itr;

				for (fs::directory_iterator itr(p); itr != end_itr; ++itr)
				{
					const auto t_files = find_files(itr->path().string(), ext, filter);

					files.insert(files.end(), t_files.begin(), t_files.end());
				}
			}
		}

		return files;
	}
}

namespace constants
{
	enum class ProbType { robust = 0, nominal };
	enum class GrphType { sparse = 0, complete };
	enum class CostType { distance = 0, time };
	enum class ObjType { undef = -1, distance, completion, duration, slack, delta };
	enum class PrimObjType { undef = -1, distance, duration, completion};
	enum class SecObjType { undef = -1, slack, delta };
	enum class OptStat { nosol = 0, feasible, optfeas, optinfeas };
	enum class MethodType { rectangle=0, multi_direction };
}

inline std::ostream& operator << (std::ostream& out, const constants::ObjType& f)
{
	switch (f) {
	case constants::ObjType::undef:
		out << "undefined";
		break;
	case constants::ObjType::distance:
		out << "cost";
		break;
	case constants::ObjType::completion:
		out << "completion";
		break;
	case constants::ObjType::duration:
		out << "duration";
		break;
	case constants::ObjType::slack:
		out << "slack";
		break;
	case constants::ObjType::delta:
		out << "delta";
		break;
	}
	return out;
}

inline std::ostream& operator << (std::ostream& out, const constants::PrimObjType& f)
{
	switch (f) {
	case constants::PrimObjType::undef:
		out << "undefined";
		break;
	case constants::PrimObjType::distance:
		out << "cost";
		break;
	case constants::PrimObjType::duration:
		out << "duration";
		break;
	case constants::PrimObjType::completion:
		out << "completion";
		break;
	}
	return out;
}

inline std::ostream& operator << (std::ostream& out, const constants::SecObjType& f)
{
	switch (f) {
	case constants::SecObjType::undef:
		out << "undefined";
		break;
	case constants::SecObjType::slack:
		out << "slack";
		break;
	case constants::SecObjType::delta:
		out << "delta";
		break;
	}
	return out;
}

inline std::ostream& operator << (std::ostream& out, const constants::MethodType& f)
{
	switch (f) {
	case constants::MethodType::multi_direction:
		out << "MDLS";
		break;
	case constants::MethodType::rectangle:
		out << "MBBM";
		break;
	}
	return out;
}

namespace parameters
{
	const auto problem_type = constants::ProbType::robust;
	const auto network_type = constants::GrphType::complete;
	const auto cost_type = constants::CostType::distance; //GDE60 and rbg distance / test instance time

	static int num_exp = 20;
	static int num_insample = 20;
	static int num_outsample = 20000;

	static double BigM = 1e6;
	static double Epsilon = 1e-4;
	static int Nmax = 50;
}