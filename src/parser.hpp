#pragma once 

#include <json/json.hpp>

#include "lexer.hpp"

namespace json::internal {

  class parser {
  public:
    parser(lexer& lex) : _lexer(lex) {}

    value parse();

  private:
    lexer& _lexer;
    void throw_error(const std::string_view msg);
  };

}