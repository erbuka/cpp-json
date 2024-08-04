#include "parser.hpp"

#include <format>
#include <stack>

namespace json::internal {

  enum class parser_state {
    none,
    object,
    array
  };

  void parser::throw_error(const std::string_view msg) {
    throw std::runtime_error(std::format("[Parse error at {}:{}]: {}",
      _lexer.get_current_line(),
      _lexer.get_current_character(),
      msg));
  }


  value parser::parse() {

    parser_state state{ parser_state::none };

    while (_lexer.peek_token() != token_types::eof{}) {

      if (state == parser_state::none) {

        const auto tok = _lexer.next_token();

        if (tok == token_types::open_square{}) {
          state = parser_state::array;
        }
        else if (tok == token_types::open_curly{}) {
          state = parser_state::object;
        }
        else if (tok == token_types::null_value{}) {
          return null{};
        }
        else if (auto str = std::get_if<token_types::string_literal>(&tok); str) {
          return string{ str->value };
        }
        else if(auto bln = std::get_if<token_types::boolean_literal>(&tok); bln) {
          return boolean{ bln->value };
        }
        else if (auto num = std::get_if<token_types::number_literal>(&tok); num) {
          return number{ num->value };
        }
        else {
          throw_error("Unexpected token");
        }

      }
      else if (state == parser_state::array) {

        array array_stack;

        if (_lexer.peek_token() == token_types::close_square{}) {
          _lexer.next_token(); // Consume close square
          return array_stack; // Empty array
        }

        array_stack.push_back(parse());
        while (_lexer.peek_token() == token_types::comma{}) {
          _lexer.next_token(); // Consume comma
          array_stack.push_back(parse());
        }

        _lexer.require_token<token_types::close_square>();

        return array_stack;

      }
      else if (state == parser_state::object) {

        object object_stack;

        if (_lexer.peek_token() == token_types::close_curly{}) {
          _lexer.next_token(); // Consume close curly
          return object_stack; // Empty object
        }

        const auto key = _lexer.require_token<token_types::string_literal>();
        _lexer.require_token<token_types::colon>();
        object_stack[key.value] = parse();

        while (_lexer.peek_token() == token_types::comma{}) {
          _lexer.next_token(); // Consume comma
          const auto key = _lexer.require_token<token_types::string_literal>();
          _lexer.require_token<token_types::colon>();
          object_stack[key.value] = parse();
        }

        _lexer.require_token<token_types::close_curly>();

        return object_stack;
      }
    }

    throw_error("Unexpected end of input");

  };



}