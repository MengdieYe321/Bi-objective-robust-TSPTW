#include"instance.h"

instance::instance(const std::string inputfile, PrimObjType obj1, SecObjType obj2)
	: _prob_name("none")
	, _num_node(0)
	, delta(BigM)
	, _primobj_type(obj1)
	, _secobj_type(obj2)
{
	bool input = read_data(inputfile);
	if (input)
	{
		std::cout << "LOAD INSTANCE " << _prob_name << " SUCCESSFULLY." << std::endl;

		data_preprocessing();
	}
	else
	{
		std::cout << "ERROR OPENING INSTANVE FILE. TERMINATE." << std::endl;
		exit(1);
	}
}

instance::~instance()
{
	//empty
}

/*-------------------------------------------------------*/
/*read instance data from input file*/
bool instance::read_data(std::string input)
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		return false;
	}

	in >> _prob_name >> _num_node;

	delta_min = delta_min = BigM;

	if (TIME_UNCERTAIN)
	{
		if (PRE_BUDGET_VALUE)
		{
			in >> delta_min;

			if (BUDGET_INTERVAL)
			{
				in >> delta_max;
			}
		}
		else
		{
			/*load the budget interval values*/
			std::string budgetfile;
			if (MULTIPLE_INTERVALS && _secobj_type == SecObjType::slack)
			{
				budgetfile = paths::input_interval + "N12/size_20/MBBM/" + _prob_name + ".txt"; //when cost-slack model + multiple interval
			}
			else
			{
				budgetfile = paths::input_interval + "general_interval.txt";
			}
			bool interval_input = read_budget_intervals(budgetfile);
			if (interval_input)
			{
				std::cout << "LOAD BUDGET INTERVAL SUCCESSFULLY." << std::endl;
			}
			else
			{
				std::cout << "ERROR LOADING BUDGET FILE. TERMINATE." << std::endl;
				exit(1);
			}

			delta_min = delta_min_values[0];

			if (BUDGET_INTERVAL)
			{
				delta_max = delta_max_values[0];
			}
		}
	}

	delta = delta_min;
	if (_secobj_type == SecObjType::delta || BUDGET_COMPUTE)
	{
		delta = BigM;
	}

	for (int i = 0; i <= _num_node; i++)
	{
		_nodes.push_back(i); //0,1,...,n,n+1
	}

	_cost.resize(_num_node, std::vector < double>(_num_node));
	_t_min.resize(_num_node, std::vector < double>(_num_node));
	_t_max.resize(_num_node, std::vector < double>(_num_node));
	_max_delay.resize(_num_node, std::vector < double>(_num_node));
	_shp_t.resize(_num_node, std::vector < double>(_num_node));
	_shp_c.resize(_num_node, std::vector < double>(_num_node));
	//_sev_t_min.resize(_num_node);
	//_sev_t_max.resize(_num_node);
	_earl.resize(_num_node);
	_lat.resize(_num_node);

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

	/*if (SERVICE_TIME)
	{
		for (int i = 0; i < _num_node; i++)
		{
			in >> _sev_t_min[i];

			if (TIME_UNCERTAIN)
			{
				in >> _sev_t_max[i];
			}
		}

		for (int i = 0; i < _num_node; i++)
		{
			for (int j = 0; j < _num_node; j++)
			{
				_t_min[i][j] = _t_min[i][j] + _sev_t_min[i];
				_t_max[i][j] = _t_max[i][j] + _sev_t_max[i];
			}
		}
	}
	else
	{
		for (int i = 0; i < _num_node; i++)
		{
			_sev_t_min[i] = _sev_t_max[i] = 0;
		}
	}*/

	/*read the time windows*/
	for (int i = 0; i < _num_node; i++)
	{
		in >> _earl[i] >> _lat[i];
	}

	in.close();

	return true;
}



auto instance::read_budget_intervals(std::string input) ->bool
{
	std::ifstream in;
	in.open(input, std::ifstream::in);
	if (!in.is_open())
	{
		return false;
	}

	std::string name;
	in >> name;
	if (MULTIPLE_INTERVALS && _secobj_type==SecObjType::slack && (name != _prob_name))
	{
		std::cout << "INSTANCE FILE DEVIATE FROM BUDGET INTERVAL FILE.\n";
		return false;
	}

	double min, max;
	if (!BUDGET_INTERVAL)
	{
		while ((in >> min))
		{
			delta_min_values.push_back(min);
		}
	}
	else
	{
		while ((in >> min >> max))
		{
			delta_min_values.push_back(min);
			delta_max_values.push_back(max);
		}
	}

	return true;
}

/*return / set the budget inteval values*/
//auto instance::budget_min() const -> double
//{
//	return _delta_min;
//}
//
//auto instance::budget_max() const -> double
//{
//	return _delta_max;
//}
//
//auto instance::set_budget_min(double min) -> void
//{
//	_delta_min = min;
//}
//
//auto instance::set_budget_max(double max) -> void
//{
//	_delta_max = max;
//}

/*return the set of nodes of the instance (incl. depot)*/
auto instance::nodes() const -> const std::vector<int>&
{
	return _nodes;
}

/*return the cost between node i and node j*/
auto instance::cost(int i, int j) const ->double
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

/*return the nominal travel time between two nodes*/
auto instance::min_time(int i, int j) const ->double
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

/*return the maximal travel time between two nodes*/
auto instance::max_time(int i, int j) const ->double
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

/*return the maximum delay between two nodes*/
auto instance::maxdelay(int i, int j) const ->double
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

//auto instance::min_sev_time(int i) const ->double
//{
//	if (i == _num_node)
//	{
//		return _sev_t_min[0];
//	}
//	else
//	{
//		return _sev_t_min[i];
//	}
//}
//
//auto instance::max_sev_time(int i) const ->double
//{
//	if (i == _num_node)
//	{
//		return _sev_t_max[0];
//	}
//	else
//	{
//		return _sev_t_max[i];
//	}
//}

auto instance::shp_min_time(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _shp_t[0][0];
		}
		else
		{
			return _shp_t[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _shp_t[i][0];
		}
		else
		{
			return _shp_t[i][j];
		}
	}
}

auto instance::shp_min_cost(int i, int j) const ->double
{
	if (i == _num_node)
	{
		if (j == _num_node)
		{
			return _shp_c[0][0];
		}
		else
		{
			return _shp_c[0][j];
		}
	}
	else
	{
		if (j == _num_node)
		{
			return _shp_c[i][0];
		}
		else
		{
			return _shp_c[i][j];
		}
	}
}



/*return the begin fo time window of a node*/
auto instance::earliest(int i) const ->double
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

/*return the end of timw window of a node*/
auto instance::latest(int i) const ->double
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



/*check whether a given arc is feasible w.r.t. preprocessing*/
auto instance::is_arc_feasible(const arc& a) const ->bool
{
	const auto i = a.i;
	const auto j = a.j;

	if (i == j)
	{
		return false;
	}
	else if (min_time(i, j) == BigM)
	{
		return false;
	}
	else
	{
		return true;
	}
}


/*-------------------------------------------------------*/
	/*--arc elimination--*/
void instance::arc_eliminate()
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

/*--time windows tightening--*/
void instance::timewindow()
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

/*--compute shortest path--*/
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
void instance::data_preprocessing()
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
		arc_eliminate();

		timewindow();

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