#include "instance.h"

instance::instance(const std::string inputfile, CostType cost_type)
	: _inst_name("none")
	, _num_node(0)
{
	bool input = readOriData(inputfile, cost_type);
	if (input)
	{
		std::cout << "Load instance " << _inst_name << " successfully." << std::endl;
	}
	else
	{
		std::cout << "Erroe opening instance file. Terminate." << std::endl;
		exit(1);
	}
}

instance::~instance()
{
	//empty
}

/*--- original instance ---*/
bool instance::readOriData(std::string input, CostType cost_type)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		return false;
	}

	in >> _inst_name >> _num_node;

	for (int i = 0; i <= _num_node; i++)
	{
		_nodes.push_back(i); //0,1,...,n,n+1
	}

	_ori_cost.resize(_num_node, std::vector < double>(_num_node));
	_ori_pur_time.resize(_num_node, std::vector < double>(_num_node));
	_ori_time.resize(_num_node, std::vector < double>(_num_node));
	_ori_sev_time.resize(_num_node);
	_earl.resize(_num_node);
	_lat.resize(_num_node);

	/*read the cost matrix*/
	for (int i = 0; i < _num_node; i++)
	{
		for (int j = 0; j < _num_node; j++)
		{
			in >> _ori_cost[i][j];

			_ori_pur_time[i][j] = _ori_cost[i][j];
		}
	}

	/*read the nominal time matrix if the travel time is not equal to cost*/
	if (cost_type != CostType::time)
	{
		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				in >> _ori_pur_time[i][j];
			}
		}
	}

	if (SERVICE_TIME)
	{
		for (int i = 0; i < _num_node; i++)
		{
			in >> _ori_sev_time[i];
		}
	}
	else
	{
		for (int i = 0; i < _num_node; i++)
		{
			_ori_sev_time[i] = 0;
		}
	}

	for (int i = 0; i < _num_node; i++)
	{
		for (int j = 0; j < _num_node; j++)
		{
			_ori_time[i][j] = _ori_pur_time[i][j] + _ori_sev_time[i];
		}
	}

	/*read the time windows*/
	for (int i = 0; i < _num_node; i++)
	{
		in >> _earl[i] >> _lat[i];
	}

	in.close();

	return true;
}

auto instance::nodes() const -> const std::vector<int>&
{
	return _nodes;
}

auto instance::getOriCost(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _ori_cost[0][0];
		}
		else
		{
			return _ori_cost[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _ori_cost[i][0];
		}
		else
		{
			return _ori_cost[i][j];
		}
	}
}

auto instance::getOriArcTime(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _ori_pur_time[0][0];
		}
		else
		{
			return _ori_pur_time[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _ori_pur_time[i][0];
		}
		else
		{
			return _ori_pur_time[i][j];
		}
	}
}

auto instance::getOriTime(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _ori_time[0][0];
		}
		else
		{
			return _ori_time[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _ori_time[i][0];
		}
		else
		{
			return _ori_time[i][j];
		}
	}
}

auto instance::getOriSevTime(int i) const ->double
{
	if (i == _num_node)
	{
		return _ori_sev_time[0];
	}
	else
	{
		return _ori_sev_time[i];
	}
}

auto instance::getEarliest(int i) const ->double
{
	if (i == _num_node)
	{
		return _earl[0];
	}
	else
	{
		return _earl[i];
	}
}

auto instance::getLatest(int i) const ->double
{
	if (i == _num_node)
	{
		return _lat[0];
	}
	else
	{
		return _lat[i];
	}
}


/*---- for scenarios ----*/
/*read in already existing scenarios*/
bool instance::readScenarioData(std::string inputfile, CostType cost_type, int sample_index, bool out_sample)
{
	std::ifstream in;
	in.open(inputfile, std::ifstream::in);
	if (!in.is_open())
	{
		std::cout << "Error loading scenario file." << std::endl;
		return false;
	}

	std::string name;
	int node;
	in >> name >> node;

	if (node != _num_node)
	{
		std::cout << "Warning: Wrong number of nodes in the scenario file! Please check!" << std::endl;
		return false;
	}

	for (int i = 0; i < _num_node; i++)
	{
		for (int j = 0; j < _num_node; j++)
		{
			if (!out_sample)
			{
				in >> _cost_scen_insample[sample_index][i][j];
				_time_scen_insample[sample_index][i][j] = _cost_scen_insample[sample_index][i][j];
			}
			else
			{
				in >> _cost_scen_outsample[sample_index][i][j];
				_time_scen_outsample[sample_index][i][j] = _cost_scen_outsample[sample_index][i][j];
			}
		}
	}

	/*time matrix*/
	if (cost_type != CostType::time)
	{
		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				if (!out_sample)
				{
					in >> _time_scen_insample[sample_index][i][j];
				}
				else
				{
					in >> _time_scen_outsample[sample_index][i][j];
				}
			}
		}
	}

	/*time windows*/
	double earl, lat;
	for (int i = 0; i < _num_node; i++)
	{
		in >> earl >> lat;
	}

	in.close();

	return true;
}

auto instance::getInScenTime(int s, int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _time_scen_insample[s][0][0];
		}
		else
		{
			return _time_scen_insample[s][0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _time_scen_insample[s][i][0];
		}
		else
		{
			return _time_scen_insample[s][i][j];
		}
	}
}

auto instance::getOutScenTime(int s, int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _time_scen_outsample[s][0][0];
		}
		else
		{
			return _time_scen_outsample[s][0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _time_scen_outsample[s][i][0];
		}
		else
		{
			return _time_scen_outsample[s][i][j];
		}
	}
}

auto instance::getInScenCost(int s, int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _cost_scen_insample[s][0][0];
		}
		else
		{
			return _cost_scen_insample[s][0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _cost_scen_insample[s][i][0];
		}
		else
		{
			return _cost_scen_insample[s][i][j];
		}
	}
}

auto instance::getOutScenCost(int s, int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _cost_scen_outsample[s][0][0];
		}
		else
		{
			return _cost_scen_outsample[s][0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _cost_scen_outsample[s][i][0];
		}
		else
		{
			return _cost_scen_outsample[s][i][j];
		}
	}
}

/*--- robust / biobjective instances ---*/
bool instance::readRobustData(std::string input, CostType cost_type)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		return false;
	}

	int n;
	in >> _prob_name >> n;

	/*the process involving the budget size is skipped here*/

	_cost.resize(_num_node, std::vector < double>(_num_node));
	_t_min.resize(_num_node, std::vector < double>(_num_node));
	_t_max.resize(_num_node, std::vector < double>(_num_node));
	_max_delay.resize(_num_node, std::vector < double>(_num_node));
	_earliest.resize(_num_node);
	_latest.resize(_num_node);
	_shp_c.resize(_num_node, std::vector < double>(_num_node));
	_shp_t.resize(_num_node, std::vector < double>(_num_node));

	/*read the cost matrix*/
	for (int i = 0; i < _num_node; i++)
	{
		for (int j = 0; j < _num_node; j++)
		{
			in >> _cost[i][j];

			_t_min[i][j] = _cost[i][j];
		}
	}

	/*read the nominal time matrix if the travel time is not equal to cost*/
	if (cost_type != CostType::time)
	{
		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				in >> _t_min[i][j];
			}
		}
	}

	if (TIME_UNCERTAIN)
	{
		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				in >> _t_max[i][j];
			}
		}
	}
	else
	{
		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				_t_max[i][j] = _t_min[i][j];
			}
		}
	}

	/*read the time windows*/
	for (int i = 0; i < _num_node; i++)
	{
		in >> _earliest[i] >> _latest[i];
	}

	in.close();

	return true;
}

auto instance::getCost(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _cost[0][0];
		}
		else
		{
			return _cost[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _cost[i][0];
		}
		else
		{
			return _cost[i][j];
		}
	}
}

auto instance::getMinTime(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _t_min[0][0];
		}
		else
		{
			return _t_min[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _t_min[i][0];
		}
		else
		{
			return _t_min[i][j];
		}
	}
}

auto instance::getMaxTime(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _t_max[0][0];
		}
		else
		{
			return _t_max[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _t_max[i][0];
		}
		else
		{
			return _t_max[i][j];
		}
	}
}

auto instance::getMaxDelay(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _max_delay[0][0];
		}
		else
		{
			return _max_delay[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _max_delay[i][0];
		}
		else
		{
			return _max_delay[i][j];
		}
	}
}

auto instance::getEarlTW(int i) const ->double
{
	if (i == _num_node)
	{
		return _earliest[0];
	}
	else
	{
		return _earliest[i];
	}
}

auto instance::getLatTW(int i) const ->double
{
	if (i == _num_node)
	{
		return _latest[0];
	}
	else
	{
		return _latest[i];
	}
}

void instance::arcElimination()
{
	/*nominal: r(i) + t(i,j) > d(j), eliminate (i,j)*/
	for (int i = 0; i < _num_node; i++)
	{
		for (int j = 0; j < _num_node; j++)
		{
			if (i != j && (_t_min[i][j] != BigM))
			{
				if ((_earl[i] + _t_min[i][j]) > _lat[j])
				{
					_t_min[i][j] = BigM;
				}
			}
		}
	}

	/*robust C: r(i) + t(i,j) + min{delta(i,j), delay} > d(j), eliminate(i,j)*/
	if (problem_type == ProbType::robust && delta != BigM)
	{
		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				if (i != j && (_t_min[i][j] != BigM))
				{
					double temp = std::min(_max_delay[i][j], delta);
					if ((_earl[i] + _t_min[i][j] + temp) > _lat[j])
					{
						_t_min[i][j] = BigM;
					}
				}
			}
		}
	}

	for (int i = 0; i < _num_node; i++)
	{
		for (int j = 0; j < _num_node; j++)
		{
			if (_t_min[i][j] == BigM)
			{
				_t_max[i][j] = BigM;
				_max_delay[i][j] = BigM;
				_cost[i][j] = BigM;
				_shp_c[i][j] = BigM;
				_shp_t[i][j] = BigM;
			}
		}
	}
}

void instance::narrowTimeWindow()
{
	double earlist, latest;
	double release, due;

	//increase the release date due to the earliest arrival time
	for (int i = 0; i < _num_node; i++)
	{
		earlist = BigM;
		release = 0;
		if (i == 0)
		{
			release = 0;
		}
		else
		{
			for (int j = 0; j < _num_node; j++)
			{
				if (i != j && _t_min[j][i] != BigM)
				{
					if (earlist > (_earl[j] + _t_min[j][i]))
					{
						earlist = _earl[j] + _t_min[j][i];
					}
				}
			}
			release = std::max(_earl[i], earlist);
		}
		_earl[i] = release;
	}

	//decrease the due date due to the latest arrival time
	for (int i = 0; i < _num_node; i++)
	{
		latest = 0;
		due = 0;
		for (int j = 0; j < _num_node; j++)
		{
			if (i != j && _t_max[j][i] != BigM)
			{
				if (latest < (_lat[j] + _t_max[j][i]))
				{
					latest = _lat[j] + _t_max[j][i];
				}
			}
		}
		double data = std::max(_earl[i], latest);
		due = std::min(_lat[i], data);
		_lat[i] = due;
	}

	//increase the release date due to the waiting time of the successors
	for (int i = 0; i < _num_node; i++)
	{
		release = 0;
		earlist = BigM;

		if (i == 0)
		{
			release = 0;
		}
		else
		{
			for (int j = 0; j < _num_node; j++)
			{
				if (i != j && _t_max[i][j] != BigM)
				{
					if (earlist > (_earl[j] - _t_max[i][j]))
					{
						earlist = _earl[j] - _t_max[i][j];
					}
				}
			}
			double data = std::min(_lat[i], earlist);
			release = std::max(_earl[i], data);
		}
		_earl[i] = release;
	}

	// deceease the due date due to the latest possible departure time
	for (int i = 0; i < _num_node; i++)
	{
		latest = 0;
		due = 0;

		if (i == 0)
		{
			due = _lat[i];
		}
		else
		{
			for (int j = 0; j < _num_node; j++)
			{
				if (i != j && _t_min[i][j] != BigM)
				{
					if (latest < (_lat[j] - _t_min[i][j]))
					{
						latest = _lat[j] - _t_min[i][j];
					}
				}
			}
			due = std::min(_lat[i], latest);
		}
		_lat[i] = due;
	}
}

void instance::floydwarshall()
{
	/*inistialize*/
	double** spred = new double* [_num_node];
	for (int i = 0; i < _num_node; i++)
	{
		spred[i] = new double[_num_node];
	}

	double** scard = new double* [_num_node];
	for (int i = 0; i < _num_node; i++)
	{
		scard[i] = new double[_num_node];
	}

	for (int i = 0; i < _num_node; i++)
	{
		for (int j = 0; j < _num_node; j++)
		{
			_shp_t[i][j] = BigM;
			_shp_c[i][j] = BigM;

			if (spred) spred[i][j] = -1;
			if (scard) scard[i][j] = BigM;

			if (_t_min[i][j] != BigM)
			{
				if (i == j)
				{
					_shp_t[i][j] = 0;
					_shp_c[i][j] = 0;

					if (spred) spred[i][j] = -1;
					if (scard) scard[i][j] = 0;
				}
				else
				{
					_shp_t[i][j] = _t_min[i][j];
					_shp_c[i][j] = _cost[i][j];

					if (spred) spred[i][j] = i;
					if (scard) scard[i][j] = 1;
				}
			}
		}
		_shp_t[i][i] = 0;
		_shp_c[i][i] = 0;
	}

	/*shortest path*/
	for (int kx = 0; kx < _num_node; kx++)
	{
		for (int ix = 0; ix < _num_node; ix++)
		{
			for (int jx = 0; jx < _num_node; jx++)
			{
				if ((_shp_t[ix][kx] > BigM) || (_shp_t[kx][jx] > BigM)) continue;

				if ((_shp_t[ix][kx] + _shp_t[kx][jx]) < (_shp_t[ix][jx] - Epsilon))
				{
					_shp_t[ix][jx] = _shp_t[ix][kx] + _shp_t[kx][jx];
					_shp_c[ix][jx] = _shp_c[ix][kx] + _shp_c[kx][jx];

					if (spred) spred[ix][jx] = spred[kx][jx];
					if (scard) scard[ix][jx] = scard[ix][kx] + scard[kx][jx];
				}
			}
		}
	}

	/*free each array*/
	for (int i = 0; i < _num_node; i++)
	{
		delete[] spred[i];
		delete[] scard[i];
	}
	delete[] spred;
	delete[] scard;
}

/*-------------------------------------------------------*/
/*data preprocessing*/
void instance::dataPreprocessing()
{
	/*update the distance or travel time matrix w.r.t. the triangle inequality*/
	floydwarshall();

	bool repeat = true;
	while (repeat)
	{
		repeat = false;
		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				if (TRIANGLE_INEQUALITY)
				{
					if (_t_min[i][j] > _shp_t[i][j])
					{
						_t_min[i][j] = _shp_t[i][j];
						_cost[i][j] = _shp_c[i][j];
						repeat = true;
					}
				}
			}
		}
	}

	for (int i = 0; i < _num_node; i++)
	{
		for (int j = 0; j < _num_node; j++)
		{
			if (!TIME_UNCERTAIN)
			{
				_t_max[i][j] = _t_min[i][j];
			}
			_max_delay[i][j] = _t_max[i][j] - _t_min[i][j];
		}
	}

	//arc elimination & tw_tightening
	int changearc[100];
	for (int i = 0; i < 100; i++)
	{
		changearc[i] = 0;
	}

	int k = 0;
	int arcsum = 0;
	while (k >= 0)
	{
		arcElimination();
		narrowTimeWindow();

		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				if (_t_min[i][j] == BigM)
				{
					changearc[k]++;
				}
			}
		}

		if (k == 0)
		{
			arcsum = changearc[k];
			k++;
		}
		else
		{
			if (changearc[k] == changearc[k - 1])
			{
				break;
			}
			else
			{
				arcsum = changearc[k];
				k++;
			}
		}
	}
}