#include <string_view>
#include <sstream>
#include <iostream>

#include "macros.hpp"

#include "parser.hpp"
#include "lexer.hpp"


bool parse_test(const std::string_view src, const json::value comp) {
  std::stringstream ss(std::string{ src });
  json::internal::lexer lex(ss);
  json::internal::parser par(lex);
  json::value parsed = par.parse();

  std::cout << parsed.dump() << std::endl;

  return parsed == comp;
}

int main() {

  json::value val("Hello, World!");

  try {
    ASSERT_TRUE(parse_test("1", json::number(1)));
    ASSERT_TRUE(parse_test("\"Hello, World!\"", json::string("Hello, World!")));
    ASSERT_TRUE(parse_test("null", json::null{}));
    ASSERT_FALSE(parse_test("1", json::string("1")));
    ASSERT_FALSE(parse_test("\"null\"", json::null{}));

    ASSERT_TRUE(parse_test("[]", json::array{}));
    ASSERT_TRUE(parse_test("[1, null, \"Hello\", false]", json::array{ 1, nullptr, "Hello", false }));
    ASSERT_FALSE(parse_test("[1, null, \"Hello\", true]", json::array{ "1", nullptr, "Hello", true }));

    ASSERT_TRUE(parse_test("{}", json::object()));
    ASSERT_TRUE(parse_test("{\"a\":1, \"b\" : false}", json::object({ {"a", 1}, { "b", false } })));
    ASSERT_TRUE(parse_test("{\"a\":1, \"b\":null}", json::object({ {"a", 1}, {"b", nullptr} })));
    ASSERT_TRUE(parse_test("[{\"a\":1, \"b\":null}]", json::array({ json::object({ {"a", 1}, {"b", nullptr} }) })));

    {
      ASSERT_TRUE(parse_test("[{}]", json::array({ json::object{} })));
      ASSERT_TRUE(parse_test("[[]]", json::array{ { json::array{} } }));
      ASSERT_TRUE(parse_test("[[1, 2, 3]]", json::array{ { json::array{1, 2, 3} } }));
    }
    return 0;

  }
  catch (std::runtime_error& err) {
    std::cout << err.what() << '\n';
    return 1;
  }
}