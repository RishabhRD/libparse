#include "test_include.hpp"
#include <cctype>

constexpr auto to_dig(char c) { return c - '0'; }
constexpr auto greater_than_5 = [](auto num) { return num > 5; };
constexpr auto dig_parser =
  parser::transform(parser::one_of("0123456789"), to_dig);
constexpr auto greater_than_5_parser = dig_parser//
                                       | parser::if_satisfies(greater_than_5);

TEST_CASE("predicate satisfies") {
  static_assert(greater_than_5_parser("678") == std::pair{ 6, "78"sv });
}

TEST_CASE("explicit parser fail") {
  static_assert(greater_than_5_parser("567") == std::nullopt);
}
