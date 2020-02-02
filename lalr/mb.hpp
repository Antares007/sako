#pragma once
// this is for my bro
// keep it simple

struct head_ray;
constexpr inline auto lhead = static_cast<head_ray *>(nullptr);
struct ltail {
  void operator()(const auto &) const {}
};

#define L1(a) [&](const auto &o) { o(lhead, a, ltail{}); }
#define L2(a, b)                                                               \
  [&](const auto &o) {                                                         \
    o(lhead, a, [&](const auto &o) { o(lhead, b, ltail{}); });                 \
  }
#define L3(a, b, c)                                                            \
  [&](const auto &o) {                                                         \
    o(lhead, a, [&](const auto &o) {                                           \
      o(lhead, b, [&](const auto &o) { o(lhead, c, ltail{}); });               \
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
