#include "test_include.hpp"

constexpr auto is_same_char(char a, char b) -> bool { return a == b; }

suite const combine = [] {
  constexpr auto c_parser = prs::ele('c');
  constexpr auto d_parser = prs::ele('d');
  constexpr auto cc_parser = prs::combine(c_parser, c_parser, is_same_char);
  constexpr auto cd_parser = prs::combine(c_parser, d_parser, is_same_char);

  "first fail"_test = [=] { static_assert(cc_parser("bcd") == std::nullopt); };

  "second fail"_test = [=] { static_assert(cc_parser("cbd") == std::nullopt); };

  "both passes"_test = [=] {
    static_assert(cd_parser("cde") == std::pair{ false, "e"sv });
  };
};
