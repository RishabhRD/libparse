#include "test_include.hpp"

TEST_CASE("empty") {
  static_assert(parser::one_of("test")("") == std::nullopt);
}

TEST_CASE("wrong elements") {
  static_assert(parser::one_of("test")("unit") == std::nullopt);
  static_assert(parser::one_of("abc")("test") == std::nullopt);
}

TEST_CASE("matching elements") {
  static_assert(parser::one_of("test")("sample") == std::pair('s', "ample"sv));
  static_assert(parser::one_of("unit")("test") == std::pair('t', "est"sv));
}
