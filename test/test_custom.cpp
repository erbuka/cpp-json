#include <json/json.hpp>

#include <iostream>

#include "macros.hpp"

struct test_str {
  float x{};
  float y{};
  std::string name{};
};

template<>
class json::serializer<test_str> {
public:
  json::value to_json(const test_str& v) {
    return json::object{
      {"x", v.x},
      {"y", v.y},
      {"name", v.name},
    };
  }

  test_str from_json(const json::value& v) {
    test_str result;
    result.x = v["x"].get<json::number>().value();
    result.y = v["y"].get<json::number>().value();
    result.name = v["name"].get<json::string>().value();
    return result;
  }
};

int main() {

  float x = 1.0f;

  test_str s0{1.0f, 2.0f, "Andrea"};

  // test to_json
  json::value val0(s0);
  ASSERT_TRUE(val0.is<json::object>());
  ASSERT_TRUE(val0["x"].get<json::number>().value() == 1.0f);
  ASSERT_TRUE(val0["y"].get<json::number>().value() == 2.0f);
  ASSERT_TRUE(val0["name"].get<json::string>().value() == "Andrea");

  // test from_json
  test_str s1 = val0.get<test_str>();
  ASSERT_TRUE(s1.x == 1.0f);
  ASSERT_TRUE(s1.y == 2.0f);
  ASSERT_TRUE(s1.name == "Andrea");

}