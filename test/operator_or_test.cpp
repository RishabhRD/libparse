#include "test_include.hpp"

constexpr auto a_parser = prs::ele('a');
constexpr auto b_parser = prs::ele('b');
constexpr auto c_parser = prs::ele('c');
constexpr auto aa_parser = a_parser//
                           | prs::or_with(a_parser);
constexpr auto ab_parser = a_parser//
                           | prs::or_with(b_parser);
constexpr auto abc_parsr = a_parser//
                           | prs::or_with(b_parser)//
                           | prs::or_with(c_parser);

TEST_CASE("both fails") { static_assert(ab_parser("def") == std::nullopt); }

TEST_CASE("first fails") {
  static_assert(ab_parser("bcd") == std::pair{ 'b', "cd"sv });
}

TEST_CASE("second fails") {
  static_assert(ab_parser("abc") == std::pair{ 'a', "bc"sv });
}

TEST_CASE("both passes") {
  static_assert(aa_parser("abc") == std::pair{ 'a', "bc"sv });
}

TEST_CASE("three parsers") {
  static_assert(abc_parsr("cba") == std::pair{ 'c', "ba"sv });
}
