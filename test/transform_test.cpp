#include "test_include.hpp"

constexpr auto to_dig(char c) -> int { return c - '0'; }

TEST_CASE("failing parser mapping") {
  constexpr auto parser = parser::empty<char>//
                          | parser::transform(to_dig);
  static_assert(parser("123") == std::nullopt);
};

TEST_CASE("working parser mapping") {
  constexpr parser::Parser auto dig_parser = parser::one_of("0123456789")//
                                             | parser::transform(to_dig);
  static_assert(dig_parser("123") == std::pair{ 1, "23"sv });
};
