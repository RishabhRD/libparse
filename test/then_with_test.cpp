#include "test_include.hpp"

constexpr auto append_integer(int a, int b) { return a * 10 + b; }

constexpr auto to_digit(char c) { return c - '0'; }
constexpr auto digit_parser = parser::one_of("0123456789")//
                              | parser::transform(to_digit);

constexpr auto two_dig_parser =
  digit_parser//
  | parser::then_with(digit_parser, append_integer);

TEST_CASE("both parsers succeed") {
  static_assert(two_dig_parser("123") == std::pair{ 12, "3"sv });
};

TEST_CASE("first parser fails") {
  static_assert(two_dig_parser("a23") == std::nullopt);
};

TEST_CASE("second parser fails") {
  static_assert(two_dig_parser("1a3") == std::nullopt);
};
