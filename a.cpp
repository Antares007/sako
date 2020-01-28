#include <type_traits>
struct head_ray;
struct tail_ray;

constexpr inline auto head_ray_v = static_cast<head_ray *>(nullptr);
constexpr inline auto tail_ray_v = static_cast<tail_ray *>(nullptr);

struct list_pith {
  template <typename H> void operator()(head_ray *, H &&) const {}
  template <typename T> void operator()(tail_ray *, T &&tail) const {
    tail(*this);
  }
};
constexpr inline auto nil = [](const auto &) {};

constexpr inline auto sample_list = [](auto o) {
  o(head_ray_v, 3);
  o(tail_ray_v, [](auto o) {
    o(head_ray_v, 6);
    o(tail_ray_v, [](auto o) { o(head_ray_v, 9); });
  });
};
template <typename...> struct list;
template <typename... Args> list(Args...) -> list<Args...>;
template <typename H> struct list<H> {
  H h;
  template <typename U> list(U &&u) : h(static_cast<U &&>(u)) {}
  template <typename O> void operator()(const O &o) const { o(head_ray_v, h); }
};
template <typename H, typename... T> struct list<H, T...> : list<T...> {
  H h;
  template <typename U, typename... V>
  list(U &&u, V &&... v)
      : list<T...>(static_cast<V &&>(v)...), h(static_cast<U &&>(u)) {}
  template <typename O> void operator()(const O &o) const {
    o(head_ray_v, h);
    o(tail_ray_v, [&](const auto &o) { list<T...>::operator()(o); });
  }
};

#include <iostream>

struct log_list_pith {
  template <typename H> void operator()(head_ray *, H &&h) const {
    std::cout << h << '\n';
  }
  template <typename T> void operator()(tail_ray *, T &&tail) const {
    std::cout << "next" << '\n';
    tail(*this);
  }
};
int main() {
  sample_list(list_pith{});
  auto l = list{"a", 2, 3.3f, 'o'};
  l(log_list_pith{});
  return 9;
}
