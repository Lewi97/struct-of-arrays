// UnitTests.cpp : Source file for your target.
//

#include "UnitTests.h"

#include <gtest/gtest.h>

#include "StructofArrays/StructofArrays.h"
#include <string>
#include <ranges>
#include <algorithm>


using namespace std::string_literals;
using namespace le;

TEST(array_of_structs, emplacing)
{
	auto aos = StructOfArrays<int, float, std::string>();
	
	aos.emplace_back(5, 10.f, "Hello, world"s);
	aos.emplace_back(5, 10.f, "Hello, world"s);
	aos.emplace_back(5, 10.f, "Hello, world"s);

	ASSERT_TRUE(std::equal(aos.begin<int>(), aos.end<int>(), aos.rbegin<int>()));
	ASSERT_TRUE(std::equal(aos.begin<float>(), aos.end<float>(), aos.rbegin<float>()));
	ASSERT_TRUE(std::equal(aos.begin<std::string>(), aos.end<std::string>(), aos.rbegin<std::string>()));

	ASSERT_EQ(aos.size(), 3);
	ASSERT_TRUE(aos.at<int>(0) == 5);
	ASSERT_TRUE(aos.at<float>(1) == 10.f);
	ASSERT_TRUE(aos.at<std::string>(2) == "Hello, world");

	aos.at<int>(1) = 20;

	ASSERT_EQ(aos.at<int>(1), 20);

	aos.emplace_back<std::string>("Hello");

	ASSERT_EQ(aos.size(), 4);
	ASSERT_TRUE(aos.at<std::string>(3) == "Hello");
	ASSERT_TRUE(aos.at<float>(3) == float{});
	ASSERT_TRUE(aos.at<int>(3) == int{});

	aos.emplace_back<0>(15);
	ASSERT_EQ(aos.size(), 5);
	ASSERT_EQ(aos.at<int>(4), 15);
}

TEST(array_of_structs, iteration)
{
	auto aos = StructOfArrays<int, float, std::string>();

	aos.resize(10);

	/* Use single type each in range functions */
	std::ranges::generate(aos.each<int>(), [i = 0]() mutable { return i++; });

	/* Iterate all */
	for (auto test{ 0 }; auto [i, f, s] : aos.each())
	{
		ASSERT_EQ(i, test);
		s = std::to_string(test);
		test++;
	}

	/* Iterate by type index */
	for (auto [i, s] : aos.each<0, 2>())
	{
		ASSERT_EQ(std::to_string(i), s);
	}

	/* Iterate over single type */
	for (auto i{ 0.f }; auto& f : aos.each<float>())
	{
		f = i++;
	}

	/* Iterate over single type by index */
	for (auto i{ 0.f }; auto& f : aos.each<1>())
	{
		ASSERT_EQ(f, i++);
	}

	/* Iterate over multiple types through their typenames */
	for (auto [f, string] : aos.each<float, std::string>())
	{
		ASSERT_EQ(std::to_string(static_cast<int>(f)), string);
	}
}

template<typename T>
concept const_ref = std::is_const_v<std::remove_reference_t<T>> and std::is_reference_v<T>;

TEST(array_of_structs, at_correctness)
{
	auto aos = StructOfArrays<int, int, float, double>();

	aos.emplace_back(1, 2, 3.f, 4.0);

	{
		/* Mix and match indices */
		auto [i, f, i2] = aos.at<0, 2, 1>(0);
		ASSERT_EQ(i, 1);
		ASSERT_EQ(f, 3.f);
		ASSERT_EQ(i2, 2);

		static_assert(std::is_reference_v<decltype(i)>);
		static_assert(std::is_reference_v<decltype(f)>);
		static_assert(std::is_reference_v<decltype(i2)>);
	}
	{
		/* Mix and match typenames of isolated types */
		auto [d, f] = aos.at<double, float>(0);
		ASSERT_EQ(d, 4.0);
		ASSERT_EQ(f, 3.f);

		static_assert(std::is_reference_v<decltype(f)>);
		static_assert(std::is_reference_v<decltype(d)>);
	}
	{
		/* Get isolated types by typename */
		decltype(auto) f = aos.at<float>(0);
		ASSERT_EQ(f, 3.f);

		static_assert(std::is_reference_v<decltype(f)>);
	}
	/* Constness */
	{
		decltype(auto) i2 = std::as_const(aos).at<1>(0);
		
		ASSERT_EQ(i2, 2);
		
		static_assert(const_ref<decltype(i2)>);
	}
	{
		decltype(auto) f = std::as_const(aos).at<float>(0);

		ASSERT_EQ(f, 3.f);

		static_assert(const_ref<decltype(f)>);
	}
	{
		auto [i, f, i2] = std::as_const(aos).at<0, 2, 1>(0);
		ASSERT_EQ(i, 1);
		ASSERT_EQ(f, 3.f);
		ASSERT_EQ(i2, 2);

		static_assert(const_ref<decltype(i)>);
		static_assert(const_ref<decltype(f)>);
		static_assert(const_ref<decltype(i2)>);
	}
	{
		auto [d, f] = std::as_const(aos).at<double, float>(0);
		ASSERT_EQ(d, 4.0);
		ASSERT_EQ(f, 3.f);

		static_assert(const_ref<decltype(f)>);
		static_assert(const_ref<decltype(d)>);
	}
}