#pragma once

#include <optional>
#include <string_view>
#include <algorithm>
#include <utility>
#include <functional>

namespace parser {
template<typename T>
using parsed_t = std::optional<std::pair<T, std::string_view>>;

template<typename ParserFunc>
concept Parser =
  std::regular_invocable<ParserFunc, std::string_view> && std::same_as<
    std::invoke_result_t<ParserFunc, std::string_view>,
    parsed_t<typename std::invoke_result_t<ParserFunc,
      std::string_view>::value_type::first_type>>;

template<Parser P>
using parser_result_t = typename std::invoke_result_t<P, std::string_view>;

template<Parser P>
using parser_value_t = typename parser_result_t<P>::value_type::first_type;

template<typename ParserFunc, typename T>
concept ParserOf =
  Parser<ParserFunc> && std::same_as<parser_value_t<ParserFunc>, T>;

namespace detail {

  namespace pipes {

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

    template<typename F> constexpr auto make_pipe_adapter(F &&f) {
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
  }// namespace pipes

  template<Parser P, std::regular_invocable<parser_value_t<P>> F>
  auto constexpr transform(P && p, F && f) noexcept {
    using R = parsed_t<std::invoke_result_t<F, parser_value_t<P>>>;
    return [p = std::forward<P>(p), f = std::forward<F>(f)](
             std::string_view str) -> R {
      auto opt_i_res = std::invoke(p, str);
      if (opt_i_res == std::nullopt) return std::nullopt;
      return std::make_pair(
        std::invoke(f, std::move(opt_i_res->first)), opt_i_res->second);
    };
  }

  template<Parser P1, Parser P2>
  requires(
    std::same_as<parser_result_t<P1>, parser_result_t<P2>>) constexpr auto
    or_with(P1 && p1, P2 && p2) noexcept {
    using R = parser_result_t<P1>;
    return [p1 = std::forward<P1>(p1), p2 = std::forward<P2>(p2)](
             std::string_view str) -> R {
      auto opt_i_res = std::invoke(p1, str);
      if (opt_i_res) return *opt_i_res;
      return std::invoke(p2, str);
    };
  }

  template<Parser P1,
    Parser P2,
    std::regular_invocable<parser_value_t<P1>, parser_value_t<P2>> F>
  constexpr auto combine(P1 && p1, P2 && p2, F && f) noexcept {
    using R =
      parsed_t<std::invoke_result_t<F, parser_value_t<P1>, parser_value_t<P2>>>;
    return [p1 = std::forward<P1>(p1),
             p2 = std::forward<P2>(p2),
             f = std::forward<F>(f)](std::string_view str) -> R {
      auto opt_i_res = std::invoke(p1, str);
      if (!opt_i_res) return {};
      auto opt_res = std::invoke(p2, opt_i_res->second);
      if (!opt_res) return {};
      return std::make_pair(
        std::invoke(f, std::move(opt_i_res->first), std::move(opt_res->first)),
        opt_res->second);
    };
  }

  template<Parser P1, Parser P2>
  constexpr auto ignore_previous(P1 && p1, P2 && p2) noexcept {
    return combine(std::forward<P1>(p1),
      std::forward<P2>(p2),
      [](auto, auto r) { return r; });
  }

  template<Parser P1, Parser P2>
  constexpr auto ignore(P1 && p1, P2 && p2) noexcept {
    return combine(std::forward<P1>(p1),
      std::forward<P2>(p2),
      [](auto r, auto) { return r; });
  }

  template<Parser P, std::predicate<parser_value_t<P>> Predicate>
  constexpr auto if_satisfies(P && p, Predicate && pr) {
    return [p = std::forward<P>(p), pr = std::forward<Predicate>(pr)](
             std::string_view str) -> parser_result_t<P> {
      if (auto res = std::invoke(p, str)) {
        if (std::invoke(pr, res->first)) {
          return std::make_pair(std::move(res->first), res->second);
        }
      }
      return {};
    };
  }

  template<Parser P, std::regular_invocable<parser_value_t<P>> F>
  requires(Parser<std::invoke_result_t<F, parser_value_t<P>>>) constexpr auto
    then(P && p, F && f) {
    using R = parser_result_t<std::invoke_result_t<F, parser_value_t<P>>>;
    return [p = std::forward<P>(p), f = std::forward<F>(f)](
             std::string_view str) -> R {
      auto opt_i_res = std::invoke(p, str);
      if (!opt_i_res) return {};
      return std::invoke(
        std::invoke(f, std::move(opt_i_res->first)), opt_i_res->second);
    };
  }

  // many: Parser a -> b -> (b -> a -> b) -> Parser b
  template<Parser P, typename B, std::invocable<B, parser_value_t<P>> F>
  requires(std::same_as<std::invoke_result_t<F, B, parser_value_t<P>>,
    B>) constexpr auto
    many(P && p, B b, F && f) {
    return [p = std::forward<P>(p), b = std::move(b), f = std::forward<F>(f)](
             std::string_view str) -> parsed_t<B> {
      auto init = b;
      while (auto res = p(str)) {
        init = std::invoke(f, std::move(init), std::move(res->first));
        str = res->second;
      }
      return std::make_pair(init, str);
    };
  }

  // many1: Parser a -> b -> (b -> a -> b) -> Parser b
  template<Parser P, typename B, std::invocable<B, parser_value_t<P>> F>
  requires(std::same_as<std::invoke_result_t<F, B, parser_value_t<P>>,
    B>) constexpr auto
    many1(P && p, B b, F && f) {
    return [p = std::forward<P>(p), b = std::move(b), f = std::forward<F>(f)](
             std::string_view str) -> parsed_t<B> {
      auto res = std::invoke(p, str);
      if (!res) return {};
      return many(p, std::invoke(f, b, std::move(res->first)), f)(res->second);
    };
  }

  template<Parser P, typename B, std::invocable<B, parser_value_t<P>> F>
  requires(std::same_as<std::invoke_result_t<F, B, parser_value_t<P>>,
    B>) constexpr auto
    exactly_n(P && p, B b, F && f, std::size_t n) {
    return
      [p = std::forward<P>(p), b = std::move(b), f = std::forward<F>(f), n](
        std::string_view str) -> parsed_t<B> {
        auto init = b;
        for (size_t i{}; i < n; ++i) {
          if (auto res = p(str)) {
            init = std::invoke(f, std::move(init), std::move(res->first));
            str = res->second;
          } else {
            return {};
          }
        }
        return std::make_pair(init, str);
      };
  }

  template<Parser P1, Parser P2, typename B, typename F>
  requires(std::same_as<std::invoke_result_t<F, B, parser_value_t<P1>>,
    B>) constexpr auto
    seperated_by(P1 && p1, P2 && p2, B b, F && f) {
    return [p1 = std::forward<P1>(p1),
             p2 = std::forward<P2>(p2),
             b = std::move(b),
             f = std::forward<F>(f)](std::string_view str) -> parsed_t<B> {
      auto res = std::invoke(p1, str);
      if (!res) return {};
      return many(ignore_previous(p2, p1),
        std::invoke(f, b, std::move(res->first)),
        f)(res->second);
    };
  }

}// namespace detail

template<typename F> constexpr auto piped(F &&f) noexcept {
  return detail::pipes::pipeable<F>{ std::forward<F>(f) };
}

constexpr auto any = [](std::string_view str) -> parsed_t<char> {
  if (std::empty(str)) return {};
  return std::make_pair(str[0], str.substr(1));
};

constexpr auto symbol(char c) noexcept {
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

template<typename T>
constexpr auto empty = [](std::string_view) -> parsed_t<T> { return {}; };

template<typename T> constexpr auto always(T val) {
  return [val = std::move(val)](std::string_view str) -> parsed_t<T> {
    return std::make_pair(val, str);
  };
}

constexpr auto transform = piped([]<typename P1, typename F>(P1 &&p1, F &&f) {
  return detail::transform(std::forward<P1>(p1), std::forward<F>(f));
});

constexpr auto or_with =
  piped([]<typename P1, typename P2>(P1 &&p1, P2 &&p2) noexcept {
    return detail::or_with(std::forward<P1>(p1), std::forward<P2>(p2));
  });

constexpr auto combine_with =
  piped([]<typename P1, typename P2, typename F>(P1 &&p1, P2 &&p2, F &&f) {
    return detail::combine(
      std::forward<P1>(p1), std::forward<P2>(p2), std::forward<F>(f));
  });

constexpr auto ignore_previous =
  piped([]<typename P1, typename P2>(P1 &&p1, P2 &&p2) noexcept {
    return detail::ignore_previous(std::forward<P1>(p1), std::forward<P2>(p2));
  });

constexpr auto snd = ignore_previous;

constexpr auto ignore =
  piped([]<typename P1, typename P2>(P1 &&p1, P2 &&p2) noexcept {
    return detail::ignore(std::forward<P1>(p1), std::forward<P2>(p2));
  });

constexpr auto fst = ignore;

constexpr auto if_satisfies =
  piped([]<typename P1, typename F>(P1 &&p1, F &&f) {
    return detail::if_satisfies(std::forward<P1>(p1), std::forward<F>(f));
  });

template<typename F> constexpr auto if_char_satisfies(F &&f) {
  return if_satisfies(any, std::forward<F>(f));
}

constexpr auto then = piped([]<typename P1, typename F>(P1 &&p1, F &&f) {
  return detail::then(std::forward<P1>(p1), std::forward<F>(f));
});

constexpr auto many =
  piped([]<typename P, typename B, typename F>(P &&p, B b, F &&f) {
    return detail::many(std::forward<P>(p), std::move(b), std::forward<F>(f));
  });

constexpr auto many1 =
  piped([]<typename P, typename B, typename F>(P &&p, B b, F &&f) {
    return detail::many1(std::forward<P>(p), std::move(b), std::forward<F>(f));
  });

constexpr auto exactly_n =
  piped([]<typename P, typename B, typename F>(P &&p, B b, F &&f, size_t n) {
    return detail::exactly_n(
      std::forward<P>(p), std::move(b), std::forward<F>(f), n);
  });

constexpr auto seperated_by =
  piped([]<typename P1, typename P2, typename B, typename F>(P1 &&p1,
          P2 &&p2,
          B b,
          F &&f) {
    return detail::seperated_by(std::forward<P1>(p1),
      std::forward<P2>(p2),
      std::move(b),
      std::forward<F>(f));
  });

constexpr auto many1_of(char c) {
  return [c](std::string_view str) {
    using namespace std::string_view_literals;
    return detail::many1(symbol(c), ""sv, [str](auto prev, auto) {
      return str.substr(0, prev.size() + 1);
    })(str);
  };
}

template<std::predicate<char> Predicate>
constexpr auto many1_if(Predicate &&p) {
  return [p = std::forward<Predicate>(p)](std::string_view str) {
    using namespace std::string_view_literals;
    return detail::many1(if_char_satisfies(p), ""sv, [str](auto prev, auto) {
      return str.substr(0, prev.size() + 1);
    })(str);
  };
}

constexpr auto many_of(char c) {
  using namespace std::string_view_literals;
  return or_with(many1_of(c), always(""sv));
}

template<std::predicate<char> Predicate> constexpr auto many_if(Predicate &&p) {
  using namespace std::string_view_literals;
  return or_with(many1_if(std::forward<Predicate>(p)), always(""sv));
}

};// namespace parser
