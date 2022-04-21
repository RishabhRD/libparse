#include "test_include.hpp"

constexpr auto sum(int a, int b) { return a + b; }

constexpr auto to_digit(char c) { return c - '0'; }

constexpr auto concat_digits(int a, char b) -> int {
  return a = a * 10 + (b - '0');
}

constexpr auto digit_parser = parser::one_of("0123456789");

constexpr auto int_parser = digit_parser//
                            | parser::many1(0, concat_digits);
constexpr auto whitespace_parser = parser::many_of(' ');

constexpr auto plus_token_parser =
  whitespace_parser//
  | parser::ignore_previous(parser::symbol('+'))//
  | parser::ignore(whitespace_parser);

constexpr auto expr_sum_parser =
  parser::seperated_by(int_parser, plus_token_parser, 0, sum);


TEST_CASE("more than 1 values") {
  static_assert(expr_sum_parser("2  + 3   +    5") == std::pair{ 10, ""sv });
}
