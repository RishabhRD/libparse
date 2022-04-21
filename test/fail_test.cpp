#include "test_include.hpp"

TEST_CASE("fail") { static_assert(prs::fail<int>()("string") == std::nullopt); }
