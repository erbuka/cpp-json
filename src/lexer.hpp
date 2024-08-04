#pragma once

#include <istream>
#include <string>
#include <variant>
#include <optional>

namespace json::internal {

  namespace token_types {

    template<typename T>
    struct identity_equality {
      bool operator==(const T&) const { return true; }
      bool operator!=(const T&) const { return false; }
    };

    struct eof : identity_equality<eof> {};
    struct comma : identity_equality<comma> {};
    struct colon : identity_equality<colon> {};
    struct open_square : identity_equality<open_square> {};
    struct close_square : identity_equality<close_square> {};
    struct open_curly : identity_equality<open_curly> {};
    struct close_curly : identity_equality<close_curly> {};
    struct open_par : identity_equality<open_par> {};
    struct close_par : identity_equality<close_par> {};

    struct null_value : identity_equality<null_value> {};

    struct number_literal { double value{}; };

    struct string_literal { std::string value; };

    struct boolean_literal { bool value{}; }; 

    constexpr bool operator==(const boolean_literal& lhs, const boolean_literal& rhs) { return lhs.value == rhs.value; }
    constexpr bool operator==(const number_literal& lhs, const number_literal& rhs) { return lhs.value == rhs.value; }
    constexpr bool operator==(const string_literal& lhs, const string_literal& rhs) { return lhs.value == rhs.value; }

  }

  using token_variant_t = std::variant<
    token_types::eof,

    token_types::comma,
    token_types::colon,

    token_types::open_square,
    token_types::close_square,

    token_types::open_curly,
    token_types::close_curly,

    token_types::open_par,
    token_types::close_par,

    token_types::null_value,

    token_types::number_literal,

    token_types::string_literal,

    token_types::boolean_literal
  >;

  class token : public token_variant_t {
  public:
    using token_variant_t::token_variant_t;
    using token_variant_t::operator=;

  };

  constexpr bool operator==(const token& lhs, const token& rhs) {
    return static_cast<token_variant_t>(lhs) == static_cast<token_variant_t>(rhs);
  }

  constexpr bool operator!=(const token& lhs, const token& rhs) {
    return static_cast<token_variant_t>(lhs) != static_cast<token_variant_t>(rhs);
  }


  class lexer {
  public:

    struct marked_position {
      std::size_t cur_line{}, cur_char{};
      std::istream::pos_type stream_pos{};
    };

    lexer(std::istream& in) : _in(in) {}
    token next_token();
    token peek_token();

    template<typename T>
    T require_token() {
      const auto tok = next_token();
      if (std::holds_alternative<T>(tok)) {
        return std::move(std::get<T>(tok));
      }
      else {
        throw_error("Unexpected token");
      }
    }

    void reset(const marked_position& pos);
    marked_position mark();

    std::size_t get_current_line() const { return _cur_line; }
    std::size_t get_current_character() const { return _cur_char; }

  private:
    char peek();
    char next();
    void expect(const char c);
    void throw_error(const std::string_view msg);
    std::size_t _cur_char{}, _cur_line{};
    std::istream& _in;
  };

}