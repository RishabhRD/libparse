#include "test_include.hpp"

constexpr auto to_dig(char c) { return c - '0'; }

TEST_CASE("right") {
  constexpr auto whitespace_parser = parser::symbol(' ');
  constexpr auto dig_parser =
    parser::transform(parser::one_of("0123456789"), to_dig);
  constexpr auto whitespace_after_digit = whitespace_parser//
                                          | parser::ignore_previous(dig_parser);
  static_assert(whitespace_after_digit(" 134") == std::pair{ 1, "34"sv });
}
