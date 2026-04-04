#include <random>
#include<time.h>

#include "neighborhoods.hpp"

namespace tsptw_tph
{
	neighborhood::neighborhood(tsptw_solution& sol, tsptw_solution& best, nondominate_list& non)
		: _sol(sol)
		, _best(best)
		, non(non)
	{
		//empty
	}

	/*====================================================================================*/
	/*relocate-one move for vio and nonvio nodes*/
	/*first improvement: if find feasible solution stop, else store the best solution*/
	auto neighborhood::one_shift_ls(bool first_improve, double time_limit) -> bool
	{
		time_t start, end;
		time(&start);

		double elasped_time = 0.0;

		int k = 1;
		while (k <= 4 && elasped_time <= time_limit)
		{
			move<dynamic> m;

			if (k == 1)
			{
				back_relocate_vionodes(first_improve, m);
			}
			else if (k == 2)
			{
				for_relocatenonvionodes(first_improve, m);
			}
			else if (k == 3)
			{
				back_relocatenonvionodes(first_improve, m);
			}
			else
			{
				for_relocate_vionodes(first_improve, m);
			}

			if (_sol.is_move_valid(m))
			{
				if (_sol.is_move_improving(m))
				{
					_sol.apply_move(m);

					if (_sol.feasible())
					{
						return true;
					}

					k = 1;
				}
				else
				{
					k++;
				}
				//_sol.update_penalty(_sol.penalty_max, _sol.penalty_min, _sol.penalty_delta, _sol.penalty_steps);
				_sol.update_penalty_set(_sol.penalty_max, _sol.penalty_min, _sol.penalty_delta, _sol.penalty_steps);
			}
			else
			{
				k++;
			}

			time(&end);

			elasped_time = difftime(end, start);
		}

		if (elasped_time > time_limit) return false;

		return true;
	}

	/*vio_nodes backward movement*///(i, j) -> { vio_node, 0..._tour[_position[vio] - 1] }
	auto neighborhood::back_relocate_vionodes(bool first_improve, move<dynamic>& m) -> void
	{
		_best_mov = m;

		relocate_one_backward::move_type m_back;

		for (const auto i : _sol.violated_nodes())
		{
			auto index = _sol.position(i) - 1;

			while (index > 0)
			{
				auto prei = _sol.node_in_position(index);

				if (relocate_one_backward::get_move({ i, prei }, _sol, m_back, _best_mov))
				{
					_sol.evaluate_move(m_back);

					if (_sol.is_move_valid(m_back))
					{
						if (_sol.is_move_feasible(m_back))
						{
							m = m_back;

							_best_mov = m;

							return;
						}

						if (m_back < _best_mov)
						{
							_best_mov = m_back;
						}

						if (first_improve)
						{
							if (is_aspiration(m_back) && (m_back.gen_objective < 0.0))//improving the obj
							{
								m = m_back;

								return;
							}
						}
					}
				}

				index--;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	/*nonvio_nodes forward movement*///(i,j) -> { node, non_vio_node, _tour[_position[non] + 1]}
	auto neighborhood::for_relocatenonvionodes(bool first_improve, move<dynamic>& m) -> void
	{
		_best_mov = m;

		relocate_one_forward::move_type m_for;

		for (const auto i : _sol.non_violated_nodes())
		{
			auto index = _sol.position(i) + 1;

			while (index < _sol.inst()._num_node)
			{
				auto nexti = _sol.node_in_position(index);

				if (relocate_one_forward::get_move({ i, nexti }, _sol, m_for, _best_mov))
				{
					_sol.evaluate_move(m_for);

					if (_sol.is_move_valid(m_for))
					{
						if (_sol.is_move_feasible(m_for))
						{
							m = m_for;

							_best_mov = m;

							return;
						}

						if (m_for < _best_mov)
						{
							_best_mov = m_for;
						}

						if (first_improve)
						{
							if (is_aspiration(m_for) && (m_for.gen_objective < 0.0))//improving the obj
							{
								m = m_for;

								return;
							}
						}
					}
				}

				index++;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	/*nonvio_nodes backward movement*/
	auto neighborhood::back_relocatenonvionodes(bool first_improve, move<dynamic>& m) -> void
	{
		_best_mov = m;

		relocate_one_backward::move_type m_back;

		for (const auto i : _sol.non_violated_nodes())
		{
			auto index = _sol.position(i) - 1; //one pos before the current node i

			while (index > 0)
			{
				auto prei = _sol.node_in_position(index);

				if (relocate_one_backward::get_move({ i, prei }, _sol, m_back, _best_mov))
				{
					_sol.evaluate_move(m_back);

					if (_sol.is_move_valid(m_back))
					{
						if (_sol.is_move_feasible(m_back))
						{
							m = m_back;

							_best_mov = m;

							return;
						}

						if (m_back < _best_mov)
						{
							_best_mov = m_back;
						}

						if (first_improve)
						{
							if (is_aspiration(m_back) && (m_back.gen_objective < 0.0))//improving the obj
							{
								m = m_back;

								return;
							}
						}
					}
				}

				index--;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	/*vio_nodes forward movement*/
	auto neighborhood::for_relocate_vionodes(bool first_improve, move<dynamic>& m) -> void
	{
		_best_mov = m;

		relocate_one_forward::move_type m_for;

		for (const auto i : _sol.violated_nodes())
		{
			auto index = _sol.position(i) + 1;

			while (index < _sol.inst()._num_node)
			{
				auto nexti = _sol.node_in_position(index);

				if (relocate_one_forward::get_move({ i, nexti }, _sol, m_for, _best_mov))
				{
					_sol.evaluate_move(m_for);

					if (_sol.is_move_valid(m_for))
					{
						if (_sol.is_move_feasible(m_for))
						{
							m = m_for;

							_best_mov = m;

							return;
						}

						if (m_for < _best_mov)
						{
							_best_mov = m_for;
						}

						if (first_improve)
						{
							if (is_aspiration(m_for) && (m_for.gen_objective < 0.0))//improving the obj
							{
								m = m_for;

								return;
							}
						}
					}
				}

				index++;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	/*====================================================================================*/
	/*combination of different move*/
	/*best improvement*/
	/*olny consider feasible moves*/
	auto neighborhood::vnd(bool first_improve, double time_limit) -> bool
	{
		time_t start, end;
		time(&start);

		double outer_elasped_time = 0.0;
		double inner_elasped_time = 0.0;

		double best_value = _best.objective();
		double last_best_value = best_value;

		while (true && outer_elasped_time <= time_limit)
		{
			move<dynamic> m;

			/*explore each neighborhood using best improvement*/
			int k = 1, km = 6;
			while (k <= km && inner_elasped_time <= time_limit)
			{
				m.reset();

				if (k == 1)
				{
					back_relocate_two(first_improve, m);
				}
				else if (k == 2)
				{
					for_relocate_two(first_improve, m);
				}
				else if (k == 3)
				{
					one_opt_move(first_improve, m);//swap_node(first_improve, m);
				}
				else if (k == 4)
				{
					back_relocate_one(first_improve, m);
				}
				else if(k == 5)
				{
					for_relocate_one(first_improve, m);
				}
				else
				{
					two_opt_move(first_improve, m);
				}

				if (_sol.is_move_valid(m))
				{
					if (_sol.is_move_improving(m))
					{
						_sol.apply_move(m);

						k = 1;
					}
					else
					{
						k++;
					}
				}
				else
				{
					k++;
				}

				time(&end);
				inner_elasped_time = difftime(end, start);
			}

			best_value = _sol.objective();

			time(&end);
			outer_elasped_time = difftime(end, start);

			if (best_value < last_best_value)
			{
				last_best_value = best_value;
			}
			else
			{
				break;
			}
		}

		if (outer_elasped_time > time_limit) return false;

		return true;
	}

	/*====================================================================================*/
	/*separete moves*/
	/*or-opt-1, relocate 1 customer to another position*/
	auto neighborhood::back_relocate_one(bool first_improve, move<dynamic>& m) -> void
	{
		move<dynamic> _best_mov;

		/*backward*/
		relocate_one_backward::move_type m_back;
		for (int i = _sol.inst()._num_node - 1; i > 1; i--) /*stop somewhere for duration obj*/
		{
			auto nod_i = _sol.node_in_position(i);

			/*if (problem_type != ProbType::robust && !_sol.OPTprim && (_sol.nominal_arrival_T(nod_i) <= _sol.inst().earliest(nod_i)))
			{
				if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
				{
					break;
				}
			}*/

			auto j = i - 1;

			while (j > 0)
			{
				auto nod_j = _sol.node_in_position(j);

				if (relocate_one_backward::get_move({ nod_i, nod_j }, _sol, m_back, _best_mov))
				{
					_sol.evaluate_move(m_back);

					if (_sol.is_move_feasible(m_back))
					{
						/*try to store new feasible solution*/
						_sol.store_temp_solution(m_back);

						non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

						bool within_bound = non.is_within_bound(_sol.cur_objective(), _sol.cur_objective2(), _sol.OPTprim);//obj2 is minus
						if (within_bound)
						{
							if (m_back < _best_mov)
							{
								_best_mov = m_back;
							}

							if (first_improve)
							{
								if (is_aspiration(m_back) && (m_back.gen_objective < 0.0))//improving the obj
								{
									m = m_back;

									return;
								}
							}
						}
					}
				}

				j--;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	auto neighborhood::for_relocate_one(bool first_improve, move<dynamic>& m) -> void
	{
		move<dynamic> _best_mov;

		/*avoid the same move as before: also consider backward manner*/
		relocate_one_forward::move_type m_for;
		for (int i = _sol.inst()._num_node - 2; i > 0; i--)
		{
			auto nod_i = _sol.node_in_position(i);

			/*if (problem_type != ProbType::robust && !_sol.OPTprim && (_sol.nominal_arrival_T(_sol.next(nod_i)) <= _sol.inst().earliest(_sol.next(nod_i))))
			{
				if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
				{
					break;
				}
			}*/

			auto j = i + 1; 

			while (j < _sol.inst()._num_node)
			{
				auto nod_j = _sol.node_in_position(j);

				/*if (problem_type != ProbType::robust && !_sol.OPTprim && (_sol.nominal_arrival_T(_sol.next(nod_j)) <= _sol.inst().earliest(_sol.next(nod_j))))
				{
					if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
					{
						break;
					}
				}*/

				if (relocate_one_forward::get_move({ nod_i, nod_j }, _sol, m_for, _best_mov))
				{
					_sol.evaluate_move(m_for);

					if (_sol.is_move_feasible(m_for))
					{
						/*try to store new feasible solution*/
						_sol.store_temp_solution(m_for);

						non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

						bool within_bound = non.is_within_bound(_sol.cur_objective(), _sol.cur_objective2(), _sol.OPTprim);
						if (within_bound)
						{
							if (m_for < _best_mov)
							{
								_best_mov = m_for;
							}

							if (first_improve)
							{
								if (is_aspiration(m_for) && (m_for.gen_objective < 0.0))
								{
									m = m_for;

									return;
								}
							}
						}
					}
				}

				j++;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	auto neighborhood::back_relocate_two(bool first_improve, move<dynamic>& m) -> void
	{
		move<dynamic> _best_mov;

		/*backward*/
		relocate_two_backward::move_type m_back;
		for (int i = _sol.inst()._num_node - 2; i > 1; i--)
		{
			auto nod_i = _sol.node_in_position(i);
			auto nod_i_plus = _sol.next(nod_i);

			/*if (problem_type != ProbType::robust && !_sol.OPTprim && ((_sol.nominal_arrival_T(nod_i) <= _sol.inst().earliest(nod_i)) || (_sol.nominal_arrival_T(nod_i_plus) <= _sol.inst().earliest(nod_i_plus))))
			{
				if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
				{
					break;
				}
			}*/

			auto j = i - 1;

			while (j > 0)
			{
				auto nod_j = _sol.node_in_position(j);

				if (relocate_two_backward::get_move({ nod_i, nod_j }, _sol, m_back, _best_mov))
				{
					_sol.evaluate_move(m_back);

					if (_sol.is_move_feasible(m_back))
					{
						/*try to store new feasible solution*/
						_sol.store_temp_solution(m_back);

						non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

						bool within_bound = non.is_within_bound(_sol.cur_objective(), _sol.cur_objective2(), _sol.OPTprim);
						if (within_bound)
						{
							if (m_back < _best_mov)
							{
								_best_mov = m_back;
							}

							if (first_improve)
							{
								if (is_aspiration(m_back) && (m_back.gen_objective < 0.0))
								{
									m = m_back;

									return;
								}
							}
						}
					}
				}

				j--;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	auto neighborhood::for_relocate_two(bool first_improve, move<dynamic>& m) -> void
	{
		move<dynamic> _best_mov;

		/*avoid the same move as before: also consider backward manner*/
		relocate_two_forward::move_type m_for;
		for (int i = _sol.inst()._num_node - 3; i > 0; i--)
		{
			auto nod_i = _sol.node_in_position(i);
			auto nod_i_plus = _sol.next(nod_i);

			/*if (problem_type != ProbType::robust && !_sol.OPTprim && ((_sol.nominal_arrival_T(nod_i) <= _sol.inst().earliest(nod_i)) || (_sol.nominal_arrival_T(nod_i_plus) <= _sol.inst().earliest(nod_i_plus))))
			{
				if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
				{
					break;
				}
			}*/

			auto j = i + 2;

			while (j < _sol.inst()._num_node)
			{
				auto nod_j = _sol.node_in_position(j);

				/*if (problem_type != ProbType::robust && !_sol.OPTprim && (_sol.nominal_arrival_T(_sol.next(nod_j)) <= _sol.inst().earliest(_sol.next(nod_j))))
				{
					if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
					{
						break;
					}
				}*/

				if (relocate_two_forward::get_move({ nod_i, nod_j }, _sol, m_for, _best_mov))
				{
					_sol.evaluate_move(m_for);

					if (_sol.is_move_feasible(m_for))
					{
						/*try to store new feasible solution*/
						_sol.store_temp_solution(m_for);

						non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

						bool within_bound = non.is_within_bound(_sol.cur_objective(), _sol.cur_objective2(), _sol.OPTprim);
						if (within_bound)
						{
							if (m_for < _best_mov)
							{
								_best_mov = m_for;
							}

							if (first_improve)
							{
								if (is_aspiration(m_for) && (m_for.gen_objective < 0.0))
								{
									m = m_for;

									return;
								}
							}
						}
					}
				}

				j++;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	auto neighborhood::swap_node(bool first_improve, move<dynamic>& m) -> void
	{
		move<dynamic> _best_mov;

		/*avoid the same move done by relocate one*/
		exchange_one::move_type m_s;
		for (int i = _sol.inst()._num_node - 2; i > 0; i--)
		{
			auto nod_i = _sol.node_in_position(i);

			auto j = i + 1;

			while (j < _sol.inst()._num_node)
			{
				auto nod_j = _sol.node_in_position(j);

				/*if (problem_type != ProbType::robust && !_sol.OPTprim && (_sol.nominal_arrival_T(_sol.next(nod_j)) <= _sol.inst().earliest(_sol.next(nod_j))))
				{
					if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
					{
						break;
					}
				}*/

				if (exchange_one::get_move({ nod_i, nod_j }, _sol, m_s, _best_mov))
				{
					_sol.evaluate_move(m_s);

					if (_sol.is_move_feasible(m_s))
					{
						/*try to store new feasible solution*/
						_sol.store_temp_solution(m_s);

						non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

						bool within_bound = non.is_within_bound(_sol.cur_objective(), _sol.cur_objective2(), _sol.OPTprim);
						if (within_bound)
						{
							if (m_s < _best_mov)
							{
								_best_mov = m_s;
							}

							if (first_improve)
							{
								if (is_aspiration(m_s) && (m_s.gen_objective < 0.0))
								{
									m = m_s;

									return;
								}
							}
						}
					}
				}

				j++;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	auto neighborhood::one_opt_move(bool first_improve, move<dynamic>& m) -> void
	{
		move<dynamic> _best_mov;

		/*avoid the same move done by relocate one*/
		one_opt::move_type m_one;
		for (int i = _sol.inst()._num_node - 2; i > 0; i--)
		{
			auto nod_i = _sol.node_in_position(i);

			auto j = i + 1;
			auto nod_j = _sol.node_in_position(j);

			/*if (problem_type != ProbType::robust && !_sol.OPTprim && (_sol.nominal_arrival_T(_sol.next(nod_j)) <= _sol.inst().earliest(_sol.next(nod_j))))
			{
				if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
				{
					break;
				}
			}*/

			if (one_opt::get_move({ nod_i, nod_j }, _sol, m_one, _best_mov))
			{
				_sol.evaluate_move(m_one);

				if (_sol.is_move_feasible(m_one))
				{
					/*try to store new feasible solution*/
					_sol.store_temp_solution(m_one);

					non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

					bool within_bound = non.is_within_bound(_sol.cur_objective(), _sol.cur_objective2(), _sol.OPTprim);
					if (within_bound)
					{
						if (m_one < _best_mov)
						{
							_best_mov = m_one;
						}

						if (first_improve)
						{
							if (is_aspiration(m_one) && (m_one.gen_objective < 0.0))
							{
								m = m_one;

								return;
							}
						}
					}
				}
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}

	auto neighborhood::two_opt_move(bool first_improve, move<dynamic>& m) -> void
	{
		move<dynamic> _best_mov;

		/*2-opt(forward)*/
		/*also backward manner?*/
		two_opt::move_type m_two;
		for (int i = _sol.inst()._num_node - 2; i > 0; i--)
		{
			auto nod_i = _sol.node_in_position(i);

			/*if (problem_type != ProbType::robust && !_sol.OPTprim && (_sol.nominal_arrival_T(nod_i) <= _sol.inst().earliest(nod_i)))
			{
				if ((_sol.inst()._primobj_type == PrimObjType::duration) || (_sol.inst()._primobj_type == PrimObjType::completion))
				{
					break;
				}
			}*/

			auto j = i - 2;

			while (j > 0)
			{
				auto nod_j = _sol.node_in_position(j);

				if (two_opt::get_move({ nod_j, nod_i }, _sol, m_two, _best_mov))
				{
					_sol.evaluate_move(m_two);

					if (_sol.is_move_feasible(m_two))
					{
						/*try to store new feasible solution*/
						_sol.store_temp_solution(m_two);

						non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

						bool within_bound = non.is_within_bound(_sol.cur_objective(), _sol.cur_objective2(), _sol.OPTprim); //obj2 is real value not minus
						if (within_bound)
						{
							if (m_two < _best_mov)
							{
								_best_mov = m_two;
							}

							if (first_improve)
							{
								if (is_aspiration(m_two) && (m_two.gen_objective < 0.0))
								{
									m = m_two;

									return;
								}
							}
						}
					}
				}
				j--;
			}
		}

		if (_best_mov < m)
		{
			m = _best_mov;
		}
	}


	/*k-level shake phase*/
	/*randomly remove k*5 nodes*/
	/*reinsert removed nodes into best position regarding cost*/
	auto neighborhood::shake_phase(int level, int seed, double time_limit) -> bool
	{
		//avoid the case to delete all the nodes
		if (level >= _sol.inst()._num_node - 1)
		{
			level = _sol.inst()._num_node - 2;
		}

		/*remove "level" nodes randomly*/
		mt19937 randomgenerator(seed);
		remove_one::move_type m_r;

		vector<int> removed_nodes;

		int count = 0;
		while (count < level)
		{
			uniform_int_distribution<int> index(1, (int)(_sol.tour().size() - 2));

			int pick_index = index(randomgenerator);
			auto pick_nod = _sol.node_in_position(pick_index);

			if (remove_one::get_move(pick_nod, _sol, m_r))
			{
				removed_nodes.push_back(pick_nod);

				_sol.apply_move(m_r);

				count++;
			}
		}

		time_t start, end;
		time(&start);
		double elasped_time = 0.0;

		/*reinsert the removed nodes*/
		count = 0;
		while (_sol.unrouted_nodes().size() > 0 && (elasped_time <= time_limit))
		{
			move<dynamic> m_best;
			insert_one::move_type m_i;

			auto i = removed_nodes[count];

			for (const auto j : _sol.tour())
			{
				if (j < _sol.inst()._num_node)
				{
					if (insert_one::get_move({ j, i }, _sol, m_i))
					{
						_sol.evaluate_move(m_i);

						if (m_i < m_best)
						{
							m_best = m_i;
						}
					}
				}
			}

			if (_sol.is_move_valid(m_best))
			{
				_sol.apply_move(m_best);
			}
			else
			{
				return false;
			}

			time(&end);
			elasped_time = difftime(end, start);

			count++;
		}

		return true;
	}

	/*k-level shake phase*/
	/*perform k-level 1 shift move*/
	auto neighborhood::level_shake_phase(int level, int seed, double time_limit) -> bool
	{
		time_t start, end;
		time(&start);
		double elasped_time = 0.0;

		mt19937 randomgenerator(seed);
		uniform_int_distribution<int> nod(1, (int)(_sol.tour().size() - 2));

		int k = 0;
		while (k < level && elasped_time <= time_limit)
		{
			move<dynamic> m;

			int nod_i = nod(randomgenerator);
			auto pos_i = _sol.position(nod_i);
			int pos_j_for = pos_i + 1;
			int pos_j_back = pos_i - 1;

			while (pos_j_back > 0)
			{
				auto nod_j = _sol.node_in_position(pos_j_back);

				relocate_one_backward::move_type m_back;

				if (relocate_one_backward::get_move({ nod_i, nod_j }, _sol, m_back, m))
				{
					_sol.evaluate_move(m_back);

					if (!_sol.phase_feasi)
					{
						if (_sol.is_move_valid(m_back))
						{
							_sol.apply_move(m_back);

							if (_sol.feasible())
							{
								return true;
							}

							k++;
							break;
						}
					}
					else
					{
						if (_sol.is_move_feasible(m_back))
						{
							/*try to store new feasible solution*/
							_sol.store_temp_solution(m_back);

							//double slack = 0.0, budget = 0.0; //always cost, budget, slack
							//if (_sol.inst()._secobj_type == SecObjType::slack)
							//{
							//	slack = _sol.cur_objective2();
							//	budget = _sol.cur_check_objective();
							//}
							//else
							//{
							//	budget = _sol.cur_objective2();
							//	slack = _sol.cur_check_objective();
							//}

							non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

							_sol.apply_move(m_back);

							k++;
							break;
						}
					}
				}
				else
				{
					pos_j_back--;
				}
			}

			while (pos_j_for < _sol.inst()._num_node)
			{
				auto nod_j = _sol.node_in_position(pos_j_for);

				relocate_one_forward::move_type m_for;

				if (relocate_one_forward::get_move({ nod_i, nod_j }, _sol, m_for, m))
				{
					_sol.evaluate_move(m_for);

					if (!_sol.phase_feasi)
					{
						if (_sol.is_move_valid(m_for))
						{
							_sol.apply_move(m_for);

							if (_sol.feasible())
							{
								return true;
							}

							k++;
							break;
						}
					}
					else
					{
						if (_sol.is_move_feasible(m_for))
						{
							/*try to store new feasible solution*/
							_sol.store_temp_solution(m_for);

							//double slack = 0.0, budget = 0.0; //always cost, budget, slack
							//if (_sol.inst()._secobj_type == SecObjType::slack)
							//{
							//	slack = _sol.cur_objective2();
							//	budget = _sol.cur_check_objective();
							//}
							//else
							//{
							//	budget = _sol.cur_objective2();
							//	slack = _sol.cur_check_objective();
							//}

							non.add_new_solution(_sol.cur_objective(), _sol.cur_objective2(), _sol.cur_check_objective(), _sol.cur_tour());

							_sol.apply_move(m_for);

							k++;

							break;
						}
					}
				}
				else
				{
					pos_j_for++;
				}
			}

			time(&end);
			elasped_time = difftime(end, start);
		}

		return true;
	}

	/*====================================================================================*/
	/* check whether the current solution is improving
	* feasible solution is always better than infeasible ones
	*/
	template <typename Move>
	auto neighborhood::is_aspiration(Move& m) const -> bool
	{
		const bool best_feas = _best.feasible(), feas = best_feas == _sol.feasible();

		if (feas)/*feasible status the same*/
		{
			return _sol.objective() + m.gen_objective < _best.objective();
		}

		return _sol.is_move_feasible(m) > best_feas;
	}
}
