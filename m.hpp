#pragma once
#define MO(...) template <typename O> void operator ()(MO_ARGS_
#define MO_ARGS_(...) __VA_ARGS__, const O &o) const noexcept
#define UFB(name)                                                              \
  template <typename A> struct name##_fn {                                     \
    A a
#define UFE(name, ...)                                                         \
  }                                                                            \
  ;                                                                            \
  constexpr inline auto name =                                                 \
      []<typename A,                                                           \
         typename = std::enable_if_t<std::conjunction_v<__VA_ARGS__>>>(A &&    \
                                                                       a) {    \
    return name##_fn<A>{std::forward<A>(a)};                                   \
  }
#define BFB(name)                                                              \
  template <typename A, typename B> struct name##_fn {                         \
    A a;                                                                       \
    B b
#define BFE(name, ...)                                                         \
  }                                                                            \
  ;                                                                            \
  constexpr inline auto name =                                                 \
      []<typename A, typename B,                                               \
         typename = std::enable_if_t<std::conjunction_v<__VA_ARGS__>>>(        \
          A && a, B && b) {                                                    \
    return name##_fn<A, B>{std::forward<A>(a), std::forward<B>(b)};            \
  }
