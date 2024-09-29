// StructofArrays.cpp : Defines the entry point for the application.
//

#include "StructofArrays.h"

#include <string>
#include <assert.h>

using namespace le;

using namespace std::string_literals;

int main()
{
	auto types = StructOfArrays<int, int, float, std::string>();

	{
		/* Emplacing objects is simple */
		auto [i, i2, f, str] = types.emplace_back(0, 1, 2.f, "Hello"s);
		
		/* Returned are references to the emplaced objects */
		i2 = 5;

		/* Get duplicate types by index */
		assert(types.at<1>(0) == 5);

		/* Non duplicates can be gotten by typename */
		assert(types.at<std::string>(0) == "Hello");
	}

	{
		/* Can also emplace a single type if the other types are default constructible */
		types.emplace_back<std::string>("world");

		assert(types.at<std::string>(1) == "world");

		/* The other types are default constructed */
		assert(types.at<float>(1) == float{});

		/* Others can be emplaced at the given index */
		types.emplace<float>(1, 5.f);
		assert(types.at<float>(1) == 5.f);
	}

	{
		/* The array is iterable in a normal for loop */
		for (auto [x, y] : types.each<0, 1>())
		{
			/* Busy work */
		}

		for (auto [f, str] : types.each<float, std::string>())
		{
			/* Busy work */
		}

		types.for_each([](int, int, float, std::string)
			{
				/* More busy work */
			});
	}

	return 0;
}
