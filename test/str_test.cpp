#include "test_include.hpp"

TEST_CASE("empty") { static_assert(parser::str("test")("") == std::nullopt); }

TEST_CASE("matching") {
  static_assert(parser::str("test")("tester") == std::pair("test"sv, "er"sv));
  static_assert(
    parser::str("unit")("unit test") == std::pair("unit"sv, " test"sv));
}

TEST_CASE("non matching") {
  static_assert(parser::str("test")("tesla") == std::nullopt);
  static_assert(parser::str("test")("te") == std::nullopt);
  static_assert(parser::str("money")("test") == std::nullopt);
}
