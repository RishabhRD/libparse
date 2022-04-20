#include "test_include.hpp"

suite const operator_or = [] {
  constexpr auto a_parser = prs::ele('a');
  constexpr auto b_parser = prs::ele('b');
  constexpr auto c_parser = prs::ele('c');
  constexpr auto a_a_parser = prs::ele('a') | prs::ele('a');
  constexpr auto ab_parser = a_parser | b_parser;
  constexpr auto abc_parsr = a_parser | b_parser | c_parser;

  "both_fails"_test = [=] { static_assert(ab_parser("def") == std::nullopt); };

  "first_fails"_test = [=] {
    static_assert(ab_parser("bcd") == std::pair{ 'b', "cd"sv });
  };

  "second fails"_test = [=] {
    static_assert(ab_parser("abc") == std::pair{ 'a', "bc"sv });
  };

  "both passes"_test = [=] {
    static_assert(a_a_parser("abc") == std::pair{ 'a', "bc"sv });
  };

  "three parsers"_test = [=] {
    static_assert(abc_parsr("cba") == std::pair{ 'c', "ba"sv });
  };
};
