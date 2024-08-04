#include <json/json.hpp>  

#include "macros.hpp"

int main() {
  using namespace json;

  string str{ "Hello, World!" };
  number num{ 123.123 };

  ASSERT_TRUE(str.value() == "Hello, World!");
  ASSERT_TRUE(num.value() == 123.123);
  ASSERT_TRUE(str == str);
  ASSERT_TRUE(num == num);
  ASSERT_TRUE(value{ str } != value{ num });

  object obj = {
    { "key1", str },
    { "key2", num },
    { "keyArr", array{1,2,3} },
    { "keyObj", object{{"key1", str}, {"key2", num}} }
  };

  obj["key1"] = str;
  obj["key2"] = num;
  obj["key3"] = "Test";
  obj["key3"] = 4.0f;
  obj["key3"] = nullptr;

  value val0{ 1.0f };
  value val1{ "Hello, World!" };
  value val2{ nullptr };

  ASSERT_TRUE(val0 == 1.0f);
  ASSERT_TRUE(val1 == "Hello, World!");
  ASSERT_TRUE(val2 == nullptr);

  array arr = { 1.0f, "Hello, World!", nullptr };

  arr.push_back(str);
  arr.push_back(num);

  return 0;
}