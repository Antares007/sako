#include "overloaded.hpp"
#include <functional>
#include <git2.h>
#include <git2/oid.h>
#include <iostream>
#include <memory>
#include <newtype.hpp>
#include <tuple>
#include <type_traits>
#include <union.hpp>

using namespace abo;

template <typename T> using UPtr = std::unique_ptr<T, void (*)(T *)>;

template <typename T, typename... Args>
constexpr auto m1(int (*c)(T **, Args...), void (*d)(T *)) {
  return [c, d](Args... args, auto o) {
    T *ptr = nullptr;
    if (int error = c(&ptr, args...))
      o(error);
    else
      o(UPtr<T>(ptr, d));
  };
}

template <typename T, typename... Args>
constexpr auto m2(int (*c)(T *, Args...)) {
  return [c](Args... args, auto o) {
    T v = {};
    if (int error = c(&v, args...))
      o(error);
    else
      o(std::move(v));
  };
}

constexpr decltype(auto) open = m1(git_repository_open, git_repository_free);
constexpr decltype(auto) oid_fromstr = m2(git_oid_fromstr);

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

NewType(A, int);
NewType(B, int);
NewType(O, int);

int main() {
  auto o = ray<A, B, O>();
  o(A{0});
  o(B{0});
  o(O{0});
  git_libgit2_init();

  (([](auto o) {
     o(A{1});
     o(O{2});
   }) *
   ([](auto o) {
     o(B{3});
     o(O{4});
   }))([](auto &&a, auto &&b) {
    std::cout << a.get() << " - " << b.get() << '\n';
  });

  // std::bind_front(open, ".")
  auto u1 = [](auto o) { o(A{1}); };
  auto u2 = [](auto o) {
    o(B{0});
    o(git_oid{});
  };
  auto u3 = u1 * u2;
  u3(overloaded{
      [](A, B) {},
      [](A, git_oid &&) {},
  });
}
