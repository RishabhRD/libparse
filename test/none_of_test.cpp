#include "test_include.hpp"

TEST_CASE("empty") { static_assert(prs::none_of("test")("") == std::nullopt); }

TEST_CASE("matching elements") {
  static_assert(prs::none_of("test")("test") == std::nullopt);
}

TEST_CASE("non matching elements") {
  static_assert(prs::none_of("user")("test") == std::pair('t', "est"sv));
}
