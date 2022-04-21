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

namespace detail::pipes {

  template<typename F> struct pipe_adapter {
    F f;

  public:
    template<typename Arg> constexpr auto operator()(Arg &&arg) const {
      return std::invoke(f, std::forward<Arg>(arg));
    }
  };

  template<typename Arg, typename F>
  constexpr auto operator|(Arg &&arg, pipe_adapter<F> const &p) {
    return p(std::forward<Arg>(arg));
  }

  template<typename F> constexpr auto make_pipe_adapter(F && f) {
    return pipe_adapter<F>{ std::forward<F>(f) };
  }

  template<typename F> struct pipeable {
  private:
    F f;

  public:
    constexpr explicit pipeable(F &&f_p) noexcept : f(std::forward<F>(f_p)) {}

    template<typename... Xs>
    requires std::invocable<F, Xs...>
    constexpr auto operator()(Xs &&...xs) const {
      return std::invoke(f, std::forward<Xs>(xs)...);
    }

    template<typename... Xs> constexpr auto operator()(Xs &&...xs) const {
      return make_pipe_adapter(
        std::bind(f, std::placeholders::_1, std::forward<Xs>(xs)...));
    }
  };

}// namespace detail::pipes

template<typename F> constexpr auto piped(F &&f) noexcept {
  return detail::pipes::pipeable<F>{ std::forward<F>(f) };
}

constexpr auto item() noexcept {
  return [](std::string_view str) -> parsed_t<char> {
    if (std::empty(str)) return {};
    return std::make_pair(str[0], str.substr(1));
  };
}

constexpr auto ele(char c) noexcept {
  return [c](std::string_view str) -> parsed_t<char> {
    if (std::empty(str) || str[0] != c) return {};
    return std::make_pair(str[0], str.substr(1));
  };
};

constexpr auto one_of(std::string_view sv) noexcept {
  return [sv](std::string_view str) -> parsed_t<char> {
    if (std::empty(str)) return {};
    const auto *const itr = std::find(cbegin(sv), cend(sv), str[0]);
    if (itr == cend(sv)) return {};
    return std::make_pair(str[0], str.substr(1));
  };
}

constexpr auto none_of(std::string_view sv) noexcept {
  return [sv](std::string_view str) -> parsed_t<char> {
    if (std::empty(str)) return {};
    const auto *const itr = std::find(cbegin(sv), cend(sv), str[0]);
    if (itr != cend(sv)) return {};
    return std::make_pair(str[0], str.substr(1));
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

constexpr auto or_with = piped([]<Parser P1, Parser P2> requires(
  std::same_as<parser_result_t<P1>, parser_result_t<P2>>)(P1 && p1,
  P2 &&p2) noexcept {
  using R = parser_result_t<P1>;
  return [p1 = std::forward<P1>(p1), p2 = std::forward<P2>(p2)](
           std::string_view str) -> R {
    auto opt_i_res = std::invoke(std::forward<decltype(p1)>(p1), str);
    if (opt_i_res) return *opt_i_res;
    return std::invoke(std::forward<decltype(p2)>(p2), str);
  };
});

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

template<Parser P = decltype(item()),
  std::predicate<parser_value_t<P>> Predicate>
constexpr auto satisfy(Predicate &&pr, P &&p = item()) {
  return [p = std::forward<P>(p), pr = std::forward<Predicate>(pr)](
           std::string_view str) -> parser_result_t<P> {
    if (auto res = std::invoke(std::forward<decltype(p)>(p), str)) {
      if (std::invoke(std::forward<decltype(pr)>(pr), res->first)) {
        return std::make_pair(std::move(res->first), res->second);
      }
    }
    return {};
  };
}

};// namespace prs
