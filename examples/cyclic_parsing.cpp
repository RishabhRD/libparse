#include <parser/parser.hpp>
#include <string_view>

using namespace std::string_view_literals;

/*
 * Grammar:
 *
 * expr = term + expr | term
 * term = factors * term | factors
 * factors = (expr) | int
 */

/*
 * Intution:
 *
 * Cyclic parsing needs a dependency cycle between parsers.
 * Lambdas are ussually great but functions in C++ is the
 * one only that serves this feature. So, our parsers would be
 * functions.
 */

constexpr auto char_to_int(char c) { return c - '0'; }

constexpr auto append_digits(int a, char b) { return a * 10 + char_to_int(b); }

constexpr auto whitespace_parser = parser::many_of(' ');

constexpr auto token(char c) {
  return whitespace_parser//
         | parser::ignore_previous(parser::symbol(c))//
         | parser::ignore(whitespace_parser);
}

constexpr auto digit_parser = parser::one_of("0123456789");

constexpr auto int_parser =
  parser::one_of("123456789")//
  | parser::then([](char c) {
      return parser::many(digit_parser, char_to_int(c), append_digits);
    });

template<parser::Parser P> constexpr auto trim_whitespace(P &&p) {
  return whitespace_parser//
         | parser::ignore_previous(std::forward<P>(p))//
         | parser::ignore(whitespace_parser);
}


constexpr auto expr_(std::string_view str) -> parser::parsed_t<int>;
constexpr auto expr = trim_whitespace(expr_);
constexpr auto terms(std::string_view str) -> parser::parsed_t<int>;
constexpr auto factors(std::string_view str) -> parser::parsed_t<int>;

constexpr auto expr_(std::string_view str) -> parser::parsed_t<int> {
  return parser::seperated_by(terms, token('+'), 0, std::plus<>{})(str);
}


constexpr auto terms(std::string_view str) -> parser::parsed_t<int> {
  return parser::seperated_by(factors, token('*'), 1, std::multiplies<>{})(str);
}

constexpr auto factors(std::string_view str) -> parser::parsed_t<int> {
  constexpr auto prs = parser::symbol('(')//
                       | parser::ignore_previous(expr)//
                       | parser::ignore(parser::symbol(')'))//
                       | parser::or_with(int_parser);
  return prs(str);
}

auto main() -> int {
  static_assert(int_parser("1").value().first == 1);
  static_assert(expr("1 +  2 + 3").value().first == 6);
  static_assert(expr("1 +  2 * 3").value().first == 7);
  static_assert(expr("1 *  2 * 3").value().first == 6);
  static_assert(expr("1 *  (2 + 3   )").value().first == 5);
  static_assert(expr("2 *  (2 + 3   )").value().first == 10);
  static_assert(expr("1 * 2").value().first == 2);
  static_assert(expr(" 1 * 2 * ( 2 + 3)").value().first == 10);
  static_assert(
    expr(
      "  1  *(2 + 3 * (   2 * 5 * (  3 +    5   ) * (5 + 2   ))) * (1 +  (1))")
      .value()
      .first
    == 3364);
}
