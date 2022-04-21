#include "test_include.hpp"

constexpr auto is_l = [](auto c) { return c == 'l'; };

TEST_CASE("predicate satisfied") {
  static_assert(
    parser::if_char_satisfies(is_l)("lower") == std::pair{ 'l', "ower"sv });
}

TEST_CASE("predicate not satisfied") {
  static_assert(parser::if_char_satisfies(is_l)("ower") == std::nullopt);
}
