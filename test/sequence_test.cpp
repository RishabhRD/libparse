#include "test_include.hpp"

constexpr auto char_to_int(char c) { return c - '0'; }

constexpr auto append_digits(int a, char b) { return a * 10 + char_to_int(b); }

constexpr auto digit_parser = parser::one_of("0123456789");

constexpr auto whitespace_parser = parser::many_of(' ');

constexpr auto int_parser =
  parser::one_of("123456789")//
  | parser::then([](char c) {
      return parser::many(digit_parser, char_to_int(c), append_digits);
    });

constexpr auto int_then_whitespace_parser =
  int_parser | parser::ignore(whitespace_parser);

constexpr auto three_int_sum_parser =
  parser::sequence([](int a, int b, int c) { return a + b + c; },
    int_then_whitespace_parser,
    int_then_whitespace_parser,
    int_then_whitespace_parser);

TEST_CASE("when parser matches") {
  static_assert(three_int_sum_parser("12   10  11")->first == 33);
}

TEST_CASE("when parser matches") {
  static_assert(three_int_sum_parser("12   1011") == std::nullopt);
}
