#include "test_include.hpp"
#include <cctype>

suite const satisfy = [] {
  constexpr auto is_l = [](auto c) { return c == 'l'; };
  constexpr auto greater_than_5 = [](auto num) { return num > 5; };
  constexpr auto dig_parser =
    prs::fmap([](char c) { return c - '0'; }, prs::one_of("0123456789"));

  "defaulted parser match"_test = [=] {
    static_assert(prs::satisfy(is_l)("lower") == std::pair{ 'l', "ower"sv });
  };

  "defaulted parser fail"_test = [=] {
    static_assert(prs::satisfy(is_l)("ower") == std::nullopt);
  };

  "explicit parser match"_test = [=] {
    static_assert(prs::satisfy(greater_than_5, dig_parser)("678")
                  == std::pair{ 6, "78"sv });
  };

  "explicit parser fail"_test = [=] {
    static_assert(
      prs::satisfy(greater_than_5, dig_parser)("578") == std::nullopt);
  };
};
