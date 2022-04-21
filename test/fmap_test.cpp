#include "test_include.hpp"

constexpr auto to_dig(char c) -> int { return c - '0'; }

TEST_CASE("failing parser mapping") {
  static_assert(prs::fmap(to_dig, prs::fail<char>())("123") == std::nullopt);
};

TEST_CASE("working parser mapping") {
  constexpr prs::Parser auto dig_parser = prs::one_of("0123456789");
  static_assert(prs::fmap(to_dig, dig_parser)("123") == std::pair{ 1, "23"sv });
};
