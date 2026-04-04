#pragma once

#include <random>
#include <time.h>

#include "exchange_one.hpp"
#include "relocate_one_backward.hpp"
#include "relocate_one_forward.hpp"
#include "relocate_two_backward.hpp"
#include "relocate_two_forward.hpp"
#include "two_opt.hpp"
#include "one_opt.hpp"
#include "create_one.hpp"
#include "insert_one.hpp"
#include "remove_one.hpp"

#include"../nondomi_list.h"

namespace tsptw_tph
{
	template <typename T>
	class neighborhood_operator_wrapper;

	/**
	 * The neighborhood operator class.
	 */
	class neighborhood_operator
	{
	public:

		template <typename T>
		using wrapper = neighborhood_operator_wrapper<T>;

		virtual auto get_move(const instance::arc& a, tsptw_solution& sol, move<dynamic>& m, move<dynamic>& best) const -> bool = 0;

		/**
		* Virtual destructor.
		*/
		virtual ~neighborhood_operator() = default;
	};

	template <typename T>
	class neighborhood_operator_wrapper : public neighborhood_operator, public T
	{
	public:
		using operator_type = T;
		using move_type = typename operator_type::move_type;

		using operator_type::get_move;

		/**
		* Creates a move from the given arc and writes it into the given move.
		* @return True if a move is available for the given trigger arc, false otherwise.
		*/
		auto get_move(const instance::arc& a, tsptw_solution& sol, move<dynamic>& m, move<dynamic>& best) const -> bool override
		{
			move_type temp;

			return T::get_move(a, sol, temp, best) ? m = temp, true : false;
		}
	};

	class neighborhood
	{
	public:
		explicit neighborhood(tsptw_solution& sol, tsptw_solution& best, nondominate_list& non);

		~neighborhood()
		{
			//empty
		}

		struct params
		{
			explicit params()
				: relocate_one_back(true)
				, relocate_one_for(true)
				, relocate_two_back(true)
				, relocate_two_for(true)
				, exchange_one(true)
				, two_opt(true)
				, one_opt(true)
			{
				//empty
			}

			bool relocate_one_back;
			bool relocate_one_for;
			bool relocate_two_back;
			bool relocate_two_for;
			bool exchange_one;
			bool two_opt;
			bool one_opt;
		};

		/*====================================================================================*/
		/*relocate-one move for vio and nonvio nodes*/
		auto one_shift_ls(bool first_improve, double time_limit) -> bool;

		auto back_relocate_vionodes(bool first_improve, move<dynamic>& m) -> void;

		auto for_relocatenonvionodes(bool first_improve, move<dynamic>& m) -> void;

		auto back_relocatenonvionodes(bool first_improve, move<dynamic>& m) -> void;

		auto for_relocate_vionodes(bool first_improve, move<dynamic>& m) -> void;

		/*====================================================================================*/
		auto vnd(bool first_improve, double time_limit) -> bool;

		auto back_relocate_one(bool first_improve, move<dynamic>& m) -> void;

		auto for_relocate_one(bool first_improve, move<dynamic>& m) -> void;

		auto back_relocate_two(bool first_improve, move<dynamic>& m) -> void;

		auto for_relocate_two(bool first_improve, move<dynamic>& m) -> void;

		auto swap_node(bool first_improve, move<dynamic>& m) -> void;

		auto two_opt_move(bool first_improve, move<dynamic>& m) -> void;

		auto one_opt_move(bool first_improve, move<dynamic>& m) -> void;

		/*k-level shake phase*/
		auto shake_phase(int level, int seed, double time_limit) -> bool;

		auto level_shake_phase(int level, int seed, double time_limit) -> bool;

		template <typename Move>
		auto is_aspiration(Move& m) const -> bool;

	private:

		tsptw_solution& _sol;

		tsptw_solution& _best;

		nondominate_list& non;

		move<dynamic> _best_mov;

		/*====================================================================================*/
		template <typename T>
		using wrapper = neighborhood_operator::wrapper<T>;

		wrapper<two_opt> _two_opt;
		wrapper<relocate_one_backward> _relocate_one_back;
		wrapper<relocate_one_forward> _relocate_one_for;
		wrapper<relocate_two_backward> _relocate_two_back;
		wrapper<relocate_two_forward> _relocate_two_for;
		wrapper<exchange_one> _exchange_one;
		wrapper<one_opt> _one_opt;

		std::vector<neighborhood_operator*> _operators;

		auto load_operators(const params& params) -> void
		{
			if (params.two_opt)
			{
				_operators.push_back(&_two_opt);
			}
			if (params.relocate_one_back)
			{
				_operators.push_back(&_relocate_one_back);
			}
			if (params.relocate_one_for)
			{
				_operators.push_back(&_relocate_one_for);
			}
			if (params.relocate_two_back)
			{
				_operators.push_back(&_relocate_two_back);
			}
			if (params.relocate_two_for)
			{
				_operators.push_back(&_relocate_two_for);
			}
			if (params.exchange_one)
			{
				_operators.push_back(&_exchange_one);
			}
			if (params.one_opt)
			{
				_operators.push_back(&_one_opt);
			}
		}
	};
}
