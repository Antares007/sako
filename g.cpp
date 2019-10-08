#include <git2.h>
#include <memory>

namespace abo {
template <class... O> struct o : O... { using O::operator()...; };
template <class... O> o(O...)->o<O...>;
template <typename... Ts> struct print;
} // namespace abo

namespace abo::git {

template <typename... Args> struct lift;
template <typename T, typename... Args> struct lift<T *, Args...> {
  int (*ctor)(T **, Args...);
  void (*dtor)(T *);
  template <typename O, typename = std::enable_if_t<std::conjunction_v<
                            std::is_invocable_r<void, O, int>,
                            std::is_invocable_r<void, O, T *>>>>
  constexpr void operator()(const O &o, Args &&... args) const {
    T *ptr = nullptr;
    if (int error = ctor(&ptr, std::move(args)...))
      o(error);
    else {
      o(ptr);
      dtor(ptr);
    }
  }
};
template <typename T, typename... Args> struct lift<T, Args...> {
  int (*f)(T *, Args...);
  template <
      typename O,
      typename = std::enable_if_t<std::conjunction_v<
          std::is_invocable_r<void, O, int>, std::is_invocable_r<void, O, T>>>>
  constexpr void operator()(const O &o, Args... args) const {
    T t;
    if (int error = f(&t, std::move(args)...))
      o(error);
    else {
      o(std::move(t));
    }
  }
};
template <typename T, typename... Args>
lift(int (*)(T **, Args...), void (*)(T *))->lift<T *, Args...>;
template <typename T, typename... Args>
lift(int (*)(T *, Args...))->lift<T, Args...>;

template <typename Pith> struct bark {
  Pith pith;
  template <typename U,
            typename = std::enable_if_t<std::is_invocable_r_v<void, U, int>>>
  bark(U &&) {}

  template <typename O,
            typename = std::enable_if_t<std::is_invocable_r_v<void, O, int>>>
  constexpr void operator()(git_repository *, O &&) const {}
};
template <typename Pith> bark(Pith)->bark<Pith>;
} // namespace abo::git

int main() {
  git_libgit2_init();

  auto a = abo::git::lift{git_repository_open, git_repository_free};
  a(abo::o{[](int) {},
           [](git_repository *) {
             //
           }},
    "");

  auto c = abo::git::lift{git_oid_fromstr};
  c(abo::o{[](int) {}, [](git_oid) {}}, "");

  return 3;
}
