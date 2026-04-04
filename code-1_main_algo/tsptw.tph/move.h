#pragma once

#include"../instance.h"

namespace tsptw_tph
{
	constexpr int dynamic = 0;

	template <typename T1, typename T2>
	class move_base
	{
	public:
		move_base()
			: gen_objective{ std::numeric_limits<double>::infinity() }
			, obj1{ 0.0 }
			, obj2{ 0.0 }
			, check_obj{0.0}
			, costs{ 0.0 }
			, CT{ 0.0 }
			, TD{ 0.0 }
			, min_slack{ 0.0 }
			, min_budget{ BigM }
			, tw_violation{ 0.0 }
			, penalty{ 0.0 }
			, inverted_seqs{ }
		{
			//empty
		}


		using inserted_type = T1;
		using removed_type = T2;

		template <typename C1, typename C2>
		auto operator <(const move_base<C1, C2>& m) const -> bool
		{
			if (gen_objective == m.gen_objective)
			{
				return costs < m.costs;
			}
			else
			{
				return gen_objective < m.gen_objective;
			}
		}

		/**
		* Checks whether this moves objective is negative.
		*/
		auto is_improving() const -> bool
		{
			return gen_objective + 0.0001 < 0.0;
		}

		auto decrease_obj1() const->bool
		{
			return obj1 < 0.0;
		}

		/**
		* Resets the values of this move.
		*/
		auto reset() -> void
		{
			inserted.clear();
			removed.clear();

			gen_objective = std::numeric_limits<double>::infinity();
			obj1 = obj2 = check_obj = costs = CT = TD = min_slack = tw_violation = penalty = 0.0;
			min_budget = BigM;
			inverted_seqs.clear();
		}

		inserted_type inserted;
		removed_type removed;

		double gen_objective;
		double obj1;
		double obj2;
		double check_obj;
		double costs;
		double CT;
		double TD;
		double min_slack;
		double min_budget;
		double tw_violation;
		double penalty;
		std::vector<instance::tour::subtour> inverted_seqs;
	};

	/**
	 * This class represents a single move. An instance of the solution class
	 * should only be modified by passing an instance of this class to solution::apply().
	 */
	template <std::size_t M, std::size_t N = M, std::string(*F)() = nullptr>
	class move : public move_base<std::array<instance::arc, M>, std::array<instance::arc, N>>
	{
	public:
		static auto name() -> std::string
		{
			if constexpr (F == nullptr)
			{
				return "Unnamed";
			}
			else return F();
		}
	};

	template <>
	class move<dynamic, dynamic, nullptr> : public move_base<std::vector<instance::arc>, std::vector<instance::arc>>
	{
	public:
		move()
			: move_base()
			, _name{ "Dynamic" }
		{
			// empty
		}

		template <std::size_t M2, std::size_t N2, std::string(*F2)()>
		auto operator =(const move<M2, N2, F2>& m) -> move &
		{
			gen_objective = m.gen_objective;
			obj1 = m.obj1;
			obj2 = m.obj2;
			check_obj = m.check_obj;
			costs = m.costs;
			CT = m.CT;
			TD = m.TD;
			min_slack = m.min_slack;
			min_budget = m.min_budget;
			tw_violation = m.tw_violation;
			penalty = m.penalty;

			inverted_seqs.clear();
			inverted_seqs.resize(m.inverted_seqs.size());
			std::copy(std::begin(m.inverted_seqs), std::end(m.inverted_seqs), std::begin(inverted_seqs));

			_name = m.name();

			inserted.resize(m.inserted.size());
			removed.resize(m.removed.size());

			std::copy(std::begin(m.inserted), std::end(m.inserted), std::begin(inserted));
			std::copy(std::begin(m.removed), std::end(m.removed), std::begin(removed));

			return *this;
		}

		auto name(const std::string& str) -> void
		{
			_name = str;
		}

		auto name() const -> std::string
		{
			return _name;
		}

	protected:
		std::string _name;
	};

	template <std::size_t M, std::size_t N, std::string(*F)()>
	auto operator <<(std::ostream& os, const move<M, N, F>& m) -> std::ostream &
	{
		os << m.name() << " Objective: " << m.objective;

		os << " Inserted: ";

		for (const auto& a : m.inserted)
		{
			os << a << " ";
		}

		os << " Removed ";

		for (const auto& a : m.removed)
		{
			os << a << " ";
		}

		return os;
	}
}
