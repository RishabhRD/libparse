#pragma once

#include <optional>
#include <string_view>
#include <algorithm>

namespace prs {
template<typename T>
using parsed_t = std::optional<std::pair<T, std::string_view>>;

template<typename ParserFunc>
concept Parser =
  std::regular_invocable<ParserFunc, std::string_view> && requires(
    std::invoke_result_t<ParserFunc, std::string_view> res) {
  std::same_as<decltype(res),
    parsed_t<typename decltype(res)::value_type::first_type>>;
};

template<Parser P>
using parser_result_t = typename std::invoke_result_t<P, std::string_view>;

template<Parser P>
using parser_value_t = typename parser_result_t<P>::value_type::first_type;

template<typename ParserFunc, typename T>
concept ParserOf =
  Parser<ParserFunc> && std::same_as<parser_value_t<ParserFunc>, T>;

constexpr auto item() noexcept {
  return [](std::string_view str) -> parsed_t<char> {
    if (std::empty(str)) return {};
    return std::make_pair(str.front(), str.substr(1));
  };
}

constexpr auto ele(char c) noexcept {
  return [c](std::string_view str) -> parsed_t<char> {
    if (std::empty(str) || str.front() != c) return {};
    return std::make_pair(str.front(), str.substr(1));
  };
};

constexpr auto one_of(std::string_view sv) noexcept {
  return [sv](std::string_view str) -> parsed_t<char> {
    if (std::empty(str)) return {};
    const auto *const itr = std::find(cbegin(sv), cend(sv), str.front());
    if (itr == cend(sv)) return {};
    return std::make_pair(str.front(), str.substr(1));
  };
}

constexpr auto none_of(std::string_view sv) noexcept {
  return [sv](std::string_view str) -> parsed_t<char> {
    if (std::empty(str)) return {};
    const auto *const itr = std::find(cbegin(sv), cend(sv), str.front());
    if (itr != cend(sv)) return {};
    return std::make_pair(str.front(), str.substr(1));
  };
}

constexpr auto str(std::string_view sv) noexcept {
  return [sv](std::string_view str) -> parsed_t<std::string_view> {
    auto [sv_itr, str_itr] =
      std::mismatch(cbegin(sv), cend(sv), cbegin(str), cend(str));
    if (sv_itr != cend(sv)) { return {}; }
    return std::make_pair(sv, str.substr(sv.size()));
  };
}

};// namespace prs
