#include "test_include.hpp"

TEST_CASE("empty") { static_assert(prs::str("test")("") == std::nullopt); }

TEST_CASE("matching") {
  static_assert(prs::str("test")("tester") == std::pair("test"sv, "er"sv));
  static_assert(
    prs::str("unit")("unit test") == std::pair("unit"sv, " test"sv));
}

TEST_CASE("non matching") {
  static_assert(prs::str("test")("tesla") == std::nullopt);
  static_assert(prs::str("test")("te") == std::nullopt);
  static_assert(prs::str("money")("test") == std::nullopt);
}
