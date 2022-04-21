#include "test_include.hpp"

constexpr auto is_same_char(char a, char b) -> bool { return a == b; }

constexpr auto c_parser = parser::symbol('c');
constexpr auto d_parser = parser::symbol('d');
constexpr auto cc_parser = c_parser//
                           | parser::combine_with(c_parser, is_same_char);
constexpr auto cd_parser = c_parser//
                           | parser::combine_with(d_parser, is_same_char);

TEST_CASE("first fail") { static_assert(cc_parser("bcd") == std::nullopt); }

TEST_CASE("second fail") { static_assert(cc_parser("cbd") == std::nullopt); }

TEST_CASE("both passes") {
  static_assert(cd_parser("cde") == std::pair{ false, "e"sv });
}
