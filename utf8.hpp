#pragma once
#include <bits/stdint-uintn.h>

namespace utf8 {
inline auto codepoint(const char *s) -> uint32_t {
  if (0xf0 == (0xf8 & s[0]))
    return ((0x07 & s[0]) << 18) | ((0x3f & s[1]) << 12) |
           ((0x3f & s[2]) << 6) | (0x3f & s[3]);
  if (0xe0 == (0xf0 & s[0]))
    return ((0x0f & s[0]) << 12) | ((0x3f & s[1]) << 6) | (0x3f & s[2]);
  if (0xc0 == (0xe0 & s[0]))
    return ((0x1f & s[0]) << 6) | (0x3f & s[1]);
  return s[0];
}
inline auto next(const char *s) -> const char * {
  if (0xf0 == (0xf8 & s[0]))
    return s + 4;
  if (0xe0 == (0xf0 & s[0]))
    return s + 3;
  if (0xc0 == (0xe0 & s[0]))
    return s + 2;
  return s + 1;
}
} // namespace utf8
