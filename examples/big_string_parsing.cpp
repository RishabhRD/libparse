#include <chrono>
#include <sstream>
#include <iostream>
#include <parser/parser.hpp>
#include <string_view>


/*
 * Peformance example:
 * Parses string of length 15000000 with
 * 50000000 1's seperated by " ," in almost
 * 200ms.
 */

using namespace std::string_view_literals;

constexpr auto char_to_int(char c) { return c - '0'; }

constexpr auto append_digits(int a, char b) { return a * 10 + char_to_int(b); }

constexpr auto whitespace_parser = parser::many_of(' ');

constexpr auto token(char c) {
  return whitespace_parser//
         | parser::ignore_previous(parser::symbol(c))//
         | parser::ignore(whitespace_parser);
}

constexpr auto digit_parser = parser::one_of("0123456789");

constexpr auto int_parser =
  parser::one_of("123456789")//
  | parser::then([](char c) {
      return parser::many(digit_parser, char_to_int(c), append_digits);
    });

auto main() -> int {
  std::ostringstream str;
  int n = 50000000;
  for (int i = 1; i < n; ++i) { str << '1' << ',' << ' '; }
  str << '1';
  std::cout << "creating string done...\n";

  auto start = std::chrono::high_resolution_clock::now();
  auto const val =
    parser::seperated_by(int_parser, token(','), 0, std::plus<>{})(str.str())
      .value()
      .first;
  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "Took: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                 stop - start)
                 .count()
            << "ms" << std::endl;

  std::cout << "Sum: " << val << std::endl;
}
