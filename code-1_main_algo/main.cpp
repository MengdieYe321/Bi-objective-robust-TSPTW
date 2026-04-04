#include"runner.h"

int main(int argc, char* argv[])
{
	/*--- desktop ---*/
	//int job_index = 0;//
	//const auto files = paths::find_files(paths::input + "DRTSPTW/N60/", ".txt");
	//while (job_index < num_exp)//
	//{	
	//	auto file = files[job_index];
	//	std::string filename = file.string();
		std::string filename = paths::input + "n60w120.001/discrete/size_80/robust/dis-n60w120.001_1.txt";
		//std::string filename = paths::input + "rbg010asep/discrete/size_20/robust/dis-rbg010asep_2.txt";

		runner run = runner();

		run.runBiObjRTSPTW(filename, PrimObjType::distance, SecObjType::delta, MethodType::rectangle, 900);

	//	job_index++;
	//}

	return 0;
}
