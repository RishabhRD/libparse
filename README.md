# libparse

A **single header** compile time functional string parsing libary in C++.
It provides a set of builtin parsers and parser combinators.
Parser combinators utilizes well known functional programming patterns.
You can chain many parsers using parser combinators to define custom parsers you need.

All you need is C++20!

## Usage

```cpp
#include <parser/parser.hpp>

constexpr auto to_digit(char c) { return c - '0'; }

constexpr auto digit_parser = parser::one_of("0123456789")//
                              | parser::transform(to_digit);
constexpr auto plus_token_parser = parser::str(" + ");


constexpr auto three_dig_sum = digit_parser
                               | parser::ignore(plus_token_parser)
                               | parser::combine_with(digit_parser, std::plus<>{})
                               | parser::ignore(plus_token_parser)
                               | parser::combine_with(digit_parser, std::plus<>{});

int main(){
  static_assert(three_dig_sum("1 + 2 + 3")->first == 6);
}
```

# Documentation

A parser of thing is a function that takes string_view and optionally returns
the thing and rest of string to parse.
Parser parses 0 or more length prefix of string_view.

```cpp
using parsed_t<T> = std::optional<std::pair<T, std::string_view>>;
```

```haskell
ParserOf<T> :: string_view -> parsed_t<T>
```

If parser returns std::nullopt, that means parser was not able to parse the string.

If a parser parses successfully, it consumes 0 or more characters from starting
of string_view. Somehow, transforms that to T and returns T and the remaining
characters of string_view to parse.

A Parser combinator accepts one or more parsers and return a new parser.
Every Parser combinator is both infix callable with pipes (pipeable) and
callable with normal function call syntax with all arguments provided.

    Prefix: F(a, b)
    Infix: a | F(b)

This makes a parser to look like a pipeline of parsers.

**NOTE**: For more examples please look into project's test and examples directory.
There is a test file for every parser and parser combinator. That test
file well demostrates the functionality of that parser in code.

## parser::any

```cpp
any :: string_view -> parsed_t<char>
```

This parser parses first character of string_view and null if empty.

```cpp
static_assert(parser::any("test") == std::pair{ 't', "est"sv  });
```

## parser::symbol

```cpp
symbol = (char c) -> (string_view -> parsed_t<char>)
```

This parser parses the first character of string_view if first character is c.

```cpp
static_assert(parser::symbol('t')("test") == std::pair{ 't', "est"sv  });
```

## parser::one_of

```cpp
one_of = (string_view s) -> (string_view -> parsed_t<char>)
```

This parser parses the first character of string_view is contained in s.

```cpp
static_assert(parser::one_of("tuv")("test") == std::pair{ 't', "est"sv  });
```

## parser::none_of

```cpp
none_of = (string_view s) -> ParserOf<Char>
```

This parser parses the first character of string_view is not contained in s.

```cpp
static_assert(parser::one_of("uv")("test") == std::pair{ 't', "est"sv  });
```

## parser::str

```cpp
str = (string_view s) -> ParserOf<string_view>
```

This parser parses if the string starts with s

```cpp
static_assert(parser::one_of("te")("test") == std::pair{ "te"sv, "st"sv  });
```

## parser::empty

```cpp
empty<T> = (string_view s) -> parsed_t<T>
```

This parsers just returns null, and consumes nothing.

```cpp
static_assert(parser::empty<char>("str") == std::nulopt);
```

## parser::always

```cpp
always = (T t) -> (string_view -> parsed_t<T>)
```

This parser always returns t and having the input string unconsumed

```cpp
static_assert(parser::always('c')("str") == std::pair{'c', "str"sv});
```

## parser::transform

```cpp
transform = (ParserOf<T1> p, F :: (T1 -> T2)) -> ParserOf<T2>
```

This parser combinator takes a parser of type T1 and a function from (T1 to T2) and
returns a parser of type T2. This is fmap equivalent of functional programming.
When parser succeds it returns result applying F otherwise returns null.

```cpp
constexpr auto to_digit(char c){
  return c - '0';
}

constexpr auto dig_9 = parser::symbol('9')
                       | parser::transform(to_dig);

static_assert(dig_9("9str") == std::pair{9, "str"sv});
```

## parser::or_with

```cpp
or_with = (ParserOf<T> p1, ParserOf<T> p2) -> ParserOf<T>
```

This parser combinator takes 2 parsers of types T and returns a parser of Type T.
It first tries to parse string with p1 if that succeds return result otherwise tries to parse with p2
if that succeeds returns result otherwise null.

```cpp
constexpr auto any_char = parser::empty<char>
                          | parser::or_with(parser::any);

static_assert(any_char("9str") == std::pair{'9', "str"sv});
```

## parser::combine_with

```cpp
or_with = (ParserOf<T1> p1, ParserOf<T2> p2, F f) -> ParserOf<F(T1, T2)>
```

This parser combinator first parses string with p1 and then rest of string with p2 and
then returns the answer by applying f on it.
If at any step any parser fails, it return null.

```cpp
constexpr auto append_dig(int a, int b){
  return a * 10 + b;
}

constexpr auto dig = parser::one_of("0123456789");
constexpr auto two_dig = dig
                         | parser::combine_with(dig, append_dig);

static_assert(two_dig("92str") == std::pair{92, "str"sv});
```

## parser::ignore_previous / parser::snd

```cpp
ignore_previous = (ParserOf<T1> p1, ParserOf<T2> p2) -> ParserOf<T2>
```

This parser combinator first parses string with p1 and then rest of string with
p2 and then return the result of p2 ignoring result of p1.

```cpp
constexpr auto to_digit(char c){
  return c - '0';
}

constexpr auto whitespace = parser::many_of(' ');
constexpr auto dig_parser = parser::one_of("0123456789")

constexpr auto dig_after_whitespace = whitespace
                                      | parser::ignore_previous(dig_parser)

static_assert(dig_after_whitespace("   9a" == std::pair{9, "a"sv});
static_assert(parser::snd(whitespace, dig_parser)("   9a" == std::pair{9, "a"sv});
```

## parser::ignore / parser::fst

```cpp
ignore = (ParserOf<T1> p1, ParserOf<T2> p2) -> ParserOf<T1>
```

This parser combinator first parses string with p1 and then rest of string with
p2 and then return the result of p1 ignoring result of p2.

```cpp
constexpr auto to_digit(char c){
  return c - '0';
}

constexpr auto whitespace = parser::many_of(' ');
constexpr auto dig_parser = parser::one_of("0123456789")

constexpr auto whitspace_after_dig = dig_parser
                                     | parser::ignore(dig_parser)

static_assert(whitspace_after_dig("9    a" == std::pair{9, "a"sv});
static_assert(parser::fst(dig_parser, whitespace)("   9a" == std::pair{9, "a"sv});
```

## parser::if_satisfies

```cpp
if_satisfies = (ParserOf<T1> p1, Predicate p) -> ParserOf<T1>
```

This parser combinator first parses string with p1. If p1 fails then returns
null. Otherwise pass result to p. If result satisfies p then return result
otherwise return null.

```cpp
constexpr auto is_c_or_d(char c){
  return c == 'c' or c == 'd';
}

constexpr auto c_or_d = parser::any
                        | parser::if_satisfies(is_c_or_d);

static_assert(c_or_d("cat" == std::pair{'c', "at"sv});
```

## parser::if_char_satisfies

```cpp
if_char_satisfies = (Predicate p) -> ParserOf<T1>
```

This parser parses a character from string_view. If that satisfies p return character otherwise null.

```cpp
constexpr auto is_c_or_d(char c){
  return c == 'c' or c == 'd';
}

constexpr auto c_or_d = parser::if_char_satisfies(is_c_or_d);

static_assert(c_or_d("cat" == std::pair{'c', "at"sv});
```

## parser::then

```cpp
then = (ParserOf<T> p, F f) -> ParserOf<F(T)>
F = T -> ParserOf<T1>
```

This parser combinator takes a parser and a function.
It parses the string with p, if that fails returns null.
Otherwise pass the result to f, and parses the rest of string
with parser returned by invoking f.

f is required to return a parser.

This is a monadic bind equivalent in functional programming.

```cpp
constexpr auto char_to_int(char c) { return c - '0'; }

constexpr auto int_parser =
  parser::one_of("123456789")
  | parser::then([](char c) {
      return parser::many(parser::one_of("0123456789"), char_to_int(c), append_digits);
    });

static_assert(int_parser("123abc") == std::pair{123, "abc"sv});
```

## parser::many

```cpp
many = (ParserOf<T1> p1, T2 init, F<T2, T1> f) -> Parser<T2>
F<T1, T2> = (T1, T2) -> T1
```

This accumulating parser combinator parses the string 0 or more times with p1.
It accumulates the result with accumulation function f with the initial value
init. After the first failure it returns the accumulated value and remaining string.

```cpp
constexpr auto concat_digits(int a, char b) -> int {
  return a = a * 10 + (b - '0');
}

constexpr auto digit_parser = parser::one_of("0123456789");
constexpr auto int_parser = digit_parser
                            | parser::many(0, concat_digits);

static_assert(int_parser("123abc") == std::pair{ 123, "abc"sv });
static_assert(int_parser("abc") == std::pair{ 0, "abc"sv });
```

## parser::many1

```cpp
many1 = (ParserOf<T1> p1, T2 init, F<T2, T1> f) -> Parser<T2>
F<T1, T2> = (T1, T2) -> T1
```

This accumulating parser combinator parses the string 1 or more times with p1.
It accumulates the result with accumulation function f with the initial value
init. After the first failure it returns the accumulated value and remaining string.
If parser fails for the first time parsing with p1 itself, then it returns a null.

```cpp
constexpr auto concat_digits(int a, char b) -> int {
  return a = a * 10 + (b - '0');
}

constexpr auto digit_parser = parser::one_of("0123456789");
constexpr auto int_parser = digit_parser
                            | parser::many(0, concat_digits);

static_assert(int_parser("123abc") == std::pair{ 123, "abc"sv });
static_assert(int_parser("abc") == std::nullopt);
```

## parser::exactly_n

```cpp
exactly_n = (ParserOf<T1> p1, T2 init, F<T2, T1> f, size_t n) -> Parser<T2>
F<T1, T2> = (T1, T2) -> T1
```

This accumulating parser combinator parses the string n times with p1.
It accumulates the result with accumulation function f with the initial value
init. If it can't parse the string n times with p1, then returns null otherwise the
accumulated value.

```cpp
constexpr auto concat_digits(int a, char b) -> int {
  return a = a * 10 + (b - '0');
}

constexpr auto digit_parser = parser::one_of("0123456789");
constexpr auto three_dig_int = digit_parser
                               | parser::exactly_n(0, concat_digits, 3);

static_assert(int_parser("1234abc") == std::pair{ 123, "4abc"sv });
static_assert(int_parser("12abc") == std::nullopt);
```

## parser::seperated_by

```cpp
seperated_by = (ParserOf<T1> p1, ParserOf<T> p2, T2 init, F<T2, T1> f) -> Parser<T2>
F<T1, T2> = (T1, T2) -> T1
```

This accumulating parser combinator parses the string in which, 1 or more
values satisfying p1 is seperated by values that satisfy p2. It accumulates all
the values with accumulation function f.

```cpp
constexpr auto to_digit(char c){
  return c - '0';
}

constexpr auto digit_parser = parser::one_of("0123456789")
                              | parse::transform(to_digit);

constexpr auto sum_dig = digit_parser
                          | parser::seperated_by(parser::symbol('+'), 0, std::plus<>{});

static_assert(sum_dig("1+2+3a") == std::pair{ 6, "a"sv });
```

## parser::many_of

```cpp
 many_of = (char c) -> Parser<string_view>
```

This accumulating parser extracts 0 or more adjacent c in starting of given string_view.
If the given string_view doesn't start with c, it returns empty string_view as result
and not consuming any of given string.

```cpp
static_assert(many_of('c')("ccabc") == std::pair{ "cc"sv, "abc"sv });
static_assert(many_of('c')("abc") == std::pair{ ""sv, "abc"sv });
```

## parser::many_if

```cpp
 many_of = (Predicate p) -> Parser<string_view>
```

This accumulating parser extracts 0 or more adjacent characters in starting of
given string_view that satisfies p. If the given string_view doesn't start with
any such characters, it returns empty string_view as result and not consuming any of given
string.

```cpp
constexpr auto is_c(char c){
  return c == 'c';
}

static_assert(many_if(is_c)("ccabc") == std::pair{ "cc"sv, "abc"sv });
static_assert(many_if(is_c)("abc") == std::pair{ ""sv, "abc"sv });
```

## parser::many1_of

```cpp
 many1_of = (char c) -> Parser<string_view>
```

This accumulating parser extracts 1 or more adjacent c in starting of given string_view.
If the given string_view doesn't start with c, it return null.

```cpp
static_assert(many1_of('c')("ccabc") == std::pair{ "cc"sv, "abc"sv });
static_assert(many1_of('c')("abc") == std::nullopt);
```

## parser::many1_if

```cpp
 many1_if = (Predicate p) -> Parser<string_view>
```

This accumulating parser extracts 1 or more adjacent characters in starting of
given string_view that satisfies p. If the given string_view doesn't start with
any such characters, it returns null.

```cpp
constexpr auto is_c(char c){
  return c == 'c';
}

static_assert(many1_if(is_c)("ccabc") == std::pair{ "cc"sv, "abc"sv });
static_assert(many1_if(is_c)("abc") == std::nullopt);
```

## parser::many1_if

```cpp
 sequence = (F f, Parser... p) -> Parser<string_view>
```

This is an applicative parser. That takes a function that combines the result
and many parsers that would be executed sequentially. The resut of those
parsers would be passed to the passed function.

```cpp
constexpr auto combine_digits(int a, int b){
  return a * 10 + b;
}

static_assert(sequence(combine_digits, digit_parser, digit_parser)("12")->first == nullopt);
```

## What's next

checkout [cycling_parsing.cpp](examples/cyclic_parsing.cpp) to understand
how to parse a grammar with libparse and potentially if that grammer contains
cyclic dependencies.

## TODO

-   Improve documentation
-   Package for conan
-   Package for vcpkg

## Credits

-   David Sankel's talk: [Monoids, Monads and Applicative Functors](https://youtu.be/giWCdQ7fnQU)
-   Ben Deane's talk: [constexpr all the things](https://youtu.be/PJwd4JLYJJY)
-   Peter Holmberg's talk: [Functional Parsing in C++20](https://youtu.be/5iXKLwoqbyw)
-   Prabhdeep Singh (@prabhdepSP) : [big_string_parsing.cpp](examples/big_string_parsing.cpp)
