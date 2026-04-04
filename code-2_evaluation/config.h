#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

/*parameter setting*/
#define TRIANGLE_INEQUALITY 0 //only activate (1) if the triangle inequality is further restricted
#define SERVICE_TIME		0 //only activate (1) if the node service time is separate from the arc travel time
#define ASYMMETRIC			0 //only activate (1) if the cost/time matrix are aymmertric

/* run algorithm*/
#define TIME_UNCERTAIN      0 //only activate (1) if the instance includes t_max
#define PRE_BUDGET_VALUE    0 //only activate (1) if the problem is robust and the budget value is given in the instance (e.g. RTSPTW with cost minimization)
#define BUDGET_INTERVAL     0 //only activate (1) if we use budget interval [min, max]
#define MULTIPLE_INTERVALS	0 //only activate (1) if we use separate budget intervals for slack model
#define BUDGET_COMPUTE      0 //only activate (1) if the problem needs to compute the budget value as an objective (e.g. budget model or tri-obj model)
#define TRI_OBJ				0 //only activate (1) if the problem is tri-objective

/*algorithmic performance*/
#define REFERENCE_EXIT		0 //only activate (1) if the real Pareto reference set is available (e.g., for BRTSPTW(B)
#define LIMIT_REFERENCE		0 //only activate (1) if the reference set is a subset (e.g., for BRTSPTW(B))
#define DELETE_SAMETOUR     0 //only activate (1) if we delete the same tours obtained in different subintervals
#define ALL_SOLUTION_MIXED  0 //only activate (1) if evaluate performance indicator using all mixed solutions

/*solution evaluation (simulation)*/
//#define GENERAL_OUTSAMPLE	1 //activate (1) if using general out-samples (not based on in-sample size nor experiment nummber)
//#define IDENTICAL_COST		0 //activate (1) if all scenarios are using the same original cost when generating

namespace paths
{
	const std::string input = "../data_input/";
	const std::string results_input = "../data_output/";
	const std::string output = "../data_output/";

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

	/*read the whole content of the target file through the given path*/
	inline auto read_from_path(const std::filesystem::path&) -> std::string;
	inline auto read_from_path(const std::filesystem::path& p) -> std::string
	{
		std::ifstream file{ p };

		std::stringstream content;
		std::string line;

		while (std::getline(file, line))
		{
			content << line << std::endl;
		}

		return content.str();
	}

	/*read the whole content of the target file though the given file name*/
	inline auto read_from_file(const std::string&) -> std::string;
	inline auto read_from_file(const std::string& filename) -> std::string
	{
		std::string str;

		std::ifstream file;
		file.open(filename, std::ifstream::in);
		if (file.is_open())
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			str = buffer.str();
		}

		return str;
	}

	inline auto read_lines(const std::string&) -> std::vector<std::string>;
	inline auto read_lines(const std::string& str) -> std::vector<std::string>
	{
		std::vector<std::string> res;

		std::stringstream ss{ str };
		std::string line;

		while (std::getline(ss, line))
		{
			if (line.size()>0)
			{
				res.push_back(line);
			}
		}

		return res;
	}

	inline auto tokenize(const std::string&, const char delimiter) -> std::vector<std::string>;
	inline auto tokenize(const std::string& line, const char delimiter = ' ') -> std::vector<std::string>
	{
		std::istringstream iss{ line };
		std::vector<std::string> tokens;
		std::string token;

		while (std::getline(iss, token, delimiter))  //get line from stream iss into string token using delimiter
		{
			tokens.emplace_back(token);
		}

		return tokens;
	}
}

namespace constants
{
	enum class ProbType { robust = 0, nominal };
	enum class GrphType { sparse = 0, complete };
	enum class CostType { distance = 0, time };
	enum class ObjType { undef = -1, distance, completion, duration, slack, delta };
	enum class PrimObjType { undef = -1, distance, duration, completion };
	enum class SecObjType { undef = -1, slack, delta };
	enum class OptStat { nosol = 0, feasible, optfeas, optinfeas };
	enum class MethodType { rectangle = 0, multi_direction };
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

	static int num_exp = 10;
	static int num_insample = 150;
	static int num_outsample = 20000;

	static double BigM = 1e6;
	static double Epsilon = 1e-4;
	static int Nmax = 50;

	static double worst_perventage = 0.05;
	static double cost_increase_radio = 0.05; // for controlling the maximum cost increase
	static double cost_increase_unit = 0.01; // for controlling the smallest unit of the cost increase
	static double radius = 0;
	static double cvar_ltail = 0.1;
	static double wconstant = radius / (1 - cvar_ltail);
}