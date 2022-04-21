#include "test_include.hpp"

TEST_CASE("fail") {
  static_assert(parser::fail<int>()("string") == std::nullopt);
}
