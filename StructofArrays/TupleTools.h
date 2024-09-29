#pragma once

#include <tuple>

namespace le
{
    namespace detail
    {
        template<typename T, typename Tuple>
        struct tuple_element_index_helper;

        template<typename T>
        struct tuple_element_index_helper<T, std::tuple<>>
        {
            static constexpr std::size_t value = 0;
        };

        template<typename T, typename... Rest>
        struct tuple_element_index_helper<T, std::tuple<T, Rest...>>
        {
            static constexpr std::size_t value = 0;
            using RestTuple = std::tuple<Rest...>;
            static_assert(
                tuple_element_index_helper<T, RestTuple>::value ==
                std::tuple_size_v<RestTuple>,
                "type appears more than once in tuple");
        };

        template<typename T, typename First, typename... Rest>
        struct tuple_element_index_helper<T, std::tuple<First, Rest...>>
        {
            using RestTuple = std::tuple<Rest...>;
            static constexpr std::size_t value = 1 +
                tuple_element_index_helper<T, RestTuple>::value;
        };
    }

    /*
    * Derived from https://devblogs.microsoft.com/oldnewthing/20200629-00/?p=103910
    */
    template<typename T, typename Tuple>
    struct tuple_element_index
    {
        static constexpr std::size_t value =
            detail::tuple_element_index_helper<T, Tuple>::value;
        static_assert(value < std::tuple_size_v<Tuple>,
            "type does not appear in tuple");
    };

    template<typename T, typename Tuple>
    inline constexpr auto tuple_element_index_v = tuple_element_index<T, Tuple>::value;
}
