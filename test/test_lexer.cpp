#include <vector>
#include <sstream>
#include <iostream>
#include "json/json.hpp"

#include <lexer.hpp>

#include "macros.hpp"


void test_lexer(const std::string_view input, const std::vector<json::internal::token> expected) {
  std::istringstream ss(std::string{ input });
  json::internal::lexer lex(ss);

  for (const auto& exp : expected) {
    const auto tok = lex.next_token();
    if (tok != exp) {
      throw std::runtime_error("Unexpected token");
    }
  }
}

int main() {

  try {
    test_lexer(",:{}[]()12312\"Hello, World!\".123 true false", { 
      json::internal::token_types::comma{}, 
      json::internal::token_types::colon{}, 
      json::internal::token_types::open_curly{}, 
      json::internal::token_types::close_curly{},
      json::internal::token_types::open_square{},
      json::internal::token_types::close_square{},
      json::internal::token_types::open_par{},
      json::internal::token_types::close_par{},
      json::internal::token_types::number_literal{ 12312 },
      json::internal::token_types::string_literal{ "Hello, World!" },
      json::internal::token_types::number_literal{ 0.123 },
      json::internal::token_types::boolean_literal{ true },
      json::internal::token_types::boolean_literal{ false },
      json::internal::token_types::eof{},
    });
    
    {
      std::stringstream ss("1{[]");
      json::internal::lexer lex(ss);

      const auto pos = lex.mark();
      ASSERT_TRUE(lex.next_token() == json::internal::token_types::number_literal{1});
      ASSERT_TRUE(lex.next_token() == json::internal::token_types::open_curly{});
      ASSERT_TRUE(lex.next_token() == json::internal::token_types::open_square{});
      lex.reset(pos);
      ASSERT_TRUE(lex.next_token() == json::internal::token_types::number_literal{1});
      ASSERT_TRUE(lex.peek_token() == json::internal::token_types::open_curly{});
      ASSERT_TRUE(lex.peek_token() == json::internal::token_types::open_curly{});
      return 0;

    }

  }
  catch(std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    return 1;
  }


}