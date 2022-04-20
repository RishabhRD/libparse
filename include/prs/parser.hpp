#pragma once

#include <optional>
#include <string_view>
#include <algorithm>
#include <utility>

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

template<typename T> constexpr auto fail() noexcept {
  return [](std::string_view) -> parsed_t<T> { return {}; };
}

template<Parser P, std::regular_invocable<parser_value_t<P>> F>
constexpr auto fmap(F &&f, P &&p) noexcept {
  using R = parsed_t<std::invoke_result_t<F, parser_value_t<P>>>;
  return [p = std::forward<P>(p), f = std::forward<F>(f)](
           std::string_view str) -> R {
    auto opt_i_res = std::invoke(std::forward<decltype(p)>(p), str);
    if (opt_i_res == std::nullopt) return std::nullopt;
    return std::make_pair(
      std::invoke(std::forward<decltype(f)>(f), std::move(opt_i_res->first)),
      opt_i_res->second);
  };
}

template<Parser P1, Parser P2>
requires(std::same_as<parser_result_t<P1>, parser_result_t<P2>>) constexpr auto
  operator|(P1 &&p1, P2 &&p2) noexcept {
  using R = parser_result_t<P1>;
  return [p1 = std::forward<P1>(p1), p2 = std::forward<P2>(p2)](
           std::string_view str) -> R {
    auto opt_i_res = std::invoke(std::forward<decltype(p1)>(p1), str);
    if (opt_i_res) return *opt_i_res;
    return std::invoke(std::forward<decltype(p2)>(p2), str);
  };
}

template<Parser P1,
  Parser P2,
  std::regular_invocable<parser_value_t<P1>, parser_value_t<P2>> F>
constexpr auto combine(P1 &&p1, P2 &&p2, F &&f) noexcept {
  using R =
    parsed_t<std::invoke_result_t<F, parser_value_t<P1>, parser_value_t<P2>>>;
  return [p1 = std::forward<P1>(p1),
           p2 = std::forward<P2>(p2),
           f = std::forward<F>(f)](std::string_view str) -> R {
    auto opt_i_res = std::invoke(std::forward<decltype(p1)>(p1), str);
    if (!opt_i_res) return {};
    auto opt_res =
      std::invoke(std::forward<decltype(p2)>(p2), opt_i_res->second);
    if (!opt_res) return {};
    return std::make_pair(
      std::invoke(f, std::move(opt_i_res->first), std::move(opt_res->first)),
      opt_res->second);
  };
}

template<Parser P1, Parser P2>
constexpr auto operator<(P1 &&p1, P2 &&p2) noexcept {
  return combine(
    std::forward<P1>(p1), std::forward<P2>(p2), [](auto, auto r) { return r; });
}

template<Parser P1, Parser P2>
constexpr auto operator>(P1 &&p1, P2 &&p2) noexcept {
  return combine(
    std::forward<P1>(p1), std::forward<P2>(p2), [](auto r, auto) { return r; });
}


};// namespace prs
