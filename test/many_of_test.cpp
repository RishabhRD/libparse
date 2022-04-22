#include "test_include.hpp"

constexpr auto many_c = parser::many_of('c');

TEST_CASE("more than 1") {
  static_assert(many_c("cccaa") == std::pair{ "ccc"sv, "aa"sv });
}

TEST_CASE("one") { static_assert(many_c("caa") == std::pair{ "c"sv, "aa"sv }); }

TEST_CASE("zero") { static_assert(many_c("aa") == std::pair{ ""sv, "aa"sv }); }
