# StructofArrays
A dependency-less struct of arrays type.

Example usage:
```cpp
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
}```

The unit test files show more usage.
