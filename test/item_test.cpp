#include "test_include.hpp"

suite const item = [] {
  "empty"_test = [=] { static_assert(prs::item()("") == std::nullopt); };

  "elements"_test = [=] {
    static_assert(
      prs::item()("test") == std::optional{ std::pair{ 't', "est"sv } });
  };
};
