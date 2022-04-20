#include "test_include.hpp"

suite const one_of = [] {
  "empty"_test = [] { static_assert(prs::one_of("test")("") == std::nullopt); };

  "wrong elements"_test = [] {
    static_assert(prs::one_of("test")("unit") == std::nullopt);
    static_assert(prs::one_of("abc")("test") == std::nullopt);
  };

  "matching elements"_test = [] {
    static_assert(prs::one_of("test")("sample") == std::pair('s', "ample"sv));
    static_assert(prs::one_of("unit")("test") == std::pair('t', "est"sv));
  };
};
