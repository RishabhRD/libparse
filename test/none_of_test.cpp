#include "test_include.hpp"

suite const none_of = [] {
  "empty"_test = [] {
    static_assert(prs::none_of("test")("") == std::nullopt);
  };

  "matching elements"_test = [] {
    static_assert(prs::none_of("test")("test") == std::nullopt);
  };

  "non matching elements"_test = [] {
    static_assert(prs::none_of("user")("test") == std::pair('t', "est"sv));
  };
};
