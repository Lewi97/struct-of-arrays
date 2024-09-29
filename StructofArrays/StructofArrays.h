// StructofArrays.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <tuple>
#include <vector>

#include "Iterable.h"
#include "TupleTools.h"

namespace le
{
	template<typename... Types>
	class StructOfArrays
	{
	public:
		constexpr StructOfArrays() = default;

		using Data = std::tuple<Types...>;
		template<typename... Ts>
		using AsRefTuple = std::tuple<Ts&...>;
		using RefData = AsRefTuple<Types...>;

		using ConstRefData = std::tuple<const Types&...>;

		template<typename T>
		using Container = std::vector<T>;

		static constexpr auto elements = sizeof...(Types);
		template <std::size_t N>
		using Nth = std::tuple_element_t<N, Data>;
		using First = Nth<0>;
		using Last = Nth<elements - 1>;

		template<size_t index>
		using IteratorFor = typename Container<Nth<index>>::iterator;

		template<typename T>
		static constexpr auto index_of = tuple_element_index_v<T, Data>;

		template<typename Derived, typename Value, typename Reference>
		class BaseAoSIterator
		{
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = Value;
			using difference_type = size_t;
			using pointer = Value*;
			using reference = Reference;

			explicit BaseAoSIterator(StructOfArrays& owner, size_t start = 0)
				: _owner(&owner)
				, _index(start)
			{}

			auto operator++() -> Derived&
			{
				_index++;
				return me();
			}

			auto operator++(int) -> Derived
			{
				Derived self = me();
				++me();
				return self;
			}

			auto operator+(difference_type delta) -> Derived
			{
				return Derived(*_owner, _index + delta);
			}

			auto operator==(const Derived& other) const -> bool
			{
				return other._index == _index and other._owner == _owner;
			}

			auto operator!=(const Derived& other) const -> bool
			{
				return !(me() == other);
			}

			auto index() const { return _index; }
		protected:
			template<typename Self>
			auto me(this Self&& self) -> decltype(auto)
			{ 
				if constexpr (std::is_const_v<std::remove_reference_t<Self>>)
				{
					return *static_cast<const Derived*>(&self);
				}
				else
				{
					return *static_cast<Derived*>(&self);
				}
			}
		protected:
			StructOfArrays* _owner{ nullptr };
			size_t _index{};
		};

		template<size_t... indices>
		class iterator final
			: public BaseAoSIterator<iterator<indices...>, std::tuple<Nth<indices>...>, std::tuple<Nth<indices>&...>>
		{
		public:
			using Base = BaseAoSIterator<iterator<indices...>, std::tuple<Nth<indices>...>, std::tuple<Nth<indices>&...>>;

			using Base::BaseAoSIterator;

			auto operator*() const -> typename Base::reference
			{
				return Base::_owner->at<indices...>(Base::_index);
			}
		};

		template<typename T, typename... Ts>
		class type_iterator final
			: public BaseAoSIterator<type_iterator<T, Ts...>, std::tuple<T, Ts...>, std::tuple<T&, Ts&...>>
		{
		public:
			using Base = BaseAoSIterator<type_iterator<T, Ts...>, std::tuple<T, Ts...>, std::tuple<T&, Ts&...>>;

			using Base::BaseAoSIterator;

			auto operator*() const -> typename Base::reference
			{
				return Base::_owner->template at<T, Ts...>(Base::_index);
			}
		};

		template<size_t N, typename T, typename... Args>
		constexpr auto emplace(size_t at, Args&&... args) -> T&
		{
			return std::get<N>(_components).at(at) = T(std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		constexpr auto emplace(size_t at, Args&&... args) -> T&
		{
			return std::get<T>(_components).at(at) = T(std::forward<Args>(args)...);
		}

		constexpr auto emplace_back(Types&&... types) -> auto
		{
			return RefData(emplace_t_back<Types>(std::forward<decltype(types)>(types))...);
		}

		template<typename T, typename... Args>
		constexpr auto emplace_back(Args&&... args) -> auto&
			requires ((std::same_as<T, Types> or std::is_default_constructible_v<Types>) and ...)
		{
			return emplace_back<tuple_element_index_v<T, Data>, Args...>(std::forward<Args>(args)...);
		}

		template<size_t index, typename... Args>
		constexpr auto emplace_back(Args&&... args) -> auto&
		{
			auto& r = emplace_t_back<index>(std::forward<Args>(args)...);
			
			auto emplace_empty_if_not_t = [&]<size_t current_index>()
			{
				if constexpr (index != current_index)
				{
					emplace_t_back<current_index>();
				}
			};

			[&]<size_t... indices>(std::index_sequence<indices...>)
			{
				(emplace_empty_if_not_t.template operator()<indices>(), ...);
			}(std::index_sequence_for<Types...>{});

			return r;
		}

		constexpr auto reserve(size_t e) -> void
		{
			for_each_container([&](auto& container)
				{
					container.reserve(e);
				});
		}

		constexpr auto resize(size_t e) -> void
		{
			for_each_container([&](auto& container)
				{
					container.resize(e);
				});
		}

		template<typename T, typename... Ts>
		constexpr auto at(size_t idx) -> decltype(auto)
		{
			if constexpr (sizeof...(Ts) == 0)
			{
				return std::get<Container<T>>(_components).at(idx);
			}
			else
			{
				return AsRefTuple<T, Ts...>(at<T>(idx), at<Ts>(idx)...);
			}
		}

		template<size_t index, size_t... indices>
		constexpr auto at(size_t idx) -> decltype(auto)
		{
			if constexpr (sizeof...(indices) == 0)
			{
				return std::get<index>(_components).at(idx);
			}
			else
			{
				return AsRefTuple<Nth<index>, Nth<indices>...>(at<index>(idx), at<indices>(idx)...);
			}
		}

		template<typename Self>
		constexpr auto at(this Self&& self, size_t idx) -> decltype(auto)
		{
			using DecayedSelf = std::remove_reference_t<Self>;
			using RetType = std::conditional_t<std::is_const_v<DecayedSelf>, ConstRefData, RefData>;
			return std::apply([&](auto&... containers) -> RetType
				{
					return RetType(containers.at(idx)...);
				}, std::forward<Self>(self)._components);
		}

		template<size_t... indices>
		constexpr auto for_each(auto invocable)
			//requires (std::same_as<Nth<indices>, std::remove_cvref_t<Args>> and ...)
		{
			for (auto i{ 0 }; i < size(); i++)
			{
				if constexpr (sizeof...(indices) > 0)
				{
					invocable(at<indices>(i)...);
				}
				else
				{
					std::apply([&](auto&... containers)
						{
							invocable(containers.at(i)...);
						}, _components);
				}
			}
		}

		constexpr auto swap(size_t t1, size_t t2) -> void
		{
			for_each_container([&](auto& container)
				{
					using std::swap;
					swap(container.at(t1), container.at(t2));
				});
		}

		constexpr auto pop_back() -> void
		{
			for_each_container([](auto& container)
				{
					container.pop_back();
				});
		}

		constexpr auto size() const -> size_t { return std::get<0>(_components).size(); }
		constexpr auto empty() const -> bool { return std::get<0>(_components).empty(); }

		template<size_t... indices>
		auto erase(iterator<indices...> iter) -> iterator<indices...>
		{
			std::apply([&](auto&... containers)
				{
					(containers.erase(std::next(containers.begin(), iter.index())), ...);
				}, _components);

			return iter;
		}

		template<size_t... indices>
		auto begin() 
		{ 
			if constexpr (sizeof...(indices) == 0)
			{
				return[this]<size_t... indices>(std::index_sequence<indices...>)
				{
					return begin<indices...>();
				}(std::make_index_sequence<elements>{});
			}
			else if constexpr (sizeof...(indices) == 1)
			{
				return std::get<indices...>(_components).begin();
			}
			else
			{
				return iterator<indices...>(*this, 0); 
			}
		}
		template<size_t... indices>
		auto end() 
		{ 
			if constexpr (sizeof...(indices) == 0)
			{
				return[this]<size_t... indices>(std::index_sequence<indices...>)
				{
					return end<indices...>();
				}(std::make_index_sequence<elements>{});
			}
			else if constexpr (sizeof...(indices) == 1)
			{
				return std::get<indices...>(_components).end();
			}
			else
			{
				return iterator<indices...>(*this, size()); 
			}
		}

		template<typename T, typename... Ts>
		auto begin() 
		{ 
			return begin<index_of<T>, index_of<Ts>...>();
		}

		template<typename T, typename... Ts>
		auto end() 
		{ 
			return end<index_of<T>, index_of<Ts>...>();
		}

		template<typename T>
		auto rbegin()
		{
			return std::get<Container<T>>(_components).rbegin();
		}

		template<typename T>
		auto rend()
		{
			return std::get<Container<T>>(_components).rend();
		}

		template<size_t... indices>
		auto each() 
		{
			if constexpr (sizeof...(indices) == 0)
			{
				return[this]<size_t... lambda_indices>(std::index_sequence<lambda_indices...>)
				{
					return each<lambda_indices...>();
				}(std::make_index_sequence<elements>{});
			}
			else if constexpr (sizeof...(indices) == 1)
			{
				return Iterable<IteratorFor<indices...>>(begin<indices...>(), end<indices...>());
			}
			else
			{
				return Iterable<iterator<indices...>>(begin<indices...>(), end<indices...>()); 
			}
		}

		template<typename T, typename... Ts>
		auto each() 
		{
			return each<index_of<T>, index_of<Ts>...>();
		}
	private:
		constexpr auto for_each_container(auto invocable) -> void
		{
			std::apply([&](Container<Types>&... containers)
				{
					(invocable(containers), ...);
				}, _components);
		}

		template<size_t N, typename... Args>
		constexpr auto emplace_t_back(Args&&... args) -> auto&
		{
			return(std::get<N>(_components).emplace_back(std::forward<Args>(args)...));
		}

		template<typename T, typename... Args>
		constexpr auto emplace_t_back(Args&&... args) -> auto&
		{
			return(std::get<Container<std::remove_cvref_t<T>>>(_components).emplace_back(std::forward<Args>(args)...));
		}
	private:
		std::tuple<Container<Types>...> _components{};
	};
}
