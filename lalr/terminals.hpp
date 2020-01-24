#pragma once
#include "../parsec.hpp"

struct PLUS {
  template <typename O> void operator()(const O &o, const char *b) const {
    if (b[0] == '+' && b[1] != '=' && b[1] != '+')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct MUL {
  template <typename O> void operator()(const O &o, const char *b) const {
    if (b[0] == '*' && b[1] != '=')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct LPAREN {
  template <typename O> void operator()(const O &o, const char *b) const {
    if (b[0] == '(')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct RPAREN {
  template <typename O> void operator()(const O &o, const char *b) const {
    if (b[0] == ')')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct ID {
  template <typename O> void operator()(const O &o, const char *b) const {
    if ('a' <= b[0] && b[0] <= 'z')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
