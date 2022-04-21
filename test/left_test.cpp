#include "test_include.hpp"

constexpr auto to_dig(char c) { return c - '0'; }

TEST_CASE("left") {
  constexpr auto whitespace_parser = parser::symbol(' ');
  constexpr auto dig_parser =
    parser::transform(parser::one_of("0123456789"), to_dig);
  constexpr auto whitespace_after_digit = dig_parser//
                                          | parser::ignore(whitespace_parser);
  static_assert(whitespace_after_digit("1 34") == std::pair{ 1, "34"sv });
}
