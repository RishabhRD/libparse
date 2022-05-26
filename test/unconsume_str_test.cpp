#include "test_include.hpp"

constexpr auto char_to_int(char c) { return c - '0'; }

constexpr auto append_digits(int a, char b) { return a * 10 + char_to_int(b); }

constexpr auto digit_parser = parser::one_of("0123456789");

constexpr auto int_parser =
  parser::one_of("123456789")//
  | parser::then([](char c) {
      return parser::many(digit_parser, char_to_int(c), append_digits);
    });

constexpr auto int_unconsume_parser = int_parser | parser::unconsume_str();

constexpr auto repeated_int_parser =
  parser::sequence([](auto a, auto b, auto c) { return a + b + c; },
    int_unconsume_parser,
    int_unconsume_parser,
    int_unconsume_parser);


TEST_CASE("when parser matches") {
  constexpr auto str = "12";
  static_assert(repeated_int_parser(str)->first == 36);
  static_assert(repeated_int_parser(str)->second == "12");
}

TEST_CASE("when parser fails to parse") {
  constexpr auto str = "a12";
  static_assert(repeated_int_parser(str) == std::nullopt);
}
