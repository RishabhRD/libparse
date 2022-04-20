#include "test_include.hpp"

suite const str = [] {
  "empty"_test = [] { static_assert(prs::str("test")("") == std::nullopt); };

  "matching"_test = [] {
    static_assert(prs::str("test")("tester") == std::pair("test"sv, "er"sv));
    static_assert(
      prs::str("unit")("unit test") == std::pair("unit"sv, " test"sv));
  };

  "non matching"_test = [] {
    static_assert(prs::str("test")("tesla") == std::nullopt);
    static_assert(prs::str("test")("te") == std::nullopt);
    static_assert(prs::str("money")("test") == std::nullopt);
  };
};
