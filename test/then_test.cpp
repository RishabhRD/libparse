#include "test_include.hpp"
#include <functional>

constexpr auto to_digit(char c) { return c - '0'; }
constexpr auto digit_parser = parser::one_of("0123456789")//
                              | parser::transform(to_digit);
constexpr auto append_digit(int a, int b) { return a * 10 + b; }

constexpr auto concat_digit(int num) {
  auto append_to_num = [=](int i) { return append_digit(num, i); };
  return digit_parser//
         | parser::transform(append_to_num);
}

constexpr auto two_digit_parser = digit_parser//
                                  | parser::then(concat_digit);

TEST_CASE("both parsers succeed") {
  static_assert(two_digit_parser("12a") == std::pair{ 12, "a"sv });
}

TEST_CASE("first parser fails") {
  static_assert(two_digit_parser("a2a") == std::nullopt);
}

TEST_CASE("second parser fails") {
  static_assert(two_digit_parser("1aa") == std::nullopt);
}
