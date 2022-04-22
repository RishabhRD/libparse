# libparse

A **single header** compile time functional string parsing libary in C++.
It provides a set of builtin parsers and parser combinators.
Parser combinators utilizes well known functional programming patterns.
We can chain many parsers using parser combinators to define custom parsers you need.

All you need is C++20!

## Usage

```cpp
#include <parser/parser.hpp>

constexpr auto to_digit(char c) { return c - '0'; }

constexpr auto digit_parser = parser::one_of("01234567889")//
                              | parser::transform(to_digit);
constexpr auto plus_token_parser = parser::str(" + ");


constexpr auto three_dig_sum = digit_parser
                               | parser::ignore(plus_token_parser)
                               | parser::then_with(digit_parser, std::plus<>{})
                               | parser::ignore(plus_token_parser)
                               | parser::then_with(digit_parser, std::plus<>{});

int main(){
  static_assert(three_dig_sum("1 + 2 + 3")->first == 6);
}
```

## Documentation

**NOTE**: For examples please look into project's test files and examples directory.
There is a test file for every parser and parser combinator. That test
file well demostrates the functionality of that parser in code.

```cpp
using parsed_t<T> = std::optional<std::pair<T, std::string_view>>;
```

A `ParserOf\<T\>` is an invocable that accepts a `string_view` and returns a
`parsed_t<T>`.

If parser returns null, that means parser was not able to parse starting of
string with it. Otherwise, it returns a pair that contain the result of type T
and a string_view that is rest of string to parse.

If a parser parses successfully, it consumes 0 or more characters from starting
of string_view. Somehow, transforms that to T and returns the remaining
characters.

```haskell
ParserOf<T> :: string_view -> parsed_t<T>
```

If parser fails to parse, it returns a std::nullopt.

A Parser combinator accepts one or more parsers and return a new parser.
Every Parser combinator is both infix callable with pipes (pipeable) and
callable with normal function call syntax with all arguments provided.

    Prefix: F(a, b)
    Infix: a | F(b)

This makes a parser to look like a pipeline of parsers.

### parser::any

```cpp
any :: string_view -> parsed_t<char>
```

This parser parses first character of string_view and null if empty.

```cpp
static_assert(parser::any("test") == std::pair{ 't', "est"sv  });
```

### parser::symbol

```cpp
symbol = (char c) -> (string_view -> parsed_t<char>)
```

This parser parses the first character of string_view if first character is c.

### parser::one_of

```cpp
one_of = (string_view s) -> (string_view -> parsed_t<char>)
```

This parser parses the first character of string_view is contained in s.

### parser::none_of

```cpp
none_of = (string_view s) -> ParserOf<Char>
```

This parser parses the first character of string_view is not contained in s.

### parser::str

```cpp
str = (string_view s) -> ParserOf<string_view>
```

This parser parses if the string starts with s

### parser::empty

```cpp
empty<T> = (string_view s) -> parsed_t<T>
```

This parsers just returns null.

### parser::always

```cpp
always = (T t) -> (string_view -> parsed_t<T>)
```

This parser always returns t and having the input string unconsumed

### parser::transform

```cpp
transform = (ParserOf<T1> p, F :: (T1 -> T2)) -> ParserOf<T2>
```

This parser combinator takes a parser of type T1 and a function from (T1 to T2) and
returns a parser of type T2. This is fmap equivalent of functional programming.
When parser succeds it returns result applying F otherwise returns null.

### parser::or_with

```cpp
or_with = (ParserOf<T> p1, ParserOf<T> p2) -> ParserOf<T>
```

This parser combinator takes 2 parsers of types T and returns a parser of Type T.
It first tries to parse string with p1 if that succeds return result otherwise tries to parse with p2
if that succeeds returns result otherwise null.

### parser::combine_with

```cpp
or_with = (ParserOf<T1> p1, ParserOf<T2> p2, F f) -> ParserOf<F(T1, T2)>
```

This parser combinator first parses string with p1 and then rest of string with p2 and
then returns the answer by applying f on it.
If at any step any parser fails, it return null.

### parser::ignore_previous / parser::snd

```cpp
ignore_previous = (ParserOf<T1> p1, ParserOf<T2> p2) -> ParserOf<T2>
```

This parser combinator first parses string with p1 and then rest of string with
p2 and then return the result of p2 ignoring result of p1.

### parser::ignore / parser::fst

```cpp
ignore = (ParserOf<T1> p1, ParserOf<T2> p2) -> ParserOf<T1>
```

This parser combinator first parses string with p1 and then rest of string with
p2 and then return the result of p1 ignoring result of p2.

### parser::if_satisfies

```cpp
if_satisfies = (ParserOf<T1> p1, Predicate p) -> ParserOf<T1>
```

This parser combinator first parses string with p1. If p1 fails then returns
null. Otherwise pass result to p. If result satisfies p then return result
otherwise return null.

### parser::if_char_satisfies

```cpp
if_char_satisfies = (Predicate p) -> ParserOf<T1>
```

This parser parses a character from string_view. If that satisfies p return character otherwise null.

### parser::then

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

### parser::many

```cpp
many = (ParserOf<T1> p1, T2 init, F<T2, T1> f) -> Parser<T2>
F<T1, T2> = (T1, T2) -> T1
```

This accumulating parser combinator parses the string 0 or more times with p1.
It accumulates the result with accumulation function f with the initial value
init. After the first failure it returns the accumulated value and remaining string.

### parser::many1

```cpp
many1 = (ParserOf<T1> p1, T2 init, F<T2, T1> f) -> Parser<T2>
F<T1, T2> = (T1, T2) -> T1
```

This accumulating parser combinator parses the string 1 or more times with p1.
It accumulates the result with accumulation function f with the initial value
init. After the first failure it returns the accumulated value and remaining string.
If parser fails for the first time parsing with p1 itself, then it returns a null.

### parser::exactly_n

```cpp
exactly_n = (ParserOf<T1> p1, T2 init, F<T2, T1> f, size_t n) -> Parser<T2>
F<T1, T2> = (T1, T2) -> T1
```

This accumulating parser combinator parses the string n times with p1.
It accumulates the result with accumulation function f with the initial value
init. If it can't parse the string n times with p1, then returns null otherwise the
accumulated value.

### parser::seperated_by

```cpp
seperated_by = (ParserOf<T1> p1, ParserOf<T> p2, T2 init, F<T2, T1> f) -> Parser<T2>
F<T1, T2> = (T1, T2) -> T1
```

This accumulating parser combinator parses the string in which, 1 or more
values satisfying p1 is seperated by values that satisfy p2. It accumulates all
the values with accumulation function f.

### parser::many_of

```cpp
 many_of = (char c) -> Parser<string_view>
```

This accumulating parser extracts 0 or more adjacent c in starting of given string_view.
If the given string_view doesn't start with c, it returns empty string_view as result
and not consuming any of given string.

### parser::many_if

```cpp
 many_of = (Predicate p) -> Parser<string_view>
```

This accumulating parser extracts 0 or more adjacent characters in starting of
given string_view that satisfies p. If the given string_view doesn't start with
any such characters, it returns empty string_view as result and not consuming any of given
string.

### parser::many1_of

```cpp
 many_of = (char c) -> Parser<string_view>
```

This accumulating parser extracts 1 or more adjacent c in starting of given string_view.
If the given string_view doesn't start with c, it return null.

### parser::many_if

```cpp
 many_of = (Predicate p) -> Parser<string_view>
```

This accumulating parser extracts 1 or more adjacent characters in starting of
given string_view that satisfies p. If the given string_view doesn't start with
any such characters, it returns null.

## TODO

-   Improve documentation
-   Package for conan
-   Package for vcpkg

## Credits

-   David Sankel's talk: [Monoids, Monads and Applicative Functors](https://youtu.be/giWCdQ7fnQU)
-   Ben Deane's talk: [constexpr all the things](https://youtu.be/PJwd4JLYJJY)
-   Peter Holmberg's talk: [Functional Parsing in C++20](https://youtu.be/5iXKLwoqbyw)
