#include "test_include.hpp"

suite const fail = [] {
  "fail"_test = [] {
    static_assert(prs::fail<int>()("string") == std::nullopt);
  };
};
