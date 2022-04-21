#include "test_include.hpp"

constexpr auto add_(int a, int b) { return a + b; }
constexpr auto add_1_(int a) { return a + 1; }

constexpr auto add = prs::piped(add_);
constexpr auto add_1 = prs::piped(add_1_);


suite const pipe_suite = [] {
  "1 param missing"_test = [] { static_assert((1 | add(2)) == 3); };
  "all param there"_test = [] { static_assert(add(1, 2) == 3); };
  "0 param pipes"_test = [] { static_assert((1 | add_1()) == 2); };
  "1 argument function"_test = [] { static_assert((add_1(1)) == 2); };
};
