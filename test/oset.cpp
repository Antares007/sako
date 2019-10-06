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

template <typename Pith, typename... Rights>
using is_pith_for_t =
    std::enable_if_t<std::is_invocable_r_v<void, Pith, ray<Rights...>>, Pith>;

template <typename F> struct lrrayF : ray<L> {
  F f;
  using ray<L>::operator();
  template <typename U, typename = decltype(f(std::declval<U>()))>
  constexpr void operator()(U &&) const {};
};
template <typename F> lrrayF(F)->lrrayF<F>;

template <typename F> struct fmap {
  F f;

  template <typename Pith, typename = std::enable_if_t<
                               std::is_invocable_r_v<void, Pith, lrrayF<F>>>>
  constexpr decltype(auto) operator()(Pith &&pith) const {
    return [p = std::forward<Pith>(pith), f = std::move(this->f)](auto &&o) {
      p(overloaded{[&o](L l) { o(std::move(l)); },
                   [&f, &o](auto &&a) { o(f(std::forward<decltype(a)>(a))); }});
    };
  }
};
template <typename F> fmap(F)->fmap<F>;

int main() {
  ///
  auto o2 = abo::o(ray<A, B>{}, [](O) { std::cout << "hi"; });

  o2(O{0});

  git_libgit2_init();

  auto a = std::bind_front(open, ".") | fmap{[](UPtr<git_repository> &&) {
             //
             return 11;
           }};

  a(overloaded{[](L &&l) { std::cout << l.message << '\n'; },
               [](int i) { std::cout << i << '\n'; }});
}
