#pragma once
// this is for my bro
// keep it simple

#define Car const auto &

struct head_ray;
constexpr inline auto head_ray_v = static_cast<head_ray *>(nullptr);

#define L1(a)                                                                  \
  [](Car o) {                                                                  \
    o(head_ray_v, a, nullptr);                                                 \
    (void(0));                                                                 \
  }
#define L2(a, b)                                                               \
  [](Car o) {                                                                  \
    o(head_ray_v, a, [](Car o) {                                               \
      o(head_ray_v, b, nullptr);                                               \
      (void(0));                                                               \
    });                                                                        \
  }
#define L3(a, b, c)                                                            \
  [](Car o) {                                                                  \
    o(head_ray_v, a, [](Car o) {                                               \
      o(head_ray_v, b, [](Car o) {                                             \
        o(head_ray_v, c, nullptr);                                             \
        (void(0));                                                             \
      });                                                                      \
    });                                                                        \
  }
#define L4(a, b, c, d)                                                         \
  [](Car o) {                                                                  \
    o(head_ray_v, a, [](Car o) {                                               \
      o(head_ray_v, b, [](Car o) {                                             \
        o(head_ray_v, c, [](Car o) {                                           \
          o(head_ray_v, d, nullptr);                                           \
          (void(0));                                                           \
        });                                                                    \
      });                                                                      \
    });                                                                        \
  }
#define RG(name)                                                               \
  struct name {                                                                \
    void operator()(int &o, const char *b) const { o=(RG_
#define RG_(expr) expr);                                                       \
  }                                                                            \
  }
#define RGB(name)                                                              \
  struct name {                                                                \
    void operator()(int &o, const char *b) const {                             \
      RGB_
#define RGB_(body)                                                             \
  body                                                                         \
  }                                                                            \
  }
#define v_(name)                                                               \
  struct name {                                                                \
    v_next
#define v_next(body)                                                           \
  void operator()(Car o) const { (body)(o); }
#define _v                                                                     \
  }                                                                            \
  ;
