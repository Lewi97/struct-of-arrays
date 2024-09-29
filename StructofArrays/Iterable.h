#pragma once

#include <iterator>

namespace le
{
	template<typename T>
	struct Iterable final
	{
		using value_type = typename std::iterator_traits<T>::value_type;
		using iterator = T;

		constexpr Iterable(T first, T last)
			: _first{ std::move(first) }
			, _end{ std::move(last) }
		{}

		constexpr auto begin() const
		{
			return _first;
		}

		constexpr auto end() const
		{
			return _end;
		}

		constexpr auto cbegin() const
		{
			return begin();
		}

		constexpr auto cend() const
		{
			return end();
		}
	private:
		T _first;
		T _end;
	};
}
