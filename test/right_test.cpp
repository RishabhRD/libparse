#include "test_include.hpp"

constexpr auto to_dig(char c) { return c - '0'; }

suite const right = [] {
  "right"_test = [] {
    constexpr auto whitespace_parser = prs::ele(' ');
    constexpr auto dig_parser = prs::fmap(to_dig, prs::one_of("0123456789"));
    constexpr auto whitespace_after_digit = whitespace_parser < dig_parser;
    static_assert(whitespace_after_digit(" 134") == std::pair{ 1, "34"sv });
  };
};
