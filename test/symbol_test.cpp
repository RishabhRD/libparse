#include "test_include.hpp"

TEST_CASE("empty") { static_assert(parser::symbol('t')("") == std::nullopt); }

TEST_CASE("wrong symbolment") {
  static_assert(parser::symbol('e')("test") == std::nullopt);
  static_assert(parser::symbol('s')("test") == std::nullopt);
}

TEST_CASE("correct symbolment") {
  static_assert(parser::symbol('t')("test") == std::pair('t', "est"sv));
  static_assert(parser::symbol('u')("unit") == std::pair('u', "nit"sv));
}
