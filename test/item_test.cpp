#include "test_include.hpp"

TEST_CASE("empty") { static_assert(parser::any("") == std::nullopt); };

TEST_CASE("elements") {
  static_assert(
    parser::any("test") == std::optional{ std::pair{ 't', "est"sv } });
};
