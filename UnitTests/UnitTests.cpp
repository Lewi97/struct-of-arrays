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
