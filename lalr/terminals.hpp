#pragma once
#include "../parsec.hpp"
#define Regular                                                                \
  template <typename O> void operator()(const O &o, const char *b) const
struct _PLUS_ {
  Regular {
    if (b[0] == '+' && b[1] != '=' && b[1] != '+')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct _MUL_ {
  Regular {
    if (b[0] == '*' && b[1] != '=')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct _LPAREN_ {
  Regular {
    if (b[0] == '(')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct _RPAREN_ {
  Regular {
    if (b[0] == ')')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct _ID_ {
  Regular {
    if ('a' <= b[0] && b[0] <= 'z')
      o(1);
    else
      o(error_ray_v, -1);
  }
};
struct _EPSILON_ {
  Regular {
    o(0);
    (void(b));
  }
};
