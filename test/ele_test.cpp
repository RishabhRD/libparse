#include "test_include.hpp"

suite const ele = [] {
  "empty"_test = [] { static_assert(prs::ele('t')("") == std::nullopt); };

  "wrong element"_test = [] {
    static_assert(prs::ele('e')("test") == std::nullopt);
    static_assert(prs::ele('s')("test") == std::nullopt);
  };

  "correct element"_test = [] {
    static_assert(prs::ele('t')("test") == std::pair('t', "est"sv));
    static_assert(prs::ele('u')("unit") == std::pair('u', "nit"sv));
  };
};
