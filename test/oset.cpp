#include "oset.hpp"

using namespace abo;

template <typename T> using UPtr = std::unique_ptr<T, void (*)(T *)>;
struct L {
  L() = delete;
  L(const L &) = delete;
  L(L &&) = default;
  template <typename T,
            typename = std::enable_if_t<std::is_convertible_v<T, std::string>>>
  L(T &&a) : message(std::forward<T>(a)) {}
  L operator+(const L &other) { return L(this->message + other.message); }
  std::string message;
};

template <typename T, typename... Args>
constexpr auto m1(int (*c)(T **, Args...), void (*d)(T *)) {
  return [c, d](Args... args, auto o) {
    T *ptr = nullptr;
    if (int error = c(&ptr, args...))
      o(L{""});
    else
      o(UPtr<T>(ptr, d));
  };
}

template <typename T, typename... Args>
constexpr auto m2(int (*c)(T *, Args...)) {
  return [c](Args... args, auto o) {
    T v = {};
    if (int error = c(&v, args...))
      o(L{""});
    else
      o(std::move(v));
  };
}

constexpr decltype(auto) open = m1(git_repository_open, git_repository_free);
// constexpr decltype(auto) oid_fromstr = m2(git_oid_fromstr);

template <typename A, typename F>
constexpr auto operator|(A &&a, F &&f) -> decltype(f(std::forward<A>(a))) {
  return f(std::forward<A>(a));
}

#define G(T, O) m1(T##_##O, T##_free, G_NEXT_
#define G_NEXT_(...) __VA_ARGS__)

template <typename... Fns> constexpr decltype(auto) o(Fns &&... fns) {
  return overloaded{[](int err) { std::cout << "Error" << err << '\n'; },
                    std::forward<Fns>(fns)...};
};

NewType(A, int);
NewType(B, int);
NewType(O, int);

template <typename Pith, typename... Rays>
constexpr inline auto is_pith_v =
    std::is_invocable_r_v<void, Pith, abo::o<Rays...>>;

template <typename F> struct proxy_ray {
  F f;
  template <typename U, typename = decltype(f(std::declval<U>()))>
  constexpr void operator()(U &&u) const {
    f(std::forward<U>(u));
  };
};
template <typename F> proxy_ray(F)->proxy_ray<F>;

struct any_ray {
  template <typename U> constexpr void operator()(U &&) const {};
};

template <typename F> struct fmap {
  F f;
  template <typename Pith, typename = std::enable_if_t<std::is_invocable_r_v<
                               void, Pith, abo::o<abo::ray<L>, proxy_ray<F>>>>>
  constexpr auto operator()(Pith &&_pith) const {
    return [pith = std::forward<Pith>(_pith), f = this->f](auto &&o) {
      pith(abo::o{
          [&o](L &&l) { o(std::forward<decltype(l)>(l)); },
          [&f, &o](auto &&a) {
            using A = decltype(a);
            using R = decltype(f(std::forward<A>(a)));
            if constexpr (std::is_invocable_r_v<void, R,
                                                abo::o<abo::ray<L>, any_ray>>)
              f(std::forward<A>(a))(o);
            else
              o(f(std::forward<A>(a)));
          }});
    };
  }
};
template <typename F> fmap(F)->fmap<F>;

template <typename Pith, typename... T> struct variant_b {
  variant_b(Pith) {}
};

int main() {
  auto p =
      [](auto o) {
        if (false)
          o(L{"hi"});
        else
          o(1);
      } //
      | fmap{[](int) { return "hello"; }};

  p(abo::o{[](L &&) {}, [](const char *i) { std::cout << i << '\n'; }});

  git_libgit2_init();
  auto a = std::bind_front(open, ".") | fmap{[](UPtr<git_repository> &&) {
             //
             return [](auto o) {
               if (false)
                 o(L{""});
               o(11);
             };
           }};

  a(abo::o{[](L &&) {}, [](int i) { std::cout << i << '\n'; }});
}
