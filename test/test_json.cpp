#include <json/json.hpp>

#include <iostream>

#include "macros.hpp"



int main() {

  json::value val = json::object{
    {"key1", 1},
    {"key2", "value2"},
    {"key3", true},
    {"key4", nullptr},
    {"key5", json::array{1, 2, 3}},
    {"key6", json::object{{"key1", 1}, {"key2", 2}}},
  };

  // Object setters
  json::object obj;
  obj["key1"] = 1;
  obj["key2"] = "value2";
  obj["key3"] = true;
  obj["key4"] = nullptr;
  obj["key5"] = json::array{1, 2, 3};
  obj["key6"] = json::object{{"key1", 1}, {"key2", 2}};

  // Array setters
  json::array arr;
  arr.push_back(1);
  arr.push_back("value2");
  arr.push_back(true);
  arr.push_back(nullptr);
  arr.push_back(json::array{1, 2, 3});
  arr.push_back(json::object{{"key1", 1}, {"key2", 2}});

  // Getters
  ASSERT_TRUE(val.size() == 6);
  ASSERT_TRUE(val["key1"].get<json::number>().value() == 1.0);
  ASSERT_TRUE(val["key2"].get<json::string>().value() == "value2");
  ASSERT_TRUE(val["key3"].get<json::boolean>().value() == true);
  ASSERT_TRUE(val["key4"].is<json::null>());
  ASSERT_TRUE(val["key5"].get<json::array>().size() == 3.0);
  ASSERT_TRUE(val["key6"].get<json::object>().size() == 2.0);
  ASSERT_TRUE(val["key6"]["key1"].get<json::number>().value() == 1.0);
  ASSERT_TRUE(val["key6"]["key2"].get<json::number>().value() == 2.0);
  
  // Loops
  for (auto& [key, v] : val.get<json::object>()) {
    std::cout << key << '\n';
  }
  for (const auto& [key, v] : val.get<json::object>()) {
    std::cout << key << '\n';
  }

  for (auto& v : val["key5"].get<json::array>()) {
    std::cout << v.get<json::number>().value() << '\n';
  }
  for (const auto& v : val["key5"].get<json::array>()) {
    std::cout << v.get<json::number>().value() << '\n';
  }



  return 0;
}
