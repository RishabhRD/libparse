#include "test_include.hpp"

constexpr auto concat_digits(int a, char b) -> int {
  return a = a * 10 + (b - '0');
}

auto concat_string_custom(std::string res, char cur) -> std::string {
  res += cur;
  return res;
}

constexpr auto digit_parser = parser::one_of("0123456789");
constexpr auto int_parser = digit_parser//
                            | parser::many(0, concat_digits);

TEST_CASE("more than one matches") {
  static_assert(int_parser("123abc") == std::pair{ 123, "abc"sv });
}

TEST_CASE("only one match") {
  static_assert(int_parser("1abc") == std::pair{ 1, "abc"sv });
}

TEST_CASE("zero match") {
  static_assert(int_parser("abc") == std::pair{ 0, "abc"sv });
}

TEST_CASE("move required parser") {
  auto const string_parser = parser::any//
                             | parser::many(""s, concat_string_custom);
  REQUIRE(string_parser("abcdef") == std::pair{ "abcdef"s, ""sv });
}
