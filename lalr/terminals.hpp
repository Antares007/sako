#pragma once
#include "../parsec.hpp"
#define Derives template <typename O> void operator()(O o) const

constexpr inline auto PLUS = parsec::str{"+"};
constexpr inline auto MUL = parsec::str{"*"};
constexpr inline auto LPAREN = parsec::str{"("};
constexpr inline auto RPAREN = parsec::str{")"};
constexpr inline auto ID =
    parsec::chr{[](auto c) { return 'a' <= c && c <= 'z'; }};
constexpr inline auto Є = [](auto o, const char *) { o(0); };
