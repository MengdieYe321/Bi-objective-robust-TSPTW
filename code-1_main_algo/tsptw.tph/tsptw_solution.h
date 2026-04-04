#pragma once

#include"move.h"

/*
* This class holds all the information of the single obective tsptw solution of the heuristic
*/
namespace tsptw_tph
{
	class tsptw_solution
	{
	public:

		tsptw_solution(const instance& inst);

		~tsptw_solution();

		void init();

		void reset_empty();

		int OPTprim = 1; //currently optimize primary objective

		int phase_feasi = 0;

		/*Gets the problem instance of this solution.*/
		auto inst() const -> const instance &;

		/*return the vector of the tour*/
		auto tour() const -> const std::vector<int> &;

		auto cur_tour() const -> const std::vector<int> &;

		auto unrouted_nodes()const -> const std::vector<int> &;

		auto violated_nodes() const -> const std::vector<int> &;

		auto non_violated_nodes() const -> const std::vector<int> &;

		/*return the position of the node i*/
		auto position(const int i)const  -> int;

		/*return the successor of the given node*/
		auto next(const int i)const  -> int;

		/*return the predecessor of the given node*/
		auto previous(const int i)const -> int;

		/*return the node of the given position*/
		auto node_in_position(const int i)const -> int;

		/*Gets the primary objective for the given tour*/
		auto objective() const -> double;
		/*Gets the secondary objective for the given tour*/
		auto objective2() const -> double;
		auto check_objective() const -> double;

		auto cur_objective() const -> double;
		auto cur_objective2() const -> double;
		auto cur_check_objective() const -> double;

		auto reset_objective(double value1, double value2) ->void
		{
			_objective = value1;
			_objective2 = value2;
		}

		auto cost() const -> double;

		auto makespan() const -> double;

		auto tour_duration() const -> double;

		auto min_rslack_scen()const  -> int;

		auto min_slack()const  -> double;

		auto rmin_slack()const  -> double;

		auto min_budget() const -> double;

		/*Gets the time window violation for the given tour*/
		auto tw_violation() const -> double;

		auto nominal_arrival_T(const int i) const -> double;

		auto robust_arrival_T(const int i, const int k) const -> double;

		auto total_penalty() const -> double;

		/*return all the variables that related to arrival time*/
		auto time_updates() const -> const std::vector<double> &;

		/*Get the current penalty coefficient of node i*//*----right now overall penalty, change to each node?*/
		auto alpha(const int i) const -> double;
		auto alpha_set() const->double;

		auto set_alpha(double alpha) -> void;
		auto set_alpha_set(double alpha) ->void;

		std::size_t alphaIterationFeasible = 0;
		std::size_t alphaIterationInfeasible = 0;

		double penalty_init = 10;
		double penalty_max = 1000.0;
		double penalty_min = 0.01;
		double penalty_delta = 2;
		int penalty_steps = 5;

		double elasped_time = 0.0;

		auto shake_iter_max() const-> const int;

		/*evaluate the time window violation */
		//template <std::size_t N>
		//auto evaluate_tw(const instance::tour::subtours<N>& segs)const->vector<double> &;
		template <typename T>
		auto evaluate_tw(const T& segs) -> bool
		{
			_time_related_vars.clear();//{tw_vio, penalty, CT, TD, min_slack, min_budget}

			/* remember the temp. tour after the move: also useful for the robust case*/
			std::vector<int> temptour;

			if (TIME_UNCERTAIN)
			{
				for (int s = 0; s < segs.size(); s++)
				{
					const auto& seg = segs[s];
					const auto length = _position[seg.last] - _position[seg.first] + 1;
					auto i = seg.first;

					for (int kl = 0; kl < length; ++kl, i = _next[i])
					{
						temptour.push_back(i);
					}
				}

				for (int i = 0; i < temptour.size() - 1; i++)
				{
					if (!_inst->is_arc_feasible({ temptour[i], temptour[i + 1] }))
					{
						return false;
					}
				}
			}

			/*keep track on the time cache: cache of nodes before the last node of the first seg are unchanged*/
			const auto& fc = _forward_tw_cache[segs.front().last];
			double dur = fc.dur;
			double tw = fc.tw;
			double earl = fc.earl;
			double lat = fc.lat;
			double aT = fc.aT;
			double dT = fc.dur + fc.earl;
			double penalty = fc.penalty;
			double min_slack = fc.min_slack;
			double min_budget = fc.min_budget;

			std::vector<double> cumulated_waitings;
			std::vector<double> cumulated_delay_inf;
			
			if (TIME_UNCERTAIN)
			{
				cumulated_waitings = fc.cumulated_waitings;
				cumulated_delay_inf = fc.cumulated_delay_inf;
			}

			double rmin_slack = BigM;
			int min_slack_scen = 0;
			std::vector<double> robust_dur;
			std::vector<double> robust_tw;
			std::vector<double> robust_earl;
			std::vector<double> robust_lat;
			std::vector<double> worst_aT;
			std::vector<double> worst_penalty;
			std::vector<double> cumulated_delay;

			double rmin_slack_max = BigM;
			int min_slack_scen_max = 0;
			std::vector<double> robust_dur_max;
			std::vector<double> robust_tw_max;
			std::vector<double> robust_earl_max;
			std::vector<double> robust_lat_max;
			std::vector<double> worst_aT_max;
			std::vector<double> cumulated_delay_max;

			if (problem_type == ProbType::robust)
			{
				const auto& rfc = _forward_robust_tw_cache[segs.front().last];
				min_slack_scen = rfc.min_slack_scen;
				rmin_slack = rfc.rmin_slack;
				robust_dur = rfc.robust_dur;
				robust_tw = rfc.robust_tw;
				robust_earl = rfc.robust_earl;
				robust_lat = rfc.robust_lat;
				worst_aT = rfc.worst_aT;
				worst_penalty = rfc.worst_penalty;
				cumulated_delay = rfc.cumulated_delay;

				if (BUDGET_INTERVAL)
				{
					const auto& rfc_max = _forward_max_robust_tw_cache[segs.front().last];
					min_slack_scen_max = rfc_max.min_slack_scen;
					rmin_slack_max = rfc_max.rmin_slack;
					robust_dur_max = rfc_max.robust_dur;
					robust_tw_max = rfc_max.robust_tw;
					robust_earl_max = rfc_max.robust_earl;
					robust_lat_max = rfc_max.robust_lat;
					worst_aT_max = rfc_max.worst_aT;
					cumulated_delay_max = rfc_max.cumulated_delay;
				}
			}

			int max_vio_scen = 0, max_pen_scen = 0;
			double max_violation = tw, max_penalty = penalty;

			auto last = segs.front().last;

			for (int s = 1; s < segs.size(); ++s)
			{
				const auto& seg = segs[s];

				const auto length = _position[seg.last] - _position[seg.first] + 1;

				auto i = seg.first;

				double tw_vio = 0.0; //nominal tw violation at this node i

				for (int kl = 0; kl < length; ++kl, i = _next[i])
				{
					if (!_inst->is_arc_feasible({ last, i }))
					{
						return false;
					}

					/*first update the nominal case*/
					double delta = dur + _inst->min_time(last, i);
					double delta_wt = std::max(_inst->earliest(i) - delta - lat, 0.0); //waiting time
					double delta_tw = std::max(earl + delta - _inst->latest(i), 0.0); //time wrap/tw violation

					aT = dur + earl + _inst->min_time(last, i); //dur and earl of last node

					dur = dur + _inst->min_time(last, i) + delta_wt;
					tw = tw + delta_tw;
					earl = std::max(_inst->earliest(i) - delta, earl) - delta_wt;
					lat = std::min(_inst->latest(i) - delta, lat) + delta_tw;

					dT = dur + earl;

					auto slack = _inst->latest(i) - aT;
					if (slack < min_slack)
					{
						min_slack = slack;
					}

					tw_vio = std::max(dT - _inst->latest(i), 0.0);
					//penalty += _alpha[i] * tw_vio;
					penalty = _alpha_set * tw;

					if (phase_feasi && (tw_vio > 0))
					{
						return false;
					}

					if (TIME_UNCERTAIN)
					{
						int i_th = 0;
						while (temptour[i_th] != i)
						{
							i_th++;//i is i-th node
						}

						double minslack_all_scen = BigM, minslack_all_scen_max = BigM;
						int minslack_scen_all_scen = 0, minslack_scen_all_scen_max = 0;

						auto temptour_size = std::min(_inst->_num_node, (int)temptour.size());
						for (int k = 0; k < temptour_size; k++)
						{
							auto tempk = temptour[k];

							double rtw_vio = 0.0; //tw_violation at this node i if delay starts from k
							double rslack = BigM, rslack_max = BigM;

							if (k < i_th)
							{
								if (problem_type == ProbType::robust)
								{
									if (BUDGET_INTERVAL)
									{
										auto residual_delay = _inst->delta_max - std::min(_inst->delta_max, cumulated_delay_max[tempk]);
										auto effective_delay = std::min(residual_delay, _inst->maxdelay(last, i));

										double rdelta = robust_dur_max[tempk] + _inst->min_time(last, i) + effective_delay;
										double rdelta_wt = std::max((_inst->earliest(i) - rdelta - robust_lat_max[tempk]), 0.0);
										double rdelta_tw = std::max((robust_earl_max[tempk] + rdelta - _inst->latest(i)), 0.0);

										worst_aT_max[tempk] = robust_dur_max[tempk] + robust_earl_max[tempk] + _inst->min_time(last, i) + effective_delay;//of last node

										robust_dur_max[tempk] = robust_dur_max[tempk] + _inst->min_time(last, i) + effective_delay + rdelta_wt;
										robust_tw_max[tempk] = robust_tw_max[tempk] + rdelta_tw;
										robust_earl_max[tempk] = std::max((_inst->earliest(i) - rdelta), robust_earl_max[tempk]) - rdelta_wt;
										robust_lat_max[tempk] = std::min((_inst->latest(i) - rdelta), robust_lat_max[tempk]) + rdelta_tw;

										/*slack w.r.t. delta_max*/
										rslack_max = _inst->latest(i) - worst_aT_max[tempk];
										if (rslack_max < minslack_all_scen_max)
										{
											minslack_all_scen_max = rslack_max;
											minslack_scen_all_scen_max = tempk;
										}

										cumulated_delay_max[tempk] = std::min(_inst->delta_max, cumulated_delay_max[tempk] + _inst->maxdelay(last, i));
									}

									/*normal case w.r.t. delta_min*/
									auto residual_delay = _inst->delta_min - std::min(_inst->delta_min, cumulated_delay[tempk]);
									auto effective_delay = std::min(residual_delay, _inst->maxdelay(last, i));

									double rdelta = robust_dur[tempk] + _inst->min_time(last, i) + effective_delay;
									double rdelta_wt = std::max((_inst->earliest(i) - rdelta - robust_lat[tempk]), 0.0);
									double rdelta_tw = std::max((robust_earl[tempk] + rdelta - _inst->latest(i)), 0.0);

									worst_aT[tempk] = robust_dur[tempk] + robust_earl[tempk] + _inst->min_time(last, i) + effective_delay;//of last node

									robust_dur[tempk] = robust_dur[tempk] + _inst->min_time(last, i) + effective_delay + rdelta_wt;
									robust_tw[tempk] = robust_tw[tempk] + rdelta_tw;
									robust_earl[tempk] = std::max((_inst->earliest(i) - rdelta), robust_earl[tempk]) - rdelta_wt;
									robust_lat[tempk] = std::min((_inst->latest(i) - rdelta), robust_lat[tempk]) + rdelta_tw;

									double worst_dT = robust_dur[tempk] + robust_earl[tempk];

									cumulated_delay[tempk] = std::min(_inst->delta_min, cumulated_delay[tempk] + _inst->maxdelay(last, i));

									/*slack*/
									rslack = _inst->latest(i) - worst_aT[tempk];
									if (rslack < minslack_all_scen)
									{
										minslack_all_scen = rslack;
										minslack_scen_all_scen = tempk;
									}

									/*tw violation and penalty cost*/
									rtw_vio = std::max(worst_dT - _inst->latest(i), 0.0);
									//worst_penalty[tempk] += _alpha[i] * rtw_vio;
									worst_penalty[tempk] = _alpha_set * robust_tw[tempk];

									if (phase_feasi && (rtw_vio > 0))
									{
										return false;
									}
								}

								cumulated_delay_inf[tempk] = std::min(_inst->delta, cumulated_delay_inf[tempk] + _inst->maxdelay(last, i));
								cumulated_waitings[tempk] += (dT - aT);
							}
							else
							{
								if (problem_type == ProbType::robust)
								{
									if (BUDGET_INTERVAL)
									{
										robust_dur_max[tempk] = dur;
										robust_tw_max[tempk] = tw;
										robust_earl_max[tempk] = earl;
										robust_lat_max[tempk] = lat;
										worst_aT_max[tempk] = aT;

										rslack_max = slack;
										if (rslack_max < minslack_all_scen_max)
										{
											minslack_all_scen_max = rslack_max;
											minslack_scen_all_scen_max = i;
										}
									}

									robust_dur[tempk] = dur;
									robust_tw[tempk] = tw;
									robust_earl[tempk] = earl;
									robust_lat[tempk] = lat;
									worst_aT[tempk] = aT;

									rslack = slack;
									if (rslack < minslack_all_scen)
									{
										minslack_all_scen = rslack;
										minslack_scen_all_scen = i;
									}

									rtw_vio = tw_vio;
									worst_penalty[tempk] = penalty;

									if (phase_feasi && (rtw_vio > 0))
									{
										return false;
									}
								}
							}

							if (problem_type == ProbType::robust)/*always w.r.t. delta_min*/
							{
								if (i == _inst->_num_node)
								{
									if (robust_tw[tempk] > max_violation)
									{
										max_violation = robust_tw[tempk];
										max_vio_scen = tempk;
									}
									if (worst_penalty[tempk] > max_penalty)
									{
										max_penalty = worst_penalty[tempk];
										max_pen_scen = tempk;
									}
								}
							}
						}

						if (problem_type == ProbType::robust)/*different in interval case*/
						{
							if (minslack_all_scen < rmin_slack)
							{
								rmin_slack = minslack_all_scen;
								min_slack_scen = minslack_scen_all_scen;
							}

							if (BUDGET_INTERVAL)
							{
								if (minslack_all_scen_max < rmin_slack_max)
								{
									rmin_slack_max = minslack_all_scen_max;
									min_slack_scen_max = minslack_scen_all_scen_max;
								}
							}
						}

						if (_inst->_secobj_type==SecObjType::delta || BUDGET_COMPUTE)
						{
							int last_i = i_th - 1;

							/*calculate the min-budget (backward)*/
							double mindelta = BigM;
							std::vector<double> tempdelta;

							while (last_i >= 0)
							{
								auto node_last_i = temptour[last_i];

								double temp_maxdelta = std::max(0.0, (_inst->latest(i) - dT + cumulated_waitings[node_last_i]));

								if (cumulated_delay_inf[node_last_i] <= cumulated_waitings[node_last_i] || (cumulated_delay_inf[node_last_i] <= temp_maxdelta))
								{
									temp_maxdelta = BigM;
								}

								bool stop = false;
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

								last_i--;
							}

							if (mindelta < min_budget)
							{
								min_budget = mindelta;
							}

							std::vector<double>().swap(tempdelta);
						}
						else
						{
							min_budget = BigM;
						}
					}
					else
					{
						min_budget = BigM;
					}

					last = i;
				}
			}

			if (problem_type != ProbType::robust)
			{
				_time_related_vars.push_back(tw);
				_time_related_vars.push_back(penalty);
				_time_related_vars.push_back(dT);
				_time_related_vars.push_back(dur);
				_time_related_vars.push_back(min_slack);
			}
			else
			{
				//const auto& lfc = _forward_robust_tw_cache[_inst->_num_node];
				//const auto& lfc_max = _forward_max_robust_tw_cache[_inst->_num_node];

				_time_related_vars.push_back(max_violation);

				//int scen = 0;
				double completion = 0.0, duration = 0.0;
				if (!BUDGET_INTERVAL)
				{
					/*scen = lfc.min_slack_scen;
					completion = lfc.robust_dur[scen] + lfc.robust_earl[scen];
					duration = lfc.robust_dur[scen];*/
					for (int i = 0; i <= inst()._num_node; i++)
					{
						auto td = robust_dur[i];
						auto ct = robust_dur[i] + robust_earl[i];

						if (td > duration)
						{
							duration = td;
						}

						if (ct > completion)
						{
							completion = ct;
						}
					}
				}
				else
				{
					/*scen = lfc_max.min_slack_scen;
					completion = lfc_max.robust_dur[scen] + lfc_max.robust_earl[scen];
					duration = lfc_max.robust_dur[scen];*/
					for (int i = 0; i <= inst()._num_node; i++)
					{
						auto td = robust_dur_max[i];
						auto ct = robust_dur_max[i] + robust_earl_max[i];

						if (td > duration)
						{
							duration = td;
						}

						if (ct > completion)
						{
							completion = ct;
						}
					}
				}

				_time_related_vars.push_back(max_penalty);
				_time_related_vars.push_back(completion);
				_time_related_vars.push_back(duration);

				if (!BUDGET_INTERVAL)
				{
					_time_related_vars.push_back(rmin_slack);
				}
				else
				{
					_time_related_vars.push_back(rmin_slack_max);
				}
			}

			_time_related_vars.push_back(min_budget);

			return true;
		}

		/*claculate the objectives of the move */
		template <std::size_t M, std::size_t N, std::string(*F)()>
		auto evaluate_move(move<M, N, F>& m) -> void
		{
			double prim_obj = 0.0;
			double sec_obj = 0.0;
			double check_obj = 0.0;

			if (_inst->_primobj_type == PrimObjType::distance)
			{
				prim_obj = m.costs;
			}
			else
			{
				prim_obj = m.TD;
			}

			if (_inst->_secobj_type == SecObjType::slack)
			{
				sec_obj = 0.0 - m.min_slack;

				if (m.min_budget > 10000)
				{
					check_obj = BigM;
				}
				else
				{
					check_obj = 0.0 - m.min_budget;
				}
			}
			else
			{
				if (m.min_budget > 10000)
				{
					sec_obj = BigM;
				}
				else
				{
					sec_obj = 0.0 - m.min_budget;
				}

				check_obj = 0.0 - m.min_slack;
			}

			if (OPTprim)
			{
				m.obj1 = prim_obj;
				m.obj2 = sec_obj;
			}
			else
			{
				m.obj1 = sec_obj;
				m.obj2 = prim_obj;
			}

			m.check_obj = check_obj;

			m.gen_objective = m.obj1 + m.penalty;
		}

		/*apply the given move to the current solution*/
		template <std::size_t M, std::size_t N, std::string(*F)()>
		auto apply_move(move<M, N, F>& m) -> void
		{
			for (const instance::arc& a : m.inserted)
			{
				const auto i = a.i;
				const auto j = a.j;

				if (i == 0)  /*depot-out arcs*/
				{
					_next[0] = j;
					_previous[j] = 0;
					_previous[0] = -1;
				}
				else if (j == _inst->_num_node)/*depot-in arcs*/
				{
					_next[i] = _inst->_num_node;
					_previous[_inst->_num_node] = i;
					_next[_inst->_num_node] = -1;
				}
				else  /*insert arc*/
				{
					_next[i] = j;
					_previous[j] = i;
				}
			}

			if (m.name() == "2-Opt" && (m.inverted_seqs.size() > 0))
			{
				auto last = m.inverted_seqs.front().last;

				for (auto it = 1; it < m.inverted_seqs.size(); it++)
				{
					const auto& seg = m.inverted_seqs[it];

					auto i = seg.first;

					_next[last] = i;
					_previous[i] = last;

					last = i;
				}
			}

			update_tour();
		}

		auto update_penalty(double maximum, double minimum, double delta, int steps) -> void;

		auto update_penalty_set(double maximum, double minimum, double delta, int steps) -> void;

		/*Checks if the given tour is feasible*/
		auto feasible() const -> bool;

		/**
	* Check if a given move is feasible.
	* @param mov Move.
	* @return True if move is feasible.
	*/
		template <std::size_t M, std::size_t N, std::string(*F)()>
		auto is_move_feasible(const move<M, N, F>& m) const -> bool
		{
			return (_tw_violation + m.tw_violation == 0.0);
		}

		/*check whether the move is valid*/
		template <std::size_t M, std::size_t N, std::string(*F)()>
		auto is_move_valid(const move<M, N, F>& m) const -> bool
		{
			return (m.gen_objective != std::numeric_limits<double>::infinity());
		}

		/**
		* Check if a given move is improving the solution.
		* @param mov Move.
		* @return True if move is improving.
		*/
		template <std::size_t M, std::size_t N, std::string(*F)()>
		auto is_move_improving(const move<M, N, F>& m) const -> bool
		{
			return m.is_improving();
		}

		template <std::size_t M, std::size_t N, std::string(*F)()>
		auto decreases_obj1(const move<M, N, F>& m) const -> bool
		{
			return m.decrease_obj1();
		}

		auto operator <(tsptw_solution& sol) -> bool;

		/*data structure for storing the non-dominated solutions during the local search*/
		template <std::size_t M, std::size_t N, std::string(*F)()>
		auto store_temp_solution(move<M, N, F>& m) -> void
		{
			for (const instance::arc& a : m.inserted)
			{
				const auto i = a.i;
				const auto j = a.j;

				if (i == 0)  /*depot-out arcs*/
				{
					_cur_next[0] = j;
					_cur_previous[j] = 0;
					_cur_previous[0] = -1;
				}
				else if (j == _inst->_num_node)/*depot-in arcs*/
				{
					_cur_next[i] = _inst->_num_node;
					_cur_previous[_inst->_num_node] = i;
					_cur_next[_inst->_num_node] = -1;
				}
				else  /*insert arc*/
				{
					_cur_next[i] = j;
					_cur_previous[j] = i;
				}
			}

			if (m.name() == "2-Opt" && (m.inverted_seqs.size() > 0))
			{
				auto last = m.inverted_seqs.front().last;

				for (auto it = 1; it < m.inverted_seqs.size(); it++)
				{
					const auto& seg = m.inverted_seqs[it];

					auto i = seg.first;

					_cur_next[last] = i;
					_cur_previous[i] = last;

					last = i;
				}
			}

			_cur_objective = _cur_objective2 = _cur_check_objective = 0.0;
			_cur_tour.clear();
			//temparoraly update the tour to store
			for (int i = 0; i != -1; i = _cur_next[i])
			{
				_cur_tour.push_back(i);
			}

			if (OPTprim)
			{
				_cur_objective = _objective + m.obj1;
				_cur_objective2 = _objective2 + m.obj2; //minus valus
			}
			else
			{
				_cur_objective = _objective2 + m.obj2;
				_cur_objective2 = _objective + m.obj1;
			}

			_cur_check_objective = _check_objective + m.check_obj;

			//recover the position structure into that of the current solution
			for (auto i : _inst->nodes())
			{
				_cur_next[i] = next(i);
				_cur_previous[i] = previous(i);
			}
		}

		auto solution_represent(const std::vector<int> obj_tour)->void;

	protected:

		const instance* _inst;

		double _objective;
		double _objective2; //minus value
		double _check_objective; //minus value
		double _cost;
		double _tw_violation;
		double _penalty_cost;

		std::vector<double> _alpha;
		double _alpha_set;

		std::vector<int> _tour;

		std::vector<int>_node_in_position;

		std::vector<int> _unrouted_nodes;
		std::vector<int> non_vio_nodes;
		std::vector<int> _vio_nodes;

		std::vector<int> _position;
		std::vector<int> _previous;
		std::vector<int> _next;

		/*used for temporarily store feasible solution during the search*/
		double _cur_objective; 
		double _cur_objective2;//minus value
		double _cur_check_objective;
		std::vector<int> _cur_tour;
		std::vector<int> _cur_previous;
		std::vector<int> _cur_next;

		struct tw_cache
		{
			double dur;
			double tw;
			double earl;
			double lat;

			double aT;
			double penalty;

			double min_slack; //min slack over all traversed node
			double min_budget;//min budget over all traversed node

			std::vector<double> cumulated_waitings;
			std::vector<double> cumulated_delay_inf;
		};

		struct robust_tw_cache
		{
			double rmin_slack;
			int min_slack_scen;

			std::vector<double> robust_dur; //minimum duration from starting node to node i: sum of travel time + waiting start at the latest possible time->TD of the node
			std::vector<double> robust_tw; //time warp == sum_violation
			std::vector<double> robust_earl; //earlist possible visiting time at the beginning node
			std::vector<double> robust_lat; //latest possible visiting time at the beginning node: the minimum is the forward time slack

			std::vector<double> worst_aT;
			std::vector<double> worst_penalty;

			std::vector<double> cumulated_delay;
		};

		std::vector<tw_cache> _forward_tw_cache; //num_node + 1

		std::vector<robust_tw_cache> _forward_robust_tw_cache; //num_node + 1
		std::vector<robust_tw_cache> _forward_max_robust_tw_cache;

		std::vector<double> _time_related_vars;

		/*Updates all helper variables of the current solution.
		* Called by Solution::applyMove()*/
		auto update_tour() -> void;
	};
}
