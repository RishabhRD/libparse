#include "test_include.hpp"

TEST_CASE("empty") {
  static_assert(parser::always(2)("") == std::pair{ 2, ""sv });
}

TEST_CASE("some string") {
  static_assert(parser::always(2)("test") == std::pair{ 2, "test"sv });
}
