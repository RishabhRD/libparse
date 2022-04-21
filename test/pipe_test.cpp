#include "test_include.hpp"

constexpr auto add_(int a, int b) { return a + b; }
constexpr auto add_1_(int a) { return a + 1; }

constexpr auto add = prs::piped(add_);
constexpr auto add_1 = prs::piped(add_1_);


TEST_CASE("1 param missing") { static_assert((1 | add(2)) == 3); }
TEST_CASE("all param there") { static_assert(add(1, 2) == 3); }
TEST_CASE("0 param pipes") { static_assert((1 | add_1()) == 2); }
TEST_CASE("1 argument function") { static_assert((add_1(1)) == 2); }
