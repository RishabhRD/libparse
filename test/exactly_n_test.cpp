#include "test_include.hpp"

constexpr auto concat_digits(int a, char b) -> int {
  return a = a * 10 + (b - '0');
}

constexpr auto digit_parser = parser::one_of("0123456789");
constexpr auto three_digit_parser = digit_parser//
                                    | parser::exactly_n(0, concat_digits, 3);


TEST_CASE("exactly 3") {
  static_assert(three_digit_parser("123abc") == std::pair{ 123, "abc"sv });
}

TEST_CASE("less than 3") {
  static_assert(three_digit_parser("12abc") == std::nullopt);
}

TEST_CASE("empty") { static_assert(three_digit_parser("") == std::nullopt); }

TEST_CASE("more than 3") {
  static_assert(three_digit_parser("1234abc") == std::pair{ 123, "4abc"sv });
}
