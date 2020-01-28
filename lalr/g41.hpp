#pragma once
#include "terminals.hpp"
#define H head_ray_v
#define T tail_ray_v
#define L [](const auto &o)
#define D void operator()(const auto &o) const
#define S(name) o(H, name);
struct expr {
  D {
    o(
        H, L { o(H, expr{}); });
    o(
        T, L {
          o(
              H, L { o(H, ID{}); });
        });
  }
  struct term {
    D {}
  };
  struct factor {
    D {}
  };
};
