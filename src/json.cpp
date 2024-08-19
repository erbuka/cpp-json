#include <json/json.hpp>

#include <sstream>
#include <iomanip>
#include <ranges>
#include <algorithm>
#include <iterator>
#include <format>

#include "parser.hpp"

namespace json {

  template<typename R, typename OutIter, typename Value = std::ranges::range_value_t<R>>
    requires (std::ranges::range<R>&& std::output_iterator<OutIter, Value>)
  static void join_with(R&& r, const Value& separator, OutIter iter) {
    if (!std::ranges::empty(r)) {
      *iter++ = *(std::ranges::begin(r));
    }

    for (const auto& el : r | std::views::drop(1)) {
      *iter++ = separator;
      *iter++ = el;
    }
  }

  std::size_t value::size() const {
    if (std::holds_alternative<array>(*this)) {
      return get<array>().size();
    }
    else if (std::holds_alternative<object>(*this)) {
      return get<object>().size();
    }
    else {
      throw std::runtime_error("size() called on non-container type");
    }
  }

  std::string value::dump() const {
    return std::visit<std::string>([]<typename T>(const T & val) {
      if constexpr (std::is_same_v<T, null>) {
        return "null";
      }
      else if constexpr (std::is_same_v<T, boolean>) {
        return val.value() ? "true" : "false";
      }
      else if constexpr (std::is_same_v<T, string>) {
        std::stringstream ss;
        ss << std::quoted(val.value());
        return ss.str();
      }
      else if constexpr (std::is_same_v<T, number>) {
        return std::to_string(val.value());
      }
      else if constexpr (std::is_same_v<T, object>) {
        std::string result;

        const auto dumped = val | std::views::transform([](const auto& keyVal) {
          return std::format("\"{}\":{}", keyVal.first, keyVal.second.dump());
          });
        std::vector<std::string> joined;
        join_with(dumped, ",", std::back_inserter(joined));

        result.append("{");
        for (auto& j : joined) {
          result.append(std::move(j));
        }
        result.append("}");
        return result;
      }
      else if constexpr (std::is_same_v<T, array>) {
        std::string result;

        const auto dumped = val | std::views::transform([](const auto& el) { return el.dump(); });
        std::vector<std::string> joined;
        join_with(dumped, ",", std::back_inserter(joined));

        result.append("[");
        for (auto& j : joined) {
          result.append(std::move(j));
        }
        result.append("]");
        return result;
      }
      else {
        return "unknown";
        //static_assert(false, "non-exhaustive visitor");
      }
    }, static_cast<value_variant_t>(*this));
  }

  value parse(const std::string& str) {
    std::istringstream in(str);
    internal::lexer lex(in);
    return internal::parser(lex).parse();
  }


}