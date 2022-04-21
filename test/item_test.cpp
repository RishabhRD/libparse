#include "test_include.hpp"

TEST_CASE("empty") { static_assert(prs::item()("") == std::nullopt); };

TEST_CASE("elements") {
  static_assert(
    prs::item()("test") == std::optional{ std::pair{ 't', "est"sv } });
};
