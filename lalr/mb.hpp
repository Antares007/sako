#pragma once
// this is for my bro
// keep it simple

struct head_ray;
struct tail_ray;

constexpr inline auto lhead = static_cast<head_ray *>(nullptr);
constexpr inline auto ltail = static_cast<tail_ray *>(nullptr);

#define L1(a) [&](const auto &o) { o(lhead, a, 0, 1); }
#define L2(a, b)                                                               \
  [&](const auto &o) {                                                         \
    o(lhead, a, 0, 1);                                                         \
    o(ltail, [&](const auto &o) { o(lhead, b, 1, 2); });                       \
  }
#define L3(a, b, c)                                                            \
  [&](const auto &o) {                                                         \
    o(lhead, a, 0, 3);                                                         \
    o(ltail, [&](const auto &o) {                                              \
      o(lhead, b, 1, 3);                                                       \
      o(ltail, [&](const auto &o) { o(lhead, c, 2, 3); });                     \
    });                                                                        \
  }
#define LRec(b)                                                                \
  [&](const auto &o, tail_ray *, const auto &t) {                              \
    if (b)                                                                     \
      t(o);                                                                    \
  }
#define RG(name)                                                               \
  struct name {                                                                \
    void operator()(const auto &o, const char *b) const { o(RG_
#define RG_(expr) expr);                                                       \
  }                                                                            \
  }
#define RGB(name)                                                              \
  struct name {                                                                \
    void operator()(const auto &o, const char *b) const {                      \
      RGB_
#define RGB_(body)                                                             \
  body                                                                         \
  }                                                                            \
  }
#define v_(name)                                                               \
  struct name {                                                                \
    v_next
#define v_next(body)                                                           \
  void operator()(const auto &o) const { (body)(o); }
#define _v                                                                     \
  }                                                                            \
  ;
