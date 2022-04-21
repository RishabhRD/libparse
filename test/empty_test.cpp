#include "test_include.hpp"

TEST_CASE("empty") {
  static_assert(parser::empty<int>("string") == std::nullopt);
}
