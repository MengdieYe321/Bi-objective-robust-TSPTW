#include"tsptw_solution.h"

/*
* This class holds all the information of the single obective tsptw solution of the heuristic
*/
namespace tsptw_tph
{
	tsptw_solution::tsptw_solution(const instance& inst)
		:_inst(&inst)
		, _objective(BigM)
		, _objective2(BigM)
		, _check_objective(BigM)
		, _cur_objective(BigM)
		, _cur_objective2(BigM)
		, _tw_violation(0.0)
		, _penalty_cost(0.0)
	{
		init();
	}

	tsptw_solution::~tsptw_solution()
	{
		//empty
	}

	void tsptw_solution::init()
	{
		auto nplus = _inst->_num_node + 1;

		_position.resize(nplus);
		std::fill(_position.begin(), _position.end(), -1);

		_node_in_position.resize(nplus);
		std::fill(_node_in_position.begin(), _node_in_position.end(), 0);

		_forward_tw_cache.resize(nplus);
		_forward_tw_cache[0] = { 0.0, 0.0, _inst->earliest(0), _inst->latest(0), 0.0, 0.0, _inst->latest(0), BigM };

		for (int i = 0; i < nplus; i++)
		{
			_forward_tw_cache[i].cumulated_waitings.clear();
			_forward_tw_cache[i].cumulated_delay_inf.clear();

			for (int k = 0; k < nplus; k++)
			{
				_forward_tw_cache[i].cumulated_waitings.push_back(0.0);
				_forward_tw_cache[i].cumulated_delay_inf.push_back(0.0);
			}
		}

		_forward_robust_tw_cache.resize(nplus);
		_forward_robust_tw_cache[0] = { _inst->latest(0), 0 };

		_forward_max_robust_tw_cache.resize(nplus);
		_forward_max_robust_tw_cache[0] = { _inst->latest(0), 0 };

		//if (problem_type == ProbType::robust)
		//{
			for (int i = 0; i < nplus; i++)
			{
				_forward_robust_tw_cache[i].robust_dur.clear();
				_forward_robust_tw_cache[i].robust_tw.clear();
				_forward_robust_tw_cache[i].robust_earl.clear();
				_forward_robust_tw_cache[i].robust_lat.clear();
				_forward_robust_tw_cache[i].worst_aT.clear();
				_forward_robust_tw_cache[i].worst_penalty.clear();
				_forward_robust_tw_cache[i].cumulated_delay.clear();

				if (BUDGET_INTERVAL)
				{
					_forward_max_robust_tw_cache[i].robust_dur.clear();
					_forward_max_robust_tw_cache[i].robust_tw.clear();
					_forward_max_robust_tw_cache[i].robust_earl.clear();
					_forward_max_robust_tw_cache[i].robust_lat.clear();
					_forward_max_robust_tw_cache[i].worst_aT.clear();
					_forward_max_robust_tw_cache[i].cumulated_delay.clear();
				}

				for (int k = 0; k < nplus; k++)
				{
					_forward_robust_tw_cache[i].robust_dur.push_back(0.0);
					_forward_robust_tw_cache[i].robust_tw.push_back(0.0);
					_forward_robust_tw_cache[i].robust_earl.push_back(_inst->earliest(i));
					_forward_robust_tw_cache[i].robust_lat.push_back(_inst->latest(i));
					_forward_robust_tw_cache[i].worst_aT.push_back(0.0);
					_forward_robust_tw_cache[i].worst_penalty.push_back(0.0);
					_forward_robust_tw_cache[i].cumulated_delay.push_back(0.0);

					if (BUDGET_INTERVAL)
					{
						_forward_max_robust_tw_cache[i].robust_dur.push_back(0.0);
						_forward_max_robust_tw_cache[i].robust_tw.push_back(0.0);
						_forward_max_robust_tw_cache[i].robust_earl.push_back(_inst->earliest(i));
						_forward_max_robust_tw_cache[i].robust_lat.push_back(_inst->latest(i));
						_forward_max_robust_tw_cache[i].worst_aT.push_back(0.0);
						_forward_max_robust_tw_cache[i].cumulated_delay.push_back(0.0);
					}
				}
			}
		//}

		_previous.resize(nplus);
		_next.resize(nplus);

		_cur_previous.resize(nplus);
		_cur_next.resize(nplus);

		_alpha.resize(nplus);

		for (const auto i : _inst->nodes())
		{
			_previous[i] = i;
			_next[i] = i;

			_cur_previous[i] = i;
			_cur_next[i] = i;

			_alpha[i] = 10;
		}

		_alpha_set = 10;

		/*unrouted nodes*/
		_unrouted_nodes.clear();
		for (const auto i : _inst->nodes())
		{
			if (i != 0 && i != _inst->_num_node)
			{
				_unrouted_nodes.push_back(i);
			}
		}
	}

	void tsptw_solution::reset_empty()
	{
		_objective = BigM;
		_objective2 = BigM;
		_check_objective = BigM;
		_cost = BigM;
		_tw_violation = 0.0;
		_penalty_cost = 0.0;

		auto nplus = _inst->_num_node + 1;

		_tour.clear();
		_unrouted_nodes.clear();
		_vio_nodes.clear();
		non_vio_nodes.clear();
		_position.clear();
		_node_in_position.clear();
		_previous.clear();
		_next.clear();
		_alpha.clear();
		_tour.clear();
		_cur_previous.clear();
		_cur_next.clear();

		for (int i = 0; i < nplus; i++)
		{
			_forward_tw_cache[i].cumulated_waitings.clear();
			_forward_tw_cache[i].cumulated_delay_inf.clear();
		}
		_forward_tw_cache.clear();

		//if (problem_type == ProbType::robust)
		//{
			for (int i = 0; i < nplus; i++)
			{
				_forward_robust_tw_cache[i].robust_dur.clear();
				_forward_robust_tw_cache[i].robust_tw.clear();
				_forward_robust_tw_cache[i].robust_earl.clear();
				_forward_robust_tw_cache[i].robust_lat.clear();
				_forward_robust_tw_cache[i].worst_aT.clear();
				_forward_robust_tw_cache[i].worst_penalty.clear();
				_forward_robust_tw_cache[i].cumulated_delay.clear();

				if (BUDGET_INTERVAL)
				{
					_forward_max_robust_tw_cache[i].robust_dur.clear();
					_forward_max_robust_tw_cache[i].robust_tw.clear();
					_forward_max_robust_tw_cache[i].robust_earl.clear();
					_forward_max_robust_tw_cache[i].robust_lat.clear();
					_forward_max_robust_tw_cache[i].worst_aT.clear();
					_forward_max_robust_tw_cache[i].cumulated_delay.clear();
				}
			}
			_forward_robust_tw_cache.clear();
			
			if (BUDGET_INTERVAL)
			{
				_forward_max_robust_tw_cache.clear();
			}
		//}

		_time_related_vars.clear();

		init();
	}

	/*Gets the problem instance of this solution.*/
	auto tsptw_solution::inst() const -> const instance &
	{
		return *_inst;
	}

	/*return the vector of the tour*/
	auto tsptw_solution::tour() const -> const std::vector<int> &
	{
		return _tour;
	}

	auto tsptw_solution::cur_tour() const -> const std::vector<int> &
	{
		return _cur_tour;
	}

	auto tsptw_solution::unrouted_nodes()const -> const std::vector<int> &
	{
		return _unrouted_nodes;
	}

	auto tsptw_solution::violated_nodes() const -> const std::vector<int> &
	{
		return _vio_nodes;
	}

	auto tsptw_solution::non_violated_nodes() const -> const std::vector<int> &
	{
		return non_vio_nodes;
	}

	/*return the position of the node i*/
	auto tsptw_solution::position(const int i)const  -> int
	{
		return _position[i];
	}

	/*return the successor of the given node*/
	auto tsptw_solution::next(const int i)const  -> int
	{
		return _next[i];
	}

	/*return the predecessor of the given node*/
	auto tsptw_solution::previous(const int i)const -> int
	{
		return _previous[i];
	}

	/*return the node of the given position*/
	auto tsptw_solution::node_in_position(const int i)const -> int
	{
		return _node_in_position[i];
	}

	/*Gets the primary objective for the given tour*/
	auto tsptw_solution::objective() const -> double
	{
		return _objective;
	}

	/*Gets the secondary objective for the given tour*/
	auto tsptw_solution::objective2() const -> double
	{
		return _objective2;
	}

	auto tsptw_solution::check_objective() const -> double
	{
		return _check_objective;
	}

	auto tsptw_solution::cur_objective() const -> double
	{
		return _cur_objective;
	}

	auto tsptw_solution::cur_objective2() const -> double
	{
		return _cur_objective2;
	}

	auto tsptw_solution::cur_check_objective() const -> double
	{
		return _cur_check_objective;
	}

	auto tsptw_solution::cost() const -> double
	{
		return _cost;
	}

	auto tsptw_solution::makespan() const -> double
	{
		if (problem_type != ProbType::robust)
		{
			const auto& lfc = _forward_tw_cache[_inst->_num_node];

			return (lfc.aT);
		}
		else
		{
			double max_ct = 0.0;

			if (!BUDGET_INTERVAL)
			{
				const auto& lfc = _forward_robust_tw_cache[_inst->_num_node];
				//int scen = lfc.min_slack_scen;
				//return (lfc.robust_dur[scen] + lfc.robust_earl[scen]);
				for (int i = 0; i <= inst()._num_node; i++)
				{
					auto ct = lfc.worst_aT[i];

					if (ct > max_ct)
					{
						max_ct = ct;
					}
				}
			}
			else
			{
				const auto& lfc = _forward_max_robust_tw_cache[_inst->_num_node];
				//int scen = lfc.min_slack_scen;
				//return (lfc.robust_dur[scen] + lfc.robust_earl[scen]);
				for (int i = 0; i <= inst()._num_node; i++)
				{
					auto ct = lfc.worst_aT[i];

					if (ct > max_ct)
					{
						max_ct = ct;
					}
				}
			}

			return max_ct;
		}
	}

	auto tsptw_solution::tour_duration() const -> double //counting the depot + D(i)
	{
		if (problem_type != ProbType::robust)
		{
			const auto& lfc = _forward_tw_cache[_inst->_num_node];

			return lfc.dur;
		}
		else
		{
			double max_td = 0.0;

			if (!BUDGET_INTERVAL)
			{
				const auto& lfc = _forward_robust_tw_cache[_inst->_num_node];
				//int scen = lfc.min_slack_scen;
				//return lfc.robust_dur[scen];
				for (int i = 0; i <= inst()._num_node; i++)
				{
					auto td = lfc.robust_dur[i];

					if (td > max_td)
					{
						max_td = td;
					}
				}
			}
			else
			{
				const auto& lfc = _forward_max_robust_tw_cache[_inst->_num_node];
				//int scen = lfc.min_slack_scen;
				//return lfc.robust_dur[scen];
				for (int i = 0; i <= inst()._num_node; i++)
				{
					auto td = lfc.robust_dur[i];

					if (td > max_td)
					{
						max_td = td;
					}
				}
			}

			return max_td;
		}
	}

	auto tsptw_solution::min_rslack_scen()const  -> int
	{
		const auto& lfc = _forward_robust_tw_cache[_inst->_num_node];
		int scen = lfc.min_slack_scen;
		return scen;
	}

	auto tsptw_solution::min_slack()const  -> double
	{
		if (problem_type != ProbType::robust)
		{
			return _forward_tw_cache[_inst->_num_node].min_slack;
		}
		else
		{
			if (!BUDGET_INTERVAL)
			{
				return _forward_robust_tw_cache[_inst->_num_node].rmin_slack;
			}
			else
			{
				return _forward_max_robust_tw_cache[_inst->_num_node].rmin_slack;
			}
		}
	}

	auto tsptw_solution::rmin_slack()const  -> double
	{
		if (!BUDGET_INTERVAL)
		{
			return _forward_robust_tw_cache[_inst->_num_node].rmin_slack;
		}
		else
		{
			return _forward_max_robust_tw_cache[_inst->_num_node].rmin_slack;
		}
	}

	auto tsptw_solution::min_budget() const -> double
	{
		return _forward_tw_cache[_inst->_num_node].min_budget;
	}

	/*Gets the time window violation for the given tour*/
	auto tsptw_solution::tw_violation() const -> double
	{
		return _tw_violation;
	}

	auto tsptw_solution::nominal_arrival_T(const int i) const -> double
	{
		return _forward_tw_cache[i].aT;
	}

	auto tsptw_solution::robust_arrival_T(const int i, const int k) const -> double
	{
		return _forward_robust_tw_cache[i].worst_aT[k];
	}

	auto tsptw_solution::total_penalty() const -> double
	{
		return _penalty_cost;
	}

	/*return all the variables that related to arrival time*/
	auto tsptw_solution::time_updates() const -> const std::vector<double> &
	{
		return _time_related_vars;
	}

	/*Get the current penalty coefficient of node i*//*----right now overall penalty, change to each node?*/
	auto tsptw_solution::alpha(const int i) const -> double
	{
		return _alpha[i];
	}

	auto tsptw_solution::alpha_set() const -> double
	{
		return _alpha_set;
	}

	auto tsptw_solution::set_alpha(double alpha) -> void
	{
		for (auto i = 0; i <= _inst->_num_node; i++)
		{
			_alpha[i] = alpha;
		}
	}

	auto tsptw_solution::set_alpha_set(double alpha) -> void
	{
		_alpha_set = alpha;
	}

	auto tsptw_solution::shake_iter_max() const-> const int
	{
		//int kmax = (_inst->_num_node - 1) / 5;
		int kmax = std::min(8, std::max(1, (_inst->_num_node - 1) / 5));

		return kmax;
	}

	auto tsptw_solution::update_penalty(double maximum, double minimum, double delta, int steps) -> void
	{
		/*update penalties*/
		if (_tw_violation != 0)
		{
			alphaIterationInfeasible++;
		}

		if (alphaIterationInfeasible == steps)
		{
			for (const auto i : _tour)
			{
				bool vio_status = false;

				if (problem_type == ProbType::robust)
				{
					for (int j = 0; j != i; j = _next[j])
					{
						if (robust_arrival_T(i, j) > _inst->latest(i))
						{
							vio_status = true;
							break;
						}
					}
				}
				else
				{
					if (nominal_arrival_T(i) > _inst->latest(i))
					{
						vio_status = true;
					}
				}

				if (vio_status)
				{
					_alpha[i] = std::min((_alpha[i] * delta), maximum);
				}
				else
				{
					_alpha[i] = std::max((_alpha[i] / delta), minimum);
				}
			}

			alphaIterationInfeasible = 0;
		}
	}

	auto tsptw_solution::update_penalty_set(double maximum, double minimum, double delta, int steps) -> void
	{
		/*update penalties*/
		if (_tw_violation != 0)
		{
			alphaIterationInfeasible++;
		}

		if (alphaIterationInfeasible == steps)
		{
			bool vio_status = false;

			for (const auto i : _tour)
			{
				if (problem_type == ProbType::robust)
				{
					for (int j = 0; j != i; j = _next[j])
					{
						if (robust_arrival_T(i, j) > _inst->latest(i))
						{
							vio_status = true;
							break;
						}
					}
				}
				else
				{
					if (nominal_arrival_T(i) > _inst->latest(i))
					{
						vio_status = true;
					}
				}

				if (vio_status)
				{
					break;
				}
			}

			if (vio_status)
			{
				_alpha_set = std::min((_alpha_set * delta), maximum);
			}
			else
			{
				_alpha_set = std::max((_alpha_set / delta), minimum);
			}

			alphaIterationInfeasible = 0;
		}
	}

	/*Checks if the given tour is feasible*/
	auto tsptw_solution::feasible() const -> bool
	{
		return (tw_violation() == 0.0) && (_unrouted_nodes.size() == 0);
	}

	auto tsptw_solution::operator <(tsptw_solution& sol) -> bool
	{
		if (feasible() == sol.feasible())
		{
			if (_objective == sol._objective)
			{
				return _objective2 < sol._objective2;
			}
			else
			{
				return (_objective < sol._objective);
			}
		}

		return feasible() && !sol.feasible();
	}

	auto tsptw_solution::solution_represent(const std::vector<int> obj_tour)->void
	{
		for (int i = 0; i <= _inst->_num_node; i++)
		{
			auto node = obj_tour[i];

			if (node == 0)
			{
				_previous[node] = -1;
			}
			else
			{
				auto prev = i - 1;
				_previous[node] = obj_tour[prev];
			}

			if (node == _inst->_num_node)
			{
				_next[node] = -1;
			}
			else
			{
				auto next = i + 1;
				_next[node] = obj_tour[next];
			}
		}

		update_tour();
	}

	/*Updates all helper variables of the current solution.
	* Called by Solution::applyMove()*/
	auto tsptw_solution::update_tour() -> void
	{
		_objective = 0.0;
		_objective2 = 0.0;
		_check_objective = 0.0;
		_cost = 0.0;
		_tw_violation = 0.0;
		_penalty_cost = 0.0;

		/*update the tour*/
		_tour.clear();

		std::fill(_position.begin(), _position.end(), -1);
		std::fill(_node_in_position.begin(), _node_in_position.end(), 0);

		int position = 0;
		for (int i = 0; i != -1; i = _next[i])
		{
			_tour.push_back(i);

			_position[i] = position;

			_node_in_position[position] = i;

			position++;
		}

		/*update the cost*/
		for (int i = 0; i < _inst->_num_node; i = _next[i])
		{
			_cost += _inst->cost(i, _next[i]);
		}

		/*update the forward time cache in either nominal or robust cases*/
		/*for the depot*/
		auto& fc = _forward_tw_cache[0];
		auto& rfc = _forward_robust_tw_cache[0];
		auto& rfc_max = _forward_max_robust_tw_cache[0];

		fc.dur = 0.0;
		fc.tw = 0.0;
		fc.earl = _inst->earliest(0);
		fc.lat = _inst->latest(0);
		fc.aT = 0.0;
		fc.penalty = 0.0;
		fc.min_slack = _inst->latest(0);
		fc.min_budget = BigM;

		if (problem_type == ProbType::robust)
		{
			rfc.min_slack_scen = 0;
			rfc.rmin_slack = _inst->latest(0);

			if (BUDGET_INTERVAL)
			{
				rfc_max.min_slack_scen = 0;
				rfc_max.rmin_slack = _inst->latest(0);
			}
		}

		if (TIME_UNCERTAIN)
		{
			for (int k = 0; k <= _inst->_num_node; k++)
			{
				fc.cumulated_waitings[k] = 0.0;
				fc.cumulated_delay_inf[k] = 0.0;

				if (problem_type == ProbType::robust)
				{
					rfc.robust_dur[k] = 0.0;
					rfc.robust_tw[k] = 0.0;
					rfc.robust_earl[k] = _inst->earliest(0);
					rfc.robust_lat[k] = _inst->latest(0);
					rfc.worst_aT[k] = 0.0;
					rfc.worst_penalty[k] = 0.0;
					rfc.cumulated_delay[k] = 0.0;

					if (BUDGET_INTERVAL)
					{
						rfc_max.robust_dur[k] = 0.0;
						rfc_max.robust_tw[k] = 0.0;
						rfc_max.robust_earl[k] = _inst->earliest(0);
						rfc_max.robust_lat[k] = _inst->latest(0);
						rfc_max.worst_aT[k] = 0.0;
						rfc_max.cumulated_delay[k] = 0.0;
					}
				}
			}
		}

		/*for other nodes*/
		double max_violation = 0.0, max_penalty = 0.0;
		int max_vio_scen = 0;
		int max_pen_scen = 0;

		for (int i = 0, j = _next[i]; j != -1; i = j, j = _next[j])
		{
			const auto& ic = _forward_tw_cache[i];
			auto& jc = _forward_tw_cache[j];

			const auto& ric = _forward_robust_tw_cache[i];
			auto& rjc = _forward_robust_tw_cache[j];

			const auto& ric_max = _forward_max_robust_tw_cache[i];
			auto& rjc_max = _forward_max_robust_tw_cache[j];

			/*first update the nominal cache---------------------------------*/
			const auto delta = ic.dur + _inst->min_time(i, j);
			const auto delta_wt = std::max((_inst->earliest(j) - delta - ic.lat), 0.0);
			const auto delta_tw = std::max((ic.earl + delta - _inst->latest(j)), 0.0);

			jc.dur = ic.dur + _inst->min_time(i, j) + delta_wt;
			jc.tw = ic.tw + delta_tw;
			jc.earl = std::max((_inst->earliest(j) - delta), ic.earl) - delta_wt;
			jc.lat = std::min((_inst->latest(j) - delta), ic.lat) + delta_tw;

			jc.aT = ic.dur + ic.earl + _inst->min_time(i, j);
			double dT = jc.dur + jc.earl;

			/*min slack in nominal case*/
			double slack = _inst->latest(j) - jc.aT;
			if (slack < ic.min_slack)
			{
				jc.min_slack = slack;
			}
			else
			{
				jc.min_slack = ic.min_slack;
			}

			/*nominal tw violation*/
			double tw_vio = std::max(dT - _inst->latest(j), 0.0);
			//jc.penalty = ic.penalty + tw_vio * _alpha[j];
			jc.penalty = jc.tw * _alpha_set;

			/*update the robust cache if the problem is of robust type or of budget-obj type*/
			if (TIME_UNCERTAIN)
			{
				/*initialize*/
				for (int k = 0; k <= _inst->_num_node; k++)
				{
					jc.cumulated_waitings[k] = 0.0;
					jc.cumulated_delay_inf[k] = 0.0;

					if (problem_type == ProbType::robust)
					{
						rjc.robust_dur[k] = jc.dur;
						rjc.robust_tw[k] = jc.tw;
						rjc.robust_earl[k] = jc.earl;
						rjc.robust_lat[k] = jc.lat;
						rjc.worst_aT[k] = jc.aT;
						rjc.worst_penalty[k] = jc.penalty;
						rjc.cumulated_delay[k] = 0.0;

						if (BUDGET_INTERVAL)
						{
							rjc_max.robust_dur[k] = jc.dur;
							rjc_max.robust_tw[k] = jc.tw;
							rjc_max.robust_earl[k] = jc.earl;
							rjc_max.robust_lat[k] = jc.lat;
							rjc_max.worst_aT[k] = jc.aT;
							rjc_max.cumulated_delay[k] = 0.0;
						}
					}
				}

				double minslack_all_scen = BigM, minslack_all_scen_max = BigM;
				int minslack_scen_all_scen = 0, minslack_scen_all_scen_max = 0;

				/*start before j*/
				for (int k = 0; k != j; k = _next[k])
				{
					double rtw_vio = 0.0;
					double rslack = BigM, rslack_max = BigM;

					jc.cumulated_delay_inf[k] = std::min(_inst->delta, (ic.cumulated_delay_inf[k] + _inst->maxdelay(i, j)));
					jc.cumulated_waitings[k] = ic.cumulated_waitings[k] + (dT - jc.aT);//norminal time

					if (problem_type == ProbType::robust)
					{
						if (BUDGET_INTERVAL)
						{
							rjc_max.cumulated_delay[k] = std::min(_inst->delta_max, (ric_max.cumulated_delay[k] + _inst->maxdelay(i, j)));

							double residual_delay = _inst->delta_max - (std::min(_inst->delta_max, ric_max.cumulated_delay[k]));
							double effective_delay = std::min(residual_delay, _inst->maxdelay(i, j));

							const auto rdelta = ric_max.robust_dur[k] + _inst->min_time(i, j) + effective_delay;
							const auto rdelta_wt = std::max((_inst->earliest(j) - rdelta - ric_max.robust_lat[k]), 0.0);
							const auto rdelta_tw = std::max((ric_max.robust_earl[k] + rdelta - _inst->latest(j)), 0.0);

							rjc_max.robust_dur[k] = ric_max.robust_dur[k] + _inst->min_time(i, j) + effective_delay + rdelta_wt;
							rjc_max.robust_tw[k] = ric_max.robust_tw[k] + rdelta_tw;
							rjc_max.robust_earl[k] = std::max((_inst->earliest(j) - rdelta), ric_max.robust_earl[k]) - rdelta_wt;
							rjc_max.robust_lat[k] = std::min((_inst->latest(j) - rdelta), ric_max.robust_lat[k]) + rdelta_tw;

							rjc_max.worst_aT[k] = ric_max.robust_dur[k] + ric_max.robust_earl[k] + _inst->min_time(i, j) + effective_delay;

							rslack_max = _inst->latest(j) - rjc_max.worst_aT[k];
							if (rslack_max < minslack_all_scen_max)
							{
								minslack_all_scen_max = rslack_max;
								minslack_scen_all_scen_max = k;
							}
						}

						rjc.cumulated_delay[k] = std::min(_inst->delta_min, (ric.cumulated_delay[k] + _inst->maxdelay(i, j)));

						double residual_delay = _inst->delta_min - (std::min(_inst->delta_min, ric.cumulated_delay[k]));
						double effective_delay = std::min(residual_delay, _inst->maxdelay(i, j));

						const auto rdelta = ric.robust_dur[k] + _inst->min_time(i, j) + effective_delay;
						const auto rdelta_wt = std::max((_inst->earliest(j) - rdelta - ric.robust_lat[k]), 0.0);
						const auto rdelta_tw = std::max((ric.robust_earl[k] + rdelta - _inst->latest(j)), 0.0);

						rjc.robust_dur[k] = ric.robust_dur[k] + _inst->min_time(i, j) + effective_delay + rdelta_wt;
						rjc.robust_tw[k] = ric.robust_tw[k] + rdelta_tw;
						rjc.robust_earl[k] = std::max((_inst->earliest(j) - rdelta), ric.robust_earl[k]) - rdelta_wt;
						rjc.robust_lat[k] = std::min((_inst->latest(j) - rdelta), ric.robust_lat[k]) + rdelta_tw;

						rjc.worst_aT[k] = ric.robust_dur[k] + ric.robust_earl[k] + _inst->min_time(i, j) + effective_delay;
						double worst_dT = rjc.robust_dur[k] + rjc.robust_earl[k];

						rslack = _inst->latest(j) - rjc.worst_aT[k];
						if (rslack < minslack_all_scen)
						{
							minslack_all_scen = rslack;
							minslack_scen_all_scen = k;
						}

						rtw_vio = std::max(worst_dT - _inst->latest(j), 0.0);

						//rjc.worst_penalty[k] = ric.worst_penalty[k] + _alpha[j] * rtw_vio;
						rjc.worst_penalty[k] = _alpha_set * rjc.robust_tw[k];

						if (j == _inst->_num_node)
						{
							if (rjc.robust_tw[k] > max_violation)
							{
								max_violation = rjc.robust_tw[k];
								max_vio_scen = k;
							}
							if (rjc.worst_penalty[k] > max_penalty)
							{
								max_penalty = rjc.worst_penalty[k];
								max_pen_scen = k;
							}
						}
					}
				}

				/*minimum slack*/
				if (problem_type == ProbType::robust)
				{
					if (BUDGET_INTERVAL)
					{
						if (minslack_all_scen_max < ric_max.rmin_slack)
						{
							rjc_max.rmin_slack = minslack_all_scen_max;
							rjc_max.min_slack_scen = minslack_scen_all_scen_max;
						}
						else
						{
							rjc_max.rmin_slack = ric_max.rmin_slack;
							rjc_max.min_slack_scen = ric_max.min_slack_scen;
						}
					}

					if (minslack_all_scen < ric.rmin_slack)
					{
						rjc.rmin_slack = minslack_all_scen;
						rjc.min_slack_scen = minslack_scen_all_scen;
					}
					else
					{
						rjc.rmin_slack = ric.rmin_slack;
						rjc.min_slack_scen = ric.min_slack_scen;
					}
				}

				/*calculate the min-budget starting from different nodes*/
				if (_inst->_secobj_type == SecObjType::delta || BUDGET_COMPUTE)
				{
					double mindelta = BigM;
					std::vector<double> tempdelta;
					for (int k = i; k >= 0; k = _previous[k])
					{
						double temp_maxdelta = std::max(0.0, (_inst->latest(j) - dT + jc.cumulated_waitings[k]));

						if (jc.cumulated_delay_inf[k] <= jc.cumulated_waitings[k] || (jc.cumulated_delay_inf[k] <= temp_maxdelta))
						{
							temp_maxdelta = BigM;
						}

						bool stop = false;
						tempdelta.clear();
						tempdelta.push_back(temp_maxdelta);

						if (tempdelta.size() > 1)
						{
							for (int it = 0; it < tempdelta.size() - 1; it++)
							{
								auto itplus = it + 1;

								if (tempdelta[itplus] != BigM && (tempdelta[it] != BigM) && (tempdelta[itplus] > tempdelta[it]))
								{
									tempdelta[itplus] = BigM;
									stop = true;
								}
							}
						}

						if (stop) break;

						if (temp_maxdelta != BigM && (temp_maxdelta < mindelta))
						{
							mindelta = temp_maxdelta;
						}
					}
					std::vector<double>().swap(tempdelta);

					if (mindelta < ic.min_budget)
					{
						jc.min_budget = mindelta;
					}
					else
					{
						jc.min_budget = ic.min_budget;
					}
				}
				else
				{
					jc.min_budget = BigM;
				}
			}
			else
			{
				jc.min_budget = BigM;
			}
		}

		/*find the maximum violation and penalty cost in the robust case*/
		if (problem_type == ProbType::robust)
		{
			const auto& lfc = _forward_robust_tw_cache[_inst->_num_node];

			_tw_violation = max_violation;
			_penalty_cost = max_penalty;
		}
		else
		{
			_tw_violation = _forward_tw_cache[_inst->_num_node].tw;
			_penalty_cost = _forward_tw_cache[_inst->_num_node].penalty;
		}

		/*unrouted nodes*/
		_unrouted_nodes.clear();
		for (const auto i : _inst->nodes())
		{
			if (i != 0 && i != _inst->_num_node && (_position[i] == -1))
			{
				_unrouted_nodes.push_back(i);
			}
		}

		for (const auto i : _unrouted_nodes)
		{
			_next[i] = i;
			_previous[i] = i;
		}

		for (const auto i : _inst->nodes())
		{
			_cur_next[i] = _next[i];
			_cur_previous[i] = _previous[i];
		}

		/*violated or non-violated nodes*/
		/*consider the backward manner*/
		_vio_nodes.clear();
		non_vio_nodes.clear();

		for (int i = _previous[_tour.back()]; i != 0; i = _previous[i]) //backward
		{
			if (problem_type == ProbType::robust)
			{
				bool vio_status = false;
				for (int k = 0; k != i; k = _next[k])
				{
					if (robust_arrival_T(i, k) > _inst->latest(i))
					{
						vio_status = true;
						break;
					}
				}

				if (!vio_status)
				{
					non_vio_nodes.push_back(i);
				}
				else
				{
					_vio_nodes.push_back(i);
				}
			}
			else
			{
				if (nominal_arrival_T(i) <= _inst->latest(i))
				{
					non_vio_nodes.push_back(i);
				}
				else
				{
					_vio_nodes.push_back(i);
				}
			}
		}

		/*obj1 and obj2*/
		double prim_obj = 0.0, sec_obj = 0.0, check_obj = 0.0;

		if (_inst->_primobj_type == PrimObjType::distance)
		{
			prim_obj = _cost;
		}
		else
		{
			prim_obj = tour_duration();
		}

		if (_inst->_secobj_type == SecObjType::slack)
		{
			sec_obj = 0.0 - min_slack();

			if (min_budget() != BigM)
			{
				check_obj = 0.0 - min_budget();
			}
			else
			{
				check_obj = BigM;
			}
		}
		else
		{
			if (min_budget() != BigM)
			{
				sec_obj = 0.0 - min_budget();
			}
			else
			{
				sec_obj = BigM;
			}

			check_obj = 0.0 - min_slack();
		}

		if (OPTprim)
		{
			_objective = prim_obj + _penalty_cost;
			_objective2 = sec_obj;
		}
		else
		{
			_objective = sec_obj + _penalty_cost;
			_objective2 = prim_obj;
		}

		_check_objective = check_obj;
	}

}
