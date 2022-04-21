#include "test_include.hpp"

TEST_CASE("empty") { static_assert(parser::ele('t')("") == std::nullopt); }

TEST_CASE("wrong element") {
  static_assert(parser::ele('e')("test") == std::nullopt);
  static_assert(parser::ele('s')("test") == std::nullopt);
}

TEST_CASE("correct element") {
  static_assert(parser::ele('t')("test") == std::pair('t', "est"sv));
  static_assert(parser::ele('u')("unit") == std::pair('u', "nit"sv));
}
