#include "test_include.hpp"
#include <cctype>

constexpr auto is_l = [](auto c) { return c == 'l'; };
constexpr auto greater_than_5 = [](auto num) { return num > 5; };
constexpr auto dig_parser =
  prs::fmap([](char c) { return c - '0'; }, prs::one_of("0123456789"));

TEST_CASE("defaulted parser match") {
  static_assert(prs::satisfy(is_l)("lower") == std::pair{ 'l', "ower"sv });
}

TEST_CASE("defaulted parser fail") {
  static_assert(prs::satisfy(is_l)("ower") == std::nullopt);
}

TEST_CASE("explicit parser match") {
  static_assert(
    prs::satisfy(greater_than_5, dig_parser)("678") == std::pair{ 6, "78"sv });
}

TEST_CASE("explicit parser fail") {
  static_assert(
    prs::satisfy(greater_than_5, dig_parser)("578") == std::nullopt);
}
