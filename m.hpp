#pragma once
#define M(...) template <typename O> constexpr void operator ()(M_ARGS_
#define M_ARGS_(...) const O &o, __VA_ARGS__) const noexcept
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
    return name##_fn<A>{static_cast<A &&>(a)};                                 \
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
    return name##_fn<A, B>{static_cast<A &&>(a), static_cast<B &&>(b)};        \
  }
#define BOP(oper, ...)                                                         \
  template <typename L, typename R,                                            \
            typename = std::enable_if_t<std::conjunction_v<__VA_ARGS__>>>      \
  constexpr auto operator oper(L &&l, R &&r)
