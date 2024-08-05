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
    [[nodiscard]] bool operator==(const null&) const { return true; }
    [[nodiscard]] bool operator!=(const null&) const { return false; }
  };

  class string {
  public:
    string() = default;
    string(const std::string_view value) : _value(value) {}

    template<std::size_t N>
    string(const char(&value)[N]) : _value(value) {}

    [[nodiscard]] auto operator<=>(const string& other) const { return _value <=> other._value; };
    [[nodiscard]] bool operator==(const string& other) const = default;
    [[nodiscard]] bool operator!=(const string& other) const = default;

    [[nodiscard]] std::string value() const { return _value; }
  private:
    std::string _value{};
  };

  class number {
  public:
    number() = default;

    template <typename T> requires ((std::floating_point<T> || std::integral<T>) && !std::same_as<std::remove_cvref_t<T>, bool>)
      number(const T value) : _value(value) {}

    [[nodiscard]] auto operator<=>(const number& other) const { return _value <=> other._value; };
    [[nodiscard]] bool operator==(const number& other) const = default;
    [[nodiscard]] bool operator!=(const number& other) const = default;
    [[nodiscard]] double value() const { return _value; }
  private:
    double _value{};
  };

  class boolean {
  public:
    boolean() = default;
    
    template <typename T> requires std::same_as<std::remove_cvref_t<T>, bool>
    boolean(const T value) : _value(value) {}

    [[nodiscard]] bool operator==(const boolean& other) const { return _value == other._value; };
    [[nodiscard]] bool operator!=(const boolean& other) const { return _value != other._value; };
    [[nodiscard]] bool value() const { return _value; }
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
    std::size_t size() const;

    [[nodiscard]] inline value& operator[](const std::string& key) { return get<json::object>().at(key); }
    [[nodiscard]] inline const value& operator[](const std::string& key) const { return get<json::object>().at(key); }

    [[nodiscard]] inline value& operator[](const std::size_t index) { return get<json::array>()[index]; }
    [[nodiscard]] inline const value& operator[](const std::size_t index) const { return get<json::array>()[index]; }

    template<typename T>
    [[nodiscard]] T& get() {
      return std::get<T>(*this);  
    }

    template<typename T> 
    [[nodiscard]] const T& get() const {
      return std::get<T>(*this);
    }


    template<typename T>
    [[nodiscard]] bool is() const {
      return std::holds_alternative<T>(*this);
    }

    template<typename T>
    [[nodiscard]] T* get_if() {
      return std::get_if<T>(this);
    }

    template<typename T>
    [[nodiscard]] const T* get_if() const {
      return std::get_if<T>(this);
    }

  };

  [[nodiscard]] inline bool operator==(const value& lhs, const value& rhs) {
    return static_cast<value_variant_t>(lhs) == static_cast<value_variant_t>(rhs);
  }

  [[nodiscard]] inline bool operator!=(const value& lhs, const value& rhs) {
    return static_cast<value_variant_t>(lhs) != static_cast<value_variant_t>(rhs);
  }


}