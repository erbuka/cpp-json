#include "lexer.hpp"

#include <format>
#include <stdexcept>

namespace json::internal {


  void lexer::throw_error(const std::string_view msg) {
    throw std::runtime_error(std::format("[Lexing error at {}:{}]: {} ", _cur_line, _cur_char, msg));
  }

  void lexer::expect(const char c) {
    const auto ret = next();
    if (ret != c) {
      throw_error(std::format("Unexpected character {}", c));
    }
  }

  char lexer::peek() {
    return _in.peek();
  }

  char lexer::next() {
    char ret;
    _in.read(&ret, 1);
    if (ret == '\n') {
      ++_cur_line;
      _cur_char = 0;
    }
    else {
      ++_cur_char;
    }
    return ret;
  }


  void lexer::reset(const marked_position& pos) {
    _cur_line = pos.cur_line;
    _cur_char = pos.cur_char;
    _in.seekg(pos.stream_pos, std::ios_base::beg);
  }

  lexer::marked_position lexer::mark() {
    return {
      .cur_line = _cur_line,
      .cur_char = _cur_char,
      .stream_pos = _in.tellg()
    };
  }

  token lexer::peek_token() {
    const auto pos = mark();
    const auto result = next_token();
    reset(pos);
    return result;
  }

  token lexer::next_token() {

    enum class state {
      none = 0,
      string_literal,
      number_literal,
      null_value,
      true_value,
      false_value,
    } cur_state = state::none;

    std::string accumulator;

    while (_in.good()) {

      if (cur_state == state::none) {

        // Skip spaces
        while (_in.good() && std::isspace(peek()))
          next();

        // If eof, no more tokens
        if (_in.eof()) {
          return token_types::eof{};
        }

        const char ch = next();

        switch (ch) {
        case '{': return token_types::open_curly{};
        case '}': return token_types::close_curly{};
        case '[': return token_types::open_square{};
        case ']': return token_types::close_square{};
        case '(': return token_types::open_par{};
        case ')': return token_types::close_par{};
        case ',': return token_types::comma{};
        case ':': return token_types::colon{};
        case 'n': cur_state = state::null_value; break;
        case 't': cur_state = state::true_value; break;
        case 'f': cur_state = state::false_value; break;
        case '"': cur_state = state::string_literal; break;
        default: {
          if (std::isdigit(ch) || ch == '.') {
            accumulator += ch;
            cur_state = state::number_literal;
          }
          else {
            throw_error(std::format("Unexpected character {}", ch));
          }
          break;
        }
        }
      }
      else if (cur_state == state::string_literal) {

        const char ch = next();

        if (ch == '"') {
          cur_state = state::none;
          return token_types::string_literal{ .value = std::exchange(accumulator, "") };
        }
        else if (ch != '\\') {
          accumulator += ch;
        }

      }
      else if (cur_state == state::number_literal) {

        const auto ch = peek();

        if (std::isdigit(ch) || ch == '.') {
          accumulator += ch;
          next();
        }
        else {
          cur_state = state::none;
          return token_types::number_literal{ .value = std::atof(std::exchange(accumulator, "").c_str()) };
        }

      }
      else if (cur_state == state::null_value) {
        expect('u');
        expect('l');
        expect('l');
        cur_state = state::none;
        return token_types::null_value{};
      }
      else if(cur_state == state::true_value) {
        expect('r');
        expect('u');
        expect('e');
        cur_state = state::none;
        return token_types::boolean_literal{true};
      }
      else if (cur_state == state::false_value) {
        expect('a');
        expect('l');
        expect('s');
        expect('e');
        cur_state = state::none;
        return token_types::boolean_literal{false};
      }

    }

    if (cur_state != state::none) {
      throw_error("Unexpected end of input");
    }

    return token_types::eof{};

  }
}
