#include <iostream>
#include <parser/parser.hpp>
#include <string_view>

using namespace std::string_view_literals;

constexpr auto to_digit(char c) { return c - '0'; }
constexpr auto concat_digit(int a, int b) { return a * 10 + b; }
constexpr auto add_int(int a, int b) { return a + b; }

constexpr auto digit_parser = parser::one_of("01234567889")//
                              | parser::transform(to_digit);
constexpr auto whitespace_parser = parser::symbol(' ');
constexpr auto plus_parser = parser::symbol('+');


// parses string like <1 2 3> -> 123
constexpr auto three_dig_parser =
  digit_parser//
  | parser::ignore(whitespace_parser)
  | parser::then_with(digit_parser, concat_digit)
  | parser::ignore(whitespace_parser)
  | parser::then_with(digit_parser, concat_digit);

// parse string like <1 + 2 + 3 > -> 6
constexpr auto three_dig_sum = digit_parser//
                               | parser::ignore(parser::str(" + "))//
                               | parser::then_with(digit_parser, add_int)//
                               | parser::ignore(parser::str(" + "))//
                               | parser::then_with(digit_parser, add_int)//
                               | parser::ignore(whitespace_parser);


auto main() -> int {
  static_assert(three_dig_parser("3 6 1") == std::pair{ 361, ""sv });
  static_assert(three_dig_sum("3 + 6 + 1 ").value().first == 10);
}
