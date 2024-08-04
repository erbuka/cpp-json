#pragma once

#include <unordered_map>
#include <variant>
#include <string>
#include <stdexcept>
#include <concepts>

namespace json {

  class null {
  public:
    null() = default;
    null(std::nullptr_t) {}
    bool operator==(const null&) const { return true; }
    bool operator!=(const null&) const { return false; }
  };

  class string {
  public:
    string() = default;
    string(const std::string_view value) : _value(value) {}

    template<std::size_t N>
    string(const char(&value)[N]) : _value(value) {}

    auto operator<=>(const string& other) const { return _value <=> other._value; };
    bool operator==(const string& other) const = default;
    bool operator!=(const string& other) const = default;

    std::string value() const { return _value; }
  private:
    std::string _value{};
  };

  class number {
  public:
    number() = default;

    template <typename T> requires ((std::floating_point<T> || std::integral<T>) && !std::same_as<std::remove_cvref_t<T>, bool>)
      number(const T value) : _value(value) {}

    auto operator<=>(const number& other) const { return _value <=> other._value; };
    bool operator==(const number& other) const = default;
    bool operator!=(const number& other) const = default;
    double value() const { return _value; }
  private:
    double _value{};
  };

  class boolean {
  public:
    boolean() = default;
    
    template <typename T> requires std::same_as<std::remove_cvref_t<T>, bool>
    boolean(const T value) : _value(value) {}

    bool operator==(const boolean& other) const { return _value == other._value; };
    bool operator!=(const boolean& other) const { return _value != other._value; };
    bool value() const { return _value; }
  private:
    bool _value{};
  };

  class value;

  using array = std::vector<value>;
  using object = std::unordered_map<std::string, value>;

  using value_variant_t = std::variant<
    array,
    object,
    null,
    string,
    boolean,
    number>;

  class value : public value_variant_t {
  public:
    using value_variant_t::value_variant_t;
    using value_variant_t::operator=;
    std::string dump() const;

    template<typename T>
    T& as() {
      return std::get<T>(*this);  
    }

  };

  constexpr bool operator==(const value& lhs, const value& rhs) {
    return static_cast<value_variant_t>(lhs) == static_cast<value_variant_t>(rhs);
  }

  constexpr bool operator!=(const value& lhs, const value& rhs) {
    return static_cast<value_variant_t>(lhs) != static_cast<value_variant_t>(rhs);
  }


}