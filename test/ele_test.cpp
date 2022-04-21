#include "test_include.hpp"

TEST_CASE("empty") { static_assert(prs::ele('t')("") == std::nullopt); }

TEST_CASE("wrong element") {
  static_assert(prs::ele('e')("test") == std::nullopt);
  static_assert(prs::ele('s')("test") == std::nullopt);
}

TEST_CASE("correct element") {
  static_assert(prs::ele('t')("test") == std::pair('t', "est"sv));
  static_assert(prs::ele('u')("unit") == std::pair('u', "nit"sv));
}
